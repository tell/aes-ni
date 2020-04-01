#include <type_traits>
#include <tuple>

#include <x86intrin.h>

#include <clt/rdrand.hpp>

namespace clt {

// NOTE: https://qiita.com/kabakiyo/items/09974310d37d7df09e24
template <std::size_t N, class... Args> struct arg_type_impl {
    using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
};
// 可変個引数テンプレート
template <std::size_t N, class... Args> struct arg_type {
    using type = typename arg_type_impl<N, Args...>::type;
};
// 関数
template <std::size_t N, typename R, typename... Args>
struct arg_type<N, R (*)(Args...)> {
    using type = typename arg_type_impl<N, Args...>::type;
};

namespace rng {
#if defined(__RDRND__)
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