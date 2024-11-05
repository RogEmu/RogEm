enable_testing()

set(TEST_BINARY_NAME rogem_tests)

add_executable(
    ${TEST_BINARY_NAME}
    sample.cpp
)

target_compile_options(${TEST_BINARY_NAME} PRIVATE --coverage)
target_link_libraries(
    ${TEST_BINARY_NAME}
    GTest::gtest_main
    gcov
)

include(GoogleTest)
gtest_discover_tests(${TEST_BINARY_NAME})
