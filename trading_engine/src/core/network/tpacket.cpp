#include <linux/if_ether.h>
#include <atomic>

#include "tpacket.hpp"
#include "packet.hpp"
#include "../common/intrinsics.hpp"

namespace network
{
    void TPacketProcessor::process_bytes()
    {
        static auto block_idx = 0uz;

        for (auto block_iter = 0uz; block_iter < ring_.blocks_.size(); block_iter++) {
            if (!is_block_readable(ring_.blocks_[block_idx])) {
                block_idx = (block_idx + 1) & (ring_.blocks_.size() - 1);
                continue;
            }

            process_block(ring_.blocks_[block_idx]);
            release_block(ring_.blocks_[block_idx]);
            block_idx = (block_idx + 1) & (ring_.blocks_.size() - 1);

            block_iter = 0;
        }
    }

    void TPacketProcessor::process_frame(std::span<const std::byte> frame) noexcept
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

        // process_mold(frame.subspan(offset));
    }

    void TPacketProcessor::process_block(std::span<std::byte> block) noexcept
    {
        if (block.size() < sizeof(tpacket_block_desc)) [[unlikely]] return;

        auto* bdesc = reinterpret_cast<tpacket_block_desc *>(block.data());
        auto* bhdr = &bdesc->hdr.bh1;

        const std::byte* base = block.data();
        const std::byte* end = block.data() + block.size();

        uint32_t num_pkts = bhdr->num_pkts;
        uint32_t offset = bhdr->offset_to_first_pkt;

        if (offset >= block.size()) [[unlikely]] {
            release_block(block);
            return;
        }

        if (num_pkts) 
            prefetch(base + offset);
    
        for (uint32_t i = 0; i < num_pkts; i++) {
            const std::byte* tpacket = base + offset;

            if (!ptr_in_range(tpacket + sizeof(tpacket3_hdr), base, end)) [[unlikely]] break;

            auto* tph = reinterpret_cast<const tpacket3_hdr *>(tpacket);

            const uint32_t next_pkt = tph->tp_next_offset;
            if (next_pkt == 0 || offset + next_pkt > block.size()) [[unlikely]] break;

            const uint32_t snaplen = tph->tp_snaplen;
            const uint16_t mac_offset  = tph->tp_mac;
            
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

            // LOG(...) -> tpacket time stamp
        }
    }

    bool TPacketProcessor::is_block_readable(std::span<std::byte> block) noexcept
    {
        auto* bdesc = reinterpret_cast<tpacket_block_desc *>(block.data());

        std::atomic_thread_fence(std::memory_order_acquire); 
        return bdesc->hdr.bh1.block_status & TP_STATUS_USER;
    }

    void TPacketProcessor::release_block(std::span<std::byte> block) noexcept
    {
        auto* bdesc = reinterpret_cast<tpacket_block_desc *>(block.data());

        std::atomic_thread_fence(std::memory_order_release);
        bdesc->hdr.bh1.block_status = TP_STATUS_KERNEL;
    }

} // namespace network