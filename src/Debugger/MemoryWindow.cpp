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

void MemoryWindow::setBaseAddr(uint32_t baseAddr)
{
    m_baseAddr = baseAddr;
}

void MemoryWindow::setReadOnly(bool readOnly)
{
    m_memEdit.ReadOnly = readOnly;
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
    m_memEdit.DrawWindow(title, data, size, baseAddr);
}
