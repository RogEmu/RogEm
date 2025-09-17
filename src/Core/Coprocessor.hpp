/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** Coprocessor interface file
*/

#ifndef COPROCESSOR_HPP_
#define COPROCESSOR_HPP_

#include <cstdint>

class Coprocessor {
    public:
        virtual ~Coprocessor() = default;

        virtual void execute(uint32_t opcode) = 0;

        virtual void mtc(uint8_t reg, uint32_t value) = 0;

        virtual uint32_t mfc(uint8_t reg) = 0;

        virtual void ctc(uint8_t reg, uint32_t value) = 0;

        virtual uint32_t cfc(uint8_t reg) = 0;
};

#endif /* !COPROCESSOR_HPP_ */
