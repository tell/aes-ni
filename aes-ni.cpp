#include <x86intrin.h>

#include "aes-ni.hpp"

/**
 * NOTE: According to
 * https://software.intel.com/sites/landingpage/IntrinsicsGuide/#othertechs=AES&expand=233
 * AESENC, AESENCLAST, AESDEC, AESDECLAST are latency 4 and throughput 1 on
 * Skylake CPUs. Therefore, 4 independent blocks can be processed in parallel by
 * pipelining.
 */

namespace clt {

static_assert(aes128::block_bytes == sizeof(__m128i));
static_assert(aes128::key_bytes == sizeof(__m128i));

} // namespace clt

#include "aes-ni_impl/aen-ni_enc_impl.hpp"
#include "aes-ni_impl/aes-ni_key-exp_impl.hpp"

namespace clt {
inline void aes128_key_expansion(__m128i *keys) {
    internal::aes128_key_expansion_impl<0>(keys);
    internal::aes128_key_expansion_imc_impl<0>(keys);
}

AES128::AES128(const uint8_t *key) noexcept {
    __m128i keys[2 * aes128::num_rounds];
    static_assert(sizeof(keys) == sizeof(expanded_keys_));
    keys[0] = _mm_loadu_si128(reinterpret_cast<const __m128i *>(key));
    aes128_key_expansion(keys);
    auto *p_out = reinterpret_cast<__m128i *>(expanded_keys_);
    for (size_t i = 0; i < std::size(keys); i++) {
        _mm_storeu_si128(p_out + i, keys[i]);
    }
}

namespace internal {
template <size_t N>
inline void aes128_enc_impl(__m128i &m0, __m128i &m1, const __m128i *keys) {
    static_assert(1 <= N);
    static_assert(N < 10);
    m0 = _mm_aesenc_si128(m0, keys[N]);
    m1 = _mm_aesenc_si128(m1, keys[N]);
    aes128_enc_impl<N + 1>(m0, m1, keys);
}

template <>
inline void aes128_enc_impl<0>(__m128i &m0, __m128i &m1, const __m128i *keys) {
    m0 = _mm_xor_si128(m0, keys[0]);
    m1 = _mm_xor_si128(m1, keys[0]);
    aes128_enc_impl<1>(m0, m1, keys);
}

template <>
inline void aes128_enc_impl<10>(__m128i &m0, __m128i &m1, const __m128i *keys) {
    m0 = _mm_aesenclast_si128(m0, keys[10]);
    m1 = _mm_aesenclast_si128(m1, keys[10]);
}

inline void aes128_load_expkey_for_enc(__m128i *keys, const uint8_t *in) {
    const auto *p_in = reinterpret_cast<const __m128i *>(in);
    for (size_t i = 0; i < (aes128::num_rounds + 1); i++) {
        keys[i] = _mm_loadu_si128(p_in + i);
    }
}
} // namespace internal

void AES128::enc(uint8_t *out, const uint8_t *in) const noexcept {
    using internal::single::aes128_enc_impl;
    __m128i keys[11];
    internal::aes128_load_expkey_for_enc(keys, expanded_keys_);
    __m128i m = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in));
    aes128_enc_impl<0>(m, keys);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(out), m);
}

