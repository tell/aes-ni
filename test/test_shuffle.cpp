#include <vector>
#include <numeric>

#include <gtest/gtest.h>

#include <clt/shuffle.hpp>
#include <clt/rng.hpp>
#include <clt/statistics.hpp>

using namespace std;
using namespace clt;
using namespace clt::rng;

class ShuffleTest : public ::testing::Test {
};

TEST_F(ShuffleTest, shuffle_FY)
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

TEST_F(ShuffleTest, permutation_rank)
{
    const size_t degree = 1 << 10;
    vector<uint32_t> perm(degree);
    iota(begin(perm), end(perm), 0);
    shuffle(perm, rng_global);

    const auto rank_pi = clt::rank(perm);
    const auto pi = clt::unrank(rank_pi, perm.size());
    ASSERT_EQ(pi, perm);
    const auto rank_pi2 = clt::rank(pi);
    ASSERT_EQ(rank_pi, rank_pi2);
}

TEST_F(ShuffleTest, shuffle_FY_statistics)
{
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
        shuffle(perm, rng_global);
        const auto rank_perm = clt::rank(perm);
        counter[rank_perm.get_ui()]++;
    }
    EXPECT_TRUE(check_udist_by_chisq(counter, expectation))
        << "Statistical check failed, but not fatal.";
}

TEST_F(ShuffleTest, shuffle_RS)
{
    vector<uint32_t> perm(1 << 10);
    iota(begin(perm), end(perm), 0);
    shuffle_RS(perm, rng_global);
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
        shuffle_RS(perm, rng_global);
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
