# set(TEST_LIBS ${TEST_LIBS} ${CHECK_LIBRARIES} judy libr3)
# set(TEST_LIBS ${TEST_LIBS} ${CHECK_LIBRARIES} judy libr3)
include_directories("${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}")
find_package(Check REQUIRED)
find_package(PCRE REQUIRED)
# find_package(Judy REQUIRED)

if (NOT CHECK_FOUND)
    message(STATUS "Skipping unit tests, Check library not found!")
else (NOT CHECK_FOUND)
    set(TEST_LIBS ${LIBS} ${CHECK_LIBRARIES} ${PCRE_LIBRARIES} r3)

    include_directories(${CHECK_INCLUDE_DIRS})
    # include_directories("${PROJECT_SOURCE_DIR}/include/r2")
    add_executable(check_tree check_tree.c)
    target_link_libraries(check_tree ${TEST_LIBS})

    add_custom_command(
        TARGET check_tree POST_BUILD
        COMMENT "Running unit tests"
        COMMAND check_tree
    )
endif (NOT CHECK_FOUND)


