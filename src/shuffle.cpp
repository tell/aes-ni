#include <gmpxx.h>

#include <clt/shuffle.hpp>

namespace clt {
namespace rng {
namespace internal {
inline size_t rank_impl(const size_t n, perm_t &pi, perm_t &inv_pi)
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

size_t rank(const perm_t &pi)
{
    const auto degree = pi.size();
    assert(pi.size() < 13);
    auto pi_ = pi;
    auto inv_pi = inverse_permutation(pi);
    return internal::rank_impl(degree, pi_, inv_pi);
}

namespace internal {
inline void unrank_impl(size_t n, size_t r, perm_t &pi)
{
    while (n > 0) {
        std::swap(pi[n - 1], pi[r % n]);
        r /= n;
        n--;
    }
}
} // namespace internal

perm_t unrank(const size_t r, const size_t degree)
{
    perm_t pi(degree);
    std::iota(pi.begin(), pi.end(), 0);
    internal::unrank_impl(degree, r, pi);
    return pi;
}
} // namespace rng
} // namespace clt
