#include "MemoryWindow.hpp"
#include <fmt/format.h>

#include "BIOS.h"
#include "Debugger.hpp"


MemoryWindow::MemoryWindow(Debugger *debugger) :
    m_debugger(debugger),
    m_title(""),
    m_baseAddr(0)
{
}

MemoryWindow::~MemoryWindow()
{
}

void MemoryWindow::update()
{
    drawEditor(m_title, m_baseAddr);
}

void MemoryWindow::setTitle(const char *title)
{
    m_title = title;
}

const char *MemoryWindow::getTitle() const
{
    return m_title;
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

void MemoryWindow::gotoAddress(uint32_t addr)
{
    m_memEdit.GotoAddr = addr - m_baseAddr;
}

void MemoryWindow::drawEditor(const char *title, uint32_t baseAddr)
{
    m_size = 0;
    void *data = nullptr;
    auto slice = m_debugger->memoryRange(baseAddr);

    if (slice)
    {
        data = static_cast<void *>(slice->data());
        m_size = slice->size();
    }
    m_memEdit.DrawWindow(title, data, m_size, baseAddr);
}
