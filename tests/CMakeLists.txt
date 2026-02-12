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
    GTE_coordinate_tests.cpp
    GTE_general_tests.cpp
    GTE_instructions_tests.cpp
    GTE_projections_tests.cpp
    GTE_general_tests.cpp
    GTE_instructions_tests.cpp
    GTE_projections_tests.cpp
    GTE_calculation_tests.cpp
    GTE_calculation_tests_two.cpp
    GPU_register_basic_tests.cpp
    BIOS_tests.cpp
    DMA_tests.cpp
    COP0_tests.cpp
    Expansion2_tests.cpp
    RAM_tests.cpp
    SPU_tests.cpp
    PsxExecutable_tests.cpp
)

target_include_directories(${TEST_BINARY_NAME}
    PRIVATE ${CMAKE_SOURCE_DIR}/src/
)

target_link_libraries(${TEST_BINARY_NAME} PRIVATE
    GTest::gtest
    GTest::gtest_main
    fmt::fmt
    rgmcore
)

include(GoogleTest)
gtest_discover_tests(${TEST_BINARY_NAME})
