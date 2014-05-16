
include_directories("${PROJECT_SOURCE_DIR}/include")
# install(TARGETS swiftnav-static DESTINATION lib${LIB_SUFFIX})

set(libr3_SRCS node.c str.c list.c token.c)
set(CMAKE_CFLAGS "-Wall -pipe -g3 -funroll-loops")
set(LIBS ${LIBS} ${PCRE_LIBRARIES} ${Judy_LIBRARIES} ${Jemalloc_LIBRARIES} r3)

# add_library(r3-static STATIC ${libr3_SRCS})
add_library(r3 SHARED ${libr3_SRCS})
target_link_libraries(r3 ${LIBS})

# target_link_libraries(r3 cblas)
# install(FILES ${libswiftnav_HEADERS} DESTINATION include/libswiftnav)
