#pragma once

#include <cstdint>
#include <fstream>

namespace clt {

namespace rng {
class RNG {
    std::ifstream fst_rdev_;

public:
    RNG()
        : fst_rdev_("/dev/urandom", std::ios_base::in | std::ios_base::binary) {
    }

    bool getrandom(void *buff, const size_t byte_size);
};
} // namespace rng
} // namespace clt