#pragma once

#include <cstdint>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <x86intrin.h>
#include <fmt/format.h>

namespace clt {
inline void m128i_to_uint8(uint8_t *out, const __m128i& in) {
    auto *out_ = reinterpret_cast<__m128i *>(out);
    _mm_storeu_si128(out_, in);
}
inline std::string join(const uint8_t *in, const size_t& n) {
    std::stringstream sst;
    if (n == 0) {
        return "";
    }
    sst << fmt::format("{:>02x}", (uint_fast64_t(in[0]) & 0xffull));
    if (n > 1)
    {
        for (size_t i = 1; i < n; i++) {
            sst << fmt::format(":{:>02x}", (uint_fast64_t(in[i]) & 0xffull));
        }
    }
    return sst.str();
}
template<class T, size_t N>
inline std::string join(const T (&in)[N]) { return join(in, N); }
inline std::string join(const __m128i *in) {
    const auto *in_ = reinterpret_cast<const uint8_t *>(in);
    return join(in_, sizeof(__m128i));
}
namespace aes128 {
constexpr size_t block_bytes = 16;
constexpr size_t key_bytes = 16;
constexpr uint8_t zero_key[key_bytes] = {0};
constexpr size_t num_rounds = 10;
} // namespace aes128
class AES128 {
    __m128i expanded_keys_[20];
public:
    explicit AES128(const uint8_t *key);
    explicit AES128() : AES128(aes128::zero_key) {}
    friend std::ostream& operator<<(std::ostream& ost, const AES128& x) {
        ost << "AES128[";
        for (size_t i = 0; i < 20; i++) {
            ost << "[";
            ost << join(&x.expanded_keys_[i]);
            ost << "]";
        }
        ost << "]";
        return ost;
    }
    void enc(uint8_t *out, const uint8_t *in) const;
    void enc(uint8_t *out, const uint8_t *in, const size_t iter_n) const;
};
} // namespace clt
// vim: set expandtab :
