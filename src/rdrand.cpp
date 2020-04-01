#include <type_traits>
#include <tuple>

#include <x86intrin.h>

#include <clt/arg_type.hpp>
#include <clt/rdrand.hpp>

namespace clt {
namespace rng {
#if defined(__RDRND__) || defined(__APPLE__)
void rdrand(uint16_t *out, const size_t num_elems) noexcept
{
    for (size_t i = 0; i < num_elems; i += _rdrand16_step(out + i)) {
    }
}
void rdrand(uint32_t *out, const size_t num_elems) noexcept
{
    for (size_t i = 0; i < num_elems; i += _rdrand32_step(out + i)) {
    }
}
void rdrand(uint64_t *out, const size_t num_elems) noexcept
{
    using rdrand_arg_type =
        typename arg_type<0, decltype(&_rdrand64_step)>::type;
    for (size_t i = 0; i < num_elems;
         i += _rdrand64_step(reinterpret_cast<rdrand_arg_type>(out) + i)) {
    }
}
#endif
} // namespace rng
} // namespace clt