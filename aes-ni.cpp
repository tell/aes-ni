#include <x86intrin.h>
#include "aes-ni.hpp"

namespace clt {

constexpr int rcon_array[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};

inline __m128i aes128_key_exp_internal(const __m128i& k0, const __m128i& k1) {
    __m128i key = _mm_xor_si128(k0, _mm_slli_si128(k0, 4));
    key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
    key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
    __m128i gen_key = _mm_shuffle_epi32(k1, _MM_SHUFFLE(3, 3, 3, 3));
    return _mm_xor_si128(key, gen_key);
}

template<size_t N>
inline void aes128_key_exp_loop(__m128i *keys) {
    static_assert(0 <= N);
    static_assert(N < aes128::num_rounds);
    const auto key_ass = _mm_aeskeygenassist_si128(keys[N], rcon_array[N]);
    keys[N + 1] = aes128_key_exp_internal(keys[N], key_ass);
    aes128_key_exp_loop<N + 1>(keys);
}

template<>
inline void aes128_key_exp_loop<aes128::num_rounds>(__m128i *) {}

AES128::AES128(const uint8_t *key) {
    const auto *key_ = reinterpret_cast<const __m128i *>(key);
    expanded_keys_[0] = _mm_loadu_si128(key_);
    aes128_key_exp_loop<0>(expanded_keys_);
}

} // namespace clt
// vim: set expandtab :
