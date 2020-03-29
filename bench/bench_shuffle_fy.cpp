#include <numeric>

#include <clt/aes-ni.hpp>
#include <clt/benchmark.hpp>

using namespace std;
using namespace clt;
using namespace clt::rng;
using namespace clt::bench;

inline void do_shuffle_ys_iteration()
{
    size_t current = start_byte_size;
    vector<uint32_t> buff;
    buff.reserve(stop_byte_size);
    while (current <= stop_byte_size) {
        buff.resize(current);
        iota(begin(buff), end(buff), 0);
        print_throughput(
            "shuffle_ys_dev-urandom", size(buff),
            [&]() { shuffle(buff.data(), size(buff), rng_global); },
            "32bit_elems");
        current <<= 1;
    }
}

int main()
{
    print_diagnosis();
    do_shuffle_ys_iteration();
    return 0;
}
