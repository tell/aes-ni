#include <vector>
#include <numeric>

#include <gtest/gtest.h>

#include <clt/shuffle.hpp>
#include <clt/aes-ni.hpp>
#include <clt/statistics.hpp>

using namespace std;
using namespace clt;
using namespace clt::rng;

class ShuffleTest : public ::testing::Test {
protected:
    AES128::key_t random_key_;
    void set_random_key() { random_key_ = gen_key(); }
    void SetUp()
    {
        set_random_key();
        fmt::print("random_key_ = {}\n", join(random_key_));
    }
};

TEST_F(ShuffleTest, permutation_rank_identity)
{
    for (size_t i = 1; i < 20; i++) {
        Permutation perm(i);
        mpz_class r = clt::rank(perm.indices_);
        ASSERT_EQ(0, r);
    }
    AESPRF128_CTR prf(random_key_.data());
    for (size_t degree = 1; degree < 10; degree++) {
        Permutation perm(degree);
        for (size_t i = 0; i < 10; i++) {
            perm.shuffle(prf);
            cout << perm << endl;
        }
    }
}

TEST_F(ShuffleTest, permutation_rank)
{
    AESPRF128_CTR prf(random_key_.data());
    const size_t degree = 1 << 10;
    Permutation perm(degree);
    perm.shuffle(prf);

    const auto rank_pi = clt::rank(perm.indices_);
    const auto pi = clt::unrank(rank_pi, perm.indices_.size());
    ASSERT_EQ(pi, perm.indices_);
    const auto rank_pi2 = clt::rank(pi);
    ASSERT_EQ(rank_pi, rank_pi2);
}

TEST_F(ShuffleTest, permutation_composite)
{
    AESPRF128_CTR prf(random_key_.data());
    const size_t degree = 1 << 10;
    Permutation perm0(degree), perm1(degree);
    perm0.shuffle(prf);
    perm1.shuffle(prf);
    Permutation perm_comp0 = perm0 * perm1;
    Permutation perm_comp1 = perm0.apply(perm1);
    ASSERT_EQ(perm_comp0.indices_, perm_comp1.indices_);
}

TEST_F(ShuffleTest, permutation_inverse)
{
    AESPRF128_CTR prf(random_key_.data());
    const size_t degree = 1 << 10;
    Permutation perm(degree);
    perm.shuffle(prf);

    const auto inv_perm = perm.inverse();
    if (0 != clt::rank(perm.indices_)) {
        ASSERT_NE(perm.indices_, inv_perm.indices_);
    }
    const auto id = perm * inv_perm;
    const auto id_rank = clt::rank(id.indices_);
    ASSERT_EQ(0, id_rank);
}

TEST_F(ShuffleTest, shuffle_FY)
{
    AESPRF128_CTR prf(random_key_.data());
    Permutation perm(1 << 10);
    perm.shuffle(prf);
    for (size_t i = 0; i < perm.indices_.size(); i++) {
        const auto at_v = find(perm.indices_.begin(), perm.indices_.end(), i);
        ASSERT_NE(at_v, perm.indices_.end());
    }
    const auto inv_perm = perm.inverse();
    vector<uint64_t> buff(perm.indices_.size());
    iota(begin(buff), end(buff), 100);
    const auto perm_buff = perm.apply(buff);
    const auto inv_perm_buff = inv_perm.apply(perm_buff);
    ASSERT_EQ(buff, inv_perm_buff);
}

TEST_F(ShuffleTest, shuffle_FY_various)
{
    vector<string> buff{"abc", "def", "ghi", "jkl"};
    AESPRF128_CTR prf(random_key_.data());
    Permutation perm(buff.size());
    perm.shuffle(prf);
    const auto inv_perm = perm.inverse();
    const auto perm_buff = perm.apply(buff);
    const auto inv_perm_buff = inv_perm.apply(perm_buff);
    ASSERT_EQ(buff, inv_perm_buff);
}

TEST_F(ShuffleTest, shuffle_FY_statistics)
{
    AESPRF128_CTR prf(random_key_.data());
    const size_t degree = 5;
    Permutation perm(degree);

    const mpz_class perm_space_size = clt::factorial(degree);
    EXPECT_TRUE(perm_space_size.fits_ulong_p())
        << "Given permutation space is too large.";
    vector<uint32_t> counter(perm_space_size.get_ui(), 0);

    const size_t expectation = 2000;
    const mpz_class num_loop = expectation * perm_space_size;
    EXPECT_TRUE(num_loop.fits_ulong_p())
        << "Given permutation space is too large.";
    for (size_t i = 0; i < num_loop; i++) {
        perm.shuffle(prf);
        const auto rank_perm = clt::rank(perm.indices_);
        counter[rank_perm.get_ui()]++;
    }
    EXPECT_TRUE(check_udist_by_chisq(counter, expectation))
        << "Statistical check failed, but not fatal.";
}

TEST_F(ShuffleTest, shuffle_RS)
{
    AESPRF128_CTR prf(random_key_.data());
    vector<uint32_t> perm(1 << 10);
    iota(begin(perm), end(perm), 0);
    shuffle_RS(perm, prf);
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

TEST_F(ShuffleTest, shuffle_RS_statistics)
{
    AESPRF128_CTR prf(random_key_.data());
    const size_t degree = 5;
    vector<uint32_t> perm(degree);
    iota(begin(perm), end(perm), 0);

    const mpz_class perm_space_size = clt::factorial(degree);
    EXPECT_TRUE(perm_space_size.fits_ulong_p())
        << "Given permutation space is too large.";
    vector<uint32_t> counter(perm_space_size.get_ui(), 0);

    const size_t expectation = 1000;
    const mpz_class num_loop = expectation * perm_space_size;
    EXPECT_TRUE(num_loop.fits_ulong_p())
        << "Given permutation space is too large.";
    for (size_t i = 0; i < num_loop; i++) {
        shuffle_RS(perm, prf);
        const auto rank_perm = clt::rank(perm);
        counter[rank_perm.get_ui()]++;
    }
    EXPECT_TRUE(check_udist_by_chisq(counter, expectation))
        << "Statistical check failed, but not fatal.";
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
