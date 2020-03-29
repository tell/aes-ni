find_package(OpenMP)
if(OpenMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

if(NOT (IS_DIRECTORY "${CMAKE_SOURCE_DIR}/third_party"))
  make_directory("${CMAKE_SOURCE_DIR}/third_party")
endif()

if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/third_party/fmt")
  add_subdirectory("${CMAKE_SOURCE_DIR}/third_party/fmt")
else()
  find_package(fmt REQUIRED)
endif()

#if(NOT (IS_DIRECTORY "${CMAKE_SOURCE_DIR}/third_party/randen"))
#  make_directory("${CMAKE_SOURCE_DIR}/third_party/randen")
#endif()
#foreach(filename IN ITEMS "LICENSE" "vector128.h" "randen.h" "randen.cc")
#  if(NOT (EXISTS "${CMAKE_SOURCE_DIR}/third_party/randen/${filename}"))
#    message("Download: ${filename}")
#    file(DOWNLOAD "https://raw.githubusercontent.com/google/randen/master/${filename}"
#      "${CMAKE_SOURCE_DIR}/third_party/randen/${filename}"
#      SHOW_PROGRESS TLS_VERIFY on)
#  endif()  
#endforeach()
add_library(randen "${CMAKE_SOURCE_DIR}/third_party/randen/randen.cc")
target_include_directories(randen PUBLIC "${CMAKE_SOURCE_DIR}/third_party/randen")
