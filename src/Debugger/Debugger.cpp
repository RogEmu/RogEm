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
#include "System.hpp"

Debugger::Debugger(System *system) :
    m_system(system),
    m_paused(false),
    m_simSpeed(1.0f)
{
    // m_windows.push_back(std::make_unique<RegisterWindow>(cpu));
    // m_windows.emplace_back(std::make_unique<MemoryWindow>(this));
    m_windows.emplace_back(std::make_unique<InstructionWindow>(this));
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

void Debugger::stepOver()
{
    m_system->getCPU()->step();
}

uint32_t Debugger::getPc() const
{
    return m_system->getCPU()->pc;
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
