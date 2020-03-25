#pragma once

namespace clt {
namespace bench {
namespace internal {
template <class Func> auto measure_walltime(Func &&f)
{
    using sec_spec = std::chrono::microseconds;
    constexpr double time_scale = 1e-6;

    const auto start = std::chrono::system_clock::now();
    f();
    const auto stop = std::chrono::system_clock::now();
    return std::chrono::duration_cast<sec_spec>(stop - start).count() *
           time_scale;
}

template <class Func> auto measure_cputime(Func &&f)
{
    const double start = clock();
    f();
    const double stop = clock();
    return (stop - start) / CLOCKS_PER_SEC;
}
} // namespace internal

template <class Func> auto measure(Func &&f)
{
    return internal::measure_cputime(std::forward<Func>(f));
}
} // namespace bench

constexpr size_t start_byte_size = 1 << 10;
constexpr size_t stop_byte_size = 1 << 30;

inline void init(std::vector<uint8_t> &buff, const size_t num_bytes)
{
    buff.resize(num_bytes);
    const auto status = clt::rng::rng_global(buff.data(), num_bytes);
    if (!status) {
        fmt::print(std::cerr, "ERROR!! gen_key is failed.");
        abort();
    }
}

inline void gen_key(clt::AES128::key_t &key)
{
    if (!clt::rng::rng_global(key.data(), clt::aes128::key_bytes)) {
        fmt::print(std::cerr, "ERROR!! gen_key is failed.");
    }
}
} // namespace clt