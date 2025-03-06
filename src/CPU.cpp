/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** CPU
*/

#include "CPU.h"
#include <iostream>

#include "Disassembler.h"

static const std::string m_registerNames[32] = {
    "zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"
};

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
    if (a > 0 && b > 0 && a > INT32_MAX - b)
        return true;
    if (a < 0 && b < 0 && a < INT32_MIN - b)
        return true;
    return false;
}

static bool subOverflow(int32_t a, int32_t b)
{
    if (b < 0 && a > INT32_MAX + b)
        return true;
    if (b > 0 && a < INT32_MIN + b)
        return true;
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

    std::cout << Disassembler::disassemble(m_pc, instruction) << std::endl;
    executeInstruction(instruction);
    // printInstruction(instruction);
    // debugState();
}

Instruction CPU::fetchInstruction()
{
    uint32_t instruction = m_bus.loadWord(m_pc);
    m_pc += 4;
    return Instruction{.raw=instruction};
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
    case PrimaryOpCode::JAL:
        jumpAndLink(instruction);
        break;
    case PrimaryOpCode::LW:
        loadWord(instruction);
        break;
    case PrimaryOpCode::SH:
        storeHalfWord(instruction);
        break;
    case PrimaryOpCode::LB:
        loadByte(instruction);
        break;
    case PrimaryOpCode::LBU:
        loadByteUnsigned(instruction);
        break;
    case PrimaryOpCode::LHU:
        loadHalfWordUnsigned(instruction);
        break;
    case PrimaryOpCode::XORI:
        xorImmediateWord(instruction);
        break;
    case PrimaryOpCode::ANDI:
        andImmediateWord(instruction);
        break;
    case PrimaryOpCode::SB:
        storeByte(instruction);
        break;
    case PrimaryOpCode::LH:
        loadHalfWord(instruction);
        break;
    case PrimaryOpCode::SWR:
        storeWordRight(instruction);
        break;
    case PrimaryOpCode::SWL:
        storeWordLeft(instruction);
        break;
    case PrimaryOpCode::LWR:
        loadWordRight(instruction);
        break;
    case PrimaryOpCode::LWL:
        loadWordLeft(instruction);
        break;
    case PrimaryOpCode::SLTI:
        setOnLessThanImmediate(instruction);
        break;
    case PrimaryOpCode::SLTIU:
        setOnLessThanImmediateUnsigned(instruction);
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
    case SecondaryOpCode::XOR:
        xorWord(instruction);
        break;
    case SecondaryOpCode::NOR:
        norWord(instruction);
        break;
    case SecondaryOpCode::ADDU:
        addWordUnsigned(instruction);
        break;
    case SecondaryOpCode::SUBU:
        substractWordUnsigned(instruction);
        break;
    case SecondaryOpCode::AND:
        andWord(instruction);
        break;
    case SecondaryOpCode::SUB:
        substractWord(instruction);
        break;
    case SecondaryOpCode::ADD:
        addWord(instruction);
        break;
    case SecondaryOpCode::SLT:
        setOnLessThan(instruction);
        break;
    case SecondaryOpCode::SLTU:
        setOnLessThanUnsigned(instruction);
        break;
    case SecondaryOpCode::SLLV:
        shiftLeftLogicalVariable(instruction);
        break;
    case SecondaryOpCode::SRL:
        shiftRightLogical(instruction);
        break;
    case SecondaryOpCode::SRLV:
        shiftRightLogicalVariable(instruction);
        break;
    case SecondaryOpCode::SRA:
        shiftRightArithmetic(instruction);
        break;
    case SecondaryOpCode::SRAV:
        shiftRightArithmeticVariable(instruction);
        break;
    case SecondaryOpCode::MULT:
        multiply(instruction);
        break;
    case SecondaryOpCode::MULTU:
        multiplyUnsigned(instruction);
        break;
    case SecondaryOpCode::DIV:
        divide(instruction);
        break;
    case SecondaryOpCode::DIVU:
        divideUnsigned(instruction);
        break;
    case SecondaryOpCode::MFHI:
        moveFromHi(instruction);
        break;
    case SecondaryOpCode::MFLO:
        moveFromLo(instruction);
        break;
    case SecondaryOpCode::MTHI:
        moveToHi(instruction);
        break;
    case SecondaryOpCode::MTLO:
        moveToLo(instruction);
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
    uint16_t value = static_cast<uint16_t>(getReg(instruction.i.rt));

    m_bus.storeHalfWord(address, value);
}

