#pragma once

#include <bpf/libbpf.h>

struct xdp_program_info
{
    const char* obj_path = nullptr;
    bpf_object* obj = nullptr;
    bpf_program* prog = nullptr;
    int32_t prog_fd = -1;
    int32_t ifindex = -1;
};

xdp_program_info attach_xdp_filter(const char* interface);
void detach_xdp_filter(xdp_program_info info);
