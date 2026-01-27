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

SerialInterface::SerialInterface(Bus *bus) :
    PsxDevice(bus)
{
    m_memoryRange = MemoryMap::SERIAL_PORT_RANGE;
}

SerialInterface::~SerialInterface()
{
}

void SerialInterface::write8(uint8_t value, uint32_t address)
{
    spdlog::debug("SerialInterface: Write byte 0x{:02X} to 0x{:04X}", value, address);
}

void SerialInterface::write16(uint16_t value, uint32_t address)
{
    uint32_t offset = mapAddress(address) & 0xF;

    switch (static_cast<SIORegister>(offset)) {
        case SIORegister::TX_DATA:
            spdlog::debug("SerialInterface: Write TX_DATA = 0x{:04X} at 0x{:04X}", value, address);
            break;
        case SIORegister::MODE:
            spdlog::debug("SerialInterface: Write MODE = 0x{:04X} at 0x{:04X}", value, address);
            break;
        case SIORegister::CTRL:
            spdlog::debug("SerialInterface: Write CTRL = 0x{:04X} at 0x{:04X}", value, address);
            break;
        case SIORegister::BAUD:
            spdlog::debug("SerialInterface: Write BAUD = 0x{:04X} at 0x{:04X}", value, address);
            break;
        default:
            spdlog::debug("SerialInterface: Write halfword 0x{:04X} to 0x{:04X}", value, address);
            break;
    }
}

void SerialInterface::write32(uint32_t value, uint32_t address)
{
    spdlog::debug("SerialInterface: Write word 0x{:08X} to 0x{:04X}", value, address);
    write16(static_cast<uint16_t>(value & 0xFFFF), address);
}

uint8_t SerialInterface::read8(uint32_t address)
{
    spdlog::debug("SerialInterface: Read byte at 0x{:04X}", address);
    return 0;
}

uint16_t SerialInterface::read16(uint32_t address)
{
    uint32_t offset = mapAddress(address) & 0xF;

    switch (static_cast<SIORegister>(offset)) {
        case SIORegister::TX_DATA:
            spdlog::debug("SerialInterface: Read TX_DATA 0x{:04X}", address);
            return 0;
        case SIORegister::STAT:
            spdlog::debug("SerialInterface: Read STAT 0x{:04X}", address);
            return 0x07;
        case SIORegister::MODE:
            spdlog::debug("SerialInterface: Read MODE 0x{:04X}", address);
            return 0;
        case SIORegister::CTRL:
            spdlog::debug("SerialInterface: Read CTRL 0x{:04X}", address);
            return 0;
        case SIORegister::BAUD:
            spdlog::debug("SerialInterface: Read BAUD 0x{:04X}", address);
            return 0;
        default:
            spdlog::debug("SerialInterface: Read halfword at offset 0x{:02X}", offset);
            return 0;
    }
}

uint32_t SerialInterface::read32(uint32_t address)
{
    spdlog::debug("SerialInterface: Read word at 0x{:04X}", address);
    return 0;
}
