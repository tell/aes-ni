#include <array>

#include <clt/util.hpp>
#include <clt/util_omp.hpp>

#include <gtest/gtest.h>

using namespace std;
using namespace clt;

class CLTUtilTest : public ::testing::Test {
};

TEST_F(CLTUtilTest, join_uint8_t)
{
    {
        const uint8_t zero = 0;
        const auto zerostr = join(&zero, 1);
        ASSERT_EQ(zerostr, "00");
    }
    {
        const uint8_t eleven = 11;
        const auto elevenstr = join(&eleven, 1);
        ASSERT_EQ(elevenstr, "0b");
    }
    {
        const uint8_t none = -1;
        const auto str = join(&none, 1);
        ASSERT_EQ(str, "ff");
    }
    {
        const array<uint8_t, 4> ary{0, 1, 2, static_cast<uint8_t>(-1)};
        const auto str = join(ary, ":");
        ASSERT_EQ(str, "00:01:02:ff");
    }
}

TEST_F(CLTUtilTest, join_uint64_t)
{
    {
        const uint64_t zero = 0;
        const auto zerostr = join(&zero, 1);
        ASSERT_EQ(zerostr, "0000000000000000");
    }
    {
        const array<uint8_t, 8> ary{0, 1, 2, 3, 4, 5, 6, 7};
        uint64_t x;
        x = *reinterpret_cast<const uint64_t *>(ary.data());
        const auto str = join(&x, 1);
        ASSERT_EQ(str, "0706050403020100");
    }
}

TEST_F(CLTUtilTest, omp_diagnosis) { print_omp_diagnosis(); }

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
