cmake_minimum_required(VERSION 3.20)

project(jsean LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

add_subdirectory(tests)
