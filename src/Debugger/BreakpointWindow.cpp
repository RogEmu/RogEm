#include "BreakpointWindow.hpp"

#include "imgui.h"
#include "imgui/imgui_memory_editor.h"
#include <fmt/format.h>

BreakpointWindow::BreakpointWindow(Debugger *debugger) :
    m_debugger(debugger)
{
}

BreakpointWindow::~BreakpointWindow()
{
}

void BreakpointWindow::AddBreakpointButton()
{
    if (ImGui::Button("Add Breakpoint")){
        ImGui::OpenPopup("Breapkpoint Popup");
    }
    if (ImGui::BeginPopupContextItem("Breapkpoint Popup"))
    {
        static char addr[9] = "\0";
        static char labelText[32] = "\0";
        ImGui::Text("Add Breakpoint");
        ImGui::InputText("Address", addr, 9, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_AutoSelectAll);
        if (ImGui::BeginCombo("Breakpoint Type", "Breakpoint"))
        {
            for (int i = 0; i < 3; i++)
            {
                const char* type = (i == 0) ? "Exec" : (i == 1) ? "Read" : "Write";
                if (ImGui::Selectable(type, false))
                {
                    m_breakpointType = i;
                }
            }
            ImGui::EndCombo();
        }
        ImGui::InputText("Label", labelText, 32, ImGuiInputTextFlags_AutoSelectAll);
        if (ImGui::Button("Add"))
        {
            char* end;
            uint32_t value = std::strtoul(addr, &end, 16);
            if (!*end) {
                m_breakpoints.push_back({value, m_breakpointType, labelText});
                printf("Breakpoint added at %08X with type %d and label %s\n", value, m_breakpointType, labelText);
            }
            ImGui::CloseCurrentPopup();
            addr[0] = '\0';
            labelText[0] = '\0';
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
            addr[0] = '\0';
            labelText[0] = '\0';
        }
        ImGui::EndPopup();
    }
}

void BreakpointWindow::update()
{
    ImGui::Begin("Breakpoints");
    AddBreakpointButton();
    ImGui::End();
}