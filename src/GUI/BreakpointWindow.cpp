#include "BreakpointWindow.hpp"
#include "Debugger/Debugger.hpp"

#include "imgui.h"
#include "imgui_stdlib.h"
#include <fmt/format.h>

BreakpointWindow::BreakpointWindow(Debugger *debugger) :
    m_debugger(debugger)
{
    setTitle("Breakpoints");
    m_breakpointType = 0;
}

BreakpointWindow::~BreakpointWindow()
{
}

void BreakpointWindow::addBreakpointButton()
{
    if (ImGui::Button("Add Breakpoint")){
        ImGui::OpenPopup("Breakpoint Popup");
    }
    if (ImGui::BeginPopupContextItem("Breakpoint Popup"))
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
                m_debugger->addBreakpoint(value, static_cast<BreakpointType>(m_breakpointType), labelText, false);
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
        std::string labelId = fmt::format("##label{}", index);
        ImGui::PushItemWidth(-1);
        ImGui::InputText(labelId.c_str(), &bp.label, ImGuiInputTextFlags_AutoSelectAll);
        ImGui::PopItemWidth();
        m_debugger->saveBreakpointsToFile();
        ImGui::TableNextColumn();
        std::string EnableLabel = fmt::format("Enable##{}", index);
        ImGui::Checkbox(EnableLabel.c_str(), &bp.enabled);
        ImGui::SameLine();
        std::string RemoveLabel = fmt::format("Remove##{}", index);
        if (ImGui::Button(RemoveLabel.c_str())) {
            m_debugger->removeBreakpoint(index);
        }
        index++;
    }
    ImGui::EndTable();
}

void BreakpointWindow::update()
{
    if (ImGui::Begin("Breakpoints")) {
        static bool enableBreakpoints = true;

        if (ImGui::Checkbox("Enable breakpoints", &enableBreakpoints)) {
            for (auto &bp : m_debugger->getBreakpoints()) {
                bp.enabled = enableBreakpoints;
            }
            m_debugger->saveBreakpointsToFile();
        }

        if (!m_debugger->getBreakpoints().empty()) {

            displayBreakpoints();
        }
        addBreakpointButton();
    }
    ImGui::End();
}
