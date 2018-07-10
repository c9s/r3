function(add_r3_test NAME)
  add_executable(${NAME} ${ARGN})

  target_include_directories(${NAME}
    PRIVATE
      ${CHECK_INCLUDE_DIRS}
      ${PROJECT_BINARY_DIR}
      ${PROJECT_SOURCE_DIR}/src)

  target_link_libraries(${NAME}
    ${CHECK_LDFLAGS}
    r3)

  add_test(NAME ${NAME} COMMAND ${NAME})
endfunction()

add_r3_test(check_tree check_tree.c)
add_r3_test(check_slug check_slug.c)
add_r3_test(check_routes check_routes.c)
add_r3_test(check_str_array check_str_array.c)

add_executable(bench bench.c)
target_link_libraries(bench r3)
