#include <linux/if_link.h>
#include <net/if.h>
#include <print>

#include "../../common/macros.hpp"
#include "xdp_loader.hpp"

xdp_program_info attach_xdp_filter(const char* interface)
{
    xdp_program_info info;
    info.obj_path = "./filters/xdp_filter.o";

    info.ifindex = if_nametoindex(interface);
    if (info.ifindex == 0)
        error_exit("Interface not found");

    info.obj = bpf_object__open_file(info.obj_path, nullptr);
    if (libbpf_get_error(info.obj))
        error_exit("Failed to open BPF object file");

    int ret = bpf_object__load(info.obj);
    if (ret) {
        bpf_object__close(info.obj);
        error_exit("Failed to load BPF object");
    }

    info.prog = bpf_object__find_program_by_name(info.obj, "xdp_filter");
    if (!info.prog) {
        bpf_object__close(info.obj);
        error_exit("Failed to find XDP program in object file");
    }

    info.prog_fd = bpf_program__fd(info.prog);
    if (info.prog_fd < 0) {
        bpf_object__close(info.obj);
        error_exit("XDP program file descriptor could not be obtained");
    }

    ret = bpf_xdp_attach(info.ifindex, info.prog_fd, XDP_FLAGS_SKB_MODE, nullptr);
    if (ret) {
        bpf_object__close(info.obj);
        error_exit("Failed to attach XDP program to interface");
    }

    std::println("Successfully attached XDP program to interface: {}", interface);

    return info;
}

void detach_xdp_filter(xdp_program_info info)
{
    int ret = bpf_xdp_detach(info.ifindex, XDP_FLAGS_SKB_MODE, nullptr);
    if (ret)
        error_exit("Failed to detach XDP program");

    std::println("Detached XDP program from interface");

    bpf_object__close(info.obj);
}
