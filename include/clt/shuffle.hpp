#pragma once

#include <vector>

namespace clt {
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
              [](const tuple_t &lhs, const tuple_t &rhs) {
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

namespace internal {
template <class T>
inline size_t rank_impl(const size_t n, std::vector<T> &pi,
                        std::vector<T> &inv_pi)
{
    if (n == 1) {
        return 0;
    }
    const auto s = pi[n - 1];
    std::swap(pi[n - 1], pi[inv_pi[n - 1]]);
    std::swap(inv_pi[s], inv_pi[n - 1]);
    return s + n * rank_impl(n - 1, pi, inv_pi);
}
} // namespace internal

template <class T> inline auto rank(const std::vector<T> &pi)
{
    const auto degree = pi.size();
    assert(pi.size() < 13);
    auto pi_ = pi;
    auto inv_pi = inverse_permutation(pi);
    return internal::rank_impl(degree, pi_, inv_pi);
}

namespace internal {
inline void unrank_impl(size_t n, size_t r, std::vector<uint32_t> &pi)
{
    while (n > 0) {
        std::swap(pi[n - 1], pi[r % n]);
        r /= n;
        n--;
    }
}
} // namespace internal

inline auto unrank(const size_t r, const size_t degree)
{
    std::vector<uint32_t> pi(degree);
    std::iota(pi.begin(), pi.end(), 0);
    internal::unrank_impl(degree, r, pi);
    return pi;
}
} // namespace rng
} // namespace clt
