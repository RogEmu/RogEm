#include "SettingsWindow.hpp"
#include "MemoryWindow.hpp"
#include "Debugger.hpp"
#include "CPU.h"
#include <fstream>
#include <fmt/format.h>

SettingsWindow::SettingsWindow(Debugger* debugger, MemoryWindow* memoryWindow)
    : m_debugger(debugger), m_memoryWindow(memoryWindow)
{}

SettingsWindow::~SettingsWindow()
{}

void SettingsWindow::resetButton()
{
    if (ImGui::Button("Reset System")){
        m_debugger->CPUReset();
    }
}

void SettingsWindow::dumpBIOSButton()
{
    if (!m_memoryWindow) {
        fmt::println("m_memoryWindow is null!");
        return;
    }

    uint32_t baseAddr = m_memoryWindow->getBaseAddr();

    if (ImGui::Button("Dump BIOS to File")) {
        auto slice = m_debugger->memoryRange(baseAddr);
        if (slice && !slice->empty()) {
            const char* filename = "bios_dump.bin";
            std::ofstream outFile(filename, std::ios::binary);
            if (outFile.is_open()) {
                outFile.write(reinterpret_cast<const char*>(slice->data()), slice->size());
                outFile.close();
                fmt::println("BIOS dumped to {}", filename);
            } else {
                fmt::println("Failed to open {} for writing", filename);
            }
        } else {
            fmt::println("Failed to get BIOS memory slice");
        }
    }
}

void SettingsWindow::update()
{
    if (ImGui::Begin("Settings")) {
        resetButton();
        dumpBIOSButton();
    }
    ImGui::End();
}