void AES128::enc(uint8_t *out, const uint8_t *in, const size_t num_blocks) const
    noexcept {
    _mm256_zeroall();
    __m128i keys[11];
    internal::aes128_load_expkey_for_enc(keys, expanded_keys_);
    const size_t num_blocks_q4 = num_blocks / 4;
    const size_t num_blocks_r4 = num_blocks % 4;
    assert((4 * num_blocks_q4 + num_blocks_r4) == num_blocks);
    const auto *p_in = reinterpret_cast<const __m128i *>(in);
    assert(sizeof(__m128i) == (uintptr_t(p_in + 1) - uintptr_t(p_in)));
    auto *p_out = reinterpret_cast<__m128i *>(out);
    assert(sizeof(__m128i) == (uintptr_t(p_out + 1) - uintptr_t(p_out)));
    for (size_t i = 0; i < num_blocks_q4; i++) {
        const auto *p_in4 = p_in + 4 * i;
        __m128i m0 = _mm_loadu_si128(p_in4);
        __m128i m1 = _mm_loadu_si128(p_in4 + 1);
        __m128i m2 = _mm_loadu_si128(p_in4 + 2);
        __m128i m3 = _mm_loadu_si128(p_in4 + 3);
        internal::quad::aes128_enc_impl<0>(m0, m1, m2, m3, keys);
        auto *p_out4 = p_out + 4 * i;
        _mm_storeu_si128(p_out4, m0);
        _mm_storeu_si128(p_out4 + 1, m1);
        _mm_storeu_si128(p_out4 + 2, m2);
        _mm_storeu_si128(p_out4 + 3, m3);
    }
    const auto *p_in_last = p_in + 4 * num_blocks_q4;
    auto *p_out_last = p_out + 4 * num_blocks_q4;
    using internal::single::aes128_enc_impl;
    for (size_t i = 0; i < num_blocks_r4; i++) {
        __m128i m = _mm_loadu_si128(p_in_last + i);
        aes128_enc_impl<0>(m, keys);
        _mm_storeu_si128(p_out_last + i, m);
    }
}

namespace internal {
template <size_t N>
inline void aes128_dec_impl(__m128i &m, const __m128i *keys) {
    static_assert(1 <= N);
    static_assert(N < 10);
    m = _mm_aesdec_si128(m, keys[N]);
    aes128_dec_impl<N + 1>(m, keys);
}

template <> inline void aes128_dec_impl<0>(__m128i &m, const __m128i *keys) {
    m = _mm_xor_si128(m, keys[0]);
    aes128_dec_impl<1>(m, keys);
}

template <> inline void aes128_dec_impl<10>(__m128i &m, const __m128i *keys) {
    m = _mm_aesdeclast_si128(m, keys[10]);
}

template <size_t N>
inline void aes128_dec_impl(__m128i &m0, __m128i &m1, __m128i &m2, __m128i &m3,
                            const __m128i *keys) {
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
                               __m128i &m3, const __m128i *keys) {
    m0 = _mm_xor_si128(m0, keys[0]);
    m1 = _mm_xor_si128(m1, keys[0]);
    m2 = _mm_xor_si128(m2, keys[0]);
    m3 = _mm_xor_si128(m3, keys[0]);
    aes128_dec_impl<1>(m0, m1, m2, m3, keys);
}

template <>
inline void aes128_dec_impl<10>(__m128i &m0, __m128i &m1, __m128i &m2,
                                __m128i &m3, const __m128i *keys) {
    m0 = _mm_aesdeclast_si128(m0, keys[10]);
    m1 = _mm_aesdeclast_si128(m1, keys[10]);
    m2 = _mm_aesdeclast_si128(m2, keys[10]);
    m3 = _mm_aesdeclast_si128(m3, keys[10]);
}

inline void aes128_load_expkey_for_dec(__m128i *keys, const uint8_t *in,
                                       const uint8_t *last) {
    const auto *p_in = reinterpret_cast<const __m128i *>(in);
    for (size_t i = 0; i < aes128::num_rounds; i++) {
        keys[i] = _mm_loadu_si128(p_in + i);
    }
    keys[10] = _mm_loadu_si128(reinterpret_cast<const __m128i *>(last));
}
} // namespace internal

void AES128::dec(uint8_t *out, const uint8_t *in) const noexcept {
    __m128i keys[11];
    internal::aes128_load_expkey_for_dec(
        keys, expanded_keys_ + 10 * aes128::block_bytes, expanded_keys_);
    __m128i m = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in));
    internal::aes128_dec_impl<0>(m, keys);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(out), m);
}

