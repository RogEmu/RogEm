/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** Bus
*/

#include "Bus.hpp"

#include <spdlog/spdlog.h>

#include "CPU.hpp"
#include "MemoryMap.hpp"
#include "Memory.hpp"
#include "RAM.hpp"
#include "BIOS.hpp"
#include "ScratchPad.hpp"
#include "GPU.hpp"
#include "DMA.hpp"
#include "SPU.hpp"
#include "Timers.hpp"
#include "InterruptController.hpp"
#include "MemoryControl1.hpp"
#include "CacheControl.hpp"
#include "Expansion2.hpp"
#include "MemoryControl2.hpp"

Bus::Bus() :
    m_cacheControl(0),
    m_cpu(nullptr)
{
    addDevice(std::make_unique<BIOS>(this));
    addDevice(std::make_unique<RAM>(this));
    addDevice(std::make_unique<ScratchPad>(this));
    addDevice(std::make_unique<DMA>(this));
    addDevice(std::make_unique<GPU>(this));
    addDevice(std::make_unique<SPU>(this));
    addDevice(std::make_unique<Timers>(this));
    addDevice(std::make_unique<InterruptController>(this));
    addDevice(std::make_unique<MemoryControl1>(this));
    addDevice(std::make_unique<CacheControl>(this));
    addDevice(std::make_unique<Expansion2>(this));
    addDevice(std::make_unique<MemoryControl2>(this));
}

Bus::~Bus()
{
}

std::vector<uint8_t> *Bus::getMemoryRange(uint32_t addr)
{
    auto mappedAddress = MemoryMap::mapAddress(addr);

    for (auto &[_, device] : m_devices) {
        if (device->isAddressed(mappedAddress)) {
            auto memoryDev = dynamic_cast<Memory *>(device.get());
            if (memoryDev) {
                return memoryDev->data();
            }
        }
    }
    return nullptr;
}

const std::vector<uint8_t> *Bus::getMemoryRange(uint32_t addr) const
{
    auto mappedAddress = MemoryMap::mapAddress(addr);

    for (auto &[_, device] : m_devices) {
        if (device->isAddressed(mappedAddress)) {
            auto memoryDev = dynamic_cast<Memory *>(device.get());
            if (memoryDev) {
                return memoryDev->data();
            }
        }
    }
    return nullptr;
}

void Bus::updateDevices(int cycles)
{
    for (auto &[_, device] : m_devices) {
        device->update(cycles);
    }
}

void Bus::connectCpu(CPU *cpu)
{
    m_cpu = cpu;
}

CPU *Bus::getCpu()
{
    return m_cpu;
}
