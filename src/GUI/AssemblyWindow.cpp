#include "AssemblyWindow.hpp"
#include <fmt/format.h>

#include "Core/MemoryMap.hpp"
#include "MemoryWindow.hpp"

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
    uint32_t pc = m_debugger->getCpuReg(CpuReg::PC);
    bool isBreakpoint = (m_debugger->getBreakpointIndex(pc) != -1);

    ImGui::BeginGroup();
    if (ImGui::Checkbox("Pause", &paused)) {
        if (isBreakpoint)
            m_debugger->setResumeOnBreakpoint(true);
        else
            m_debugger->pause(paused);
    }
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

    m_debugger->setSimulationSpeed(simSpeed);
}

void AssemblyWindow::drawAssembly()
{
    auto tableFlags = ImGuiTableFlags_RowBg
                    | ImGuiTableFlags_Borders
                    | ImGuiTableFlags_ScrollY
                    | ImGuiTableFlags_HighlightHoveredColumn
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
        jumpToPC();
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

void AssemblyWindow::drawContextMenu(uint32_t addr, bool isSelected, bool hasBreakpoint)
{
    uint32_t pc = m_debugger->getCpuReg(CpuReg::PC);
    bool isBreakpoint = (m_debugger->getBreakpointIndex(pc) != -1);

    if (isSelected && ImGui::BeginPopupContextItem("BreakpointContextMenu"))
    {
        if (ImGui::MenuItem("Jump to memory address"))
        {
            std::string windowTitle = (addr < 0xBFC00000) ? "RAM" : "BIOS";
            for (auto& window : m_debugger->getWindows()) {
                if (auto memWin = dynamic_cast<MemoryWindow*>(window.get())) {
                    if (memWin->getTitleString() == windowTitle) {
                        memWin->gotoAddress(addr);
                        memWin->requestFocus();
                    }
                }
            }
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Run to address"))
        {
            if (isBreakpoint)
                m_debugger->setResumeOnBreakpoint(true);
            m_debugger->pause(false);
            m_debugger->addBreakpoint(addr, BreakpointType::EXEC, fmt::format("Run to 0x{:08X}", addr), true);
        }
        ImGui::Separator();
        if (hasBreakpoint)
        {
            uint32_t bpIndex = m_debugger->getBreakpointIndex(addr);
            if (m_debugger->isBreakpointEnabled(bpIndex))
            {
                if (ImGui::MenuItem("Disable Breakpoint"))
                {
                    m_debugger->toggleBreakpoint(bpIndex, false);
                }
            }
            else
            {
                if (ImGui::MenuItem("Enable Breakpoint"))
                {
                    m_debugger->toggleBreakpoint(bpIndex, true);
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Remove Breakpoint"))
            {
                m_debugger->removeBreakpoint(bpIndex);
            }
        }
        else
        {
            if (ImGui::MenuItem("Add Breakpoint"))
            {
                m_debugger->addBreakpoint(addr, BreakpointType::EXEC, fmt::format("Breakpoint at 0x{:08X}", addr), false);
            }
        }
        ImGui::EndPopup();
    }
}

void AssemblyWindow::drawAssemblyLine(uint32_t addr)
{
    uint32_t pc = m_debugger->getCpuReg(CpuReg::PC);
    ImColor lineColor(IM_COL32_WHITE);
    uint32_t currentInstruction = m_debugger->readWord(addr);

    const bool isPCLine = (getLineFromAddress(pc) == getLineFromAddress(addr));
    const bool isSelected = (m_selectedAddr == addr);
    const bool hasBreakpoint = (m_debugger->getBreakpointIndex(addr) != -1);

    if (isSelected)
    {
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.6f, 1.0f, 0.4f));         // highlight selected line
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.3f, 0.7f, 1.0f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.4f, 0.8f, 1.0f, 0.9f));
    }

    if (isPCLine)
    {
        lineColor = ImColor(255, 209, 25);
        ImGui::PushStyleColor(ImGuiCol_Text, lineColor.Value); // highlight PC line
    }

    ImGuiSelectableFlags selectable_flags = ImGuiSelectableFlags_SpanAllColumns;

    ImVec2 cursorPos = ImGui::GetCursorScreenPos();

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 16);

    if (ImGui::Selectable(fmt::format("0x{:08X}", addr).c_str(), isSelected, selectable_flags))
        m_selectedAddr = addr;

    if (isSelected)
        ImGui::PopStyleColor(3);

    if (isPCLine)
    ImGui::PopStyleColor();
    if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
    {
        m_selectedAddr = addr;
        ImGui::OpenPopup("BreakpointContextMenu");
    }
    drawContextMenu(addr, isSelected, hasBreakpoint);

    if (hasBreakpoint)
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 circleCenter = ImVec2(cursorPos.x + 8, cursorPos.y + ImGui::GetTextLineHeight() * 0.5f);
        int bpIndex = m_debugger->getBreakpointIndex(addr);
        bool isEnabled = m_debugger->isBreakpointEnabled(bpIndex);

        if (isEnabled)
            draw_list->AddCircleFilled(circleCenter, 5.0f, IM_COL32(255, 50, 50, 255)); // filled red
        else
            draw_list->AddCircle(circleCenter, 5.0f, IM_COL32(255, 50, 50, 255), 16, 2.0f); // outlined red
    }


    if (ImGui::IsItemClicked() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
    {
        int bpIndex = m_debugger->getBreakpointIndex(addr);
        if (bpIndex != -1) {
            bool enabled = m_debugger->isBreakpointEnabled(bpIndex);
            m_debugger->toggleBreakpoint(bpIndex, !enabled);
        }
        else
            m_debugger->addBreakpoint(addr, BreakpointType::EXEC, fmt::format("Breakpoint at 0x{:08X}", addr), false);
    }

    InstructionData idata = m_debugger->getDisassembler().disasm(currentInstruction, addr);
    ImGui::TableNextColumn();
    ImGui::TextColored(lineColor, "%s", idata.bytes.c_str());
    ImGui::TableNextColumn();
    std::string assemblyStr = fmt::format("{:12}{}", idata.mnemonic, idata.operands);
    ImGui::TextColored(lineColor, "%s", assemblyStr.c_str());
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
    auto pc = m_debugger->getCpuReg(CpuReg::PC);
    auto line = getLineFromAddress(pc);
    m_pcCursor = line * itemHeight;
}
