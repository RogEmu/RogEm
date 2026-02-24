/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** PsxDevice
*/

#ifndef PSXDEVICE_HPP_
#define PSXDEVICE_HPP_

#include <cstdint>

#include "MemoryMap.hpp"

class Bus;
class StateBuffer;

class PsxDevice
{
    public:
        PsxDevice(Bus *bus) : m_bus(bus) {}
        virtual ~PsxDevice() = default;

        virtual void update(int cycles) { (void)cycles; };
        virtual void reset() {};

        virtual void serialize(StateBuffer &buf) const { (void)buf; }
        virtual void deserialize(StateBuffer &buf) { (void)buf; }

        bool isAddressed(uint32_t address) const {
            return m_memoryRange.contains(address);
        }

        uint32_t mapAddress(uint32_t address) const {
            return m_memoryRange.remap(address);
        }

        virtual void write8(uint8_t data, uint32_t address) = 0;
        virtual void write16(uint16_t data, uint32_t address) = 0;
        virtual void write32(uint32_t data, uint32_t address) = 0;

        virtual uint8_t read8(uint32_t address) = 0;
        virtual uint16_t read16(uint32_t address) = 0;
        virtual uint32_t read32(uint32_t address) = 0;

    protected:
        MemoryMap::MemRange m_memoryRange;
        Bus *m_bus;
};

#endif /* !PSXDEVICE_HPP_ */
