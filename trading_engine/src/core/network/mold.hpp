#pragma once

#include <cstdint>
#include <cstring>
#include <span>

#include "message.hpp"
#include "../orderbook/symbol_directory.hpp"
#include "../common/queues/SPSCQueuePool.hpp"
#include "../common/bytes.hpp"
#include "../common/intrinsics.hpp"

inline void on_message(const moldmsg* msg) noexcept
{
    auto& symbol_dir = SymbolDirectory::instance();

    Message decoded_msg = deserialize(msg);
    // if decoded_msg is empty return (not one of the msg types we were anticipating)

    if (static_cast<char>(decoded_msg.msg_type) == 'R') [[unlikely]] {
        symbol_dir.update(decoded_msg);
        return;
    }

    auto idx = symbol_dir.index(decoded_msg.locate);
    if (idx < 0)
        return;

    uint64_t symbol = symbol_dir[idx];

    auto& queue = SPSCQueuePool<Message>::instance();

    queue[symbol]->try_push(decoded_msg);
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

        if (msg_len == 0x0000) [[unlikely]] { }

        std::size_t offset = sizeof(msg_len) + msg_len;

        if (!ptr_in_range(mold_msg + offset, base, end)) [[unlikely]] return;

        if (i + 1 < msg_count) [[likely]] {
            const std::byte* next_mold_msg = base + offset;
            
            if (ptr_in_range(next_mold_msg, base, end)) [[likely]] {
                prefetch(next_mold_msg);
            }
        }

        on_message(msg);
    }
}