#include "MemoryWindow.hpp"
#include "imgui.h"
#include <fmt/format.h>

#include "Debugger.hpp"

MemoryWindow::MemoryWindow(Debugger *debugger) :
    m_debugger(debugger)
{
}

MemoryWindow::~MemoryWindow()
{
}

void MemoryWindow::update()
{
    ImGui::Begin("Memory");

    auto tableFlags = ImGuiTableFlags_Borders
                    | ImGuiTableFlags_RowBg;

    if (ImGui::BeginTable("Memory", 18, tableFlags)) {
        ImGui::TableSetupColumn("Address");
        for (int i = 0; i < 16; i++) {
            ImGui::TableSetupColumn(fmt::format("{:X}", i).c_str());
        }
        ImGui::TableSetupColumn("ASCII");
        ImGui::TableHeadersRow();

        for (int i = 0; i < 256; i += 16) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%08X", i);

            std::string asciiStr;
            for (int j = 0; j < 16; j++) {
                uint8_t byte = m_debugger->readByte(i + j);
                ImGui::TableNextColumn();
                ImGui::Text("%02X", byte);

                char c = byte > 126 || byte < 32 ? '.' : byte;
                asciiStr += c;
            }
            ImGui::TableNextColumn();
            ImGui::Text("%s", asciiStr.c_str());
        }
        ImGui::EndTable();
    }
    ImGui::End();
}