void AES128::dec(uint8_t *out, const uint8_t *in, const size_t num_blocks) const
    noexcept {
    _mm256_zeroall();
    __m128i keys[11];
    internal::aes128_load_expkey_for_dec(
        keys, expanded_keys_ + 10 * aes128::block_bytes, expanded_keys_);
    const size_t num_blocks_q4 = num_blocks / 4;
    const size_t num_blocks_r4 = num_blocks % 4;
    assert((4 * num_blocks_q4 + num_blocks_r4) == num_blocks);
    const auto *p_in = reinterpret_cast<const __m128i *>(in);
    assert(sizeof(__m128i) == (uintptr_t(p_in + 1) - uintptr_t(p_in)));
    auto *p_out = reinterpret_cast<__m128i *>(out);
    assert(sizeof(__m128i) == (uintptr_t(p_out + 1) - uintptr_t(p_out)));
    for (size_t i = 0; i < num_blocks_q4; i++) {
        const auto *p_in4 = p_in + 4 * i;
        __m128i m0 = _mm_loadu_si128(p_in4);
        __m128i m1 = _mm_loadu_si128(p_in4 + 1);
        __m128i m2 = _mm_loadu_si128(p_in4 + 2);
        __m128i m3 = _mm_loadu_si128(p_in4 + 3);
        internal::aes128_dec_impl<0>(m0, m1, m2, m3, keys);
        auto *p_out4 = p_out + 4 * i;
        _mm_storeu_si128(p_out4, m0);
        _mm_storeu_si128(p_out4 + 1, m1);
        _mm_storeu_si128(p_out4 + 2, m2);
        _mm_storeu_si128(p_out4 + 3, m3);
    }
    const auto *p_in_last = p_in + 4 * num_blocks_q4;
    auto *p_out_last = p_out + 4 * num_blocks_q4;
    for (size_t i = 0; i < num_blocks_r4; i++) {
        __m128i m = _mm_loadu_si128(p_in_last + i);
        internal::aes128_dec_impl<0>(m, keys);
        _mm_storeu_si128(p_out_last + i, m);
    }
}

MMO128::MMO128(const uint8_t *key) noexcept {
    __m128i keys[aes128::num_rounds + 1];
    static_assert(sizeof(keys) == sizeof(expanded_keys_));
    keys[0] = _mm_loadu_si128(reinterpret_cast<const __m128i *>(key));
    internal::aes128_key_expansion_impl<0>(keys);
    auto *p_out = reinterpret_cast<__m128i *>(expanded_keys_);
    for (size_t i = 0; i < std::size(keys); i++) {
        _mm_storeu_si128(p_out + i, keys[i]);
    }
}

void MMO128::hash(uint8_t *out, const uint8_t *in) const noexcept {
    using internal::single::aes128_enc_impl;
    __m128i keys[11];
    internal::aes128_load_expkey_for_enc(keys, expanded_keys_);
    __m128i m = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in));
    const __m128i t = m;
    aes128_enc_impl<0>(m, keys);
    m = _mm_xor_si128(m, t);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(out), m);
}

void hash_impl(uint8_t *out, const uint8_t *in, const size_t num_blocks,
               const uint8_t *exp_keys) noexcept {
    _mm256_zeroall();
    __m128i keys[11];
    internal::aes128_load_expkey_for_enc(keys, exp_keys);
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

void MMO128::hash(uint8_t *out, const uint8_t *in,
                  const size_t num_blocks) const noexcept {
    _mm256_zeroall();
    __m128i keys[11];
    internal::aes128_load_expkey_for_enc(keys, expanded_keys_);
    const auto *p_in = reinterpret_cast<const __m128i *>(in);
    assert(sizeof(__m128i) == (uintptr_t(p_in + 1) - uintptr_t(p_in)));
    auto *p_out = reinterpret_cast<__m128i *>(out);
    assert(sizeof(__m128i) == (uintptr_t(p_out + 1) - uintptr_t(p_out)));
    using internal::single::aes128_enc_impl;
    for (size_t i = 0; i < num_blocks; i++) {
        __m128i m = _mm_loadu_si128(p_in + i);
        aes128_enc_impl<0>(m, keys);
        m = _mm_xor_si128(m, *(p_in + i));
        _mm_storeu_si128(p_out + i, m);
    }
}
} // namespace clt
// vim: set expandtab :
