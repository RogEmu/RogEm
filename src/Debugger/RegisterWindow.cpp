#include "RegisterWindow.hpp"

#include <fmt/format.h>

#include "imgui.h"
#include "Debugger.hpp"
#include "CPU.h"

RegisterWindow::RegisterWindow(Debugger *debugger) :
    m_debugger(debugger),
    m_editorOpen(false),
    m_registerNameToChange("")
{
    is_highlighted.resize(NB_GPR, false);
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
                    m_debugger->setCpuReg(static_cast<CpuReg>(m_registerIndex), value);
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
                drawRegister(i);
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
        m_debugger->getCpuReg(CpuReg::PC),
        m_debugger->getCpuReg(CpuReg::HI),
        m_debugger->getCpuReg(CpuReg::LO)
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

            for (uint8_t i = 0; i < COP0_NB_REG; i++) {
                ImGui::TableNextColumn();
                uint32_t regVal = m_debugger->getCop0Reg(i);

                ImColor textColor(regVal != 0 ?
                                ImGui::GetColorU32(ImGuiCol_Text)
                                : ImGui::GetColorU32(ImGuiCol_TextDisabled));
                m_prevCop0Regs[i] = regVal;
                ImGui::Text("%s", m_debugger->getDisassembler().copRegName(0, i).c_str());
                ImGui::TableNextColumn();
                ImGui::TextColored(textColor, "%08X", regVal);
            }
            ImGui::EndTable();
        }
        ImGui::EndTabItem();
    }
}

bool isGPREqual(uint32_t* current, uint32_t* previous)
{
    for (uint8_t i = 0; i < NB_GPR; i++)
    {
        if (current[i] != previous[i])
            return false;
    }
    return true;
}

void RegisterWindow::updateRegisterChanges()
{
    for (uint8_t i = 0; i < NB_GPR; i++)
    {
        m_currentGPR[i] = m_debugger->getCpuReg(static_cast<CpuReg>(i));
    }
    if (isGPREqual(m_currentGPR, m_prevGPR) == false)
    {
        for (uint8_t i = 0; i < NB_GPR; i++)
        {
            if (m_currentGPR[i] != m_prevGPR[i])
            {
                is_highlighted[i] = true;
            }
            else
            {
                is_highlighted[i] = false;
            }
        }
        std::copy(std::begin(m_currentGPR), std::end(m_currentGPR), std::begin(m_prevGPR));
    }
}


void RegisterWindow::drawRegister(uint8_t index)
{
    ImColor textColor(ImGui::GetColorU32(ImGuiCol_Text));
    auto value = m_debugger->getCpuReg(static_cast<CpuReg>(index));
    auto name = m_debugger->getDisassembler().cpuRegName(index).c_str();

    if (value == 0)
        textColor = ImGui::GetColorU32(ImGuiCol_TextDisabled);
    if (is_highlighted[index]) {
        textColor = ImColor(255, 209, 25);
    }
    ImGui::Text("%s", name);
    ImGui::TableNextColumn();
    ImGui::TextColored(textColor, "%08X", m_debugger->getCpuReg(static_cast<CpuReg>(index)));
    addEditButton(name, index);
}

void RegisterWindow::update()
{
    if (ImGui::Begin("Registers")) {

        if (ImGui::BeginTabBar("##CPURegistersBar"))
        {
            updateRegisterChanges();
            drawGpr();
            drawCop0Regs();
            ImGui::EndTabBar();
        }
        if (m_editorOpen == true)
        {
            displayPopup();
        }
    }
    ImGui::End();
}
