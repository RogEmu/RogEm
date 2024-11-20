set(SRC_FILES
    main.cpp
    BIOS.cpp
    Bus.cpp
    CPU.cpp
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(${BINARY_NAME} ${SRC_FILES})
