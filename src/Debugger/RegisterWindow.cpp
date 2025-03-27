#include "RegisterWindow.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "CPU.h"
#include "Disassembler.h"

#include <fmt/format.h>

RegisterWindow::RegisterWindow(const std::shared_ptr<CPU> &cpu) :
    m_cpu(cpu),
    m_editorOpen(false),
    m_registerNameToChange("")
{
    previousRegisters.resize(NB_GPR);
    while (previousRegisters.size() < NB_GPR)
    {
        previousRegisters.push_back(0);
    }
    previousExtraRegisters.resize(3);
    while (previousExtraRegisters.size() < 3)
    {
        previousExtraRegisters.push_back(0);
    }
    previousCopRegisters.resize(2);
    while (previousCopRegisters.size() < 2)
    {
        previousCopRegisters.push_back(0);
    }
}

void PushColorText(bool condition)
{
    if (condition)
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 200, 0, 255));
    else
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(150, 150, 150, 255));
}

void RegisterWindow::AddEditButton(const char* regName, int regIndex)
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

void RegisterWindow::DisplayPopup()
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
                    m_cpu->m_registers[m_registerIndex] = value;
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

void RegisterWindow::update()
{
    auto tableFlags = ImGuiTableFlags_Borders
                    | ImGuiTableFlags_RowBg;
    ImGui::Begin("Registers");
    if(ImGui::BeginTable("Registers", 2, tableFlags))
    {
        ImGui::TableSetupColumn("Register");
        ImGui::TableSetupColumn("Value");
        ImGui::TableHeadersRow();

        for (int i = 0; i < NB_GPR; i++) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            const char* regName = Disassembler::getRegisterName(i).c_str();
            PushColorText(previousRegisters[i] != m_cpu->m_registers[i]);
            ImGui::Text("%s", regName);
            ImGui::TableNextColumn();
            previousRegisters[i] = m_cpu->m_registers[i];
            ImGui::Text("%08X", m_cpu->m_registers[i]);
            ImGui::PopStyleColor();
            AddEditButton(regName, i);
        }

        const char* extraRegisters[] = {"PC", "HI", "LO"};
        uint32_t extraValues[] = {m_cpu->m_pc, m_cpu->m_hi, m_cpu->m_lo};
        for (int i = 0; i < 3; i++) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            PushColorText(previousExtraRegisters[i] != extraValues[i]);
            ImGui::Text("%s", extraRegisters[i]);
            ImGui::TableNextColumn();
            previousExtraRegisters[i] = extraValues[i];
            ImGui::Text("%08X", extraValues[i]);
            ImGui::PopStyleColor();
        }

        //COP0 Registers
        ImGui::TableNextColumn();
        PushColorText(previousCopRegisters[0] != m_cpu->m_cop0Reg[12]);
        previousCopRegisters[0] = m_cpu->m_cop0Reg[12];
        ImGui::Text("COP0-SR");
        ImGui::TableNextColumn();
        ImGui::Text("%08X", m_cpu->m_cop0Reg[12]);
        ImGui::PopStyleColor();

        ImGui::TableNextColumn();
        PushColorText(previousCopRegisters[1] != m_cpu->m_cop0Reg[13]);
        ImGui::Text("COP0-CAUSE");
        ImGui::TableNextColumn();
        previousCopRegisters[1] = m_cpu->m_cop0Reg[13];
        ImGui::Text("%08X", m_cpu->m_cop0Reg[13]);
        ImGui::PopStyleColor();

        ImGui::EndTable();
        if (m_editorOpen == true)
        {
            DisplayPopup();
        }
    }
    ImGui::End();
}
