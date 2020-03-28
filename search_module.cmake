find_package(OpenMP)
if(OpenMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

find_package(GTest REQUIRED)

if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/third_party/fmt")
  add_subdirectory("${CMAKE_SOURCE_DIR}/third_party/fmt")
else()
  find_package(fmt REQUIRED)
endif()
