#pragma once

#include <x86intrin.h>

namespace clt {
namespace internal {
namespace single {
template <size_t N>
inline void aes128_enc_impl(__m128i &m, const __m128i *keys) {
    static_assert(1 <= N);
    static_assert(N < 10);
    m = _mm_aesenc_si128(m, keys[N]);
    aes128_enc_impl<N + 1>(m, keys);
}

template <> inline void aes128_enc_impl<0>(__m128i &m, const __m128i *keys) {
    m = _mm_xor_si128(m, keys[0]);
    aes128_enc_impl<1>(m, keys);
}

template <> inline void aes128_enc_impl<10>(__m128i &m, const __m128i *keys) {
    m = _mm_aesenclast_si128(m, keys[10]);
}
} // namespace single
namespace sfinae {
template <size_t W, size_t N>
using width_round_t = std::integer_sequence<size_t, W, N>;

template <size_t W>
inline void aes128_enc_impl(__m128i *ms, const width_round_t<W, 10> &,
                            const __m128i *keys) {
    for (size_t i = 0; i < W; i++) {
        ms[i] = _mm_aesenclast_si128(ms[i], keys[10]);
    }
}

template <size_t W, size_t N,
          class T = std::enable_if_t<(0 < W) && (1 <= N) && (N < 10)>>
inline void aes128_enc_impl(__m128i *ms, const width_round_t<W, N> &,
                            const __m128i *keys) {
    // NOTE: Two static assertions are moved into SFINAE.
    for (size_t i = 0; i < W; i++) {
        ms[i] = _mm_aesenc_si128(ms[i], keys[N]);
    }
    aes128_enc_impl(ms, width_round_t<W, N + 1>{}, keys);
}

template <size_t W>
inline void aes128_enc_impl(__m128i *ms, const width_round_t<W, 0> &,
                            const __m128i *keys) {
    for (size_t i = 0; i < W; i++) {
        ms[i] = _mm_xor_si128(ms[i], keys[0]);
    }
    aes128_enc_impl(ms, width_round_t<W, 1>{}, keys);
}
} // namespace sfinae
} // namespace internal
} // namespace clt
