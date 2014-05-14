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


# include_directories(${INCLUDE_DIRECTORIES} ${OPENCV_INCLUDE_DIR})

# find_package(OpenCV REQUIRED)

# set(Boost_USE_STATIC_LIBS ON)
# set(Boost_USE_MULTITHREADED ON)
# set(Boost_USE_STATIC_RUNTIME OFF)
# find_package(Boost 1.55.0 COMPONENTS program_options filesystem regex)
# find_package(GFlags REQUIRED)
# find_package(GLog REQUIRED)
# find_package(Gearman REQUIRED)
# find_package(JsonCpp REQUIRED)
find_package(PCRE REQUIRED)
find_package(Judy REQUIRED)

set(LIBS ${LIBS} ${PCRE_LIBRARIES} ${Judy_LIBRARIES} r3)

# find_package(opencv)

# include_directories(${INCLUDE_DIRECTORIES} ${opencv_INCLUDE_DIRS})
# link_directories(${LINK_DIRECTORIES} ${opencv_LIBRARY_DIRS})

# set (CMAKE_CXX_FLAGS "-std=c++0x -arch x86_64 -stdlib=libc++ -g3 -Wall -O0")
enable_testing()
# set (CMAKE_CXX_FLAGS "-std=c++0x -arch x86_64 -stdlib=libc++ -g3 -Wall -O0")

add_subdirectory(src)
add_subdirectory(tests)

add_test(test_tree ${CMAKE_CURRENT_BINARY_DIR}/test_tree)

add_executable(demo demo.c)
# target_link_libraries(alphablue-gui ${OpenCV_LIBS} ${GFlags_LIBS} ${GLog_LIBS} ${Boost_LIBS})
# install(TARGETS alphablue-gui DESTINATION bin)

# add_executable(alphablue-worker worker.cc cv.cc)
# target_link_libraries(alphablue-worker ${OpenCV_LIBS} ${GFlags_LIBS} ${GLog_LIBS} ${Boost_LIBRARIES} ${Gearman_LIBS} ${JsonCpp_LIBS})
# install(TARGETS alphablue-worker DESTINATION bin)

# add_executable(wc_worker wc_worker.cc)
# target_link_libraries(wc_worker ${OpenCV_LIBS} ${GFlags_LIBS} ${GLog_LIBS} ${Boost_LIBRARIES} ${Gearman_LIBS})
# install(TARGETS wc_worker DESTINATION bin)
