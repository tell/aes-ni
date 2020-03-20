#pragma once

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
        : fst_rdev_(dev_random, std::ios_base::in | std::ios_base::binary) {}

    bool operator()(void *buff, const size_t byte_size);
};
} // namespace rng
} // namespace clt
