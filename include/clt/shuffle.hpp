#pragma once

#include <cassert>
#include <numeric>
#include <vector>
#include <tuple>
#include <functional>

#include <gmpxx.h>

namespace clt {
inline mpz_class factorial(const uint64_t n)
{
    mpz_class fact = 1;
    for (mpz_class i = 2; mpz_cmp_ui(i.get_mpz_t(), n) <= 0; i++) {
        fact *= i;
    }
    return fact;
}

namespace rng {
template <class T, class Func>
inline void shuffle(T *inplace, const size_t n, Func &&rng)
{
    // NOTE: FY shuffle.
    using index_t = uint64_t;
    assert(n < (index_t(1) << 32));
    static_assert((2 * sizeof(T)) <= sizeof(index_t));
    constexpr auto elem_bytes = sizeof(index_t);
    const auto random_bytes = n * elem_bytes;
    std::vector<index_t> random_indices(n);
    rng(random_indices.data(), random_bytes);
    for (size_t i = 1; i < (n - 1); i++) {
        const auto j = random_indices[n - i] % (n - i);
        std::swap(inplace[j], inplace[n - i]);
    }
}

template <class T, class Func>
inline void shuffle(std::vector<T> &inplace, Func &&rng)
{
    shuffle(inplace.data(), std::size(inplace), std::forward<Func>(rng));
}
} // namespace rng

template <class T, class U>
inline void apply_permutation(T *out, const T *in, const U *perm,
                              const size_t n)
{
    assert(out != in);
    for (size_t i = 0; i < n; i++) {
        out[i] = in[perm[i]];
    }
}

template <class T, class U>
inline auto apply_permutation(const std::vector<T> &in,
                              const std::vector<U> &perm)
{
    assert(std::size(in) == std::size(perm));
    std::vector<T> out(std::size(in));
    apply_permutation(out.data(), in.data(), perm.data(), std::size(in));
    return out;
}

template <class T>
inline void inverse_permutation(T *out, const T *in, const size_t n)
{
    using tuple_t = std::tuple<T, uint32_t>;
    std::vector<tuple_t> t_vec;
    t_vec.reserve(n);
    for (size_t i = 0; i < n; i++) {
        t_vec.emplace_back(in[i], i);
    }
    std::sort(t_vec.begin(), t_vec.end(),
              [](const tuple_t &lhs, const tuple_t &rhs) -> bool {
                  return std::get<0>(lhs) < std::get<0>(rhs);
              });
    for (size_t i = 0; i < n; i++) {
        out[i] = std::get<1>(t_vec[i]);
    }
}

template <class T> inline auto inverse_permutation(const std::vector<T> &in)
{
    std::vector<T> out(std::size(in));
    inverse_permutation(out.data(), in.data(), std::size(in));
    return out;
}

using permutation_t = std::vector<uint32_t>;
mpz_class rank(const permutation_t &pi);
permutation_t unrank(const mpz_class &r, const size_t degree);

namespace internal {
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

template <class T> inline auto bit_at(const std::vector<T> &in, const size_t at)
{
    const size_t elem_bit_size = sizeof(T) * CHAR_BIT;
    const size_t position = at / elem_bit_size;
    const size_t bit_position = at % elem_bit_size;
    const auto data = in[position];
    const auto mask = T(1) << bit_position;
    return data & mask;
}
} // namespace internal

template <class T, class RngFunc>
inline void shuffle_RS(std::vector<T> &inplace, RngFunc &&rng)
{
    const size_t n = inplace.size();
    assert(n > 0);
    if (n == 1) {
        return;
    } else if (n == 2) {
        uint8_t r;
        rng(&r, sizeof(decltype(r)));
        if (r & 0x1ull) {
            return;
        } else {
            std::swap(inplace[0], inplace[1]);
        }
    } else {
        std::vector<T> lhs, rhs;
        lhs.reserve(n);
        rhs.reserve(n);
        const size_t num_bytes = internal::int_ceiling(n, CHAR_BIT);
        const size_t num_elems =
            internal::int_ceiling(n, sizeof(uint64_t) * CHAR_BIT);
        std::vector<uint64_t> rs(num_elems);
        assert((rs.size() * sizeof(uint64_t) * CHAR_BIT) >= n);
        rng(rs.data(), num_bytes);
        for (size_t i = 0; i < n; i++) {
            if (internal::bit_at(rs, i)) {
                lhs.emplace_back(inplace[i]);
            } else {
                rhs.emplace_back(inplace[i]);
            }
        }
        if (lhs.size() > 0) {
            shuffle_RS(lhs, rng);
        }
        if (rhs.size() > 0) {
            shuffle_RS(rhs, rng);
        }
        auto inplace_iter = inplace.begin();
        std::copy(lhs.begin(), lhs.end(), inplace_iter);
        std::advance(inplace_iter, lhs.size());
        std::copy(rhs.begin(), rhs.end(), inplace_iter);
    }
} // namespace clt
} // namespace clt
