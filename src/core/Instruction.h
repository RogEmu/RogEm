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
    BCONDZ = 0x01,
    J = 0x02,
    JAL = 0x03,
    BEQ = 0x04,
    BNE = 0x05,
    BLEZ = 0x06,
    BGTZ = 0x07,
    ADDI = 0x08,
    ADDIU = 0x09,
    ANDI = 0x0C,
    ORI = 0x0D,
    XORI = 0x0E,
    LUI = 0x0F,
    COP0 = 0x10,
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
    JR = 0x08,
    JALR = 0x09,
    SYSCALL = 0xC,
    BREAK = 0xD,
    MFHI = 0x10,
    MTHI = 0x11,
    MFLO = 0x12,
    MTLO = 0x13,
    MULT = 0x18,
    MULTU = 0x19,
    DIV = 0x1A,
    DIVU = 0x1B,
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

enum class BranchOnConditionZero
{
    BLTZ = 0x00,
    BGEZ = 0x01,
    BLTZAL = 0x10,
    BGEZAL = 0x11,
};

enum class CoprocessorOpcode
{
    MFC = 0b0,
    CFC = 0b01,
    MTC = 0b100,
    BC = 0b1000,
    IMM25 = 0b10000
};

enum class GTEFunction : uint8_t {
    RTPS   = 0x01,
    NCLIP  = 0x06,
    OP     = 0x0C,
    DPCS   = 0x10,
    INTPL  = 0x11,
    MVMVA  = 0x12,
    NCDS   = 0x13,
    CDP    = 0x14,
    NCDT   = 0x16,
    NCCS   = 0x1B,
    CC     = 0x1C,
    NCS    = 0x1E,
    NCT    = 0x20,
    SQR    = 0x28,
    DCPL   = 0x29,
    DPCT   = 0x2A,
    AVSZ3  = 0x2D,
    AVSZ4  = 0x2E,
    RTPT   = 0x30,
    GPF    = 0x3D,
    GPL    = 0x3E,
    NCCT   = 0x3F
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
