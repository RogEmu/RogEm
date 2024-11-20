/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** CPU
*/

#include "CPU.h"
#include <iostream>

static void printCopInstruction(const Instruction &i)
{
    
}

static void printInstruction(const Instruction &inst)
{
    switch (inst.r.opcode)
    {
    case 0x00:
        printf("op:0x%02X rs:0x%02X rt:0x%02X rd:0x%02X shamt:0x%02X func:0x%02X\n",
        inst.r.opcode, inst.r.rs, inst.r.rt, inst.r.rd, inst.r.shamt, inst.r.funct);
        break;
    case 0x02:
    case 0x03:
        printf("op:0x%02X addr:0x%08X\n", inst.j.opcode, inst.j.address);
        break;
    default:
        printf("op:0x%02X rs:0x%02X rt:0x%02X imm:0x%04X\n", inst.i.opcode, inst.i.rs, inst.i.rt, inst.i.immediate);
        break;
    }
}

static bool addOverflow(int32_t a, int32_t b)
{
    uint32_t res = a + b;
    int32_t signedResult = (int32_t)res;

    if ((a > 0 && b > 0 && signedResult < 0) || // Positive overflow
        (a < 0 && b < 0 && signedResult > 0)) { // Negative overflow
        return true;
    }
    return false;
}

static bool subOverflow(int32_t a, int32_t b)
{
    uint32_t res = a - b;
    int32_t signedResult = (int32_t)res;

    // Check for overflow conditions
    if ((a > 0 && b < 0 && signedResult < 0) || // Positive overflow
        (a < 0 && b > 0 && signedResult > 0)) { // Negative overflow
        return true;
    }
    return false;
}

CPU::CPU(const Bus &bus) :
    m_pc(RESET_VECTOR),
    m_bus(bus)
{
    m_registers[0] = 0;
}

void CPU::step()
{
    Instruction instruction = fetchInstruction();

    executeInstruction(instruction);
    printInstruction(instruction);
    debugState();
}

Instruction CPU::fetchInstruction()
{
    uint32_t instruction = loadWord(m_pc);
    m_pc += 4;
    return (Instruction){.raw=instruction};
}

uint32_t CPU::loadWord(uint32_t addr)
{
    return m_bus.loadWord(addr);
}

void CPU::executeInstruction(const Instruction &instruction)
{
    switch (static_cast<PrimaryOpCode>(instruction.r.opcode))
    {
    case PrimaryOpCode::LUI:
        loadUpperImmediate(instruction);
        break;
    case PrimaryOpCode::ORI:
        orImmediateWord(instruction);
        break;
    case PrimaryOpCode::SW:
        storeWord(instruction);
        break;
    case PrimaryOpCode::ADDIU:
        addImmediateUnsigned(instruction);
        break;
    case PrimaryOpCode::J:
        jump(instruction);
        break;
    case PrimaryOpCode::SPECIAL:
        specialInstruction(instruction);
        break;
    default:
        illegalInstruction(instruction);
        break;
    }
}

void CPU::specialInstruction(const Instruction &instruction)
{
    switch (static_cast<SecondaryOpCode>(instruction.r.funct))
    {
    case SecondaryOpCode::SLL:
        shiftLeftLogical(instruction);
        break;
    case SecondaryOpCode::OR:
        orWord(instruction);
        break;
    default:
        illegalInstruction(instruction);
        break;
    }
}

void CPU::debugState() const
 {
    std::cout << "CPU Register State:\n";
    std::cout << "PC : 0x" << std::hex << std::setw(8) << std::setfill('0') << m_pc << "    ";
    std::cout << "HI : 0x" << std::setw(8) << m_hi << "    ";
    std::cout << "LO : 0x" << std::setw(8) << m_lo << "\n";
    for (int i = 0; i < 32; i++) {
        std::cout << std::setw(4) << std::setfill(' ') << m_registerNames[i] << " : 0x"
                << std::hex << std::setw(8) << std::setfill('0') << m_registers[i];
        if (i % 4 == 3) std::cout << "\n";
        else std::cout << "    ";
    }
}

uint32_t CPU::getReg(uint8_t reg) const
{
    return m_registers[reg];
}

void CPU::setReg(uint8_t reg, uint32_t val)
{
    m_registers[reg] = val;
    m_registers[0] = 0;
}

