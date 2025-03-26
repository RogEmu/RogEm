#include "InstructionWindow.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <fmt/format.h>

#include "CPU.h"
#include "Disassembler.h"

InstructionWindow::InstructionWindow(const std::shared_ptr<CPU> &cpu, bool &systemPaused, float &simSpeed) :
    m_cpu(cpu),
    m_systemPaused(systemPaused),
    m_simSpeed(simSpeed)
{
}

InstructionWindow::~InstructionWindow()
{
}

void InstructionWindow::update()
{
    ImGui::Begin("Instructions");

    ImGui::BeginGroup();
    ImGui::Checkbox("Pause", &m_systemPaused);
    ImGui::SameLine();
    if (ImGui::ArrowButton("##CPU_Step_Fw", ImGuiDir_Right) && m_systemPaused)
    {
        m_cpu->step();
    }
    ImGui::EndGroup();
    ImGui::SliderFloat("Simulation Speed", &m_simSpeed, 0.00001f, 1.0f, "%.5f");

    auto tableFlags = ImGuiTableFlags_RowBg
                    | ImGuiTableFlags_Borders;

    if (ImGui::BeginTable("Instructions", 1, tableFlags)) {
        ImGui::TableSetupColumn("Instructions");
        ImGui::TableHeadersRow();

        int nbInstructions = 32;
        int half = nbInstructions / 2;

        for (int i = -half; i < half; i++) {
            ImGui::TableNextColumn();
            uint32_t pc = m_cpu->m_pc + i * 4;
            Instruction instr = {.raw = m_cpu->m_bus->loadWord(pc)};
            auto str = Disassembler::disassemble(pc, instr);
            if (pc == m_cpu->m_pc)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 200, 0, 255));
                ImGui::Text("%s", str.c_str());
                ImGui::PopStyleColor();
            } else
            {
                ImGui::Text("%s", str.c_str());
            }
        }
        ImGui::EndTable();
    }
    ImGui::End();
}
