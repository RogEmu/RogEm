/*
** EPITECH PROJECT, 2024
** rogem
** File description:
** Disassembler
*/

#ifndef DISASSEMBLER_H_
#define DISASSEMBLER_H_

#include <cstdint>
#include <string>

#include <capstone/capstone.h>

struct InstructionData
{
    std::string mnemonic;
    std::string operands;
    std::string bytes;
};

class Disassembler
{
    public:
        Disassembler();
        ~Disassembler();

        InstructionData disasm(uint32_t inst, uint32_t addr) const;
        std::string cpuRegName(uint8_t reg) const;
        std::string copRegName(int cop, uint8_t reg) const;

    private:
        csh m_csHandle;
};

#endif /* !DISASSEMBLER_H_ */
