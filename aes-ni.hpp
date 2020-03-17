#pragma once

#include <cstdint>
#include <iostream>
#include <x86intrin.h>

namespace clt {
namespace aes128 {
constexpr size_t key_bytes = 16;
constexpr uint8_t zero_key[key_bytes] = { 0x00, };
constexpr size_t num_rounds = 10;
} // namespace aes128
class AES128 {
    __m128i expanded_keys_[20];
public:
    explicit AES128(const uint8_t *key);
    explicit AES128() : AES128(aes128::zero_key) {}
    friend std::ostream& operator<<(std::ostream& ost, const AES128& x) {
        return ost;
    }
};
} // namespace clt
// vim: set expandtab :
