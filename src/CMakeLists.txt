set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CURSES_NEED_NCURSES TRUE)

# Find the required packages
find_package(fmt CONFIG REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# Specify the source files for the project
set(SRC_FILES
    main.cpp
    BIOS.cpp
    RAM.cpp
    Bus.cpp
    CPU.cpp
    Disassembler.cpp
    # Debugger/Window.cpp
    # Debugger/Widget.cpp
    # Debugger/RegistersWindow.cpp
    # Debugger/InstructionsWindow.cpp
    # Debugger/MemoryWindow.cpp
    # Debugger/ABoxLayout.cpp
    # Debugger/HBoxLayout.cpp
    # Debugger/VBoxLayout.cpp
    # Debugger/Debugger.cpp
    NewDebugger/Debugger.cpp
)

# Create the executable
add_executable(${BINARY_NAME} ${SRC_FILES})

# Set the correct path for imgui_impl_glfw.cpp and imgui_impl_opengl3.cpp
# set(IMGUI_BACKENDS
#     ${CMAKE_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp
#     ${CMAKE_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp
# )

# Add them to your executable target (not linking as libraries)
# target_sources(${BINARY_NAME} PRIVATE ${IMGUI_BACKENDS})

# Include directories for imgui and backends
include_directories(
    ./
    NewDebugger/
    ${CMAKE_SOURCE_DIR}/deps/imgui
    ${CMAKE_SOURCE_DIR}/deps/imgui/backends
)

# Link the required libraries to the executable
target_link_libraries(${BINARY_NAME} PRIVATE
    fmt::fmt
    glfw
    OpenGL::GL
    imgui
)
