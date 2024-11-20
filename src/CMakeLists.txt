set(SRC_FILES
    main.cpp
    BIOS.cpp
    Bus.cpp
    CPU.cpp
    Disassembler.cpp
)

find_package(fmt)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(${BINARY_NAME} ${SRC_FILES})

target_link_libraries(${BINARY_NAME}
    fmt
)
