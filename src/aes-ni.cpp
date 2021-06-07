#include <x86intrin.h>

#include <clt/aes-ni.hpp>
#include <clt/rng.hpp>

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

AES128::key_t gen_key()
{
    AES128::key_t key;
    if (!clt::rng::rng_global(key.data(), aes128::key_bytes)) {
        throw std::runtime_error("Random bytes generation is failed.");
    }
    return key;
}

inline void aes128_key_expansion(__m128i *keys)
{
    internal::aes128_key_expansion_impl<0>(keys);
    internal::aes128_key_expansion_imc_impl<0>(keys);
}

AES128::AES128(const void *key) noexcept
{
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
inline void aes128_load_expkey_for_enc(__m128i *keys,
                                       const uint8_t *in) noexcept
{
    const auto *p_in = reinterpret_cast<const __m128i *>(in);
    for (size_t i = 0; i < (aes128::num_rounds + 1); i++) {
        keys[i] = _mm_loadu_si128(p_in + i);
    }
}
} // namespace internal

void AES128::enc(void *out, const void *in) const noexcept
{
    using internal::single::aes128_enc_impl;
    __m128i keys[11];
    internal::aes128_load_expkey_for_enc(keys, expanded_keys_);
    __m128i m = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in));
    aes128_enc_impl<0>(m, keys);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(out), m);
}

void AES128::enc(void *out, const void *in,
                 const size_t num_blocks) const noexcept
{
    // _mm256_zeroall();
    __m128i keys[11];
    internal::aes128_load_expkey_for_enc(keys, expanded_keys_);
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
        using internal::variadic::aes128_enc_impl;
        using internal::variadic::round_t;
        aes128_enc_impl(round_t<0>{}, keys, m0, m1, m2, m3);
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
        aes128_enc_impl<0>(m, keys);
        _mm_storeu_si128(p_out_last + i, m);
    }
}

auto AES128::ctr_stream(void *out, const uint64_t num_blocks,
                        const uint64_t start_count) const noexcept
    -> decltype(num_blocks + start_count)
{
    // _mm256_zeroall();
    __m128i keys[11];
    internal::aes128_load_expkey_for_enc(keys, expanded_keys_);
    auto ctr = _mm_cvtsi64_si128(start_count);
    const auto inc_v = _mm_cvtsi64_si128(1);
    for (size_t i = 0; i < num_blocks; i++) {
        __m128i m = ctr;
        ctr = _mm_add_epi64(ctr, inc_v);
        using internal::variadic::aes128_enc_impl;
        using internal::variadic::round_t;
        aes128_enc_impl(round_t<0>{}, keys, m);
        _mm_storeu_si128(reinterpret_cast<__m128i *>(out) + i, m);
    }
    return num_blocks + start_count;
}

auto AES128::ctr_byte_stream(void *out, const uint64_t num_bytes,
                             const uint64_t start_count) const noexcept
    -> decltype(num_bytes + start_count)
{
    const auto num_blocks = num_bytes / aes128::block_bytes;
    const auto rem_bytes = num_bytes % aes128::block_bytes;
    const auto counter = ctr_stream(out, num_blocks, start_count);
    if (rem_bytes > 0) {
        std::array<uint8_t, aes128::block_bytes> m;
#ifndef NDEBUG
        const auto counter_ =
#endif
            ctr_stream(m.data(), 1, counter);
        assert(counter_ == counter + 1);
        auto *const ptr_rem_out =
            reinterpret_cast<uint8_t *>(out) + num_blocks * aes128::block_bytes;
        std::copy(m.begin(), m.begin() + rem_bytes, ptr_rem_out);
        return counter + 1;
    } else {
        return counter;
    }
}

namespace internal {
inline void aes128_load_expkey_for_dec(__m128i *keys, const uint8_t *in,
                                       const uint8_t *last)
{
    const auto *p_in = reinterpret_cast<const __m128i *>(in);
    for (size_t i = 0; i < aes128::num_rounds; i++) {
        keys[i] = _mm_loadu_si128(p_in + i);
    }
    keys[10] = _mm_loadu_si128(reinterpret_cast<const __m128i *>(last));
}
} // namespace internal

void AES128::dec(void *out, const void *in) const noexcept
{
    __m128i keys[11];
    internal::aes128_load_expkey_for_dec(
        keys, expanded_keys_ + 10 * aes128::block_bytes, expanded_keys_);
    __m128i m = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in));
    using internal::single::aes128_dec_impl;
    aes128_dec_impl<0>(m, keys);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(out), m);
}

void AES128::dec(void *out, const void *in,
                 const size_t num_blocks) const noexcept
{
    // _mm256_zeroall();
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
        using internal::quad::aes128_dec_impl;
        aes128_dec_impl<0>(m0, m1, m2, m3, keys);
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
        using internal::single::aes128_dec_impl;
        aes128_dec_impl<0>(m, keys);
        _mm_storeu_si128(p_out_last + i, m);
    }
}

MMO128::MMO128(const void *key) noexcept
{
    __m128i keys[aes128::num_rounds + 1];
    static_assert(sizeof(keys) == sizeof(expanded_keys_));
    keys[0] = _mm_loadu_si128(reinterpret_cast<const __m128i *>(key));
    internal::aes128_key_expansion_impl<0>(keys);
    auto *p_out = reinterpret_cast<__m128i *>(expanded_keys_);
    for (size_t i = 0; i < std::size(keys); i++) {
        _mm_storeu_si128(p_out + i, keys[i]);
    }
}

void MMO128::operator()(void *out, const void *in) const noexcept
{
    using internal::single::aes128_enc_impl;
    __m128i keys[11];
    internal::aes128_load_expkey_for_enc(keys, expanded_keys_);
    __m128i m = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in));
    const __m128i t = m;
    aes128_enc_impl<0>(m, keys);
    m = _mm_xor_si128(m, t);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(out), m);
}

