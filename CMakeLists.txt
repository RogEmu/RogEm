cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)

project("RogEm")

set(BINARY_NAME "rogem")
set(CMAKE_COLOR_DIAGNOSTICS ON)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

add_subdirectory(deps)
add_subdirectory(src)

option(ENABLE_TESTING "Compile Unit Tests" ON)

if(${ENABLE_TESTING})
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    enable_testing()
    add_subdirectory(tests)
endif()