void CPU::loadUpperImmediate(const Instruction &instruction)
{
    uint32_t res = instruction.i.immediate;
    res = res << 16;
    setReg(instruction.i.rt, res);
}

void CPU::storeWord(const Instruction &instruction)
{
    int32_t imm = (int16_t)instruction.i.immediate;
    uint32_t address = getReg(instruction.i.rs) + imm;
    uint32_t value = getReg(instruction.i.rt);

    m_bus.storeWord(address, value);
}

void CPU::storeHalfWord(const Instruction &instruction)
{
    int32_t imm = (int16_t)instruction.i.immediate;
    uint32_t address = getReg(instruction.i.rs) + imm;
    uint16_t value = getReg(instruction.i.rt);

    m_bus.storeHalfWord(address, value);
}

void CPU::storeByte(const Instruction &instruction)
{
    int32_t imm = (int16_t)instruction.i.immediate;
    uint32_t address = getReg(instruction.i.rs) + imm;
    uint8_t value = getReg(instruction.i.rt);

    m_bus.storeByte(address, value);
}

void CPU::shiftLeftLogical(const Instruction &instruction)
{
    uint32_t res = getReg(instruction.r.rt) << instruction.r.shamt;

    setReg(instruction.r.rd, res);
}

void CPU::addImmediateUnsigned(const Instruction &instruction)
{
    int32_t imm = (int16_t)instruction.i.immediate;
    uint32_t res = getReg(instruction.i.rs) + imm;

    setReg(instruction.i.rt, res);
}

void CPU::substractWordUnsigned(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.r.rs);
    uint32_t right = getReg(instruction.r.rt);
    uint32_t tmp = left - right;

    setReg(instruction.r.rd, tmp);
}

void CPU::substractWord(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.r.rs);
    uint32_t right = getReg(instruction.r.rt);
    uint32_t tmp = left - right;

    if (subOverflow(left, right))
    {
        // Overflow: need to raise exception on the system
        std::cout << "Substraction overflow!" << std::endl;
        return;
    }
    setReg(instruction.r.rd, tmp);
}

void CPU::addWord(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.r.rs);
    uint32_t right = getReg(instruction.r.rt);
    uint32_t tmp = left + right;

    if (addOverflow(left, right))
    {
        // Overflow: need to raise exception on the system
        std::cout << "Addition overflow!" << std::endl;
        return;
    }
    setReg(instruction.r.rd, tmp);
}

void CPU::addWordUnsigned(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.r.rs);
    uint32_t right = getReg(instruction.r.rt);
    uint32_t tmp = left + right;

    setReg(instruction.r.rd, tmp);
}

void CPU::andWord(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.r.rs);
    uint32_t right = getReg(instruction.r.rt);
    uint32_t res = left & right;

    setReg(instruction.r.rd, res);
}

void CPU::orWord(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.r.rs);
    uint32_t right = getReg(instruction.r.rt);
    uint32_t res = left | right;

    setReg(instruction.r.rd, res);
}

void CPU::xorWord(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.r.rs);
    uint32_t right = getReg(instruction.r.rt);
    uint32_t res = left ^ right;

    setReg(instruction.r.rd, res);
}

void CPU::norWord(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.r.rs);
    uint32_t right = getReg(instruction.r.rt);
    uint32_t res = !(left | right);

    setReg(instruction.r.rd, res);
}

void CPU::andImmediateWord(const Instruction &instruction)
{
    uint32_t res = getReg(instruction.i.rs) & (int32_t)instruction.i.immediate;

    setReg(instruction.i.rt, res);
}

void CPU::orImmediateWord(const Instruction &instruction)
{
    uint32_t res = getReg(instruction.i.rs) | (int32_t)instruction.i.immediate;

    setReg(instruction.i.rt, res);
}

void CPU::xorImmediateWord(const Instruction &instruction)
{
    uint32_t res = getReg(instruction.i.rs) ^ (int32_t)instruction.i.immediate;

    setReg(instruction.i.rt, res);
}

void CPU::jump(const Instruction &instruction)
{
    m_pc = (m_pc & 0xF0000000) | (((uint32_t)instruction.j.address) << 2);
}

void CPU::illegalInstruction(const Instruction &instruction)
{
    fprintf(stderr, "Illegal instruction: ");
    printInstruction(instruction);

    // Temporary exit until exception handling is properly implemented
    exit(1);
}
