#pragma once

#include <cstdint>

namespace clt {
namespace rng {
// NOTE: https://software.intel.com/sites/landingpage/IntrinsicsGuide/#expand=4546,4539,4539&text=rdrand
void rdrand(uint16_t *out, const size_t num_elems);
void rdrand(uint32_t *out, const size_t num_elems);
void rdrand(uint64_t *out, const size_t num_elems);
using generic_rdrand_t = uint64_t;
template <class T> void rdrand(T *out, const size_t num_elems)
{
    rdrand(reinterpret_cast<uint64_t *>(out), num_elems);
}
} // namespace rng
} // namespace clt
