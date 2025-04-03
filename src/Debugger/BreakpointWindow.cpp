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
        static char label[9] = "\0";
        ImGui::Text("Add Breakpoint");
        if(ImGui::InputText("Address", label, 9, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            char* end;
            uint32_t value = std::strtoul(label, &end, 16);
            if (!*end) {
                m_breakpoints.push_back(value);
            }
            ImGui::CloseCurrentPopup();
            label[0] = '\0';
        }
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
            label[0] = '\0';
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