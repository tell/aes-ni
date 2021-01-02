cmake_minimum_required(VERSION 3.12.0)

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
add_library(randen "randen/randen.cc")
target_include_directories(randen PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/randen")
