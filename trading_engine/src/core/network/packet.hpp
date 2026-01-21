#pragma once

#include <atomic>
#include <span>
#include <cstdint>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <sys/socket.h>

#include "message.hpp"
#include "../common/intrinsics.hpp"
#include "../common/macros.hpp"
#include "../common/bytes.hpp"
#include "../orderbook/symbol_directory.hpp"

namespace network
{
    struct ipv4_min
    {
        std::uint8_t ver_ihl;
        std::uint8_t tos;
        std::uint16_t tot_len;
        std::uint16_t id;
        std::uint16_t frag_off;
        std::uint8_t ttl;
        std::uint8_t proto;
        std::uint16_t csum;
        std::uint32_t src_addr;
        std::uint32_t dst_addr;
    } PACKED;

    static_assert(sizeof(ipv4_min) == 20);

    struct Ring
    {
        std::vector<std::span<std::byte>> blocks;
        std::byte* buffer{ nullptr };
        tpacket_req3 req{ };
    };

    inline constexpr uint32_t BLOCK_SIZE{ 2 * 1024 * 1024 };
    inline constexpr uint32_t BLOCK_COUNT{ 8 };
    inline constexpr uint32_t FRAME_SIZE{ 2048 };

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
            error_exit("setsockopt(), PACKET_RX_RING");

        void* map = mmap(nullptr, ring.req.tp_block_size * ring.req.tp_block_nr,
                    PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED, fd, 0);

        if (map == MAP_FAILED)
            error_exit("mmap()");

        ring.buffer = reinterpret_cast<std::byte *>(map);
        
