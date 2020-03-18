#pragma once

#include <x86intrin.h>

namespace clt {
constexpr int rcon_array[] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36,
};

namespace internal {
inline __m128i aes128_key_expansion_shift_xor(const __m128i &k0,
                                              const __m128i &k1) {
    __m128i key = _mm_xor_si128(k0, _mm_slli_si128(k0, 4));
    key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
    key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
    __m128i gen_key = _mm_shuffle_epi32(k1, _MM_SHUFFLE(3, 3, 3, 3));
    return _mm_xor_si128(key, gen_key);
}

template <size_t N> inline void aes128_key_expansion_impl(__m128i *keys) {
    static_assert(0 <= N);
    static_assert(N < aes128::num_rounds);
    // NOTE: The second argument in the next function call must be constant.
    const auto key_ass = _mm_aeskeygenassist_si128(keys[N], rcon_array[N]);
    keys[N + 1] = aes128_key_expansion_shift_xor(keys[N], key_ass);
    aes128_key_expansion_impl<N + 1>(keys);
}

template <>
inline void aes128_key_expansion_impl<aes128::num_rounds>(__m128i *) {}

template <size_t N> inline void aes128_key_expansion_imc_impl(__m128i *keys) {
    static_assert(0 <= N);
    static_assert(N < (aes128::num_rounds - 1));
    keys[N + 11] = _mm_aesimc_si128(keys[9 - N]);
    aes128_key_expansion_imc_impl<N + 1>(keys);
}

template <> inline void aes128_key_expansion_imc_impl<9>(__m128i *) {}
} // namespace internal
}