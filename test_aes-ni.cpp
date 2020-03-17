#include "aes-ni.hpp"

using namespace std;

int main() {
    constexpr uint8_t sample_key[clt::aes128::key_bytes] = {
        // 2b7e151628aed2a6abf7158809cf4f3c
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
    };
    cout << "A sample key = " << clt::join(sample_key) << endl;
    clt::AES128 cipher(sample_key);
    cout << cipher << endl;
    uint8_t sample_out[clt::aes128::block_bytes];
    const uint8_t sample_plaintext[clt::aes128::block_bytes] = {
        // 6bc1bee22e409f96e93d7e117393172a
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
        0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    };
    const uint8_t sample_ciphertext[clt::aes128::block_bytes] = {
        // 3ad77bb40d7a3660a89ecaf32466ef97
        0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60,
        0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97,
    };
    cout << "A sample plaintext = " << clt::join(sample_plaintext) << endl;
    cout << "A sample ciphertext = " << clt::join(sample_ciphertext) << endl;
    cipher.enc(sample_out, sample_plaintext, 1);
    cout << "enc(k, p) = " << clt::join(sample_out) << endl;
    for (size_t i = 0; i < clt::aes128::block_bytes; i++) {
        if (sample_ciphertext[i] != sample_out[i]) {
            cout << "!!!!" << endl;
        }
    }
    cipher.dec(sample_out, sample_out, 1);
    cout << "dec(k, c) = " << clt::join(sample_out) << endl;
    for (size_t i = 0; i < clt::aes128::block_bytes; i++) {
        if (sample_plaintext[i] != sample_out[i]) {
            cout << "!!!!" << endl;
        }
    }
    return 0;
}
// vim: set expandtab :
