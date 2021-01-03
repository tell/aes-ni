find_package(GTest REQUIRED)

function(add_unit_test testname)
    add_executable(${testname} ${testname}.cpp)
    target_link_libraries(${testname}
        fmt::fmt
        spdlog::spdlog_header_only
        GTest::GTest
        aes-ni
        randen
        GMP::GMPXX
        $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>
    )
    add_test(NAME ${testname}
        COMMAND $<TARGET_FILE:${testname}>
    )
endfunction()

add_unit_test(test_aes-ni)
add_unit_test(test_util)
add_unit_test(test_shuffle)
