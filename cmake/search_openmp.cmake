if(CMAKE_HOST_APPLE)
  # NOTE: See https://gitlab.kitware.com/cmake/cmake/-/blob/master/Modules/FindOpenMP.cmake
  message("To find OpenMP in macOS platform, the following procedures might change variables: CMAKE_{C,CXX}_IMPLICIT_LINK_DIRECTORIES, OpenMP_{C,CXX}_INCLUDE_DIR, and OpenMP_{C,CXX}_FLAG")
  foreach(lang IN ITEMS C CXX)
    if(CMAKE_${lang}_COMPILER_ID STREQUAL "AppleClang")
      list(APPEND CMAKE_${lang}_IMPLICIT_LINK_DIRECTORIES "/usr/local/lib" "/opt/local/lib/libomp")
      if(IS_DIRECTORY "/opt/local/include/libomp")
        set(OpenMP_${lang}_INCLUDE_DIR "/opt/local/include/libomp")
      endif()
      set(OpenMP_${lang}_FLAG "-Xpreprocessor -fopenmp")
    endif()
    message("CMAKE_${lang}_IMPLICIT_LINK_DIRECTORIES = ${CMAKE_${lang}_IMPLICIT_LINK_DIRECTORIES}")
  endforeach()
endif()

find_package(OpenMP)
message("OpenMP_libomp_LIBRARY = ${OpenMP_libomp_LIBRARY}")
foreach(LANG IN ITEMS C CXX)
  message("OpenMP_${LANG}_FLAGS = ${OpenMP_${LANG}_FLAGS}")
  message("OpenMP_${LANG}_INCLUDE_DIRS = ${OpenMP_${LANG}_INCLUDE_DIRS}")
  message("OpenMP_${LANG}_LIB_NAMES = ${OpenMP_${LANG}_LIB_NAMES}")
endforeach()
