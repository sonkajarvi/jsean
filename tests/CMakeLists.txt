cmake_minimum_required(VERSION 3.20)

add_executable(tests
    main.c

    json/test_array.c
    json/test_value.c
    json/test_object.c

    string/test_string.c
)

target_include_directories(tests PRIVATE
    .
    ../include
)

target_link_libraries(tests PRIVATE
    jsean
)

target_compile_options(tests PRIVATE
    -g
)
