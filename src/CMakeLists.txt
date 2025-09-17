set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(fmt CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(capstone CONFIG REQUIRED)

set(SRC_FILES
    main.cpp
    System.cpp
    Debugger/Debugger.cpp
    Debugger/Disassembler.cpp
    Debugger/AssemblyWindow.cpp
    Debugger/RegisterWindow.cpp
    Debugger/MemoryWindow.cpp
    Debugger/BreakpointWindow.cpp
    Debugger/LogWindow.cpp
    Debugger/SettingsWindow.cpp
)

add_subdirectory(core)

add_executable(${BINARY_NAME} ${SRC_FILES})

target_include_directories(${BINARY_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/src/
    ${CMAKE_SOURCE_DIR}/src/Debugger
    ${CMAKE_SOURCE_DIR}/third_party/
)

target_link_libraries(${BINARY_NAME} PRIVATE
    rgmcore
    fmt::fmt
    glfw
    imgui::imgui
    spdlog::spdlog
    glad::glad
    capstone::capstone
)
