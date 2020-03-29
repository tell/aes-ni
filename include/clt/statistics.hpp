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

template <class T, class U = std::enable_if_t<std::is_integral_v<T>>>
inline auto check_random_bits(const T v)
{
    /**
     * NOTE: Very rough statistical check, not believe this.
     */
    using value_t = T;
    const size_t num_bytes = sizeof(value_t);
    const size_t num_bits = num_bytes * CHAR_BIT;
    const auto stats = binomial_statistics(num_bits);
    const auto low = std::get<2>(stats);
    const auto high = std::get<3>(stats);
    const auto popcnt_vec = _mm_popcnt_u64(v);
    return (popcnt_vec < low) || (high < popcnt_vec);
}

template <class T, class U = std::enable_if_t<!std::is_integral_v<T>>>
inline auto check_random_bits(const T &vec)
{
    /**
     * NOTE: Very rough statistical check, not believe this.
     */
    using value_t = typename T::value_type;
    const size_t num_bytes = std::size(vec) * sizeof(value_t);
    const size_t num_bits = num_bytes * CHAR_BIT;
    const auto stats = binomial_statistics(num_bits);
    const auto low = std::get<2>(stats);
    const auto high = std::get<3>(stats);
    const auto popcnt_vec = popcnt(vec);
    return (popcnt_vec < low) || (high < popcnt_vec);
}

} // namespace clt
