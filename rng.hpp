#pragma once

#if defined(__linux__)
#include <string.h>
#include <sys/random.h>
#endif

#include <fstream>
#if __cpp_lib_filesystem && !defined(__APPLE__)
#include <filesystem>
namespace clt {
using path = std::filesystem::path;
}
#else
namespace clt {
using path = std::string;
}
#endif

namespace clt {
namespace rng {
class RNG {
    std::ifstream fst_rdev_;

public:
    RNG(const path &dev_random = "/dev/urandom")
        : fst_rdev_(dev_random, std::ios_base::in | std::ios_base::binary)
    {
    }

    bool operator()(void *buff, const size_t byte_size);
};

inline void getrandom(void *out, const size_t num_bytes)
{
#if defined(__linux__)
    auto *p_out = reinterpret_cast<uint8_t *>(out);
    size_t gened_bytes = 0;
    errno = 0;
    do {
        const auto ret_bytes =
            ::getrandom(p_out, num_bytes - gened_bytes, GRND_NONBLOCK);
        if (ret_bytes < 0) {
            const auto str = std::string(strerror(errno));
            throw std::runtime_error(str);
        }
        p_out += ret_bytes;
        gened_bytes += ret_bytes;
    } while (gened_bytes < num_bytes);
#else
    throw std::runtime_error("not implemented yet");
#endif
}
} // namespace rng
} // namespace clt
