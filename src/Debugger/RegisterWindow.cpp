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
    auto tableFlags = ImGuiTableFlags_BordersOuter
                    | ImGuiTableFlags_RowBg
                    | ImGuiTableFlags_ScrollY;

    if (ImGui::BeginTabItem("GPR"))
    {
        if(ImGui::BeginTable("Registers", 2, tableFlags))
        {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Register", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Value");
            ImGui::TableHeadersRow();

            for (uint8_t i = 0; i < NB_GPR; i++)
            {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                uint32_t value = m_debugger->getGPR(i);
                drawRegister(i);
                m_prevGPR[i] = value;
            }
            drawSpecialRegs();
            ImGui::EndTable();
        }
        ImGui::EndTabItem();
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
        ImGui::Text("%08X", extraValues[i]);
        m_prevSpecialRegs[i] = extraValues[i];
    }
}

void RegisterWindow::drawCop0Regs()
{
    auto tableFlags = ImGuiTableFlags_BordersOuter
                    | ImGuiTableFlags_RowBg
                    | ImGuiTableFlags_ScrollY;

    if (ImGui::BeginTabItem("COP0"))
    {
        if(ImGui::BeginTable("Registers", 2, tableFlags))
        {

            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Register", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Value");
            ImGui::TableHeadersRow();

            ImGui::TableNextColumn();
            m_prevCop0Regs[0] = m_debugger->getCop0Reg(12);
            ImGui::Text("COP0-SR");
            ImGui::TableNextColumn();
            ImGui::Text("%08X", m_debugger->getCop0Reg(12));

            ImGui::TableNextColumn();
            ImGui::Text("COP0-CAUSE");
            ImGui::TableNextColumn();
            m_prevCop0Regs[1] = m_debugger->getCop0Reg(13);
            ImGui::Text("%08X", m_debugger->getCop0Reg(13));
            ImGui::EndTable();
        }
        ImGui::EndTabItem();
    }
}

void RegisterWindow::drawRegister(uint8_t index)
{
    ImColor textColor(ImGui::GetColorU32(ImGuiCol_Text));
    auto value = m_debugger->getGPR(index);
    auto name = Disassembler::getRegisterName(index).c_str();

    if (value == 0)
        textColor = ImGui::GetColorU32(ImGuiCol_TextDisabled);
    if (value != m_prevGPR[index])
        textColor = ImColor(255, 209, 25);
    ImGui::Text("%s", name);
    ImGui::TableNextColumn();
    ImGui::TextColored(textColor, "%08X", m_debugger->getGPR(index));
    addEditButton(name, index);
}

void RegisterWindow::update()
{
    // auto tableFlags = ImGuiTableFlags_BordersOuter
    //                 | ImGuiTableFlags_RowBg
    //                 | ImGuiTableFlags_ScrollY;

    ImGui::Begin("Registers");
    if (ImGui::BeginTabBar("##CPURegistersBar"))
    {
        drawGpr();
        drawCop0Regs();
        ImGui::EndTabBar();
    }
    if (m_editorOpen == true)
    {
        displayPopup();
    }
    ImGui::End();
}
