#include <vector>
#include <numeric>
#include <algorithm>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <clt/aes-ni.hpp>
#include <clt/rng.hpp>
#include <clt/shuffle.hpp>
#include <clt/statistics.hpp>

#include <gtest/gtest.h>

using namespace std;
using namespace clt;
using namespace clt::rng;

class BasicTest : public ::testing::Test {
protected:
    static vector<uint8_t> sample_key_;
    vector<uint8_t> random_key_;
    static void set_sample_key()
    {
        const uint8_t sample_key[aes128::key_bytes] = {
            // 2b7e151628aed2a6abf7158809cf4f3c
            0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
            0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
        };
        sample_key_.resize(size(sample_key));
        copy(begin(sample_key), end(sample_key), begin(sample_key_));
    }
    void set_random_key()
    {
        random_key_.resize(aes128::key_bytes);
        init(random_key_);
    }
    static vector<uint8_t> plaintexts_;
    static void set_plaintexts()
    {
        const uint8_t sample_plaintexts[] = {
            // 6bc1bee22e409f96e93d7e117393172a
            0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e,
            0x11, 0x73, 0x93, 0x17, 0x2a,
            // ae2d8a571e03ac9c9eb76fac45af8e51
            0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f,
            0xac, 0x45, 0xaf, 0x8e, 0x51,
            // 30c81c46a35ce411e5fbc1191a0a52ef
            0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1,
            0x19, 0x1a, 0x0a, 0x52, 0xef,
            // f69f2445df4f9b17ad2b417be66c3710
            0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41,
            0x7b, 0xe6, 0x6c, 0x37, 0x10,
            //
        };
        plaintexts_.resize(size(sample_plaintexts));
        copy(begin(sample_plaintexts), end(sample_plaintexts),
             begin(plaintexts_));
    }
    static vector<uint8_t> ciphertexts_;
    static void set_ciphertexts()
    {
        const uint8_t sample_ciphertexts[] = {
            // 3ad77bb40d7a3660a89ecaf32466ef97
            0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60, 0xa8, 0x9e, 0xca,
            0xf3, 0x24, 0x66, 0xef, 0x97,
            // f5d3d58503b9699de785895a96fdbaaf
            0xf5, 0xd3, 0xd5, 0x85, 0x03, 0xb9, 0x69, 0x9d, 0xe7, 0x85, 0x89,
            0x5a, 0x96, 0xfd, 0xba, 0xaf,
            // 43b1cd7f598ece23881b00e3ed030688
            0x43, 0xb1, 0xcd, 0x7f, 0x59, 0x8e, 0xce, 0x23, 0x88, 0x1b, 0x00,
            0xe3, 0xed, 0x03, 0x06, 0x88,
            // 7b0c785e27e8ad3f8223207104725dd4
            0x7b, 0x0c, 0x78, 0x5e, 0x27, 0xe8, 0xad, 0x3f, 0x82, 0x23, 0x20,
            0x71, 0x04, 0x72, 0x5d, 0xd4,
            //
        };
        ciphertexts_.resize(size(sample_ciphertexts));
        copy(begin(sample_ciphertexts), end(sample_ciphertexts),
             begin(ciphertexts_));
    }
    static void SetUpTestCase()
    {
        // Set test vectors.
        set_sample_key();
        set_plaintexts();
        set_ciphertexts();
        ASSERT_EQ(size(sample_key_) % aes128::block_bytes, 0);
        ASSERT_EQ(size(plaintexts_), size(ciphertexts_));
        ASSERT_EQ(size(plaintexts_) % aes128::block_bytes, 0);
        EXPECT_TRUE(check_random_bytes(sample_key_))
            << "Statistical check failed, but not fatal.";
        EXPECT_TRUE(check_random_bytes(ciphertexts_))
            << "Statistical check failed, but not fatal.";
    }
    void SetUp()
    {
        set_random_key();
        EXPECT_TRUE(check_random_bytes(random_key_))
            << "Statistical check failed, but not fatal.";
    }
};

vector<uint8_t> BasicTest::sample_key_;
vector<uint8_t> BasicTest::plaintexts_;
vector<uint8_t> BasicTest::ciphertexts_;

