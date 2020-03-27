#include <numeric>
#include <clt/aes-ni.hpp>

#include "bench_config.hpp"

using namespace std;
using namespace clt;
using namespace clt::rng;
using namespace clt::bench;

template <class Func>
inline void do_shuffle_ys(const std::string &label, const size_t num,
                          Func &&func)
{
    const std::string format = label + ",{},{:.5e}\n";
    const auto elapsed_time = measure(forward<Func>(func));
    fmt::print(format, num, elapsed_time);
}

inline void do_shuffle_ys_iteration()
{
    size_t current = start_byte_size;
    vector<uint32_t> buff;
    buff.reserve(stop_byte_size);
    fmt::print("mode,num,sec\n");
    while (current <= stop_byte_size) {
        buff.resize(current);
        iota(begin(buff), end(buff), 0);
        do_shuffle_ys("shuffle_ys_dev-urandom", size(buff),
                      [&]() { shuffle(buff.data(), size(buff), rng_global); });
        current <<= 1;
    }
}

int main()
{
    print_diagnosis();
    do_shuffle_ys_iteration();
    return 0;
}
