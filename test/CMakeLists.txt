cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(jsean_test
    main.c

    test_string.c
)

target_include_directories(jsean_test PRIVATE
    ../include
)

target_link_libraries(jsean_test PRIVATE
    jsean
)
