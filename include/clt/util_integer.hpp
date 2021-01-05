#pragma once

#include <type_traits>

namespace clt {
template <class IntType> inline auto least2pow(IntType n) -> decltype(n + 1)
{
    static_assert(std::is_integral_v<IntType>);
    static_assert(std::is_unsigned_v<IntType>);
    constexpr auto bit_size = sizeof(IntType) * CHAR_BIT;
    if (n == 0) {
        return 0;
    }
    n -= 1;
    for (size_t i = 1; i < bit_size; i <<= 1) {
        n |= n >> i;
    }
    return n + 1;
}
} // namespace clt
