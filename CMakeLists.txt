cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

if(CMAKE_HOST_APPLE)
  message("This is macOS platform, therefore clang and clang++ are used.")
  set(CMAKE_C_COMPILER "clang")
  set(CMAKE_CXX_COMPILER "clang++")
endif()

enable_language(CXX)
project(aes-ni VERSION 0.1.0)

message("CMAKE_C_COMPILER = ${CMAKE_C_COMPILER}")
message("CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -Ofast -g -fsanitize=address")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -Os -s -DNDEBUG")

include("${CMAKE_SOURCE_DIR}/search_module.cmake")

add_subdirectory("${CMAKE_SOURCE_DIR}/src")
add_subdirectory("${CMAKE_SOURCE_DIR}/bench")
enable_testing()
add_subdirectory("${CMAKE_SOURCE_DIR}/test")
