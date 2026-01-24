#include "System.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <spdlog/spdlog.h>
#include <chrono>
#include <thread>

#include "PsxExecutable.hpp"
#include "GPU.hpp"
#include "InterruptController.hpp"
#include "RAM.hpp"

System::System() :
    m_bus(nullptr),
    m_cpu(nullptr),
    m_state(SystemState::RUNNING),
    m_executablePath("")
{
}

System::~System()
{
}

CPU *System::getCPU()
{
    return m_cpu.get();
}

Bus *System::getBus()
{
    return m_bus.get();
}

void System::setExecutablePath(const std::string &path)
{
    m_executablePath = path;
}

int System::init()
{
    m_bus = std::make_unique<Bus>();
    m_cpu = std::make_unique<CPU>(m_bus.get());
    m_bus->connectCpu(m_cpu.get());
    return 0;
}

void System::tick()
{
    if (m_cpu->getReg(CpuReg::PC) == 0x80030000 && !m_executablePath.empty()) {
        loadExecutable(m_executablePath.c_str());
    }
    m_cpu->step();
    m_bus->updateDevices(2);
    if (m_debuggerCallback) {
        m_debuggerCallback();
    }
    if (m_cpu->getTtyOutputFlag()) {
        auto output = m_cpu->getTtyOutput();
        if (m_ttyCallback) {
            m_ttyCallback(output);
        }
    }
}

void System::update()
{
    const int cpuFreq = 33868800;
    const int cyclesPerFrame = cpuFreq / 60;
    int cycles = 0;

    while (cycles < cyclesPerFrame) {
        if (m_state == SystemState::RUNNING) {
            tick();
        }
        cycles += 2;
    }
    auto irqc = m_bus->getDevice<InterruptController>();
    irqc->triggerIRQ(DeviceIRQ::VBLANK);
}

void System::reset()
{
    m_cpu->reset();
    m_bus->reset();
}

void System::loadExecutable(const char *path)
{
    PsxExecutable exe(path);

    auto ram = m_bus->getDevice<RAM>();

    if (exe.load()) {
        m_cpu->setReg(CpuReg::PC, exe.initialPc);
        m_cpu->setReg(CpuReg::GP, exe.initialGp);
        m_cpu->setReg(CpuReg::SP, exe.initialSpBase);
        m_cpu->setReg(CpuReg::FP, exe.initialSpBase);
        ram->loadExecutable(exe.ramDestination, exe.exeData);
        spdlog::info("Loaded PSX-EXE file successfuly");
    } else {
        spdlog::error("Error while loading PSX-EXE file");
    }
}

void System::loadBios(const char *path)
{
    BIOS *bios = m_bus->getDevice<BIOS>();
    bios->loadFromFile(path);
}

void System::setDebuggerCallback(const std::function<void()> &callback)
{
    m_debuggerCallback = callback;
}

void System::setTtyCallback(const std::function<void(const std::string &)> &callback)
{
    m_ttyCallback = callback;
}
