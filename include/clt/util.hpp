#pragma once

#include <cstdint>
#include <sstream>
#include <array>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <clt/rng.hpp>

namespace clt {
inline std::string join(const uint64_t *in, const size_t &n)
{
    std::stringstream sst;
    if (n == 0) {
        return "";
    }
    sst << fmt::format("{:>016x}", uint_fast64_t(in[0]));
    if (n > 1) {
        for (size_t i = 1; i < n; i++) {
            sst << fmt::format(":{:>016x}", uint_fast64_t(in[i]));
        }
    }
    return sst.str();
}

inline std::string join(const uint32_t *in, const size_t &n)
{
    std::stringstream sst;
    if (n == 0) {
        return "";
    }
    sst << fmt::format("{:>08x}", uint_fast64_t(in[0]));
    if (n > 1) {
        for (size_t i = 1; i < n; i++) {
            sst << fmt::format(":{:>08x}", uint_fast64_t(in[i]));
        }
    }
    return sst.str();
}

inline std::string join(const uint16_t *in, const size_t &n)
{
    std::stringstream sst;
    if (n == 0) {
        return "";
    }
    sst << fmt::format("{:>04x}", uint_fast64_t(in[0]));
    if (n > 1) {
        for (size_t i = 1; i < n; i++) {
            sst << fmt::format(":{:>04x}", uint_fast64_t(in[i]));
        }
    }
    return sst.str();
}

inline std::string join(const uint8_t *in, const size_t &n)
{
    std::stringstream sst;
    if (n == 0) {
        return "";
    }
    sst << fmt::format("{:>02x}", (uint_fast64_t(in[0]) & 0xffull));
    if (n > 1) {
        for (size_t i = 1; i < n; i++) {
            sst << fmt::format(":{:>02x}", (uint_fast64_t(in[i]) & 0xffull));
        }
    }
    return sst.str();
}

template <class T, size_t N> inline auto join(const T (&in)[N])
{
    return join(in, N);
}

template <class T, size_t N> inline auto join(const std::array<T, N> &in)
{
    return join(in.data(), N);
}

template <class T> inline auto join(const std::vector<T> &in)
{
    return join(in.data(), in.size());
}

template <class T>
void init_iota(T &&out, const size_t n, const size_t elem_per_block = 2)
{
    assert(0 < elem_per_block);
    assert((n * elem_per_block) <= size(out));
    for (size_t i = 0; i < n; i++) {
        out[i * elem_per_block] = i;
        for (size_t j = 1; j < elem_per_block; j++) {
            out[i * elem_per_block + j] = 0;
        }
    }
}

template <class T> inline void init(T *buff, const size_t num_elems)
{
    const size_t num_bytes = sizeof(T) * num_elems;
    const auto status = clt::rng::rng_global(buff, num_bytes);
    if (!status) {
        fmt::print(std::cerr, "ERROR!! failed: {}\n", __func__);
        abort();
    }
}

template <class T> inline void init(std::vector<T> &buff)
{
    init(buff.data(), buff.size());
}

template <class T, size_t N> inline void init(std::array<T, N> &buff)
{
    init(buff.data(), N);
}

inline auto gen_key()
{
    AES128::key_t key;
    if (!clt::rng::rng_global(key.data(), aes128::key_bytes)) {
        fmt::print(std::cerr, "ERROR!! failed: {}\n", __func__);
    }
    return key;
}
} // namespace clt
