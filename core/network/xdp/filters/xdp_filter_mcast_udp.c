#include <linux/bpf.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/if_ether.h>
#include <bpf/bpf_helpers.h>

#define MCAST_IP_ADDR 0xEF000001 // 239.0.0.1
#define UDP_PORT 33333

SEC("xdp_filter_mcast_udp")
int xdp_filter(struct xdp_md* ctx) {
    void* data = (void*)(long)ctx->data;
    void* data_end = (void*)(long)ctx->data_end;
    struct ethhdr* eth = data;
    struct iphdr* ip; 
    struct udphdr* udp;
    
    if (data + sizeof(*eth) + sizeof(*ip) + sizeof(*udp) > data_end)
        return XDP_DROP;

    if (eth->h_proto != __constant_htons(ETH_P_IP))
        return XDP_DROP;

    ip = data + sizeof(*eth);
    if (ip->protocol != IPPROTO_UDP || ip->daddr != __constant_htonl(MCAST_IP_ADDR))
        return XDP_DROP;

    udp = (void*)ip + (ip->ihl << 2);
    if (udp->dest != __constant_htons(UDP_PORT))
        return XDP_DROP;

    return XDP_PASS;
}

char _license[] SEC("license") = "GPL";