TEST_F(BasicTest, simple_use_aes_ni_with_sample_key_and_texts)
{
    AES128 cipher(sample_key_.data());
    const auto num_blocks = size(plaintexts_) / aes128::block_bytes;
    vector<uint8_t> out(size(plaintexts_));
    cipher.enc(out.data(), plaintexts_.data(), num_blocks);
    ASSERT_EQ(out, ciphertexts_);
    cipher.dec(out.data(), out.data(), num_blocks);
    ASSERT_EQ(out, plaintexts_);
}

TEST_F(BasicTest, simple_use_aes_ni)
{
    AES128 cipher(random_key_.data());
    const auto num_blocks = 1 << 10;
    const auto num_bytes = num_blocks * aes128::block_bytes;
    vector<uint8_t> pt(num_bytes), ct(num_bytes), dec_ct(num_bytes);
    init(pt);
    cipher.enc(ct.data(), pt.data(), num_blocks);
    cipher.dec(dec_ct.data(), ct.data(), num_blocks);
    ASSERT_EQ(pt, dec_ct);
    EXPECT_TRUE(check_random_bytes(ct))
        << "Statistical check failed, but not fatal.";
}

TEST_F(BasicTest, mmo_with_sample_key_and_texts)
{
    MMO128 crh(sample_key_.data());
    const auto num_blocks = size(plaintexts_) / clt::aes128::block_bytes;
    vector<uint8_t> out(size(plaintexts_));
    crh(out.data(), plaintexts_.data(), num_blocks);
    vector<uint8_t> expected_out(size(plaintexts_));
    for (size_t i = 0; i < size(expected_out); i++) {
        expected_out[i] = plaintexts_[i] ^ ciphertexts_[i];
    }
    ASSERT_EQ(out, expected_out);
    EXPECT_TRUE(check_random_bytes(out))
        << "Statistical check failed, but not fatal.";
}

TEST_F(BasicTest, aes_ctr_stream)
{
    AES128 cipher(random_key_.data());
    vector<uint64_t> buff, enc_buff, dec_buff, str_buff;
    constexpr size_t num_blocks = 1 << 10;
    static_assert((aes128::block_bytes % sizeof(uint64_t)) == 0);
    buff.resize(num_blocks * aes128::block_bytes / sizeof(uint64_t));
    enc_buff.resize(buff.size());
    dec_buff.resize(buff.size());
    str_buff.resize(buff.size());
    init_iota(buff, num_blocks, 2);
    cipher.enc(enc_buff.data(), buff.data(), num_blocks);
    const auto counter = cipher.ctr_stream(str_buff.data(), num_blocks, 0);
    ASSERT_EQ(str_buff, enc_buff);
    ASSERT_EQ(counter, num_blocks);
    EXPECT_TRUE(check_random_bytes(enc_buff))
        << "Statistical check failed, but not fatal.";
    EXPECT_TRUE(check_random_bytes(str_buff))
        << "Statistical check failed, but not fatal.";
}

TEST_F(BasicTest, aesprf_ctr_stream)
{
    AESPRF128 prf(random_key_.data());
    vector<uint64_t> buff, prf_buff, str_buff;
    constexpr size_t num_blocks = 1 << 10;
    static_assert((aes128::block_bytes % sizeof(uint64_t)) == 0);
    buff.resize(num_blocks * aes128::block_bytes / sizeof(uint64_t));
    prf_buff.resize(buff.size());
    str_buff.resize(buff.size());
    init_iota(buff, num_blocks, 2);
    prf(prf_buff.data(), buff.data(), num_blocks);
    const auto counter = prf.ctr_stream(str_buff.data(), num_blocks, 0);
    ASSERT_EQ(prf_buff, str_buff);
    ASSERT_EQ(counter, num_blocks);
    EXPECT_TRUE(check_random_bytes(str_buff))
        << "Statistical check failed, but not fatal.";
}

TEST_F(BasicTest, shuffle_FY)
{
    vector<uint32_t> perm(1 << 10);
    iota(begin(perm), end(perm), 0);
    shuffle(perm, rng_global);
    for (size_t i = 0; i < size(perm); i++) {
        const auto at_v = find(begin(perm), end(perm), i);
        ASSERT_NE(at_v, end(perm));
    }
    const auto inv_perm = inverse_permutation(perm);
    vector<uint64_t> buff(size(perm));
    iota(begin(buff), end(buff), 100);
    const auto perm_buff = apply_permutation(buff, perm);
    const auto inv_perm_buff = apply_permutation(perm_buff, inv_perm);
    ASSERT_EQ(buff, inv_perm_buff);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
// vim: set expandtab :
