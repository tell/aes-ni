add_custom_target(benchmarks)
file(GLOB bench_srcs RELATIVE "${CMAKE_SOURCE_DIR}/bench" "bench_*.cpp")
message("Found benchmark source files = ${bench_srcs}")
foreach(filename IN ITEMS ${bench_srcs})
    get_filename_component(basename "${filename}" NAME_WE)
    add_executable("${basename}" "${basename}.cpp")
    target_link_libraries("${basename}" aes-ni fmt::fmt-header-only randen)
    add_dependencies(benchmarks "${basename}")
endforeach()

add_custom_target(run_benchmarks
    bash "${CMAKE_SOURCE_DIR}/bench/run_benchmarks.sh"
    DEPENDS benchmarks
)
