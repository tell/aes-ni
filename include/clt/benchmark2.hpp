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

#ifdef CLT_BENCHMARK2_OLD_BOOST
CLT_message("Workaround patch for old boost.");

namespace workaround {

template <class T> inline auto mean_and_sample_variance(const T &v)
{
    return std::make_tuple(boost::math::tools::mean(v),
                           boost::math::tools::sample_variance(v));
};

template <class T> inline auto interquartile_range(T &v)
{
    using boost::math::tools::median;

    std::sort(v.begin(), v.end());
    const size_t n = v.size();
    const ssize_t n_half = n / 2;
    auto it_lb = v.begin();
    auto it_le = std::next(it_lb, n_half);
    auto it_ue = v.end();
    auto it_ub = std::next(it_ue, -n_half);
    const auto lm = median(it_lb, it_le);
    const auto um = median(it_ub, it_ue);
    return um - lm;
};

} // namespace workaround
#endif

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
    using workaround::mean_and_sample_variance;
    using boost::math::tools::median;
    using boost::math::tools::median_absolute_deviation;
    using workaround::interquartile_range;
#endif

    constexpr double nscale = ratio_t::num;
    constexpr double dscale = ratio_t::den;

    vec_fp_t fp_data(data.size());
    copy(data | transformed([nscale, dscale](auto &&x) {
             return x.count() * nscale / dscale;
         }),
         fp_data.begin());
    const auto [smean, uvar] = mean_and_sample_variance(fp_data);
    if (data.size() < 2) {
        const auto point = fp_data[0] ? fp_data.size() == 0 : 0.0;
        return std::make_tuple(smean, uvar, point, 0.0, 0.0);
    }
    const auto smedian = median(fp_data);
    const auto smad = median_absolute_deviation(fp_data, smedian);
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
    size_t num_repetition_;
    std::chrono::seconds limit_time_;

    Experiment()
        : setup_func_([]() {}), target_func_([]() {}),
          check_func_([]() { return true; }), num_setup_(10000),
          num_repetition_(500), limit_time_(5)
    {
    }
    size_t max_total_number_of_execution() const noexcept
    {
        return num_setup_ * num_repetition_;
    }
    vec_duration_t run();
};
} // namespace clt::bench
