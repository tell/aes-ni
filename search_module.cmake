include(FindPackageHandleStandardArgs)

find_path(GMP_INCLUDE_DIR
  NAMES gmp.h
  REQUIRED)
find_library(GMP_LIBRARY
  NAMES gmp
  HINTS
    ${CMAKE_C_IMPLICIT_LINK_DIRECTORIES}
    ${CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES}
  REQUIRED)
find_package_handle_standard_args(GMP
  DEFAULT_MSG
  GMP_INCLUDE_DIR GMP_LIBRARY)
message("GMP_INCLUDE_DIR = ${GMP_INCLUDE_DIR}")
message("GMP_LIBRARY = ${GMP_LIBRARY}")
find_path(GMPXX_INCLUDE_DIR
  NAMES gmpxx.h
  REQUIRED)
find_library(GMPXX_LIBRARY
  NAMES gmpxx
  HINTS ${CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES}
  REQUIRED)
find_package_handle_standard_args(GMPXX
  DEFAULT_MSG
  GMPXX_INCLUDE_DIR GMPXX_LIBRARY)
message("GMPXX_INCLUDE_DIR = ${GMPXX_INCLUDE_DIR}")
message("GMPXX_LIBRARY = ${GMPXX_LIBRARY}")

find_package(Threads REQUIRED)

include("${CMAKE_CURRENT_SOURCE_DIR}/search_openmp.cmake")

if(NOT (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/third_party"))
  message(WARNING "Exec mkdir ${CMAKE_CURRENT_SOURCE_DIR}/third_party")
  make_directory("${CMAKE_CURRENT_SOURCE_DIR}/third_party")
endif()

if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/third_party/module/lib/cmake/fmt")
  message("Found a directory fmt in ${CMAKE_CURRENT_SOURCE_DIR}/third_party/module/lib/cmake")
  set(fmt_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/module/lib/cmake/fmt")
endif()
find_package(fmt REQUIRED)

if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/third_party/module/lib/cmake/spdlog")
  message("Found a directory spdlog in ${CMAKE_CURRENT_SOURCE_DIR}/third_party/module/lib/cmake")
  set(spdlog_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/module/lib/cmake/spdlog")
endif()
find_package(spdlog REQUIRED)

#if(NOT (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/third_party/randen"))
#  make_directory("${CMAKE_CURRENT_SOURCE_DIR}/third_party/randen")
#endif()
#foreach(filename IN ITEMS "LICENSE" "vector128.h" "randen.h" "randen.cc")
#  if(NOT (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/randen/${filename}"))
#    message("Download: ${filename}")
#    file(DOWNLOAD "https://raw.githubusercontent.com/google/randen/master/${filename}"
#      "${CMAKE_CURRENT_SOURCE_DIR}/third_party/randen/${filename}"
#      SHOW_PROGRESS TLS_VERIFY on)
#  endif()
#endforeach()
add_library(randen "${CMAKE_CURRENT_SOURCE_DIR}/third_party/randen/randen.cc")
target_include_directories(randen PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/third_party/randen")
