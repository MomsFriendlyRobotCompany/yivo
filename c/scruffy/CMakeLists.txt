
list(APPEND tests
    test.c
)

add_executable(test-all ${tests})
target_link_libraries(test-all scruffy ${PROJECT_NAME})
