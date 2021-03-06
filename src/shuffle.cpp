#include <gmpxx.h>
static_assert(sizeof(uint64_t) == sizeof(unsigned long int));

#include <clt/shuffle.hpp>

namespace clt {
namespace internal {
inline mpz_class rank_impl(const size_t n, permutation_t &pi,
                           permutation_t &inv_pi)
{
    if (n == 1) {
        return 0;
    }
    const auto s = pi[n - 1];
    std::swap(pi[n - 1], pi[inv_pi[n - 1]]);
    std::swap(inv_pi[s], inv_pi[n - 1]);
    mpz_class mpz_s;
    mpz_set_ui64(mpz_s, s);
    return mpz_s + n * rank_impl(n - 1, pi, inv_pi);
}
} // namespace internal

mpz_class rank(const permutation_t &pi)
{
    const auto degree = pi.size();
    auto pi_ = pi;
    auto inv_pi = inverse_permutation(pi);
    return factorial(degree) - internal::rank_impl(degree, pi_, inv_pi) - 1;
}

namespace internal {
inline void unrank_impl(size_t n, mpz_class &r, permutation_t &pi)
{
    while (n > 0) {
        mpz_class r_mod_n;
        mpz_mod_ui(r_mod_n.get_mpz_t(), r.get_mpz_t(), n);
        assert(r_mod_n.fits_ulong_p());
        std::swap(pi[n - 1], pi[r_mod_n.get_ui()]);
        r /= n;
        n--;
    }
}
} // namespace internal

permutation_t unrank(const mpz_class &r, const size_t degree)
{
    permutation_t pi(degree);
    std::iota(pi.begin(), pi.end(), 0);
    mpz_class r_ = factorial(degree) - r - 1;
    internal::unrank_impl(degree, r_, pi);
    return pi;
}
} // namespace clt
