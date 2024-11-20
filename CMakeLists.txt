cmake_minimum_required(VERSION 3.20)
project("RogEm")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(BINARY_NAME "rogem")
set(CMAKE_COLOR_DIAGNOSTICS ON)

add_subdirectory(src)

option(ENABLE_TESTING "Compile Unit Tests" ON)

if(${ENABLE_TESTING})
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/b514bdc898e2951020cbdca1304b75f5950d1f59.zip
    )

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    add_subdirectory(tests)
endif()
