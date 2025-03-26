#include "RegisterWindow.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "CPU.h"
#include "Disassembler.h"

RegisterWindow::RegisterWindow(const std::shared_ptr<CPU> &cpu) :
    m_cpu(cpu)
{
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
            ImGui::Text("%s", Disassembler::getRegisterName(i).c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%08X", m_cpu->m_registers[i]);
        }
        const char* extraRegisters[] = {"PC", "HI", "LO"};
        uint32_t extraValues[] = {m_cpu->m_pc, m_cpu->m_hi, m_cpu->m_lo};

        for (int i = 0; i < 3; i++) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%s", extraRegisters[i]);
            ImGui::TableNextColumn();
            ImGui::Text("%08X", extraValues[i]);
        }

        //COP0 Registers
        ImGui::TableNextColumn();
        ImGui::Text("COP0-SR");
        ImGui::TableNextColumn();
        ImGui::Text("%08X", m_cpu->m_cop0Reg[12]);
        ImGui::TableNextColumn();
        ImGui::Text("COP0-CAUSE");
        ImGui::TableNextColumn();
        ImGui::Text("%08X", m_cpu->m_cop0Reg[13]);

        ImGui::EndTable();
    }
    ImGui::End();
}
