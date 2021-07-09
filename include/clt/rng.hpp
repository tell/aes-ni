#pragma once

#include <iostream>
#include <vector>

#if defined(__linux__) && (__GLIBC__ > 2 || __GLIBC_MINOR__ > 24)
#define IS_SYSCALL_GETRANDOM_ENABLED 1
#else
#define IS_SYSCALL_GETRANDOM_ENABLED 0
#endif

#if IS_SYSCALL_GETRANDOM_ENABLED
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

#include <fmt/format.h>

#include "util.hpp"
#include "util_integer.hpp"

namespace clt {
namespace rng {
class RandomDevice {
    std::ifstream fst_rdev_;

public:
    RandomDevice(const RandomDevice &) = delete;
    RandomDevice &operator=(const RandomDevice &) = delete;
    explicit RandomDevice(const path &dev_random);
    RandomDevice() : RandomDevice("/dev/urandom"){};
    bool operator()(void *buff, const size_t byte_size);
};

extern RandomDevice rng_global;

template <class T> inline void init(T *buff, const size_t num_elems)
{
    const size_t num_bytes = sizeof(T) * num_elems;
    const auto status = rng_global(buff, num_bytes);
    if (!status) {
        throw std::runtime_error("Randomness generation failed.");
    }
}

template <class T> inline void init(T &buff)
{
    init(buff.data(), std::size(buff));
}

inline void getrandom([[maybe_unused]] void *out,
                      [[maybe_unused]] const size_t num_bytes)
{
#if IS_SYSCALL_GETRANDOM_ENABLED
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
    throw std::runtime_error("Not implemented yet.");
#endif
}

template <class IntType, class Func>
inline auto rejection_sample_modulo_n(const IntType mod, Func &&rng)
{
    static_assert(std::is_integral_v<IntType>);
    static_assert(std::is_unsigned_v<IntType>);
    const auto byte_size = sizeof(IntType);
    assert(mod > 1);
    const auto mask = least2pow(mod) - 1;
    assert(mod <= (mask + 1));
    IntType out;
    rng(&out, byte_size);
    out &= mask;
    while (out >= mod) {
        rng(&out, byte_size);
        out &= mask;
    }
    return out;
}
} // namespace rng
} // namespace clt

#include "detail/rng_inline.hpp"
