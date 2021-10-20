#include <clt/benchmark2.hpp>

using namespace std;
using namespace std::chrono;

namespace clt::bench {
Experiment::vec_duration_t Experiment::run()
{
    constexpr size_t ratio = 10;
    using limit_time_t = decay_t<decltype(limit_time_)>;

    vec_duration_t results;
    const auto start_time = timer_t::now();
    duration_t cum_etime = {};
    for (size_t i = 0; i < num_setup_; i++) {
        (void)setup_func_();
        for (size_t j = 0; j < num_repetition_; j++) {
            const auto begin_time = timer_t::now();
            (void)target_func_();
            const auto end_time = timer_t::now();
            assert(check_func_());

            const auto elapsed_time =
                duration_cast<duration_t>(end_time - begin_time);
            results.emplace_back(elapsed_time);
            cum_etime += elapsed_time;

            const auto total_elapsed_time = timer_t::now() - start_time;
            if ((limit_time_ < duration_cast<limit_time_t>(cum_etime)) ||
                ((ratio * limit_time_) <
                 duration_cast<limit_time_t>(total_elapsed_time))) {
                return results;
            }
        }
    }
    return results;
}
} // namespace clt::bench
