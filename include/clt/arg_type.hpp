#pragma once

#include <type_traits>
#include <tuple>

// NOTE: https://qiita.com/kabakiyo/items/09974310d37d7df09e24

namespace clt {
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
// メンバー関数
template <std::size_t N, typename C, typename R, typename... Args>
struct arg_type<N, R (C::*)(Args...)> {
    using type = typename arg_type_impl<N, Args...>::type;
};
// メンバー関数（const）
template <std::size_t N, typename C, typename R, typename... Args>
struct arg_type<N, R (C::*)(Args...) const> {
    using type = typename arg_type_impl<N, Args...>::type;
};
}