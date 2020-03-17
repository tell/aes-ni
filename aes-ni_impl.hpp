#pragma once

#include <iostream>

namespace clt {
inline std::ostream &operator<<(std::ostream &ost, const AES128 &x) {
    ost << "AES128[";
    for (size_t i = 0; i < 20; i++) {
        ost << "[";
        ost << join(&x.expanded_keys_[i], aes128::block_bytes);
        ost << "]";
    }
    ost << "]";
    return ost;
}
} // namespace clt