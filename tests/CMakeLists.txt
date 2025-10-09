cmake_minimum_required(VERSION 3.20)

add_executable(tests
    "main.c"
    "test_array.c"
    "test_object.c"
    "test_read_array.c"
    "test_read_number.c"
    "test_read_object.c"
    "test_read_string.c"
    "test_read_value.c"
    "test_value.c"
)

target_include_directories(tests PRIVATE
    "../include"
)

target_link_libraries(tests PRIVATE
    "jsean"
)

target_compile_options(tests PRIVATE
    "-Wall"
    "-Wextra"
    "-Werror"
    "-Wstrict-prototypes"
)
