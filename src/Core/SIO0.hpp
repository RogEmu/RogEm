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

    private:
        DigitalPad m_pad[2];
};

#endif /* !SIO0_HPP_ */
