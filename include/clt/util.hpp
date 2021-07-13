#pragma once

#include <cassert>
#include <cstdint>
#include <sstream>
#include <array>
#include <type_traits>
#include <vector>

#include <fmt/format.h>
#include <fmt/ostream.h>

namespace clt {

template <class ElemType> struct default_format_str {
    static const std::string value;
};
template <class ElemType>
const std::string default_format_str<ElemType>::value = "{}";

template <> struct default_format_str<int64_t> {
    static const std::string value;
};
template <> struct default_format_str<int32_t> {
    static const std::string value;
};
template <> struct default_format_str<int16_t> {
    static const std::string value;
};
template <> struct default_format_str<int8_t> {
    static const std::string value;
};
template <> struct default_format_str<uint64_t> {
    static const std::string value;
};
template <> struct default_format_str<uint32_t> {
    static const std::string value;
};
template <> struct default_format_str<uint16_t> {
    static const std::string value;
};
template <> struct default_format_str<uint8_t> {
    static const std::string value;
};

template <class IntType>
inline std::string
join(const IntType *in, const size_t &n, const std::string separator = ",",
     const std::string format_str = default_format_str<IntType>::value)
{
    if (n == 0) {
        return "";
    }
    std::stringstream sst;
    sst << fmt::format(fmt::runtime(format_str), in[0]);
    for (size_t i = 1; i < n; i++) {
        sst << separator << fmt::format(fmt::runtime(format_str), in[i]);
    }
    return sst.str();
}

template <class T, size_t N, class U = typename T::value_type>
inline auto join(const T (&in)[N], const std::string &separator = ",",
                 const std::string &format = default_format_str<U>::value)
{
    return join(in, N, separator, format);
}

template <class T, size_t N>
inline auto join(const std::array<T, N> &in, const std::string &separator = ",",
                 const std::string &format = default_format_str<T>::value)
{
    return join(in.data(), N, separator, format);
}

template <class T>
inline auto join(const std::vector<T> &in, const std::string &separator = ",",
                 const std::string &format = default_format_str<T>::value)
{
    return join(in.data(), in.size(), separator, format);
}

template <class T>
void init_iota(T &out, const size_t n, const size_t elem_per_block = 2)
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

template <class T> auto all_xor(T &vec)
{
    using value_t = typename T::value_type;
    value_t v = 0;
    for (auto &&x : vec) {
        v ^= x;
    }
    return v;
}

inline auto int_ceiling(const size_t n, const size_t d)
{
    const auto q = n / d;
    const auto r = n % d;
    if (r == 0) {
        return q;
    } else {
        return q + 1;
    }
}
} // namespace clt
