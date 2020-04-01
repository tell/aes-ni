#pragma once

namespace clt {
namespace rng {

inline RandomDevice::RandomDevice(const path &dev_random)
    : fst_rdev_(dev_random, std::ios_base::in | std::ios_base::binary)
{
}

inline bool RandomDevice::operator()(void *buff, const size_t byte_size)
{
    using char_t = typename decltype(fst_rdev_)::char_type;
    fst_rdev_.read(reinterpret_cast<char_t *>(buff), byte_size);
    return fst_rdev_.good();
}

} // namespace rng
} // namespace clt