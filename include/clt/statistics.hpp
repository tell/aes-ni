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
    constexpr double conf_scale = 4.0;
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

namespace internal {

template <class T, class U = std::enable_if_t<std::is_integral_v<T>>>
inline auto bytes_popcnt(const T v)
{
    using value_t = T;
    const size_t num_bytes = sizeof(value_t);
    const auto popcnt_v = _mm_popcnt_u64(v);
    return std::make_tuple(num_bytes, popcnt_v);
}

template <class T, class U = std::enable_if_t<!std::is_integral_v<T>>>
inline auto bytes_popcnt(const T &vec)
{
    using value_t = typename T::value_type;
    const size_t num_bytes = std::size(vec) * sizeof(value_t);
    const auto popcnt_vec = popcnt(vec);
    return std::make_tuple(num_bytes, popcnt_vec);
}
} // namespace internal

template <class T> inline auto check_random_bytes(T &&v)
{
    /**
     * NOTE: Very rough statistical check, not believe this.
     */
    const auto [num_bytes, popcnt_v] = internal::bytes_popcnt(v);
    const auto num_bits = num_bytes * CHAR_BIT;
    const auto [mean, stdv, low, high] = binomial_statistics(num_bits);
    return (low <= popcnt_v) && (popcnt_v <= high);
}

} // namespace clt
