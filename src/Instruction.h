/*
** EPITECH PROJECT, 2024
** rogem
** File description:
** Instruction
*/

#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

#include <cstdint>

enum class PrimaryOpCode
{
    ILLEGAL = -1,
    SPECIAL = 0x00,
    J = 0x02,
    ADDI = 0x08,
    ADDIU = 0x09,
    ANDI = 0x0C,
    ORI = 0x0D,
    XORI = 0x0E,
    LUI = 0x0F,
    LB = 0x20,
    LH = 0x21,
    LWL = 0x22,
    LW = 0x23,
    LBU = 0x24,
    LHU = 0x25,
    LWR = 0x26,
    SB = 0x28,
    SH = 0x29,
    SWL = 0x2A,
    SW = 0x2B,
    SWR = 0x2E,
    SLTI = 0x0A,
    SLTIU = 0x0B,
};

enum class SecondaryOpCode
{
    ILLEGAL = -1,
    SLL = 0x00,
    SRL = 0x02,
    SRA = 0x03,
    SLLV = 0x04,
    SRLV = 0x06,
    SRAV = 0x07,
    ADD = 0x20,
    ADDU = 0x21,
    SUB = 0x22,
    SUBU = 0x23,
    AND = 0x24,
    OR = 0x25,
    XOR = 0x26,
    NOR = 0x27,
    SLT = 0x2A,
    SLTU = 0x2B,
};

union Instruction {
    uint32_t raw;  // The raw 32-bit instruction

    // R-type format: opcode | rs | rt | rd | shamt | funct
    struct {
        uint32_t funct : 6;
        uint32_t shamt : 5;
        uint32_t rd    : 5;
        uint32_t rt    : 5;
        uint32_t rs    : 5;
        uint32_t opcode: 6;
    } r;

    // I-type format: opcode | rs | rt | immediate
    struct {
        uint32_t immediate : 16;
        uint32_t rt        : 5;
        uint32_t rs        : 5;
        uint32_t opcode    : 6;
    } i;

    // J-type format: opcode | address
    struct {
        uint32_t address : 26;
        uint32_t opcode  : 6;
    } j;
};

#endif /* !INSTRUCTION_H_ */
