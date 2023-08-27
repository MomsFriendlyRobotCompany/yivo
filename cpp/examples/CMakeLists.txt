list(APPEND gtests
    main.cpp
    save.cpp
)

# message(STATUS "yivo::Examples ----------------------")
foreach(src ${gtests})
    get_filename_component(name ${src} NAME_WE)
    # message(STATUS " -> ${name}")
    add_executable(${name} ${src})
    target_link_libraries(${name} ${PROJECT_NAME})
    add_test(NAME ${name} COMMAND ${name})
endforeach()
