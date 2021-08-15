#pragma once

#include <cmath>
#include <vector>
#include <chrono>
#include <functional>
#include <algorithm>

#include <boost/version.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/copy.hpp>
#if BOOST_VERSION >= 107200
#include <boost/math/statistics/univariate_statistics.hpp>
#else
#include <boost/math/tools/univariate_statistics.hpp>
#define CLT_BENCHMARK2_OLD_BOOST
#endif
#include <boost/math/distributions/normal.hpp>

#include "util.hpp"

namespace clt::bench {

template <class T> inline auto calc_stats(const T &data)
{
    using vec_fp_t = std::vector<double>;
    using duration_t = typename T::value_type;
    using ratio_t = typename duration_t::period;
    using boost::copy;
    using boost::adaptors::transformed;
#ifndef CLT_BENCHMARK2_OLD_BOOST
    using boost::math::statistics::mean_and_sample_variance;
    using boost::math::statistics::median;
    using boost::math::statistics::median_absolute_deviation;
    using boost::math::statistics::interquartile_range;
#else
    CLT_message("Workaround patch for old boost.");
    const auto mean_and_sample_variance = [](auto &v) {
        return std::make_tuple(boost::math::tools::mean(v),
                               boost::math::tools::sample_variance(v));
    };
    using boost::math::tools::median;
    using boost::math::tools::median_absolute_deviation;
    const auto interquartile_range = [](auto) -> double { return 0; };
#endif

    constexpr double nscale = ratio_t::num;
    constexpr double dscale = ratio_t::den;

    vec_fp_t fp_data(data.size());
    copy(data | transformed([nscale, dscale](auto &&x) {
             return x.count() * nscale / dscale;
         }),
         fp_data.begin());
    const auto [smean, uvar] = mean_and_sample_variance(fp_data);
    const auto smedian = median(fp_data);
    const auto smad = median_absolute_deviation(fp_data);
    const auto siqr = interquartile_range(fp_data);
    return std::make_tuple(smean, uvar, smedian, smad, siqr);
}

/**
 * Output alpha and bound.
 * alpha-percent CI is [smean - bound, smean + bound].
 */
template <class RealType>
inline auto confidence_interval_bound(const RealType uvar,
                                      const size_t nsamples,
                                      const double alpha = 0.95)
{
    using std::sqrt;
    using norm_t = boost::math::normal_distribution<>;
    using boost::math::quantile;

    assert(0 <= alpha);
    assert(alpha <= 1);
    const auto residue = (1 - alpha) / 2;
    const auto ustdev = std::sqrt(uvar / nsamples);
    const auto stdnorm = norm_t{};
    const auto tail = quantile(stdnorm, 1 - residue);
    const auto bound = tail * ustdev;
    return std::make_tuple(alpha, bound);
}

struct Experiment {
    using timer_t = std::chrono::high_resolution_clock;
    using duration_t = timer_t::duration;
    using vec_duration_t = std::vector<duration_t>;

    std::function<void()> setup_func_;
    std::function<void()> target_func_;
    std::function<bool()> check_func_;
    size_t num_setup_;
    size_t num_reptition_;
    std::chrono::seconds limit_time_;

    Experiment()
        : setup_func_([]() {}), target_func_([]() {}),
          check_func_([]() { return true; }), num_setup_(10000),
          num_reptition_(500), limit_time_(5)
    {
    }
    size_t max_total_number_of_execution() const noexcept
    {
        return num_setup_ * num_reptition_;
    }
    vec_duration_t run();
};
} // namespace clt::bench
