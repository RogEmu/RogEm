/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** SerialInterface
*/

#include "SerialInterface.hpp"

#include <spdlog/spdlog.h>

#include "MemoryMap.hpp"
#include "Bus.hpp"
#include "InterruptController.hpp"

SerialInterface::SerialInterface(Bus *bus) :
    PsxDevice(bus)
{
    m_memoryRange = MemoryMap::SERIAL_PORT_RANGE;
}

SerialInterface::~SerialInterface()
{
}

void SerialInterface::update(int cycles)
{
    m_sio0.update(cycles);

    if (m_sio0.irq()) {
        auto irqc = m_bus->getDevice<InterruptController>();
        irqc->triggerIRQ(DeviceIRQ::CONTROLLER_MEMCARD);
    }
}

void SerialInterface::reset()
{
    m_sio0.reset();
}

void SerialInterface::write8(uint8_t value, uint32_t address)
{
    spdlog::debug("SerialInterface: Write byte 0x{:02X} to 0x{:04X}", value, address);
    write16(static_cast<uint16_t>(value), address);
}

void SerialInterface::write16(uint16_t value, uint32_t address)
{
    uint32_t mappedAddr = mapAddress(address);
    uint8_t channel = (mappedAddr >> 4) & 1;
    uint8_t offset = mappedAddr & 0xF;

    if (channel) {
        writeSIO1(value, address);
        return;
    }
    m_sio0.write(static_cast<SIORegister>(offset), value);
}

void SerialInterface::write32(uint32_t value, uint32_t address)
{
    spdlog::debug("SerialInterface: Write word 0x{:08X} to 0x{:04X}", value, address);
    write16(static_cast<uint16_t>(value & 0xFFFF), address);
}

uint8_t SerialInterface::read8(uint32_t address)
{
    spdlog::debug("SerialInterface: Read byte at 0x{:04X}", address);
    return static_cast<uint8_t>(read16(address));
}

uint16_t SerialInterface::read16(uint32_t address)
{
    uint32_t mappedAddr = mapAddress(address);
    uint8_t channel = (mappedAddr >> 4) & 1;
    uint8_t offset = mappedAddr & 0xF;

    if (channel) {
        return readSIO1(address);
    }
    return m_sio0.read(static_cast<SIORegister>(offset));
}

uint32_t SerialInterface::read32(uint32_t address)
{
    spdlog::debug("SerialInterface: Read word at 0x{:04X}", address);
    return static_cast<uint32_t>(read16(address));
}

void SerialInterface::writeSIO1(uint16_t /* value */, uint32_t address)
{
    spdlog::error("SerialInterface: Write to unsupported SIO1 at 0x{:04X}", address);
}

uint16_t SerialInterface::readSIO1(uint32_t address)
{
    spdlog::error("SerialInterface: Read from unsupported SIO1 at 0x{:04X}", address);
    return 0x7;
}

