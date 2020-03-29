#include <clt/aes-ni.hpp>
#include <clt/statistics.hpp>
#include <clt/benchmark.hpp>

using namespace std;
using namespace clt;
using namespace clt::rng;
using namespace clt::bench;

inline void do_rng_iteration()
{
    size_t current = start_byte_size;
    vector<uint8_t> buff;
    buff.reserve(stop_byte_size);
    while (current <= stop_byte_size) {
        buff.resize(current);
        print_throughput("/dev/urandom", buff.size(), [&]() {
            clt::rng::rng_global(buff.data(), size(buff));
        });
        if (check_random_bytes(buff)) {
            fmt::print(cerr, "WARNING: Outside statistics.\n");
        }
        current <<= 1;
    }
}

int main()
{
    print_diagnosis();
    do_rng_iteration();
    return 0;
}
