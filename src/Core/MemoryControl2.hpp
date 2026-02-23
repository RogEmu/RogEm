#ifndef MEMORYCONTROL2_HPP_
#define MEMORYCONTROL2_HPP_

#include "PsxDevice.hpp"

class StateBuffer;

class MemoryControl2 : public PsxDevice
{
    public:
        MemoryControl2(Bus *bus);
        ~MemoryControl2();

        void serialize(StateBuffer &buf) const override;
        void deserialize(StateBuffer &buf) override;

        void write8(uint8_t value, uint32_t address) override;
        void write16(uint16_t value, uint32_t address) override;
        void write32(uint32_t value, uint32_t address) override;

        uint8_t read8(uint32_t address) override;
        uint16_t read16(uint32_t address) override;
        uint32_t read32(uint32_t address) override;

    private:
        uint32_t m_memControl2;
};

#endif /* !MEMORYCONTROL2_HPP_ */
