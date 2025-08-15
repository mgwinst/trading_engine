#!/bin/bash

clang -O2 -target bpf -c xdp_filter_mcast_udp.c -o xdp_filter_mcast_udp.o

# bpftool prog load xdp_filter_mcast_udp.o /sys/fs/bpf/xdp_filter_mcast_udp

# bpftool net attach xdp name xdp_filter_mcast_udp dev eno1.131



