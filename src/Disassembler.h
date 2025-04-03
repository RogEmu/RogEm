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

enum class type
{
    UNKNOWN = -1,
    EXEC = 0,
    READ = 1,
    WRITE = 2,
};

namespace Disassembler
{
std::string disassemble(uint32_t pc, uint32_t word);
std::string disassemble(uint32_t pc, Instruction i);
std::string formatAsHexBytes(uint32_t value);
std::string getRegisterName(uint8_t reg);
type getInstructionType(Instruction i);
};


#endif /* !DISASSEMBLER_H_ */
