include(FindPackageHandleStandardArgs)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/search_gmp.cmake")

find_package(Threads REQUIRED)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/search_openmp.cmake")

if(NOT (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/third_party"))
  message(WARNING "Exec mkdir ${CMAKE_CURRENT_SOURCE_DIR}/third_party")
  make_directory("${CMAKE_CURRENT_SOURCE_DIR}/third_party")
endif()

find_package(fmt REQUIRED
  HINTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/module/lib/cmake")

find_package(spdlog REQUIRED
  HINTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/module/lib/cmake")

if(ROOT_PROJECT)
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
endif()
