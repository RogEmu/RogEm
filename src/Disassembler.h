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

#include "Instruction.h"

namespace Disassembler
{

struct InstructionData
{
    std::string name;
    std::string asmFormat;
};

std::string disassemble(uint32_t pc, Instruction i);
void debugState(uint32_t pc, const uint32_t *registers);

std::string getRegisterName(uint8_t reg);
};


#endif /* !DISASSEMBLER_H_ */
