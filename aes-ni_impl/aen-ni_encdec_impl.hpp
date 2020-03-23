#pragma once

#include <x86intrin.h>

#include "../aes-ni.hpp"

namespace clt {
namespace internal {
inline void aes128_load_expkey_for_enc(__m128i *keys,
                                       const uint8_t *in) noexcept
{
    const auto *p_in = reinterpret_cast<const __m128i *>(in);
    for (size_t i = 0; i < (aes128::num_rounds + 1); i++) {
        keys[i] = _mm_loadu_si128(p_in + i);
    }
}

namespace single {
template <size_t N> inline void aes128_enc_impl(__m128i &m, const __m128i *keys)
{
    static_assert(1 <= N);
    static_assert(N < 10);
    m = _mm_aesenc_si128(m, keys[N]);
    aes128_enc_impl<N + 1>(m, keys);
}

template <> inline void aes128_enc_impl<0>(__m128i &m, const __m128i *keys)
{
    m = _mm_xor_si128(m, keys[0]);
    aes128_enc_impl<1>(m, keys);
}

template <> inline void aes128_enc_impl<10>(__m128i &m, const __m128i *keys)
{
    m = _mm_aesenclast_si128(m, keys[10]);
}

template <size_t N> inline void aes128_dec_impl(__m128i &m, const __m128i *keys)
{
    static_assert(1 <= N);
    static_assert(N < 10);
    m = _mm_aesdec_si128(m, keys[N]);
    aes128_dec_impl<N + 1>(m, keys);
}

template <> inline void aes128_dec_impl<0>(__m128i &m, const __m128i *keys)
{
    m = _mm_xor_si128(m, keys[0]);
    aes128_dec_impl<1>(m, keys);
}

template <> inline void aes128_dec_impl<10>(__m128i &m, const __m128i *keys)
{
    m = _mm_aesdeclast_si128(m, keys[10]);
}

template <size_t N>
inline void aesprf128_enc_impl(__m128i &m, const __m128i *keys)
{
    static_assert(1 <= N);
    static_assert(N < 10);
    m = _mm_aesenc_si128(m, keys[N]);
    aesprf128_enc_impl<N + 1>(m, keys);
}

template <> inline void aesprf128_enc_impl<0>(__m128i &m, const __m128i *keys)
{
    m = _mm_xor_si128(m, keys[0]);
    aesprf128_enc_impl<1>(m, keys);
}

template <> inline void aesprf128_enc_impl<5>(__m128i &m, const __m128i *keys)
{
    m = _mm_aesenc_si128(m, keys[5]);
    const __m128i m5 = m;
    aesprf128_enc_impl<6>(m, keys);
    m = _mm_xor_si128(m, m5);
}

template <> inline void aesprf128_enc_impl<10>(__m128i &m, const __m128i *keys)
{
    m = _mm_aesenclast_si128(m, keys[10]);
}
} // namespace single
namespace quad {
template <size_t N>
inline void aes128_enc_impl(__m128i &m0, __m128i &m1, __m128i &m2, __m128i &m3,
                            const __m128i *keys)
{
    static_assert(1 <= N);
    static_assert(N < 10);
    m0 = _mm_aesenc_si128(m0, keys[N]);
    m1 = _mm_aesenc_si128(m1, keys[N]);
    m2 = _mm_aesenc_si128(m2, keys[N]);
    m3 = _mm_aesenc_si128(m3, keys[N]);
    aes128_enc_impl<N + 1>(m0, m1, m2, m3, keys);
}

template <>
inline void aes128_enc_impl<0>(__m128i &m0, __m128i &m1, __m128i &m2,
                               __m128i &m3, const __m128i *keys)
{
    m0 = _mm_xor_si128(m0, keys[0]);
    m1 = _mm_xor_si128(m1, keys[0]);
    m2 = _mm_xor_si128(m2, keys[0]);
    m3 = _mm_xor_si128(m3, keys[0]);
    aes128_enc_impl<1>(m0, m1, m2, m3, keys);
}

template <>
inline void aes128_enc_impl<10>(__m128i &m0, __m128i &m1, __m128i &m2,
                                __m128i &m3, const __m128i *keys)
{
    m0 = _mm_aesenclast_si128(m0, keys[10]);
    m1 = _mm_aesenclast_si128(m1, keys[10]);
    m2 = _mm_aesenclast_si128(m2, keys[10]);
    m3 = _mm_aesenclast_si128(m3, keys[10]);
}

template <size_t N>
inline void aes128_dec_impl(__m128i &m0, __m128i &m1, __m128i &m2, __m128i &m3,
                            const __m128i *keys)
{
    static_assert(1 <= N);
    static_assert(N < 10);
    m0 = _mm_aesdec_si128(m0, keys[N]);
    m1 = _mm_aesdec_si128(m1, keys[N]);
    m2 = _mm_aesdec_si128(m2, keys[N]);
    m3 = _mm_aesdec_si128(m3, keys[N]);
    aes128_dec_impl<N + 1>(m0, m1, m2, m3, keys);
}

template <>
inline void aes128_dec_impl<0>(__m128i &m0, __m128i &m1, __m128i &m2,
                               __m128i &m3, const __m128i *keys)
{
    m0 = _mm_xor_si128(m0, keys[0]);
    m1 = _mm_xor_si128(m1, keys[0]);
    m2 = _mm_xor_si128(m2, keys[0]);
    m3 = _mm_xor_si128(m3, keys[0]);
    aes128_dec_impl<1>(m0, m1, m2, m3, keys);
}

template <>
inline void aes128_dec_impl<10>(__m128i &m0, __m128i &m1, __m128i &m2,
                                __m128i &m3, const __m128i *keys)
{
    m0 = _mm_aesdeclast_si128(m0, keys[10]);
    m1 = _mm_aesdeclast_si128(m1, keys[10]);
    m2 = _mm_aesdeclast_si128(m2, keys[10]);
    m3 = _mm_aesdeclast_si128(m3, keys[10]);
}
} // namespace quad
namespace sfinae {
template <size_t W, size_t N>
using width_round_t = std::integer_sequence<size_t, W, N>;

template <size_t W>
inline void aes128_enc_impl(__m128i *ms, const width_round_t<W, 10> &,
                            const __m128i *keys)
{
    for (size_t i = 0; i < W; i++) {
        ms[i] = _mm_aesenclast_si128(ms[i], keys[10]);
    }
}

template <size_t W, size_t N,
          class T = std::enable_if_t<(0 < W) && (1 <= N) && (N < 10)>>
inline void aes128_enc_impl(__m128i *ms, const width_round_t<W, N> &,
                            const __m128i *keys)
{
    // NOTE: Two static assertions are moved into SFINAE.
    for (size_t i = 0; i < W; i++) {
        ms[i] = _mm_aesenc_si128(ms[i], keys[N]);
    }
    aes128_enc_impl(ms, width_round_t<W, N + 1>{}, keys);
}

template <size_t W>
inline void aes128_enc_impl(__m128i *ms, const width_round_t<W, 0> &,
                            const __m128i *keys)
{
    for (size_t i = 0; i < W; i++) {
        ms[i] = _mm_xor_si128(ms[i], keys[0]);
    }
    aes128_enc_impl(ms, width_round_t<W, 1>{}, keys);
}
} // namespace sfinae
namespace variadic {
template <size_t Round> using round_t = std::integer_sequence<size_t, Round>;

template <class... Args>
inline void aes128_enc_impl(const round_t<10> &, const __m128i *keys,
                            Args &&... args)
{
    using swallow = std::initializer_list<int>;
    (void)swallow{(void(args = _mm_aesenclast_si128(args, keys[10])), 0)...};
}

template <size_t Round,
          class T = std::enable_if_t<(1 <= Round) && (Round < 10)>,
          class... Args>
inline void aes128_enc_impl(const round_t<Round> &, const __m128i *keys,
                            Args &&... args)
{
    using swallow = std::initializer_list<int>;
    (void)swallow{(void(args = _mm_aesenc_si128(args, keys[Round])), 0)...};
    aes128_enc_impl(round_t<Round + 1>{}, keys, std::forward<Args>(args)...);
}

template <class... Args>
inline void aes128_enc_impl(const round_t<0> &, const __m128i *keys,
                            Args &&... args)
{
    using swallow = std::initializer_list<int>;
    (void)swallow{(void(args = _mm_xor_si128(args, keys[0])), 0)...};
    aes128_enc_impl(round_t<1>{}, keys, std::forward<Args>(args)...);
}
} // namespace variadic
inline void hash_impl(uint8_t *out, const uint8_t *in, const size_t num_blocks,
                      const uint8_t *exp_keys) noexcept
{
    _mm256_zeroall();
    __m128i keys[11];
    aes128_load_expkey_for_enc(keys, exp_keys);
    constexpr size_t grain_size = 4;
    const size_t num_blocks_q = num_blocks / grain_size;
    const size_t num_blocks_r = num_blocks % grain_size;
    assert((4 * num_blocks_q + num_blocks_r) == num_blocks);
    const auto *p_in = reinterpret_cast<const __m128i *>(in);
    assert(sizeof(__m128i) == (uintptr_t(p_in + 1) - uintptr_t(p_in)));
    auto *p_out = reinterpret_cast<__m128i *>(out);
    assert(sizeof(__m128i) == (uintptr_t(p_out + 1) - uintptr_t(p_out)));
    for (size_t i = 0; i < num_blocks_q; i++) {
        const auto *p_inq = p_in + grain_size * i;
        __m128i m0 = _mm_loadu_si128(p_inq);
        __m128i m1 = _mm_loadu_si128(p_inq + 1);
        __m128i m2 = _mm_loadu_si128(p_inq + 2);
        __m128i m3 = _mm_loadu_si128(p_inq + 3);
        internal::quad::aes128_enc_impl<0>(m0, m1, m2, m3, keys);
        m0 = _mm_xor_si128(m0, *p_inq);
        m1 = _mm_xor_si128(m1, *(p_inq + 1));
        m2 = _mm_xor_si128(m2, *(p_inq + 2));
        m3 = _mm_xor_si128(m3, *(p_inq + 3));
        auto *p_outq = p_out + grain_size * i;
        _mm_storeu_si128(p_outq, m0);
        _mm_storeu_si128(p_outq + 1, m1);
        _mm_storeu_si128(p_outq + 2, m2);
        _mm_storeu_si128(p_outq + 3, m3);
    }
    const auto *p_in_last = p_in + grain_size * num_blocks_q;
    auto *p_out_last = p_out + grain_size * num_blocks_q;
    using internal::single::aes128_enc_impl;
    for (size_t i = 0; i < num_blocks_r; i++) {
        __m128i m = _mm_loadu_si128(p_in_last + i);
        const __m128i t = m;
        aes128_enc_impl<0>(m, keys);
        m = _mm_xor_si128(m, t);
        _mm_storeu_si128(p_out_last + i, m);
    }
}
} // namespace internal
} // namespace clt
