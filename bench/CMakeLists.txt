add_custom_target(benchmarks)
file(GLOB bench_srcs RELATIVE "${aes-ni_SOURCE_DIR}/bench" "bench_*.cpp")
message("Found benchmark source files = ${bench_srcs}")
foreach(filename IN ITEMS ${bench_srcs})
    get_filename_component(basename "${filename}" NAME_WE)
    add_executable("${basename}" "${basename}.cpp")
    target_link_libraries("${basename}"
        aes-ni
        spdlog::spdlog_header_only
        fmt::fmt-header-only
        $<$<AND:$<BOOL:${OpenMP_FOUND}>,$<BOOL:${CMAKE_HOST_APPLE}>>:-lomp>)
    add_dependencies(benchmarks "${basename}")
endforeach()

target_link_libraries(bench_randen randen)

add_custom_target(run_benchmarks
    ln -s -f "${CMAKE_CURRENT_SOURCE_DIR}/run_benchmarks.sh"
    COMMAND ln -s -f "${CMAKE_CURRENT_SOURCE_DIR}/benchmark.plot"
    COMMAND ln -s -f "${CMAKE_CURRENT_SOURCE_DIR}/run_plotting.sh"
    DEPENDS benchmarks
)
