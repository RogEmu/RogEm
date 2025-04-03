#include "BreakpointWindow.hpp"

#include "imgui.h"
#include "imgui/imgui_memory_editor.h"
#include <fmt/format.h>

BreakpointWindow::BreakpointWindow(Debugger *debugger) :
    m_debugger(debugger),
    m_displayBreakpoints(true)
{
    m_breakpointType = 0;
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
        if (ImGui::BeginCombo("Breakpoint Type", (m_breakpointType == 0) ? "Exec" : (m_breakpointType == 1) ? "Read" : "Write"))
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
                m_breakpoints.push_back({value, m_breakpointType, labelText, true});
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

void BreakpointWindow::DisplayBreakpoints()
{
    ImGui::Checkbox("Display Breakpoints", &m_displayBreakpoints);
    if (!m_displayBreakpoints)
        return;
    ImGui::BeginTable("Breakpoints", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable);
    ImGui::TableSetupColumn("#");
    ImGui::TableSetupColumn("Address");
    ImGui::TableSetupColumn("Type");
    ImGui::TableSetupColumn("Label");
    ImGui::TableSetupColumn("Action");
    ImGui::TableHeadersRow();

    for (auto& bp : m_breakpoints)
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("%ld", &bp - &m_breakpoints[0]);
        ImGui::TableNextColumn();
        ImGui::Text("%08X", bp.addr);
        ImGui::TableNextColumn();
        ImGui::Text("%s", (bp.type == 0) ? "Exec" : (bp.type == 1) ? "Read" : "Write");
        ImGui::TableNextColumn();
        ImGui::Text("%s", bp.label.c_str());
        ImGui::TableNextColumn();
        std::string EnableLabel = fmt::format("Enable##{}", &bp - &m_breakpoints[0]);
        ImGui::Checkbox(EnableLabel.c_str(), &bp.enabled);
        ImGui::SameLine();
        std::string RemoveLabel = fmt::format("Remove##{}", &bp - &m_breakpoints[0]);
        if (ImGui::Button(RemoveLabel.c_str()))
        {
            m_breakpoints.erase(m_breakpoints.begin() + (&bp - &m_breakpoints[0]));
            break;
        }
    }
    ImGui::EndTable();
}

void BreakpointWindow::update()
{
    ImGui::Begin("Breakpoints");
    if (!m_breakpoints.empty())
        DisplayBreakpoints();
    AddBreakpointButton();
    ImGui::End();
}