void CPU::storeByte(const Instruction &instruction)
{
    int32_t imm = (int16_t)instruction.i.immediate;
    uint32_t address = getReg(instruction.i.rs) + imm;
    uint8_t value = static_cast<uint8_t>(getReg(instruction.i.rt));

    m_bus.storeByte(address, value);
}

void CPU::shiftLeftLogical(const Instruction &instruction)
{
    uint32_t res = getReg(instruction.r.rt) << instruction.r.shamt;

    setReg(instruction.r.rd, res);
}

void CPU::addImmediateUnsigned(const Instruction &instruction)
{
    uint32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t val = getReg(instruction.i.rs);
    uint32_t res = val + imm;

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
        std::cout << "Substraction overflow! : TODO Raise Exception" << std::endl;
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
        std::cout << "Addition overflow! : TODO Raise Exception : TODO Raise Exception" << std::endl;
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

void CPU::addImmediate(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.i.rs);
    uint32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t tmp = left + imm;

    printf("orig: %i %x imm: %i %x\n", instruction.i.immediate, instruction.i.immediate, imm, imm);
    printf("reg: %i %x\n", left, left);
    if (addOverflow(left, imm))
    {
        // Overflow: need to raise exception on the system
        // Do not modify register on overflow
        std::cout << "Addition overflow! : TODO Raise Exception" << std::endl;
        return;
    }
    setReg(instruction.i.rt, tmp);
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
    uint32_t res = ~(left | right);

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

void CPU::loadWord(const Instruction &instruction)
{
    int32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(instruction.i.rs) + imm;

    if (address % 4 != 0) {
        // Misaligned: need to raise exception on the system
        std::cout << "Address is misaligned,!" << std::endl;
        return;
    }

    uint32_t value = m_bus.loadWord(address);
    setReg(instruction.i.rt, value);  // No sign extension needed for 32-bit word load
}

void CPU::loadHalfWord(const Instruction &instruction)
{
    int32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(instruction.i.rs) + imm;

    if (address % 2 != 0) {
        // Misaligned: need to raise exception on the system
        std::cout << "Address is misaligned,!" << std::endl;
        return;
    }

    int16_t value = static_cast<int16_t>(m_bus.loadHalfWord(address));  // Sign-extend
    setReg(instruction.i.rt, static_cast<int32_t>(value));
}

void CPU::loadHalfWordUnsigned(const Instruction &instruction)
{
    int32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(instruction.i.rs) + imm;

    if (address % 2 != 0) {
        // Misaligned: need to raise exception on the system
        std::cout << "Address is misaligned,!" << std::endl;
        return;
    }

    uint16_t value = m_bus.loadHalfWord(address);
    setReg(instruction.i.rt, static_cast<uint32_t>(value));
}

void CPU::loadByte(const Instruction &instruction)
{
    int32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(instruction.i.rs) + imm;

    int8_t value = static_cast<int8_t>(m_bus.loadByte(address));
    setReg(instruction.i.rt, static_cast<int32_t>(value));
}

void CPU::loadByteUnsigned(const Instruction &instruction)
{
    int32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(instruction.i.rs) + imm;

    uint8_t value = m_bus.loadByte(address);
    setReg(instruction.i.rt, static_cast<uint32_t>(value));
}

void CPU::loadWordRight(const Instruction &instruction)
{
    int32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(instruction.i.rs) + imm;
    uint32_t value = m_bus.loadWord(address & ~3);

    uint32_t shift = (address & 3) * 8;
    uint32_t mask = 0xFFFFFFFF >> shift;
    uint32_t right = (value << shift) & mask;

    uint32_t res = (getReg(instruction.i.rt) & ~mask) | right;
    setReg(instruction.i.rt, res);
}

void CPU::loadWordLeft(const Instruction &instruction)
{
    int32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(instruction.i.rs) + imm;
    uint32_t value = m_bus.loadWord(address & ~3);

    uint32_t shift = (3 - (address & 3)) * 8;
    uint32_t mask = 0xFFFFFFFF << shift;
    uint32_t left = (value >> shift) & mask;

    uint32_t res = (getReg(instruction.i.rt) & ~mask) | left;
    setReg(instruction.i.rt, res);
}

void CPU::storeWordRight(const Instruction &instruction)
{
    int32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(instruction.i.rs) + imm;
    uint32_t value = getReg(instruction.i.rt);

    uint32_t shift = (address & 3) * 8;
    uint32_t mask = 0xFFFFFFFF >> shift;
    uint32_t oldValue = m_bus.loadWord(address & ~3);

    uint32_t res = (oldValue & ~mask) | ((value << shift) & mask);
    m_bus.storeWord(address & ~3, res);
}

void CPU::storeWordLeft(const Instruction &instruction)
{
    int32_t imm = static_cast<int16_t>(instruction.i.immediate);
    uint32_t address = getReg(instruction.i.rs) + imm;
    uint32_t value = getReg(instruction.i.rt);

    uint32_t shift = (3 - (address & 3)) * 8;
    uint32_t mask = 0xFFFFFFFF << shift;
    uint32_t oldValue = m_bus.loadWord(address & ~3);

    uint32_t res = (oldValue & ~mask) | ((value >> shift) & mask);
    m_bus.storeWord(address & ~3, res);
}

void CPU::setOnLessThan(const Instruction &instruction)
{
    int32_t left = getReg(instruction.r.rs);
    int32_t right = getReg(instruction.r.rt);
    int32_t res = left < right ? 1 : 0;

    setReg(instruction.r.rd, res);
}

void CPU::setOnLessThanUnsigned(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.r.rs);
    uint32_t right = getReg(instruction.r.rt);
    int32_t res = left < right ? 1 : 0;

    setReg(instruction.r.rd, res);
}

