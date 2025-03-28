/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** Debugger
*/

#include <algorithm>

#include "Debugger.hpp"
#include "RegisterWindow.hpp"
#include "AssemblyWindow.hpp"
#include "MemoryWindow.hpp"
#include "CPU.h"
#include "System.hpp"

Debugger::Debugger(System *system) :
    m_system(system),
    m_paused(false),
    m_simSpeed(1.0f)
{
    m_windows.emplace_back(std::make_unique<RegisterWindow>(this));
    m_windows.emplace_back(std::make_unique<MemoryWindow>(this));
    m_windows.emplace_back(std::make_unique<AssemblyWindow>(this));
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

uint32_t Debugger::getGPR(uint8_t reg) const
{
    return m_system->getCPU()->getReg(reg);
}

void Debugger::setGPR(uint8_t reg, uint32_t value)
{
    m_system->getCPU()->setReg(reg, value);
}

uint32_t Debugger::getSpecialReg(uint8_t reg) const
{
    return m_system->getCPU()->getSpecialReg(reg);
}

void Debugger::setSpecialReg(uint8_t reg, uint32_t val)
{
    m_system->getCPU()->setSpecialReg(reg, val);
}

uint32_t Debugger::getCop0Reg(uint8_t reg) const
{
    return m_system->getCPU()->getCop0Reg(reg);
}

void Debugger::setCop0Reg(uint8_t reg, uint32_t val)
{
    m_system->getCPU()->setCop0Reg(reg, val);
}

uint8_t Debugger::readByte(uint32_t addr) const
{
    return m_system->getBus()->loadByte(addr);
}

uint16_t Debugger::readHalfWord(uint32_t addr) const
{
    return m_system->getBus()->loadHalfWord(addr);
}

uint32_t Debugger::readWord(uint32_t addr) const
{
    return m_system->getBus()->loadWord(addr);
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
