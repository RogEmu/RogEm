set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CURSES_NEED_NCURSES TRUE)

find_package(fmt CONFIG REQUIRED)
find_package(Curses REQUIRED)

set(SRC_FILES
    main.cpp
    BIOS.cpp
    RAM.cpp
    Bus.cpp
    CPU.cpp
    Disassembler.cpp
    Debugger/Window.cpp
    Debugger/Widget.cpp
    Debugger/RegistersWindow.cpp
    Debugger/InstructionsWindow.cpp
    Debugger/MemoryWindow.cpp
    Debugger/ABoxLayout.cpp
    Debugger/HBoxLayout.cpp
    Debugger/VBoxLayout.cpp
    Debugger/Debugger.cpp
)

add_executable(${BINARY_NAME} ${SRC_FILES})

target_include_directories(${BINARY_NAME} PRIVATE
    ./
    Debugger/
    ${NCURSES_INCLUDE_DIRS}
)

target_compile_options(${BINARY_NAME} PRIVATE
    ${CURSES_CFLAGS}
)

target_link_libraries(${BINARY_NAME} PRIVATE
    fmt::fmt
    ${CURSES_LIBRARIES}
)
