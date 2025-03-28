#include "AssemblyWindow.hpp"
#include "imgui.h"
#include <fmt/format.h>

#include "CPU.h"
#include "Disassembler.h"
#include "Debugger.hpp"

AssemblyWindow::AssemblyWindow(Debugger *debugger) :
    m_debugger(debugger)
{
}

AssemblyWindow::~AssemblyWindow()
{
}

void AssemblyWindow::update()
{
    ImGui::Begin("Assembly");

    drawTopBar();
    drawAssembly();
}

void AssemblyWindow::drawTopBar()
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

void AssemblyWindow::drawAssembly()
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

    ImGuiListClipper clipper;
    clipper.Begin(size / 4 + 1);
    while (clipper.Step())
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        {
            ImGui::TableNextColumn();
            uint32_t currentAddr = startAddr + i * 4;
            drawAssemblyLine(currentAddr);
        }
    }
    ImGui::EndTable();
    ImGui::End();
}

void AssemblyWindow::drawAssemblyLine(uint32_t addr)
{
    uint32_t pc = m_debugger->getSpecialReg((uint8_t)SpecialRegIndex::PC);
    ImColor lineColor(IM_COL32_WHITE);
    uint32_t currenInstruction = m_debugger->readWord(addr);

    if (addr == pc)
        lineColor = ImColor(255, 209, 25);
    ImGui::TextColored(lineColor, "0x%08x", addr);
    ImGui::TableNextColumn();
    ImGui::TextColored(lineColor, "%s", Disassembler::formatAsHexBytes(currenInstruction).c_str());
    ImGui::TableNextColumn();
    ImGui::TextColored(lineColor, "%s", Disassembler::disassemble(pc, currenInstruction).c_str());
}
