find_package(GTest REQUIRED)

function(add_unit_test testname)
    add_executable(${testname} ${testname}.cpp)
    target_link_libraries(${testname}
        fmt::fmt-header-only
        spdlog::spdlog_header_only
        GTest::GTest
        aes-ni
        randen
        -lgmpxx
        -lgmp
        $<$<BOOL:${OpenMP_CXX_FOUND}>:-lomp>
    )
    add_test(NAME ${testname}
        COMMAND $<TARGET_FILE:${testname}>
    )
endfunction()

add_unit_test(test_aes-ni)
add_unit_test(test_util)
add_unit_test(test_shuffle)
