cmake_minimum_required(VERSION 2.8)

include_directories("${PROJECT_SOURCE_DIR}/include")
# install(TARGETS swiftnav-static DESTINATION lib${LIB_SUFFIX})

set(R3_SRCS node.c str.c list.c token.c edge.c zmalloc.c)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -pipe -g3 -funroll-loops -O0")

# set(LIBS ${LIBS} ${PCRE_LIBRARIES} ${Judy_LIBRARIES} ${Jemalloc_LIBRARIES} r3)
set(LIBS ${LIBS} ${PCRE_LIBRARIES} ${Jemalloc_LIBRARIES} r3)

# add_library(r3 SHARED ${R3_SRCS})
add_library(r3 STATIC ${R3_SRCS})
target_link_libraries(r3 ${LIBS})
install (TARGETS r3 DESTINATION lib)
# target_link_libraries(r3 cblas)
# install(FILES ${libswiftnav_HEADERS} DESTINATION include/libswiftnav)
