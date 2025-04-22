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
#include "SettingsWindow.hpp"

Debugger::Debugger(System *system) :
    m_system(system),
    m_paused(true),
    m_simSpeed(1.0f),
    m_resumeOnBreakpoint(false)
{
    m_windows.emplace_back(std::make_unique<RegisterWindow>(this));
    m_windows.emplace_back(std::make_unique<AssemblyWindow>(this));
    m_windows.emplace_back(std::make_unique<BreakpointWindow>(this));
    m_windows.emplace_back(std::make_unique<LogWindow>(this));
    m_windows.emplace_back(std::make_unique<SettingsWindow>(this));
    auto biosMemoryWindow = std::make_unique<MemoryWindow>(this);
    biosMemoryWindow->setBaseAddr(0xBFC00000);
    biosMemoryWindow->setTitle("BIOS");
    biosMemoryWindow->setReadOnly(true);
    m_windows.push_back(std::move(biosMemoryWindow));

    auto ramMemoryWindow = std::make_unique<MemoryWindow>(this);
    ramMemoryWindow->setBaseAddr(0);
    ramMemoryWindow->setTitle("RAM");
    m_windows.push_back(std::move(ramMemoryWindow));

    loadBreakpointsFromFile();
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

void Debugger::CPUReset()
{
    m_system->getCPU()->reset();
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

//The following functions are required for nlohmann-json to work, but are called automatically upon conversion
void to_json(nlohmann::json& j, const Breakpoint& b)
{
    j = nlohmann::json {
        {"addr", b.addr},
        {"instructionType", static_cast<int>(b.instructionType)},
        {"label", b.label},
        {"enabled", b.enabled}
    };
}

void from_json(const nlohmann::json& j, Breakpoint& b)
{
    b.addr = j.at("addr").get<uint32_t>();
    b.instructionType = static_cast<BreakpointType>(j.at("instructionType").get<int>());
    b.label = j.at("label").get<std::string>();
    b.enabled = j.at("enabled").get<bool>();
}

void Debugger::addBreakpoint(uint32_t addr, BreakpointType type, const std::string &label, bool isRunTo)
{
    m_breakpoints.push_back({addr, type, label, true, isRunTo});
    saveBreakpointsToFile();
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

void Debugger::toggleBreakpoint(long index, bool enable)
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
    saveBreakpointsToFile();
}

void Debugger::saveBreakpointsToFile()
{
    std::ofstream file(breakpointsFilePath);
    if (file.is_open()) {
        nlohmann::json j = m_breakpoints;
        file << j.dump(4);
    }
}

void Debugger::loadBreakpointsFromFile()
{
    std::ifstream file(breakpointsFilePath);
    if (file.is_open()) {
        nlohmann::json j;
        file >> j;
        m_breakpoints = j.get<std::vector<Breakpoint>>();
    }
}

void Debugger::setBreakpoint(uint32_t addr, BreakpointType type, const std::string &label, bool enabled, bool isRunTo)
{
    auto it = std::find_if(m_breakpoints.begin(), m_breakpoints.end(), [addr](const Breakpoint &bp) {
        return bp.addr == addr;
    });
    if (it != m_breakpoints.end()) {
        it->enabled = enabled;
        it->instructionType = type;
        it->label = label;
        it->isRunTo = isRunTo;
    } else {
        m_breakpoints.push_back({addr, type, label, enabled, isRunTo});
    }
}

std::vector<Breakpoint> &Debugger::getBreakpoints()
{
    return m_breakpoints;
}

void Debugger::setResumeOnBreakpoint(bool resume)
{
    m_resumeOnBreakpoint = resume;
}

void Debugger::update()
{
    uint32_t pc = getSpecialReg((uint8_t)SpecialRegIndex::PC);

    for (auto bp : m_breakpoints) {
        if (bp.enabled){
            if (bp.instructionType == BreakpointType::EXEC) {
                if (bp.addr == pc) {
                    if (bp.isRunTo == true) {
                        removeBreakpoint(getBreakpointIndex(bp.addr));
                    }
                    m_paused = true;
                    break;
                }
            }
        }
    }
    if (m_resumeOnBreakpoint && m_paused) {
        m_paused = false;
        m_resumeOnBreakpoint = false;
    }
}

void Debugger::draw()
{
    for (auto &subwin : m_windows)
    {
        subwin->update();
    }
}

void Debugger::pause(bool pause)
{
    m_paused = pause;
}
