# cmake file examples
# https://code.google.com/p/opencv-feature-tracker/source/browse/CMakeLists.txt?r=f804b03e704147e65183c19a50f57abedb22f45c


# TODO:
# cmake clean... orz
# http://stackoverflow.com/questions/9680420/looking-for-a-cmake-clean-command-to-clear-up-cmake-output


cmake_minimum_required(VERSION 2.8)
project(r3)

SET(CMAKE_MODULE_PATH
    ${PROJECT_SOURCE_DIR}/cmake_modules
    ${CMAKE_MODULE_PATH}
    )

include_directories(. "${PROJECT_SOURCE_DIR}/include" ${INCLUDE_DIRECTORIES} /opt/local/include)
link_directories(${LINK_DIRECTORIES} /opt/local/lib)
find_package(PCRE REQUIRED)
# find_package(Judy REQUIRED)
find_package(Jemalloc REQUIRED)

set(LIBS ${LIBS} ${PCRE_LIBRARIES} ${Jemalloc_LIBRARIES} r3)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -pipe -g3 -funroll-loops")
# set (CMAKE_CXX_FLAGS "-std=c++0x -arch x86_64 -stdlib=libc++ -g3 -Wall -O0")
enable_testing()

add_subdirectory(src)
add_subdirectory(tests)

install (FILES include/r3.h DESTINATION include)
install (FILES include/r3_define.h DESTINATION include)
install (FILES include/r3_str.h DESTINATION include)

add_test(test_r3 ${CMAKE_CURRENT_BINARY_DIR}/tests/test_r3)

add_executable(demo demo.c)
