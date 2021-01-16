#pragma once

#include <iostream>
#include <thread>

#ifdef _OPENMP
#include <omp.h>
#endif

#include <fmt/format.h>
#include <fmt/ostream.h>

namespace clt {
void print_omp_diagnosis()
{
    const auto ncpu = std::thread::hardware_concurrency();
    fmt::print(std::cerr, "# std::thread::hardware_concurrency() = {}\n", ncpu);
#ifdef _OPENMP
    const auto omp_max_threads = omp_get_max_threads();
    fmt::print(std::cerr, "# omp_get_max_threads() = {} (OMP_NUM_THREADS)\n",
               omp_max_threads);
    fmt::print(std::cerr,
               "# NOTE: env. var. OMP_NUM_THREADS should be set manually.\n");
#else
    fmt::print(std::cerr, "OpenMP is disabled.\n");
#endif
}
} // namespace clt