#include "MemoryWindow.hpp"
#include <fmt/format.h>

#include "Core/BIOS.hpp"

MemoryWindow::MemoryWindow(Debugger *debugger) :
    m_debugger(debugger),
    m_baseAddr(0)
{
}

MemoryWindow::~MemoryWindow()
{
}

void MemoryWindow::update()
{
    drawEditor(getTitleChar(), m_baseAddr);
}

void MemoryWindow::setBaseAddr(uint32_t baseAddr)
{
    m_baseAddr = baseAddr;
}

void MemoryWindow::setReadOnly(bool readOnly)
{
    m_memEdit.ReadOnly = readOnly;
}

MemoryEditor* MemoryWindow::getMemoryEditor()
{
    return &m_memEdit;
}

void MemoryWindow::requestFocus()
{
    m_wantsFocus = true;
}

void MemoryWindow::gotoAddress(uint32_t addr)
{
    m_memEdit.GotoAddrAndHighlight(addr - m_baseAddr, addr + 4 - m_baseAddr);
}

void MemoryWindow::drawEditor(const char *title, uint32_t baseAddr)
{
    size_t size = 0;
    void *data = nullptr;
    auto slice = m_debugger->memoryRange(baseAddr);

    if (slice)
    {
        data = static_cast<void *>(slice->data());
        size = slice->size();
    }
    if (m_wantsFocus)
    {
        ImGui::SetNextWindowFocus();
        m_wantsFocus = false;
    }

    m_memEdit.DrawWindow(title, data, size, baseAddr);
}
