cmake_minimum_required(VERSION 3.20)

project(jsean LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS "Build tests" OFF)

add_library(jsean
    "src/array.c"
    "src/object.c"
    "src/read.c"
    "src/strbuf.c"
    "src/value.c"
)

target_include_directories(jsean PRIVATE
    "include"
)

target_compile_options(jsean PRIVATE
    "-Wall"
    "-Wextra"
    "-Werror"
    "-Wstrict-prototypes"
)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()
