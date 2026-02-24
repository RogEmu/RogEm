#include "System.hpp"
#include "StateBuffer.hpp"

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
#include "SerialInterface.hpp"

static constexpr uint32_t SAVESTATE_MAGIC = 0x524F4745;
static constexpr uint32_t SAVESTATE_VERSION = 1;

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
        spdlog::info("System: Loaded PSX-EXE file successfuly");
    } else {
        spdlog::error("System: Error while loading PSX-EXE file");
    }
}

void System::updatePadInputs(uint16_t buttonsPort)
{
    m_bus->getDevice<SerialInterface>()->getPad(0).updateButtons(buttonsPort);
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

bool System::saveState(const std::string &path)
{
    StateBuffer buf;

    buf.write(SAVESTATE_MAGIC);
    buf.write(SAVESTATE_VERSION);

    m_cpu->serialize(buf);
    m_bus->serialize(buf);

    std::ofstream file(path, std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        spdlog::error("System: Cannot open file \"{}\" for saving state", path);
        return false;
    }
    file.write(reinterpret_cast<const char *>(buf.data().data()), buf.size());
    if (file.fail()) {
        spdlog::error("System: Failed to write state to \"{}\"", path);
        return false;
    }
    file.close();
    spdlog::info("System: State saved to \"{}\" ({} bytes)", path, buf.size());
    return true;
}

bool System::loadState(const std::string &path)
{
    std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        spdlog::error("System: Cannot open file \"{}\" for loading state", path);
        return false;
    }

    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> data(fileSize);
    file.read(reinterpret_cast<char *>(data.data()), fileSize);
    if (file.fail()) {
        spdlog::error("System: Failed to read state from \"{}\"", path);
        return false;
    }
    file.close();

    StateBuffer buf;
    buf.setData(std::move(data));

    uint32_t magic = 0;
    uint32_t version = 0;
    buf.read(magic);
    buf.read(version);

    if (magic != SAVESTATE_MAGIC) {
        spdlog::error("System: Invalid savestate file (bad magic: 0x{:08X})", magic);
        return false;
    }
    if (version != SAVESTATE_VERSION) {
        spdlog::error("System: Incompatible savestate version (expected {}, got {})",
                       SAVESTATE_VERSION, version);
        return false;
    }

    m_cpu->deserialize(buf);
    m_bus->deserialize(buf);

    spdlog::info("System: State loaded from \"{}\"", path);
    return true;
}
