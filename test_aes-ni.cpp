#include "aes-ni.hpp"

using namespace std;

int main() {
    constexpr uint8_t sample_key[clt::aes128::key_bytes] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
    };
    cout << "A sample key = " << clt::join(sample_key) << endl;
    clt::AES128 cipher(sample_key);
    cout << cipher << endl;
    uint8_t sample_out[clt::aes128::block_bytes];
    const uint8_t sample_plaintext[clt::aes128::block_bytes] = {
        0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d,
        0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34,
    };
    const uint8_t sample_ciphertext[clt::aes128::block_bytes] = {
        0x39, 0x25, 0x84, 0x1d, 0x02, 0xdc, 0x09, 0xfb,
        0xdc, 0x11, 0x85, 0x97, 0x19, 0x6a, 0x0b, 0x32,
    };
    cout << "A sample plaintext = " << clt::join(sample_plaintext) << endl;
    cout << "A sample ciphertext = " << clt::join(sample_ciphertext) << endl;
    cipher.enc(sample_out, sample_plaintext, 1);
    cout << "enc(k, p) = " << clt::join(sample_out) << endl;
    for(size_t i = 0; i < clt::aes128::block_bytes; i++) {
        if(sample_ciphertext[i] != sample_out[i]) {
            return 1;
        }
    }
    cipher.dec(sample_out, sample_out);
    cout << "dec(k, c) = " << clt::join(sample_out) << endl;
    for(size_t i = 0; i < clt::aes128::block_bytes; i++) {
        if(sample_plaintext[i] != sample_out[i]) {
            return 1;
        }
    }
    return 0;
}
// vim: set expandtab :
