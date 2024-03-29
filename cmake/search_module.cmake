
include("${CMAKE_SOURCE_DIR}/cmake/search_gmp.cmake")

find_package(Threads REQUIRED)

include("${CMAKE_SOURCE_DIR}/cmake/search_openmp.cmake")

find_package(fmt REQUIRED
  HINTS "${CMAKE_SOURCE_DIR}/third_party/module/lib/cmake")

# find_package(spdlog REQUIRED
#   HINTS "${CMAKE_SOURCE_DIR}/third_party/module/lib/cmake")

set(BOOST_ROOT_MacPorts "/opt/local/libexec/boost/1.77")
if(EXISTS ${BOOST_ROOT_MacPorts})
  set(BOOST_ROOT ${BOOST_ROOT_MacPorts})
endif()
find_package(Boost REQUIRED)
