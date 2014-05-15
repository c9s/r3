
include_directories("${PROJECT_SOURCE_DIR}/include")
# install(TARGETS swiftnav-static DESTINATION lib${LIB_SUFFIX})

set(libr3_SRCS node.c str.c list.c token.c)
set(CMAKE_CFLAGS "-Wall -pipe -g3 -funroll-loops")

# add_library(r3-static STATIC ${libr3_SRCS})
add_library(r3 SHARED ${libr3_SRCS})
# target_link_libraries(r3 cblas)
# install(FILES ${libswiftnav_HEADERS} DESTINATION include/libswiftnav)