void CPU::setOnLessThanImmediate(const Instruction &instruction)
{
    int32_t left = getReg(instruction.i.rs);
    int32_t right = static_cast<int16_t>(instruction.i.immediate);
    int32_t res = left < right ? 1 : 0;

    setReg(instruction.i.rt, res);
}

void CPU::setOnLessThanImmediateUnsigned(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.i.rs);
    uint32_t right = static_cast<uint32_t>(static_cast<int16_t>(instruction.i.immediate));
    int32_t res = left < right ? 1 : 0;

    setReg(instruction.i.rt, res);
}

void CPU::shiftLeftLogicalVariable(const Instruction &instruction)
{
    uint32_t res = getReg(instruction.r.rt) << (getReg(instruction.r.rs) & 0x1F);

    setReg(instruction.r.rd, res);
}

void CPU::shiftRightLogical(const Instruction &instruction)
{
    uint32_t res = getReg(instruction.r.rt) >> instruction.r.shamt;

    setReg(instruction.r.rd, res);
}

void CPU::shiftRightLogicalVariable(const Instruction &instruction)
{
    uint32_t res = getReg(instruction.r.rt) >> (getReg(instruction.r.rs) & 0x1F);

    setReg(instruction.r.rd, res);
}

void CPU::shiftRightArithmetic(const Instruction &instruction)
{
    int32_t value = getReg(instruction.r.rt);
    int32_t res = value >> instruction.r.shamt;

    setReg(instruction.r.rd, res);
}

