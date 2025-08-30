#pragma once

#include <linux/if_xdp.h>
#include <cstdint>

namespace network 
{
    struct xsk_info 
    {
        std::size_t chunk_size_;
        std::size_t chunk_count_;
        std::size_t umem_len_;
        std::size_t rx_ring_len_;
        std::size_t fill_ring_len_;
        std::size_t queue_id_;
        xdp_mmap_offsets offsets_;
    };

    struct xsk_rings 
    {
        void* rx_ring_mmap_{ nullptr };
        void* fill_ring_mmap_{ nullptr };

        uint32_t* rx_ring_consumer_{ nullptr };
        uint32_t* rx_ring_producer_{ nullptr };
        uint32_t* fill_ring_consumer_{ nullptr };
        uint32_t* fill_ring_producer_{ nullptr };

        xdp_desc* rx_ring_{ nullptr };
        xdp_desc* fill_ring_{ nullptr };
    };
    
} // end of namespace


    