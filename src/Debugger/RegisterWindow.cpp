#include "RegisterWindow.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "CPU.h"
#include "Disassembler.h"

RegisterWindow::RegisterWindow(const std::shared_ptr<CPU> &cpu) :
    m_cpu(cpu)
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

RegisterWindow::~RegisterWindow()
{
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
            if (previousRegisters[i] != m_cpu->m_registers[i])
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 200, 0, 255));
            else
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(150, 150, 150, 255));
            ImGui::Text("%s", Disassembler::getRegisterName(i).c_str());
            ImGui::TableNextColumn();
            previousRegisters[i] = m_cpu->m_registers[i];
            ImGui::Text("%08X", m_cpu->m_registers[i]);
            ImGui::PopStyleColor();

        }
        const char* extraRegisters[] = {"PC", "HI", "LO"};
        uint32_t extraValues[] = {m_cpu->m_pc, m_cpu->m_hi, m_cpu->m_lo};

        for (int i = 0; i < 3; i++) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            if (previousExtraRegisters[i] != extraValues[i])
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 200, 0, 255));
            else
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(150, 150, 150, 255));
            ImGui::Text("%s", extraRegisters[i]);
            ImGui::TableNextColumn();
            previousExtraRegisters[i] = extraValues[i];
            ImGui::Text("%08X", extraValues[i]);
            ImGui::PopStyleColor();
        }

        //COP0 Registers
        ImGui::TableNextColumn();
        if (previousCopRegisters[0] != m_cpu->m_cop0Reg[12])
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 200, 0, 255));
        else
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(150, 150, 150, 255));
        previousCopRegisters[0] = m_cpu->m_cop0Reg[12];
        ImGui::Text("COP0-SR");
        ImGui::TableNextColumn();
        ImGui::Text("%08X", m_cpu->m_cop0Reg[12]);
        ImGui::PopStyleColor();
        ImGui::TableNextColumn();
        if (previousCopRegisters[1] != m_cpu->m_cop0Reg[13])
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 200, 0, 255));
        else
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(150, 150, 150, 255));
        ImGui::Text("COP0-CAUSE");
        ImGui::TableNextColumn();
        previousCopRegisters[1] = m_cpu->m_cop0Reg[13];
        ImGui::Text("%08X", m_cpu->m_cop0Reg[13]);
        ImGui::PopStyleColor();

        ImGui::EndTable();
    }
    ImGui::End();
}
