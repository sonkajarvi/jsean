cmake_minimum_required(VERSION 3.20)

add_executable(tests
    "main.c"
    "test_array.c"
    "test_object.c"
    "test_value.c"
)

target_include_directories(tests PRIVATE
    "../include"
)

target_link_libraries(tests PRIVATE
    "jsean"
)

target_compile_options(tests PRIVATE
    "-g"
    "-O2"
    "-Wall"
    "-Wextra"
    "-Werror"
    "-Wstrict-prototypes"
)
