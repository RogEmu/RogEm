#include "AssemblyWindow.hpp"
#include <fmt/format.h>

#include "MemoryMap.hpp"
#include "CPU.h"
#include "Disassembler.h"
#include "Debugger.hpp"

AssemblyWindow::AssemblyWindow(Debugger *debugger) :
    m_jumpToPc(false),
    m_autoFollowPc(false),
    m_pcCursor(0.0f),
    m_debugger(debugger)
{
}

AssemblyWindow::~AssemblyWindow()
{
}

void AssemblyWindow::update()
{
    if (ImGui::Begin("Assembly"))
    {
        drawTopBar();
        drawAssembly();
    }
    ImGui::End();
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
    ImGui::SameLine();
    m_jumpToPc = ImGui::Button("Jump to PC");
    ImGui::SameLine();
    ImGui::Checkbox("Auto follow PC", &m_autoFollowPc);
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

    uint32_t size = (MemoryMap::RAM_RANGE.length + MemoryMap::BIOS_RANGE.length) / 4;

    if (m_jumpToPc || m_autoFollowPc)
    {
        jumpToPC();
    }
    ImGuiListClipper clipper;
    clipper.Begin(size);
    while (clipper.Step())
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        {
            ImGui::TableNextColumn();
            uint32_t currentAddr = getAddressFromLine(i);
            drawAssemblyLine(currentAddr);
        }
    }
    updatePcCursor(clipper.ItemsHeight);
    ImGui::EndTable();
}

void AssemblyWindow::drawAssemblyLine(uint32_t addr)
{
    uint32_t pc = m_debugger->getSpecialReg((uint8_t)SpecialRegIndex::PC);
    ImColor lineColor(IM_COL32_WHITE);
    uint32_t currentInstruction = m_debugger->readWord(addr);

    if (getLineFromAddress(pc) == getLineFromAddress(addr))
    {
        lineColor = ImColor(255, 209, 25);
    }
    ImGui::TextColored(lineColor, "0x%08x", addr);
    ImGui::TableNextColumn();
    ImGui::TextColored(lineColor, "%s", Disassembler::formatAsHexBytes(currentInstruction).c_str());
    ImGui::TableNextColumn();
    ImGui::TextColored(lineColor, "%s", Disassembler::disassemble(addr, currentInstruction).c_str());
}

uint32_t AssemblyWindow::getAddressFromLine(int line)
{
    uint32_t currentAddr = line * 4;

    if (MemoryMap::RAM_RANGE.contains(currentAddr))
    {
        return currentAddr;
    }
    else
    {
        return MemoryMap::BIOS_BASE_KSEG1 + (currentAddr - MemoryMap::RAM_RANGE.length);
    }
}

int AssemblyWindow::getLineFromAddress(uint32_t addr)
{
    auto mappedAddr = MemoryMap::mapAddress(addr);

    if (MemoryMap::RAM_RANGE.contains(mappedAddr))
    {
        return (mappedAddr - MemoryMap::RAM_BASE_KUSEG) / 4;
    }
    else if (MemoryMap::BIOS_RANGE.contains(mappedAddr))
    {
        return (MemoryMap::RAM_RANGE.length + (mappedAddr - MemoryMap::BIOS_BASE_KUSEG)) / 4;
    }
    return -1;
}

void AssemblyWindow::jumpToPC()
{
    ImGui::SetScrollFromPosY(ImGui::GetCursorStartPos().y + m_pcCursor, 0.42f);
}

void AssemblyWindow::updatePcCursor(float itemHeight)
{
    auto pc = m_debugger->getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC));
    auto line = getLineFromAddress(pc);
    m_pcCursor = line * itemHeight;
}
