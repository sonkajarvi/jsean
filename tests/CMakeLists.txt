cmake_minimum_required(VERSION 3.20)

add_executable(tests
    main.c

    string/test_string.c
)

target_include_directories(tests PRIVATE
    .
    ../include
)

target_link_libraries(tests PRIVATE
    jsean
)
