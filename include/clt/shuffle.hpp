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
    shuffle(inplace.data(), size(inplace), std::forward<Func>(rng));
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
    assert(size(in) == size(perm));
    std::vector<T> out(size(in));
    apply_permutation(out.data(), in.data(), perm.data(), size(in));
    return out;
}

template <class T>
inline void inverse_permutation(T *out, const T *in, const size_t n)
{
    using tuple_t = std::tuple<T, uint32_t>;
    std::vector<tuple_t> t_vec(n);
    for (size_t i = 0; i < n; i++) {
        t_vec[i] = std::forward_as_tuple(in[i], i);
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
    std::vector<T> out(size(in));
    inverse_permutation(out.data(), in.data(), size(in));
    return out;
}
} // namespace rng
} // namespace clt
