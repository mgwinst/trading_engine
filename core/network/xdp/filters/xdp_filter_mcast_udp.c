#include <linux/bpf.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/if_ether.h>
#include <bpf/bpf_helpers.h>

#define MCAST_IP_ADDR 0xEF000001 // 239.0.0.1
#define UDP_PORT 33333

struct {
    __uint(type, BPF_MAP_TYPE_XSKMAP);
    __uint(max_entries, 256);
    __type(key, __u32);
    __type(value, __u32);
} xsk SEC(".maps");

SEC("xdp_filter_mcast_udp")
int rx_filter(struct xdp_md* ctx) 
{
    void* data = (void*)(long)ctx->data;
    void* data_end = (void*)(long)ctx->data_end;

    struct iphdr* ip = NULL;
    struct udphdr* udp = NULL;
    struct ethhdr* eth = NULL;

    eth = data;
    
    if (eth + 1 < data_end) {
        if (eth->h_proto != bpf_htons(ETH_P_IP))
            return XDP_DROP;
            
        ip = (void*)(eth + 1);          

        if (ip + 1 < data_end) {
            if (ip->protocol != IPPROTO_UDP || ip->daddr != bpf_htonl(MCAST_IP_ADDR))
                return XDP_DROP;
        }

        udp = (void*)(ip + 1);

        if (!udp || udp->dest != bpf_htons(UDP_PORT))
            return XDP_DROP;
        
        // return bpf_redirect_map(&xsk, ctx->rx_queue_index, XDP_PASS);
        return XDP_PASS;
    }
}

char _license[] SEC("license") = "GPL";