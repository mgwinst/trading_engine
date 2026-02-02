#pragma once

#include <linux/if_packet.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <vector>

#include "../common/macros.hpp"

namespace network
{
    inline constexpr uint32_t BLOCK_SIZE{ 2 * 1024 * 1024 };
    inline constexpr uint32_t BLOCK_COUNT{ 8 };
    inline constexpr uint32_t FRAME_SIZE{ 2048 };

    struct TPacketRing
    {
        explicit TPacketRing(int32_t fd) noexcept
        {
            int32_t version = TPACKET_V3;

            if (setsockopt(fd, SOL_PACKET, PACKET_VERSION, &version, sizeof(version)))
                error_exit("setsockopt() PACKET_VERSION");

            req_.tp_block_size = BLOCK_SIZE;
            req_.tp_block_nr = BLOCK_COUNT;
            req_.tp_frame_size = FRAME_SIZE;
            req_.tp_frame_nr = (BLOCK_SIZE * BLOCK_COUNT) / FRAME_SIZE;
            req_.tp_retire_blk_tov = 60;    // 60 ms timeout
            req_.tp_feature_req_word = TP_FT_REQ_FILL_RXHASH;   // request RX hash

            if (setsockopt(fd, SOL_PACKET, PACKET_RX_RING, &req_, sizeof(req_)))
                error_exit("setsockopt(), PACKET_RX_RING");

            void* map = mmap(nullptr, req_.tp_block_size * req_.tp_block_nr,
                        PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED, fd, 0);

            if (map == MAP_FAILED)
                error_exit("mmap()");

            buffer_ = reinterpret_cast<std::byte *>(map);
            
            for (uint32_t i = 0; i < req_.tp_block_nr; i++) {
                auto block = std::span<std::byte>{buffer_ + (i * req_.tp_block_size), req_.tp_block_size};
                blocks_.push_back(block);
            }
        }

        ~TPacketRing() noexcept
        {
            if (munmap(buffer_, req_.tp_block_size * req_.tp_block_nr))
                error_exit("munmap()");
        }

        std::vector<std::span<std::byte>> blocks_;
        std::byte* buffer_{ nullptr };
        tpacket_req3 req_{ };
    };

    class TPacketProcessor
    {
    public:
        explicit TPacketProcessor(int32_t fd) : ring_{ fd }
        {

        }


    private:
        TPacketRing ring_;
        
        void process_block(std::span<std::byte> block) noexcept;
        void process_frame(std::span<const std::byte> frame) noexcept;
        bool is_block_readable(std::span<std::byte> block) noexcept;
        void release_block(std::span<std::byte> block) noexcept;
    };

}





