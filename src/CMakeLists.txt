include_directories("${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/3rdparty ${PROJECT_SOURCE_DIR}")
# install(TARGETS swiftnav-static DESTINATION lib${LIB_SUFFIX})
find_package(PCRE REQUIRED)
set(libr3_SRCS node.c edge.c list.c slug.c str.c token.c match_entry.c memory.c)
set(LIBS ${LIBS} ${PCRE_LIBRARIES} r3)

add_library(r3 STATIC ${libr3_SRCS})
target_link_libraries(r3 lib3rdparty pcre)
# install(FILES ${libswiftnav_HEADERS} DESTINATION include/libswiftnav)