void MMO128::operator()(void *out, const void *in,
                        const size_t num_blocks) const noexcept
{
    // _mm256_zeroall();
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

auto MMO128::ctr_stream(void *out, const uint64_t num_blocks,
                        const uint64_t start_count) const noexcept
    -> decltype(num_blocks + start_count)
{
    // _mm256_zeroall();
    __m128i keys[11];
    internal::aes128_load_expkey_for_enc(keys, expanded_keys_);
    auto ctr = _mm_cvtsi64_si128(start_count);
    const auto inc_v = _mm_cvtsi64_si128(1);
    for (size_t i = 0; i < num_blocks; i++) {
        __m128i m = ctr;
        using internal::single::aes128_enc_impl;
        aes128_enc_impl<0>(m, keys);
        m = _mm_xor_si128(m, ctr);
        _mm_storeu_si128(reinterpret_cast<__m128i *>(out) + i, m);
        ctr = _mm_add_epi64(ctr, inc_v);
    }
    return num_blocks + start_count;
}

auto MMO128::ctr_byte_stream(void *out, const uint64_t num_bytes,
                             const uint64_t start_count) const noexcept
    -> decltype(num_bytes + start_count)
{
    const auto num_blocks = num_bytes / aes128::block_bytes;
    const auto rem_bytes = num_bytes % aes128::block_bytes;
    const auto counter = ctr_stream(out, num_blocks, start_count);
    if (rem_bytes > 0) {
        std::array<uint8_t, aes128::block_bytes> m;
#ifndef NDEBUG
        const auto counter_ =
#endif
            ctr_stream(m.data(), 1, counter);
        assert(counter_ == counter + 1);
        auto *const ptr_rem_out =
            reinterpret_cast<uint8_t *>(out) + num_blocks * aes128::block_bytes;
        std::copy(m.begin(), m.begin() + rem_bytes, ptr_rem_out);
        return counter + 1;
    } else {
        return counter;
    }
}

AESPRF128::AESPRF128(const void *key) noexcept
{
    __m128i keys[aes128::num_rounds + 1];
    static_assert(sizeof(keys) == sizeof(expanded_keys_));
    keys[0] = _mm_loadu_si128(reinterpret_cast<const __m128i *>(key));
    internal::aes128_key_expansion_impl<0>(keys);
    auto *p_out = reinterpret_cast<__m128i *>(expanded_keys_);
    for (size_t i = 0; i < std::size(keys); i++) {
        _mm_storeu_si128(p_out + i, keys[i]);
    }
}

void AESPRF128::operator()(void *out, const void *in) const noexcept
{
    __m128i keys[11];
    internal::aes128_load_expkey_for_enc(keys, expanded_keys_);
    __m128i m = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in));
    using internal::single::aesprf128_enc_impl;
    aesprf128_enc_impl<0>(m, keys);
    _mm_storeu_si128(reinterpret_cast<__m128i *>(out), m);
}

void AESPRF128::operator()(void *out, const void *in,
                           const size_t num_blocks) const noexcept
{
    // _mm256_zeroall();
    __m128i keys[11];
    internal::aes128_load_expkey_for_enc(keys, expanded_keys_);
    const auto *p_in = reinterpret_cast<const __m128i *>(in);
    assert(sizeof(__m128i) == (uintptr_t(p_in + 1) - uintptr_t(p_in)));
    auto *p_out = reinterpret_cast<__m128i *>(out);
    assert(sizeof(__m128i) == (uintptr_t(p_out + 1) - uintptr_t(p_out)));
    for (size_t i = 0; i < num_blocks; i++) {
        __m128i m = _mm_loadu_si128(p_in + i);
        using internal::single::aesprf128_enc_impl;
        aesprf128_enc_impl<0>(m, keys);
        _mm_storeu_si128(p_out + i, m);
    }
}

auto AESPRF128::ctr_stream(void *out, const uint64_t num_blocks,
                           const uint64_t start_count) const noexcept
    -> decltype(num_blocks + start_count)
{
    // _mm256_zeroall();
    __m128i keys[11];
    internal::aes128_load_expkey_for_enc(keys, expanded_keys_);
    auto ctr = _mm_cvtsi64_si128(start_count);
    const auto inc_v = _mm_cvtsi64_si128(1);
    for (size_t i = 0; i < num_blocks; i++) {
        __m128i m = ctr;
        ctr = _mm_add_epi64(ctr, inc_v);
        using internal::single::aesprf128_enc_impl;
        aesprf128_enc_impl<0>(m, keys);
        _mm_storeu_si128(reinterpret_cast<__m128i *>(out) + i, m);
    }
    return num_blocks + start_count;
}

auto AESPRF128::ctr_byte_stream(void *out, const uint64_t num_bytes,
                                const uint64_t start_count) const noexcept
    -> decltype(num_bytes + start_count)
{
    const auto num_blocks = num_bytes / aes128::block_bytes;
    const auto rem_bytes = num_bytes % aes128::block_bytes;
    const auto counter = ctr_stream(out, num_blocks, start_count);
    if (rem_bytes > 0) {
        std::array<uint8_t, aes128::block_bytes> m;
#ifndef NDEBUG
        const auto counter_ =
#endif
            ctr_stream(m.data(), 1, counter);
        assert(counter_ == counter + 1);
        auto *const ptr_rem_out =
            reinterpret_cast<uint8_t *>(out) + num_blocks * aes128::block_bytes;
        std::copy(m.begin(), m.begin() + rem_bytes, ptr_rem_out);
        return counter + 1;
    } else {
        return counter;
    }
}
} // namespace clt
// vim: set expandtab :
