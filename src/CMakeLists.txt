set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CURSES_NEED_NCURSES TRUE)

find_package(fmt CONFIG REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

set(SRC_FILES
    main.cpp
    BIOS.cpp
    RAM.cpp
    Bus.cpp
    CPU.cpp
    Disassembler.cpp
    Debugger/Debugger.cpp
)

add_executable(${BINARY_NAME} ${SRC_FILES})

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/Debugger
    ${CMAKE_SOURCE_DIR}/deps/imgui
    ${CMAKE_SOURCE_DIR}/deps/imgui/backends
)

target_link_libraries(${BINARY_NAME} PRIVATE
    fmt::fmt
    glfw
    OpenGL::GL
    imgui
)
