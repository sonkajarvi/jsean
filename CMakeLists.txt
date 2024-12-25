cmake_minimum_required(VERSION 3.20)

project(jsean LANGUAGES C)

add_library(jsean
    # JSON
    src/array.c
    src/json.c
    src/object.c
    src/parse.c
    src/stringify.c

    # String utility
    src/string.c
)

set_target_properties(jsean PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
)

target_include_directories(jsean PRIVATE
    include
)

target_compile_options(jsean PRIVATE
    -g
    -O3
    -Wall
    -Wextra
    -Werror
    -Wstrict-prototypes
)

enable_testing()
add_subdirectory(test)
