include(FindPackageHandleStandardArgs)

find_path(GMP_INCLUDE_DIR
  NAMES gmp.h
  PATHS ENV GMP_PREFIX
  )
find_library(GMP_LIBRARY
  NAMES gmp
  HINTS
    ENV GMP_PREFIX
    ${CMAKE_C_IMPLICIT_LINK_DIRECTORIES}
    ${CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES}
  )
find_package_handle_standard_args(GMP
  DEFAULT_MSG
    GMP_LIBRARY
    GMP_INCLUDE_DIR
  )
message("GMP_INCLUDE_DIR = ${GMP_INCLUDE_DIR}")
message("GMP_LIBRARY = ${GMP_LIBRARY}")

if(GMP_FOUND AND (NOT TARGET GMP::GMP))
  add_library(GMP::GMP UNKNOWN IMPORTED)
  set_target_properties(GMP::GMP
    PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "C;CXX"
      IMPORTED_LOCATION "${GMP_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${GMP_INCLUDE_DIR}"
    )
endif()

find_path(GMPXX_INCLUDE_DIR
  NAMES gmpxx.h
  PATHS ENV GMP_PREFIX
  )
find_library(GMPXX_LIBRARY
  NAMES gmpxx
  HINTS
    ENV GMP_PREFIX
    ${CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES}
  )
find_package_handle_standard_args(GMPXX
  DEFAULT_MSG
    GMPXX_LIBRARY
    GMPXX_INCLUDE_DIR
  )
message("GMPXX_INCLUDE_DIR = ${GMPXX_INCLUDE_DIR}")
message("GMPXX_LIBRARY = ${GMPXX_LIBRARY}")

if(GMP_FOUND AND GMPXX_FOUND AND (NOT TARGET GMP::GMPXX))
  add_library(GMP::GMPXX UNKNOWN IMPORTED)
  set_target_properties(GMP::GMPXX
    PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
      IMPORTED_LOCATION "${GMPXX_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${GMPXX_INCLUDE_DIR}"
      INTERFACE_LINK_LIBRARIES GMP::GMP
    )
endif()
