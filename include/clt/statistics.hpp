#pragma once

#include <cmath>
#include <vector>
#include <tuple>

#include <x86intrin.h>

namespace clt {

inline auto binomial_statistics(const size_t n)
{
    const double exp = n / 2.0;
    const double stdv = std::sqrt(n) / 2.0;
    constexpr double conf_scale = 3.0;
    return std::make_tuple(exp, stdv, exp - conf_scale * stdv,
                           exp + conf_scale * stdv);
}

template <class T> inline auto popcnt(const T *vec, const size_t num_elems)
{
    size_t result = 0;
    for (size_t i = 0; i < num_elems; i++) {
        result += _mm_popcnt_u64(vec[i]);
    }
    return result;
}

template <class T> inline auto popcnt(const T &vec)
{
    size_t result = 0;
    for (auto &&x : vec) {
        result += _mm_popcnt_u64(x);
    }
    return result;
}

} // namespace clt
