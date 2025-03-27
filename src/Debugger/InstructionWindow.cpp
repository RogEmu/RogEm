#include "InstructionWindow.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <fmt/format.h>

#include "CPU.h"
#include "Disassembler.h"
#include "Debugger.hpp"

InstructionWindow::InstructionWindow(Debugger *debugger) :
    m_debugger(debugger)
{
}

InstructionWindow::~InstructionWindow()
{
}

void InstructionWindow::update()
{
    ImGui::Begin("Assembly");

    drawTopBar();
    drawAssembly();
}

void InstructionWindow::drawTopBar()
{
    bool paused = m_debugger->isPaused();
    float simSpeed = m_debugger->getSimulationSpeed();

    ImGui::BeginGroup();
    ImGui::Checkbox("Pause", &paused);
    ImGui::SameLine();
    if (ImGui::ArrowButton("##CPU_Step_Fw", ImGuiDir_Right) && paused)
    {
        m_debugger->stepOver();
    }
    ImGui::EndGroup();
    ImGui::SliderFloat("Simulation Speed", &simSpeed, 0.00001f, 1.0f, "%.5f");

    m_debugger->pause(paused);
    m_debugger->setSimulationSpeed(simSpeed);
}

void InstructionWindow::drawAssembly()
{
    auto tableFlags = ImGuiTableFlags_RowBg
                    | ImGuiTableFlags_Borders
                    | ImGuiTableFlags_ScrollY
                    | ImGuiTableFlags_Resizable;

    if (!ImGui::BeginTable("Assembly", 3, tableFlags))
        return;

    ImGui::TableSetupScrollFreeze(0, 1);
    ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize);
    ImGui::TableSetupColumn("Bytecode", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize);
    ImGui::TableSetupColumn("Assembly");
    ImGui::TableHeadersRow();

    uint32_t startAddr = 0xBFC00000;
    uint32_t size = 0x7FFFF;
    uint32_t pc = m_debugger->getPc();

    ImGuiListClipper clipper;
    clipper.Begin(size / 4 + 1);
    while (clipper.Step())
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        {
            ImGui::TableNextColumn();
            uint32_t currentAddr = startAddr + i * 4;

            if (currentAddr == pc)
                ImGui::TextColored(ImVec4(255, 200, 0, 255), "0x%08x", currentAddr);
            else
                ImGui::Text("0x%08x", currentAddr);
            ImGui::TableNextColumn();
            ImGui::TableNextColumn();
        }
    }
    ImGui::EndTable();
    ImGui::End();
}
