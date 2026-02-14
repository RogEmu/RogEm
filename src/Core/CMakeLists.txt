set(CORE_LIB_NAME "rgmcore")

find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

option(ENABLE_COVERAGE "Enable Code Coverage With GCOVR" ON)

set(CORE_SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/BIOS.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RAM.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Bus.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CPU.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GTE.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MemoryMap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Memory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PsxExecutable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GTE.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ScratchPad.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DMA.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DMAChannel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GPU.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SPU.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SerialInterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SIO0.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DigitalPad.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Timers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InterruptController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SystemControlCop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MemoryControl1.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MemoryControl2.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GPUCommand.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CacheControl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Expansion2.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/System.cpp
)

add_library(${CORE_LIB_NAME} STATIC ${CORE_SRC_FILES})

target_include_directories(${CORE_LIB_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

if (${ENABLE_COVERAGE})
    target_compile_options(${CORE_LIB_NAME} PRIVATE --coverage -g)
    target_link_libraries(${CORE_LIB_NAME} PRIVATE gcov)
endif()

target_link_libraries(${CORE_LIB_NAME} PRIVATE
    fmt::fmt
    spdlog::spdlog
)
