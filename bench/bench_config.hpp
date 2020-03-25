#pragma once

namespace clt {
namespace bench {
namespace internal {
template <class Func> inline auto measure_walltime(Func &&f)
{
    using sec_spec = std::chrono::microseconds;
    constexpr double time_scale = 1e-6;

    const auto start = std::chrono::system_clock::now();
    f();
    const auto stop = std::chrono::system_clock::now();
    return std::chrono::duration_cast<sec_spec>(stop - start).count() *
           time_scale;
}

template <class Func> inline auto measure_cputime(Func &&f)
{
    const double start = clock();
    f();
    const double stop = clock();
    return (stop - start) / CLOCKS_PER_SEC;
}
} // namespace internal

template <class Func> inline auto measure(Func &&func)
{
    return internal::measure_cputime(std::forward<Func>(func));
}

template <class Func>
inline void print_throughput(const std::string &label, Func &&func,
                             const size_t num_bytes)
{
    while (true) {
        const std::string format = label + ",{},{:.5e}\n";
        const auto elapsed_time = measure(std::forward<Func>(func));
        const auto bytes_per_sec = num_bytes / elapsed_time;
        if (std::isinf(bytes_per_sec)) {
            fmt::print(std::cerr,
                       "Obtained throughput is the infinity, try again...\n");
        } else {
            fmt::print(format, num_bytes, bytes_per_sec);
            break;
        }
    }
}
} // namespace bench

constexpr size_t start_byte_size = 1 << 10;
constexpr size_t stop_byte_size = 1 << 30;

inline void init(std::vector<uint8_t> &buff, const size_t num_bytes)
{
    buff.resize(num_bytes);
    const auto status = clt::rng::rng_global(buff.data(), num_bytes);
    if (!status) {
        fmt::print(std::cerr, "ERROR!! failed: {}\n", __func__);
        abort();
    }
}

inline void gen_key(clt::AES128::key_t &key)
{
    if (!clt::rng::rng_global(key.data(), clt::aes128::key_bytes)) {
        fmt::print(std::cerr, "ERROR!! failed: {}\n", __func__);
    }
}

template <class T>
inline bool eq_check(const std::vector<T> &buff0, const std::vector<T> &buff1)
{
    assert(buff0.size() == buff1.size());
    const size_t num_elems = buff0.size();
    for (size_t i = 0; i < num_elems; i++) {
        if (buff0[i] != buff1[i]) {
            return false;
        }
    }
    return true;
}
} // namespace clt