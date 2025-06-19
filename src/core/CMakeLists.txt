find_package(fmt CONFIG REQUIRED)

set(CORE_LIB_NAME "rgmcore")

find_package(fmt CONFIG REQUIRED)
find_package(capstone CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

set(CORE_SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/BIOS.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RAM.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bus.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CPU.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GTE.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Disassembler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MemoryMap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Memory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PsxExecutable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GTE.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ScratchPad.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DMA.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DMAChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GPU.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SPU.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Timers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InterruptController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SystemControlCop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MemoryControl1.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GPUCommand.cpp
)

add_library(${CORE_LIB_NAME} STATIC ${CORE_SRC_FILES})

target_include_directories(${CORE_LIB_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${CORE_LIB_NAME} PUBLIC
    fmt::fmt
    capstone::capstone
    spdlog::spdlog
)

if (${ENABLE_COVERAGE})
    target_compile_options(${CORE_LIB_NAME} PRIVATE --coverage -g)
    target_link_options(${CORE_LIB_NAME} PRIVATE --coverage)
    target_link_libraries(${CORE_LIB_NAME} PRIVATE gcov)
endif()
