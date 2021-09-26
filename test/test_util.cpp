#include <cmath>
#include <array>

#include <clt/util.hpp>
#include <clt/util_omp.hpp>
#include <clt/benchmark2.hpp>

#include <gtest/gtest.h>

using namespace std;
using namespace clt;

class CLTUtilTest : public ::testing::Test {
};

TEST_F(CLTUtilTest, join_uint8_t)
{
    {
        const uint8_t zero = 0;
        const auto zerostr = fmt::format("{:>02x}", zero);
        ASSERT_EQ(zerostr, "00");
    }
    {
        const uint8_t eleven = 11;
        const auto elevenstr = fmt::format("{:>02x}", eleven);
        ASSERT_EQ(elevenstr, "0b");
    }
    {
        const uint8_t none = -1;
        const auto str = fmt::format("{:>02x}", none);
        ASSERT_EQ(str, "ff");
    }
    {
        const array<uint8_t, 4> ary{0, 1, 2, static_cast<uint8_t>(-1)};
        const auto str = fmt::format("{:>02x}", fmt::join(ary, ":"));
        ASSERT_EQ(str, "00:01:02:ff");
    }
}

TEST_F(CLTUtilTest, join_uint64_t)
{
    {
        const uint64_t zero = 0;
        const auto zerostr = fmt::format("{:>016x}", zero);
        ASSERT_EQ(zerostr, "0000000000000000");
    }
    {
        const array<uint8_t, 8> ary{0, 1, 2, 3, 4, 5, 6, 7};
        uint64_t x;
        x = *reinterpret_cast<const uint64_t *>(ary.data());
        const auto str = fmt::format("{:>016x}", x);
        ASSERT_EQ(str, "0706050403020100");
    }
}

TEST_F(CLTUtilTest, omp_diagnosis) { print_omp_diagnosis(); }

TEST_F(CLTUtilTest, benchmark2)
{
    using exp = clt::bench::Experiment;

    exp t;
    {
        fmt::print("0: start.\n");
        const auto results = t.run();
        fmt::print("0: finish.\n");
        ASSERT_LE(results.size(), t.max_total_number_of_execution());
        fmt::print("0: size = {}\n", results.size());
        const auto [smean, uvar, smedian, smad, siqr] =
            clt::bench::calc_stats(results);
        fmt::print("0:    sample mean = {:e}\n", smean);
        fmt::print("0: unbiased stdev = {:e}\n", sqrt(uvar));
        const auto [alpha, bound] =
            clt::bench::confidence_interval_bound(uvar, results.size());
        fmt::print("0: CI bound {:2.0f}% = +/-{:e}\n", 100 * alpha, bound);
        fmt::print("0:         median = {:e}\n", smedian);
        fmt::print("0:            mad = {:e}\n", smad);
        fmt::print("0:            iqr = {:e}\n", siqr);
    }
    {
        t.limit_time_ = chrono::seconds(2);
        uint32_t x;
        t.setup_func_ = [&x]() { x = 0; };
        t.target_func_ = [&x]() {
            this_thread::sleep_for(chrono::milliseconds(500));
            x++;
        };
        t.check_func_ = [&x, &t]() { return x <= t.num_reptition_; };
        fmt::print("1: start.\n");
        const auto results = t.run();
        fmt::print("1: finish.\n");
        ASSERT_LE(results.size(), t.max_total_number_of_execution());
        EXPECT_LT(results.size(), t.max_total_number_of_execution());
        fmt::print("1: size = {}\n", results.size());
        const auto [smean, uvar, smedian, smad, siqr] =
            clt::bench::calc_stats(results);
        fmt::print("1:    sample mean = {:e}\n", smean);
        fmt::print("1: unbiased stdev = {:e}\n", sqrt(uvar));
        const auto [alpha, bound] =
            clt::bench::confidence_interval_bound(uvar, results.size());
        fmt::print("1: CI bound {:2.0f}% = +/-{:e}\n", 100 * alpha, bound);
        fmt::print("1:         median = {:e}\n", smedian);
        fmt::print("1:            mad = {:e}\n", smad);
        fmt::print("1:            iqr = {:e}\n", siqr);
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
