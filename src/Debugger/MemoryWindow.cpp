#include "MemoryWindow.hpp"
#include "imgui.h"
#include <fmt/format.h>
#include "imgui/imgui_memory_editor.h"

#include "Debugger.hpp"

MemoryWindow::MemoryWindow(Debugger *debugger) :
    m_debugger(debugger)
{
}

MemoryWindow::~MemoryWindow()
{
}

void MemoryWindow::update()
{
    static MemoryEditor mem_edit;

    const char *data = "Hello guys! This is a test to see if the memory editor is working!";
    mem_edit.DrawWindow("Memory Editor", (void*)data, 67, 0xFFFF);
}
