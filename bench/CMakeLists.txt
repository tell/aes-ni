add_custom_target(benchmarks)
foreach(basename
        bench_shuffle_fy
        bench_rng
        bench_aes_encdec
        bench_aes_ctr
        bench_aesmmo
        bench_aesprf
        bench_aesprf_ctr
    )
    add_executable("${basename}" "${basename}.cpp")
    target_link_libraries("${basename}" aes-ni fmt::fmt-header-only)
    add_dependencies(benchmarks "${basename}")
endforeach()

add_custom_target(run_benchmarks
    bash "${CMAKE_SOURCE_DIR}/bench/run_benchmarks.sh"
    DEPENDS benchmarks
)
