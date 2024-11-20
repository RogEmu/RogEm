enable_testing()

set(TEST_BINARY_NAME rogem_tests)
set(SRC_FILES
    ${CMAKE_SOURCE_DIR}/src/BIOS.cpp
    ${CMAKE_SOURCE_DIR}/src/Bus.cpp
    ${CMAKE_SOURCE_DIR}/src/CPU.cpp
)

add_executable(
    ${TEST_BINARY_NAME}
    ${SRC_FILES}
    Utils.cpp
    CPU_tests.cpp
    CPU_arithmetic_tests.cpp
    CPU_logical_tests.cpp
)

target_compile_options(${TEST_BINARY_NAME} PRIVATE --coverage)
target_link_libraries(
    ${TEST_BINARY_NAME}
    GTest::gtest_main
    gcov
)
target_include_directories(${TEST_BINARY_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

include(GoogleTest)
gtest_discover_tests(${TEST_BINARY_NAME})
