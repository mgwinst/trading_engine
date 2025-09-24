#pragma once

#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <cstdint>
#include <unistd.h>
#include <atomic>

#include "socket_utils.hpp"
#include "common/macros.hpp"

#define BLOCK_SIZE (2 * 1024 * 1024)
#define BLOCK_COUNT 8
#define FRAME_SIZE 2048

struct RingDesc
{
    uint8_t* block_ptr{ nullptr };
    std::size_t len{ 0 };
};

struct Ring
{
    RingDesc* rd{ nullptr };
    uint8_t* map{ nullptr };
    tpacket_req3 req{};
};

inline void configure_ring(int32_t fd, Ring& ring)
{
    int32_t version = TPACKET_V3;

    if (setsockopt(fd, SOL_PACKET, PACKET_VERSION, &version, sizeof(version)))
        error_exit("setsockopt() PACKET_VERSION");

    ring.req.tp_block_size = BLOCK_SIZE;
    ring.req.tp_block_nr = BLOCK_COUNT;
    ring.req.tp_frame_size = FRAME_SIZE;
    ring.req.tp_frame_nr = (BLOCK_SIZE * BLOCK_COUNT) / FRAME_SIZE;
    ring.req.tp_retire_blk_tov = 60;    // 60 ms timeout
    ring.req.tp_feature_req_word = TP_FT_REQ_FILL_RXHASH;   // request RX hash

    if (setsockopt(fd, SOL_PACKET, PACKET_RX_RING, &ring.req, sizeof(ring.req)))
        error_exit("setsockopt(), PACKET_RX_RING")

    ring.map = reinterpret_cast<uint8_t*>(mmap(nullptr, ring.req.tp_block_size * ring.req.tp_block_nr,
                  PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED, fd, 0));

    if (ring.map == MAP_FAILED)
        error_exit("mmap()");

    ring.rd = new RingDesc[ring.req.tp_block_nr];
    if (!ring.rd)
        error_exit("malloc()");

    for (int i = 0; i < ring.req.tp_block_nr; i++) {
        ring.rd[i].block_ptr = ring.map + (i * ring.req.tp_block_size);
        ring.rd[i].len = ring.req.tp_block_size;
    }
}

inline void flush_block(tpacket_block_desc* block_desc)
{
    std::atomic_thread_fence(std::memory_order_release);
    block_desc->hdr.bh1.block_status = TP_STATUS_KERNEL;
}

inline bool is_block_readable(tpacket_block_desc* block_desc)
{
    std::atomic_thread_fence(std::memory_order_acquire); 
    return block_desc->hdr.bh1.block_status & TP_STATUS_USER;
}

inline void process_block(tpacket_block_desc* block_desc, auto& buffer)
{
    int32_t num_pkts = block_desc->hdr.bh1.num_pkts;
    tpacket3_hdr* tpkt_hdr = reinterpret_cast<tpacket3_hdr *>((reinterpret_cast<uint8_t *>(block_desc) + block_desc->hdr.bh1.offset_to_first_pkt));

    for (std::size_t i = 0; i < num_pkts; i++)
    {
        ethhdr* eth_hdr = reinterpret_cast<ethhdr *>(reinterpret_cast<uint8_t *>(tpkt_hdr) + tpkt_hdr->tp_mac);
        iphdr* ip_hdr = reinterpret_cast<iphdr *>(reinterpret_cast<uint8_t *>(eth_hdr) + sizeof(eth_hdr));
        udphdr* udp_hdr = reinterpret_cast<udphdr *>(reinterpret_cast<uint8_t *>(ip_hdr) + (ip_hdr->ihl * 4));
        uint8_t* payload = reinterpret_cast<uint8_t *>(udp_hdr) + sizeof(udp_hdr);

        std::size_t payload_len = ntohs(udp_hdr->len) - sizeof(udp_hdr);

        std::size_t bytes_written{0};
        while (payload_len > 0)
        {
            bytes_written += buffer.write(payload + bytes_written, payload_len);
            payload_len -= bytes_written;
        }

        tpkt_hdr = reinterpret_cast<tpacket3_hdr *>(reinterpret_cast<uint8_t *>(tpkt_hdr) + tpkt_hdr->tp_next_offset);
    }
}