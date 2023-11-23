list(APPEND apps
    main
    save
    crc
    array_alt
)

# message(STATUS "yivo::Examples ----------------------")
foreach(name ${apps})
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} ${PROJECT_NAME})
endforeach()
