option(ENABLE_COVERAGE OFF)

set(TEST_BINARY_NAME rogem_tests)

find_package(GTest CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

add_executable(${TEST_BINARY_NAME}
    Bus_tests.cpp
    CPU_arithmetic_tests.cpp
    CPU_branch_tests.cpp
    CPU_comparison_tests.cpp
    CPU_cop0_tests.cpp
    CPU_exception_tests.cpp
    CPU_jump_tests.cpp
    CPU_load_tests.cpp
    CPU_logical_tests.cpp
    CPU_muldiv_tests.cpp
    CPU_shift_tests.cpp
    CPU_store_tests.cpp
)

if (${ENABLE_COVERAGE})
    target_compile_options(${TEST_BINARY_NAME} PRIVATE --coverage -g)
    target_link_libraries(${TEST_BINARY_NAME} PRIVATE gcov)
endif()

target_link_libraries( ${TEST_BINARY_NAME} PRIVATE
    GTest::gtest
    GTest::gtest_main
    fmt::fmt
    rgmcore
)

include(GoogleTest)
gtest_discover_tests(${TEST_BINARY_NAME})
