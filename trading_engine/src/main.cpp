#include <cassert>
#include <fcntl.h>
#include <filesystem>
#include <sys/mman.h>
#include <unistd.h>

#include "core/common/bytes.hpp"
#include "core/network/message.hpp"

struct mmap_file
{
    int32_t fd_;
    uint64_t len_bytes_;
    void* data_;

    mmap_file(std::string_view file_path, uint64_t len_bytes)
    {
        int fd = open(file_path.data(), O_CREAT | O_RDWR);
        if (fd < 0) {
            perror("open");
            return;
        }

        if (ftruncate(fd, len_bytes) != 0) {
            perror("ftruncate");
            close(fd);
            return;
        }

        void* map = mmap(nullptr, len_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        
        if (map == MAP_FAILED) {
            perror("mmap");
            return;
        }

        len_bytes_ = len_bytes;
        data_ = map;
        fd_ = fd;
    }

    ~mmap_file()
    {
        munmap(data_, len_bytes_);

        if (fd_ > 0)
            close(fd_);
    }
};

namespace fs = std::filesystem;

int main()
{
    std::string_view path{ "../../extra/data/itch/01_30_2019_NASDAQ_MOLDUDP64.dat" };

    assert(fs::exists(path));

    auto len_bytes = fs::file_size(path);

    mmap_file output_itch{path, len_bytes};

    std::span<const std::byte> network_bytes{(std::byte*)output_itch.data_, output_itch.len_bytes_};

    const std::byte* p = network_bytes.data();
    
    uint64_t offset = 0;

    auto cur_packet = 0;

    while (p < network_bytes.data() + network_bytes.size()) {

        auto msg_count = loadu_bswap<uint16_t>(p + offsetof(moldhdr, msg_count));

        std::println("Packet {}: Message Count: {}", cur_packet++, msg_count);

        p += sizeof(moldhdr);

        for (uint16_t i = 0; i < msg_count; i++) {
            const std::byte* msg = p;

            const moldmsg* mold_msg = (moldmsg*)p;
            
            uint16_t msg_len = loadu_bswap<uint16_t>(p);

            // on_message_test(mold_msg);

            p += msg_len + sizeof(msg_len);
        }
    }
}
