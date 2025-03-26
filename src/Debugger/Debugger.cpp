#include <iostream>

#ifdef _WIN32
    #include <Windows.h>
#endif // _WIN32

#include <GL/gl.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <fmt/format.h>
#include <sstream>
#include <vector>
#include <string>

#include "Debugger.hpp"
#include "CPU.h"
#include "Disassembler.h"

Debugger::Debugger(const std::shared_ptr<CPU> &cpu) :
    m_cpu(cpu),
    m_systemPaused(false),
    m_simSpeed(1.0f)
{
    m_windows.push_front(std::make_shared<RegisterWindow>(cpu));
    m_windows.push_front(std::make_shared<MemoryWindow>(cpu));
    m_windows.push_front(std::make_shared<InstructionWindow>(cpu, m_systemPaused, m_simSpeed));

}

Debugger::~Debugger()
{
}

bool Debugger::isPaused() const
{
    return m_systemPaused;
}

void Debugger::update()
{
    if (m_systemPaused == false)
    {
        for (auto &window : m_windows)
        {
            window->update();
        }
    }
}
