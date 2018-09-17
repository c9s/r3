add_library(r3 STATIC
  edge.c
  match_entry.c
  memory.c
  node.c
  slug.c
  str.c
  token.c)


set(R3_INCLUDE_DIRS "")  

if(WIN32)
  target_compile_definitions(r3
    PRIVATE
    _CRT_SECURE_NO_WARNINGS
  )
  list(APPEND 
    R3_INCLUDE_DIRS     
    ${PROJECT_BINARY_DIR}
    ${PCRE_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/include
  )  
else()
  target_compile_definitions(r3
    PRIVATE
    _GNU_SOURCE
  )
  list(APPEND
    R3_INCLUDE_DIRS     
    ${PROJECT_BINARY_DIR}
    ${PCRE_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/3rdparty
    ${PROJECT_SOURCE_DIR}/include
  )
endif()

target_include_directories(r3
  PUBLIC
    ${R3_INCLUDE_DIRS}
)

target_link_libraries(r3
  PUBLIC
    ${PCRE_LIBRARIES}
)

install(
  TARGETS r3
  DESTINATION lib)
