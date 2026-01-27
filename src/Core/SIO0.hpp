/*
** EPITECH PROJECT, 2026
** rogem
** File description:
** SIO0
*/

#ifndef SIO0_HPP_
#define SIO0_HPP_

#include "SIODevice.hpp"

#include "InterruptController.hpp"
#include "DigitalPad.hpp"

class SIO0 : public SIODevice
{
    public:
        SIO0();
        ~SIO0();

        void write(SIORegister reg, uint16_t value) override;
        uint16_t read(SIORegister reg) override;

        void update(int cycles) override;
        void reset();

        bool irq() const { return m_irq; }

        DigitalPad &getPad(int index) { return m_pad[index]; }

    private:
        DigitalPad m_pad[2];
        bool m_irq;
};

#endif /* !SIO0_HPP_ */
