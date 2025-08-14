#include <linux/bpf.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/if_ether.h>
#include <bpf/bpf_helpers.h>

#define MCAST_IP_ADDR 0xE0AABBCC // 224.170.187.204
#define UDP_PORT 33333

SEC("xdp_filter_mcast_udp")
int xdp_filter(struct xdp_md* ctx) {

}

char _license[] SEC("license") = "GPL";
