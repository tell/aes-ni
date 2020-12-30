if(CMAKE_HOST_APPLE)
  # NOTE: See https://gitlab.kitware.com/cmake/cmake/-/blob/master/Modules/FindOpenMP.cmake
  message("Lookup OpenMP in macOS platform...")
  foreach(LANG IN ITEMS C CXX)
    list(APPEND CMAKE_${LANG}_IMPLICIT_LINK_DIRECTORIES "/usr/local/lib" "/opt/local/lib/libomp")
    message("CMAKE_${LANG}_IMPLICIT_LINK_DIRECTORIES = ${CMAKE_${LANG}_IMPLICIT_LINK_DIRECTORIES}")
    if(IS_DIRECTORY "/opt/local/include/libomp")
      set(OpenMP_${LANG}_INCLUDE_DIR "/opt/local/include/libomp")
    endif()
    set(OpenMP_${LANG}_FLAG "-Xpreprocessor -fopenmp")
  endforeach()
endif()
find_package(OpenMP)
message("OpenMP_libomp_LIBRARY = ${OpenMP_libomp_LIBRARY}")
foreach(LANG IN ITEMS C CXX)
  message("OpenMP_${LANG}_FLAGS = ${OpenMP_${LANG}_FLAGS}")
  message("OpenMP_${LANG}_INCLUDE_DIRS = ${OpenMP_${LANG}_INCLUDE_DIRS}")
  message("OpenMP_${LANG}_LIB_NAMES = ${OpenMP_${LANG}_LIB_NAMES}")
endforeach()
