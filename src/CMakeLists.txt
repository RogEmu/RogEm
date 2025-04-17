set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(fmt CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(imgui CONFIG REQUIRED)

set(SRC_FILES
    main.cpp
    BIOS.cpp
    RAM.cpp
    Bus.cpp
    CPU.cpp
    Disassembler.cpp
    System.cpp
    MemoryMap.cpp
    Memory.cpp
    Debugger/Debugger.cpp
    Debugger/AssemblyWindow.cpp
    Debugger/RegisterWindow.cpp
    Debugger/MemoryWindow.cpp
    Debugger/BreakpointWindow.cpp
    Debugger/LogWindow.cpp
)

add_executable(${BINARY_NAME} ${SRC_FILES})

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/Debugger
    ${CMAKE_SOURCE_DIR}/third_party/
)

target_link_libraries(${BINARY_NAME} PRIVATE
    fmt::fmt
    glfw
    OpenGL::GL
    imgui::imgui
)
