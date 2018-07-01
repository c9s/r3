add_library(r3 STATIC
  edge.c
  match_entry.c
  memory.c
  node.c
  slug.c
  str.c
  token.c)

target_compile_definitions(r3
  PRIVATE
    _GNU_SOURCE)

target_include_directories(r3
  PUBLIC
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/3rdparty
    ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(r3
  PUBLIC
    ${PCRE_LIBRARIES})

install(
  TARGETS r3
  DESTINATION lib)
