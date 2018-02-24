include_directories("${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/3rdparty ${PROJECT_SOURCE_DIR}")
set(SOURCES node.c edge.c str.c token.c match_entry.c slug.c memory.c)
set(LIBS ${LIBS} ${PCRE_LIBRARIES})

add_library(r3 STATIC ${SOURCES})

install(TARGETS r3 DESTINATION lib)
# install(FILES ${libswiftnav_HEADERS} DESTINATION include/libswiftnav)
