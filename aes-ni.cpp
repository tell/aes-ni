#include <x86intrin.h>
#include "aes-ni.hpp"

namespace clt {

constexpr int rcon_array[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, };

namespace internal {
inline __m128i aes128_key_exp_internal(const __m128i &k0, const __m128i &k1) {
    __m128i key = _mm_xor_si128(k0, _mm_slli_si128(k0, 4));
    key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
    key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
    __m128i gen_key = _mm_shuffle_epi32(k1, _MM_SHUFFLE(3, 3, 3, 3));
    return _mm_xor_si128(key, gen_key);
}

template<size_t N>
inline void aes128_key_expansion_impl(__m128i *keys) {
    static_assert(0 <= N);
    static_assert(N < aes128::num_rounds);
    const auto key_ass = _mm_aeskeygenassist_si128(keys[N], rcon_array[N]);
    keys[N + 1] = aes128_key_exp_internal(keys[N], key_ass);
    aes128_key_expansion_impl<N + 1>(keys);
}

template<>
inline void aes128_key_expansion_impl<aes128::num_rounds>(__m128i *) {}

template<size_t N>
inline void aes128_key_expansion_imc_impl(__m128i *keys) {
    static_assert(0 <= N);
    static_assert(N < (aes128::num_rounds - 1));
    keys[N + 11] = _mm_aesimc_si128(keys[9 - N]);
    aes128_key_expansion_imc_impl<N + 1>(keys);
}

template<>
inline void aes128_key_expansion_imc_impl<9>(__m128i *) {}
} // namespace internal

inline void aes128_key_expansion(__m128i *keys) {
    internal::aes128_key_expansion_impl<0>(keys);
    internal::aes128_key_expansion_imc_impl<0>(keys);
}

AES128::AES128(const uint8_t *key) {
    expanded_keys_[0] = _mm_loadu_si128(reinterpret_cast<const __m128i *>(key));
    aes128_key_expansion(expanded_keys_);
}

namespace internal {
template<size_t N>
void aes128_enc_impl(__m128i& m, const __m128i *keys) {
    static_assert(1 <= N);
    static_assert(N < 10);
    m = _mm_aesenc_si128(m, keys[N]);
    aes128_enc_impl<N + 1>(m, keys);
}

template <>
void aes128_enc_impl<0>(__m128i& m, const __m128i *keys)
{
    m = _mm_xor_si128(m, keys[0]);
    aes128_enc_impl<1>(m, keys);
}

template <>
void aes128_enc_impl<10>(__m128i& m, const __m128i *keys) {
    m = _mm_aesenclast_si128(m, keys[10]);
}

template <size_t N>
void aes128_enc_impl(__m128i &m0, __m128i &m1, __m128i &m2, __m128i &m3, const __m128i *keys)
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
void aes128_enc_impl<0>(__m128i &m0, __m128i &m1, __m128i &m2, __m128i &m3, const __m128i *keys)
{
    m0 = _mm_xor_si128(m0, keys[0]);
    m1 = _mm_xor_si128(m1, keys[0]);
    m2 = _mm_xor_si128(m2, keys[0]);
    m3 = _mm_xor_si128(m3, keys[0]);
    aes128_enc_impl<1>(m0, m1, m2, m3, keys);
}

template <>
void aes128_enc_impl<10>(__m128i &m0, __m128i &m1, __m128i &m2, __m128i &m3, const __m128i *keys)
{
    m0 = _mm_aesenclast_si128(m0, keys[10]);
    m1 = _mm_aesenclast_si128(m1, keys[10]);
    m2 = _mm_aesenclast_si128(m2, keys[10]);
    m3 = _mm_aesenclast_si128(m3, keys[10]);
}
} // namespace internal

void AES128::enc(uint8_t *out, const uint8_t *in) const
{
    __m128i m = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in));
    internal::aes128_enc_impl<0>(m, expanded_keys_);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(out), m);
}

void AES128::enc(uint8_t *out, const uint8_t *in, const size_t iter_n) const
{
    const size_t iter_q4 = iter_n / 4;
    const size_t iter_r4 = iter_n % 4;
    assert((4 * iter_q4 + iter_r4) == iter_n);
    for (size_t i = 0; i < iter_q4; i++)
    {
        __m128i m0 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in) + 4 * i);
        __m128i m1 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in) + 4 * i + 1);
        __m128i m2 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in) + 4 * i + 2);
        __m128i m3 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in) + 4 * i + 3);
        internal::aes128_enc_impl<0>(m0, m1, m2, m3, expanded_keys_);
        _mm_storeu_si128(reinterpret_cast<__m128i *>(out) + 4 * i, m0);
        _mm_storeu_si128(reinterpret_cast<__m128i *>(out) + 4 * i + 1, m1);
        _mm_storeu_si128(reinterpret_cast<__m128i *>(out) + 4 * i + 2, m2);
        _mm_storeu_si128(reinterpret_cast<__m128i *>(out) + 4 * i + 3, m3);
    }
    for (size_t i = 0; i < iter_r4; i++)
    {
        __m128i m = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in) + 4 * iter_q4 + i);
        internal::aes128_enc_impl<0>(m, expanded_keys_);
        _mm_storeu_si128(reinterpret_cast<__m128i *>(out) + 4 * iter_q4 + i, m);
    }
}
} // namespace clt
// vim: set expandtab :
