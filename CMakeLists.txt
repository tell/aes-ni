cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_VERBOSE_MAKEFILE 1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_language(CXX)
# find_package(Threads REQUIRED)

if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/fmt")
  add_subdirectory("${CMAKE_SOURCE_DIR}/fmt")
else()
  find_package(fmt)
endif()

# include(CTest)
# enable_testing()

set(CMAKE_CXX_FLAGS "-march=native -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Ofast -g -fsanitize=address")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -s -DNDEBUG")

project(aes-ni VERSION 0.1.0)
add_library(aes-ni aes-ni.cpp rng.cpp)
target_link_libraries(aes-ni fmt::fmt-header-only)
add_executable(test_aes-ni test_aes-ni.cpp)
target_link_libraries(test_aes-ni aes-ni fmt::fmt-header-only)
add_executable(bench_aes-ni bench_aes-ni.cpp)
target_link_libraries(bench_aes-ni aes-ni fmt::fmt-header-only)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
