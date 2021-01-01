#include <tuple>

#include <clt/benchmark.hpp>

namespace clt {
namespace bench {

void dummy_call(void *x) { (void)x; }

namespace {

using namespace std;
using namespace std::chrono;

template <class... Args> constexpr inline decltype(auto) smt(Args &&... args)
{
    return make_tuple(forward<Args>(args)...);
}
constexpr auto sec_spec_map =
    smt(smt(seconds{}, 1e0), smt(milliseconds{}, 1e-3),
        smt(microseconds{}, 1e-6), smt(nanoseconds{}, 1e-9));
template <class T>
constexpr inline auto get_time_scale_of_sec_spec_impl(index_sequence<>)
{
    return -1.0;
}
template <class T, size_t I, size_t... Js>
constexpr inline auto get_time_scale_of_sec_spec_impl(index_sequence<I, Js...>)
{
    using U =
        decay_t<tuple_element_t<0, tuple_element_t<I, decltype(sec_spec_map)>>>;
    if constexpr (is_same_v<T, U>) {
        return get<1>(get<I>(sec_spec_map));
    } else {
        constexpr auto result =
            get_time_scale_of_sec_spec_impl<T, Js...>(index_sequence<Js...>{});
        static_assert(result > 0, "time scale is not found for given type.");
        return result;
    }
}
template <class T> constexpr inline auto get_time_scale_of_sec_spec()
{
    constexpr auto N = tuple_size_v<decltype(sec_spec_map)>;
    return get_time_scale_of_sec_spec_impl<T, 0>(make_index_sequence<N>{});
}

[[maybe_unused]] inline auto measure_walltime(const function<void()> &func)
{
    using sec_spec = microseconds;
    constexpr auto time_scale = get_time_scale_of_sec_spec<sec_spec>();

    const auto start = system_clock::now();
    (void)func();
    const auto stop = system_clock::now();
    return duration_cast<sec_spec>(stop - start).count() * time_scale;
}

[[maybe_unused]] inline auto measure_cputime(const function<void()> &func)
{
    const double start = clock();
    (void)func();
    const double stop = clock();
    return (stop - start) / CLOCKS_PER_SEC;
}
} // namespace

double measure_static(const function<void()> &func)
{
    return measure_walltime(func);
}

} // namespace bench
} // namespace clt
