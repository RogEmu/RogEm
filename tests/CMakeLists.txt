enable_testing()

option(ENABLE_COVERAGE OFF)

set(TEST_BINARY_NAME rogem_tests)
set(SRC_FILES
    ${CMAKE_SOURCE_DIR}/src/BIOS.cpp
    ${CMAKE_SOURCE_DIR}/src/Bus.cpp
    ${CMAKE_SOURCE_DIR}/src/CPU.cpp
    ${CMAKE_SOURCE_DIR}/src/Disassembler.cpp
)

find_package(GTest CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

add_executable(
    ${TEST_BINARY_NAME}
    ${SRC_FILES}
    Utils.cpp
    CPU_tests.cpp
    CPU_arithmetic_tests.cpp
    CPU_logical_tests.cpp
    CPU_multiply_tests.cpp
    CPU_shift_tests.cpp
)

if (${ENABLE_COVERAGE})
    target_compile_options(${TEST_BINARY_NAME} PRIVATE --coverage)
    target_link_libraries(${TEST_BINARY_NAME} PRIVATE gcov)
endif()

target_link_libraries( ${TEST_BINARY_NAME} PRIVATE
    GTest::gtest_main
    fmt::fmt
)

target_include_directories(${TEST_BINARY_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

include(GoogleTest)
gtest_discover_tests(${TEST_BINARY_NAME})
