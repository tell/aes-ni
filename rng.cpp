#include <clt/rng.hpp>

namespace clt {
namespace rng {
bool RNG::operator()(void *buff, const size_t byte_size)
{
    using char_t = typename decltype(fst_rdev_)::char_type;
    fst_rdev_.read(reinterpret_cast<char_t *>(buff), byte_size);
    return fst_rdev_.good();
}

RNG rng_global;
} // namespace rng
} // namespace clt