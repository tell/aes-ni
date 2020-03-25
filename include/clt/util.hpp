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
} // namespace clt
