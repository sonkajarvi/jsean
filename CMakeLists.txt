cmake_minimum_required(VERSION 3.20)

project(jsean LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS "Build tests" OFF)

add_library(jsean
    "jsean.c"
    "jsean_array.c"
    "jsean_bool.c"
    "jsean_null.c"
    "jsean_number.c"
    "jsean_object.c"
    "jsean_read.c"
    "jsean_string.c"
    "jsean_write.c"
)

target_include_directories(jsean PRIVATE
    "."
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
