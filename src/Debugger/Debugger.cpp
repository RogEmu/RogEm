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
#include "BreakpointWindow.hpp"
#include "LogWindow.hpp"

Debugger::Debugger(System *system) :
    m_system(system),
    m_paused(true),
    m_simSpeed(1.0f)
{
    m_windows.emplace_back(std::make_unique<RegisterWindow>(this));
    m_windows.emplace_back(std::make_unique<AssemblyWindow>(this));
    m_windows.emplace_back(std::make_unique<BreakpointWindow>(this));
    m_windows.emplace_back(std::make_unique<LogWindow>(this));
    auto biosMemoryWindow = std::make_unique<MemoryWindow>(this);
    biosMemoryWindow->setBaseAddr(0xBFC00000);
    biosMemoryWindow->setTitle("BIOS");
    biosMemoryWindow->setReadOnly(true);
    m_windows.push_back(std::move(biosMemoryWindow));

    auto ramMemoryWindow = std::make_unique<MemoryWindow>(this);
    ramMemoryWindow->setBaseAddr(0);
    ramMemoryWindow->setTitle("RAM");
    m_windows.push_back(std::move(ramMemoryWindow));
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

std::vector<uint8_t> *Debugger::memoryRange(uint32_t addr)
{
    auto slice = m_system->getBus()->getMemoryRange(addr);

    return slice;
}

void Debugger::addBreakpoint(uint32_t addr, BreakpointType type, const std::string &label)
{
    m_breakpoints.push_back({addr, type, label, true});
}

long Debugger::getBreakpointIndex(uint32_t addr)
{
    for (int i = 0; i < static_cast<int>(m_breakpoints.size()); i++) {
        if (m_breakpoints[i].addr == addr) {
            return i;
        }
    }
    return -1;
}

bool Debugger::isBreakpointEnabled(long index)
{
    if (index < 0 || index >= static_cast<long>(m_breakpoints.size())) {
        return false;
    }
    return m_breakpoints[index].enabled;
}

void Debugger::enableBreakpoint(long index, bool enable)
{
    if (index < 0 || index >= static_cast<long>(m_breakpoints.size())) {
        return;
    }
    m_breakpoints[index].enabled = enable;
}

void Debugger::removeBreakpoint(long index)
{
    if (index < 0 || index >= static_cast<long>(m_breakpoints.size())) {
        return;
    }
    m_breakpoints.erase(m_breakpoints.begin() + index);
}

std::vector<Breakpoint> &Debugger::getBreakpoints()
{
    return m_breakpoints;
}

void Debugger::update()
{
    uint32_t pc = getSpecialReg((uint8_t)SpecialRegIndex::PC);
    for (const auto &bp : m_breakpoints) {
        if (bp.enabled && bp.addr == pc) {
            m_paused = true;
            break;
        }
    }
}

void Debugger::draw()
{
    ImGui::ShowDemoWindow();
    for (auto &subwin : m_windows)
    {
        subwin->update();
    }
}

void Debugger::pause(bool pause)
{
    m_paused = pause;
}
