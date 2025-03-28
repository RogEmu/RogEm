#include "RegisterWindow.hpp"

#include <fmt/format.h>

#include "imgui.h"
#include "Debugger.hpp"
#include "Disassembler.h"
#include "CPU.h"

RegisterWindow::RegisterWindow(Debugger *debugger) :
    m_debugger(debugger),
    m_editorOpen(false),
    m_registerNameToChange("")
{
}

void PushColorText(bool condition)
{
    if (condition)
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 200, 0, 255));
    else
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(150, 150, 150, 255));
}

void RegisterWindow::addEditButton(const char* regName, int regIndex)
{
    ImGui::SameLine();
    std::string label = fmt::format("Edit##{}", regName);
    if (ImGui::SmallButton(label.c_str())) {
        m_editorOpen = true;
        m_registerNameToChange = regName;
        m_registerIndex = regIndex;
    }
}

RegisterWindow::~RegisterWindow()
{
}

void RegisterWindow::displayPopup()
{
    ImGui::OpenPopup("Edit Registers");
    if(ImGui::BeginPopupModal("Edit Registers", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Edit Registers of %s", m_registerNameToChange.c_str());
        static char label[9] = "\0";
        if(ImGui::InputText("Value", label, 9, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            printf("%i\n", m_registerIndex);
            char* end;
            uint32_t value = std::strtoul(label, &end, 16);
            if (!*end) {
                if (m_registerIndex < NB_GPR)
                    m_debugger->setGPR((uint8_t)m_registerIndex, value);
            }
            m_editorOpen = false;
            ImGui::CloseCurrentPopup();
            label[0] = '\0';
        }
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
            m_editorOpen = false;
            label[0] = '\0';
        }
        ImGui::EndPopup();
    }
}

void RegisterWindow::drawGpr()
{
    for (int i = 0; i < NB_GPR; i++) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        const char* regName = Disassembler::getRegisterName((uint8_t)i).c_str();
        ImGui::Text("%s", regName);
        ImGui::TableNextColumn();
        ImGui::Text("%08X", m_debugger->getGPR((uint8_t)i));
        addEditButton(regName, i);
        m_prevGPR[i] = m_debugger->getGPR((uint8_t)i);
    }
}

void RegisterWindow::drawSpecialRegs()
{
    const char* extraRegisters[] = {"PC", "HI", "LO"};
    uint32_t extraValues[] = {
        m_debugger->getSpecialReg((uint8_t)SpecialRegIndex::PC),
        m_debugger->getSpecialReg((uint8_t)SpecialRegIndex::HI),
        m_debugger->getSpecialReg((uint8_t)SpecialRegIndex::LO)
    };
    for (int i = 0; i < 3; i++)
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("%s", extraRegisters[i]);
        ImGui::TableNextColumn();
        m_prevSpecialRegs[i] = extraValues[i];
        ImGui::Text("%08X", extraValues[i]);
    }
}

void RegisterWindow::drawCop0Regs()
{
    //COP0 Registers
    ImGui::TableNextColumn();
    PushColorText(m_prevCop0Regs[0] != m_debugger->getCop0Reg(12));
    m_prevCop0Regs[0] = m_debugger->getCop0Reg(12);
    ImGui::Text("COP0-SR");
    ImGui::TableNextColumn();
    ImGui::Text("%08X", m_debugger->getCop0Reg(12));
    ImGui::PopStyleColor();

    ImGui::TableNextColumn();
    PushColorText(m_prevCop0Regs[1] != m_debugger->getCop0Reg(13));
    ImGui::Text("COP0-CAUSE");
    ImGui::TableNextColumn();
    m_prevCop0Regs[1] = m_debugger->getCop0Reg(13);
    ImGui::Text("%08X", m_debugger->getCop0Reg(13));
    ImGui::PopStyleColor();
}

void RegisterWindow::update()
{
    auto tableFlags = ImGuiTableFlags_BordersOuter
                    | ImGuiTableFlags_RowBg
                    | ImGuiTableFlags_ScrollY;

    ImGui::Begin("Registers");
    if(ImGui::BeginTable("Registers", 2, tableFlags))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Register", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Value");
        ImGui::TableHeadersRow();

        drawGpr();
        drawSpecialRegs();
        drawCop0Regs();

        ImGui::EndTable();
        if (m_editorOpen == true)
        {
            displayPopup();
        }
    }
    ImGui::End();
}
