#include "BreakpointWindow.hpp"
#include "Debugger.hpp"

#include "imgui.h"
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

void BreakpointWindow::addBreakpointButton()
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
        if (ImGui::BeginCombo("Breakpoint Type", "Exec"))
        {
            if (ImGui::Selectable("Exec", false))
                m_breakpointType = 0;
            if (ImGui::Selectable("Read", false))
                m_breakpointType = 1;
            if (ImGui::Selectable("Write", false))
                m_breakpointType = 2;
            ImGui::EndCombo();
        }
        ImGui::InputText("Label", labelText, 32, ImGuiInputTextFlags_AutoSelectAll);
        if (ImGui::Button("Add"))
        {
            char* end;
            uint32_t value = std::strtoul(addr, &end, 16);
            if (!*end) {
                if (m_breakpointType == 0)
                    m_debugger->addBreakpoint(value, BreakpointType::EXEC, labelText, false);
                else if (m_breakpointType == 1)
                    m_debugger->addBreakpoint(value, BreakpointType::READ, labelText, false);
                else
                    m_debugger->addBreakpoint(value, BreakpointType::WRITE, labelText, false);
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

void BreakpointWindow::displayBreakpoints()
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
    long index = 0;

    for (auto& bp : m_debugger->getBreakpoints())
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("%ld", index);
        ImGui::TableNextColumn();
        ImGui::Text("%08X", bp.addr);
        ImGui::TableNextColumn();
        if (bp.instructionType == BreakpointType::EXEC)
            ImGui::Text("%s", "Exec");
        else if (bp.instructionType == BreakpointType::READ)
            ImGui::Text("%s", "Read");
        else
            ImGui::Text("%s", "Write");
        ImGui::TableNextColumn();
        ImGui::Text("%s", bp.label.c_str());
        ImGui::TableNextColumn();
        std::string EnableLabel = fmt::format("Enable##{}", index);
        ImGui::Checkbox(EnableLabel.c_str(), &bp.enabled);
        ImGui::SameLine();
        std::string RemoveLabel = fmt::format("Remove##{}", index);
        if (ImGui::Button(RemoveLabel.c_str()))
        {
            m_debugger->removeBreakpoint(index);
        }
        index++;
    }
    ImGui::EndTable();
}

void BreakpointWindow::update()
{
    if (ImGui::Begin("Breakpoints")) {
        if (!m_debugger->getBreakpoints().empty())
           displayBreakpoints();
        addBreakpointButton();
    }
    ImGui::End();
}
