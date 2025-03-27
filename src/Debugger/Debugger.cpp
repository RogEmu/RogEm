/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** Debugger
*/

#include <algorithm>

#include "Debugger.hpp"
#include "RegisterWindow.hpp"
#include "InstructionWindow.hpp"
#include "MemoryWindow.hpp"
#include "CPU.h"

Debugger::Debugger(const std::shared_ptr<CPU> &cpu) :
    m_cpu(cpu),
    m_paused(false),
    m_simSpeed(1.0f)
{
    m_windows.push_front(std::make_shared<RegisterWindow>(cpu));
    m_windows.push_front(std::make_shared<MemoryWindow>(cpu));
    m_windows.push_front(std::make_shared<InstructionWindow>(cpu, m_paused, m_simSpeed));
}

Debugger::~Debugger()
{
}

bool Debugger::isPaused() const
{
    return m_paused;
}

float Debugger::getSimulationSpeed() const
{
    return m_simSpeed;
}

void Debugger::setSimulationSpeed(float speed)
{
    m_simSpeed = std::max(speed, 0.0f);
}

void Debugger::update()
{
    for (auto &window : m_windows)
        window->update();
}

void Debugger::pause(bool pause)
{
    m_paused = pause;
}
