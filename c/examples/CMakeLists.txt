


list(APPEND apps
    simple
    multi
    udp
)

# message(STATUS "yivo::Examples ----------------------")
foreach(name ${apps})
    add_executable(${name} ${name}.c)
    target_link_libraries(${name} ${PROJECT_NAME})
endforeach()
