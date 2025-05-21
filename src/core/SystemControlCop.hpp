/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** SystemControlCop
*/

#ifndef SYSTEMCONTROLCOP_HPP_
#define SYSTEMCONTROLCOP_HPP_

#include "Coprocessor.h"

#define COP0_NB_REG 16

class SystemControlCop : public Coprocessor {
    public:
        SystemControlCop();
        ~SystemControlCop();

        void reset();
        void execute(uint32_t opcode) override;

        uint32_t mfc(uint8_t reg) override;
        void mtc(uint8_t reg, uint32_t value) override;

        uint32_t cfc(uint8_t reg) override;
        void ctc(uint8_t reg, uint32_t value) override;

    private:
        uint32_t m_regs[COP0_NB_REG];
};

#endif /* !SYSTEMCONTROLCOP_HPP_ */
