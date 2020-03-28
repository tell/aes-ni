#pragma once

#include <iostream>
#include <vector>

#if defined(__linux__)
#include <string.h>
#include <sys/random.h>
#endif

#include <fstream>
#if __cpp_lib_filesystem && !defined(__APPLE__)
#include <filesystem>
namespace clt {
using path = std::filesystem::path;
}
#else
namespace clt {
using path = std::string;
}
#endif

#include <fmt/format.h>

#include <clt/util.hpp>

namespace clt {
namespace rng {
class RNG {
    std::ifstream fst_rdev_;

public:
    RNG(const path &dev_random = "/dev/urandom")
        : fst_rdev_(dev_random, std::ios_base::in | std::ios_base::binary)
    {
    }

    bool operator()(void *buff, const size_t byte_size);
};

extern RNG rng_global;

template <class T> inline void init(T *buff, const size_t num_elems)
{
    const size_t num_bytes = sizeof(T) * num_elems;
    const auto status = rng_global(buff, num_bytes);
    if (!status) {
        fmt::print(std::cerr, "ERROR!! failed: {}\n", __func__);
        abort();
    }
}

template <class T> inline void init(T &buff)
{
    init(buff.data(), std::size(buff));
}

inline void getrandom([[maybe_unused]] void *out,
                      [[maybe_unused]] const size_t num_bytes)
{
#if defined(__linux__)
    auto *p_out = reinterpret_cast<uint8_t *>(out);
    size_t gened_bytes = 0;
    errno = 0;
    do {
        const auto ret_bytes =
            ::getrandom(p_out, num_bytes - gened_bytes, GRND_NONBLOCK);
        if (ret_bytes < 0) {
            const auto str = std::string(strerror(errno));
            throw std::runtime_error(str);
        }
        p_out += ret_bytes;
        gened_bytes += ret_bytes;
    } while (gened_bytes < num_bytes);
#else
    throw std::runtime_error("not implemented yet");
#endif
}

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
    assert(out != in);
    const auto end_iter = std::next(in, n);
    for (size_t i = 0; i < n; i++) {
        const auto at_v = std::find(in, end_iter, i);
        if (at_v == end_iter) {
            throw std::runtime_error(fmt::format("Not found index: {}", i));
        }
        const auto index = std::distance(in, at_v);
        out[i] = index;
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
