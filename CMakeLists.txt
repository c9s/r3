# cmake file examples
# https://code.google.com/p/opencv-feature-tracker/source/browse/CMakeLists.txt?r=f804b03e704147e65183c19a50f57abedb22f45c


# TODO:
# cmake clean... orz
# http://stackoverflow.com/questions/9680420/looking-for-a-cmake-clean-command-to-clear-up-cmake-output

# load required modules
include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckTypeSize)


cmake_minimum_required(VERSION 3.0)
project(r3 VERSION 2.0.0)

SET(CMAKE_MODULE_PATH
    ${PROJECT_SOURCE_DIR}/cmake/Modules
    ${CMAKE_MODULE_PATH}
)

# check for availability of certain headers
CHECK_INCLUDE_FILE(dlfcn.h              HAVE_DLFCN_H)
CHECK_INCLUDE_FILE(inttypes.h           HAVE_INTTYPES_H)
CHECK_INCLUDE_FILE(jemalloc/jemalloc.h  HAVE_JEMALLOC_JEMALLOC_H)
CHECK_INCLUDE_FILE(memory.h             HAVE_MEMORY_H)
CHECK_INCLUDE_FILE(stdlib.h             HAVE_STDLIB_H)
CHECK_INCLUDE_FILE(string.h             HAVE_STRING_H)
CHECK_INCLUDE_FILE(stdbool.h            HAVE_STDBOOL_H)
CHECK_INCLUDE_FILE(strings.h            HAVE_STRINGS_H)
CHECK_INCLUDE_FILE(sys/stat.h           HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILE(sys/time.h           HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILE(unistd.h             HAVE_UNISTD_H)

# check availability of certain system functions
CHECK_SYMBOL_EXISTS("gettimeofday"  "sys/time.h"    HAVE_GETTIMEOFDAY)
CHECK_SYMBOL_EXISTS("memset"        "string.h"      HAVE_MEMSET)
CHECK_SYMBOL_EXISTS("strchr"        "string.h"      HAVE_STRCHR)
CHECK_SYMBOL_EXISTS("strdup"        "string.h"      HAVE_STRDUP)
CHECK_SYMBOL_EXISTS("strndup"       "string.h"      HAVE_STRNDUP)
CHECK_SYMBOL_EXISTS("strnstr"       "string.h"      HAVE_STRNSTR)
CHECK_SYMBOL_EXISTS("strstr"        "string.h"      HAVE_STRSTR)

# check availability of certain types
CHECK_TYPE_SIZE(_Bool   _BOOL)

# generate the configuration file
configure_file("${PROJECT_SOURCE_DIR}/config.h.cmake" "${PROJECT_BINARY_DIR}/config.h")

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/3rdparty ${PROJECT_BINARY_DIR})

# link_directories(${LINK_DIRECTORIES} /opt/local/lib)
find_package(PCRE REQUIRED)
# find_package(Judy REQUIRED)

# set(LIBS ${LIBS} ${PCRE_LIBRARIES} ${Judy_LIBRARIES} r3)
set(LIBS ${LIBS} ${PCRE_LIBRARIES} r3)

# set (CMAKE_CXX_FLAGS "-std=c++0x -arch x86_64 -stdlib=libc++ -g3 -Wall -O0")
enable_testing()

add_subdirectory(3rdparty)
add_subdirectory(src)
add_subdirectory(tests)
# add_test(test_tree ${CMAKE_CURRENT_BINARY_DIR}/check_tree)
