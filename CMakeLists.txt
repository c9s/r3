cmake_minimum_required(VERSION 3.0)
project(r3 VERSION 2.0.0)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)
set(CMAKE_C_STANDARD 99)

find_package(Check)
find_package(PCRE REQUIRED)

include(CheckSymbolExists)
check_symbol_exists(strdup string.h HAVE_STRDUP)
check_symbol_exists(strndup string.h HAVE_STRNDUP)
configure_file(config.h.cmake config.h)

add_subdirectory(src)

install(
  FILES
    include/memory.h
    include/r3.h
    include/r3_list.h
    include/r3_slug.h
    include/r3_gvc.h
    include/r3_json.h
    include/str_array.h
    include/r3.hpp
  DESTINATION include)

# Configure substitutions for r3.pc. The variables set here must match the
# @<values>@ in r3.pc.in.
set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix ${prefix})
set(includedir ${prefix}/include)
set(libdir ${prefix}/lib)
set(PACKAGE_VERSION ${PROJECT_VERSION})
configure_file(r3.pc.in r3.pc @ONLY)
install(
  FILES
    ${PROJECT_BINARY_DIR}/r3.pc
  DESTINATION lib/pkgconfig)

if(CHECK_FOUND)
  enable_testing()
  add_subdirectory(tests)
else()
  message(STATUS "Skipping unit tests, Check library not found!")
endif()
