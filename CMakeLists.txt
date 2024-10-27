cmake_minimum_required(VERSION 3.20)
project("RogEm")

set(BINARY_NAME "rogem")
set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)
