#include <spdlog/spdlog.h>

#include <clt/aes-ni.hpp>
#include <clt/rdrand.hpp>
#include <clt/statistics.hpp>
#include <clt/benchmark.hpp>

using namespace std;
using namespace clt;
using namespace clt::rng;
using namespace clt::bench;

inline void do_rdrand_iteration()
{
#if defined(__RDRND__) || defined(__APPLE__)
    assert((start_byte_size % sizeof(generic_rdrand_t)) == 0);
    assert((stop_byte_size % sizeof(generic_rdrand_t)) == 0);
    size_t current = start_byte_size;
    vector<uint8_t> buff;
    buff.reserve(stop_byte_size);
    while (current <= stop_byte_size) {
        buff.resize(current);
        print_throughput("rdrand", buff.size(), [&]() {
            rdrand(buff.data(), buff.size() / sizeof(generic_rdrand_t));
        });
        if (!check_random_bytes(buff)) {
            spdlog::warn("Outside statistics.");
        }
        current <<= 1;
    }
#endif
}

int main()
{
    print_diagnosis();
    do_rdrand_iteration();
    return 0;
}
