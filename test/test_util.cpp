#include <array>

#include <clt/util.hpp>

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
        const array<uint8_t, 3> ary{0, 1, 2};
        const auto str = join(ary);
        ASSERT_EQ(str, "00:01:02");
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