        for (uint32_t i = 0; i < ring.req.tp_block_nr; i++) {
            auto block = std::span<std::byte>{ring.buffer + (i * ring.req.tp_block_size), ring.req.tp_block_size};
            ring.blocks.push_back(block);
        }
    }

    inline bool is_block_readable(std::span<std::byte> block) noexcept
    {
        auto* bdesc = reinterpret_cast<tpacket_block_desc *>(block.data());

        std::atomic_thread_fence(std::memory_order_acquire); 
        return bdesc->hdr.bh1.block_status & TP_STATUS_USER;
    }

    inline void flush_block(std::span<std::byte> block) noexcept
    {
        auto* bdesc = reinterpret_cast<tpacket_block_desc *>(block.data());

        std::atomic_thread_fence(std::memory_order_release);
        bdesc->hdr.bh1.block_status = TP_STATUS_KERNEL;
    }

    inline void process_mold(std::span<const std::byte> mold) noexcept
    {
        const std::byte* base = mold.data();       
        const std::byte* end = mold.data() + mold.size();

        if (!ptr_in_range(base + sizeof(moldhdr), base, end)) [[unlikely]] return;

        uint8_t session_id[10];
        std::memcpy(session_id, base, 10);
        uint64_t seq_num = loadu_bswap<uint64_t>(base + offsetof(moldhdr, seq_num));
        uint16_t msg_count = loadu_bswap<uint16_t>(base + offsetof(moldhdr, msg_count));

        std::size_t offset = sizeof(moldhdr);

        // even worth it to prefetch the first packet?
        if (msg_count)
            prefetch(base + offset);
        
        for (uint16_t i = 0; i < msg_count; i++) {
            const std::byte* mold_msg = base + offset;
            
            if (!ptr_in_range(mold_msg + sizeof(moldmsg), base, end)) [[unlikely]] return;

            const moldmsg* msg = reinterpret_cast<const moldmsg *>(mold_msg);

            uint16_t msg_len = loadu_bswap<uint16_t>(mold_msg);

            if (msg_len == 0x0000) [[unlikely]] { /* end of session */ }

            std::size_t offset = sizeof(msg_len) + msg_len;

            if (!ptr_in_range(mold_msg + offset, base, end)) [[unlikely]] return;

            if (i + 1 < msg_count) [[likely]] {
                const std::byte* next_mold_msg = base + offset;
                
                if (ptr_in_range(next_mold_msg, base, end)) [[likely]] {
                    prefetch(next_mold_msg);
                }
            }

            Message decoded_msg = deserialize(msg);   

            auto idx = SymbolDirectory::instance().index(decoded_msg.locate);
            if (idx < 0)
                return;

            // queue[idx].try_push(msg) 
        }
    }
    
    inline void process_frame(std::span<const std::byte> frame) noexcept
    {
        ethhdr eth;
        if (!loadu_at(frame, 0, eth)) [[unlikely]] return;
        if (std::byteswap(eth.h_proto) != 0x800) [[unlikely]] return;

        std::size_t offset = sizeof(ethhdr);

        ipv4_min ip;
        if (!loadu_at(frame, offset, ip)) [[unlikely]] return;
        
        const uint8_t ver = ip.ver_ihl >> 4;
        const uint8_t ihl = ip.ver_ihl & 0x0F;

        if (ver != 4 || ihl != 5) [[unlikely]] return;
        
        const uint16_t frag = std::byteswap(ip.frag_off);
        
        // 0x1FFF -> lower 13 bits (frag offset)
        // 0x2000 -> MF (more frags bit)
        if ((frag & 0x1FFFu) != 0 || (frag & 0x2000u) != 0) [[unlikely]] return;

        if (ip.proto != 17) [[unlikely]] return;

        offset += sizeof(ipv4_min);
        
        udphdr udp;
        if (!loadu_at(frame, offset, udp)) [[unlikely]] return;

        const uint16_t udp_len = std::byteswap(udp.len);
        if (udp_len < sizeof(udphdr)) [[unlikely]] return;
        
        if (offset + udp_len > frame.size()) [[unlikely]] return;
        
        offset += sizeof(udphdr);

        process_mold(frame.subspan(offset));
    }

    inline void process_block(std::span<std::byte> block) noexcept
    {
        if (block.size() < sizeof(tpacket_block_desc)) [[unlikely]] return;

        auto* bdesc = reinterpret_cast<tpacket_block_desc *>(block.data());
        auto* bhdr = &bdesc->hdr.bh1;

        const std::byte* base = block.data();
        const std::byte* end = block.data() + block.size();

        uint32_t num_pkts = bhdr->num_pkts;
        uint32_t offset = bhdr->offset_to_first_pkt;

        if (offset >= block.size()) [[unlikely]] {
            flush_block(block);
            return;
        }

        if (num_pkts) 
            prefetch(base + offset);
    
        for (uint32_t i = 0; i < num_pkts; i++) {
            const std::byte* tpacket = base + offset;

            if (!ptr_in_range(tpacket + sizeof(tpacket3_hdr), base, end)) [[unlikely]] break;

            auto* tph = reinterpret_cast<const tpacket3_hdr *>(tpacket);

            const std::uint32_t next_pkt = tph->tp_next_offset;
            if (next_pkt == 0 || offset + next_pkt > block.size()) [[unlikely]] break;

            const std::uint32_t snaplen = tph->tp_snaplen;
            const std::uint16_t mac_offset  = tph->tp_mac;
            
            const std::byte* eth = tpacket + mac_offset;
            
            if (!ptr_in_range(eth + snaplen, base, end)) [[unlikely]] {
                offset += next_pkt;
                continue; // drop packet, keep walking
            }

            if (i + 1 < num_pkts) [[likely]] {
                const std::byte* next_tpacket = base + (offset + next_pkt);
                prefetch(next_tpacket);

                if (ptr_in_range(next_tpacket + sizeof(tpacket3_hdr), base, end)) [[likely]] {
                    auto* next_tph = reinterpret_cast<const tpacket3_hdr *>(next_tpacket);
                    prefetch(base + next_tph->tp_mac);
                }
            }
            
            process_frame(std::span{eth, snaplen});

            offset += next_pkt;
        }
    }

} // namespace Network
