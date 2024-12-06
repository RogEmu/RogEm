set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(fmt CONFIG REQUIRED)

set(SRC_FILES
    main.cpp
    BIOS.cpp
    RAM.cpp
    Bus.cpp
    CPU.cpp
    Disassembler.cpp
)

add_executable(${BINARY_NAME} ${SRC_FILES})

target_link_libraries(${BINARY_NAME} PRIVATE
    fmt::fmt
)
