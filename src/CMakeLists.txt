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
    Application.cpp
    Debugger/Debugger.cpp
    Debugger/Disassembler.cpp
    GUI/AssemblyWindow.cpp
    GUI/RegisterWindow.cpp
    GUI/MemoryWindow.cpp
    GUI/MainMenuBar.cpp
    GUI/BreakpointWindow.cpp
    GUI/LogWindow.cpp
    GUI/SettingsWindow.cpp
)

add_subdirectory(Core)

add_executable(${BINARY_NAME} ${SRC_FILES})

target_include_directories(${BINARY_NAME}
    PRIVATE ${CMAKE_SOURCE_DIR}/src/
    PRIVATE ${CMAKE_SOURCE_DIR}/third_party/
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
