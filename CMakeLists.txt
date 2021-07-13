cmake_minimum_required(VERSION 3.20)
project(ssparse)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FindPkgConfig)

# zlib
pkg_check_modules(zlib REQUIRED IMPORTED_TARGET zlib)
  get_target_property(
  ZLIB_INC
  PkgConfig::zlib
  INTERFACE_INCLUDE_DIRECTORIES
)

# tclap
pkg_check_modules(tclap REQUIRED IMPORTED_TARGET tclap)
  get_target_property(
  TCLAP_INC
  PkgConfig::tclap
  INTERFACE_INCLUDE_DIRECTORIES
)

# libprim
pkg_check_modules(libprim REQUIRED IMPORTED_TARGET libprim)
  get_target_property(
  LIBPRIM_INC
  PkgConfig::libprim
  INTERFACE_INCLUDE_DIRECTORIES
)

# libex
pkg_check_modules(libex REQUIRED IMPORTED_TARGET libex)
  get_target_property(
  LIBEX_INC
  PkgConfig::libex
  INTERFACE_INCLUDE_DIRECTORIES
)

# libfio
pkg_check_modules(libfio REQUIRED IMPORTED_TARGET libfio)
  get_target_property(
  LIBFIO_INC
  PkgConfig::libfio
  INTERFACE_INCLUDE_DIRECTORIES
)

# libstrop
pkg_check_modules(libstrop REQUIRED IMPORTED_TARGET libstrop)
  get_target_property(
  LIBSTROP_INC
  PkgConfig::libstrop
  INTERFACE_INCLUDE_DIRECTORIES
)

# libmut
pkg_check_modules(libmut REQUIRED IMPORTED_TARGET libmut)
  get_target_property(
  LIBMUT_INC
  PkgConfig::libmut
  INTERFACE_INCLUDE_DIRECTORIES
)

add_executable(
  ssparse
  ${PROJECT_SOURCE_DIR}/src/main.cc
  ${PROJECT_SOURCE_DIR}/src/parse/util.cc
  ${PROJECT_SOURCE_DIR}/src/parse/Filter.cc
  ${PROJECT_SOURCE_DIR}/src/parse/Engine.cc
  ${PROJECT_SOURCE_DIR}/src/parse/util.h
  ${PROJECT_SOURCE_DIR}/src/parse/Engine.h
  ${PROJECT_SOURCE_DIR}/src/parse/Filter.h
  )

target_include_directories(
  ssparse
  PUBLIC
  ${PROJECT_SOURCE_DIR}/src
  ${ZLIB_INC}
  ${TCLAP_INC}
  ${LIBPRIM_INC}
  ${LIBEX_INC}
  ${LIBFIO_INC}
  ${LIBSTROP_INC}
  ${LIBMUT_INC}
  )

target_link_libraries(
  ssparse
  PkgConfig::zlib
  PkgConfig::tclap
  PkgConfig::libprim
  PkgConfig::libex
  PkgConfig::libfio
  PkgConfig::libstrop
  PkgConfig::libmut
  )

include(GNUInstallDirs)

install(
  TARGETS
  ssparse
  )

