#include <clt/benchmark2.hpp>

using namespace std;

namespace clt::bench {
Experiment::vec_duration_t Experiment::run()
{
    using limit_time_t = decay_t<decltype(limit_time_)>;

    vec_duration_t results;
    const auto start_time = timer_t::now();
    for (size_t i = 0; i < num_setup_; i++) {
        (void)setup_func_();
        for (size_t j = 0; j < num_reptition_; j++) {
            const auto begin_time = timer_t::now();
            (void)target_func_();
            const auto end_time = timer_t::now();
            assert(check_func_());

            results.emplace_back(
                duration_cast<duration_t>(end_time - begin_time));
            const auto current_time = timer_t::now();
            if (limit_time_ <
                duration_cast<limit_time_t>(current_time - start_time)) {
                return results;
            }
        }
    }
    return results;
}
} // namespace clt::bench