void CPU::shiftRightArithmeticVariable(const Instruction &instruction)
{
    int32_t value = getReg(instruction.r.rt);
    uint32_t shiftAmount = getReg(instruction.r.rs) & 0x1F;
    uint32_t res =  value >> shiftAmount;

    setReg(instruction.r.rd, res);
}

void CPU::multiply(const Instruction &instruction)
{
    int32_t left = static_cast<int32_t>(getReg(instruction.r.rs));
    int32_t right = static_cast<int32_t>(getReg(instruction.r.rt));

    int64_t res = static_cast<int64_t>(left) * static_cast<int64_t>(right);

    m_lo = static_cast<int32_t>(res & 0xFFFFFFFF);
    m_hi = static_cast<int32_t>(res >> 32);
}

void CPU::multiplyUnsigned(const Instruction &instruction)
{
    uint32_t left = static_cast<int32_t>(getReg(instruction.r.rs));
    uint32_t right = static_cast<int32_t>(getReg(instruction.r.rt));

    uint64_t res = static_cast<uint64_t>(left) * static_cast<uint64_t>(right);

    m_lo = static_cast<uint32_t>(res & 0xFFFFFFFF);
    m_hi = static_cast<uint32_t>(res >> 32);
}

void CPU::divide(const Instruction &instruction)
{
    int32_t left = static_cast<int32_t>(getReg(instruction.r.rs));
    int32_t right = static_cast<int32_t>(getReg(instruction.r.rt));

    if (right == 0)
    {
        m_lo = 0;
        m_hi = left;
    }
    else
    {
        m_lo = left / right;
        m_hi = left % right;
    }
}

void CPU::divideUnsigned(const Instruction &instruction)
{
    uint32_t left = getReg(instruction.r.rs);
    uint32_t right = getReg(instruction.r.rt);

    if (right == 0)
    {
        m_lo = 0;
        m_hi = left;
    }
    else
    {
        m_lo = left / right;
        m_hi = left % right;
    }
}

void CPU::moveFromHi(const Instruction &instruction)
{
    setReg(instruction.r.rd, m_hi);
}

void CPU::moveFromLo(const Instruction &instruction)
{
    setReg(instruction.r.rd, m_lo);
}

void CPU::moveToHi(const Instruction &instruction)
{
    m_hi = getReg(instruction.r.rs);
}

void CPU::moveToLo(const Instruction &instruction)
{
    m_lo = getReg(instruction.r.rs);
}

void CPU::jump(const Instruction &instruction)
{
    m_pc = (m_pc & 0xF0000000) | (((uint32_t)instruction.j.address) << 2);
}

void CPU::jumpAndLink(const Instruction &instruction)
{
    setReg(31, m_pc + 8);
    m_pc = (m_pc & 0xF0000000) | (((uint32_t)instruction.j.address) << 2);
}

void CPU::jumpRegister(const Instruction &instruction)
{
    m_pc = getReg(instruction.r.rs);
}

void CPU::jumpAndLinkRegister(const Instruction &instruction)
{
    setReg(31, m_pc + 8);
    m_pc = getReg(instruction.r.rs);
}

void CPU::branchOnEqual(const Instruction &instruction)
{
    if (getReg(instruction.i.rs) == getReg(instruction.i.rt))
        m_pc += 4 + ((int16_t)instruction.i.immediate << 2);
    else
        m_pc += 4;
}

void CPU::branchOnNotEqual(const Instruction &instruction)
{
    if (getReg(instruction.i.rs) != getReg(instruction.i.rt))
        m_pc += 4 + ((int16_t)instruction.i.immediate << 2);
    else
        m_pc += 4;
}

void CPU::illegalInstruction(const Instruction &instruction)
{
    fprintf(stderr, "Illegal instruction: ");
    printInstruction(instruction);

    // Temporary exit until exception handling is properly implemented
    exit(1);
}
