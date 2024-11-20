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

};
#endif /* !DISASSEMBLER_H_ */
