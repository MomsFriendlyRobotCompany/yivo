list(APPEND apps
    main
    save
)

# message(STATUS "yivo::Examples ----------------------")
foreach(name ${apps})
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} ${PROJECT_NAME})
endforeach()
