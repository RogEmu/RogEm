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

Bus::Bus() :
    m_cacheControl(0),
    m_cpu(nullptr)
{
    m_devices[PsxDeviceType::BIOS] = std::make_unique<BIOS>(this);
    m_devices[PsxDeviceType::RAM] = std::make_unique<RAM>(this);
    m_devices[PsxDeviceType::Scratchpad] = std::make_unique<ScratchPad>(this);
    m_devices[PsxDeviceType::DMA] = std::make_unique<DMA>(this);
    m_devices[PsxDeviceType::GPU] = std::make_unique<GPU>(this);
    m_devices[PsxDeviceType::SPU] = std::make_unique<SPU>(this);
    m_devices[PsxDeviceType::Timers] = std::make_unique<Timers>(this);
    m_devices[PsxDeviceType::IRQController] = std::make_unique<InterruptController>(this);
    m_devices[PsxDeviceType::MemControl1] = std::make_unique<MemoryControl1>(this);
}

Bus::~Bus()
{
}

uint32_t Bus::loadWord(uint32_t addr) const
{
    uint32_t pAddress = MemoryMap::mapAddress(addr);

    if (addr % 4 != 0)
    {
        spdlog::error("Unaligned word load");
        return 0;
    }

    for (auto &[_, device] : m_devices) {
        if (device->isAddressed(pAddress)) {
            return device->read32(pAddress);
        }
    }
    // Check if addressing the GPU Stat Register
    // Doesn't trigger ResetCallback: Remove _96 .. when value is 0xFFFFFFFF ??
    // if (addr == 0x1F801814) {
    //     return 0x10000000;
    // }
    if (MemoryMap::CACHE_CONTROL_RANGE.contains(pAddress))
    {
        return m_cacheControl;
    }
    if (MemoryMap::EXP2_RANGE.contains(pAddress))
    {
        spdlog::warn("Read word from Expansion Region 2 (0x{:08x}): Not implemented", addr);
        return 0;
    }
    return 0;
}

void Bus::storeWord(uint32_t addr, uint32_t value)
{
    uint32_t pAddress = MemoryMap::mapAddress(addr);

    if (addr % 4 != 0)
    {
        spdlog::error("Unaligned word store");
        return;
    }

    for (auto &[_, device] : m_devices) {
        if (device->isAddressed(pAddress)) {
            device->write32(value, pAddress);
            return;
        }
    }
    if (MemoryMap::CACHE_CONTROL_RANGE.contains(pAddress))
    {
        m_cacheControl = value;
    }
    else if (MemoryMap::EXP2_RANGE.contains(pAddress))
    {
        spdlog::warn("Write word 0x{:08X} to Expansion Region 2 (0x{:08X}): Not implemented", value, addr);
    }
    else
    {
        spdlog::error("Address 0x{:08X} is not supported", addr);
    }
}

uint16_t Bus::loadHalfWord(uint32_t addr) const
{
    uint32_t pAddress = MemoryMap::mapAddress(addr);

    if (addr % 2 != 0)
    {
        spdlog::error("Unaligned halfword load");
        return 0;
    }

    for (auto &[_, device] : m_devices) {
        if (device->isAddressed(pAddress)) {
            return device->read16(pAddress);
        }
    }
    if (MemoryMap::CACHE_CONTROL_RANGE.contains(pAddress))
    {
        spdlog::warn("Read halfword from Cache Control Registers (0x{:08x}): Not implemented", addr);
        return 0;
    }
    if (MemoryMap::EXP2_RANGE.contains(pAddress))
    {
        spdlog::warn("Read halfword from Expansion Region 2 (0x{:08x}): Not implemented", addr);
        return 0;
    }
    return 0;
}

void Bus::storeHalfWord(uint32_t addr, uint16_t value)
{
    uint32_t pAddress = MemoryMap::mapAddress(addr);

    if (addr % 2 != 0)
    {
        spdlog::error("Unaligned halfword store");
        return;
    }

    for (auto &[_, device] : m_devices) {
        if (device->isAddressed(pAddress)) {
            device->write16(value, pAddress);
            return;
        }
    }
    if (MemoryMap::CACHE_CONTROL_RANGE.contains(pAddress))
    {
        spdlog::warn("Write halfword 0x{:04X} to Cache Control Registers (0x{:08X}): Not implemented", value, addr);
    }
    else if (MemoryMap::EXP2_RANGE.contains(pAddress))
    {
        spdlog::warn("Write halfword 0x{:04X} to Expansion Region 2 (0x{:08X}): Not implemented", value, addr);
    }
    else
    {
        spdlog::error("Address 0x{:08X} is not supported", addr);
    }
}

uint8_t Bus::loadByte(uint32_t addr) const
{
    uint32_t pAddress = MemoryMap::mapAddress(addr);

    for (auto &[_, device] : m_devices) {
        if (device->isAddressed(pAddress)) {
            return device->read8(pAddress);
        }
    }
    if (MemoryMap::CACHE_CONTROL_RANGE.contains(pAddress))
    {
        spdlog::warn("Read byte from Cache Control Registers (0x{:08x}): Not implemented", addr);
        return 0;
    }
    if (MemoryMap::EXP2_RANGE.contains(pAddress))
    {
        spdlog::warn("Read byte from Expansion Region 2 (0x{:08x}): Not implemented", addr);
        return 0;
    }
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
    if (MemoryMap::CACHE_CONTROL_RANGE.contains(pAddress))
    {
        spdlog::warn("Write byte 0x{:02X} to Cache Control Registers (0x{:08X}): Not implemented", value, addr);
    }
    else if (MemoryMap::EXP2_RANGE.contains(pAddress))
    {
        spdlog::warn("Write byte 0x{:02X} to Expansion Region 2 (0x{:08X}): Not implemented", value, addr);
    }
    else
    {
        spdlog::error("Address 0x{:08X} is not supported", addr);
    }
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

PsxDevice *Bus::getDevice(PsxDeviceType deviceType)
{
    auto it = m_devices.find(deviceType);
    if (it != m_devices.end()) {
        return it->second.get();
    }
    return nullptr;
}

void Bus::connectCpu(CPU *cpu)
{
    m_cpu = cpu;
}

CPU *Bus::getCpu()
{
    return m_cpu;
}
