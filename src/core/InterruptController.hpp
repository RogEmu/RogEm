/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** InterruptController
*/

#ifndef INTERRUPTCONTROLLER_HPP_
#define INTERRUPTCONTROLLER_HPP_

#include "PsxDevice.hpp"

enum class DeviceIRQ
{
    VBLANK = 0x001,
    GPU = 0x002,
    CDROM = 0x004,
    DMA = 0x008,
    TIMER0 = 0x010,
    TIMER1 = 0x020,
    TIMER2 = 0x040,
    CONTROLLER_MEMCARD = 0x080,
    SIO = 0x100,
    SPU = 0x200,
    LIGHTPEN = 0x400
};

class InterruptController : public PsxDevice
{
    public:
        InterruptController(Bus *bus);
        ~InterruptController();

        void triggerIRQ(DeviceIRQ device);

        void write8(uint8_t value, uint32_t address) override;
        void write16(uint16_t value, uint32_t address) override;
        void write32(uint32_t value, uint32_t address) override;

        uint8_t read8(uint32_t address) override;
        uint16_t read16(uint32_t address) override;
        uint32_t read32(uint32_t address) override;

    private:
        bool irqPending();
        void setCpuIrqPending(bool pending);

    private:
        uint32_t m_istat;
        uint32_t m_imask;
};

#endif /* !INTERRUPTCONTROLLER_HPP_ */
