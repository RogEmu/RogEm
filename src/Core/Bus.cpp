/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** Bus
*/

#include "Bus.hpp"
#include "StateBuffer.hpp"

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
#include "SerialInterface.hpp"
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
    addDevice(std::make_unique<SerialInterface>(this));
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

void Bus::reset()
{
    for (auto &[_, device] : m_devices) {
        device->reset();
    }
}

void Bus::serialize(StateBuffer &buf) const
{
    buf.write(m_cacheControl);

    const std::type_index order[] = {
        std::type_index(typeid(RAM)),
        std::type_index(typeid(ScratchPad)),
        std::type_index(typeid(GPU)),
        std::type_index(typeid(DMA)),
        std::type_index(typeid(SPU)),
        std::type_index(typeid(SerialInterface)),
        std::type_index(typeid(Timers)),
        std::type_index(typeid(InterruptController)),
        std::type_index(typeid(MemoryControl1)),
        std::type_index(typeid(MemoryControl2)),
        std::type_index(typeid(CacheControl)),
        std::type_index(typeid(Expansion2)),
    };

    for (const auto &ti : order) {
        auto it = m_devices.find(ti);
        if (it != m_devices.end()) {
            it->second->serialize(buf);
        }
    }
}

void Bus::deserialize(StateBuffer &buf)
{
    buf.read(m_cacheControl);

    const std::type_index order[] = {
        std::type_index(typeid(RAM)),
        std::type_index(typeid(ScratchPad)),
        std::type_index(typeid(GPU)),
        std::type_index(typeid(DMA)),
        std::type_index(typeid(SPU)),
        std::type_index(typeid(SerialInterface)),
        std::type_index(typeid(Timers)),
        std::type_index(typeid(InterruptController)),
        std::type_index(typeid(MemoryControl1)),
        std::type_index(typeid(MemoryControl2)),
        std::type_index(typeid(CacheControl)),
        std::type_index(typeid(Expansion2)),
    };

    for (const auto &ti : order) {
        auto it = m_devices.find(ti);
        if (it != m_devices.end()) {
            it->second->deserialize(buf);
        }
    }
}

uint32_t Bus::loadWord(uint32_t addr) const
{
    uint32_t pAddress = MemoryMap::mapAddress(addr);

    if (addr % 4 != 0)
    {
        spdlog::error("Bus: Unaligned word load");
        return 0;
    }

    for (auto &[_, device] : m_devices) {
        if (device->isAddressed(pAddress)) {
            return device->read32(pAddress);
        }
    }
    spdlog::error("Bus: Read word at address 0x{:08X} is not supported", addr);
    return 0;
}

void Bus::storeWord(uint32_t addr, uint32_t value)
{
    uint32_t pAddress = MemoryMap::mapAddress(addr);

    if (addr % 4 != 0)
    {
        spdlog::error("Bus: Unaligned word store");
        return;
    }

    for (auto &[_, device] : m_devices) {
        if (device->isAddressed(pAddress)) {
            device->write32(value, pAddress);
            return;
        }
    }
    spdlog::error("Bus: Write word at address 0x{:08X} is not supported", addr);
}

uint16_t Bus::loadHalfWord(uint32_t addr) const
{
    uint32_t pAddress = MemoryMap::mapAddress(addr);

    if (addr % 2 != 0)
    {
        spdlog::error("Bus: Unaligned halfword load");
        return 0;
    }

    for (auto &[_, device] : m_devices) {
        if (device->isAddressed(pAddress)) {
            return device->read16(pAddress);
        }
    }
    spdlog::error("Bus: Read halfword at address 0x{:08X} is not supported", addr);
    return 0;
}

void Bus::storeHalfWord(uint32_t addr, uint16_t value)
{
    uint32_t pAddress = MemoryMap::mapAddress(addr);

    if (addr % 2 != 0)
    {
        spdlog::error("Bus: Unaligned halfword store");
        return;
    }

    for (auto &[_, device] : m_devices) {
        if (device->isAddressed(pAddress)) {
            device->write16(value, pAddress);
            return;
        }
    }
    spdlog::error("Bus: Write halfword at address 0x{:08X} is not supported", addr);
}

uint8_t Bus::loadByte(uint32_t addr) const
{
    uint32_t pAddress = MemoryMap::mapAddress(addr);

    for (auto &[_, device] : m_devices) {
        if (device->isAddressed(pAddress)) {
            return device->read8(pAddress);
        }
    }
    spdlog::error("Bus: Read byte at address 0x{:08X} is not supported", addr);
    return 0;
}

void Bus::storeByte(uint32_t addr, uint8_t value)
{
    uint32_t pAddress = MemoryMap::mapAddress(addr);

    for (auto &[_, device] : m_devices) {
        if (device->isAddressed(pAddress)) {
            device->write8(value, pAddress);
            return;
        }
    }
    spdlog::error("Bus: Write byte at address 0x{:08X} is not supported", addr);
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
