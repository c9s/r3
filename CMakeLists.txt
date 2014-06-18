# cmake file examples
# https://code.google.com/p/opencv-feature-tracker/source/browse/CMakeLists.txt?r=f804b03e704147e65183c19a50f57abedb22f45c


# TODO:
# cmake clean... orz
# http://stackoverflow.com/questions/9680420/looking-for-a-cmake-clean-command-to-clear-up-cmake-output


cmake_minimum_required(VERSION 2.8)
project(r3)

SET(CMAKE_MODULE_PATH
    ${PROJECT_SOURCE_DIR}/cmake
    ${CMAKE_MODULE_PATH}
    )

include_directories(. ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/3rdparty /opt/local/include)
# include_directories(. ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/3rdparty ${PROJECT_SOURCE_DIR})

link_directories(${LINK_DIRECTORIES} /opt/local/lib)
find_package(PCRE REQUIRED)
find_package(Judy REQUIRED)

set(LIBS ${LIBS} ${PCRE_LIBRARIES} ${Judy_LIBRARIES} r3)

# set (CMAKE_CXX_FLAGS "-std=c++0x -arch x86_64 -stdlib=libc++ -g3 -Wall -O0")
enable_testing()

add_subdirectory(3rdparty)
add_subdirectory(src)
add_subdirectory(tests)
# add_test(test_tree ${CMAKE_CURRENT_BINARY_DIR}/check_tree)
