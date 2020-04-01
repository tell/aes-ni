#pragma once

#include <iostream>

#include "../util.hpp"

namespace clt {
inline std::ostream &operator<<(std::ostream &ost, const AES128 &x)
{
    static_assert((sizeof(x.expanded_keys_) % aes128::block_bytes) == 0);
    constexpr size_t num_exp_keys =
        sizeof(x.expanded_keys_) / aes128::block_bytes;
    ost << "AES128[";
    for (size_t i = 0; i < num_exp_keys; i++) {
        ost << "[";
        ost << join(&x.expanded_keys_[i], aes128::block_bytes);
        ost << "]";
    }
    ost << "]";
    return ost;
}

inline std::ostream &operator<<(std::ostream &ost, const MMO128 &x)
{
    static_assert((sizeof(x.expanded_keys_) % aes128::block_bytes) == 0);
    constexpr size_t num_exp_keys =
        sizeof(x.expanded_keys_) / aes128::block_bytes;
    ost << "MMO128[";
    for (size_t i = 0; i < num_exp_keys; i++) {
        ost << "[";
        ost << join(&x.expanded_keys_[i], aes128::block_bytes);
        ost << "]";
    }
    ost << "]";
    return ost;
}

inline std::ostream &operator<<(std::ostream &ost, const AESPRF128 &x)
{
    static_assert((sizeof(x.expanded_keys_) % aes128::block_bytes) == 0);
    constexpr size_t num_exp_keys =
        sizeof(x.expanded_keys_) / aes128::block_bytes;
    ost << "AESPRF128[";
    for (size_t i = 0; i < num_exp_keys; i++) {
        ost << "[";
        ost << join(&x.expanded_keys_[i], aes128::block_bytes);
        ost << "]";
    }
    ost << "]";
    return ost;
}
} // namespace clt
