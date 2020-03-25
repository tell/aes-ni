add_executable(bench_aes_enc bench_aes_enc.cpp)
target_link_libraries(bench_aes_enc aes-ni fmt::fmt-header-only)
