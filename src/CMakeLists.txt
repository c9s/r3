include_directories("${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/3rdparty ${PROJECT_SOURCE_DIR}")
# install(TARGETS swiftnav-static DESTINATION lib${LIB_SUFFIX})

set(libr3_SRCS node.c edge.c list.c slug.c str.c token.c)
# add_library(r3 STATIC ${libr3_SRCS})
add_library(r3 STATIC ${libr3_SRCS})
target_link_libraries(r3 lib3rdparty)
# install(FILES ${libswiftnav_HEADERS} DESTINATION include/libswiftnav)
