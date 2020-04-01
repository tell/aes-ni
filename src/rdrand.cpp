#include <x86intrin.h>

#include <clt/rdrand.hpp>

namespace clt {
namespace rng {
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
    for (size_t i = 0; i < num_elems; i += _rdrand64_step(out + i)) {
    }
}
} // namespace rng
} // namespace clt