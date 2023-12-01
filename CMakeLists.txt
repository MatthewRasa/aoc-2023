cmake_minimum_required(VERSION 3.24)
project(advent)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_compile_options(-W -Wall -Wextra)
add_compile_options("$<$<CONFIG:DEBUG>:-g;-O0>")
add_compile_options("$<$<CONFIG:RELEASE>:-O3>")

add_subdirectory(src)
