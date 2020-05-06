cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

# Determine this project is root-project or sub-project.
set(ROOT_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(ROOT_PROJECT ON)
  message(STATUS "CMake version: ${CMAKE_VERSION}")
  message(STATUS "This is the root project.")
endif()

enable_language(CXX)
project(aes-ni VERSION 0.1.0)

if (ROOT_PROJECT)
  message(STATUS "Modify compiler flags.")

  message("CMAKE_C_COMPILER = ${CMAKE_C_COMPILER}")
  message("CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -march=native")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -fsanitize=address -fno-omit-frame-pointer -UNDEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Ofast -DNDEBUG")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -Ofast -g -fsanitize=address -UNDEBUG")
  set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -Os -s -DNDEBUG")
endif()

include("${CMAKE_CURRENT_SOURCE_DIR}/search_module.cmake")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src")

if (ROOT_PROJECT)
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/bench")
  enable_testing()
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
else()
  message(STATUS "Skip building benchmarks and tests.")
endif()
