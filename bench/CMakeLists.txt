add_custom_target(benchmarks)
file(GLOB bench_srcs RELATIVE "${aes-ni_SOURCE_DIR}/bench" "bench_*.cpp")
message("Found benchmark source files = ${bench_srcs}")
foreach(filename IN ITEMS ${bench_srcs})
    get_filename_component(basename "${filename}" NAME_WE)
    add_executable("${basename}" "${basename}.cpp")
    target_link_libraries("${basename}"
        aes-ni
        fmt::fmt
        # spdlog::spdlog_header_only
        GMP::GMPXX
        $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>)
    add_dependencies(benchmarks "${basename}")
endforeach()

target_link_libraries(bench_randen randen)

add_custom_target(run_benchmarks
    cp -f "${CMAKE_CURRENT_SOURCE_DIR}/run_benchmarks.sh" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
    COMMAND cp -f "${CMAKE_CURRENT_SOURCE_DIR}/benchmark.plot" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
    COMMAND cp -f "${CMAKE_CURRENT_SOURCE_DIR}/run_plotting.sh" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
    DEPENDS benchmarks
)
