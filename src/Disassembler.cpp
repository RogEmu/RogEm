/*
** EPITECH PROJECT, 2024
** rogem
** File description:
** Disassembler
*/

#include "Disassembler.h"

#include <unordered_map>
#include <regex>
#include <fmt/format.h>
#include <iostream>

using namespace Disassembler;

static const std::string arithmeticFormat = "$%rd, $%rs, $%rt";
static const std::string arithmeticImmediateFormat = "$%rt, $%rs, %imm";
static const std::string loadStoreFormat = "$%rt, %imm($%rs)";

static const std::string registerNames[32] = {
    "zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"
};

struct InstructionData
{
    std::string name;
    std::string asmFormat;
    type InstructionType;
};

std::string Disassembler::getRegisterName(uint8_t reg)
{
    if (reg >= 32)
        return std::string();
    return registerNames[reg];
}

static const std::unordered_map<PrimaryOpCode, InstructionData> primaryData = {
    {PrimaryOpCode::ADDI, InstructionData{"addi", arithmeticImmediateFormat, type::EXEC}},
    {PrimaryOpCode::ADDIU, InstructionData{"addiu", arithmeticImmediateFormat, type::EXEC}},
    {PrimaryOpCode::ANDI, InstructionData{"andi", arithmeticImmediateFormat, type::EXEC}},
    {PrimaryOpCode::J, InstructionData{"j", "%address", type::EXEC}},
    {PrimaryOpCode::JAL, InstructionData{"jal", "%address", type::EXEC}},
    {PrimaryOpCode::BEQ, InstructionData{"beq", arithmeticImmediateFormat, type::EXEC}},
    {PrimaryOpCode::BNE, InstructionData{"bne", arithmeticImmediateFormat, type::EXEC}},
    {PrimaryOpCode::BGTZ, InstructionData{"bgtz", arithmeticImmediateFormat, type::EXEC}},
    {PrimaryOpCode::BLEZ, InstructionData{"blez", arithmeticImmediateFormat, type::EXEC}},
    {PrimaryOpCode::COP0, InstructionData{"cop0", "Incomplete", type::EXEC}},
    {PrimaryOpCode::LB, InstructionData{"lb", loadStoreFormat, type::READ}},
    {PrimaryOpCode::LBU, InstructionData{"lbu", loadStoreFormat, type::READ}},
    {PrimaryOpCode::LH, InstructionData{"lh", loadStoreFormat, type::READ}},
    {PrimaryOpCode::LHU, InstructionData{"lhu", loadStoreFormat, type::READ}},
    {PrimaryOpCode::LUI, InstructionData{"lui", "$%rt, %imm", type::EXEC}},
    {PrimaryOpCode::LW, InstructionData{"lw", loadStoreFormat, type::READ}},
    {PrimaryOpCode::LWL, InstructionData{"lwl", loadStoreFormat, type::READ}},
    {PrimaryOpCode::LWR, InstructionData{"lwr", loadStoreFormat, type::READ}},
    {PrimaryOpCode::ORI, InstructionData{"ori", arithmeticImmediateFormat, type::EXEC}},
    {PrimaryOpCode::SB, InstructionData{"sb", loadStoreFormat, type::WRITE}},
    {PrimaryOpCode::SH, InstructionData{"sh", loadStoreFormat, type::WRITE}},
    {PrimaryOpCode::SLTI, InstructionData{"slti", arithmeticImmediateFormat, type::EXEC}},
    {PrimaryOpCode::SLTIU, InstructionData{"sltiu", arithmeticImmediateFormat, type::EXEC}},
    {PrimaryOpCode::SW, InstructionData{"sw", loadStoreFormat, type::WRITE}},
    {PrimaryOpCode::SWL, InstructionData{"swl", loadStoreFormat, type::WRITE}},
    {PrimaryOpCode::SWR, InstructionData{"swr", loadStoreFormat, type::WRITE}},
    {PrimaryOpCode::XORI, InstructionData{"xori", arithmeticImmediateFormat, type::EXEC}},
};

static const std::unordered_map<SecondaryOpCode, InstructionData> secondaryData = {
    {SecondaryOpCode::ADD, InstructionData{"add", arithmeticFormat, type::EXEC}},
    {SecondaryOpCode::ADDU, InstructionData{"addu", arithmeticFormat, type::EXEC}},
    {SecondaryOpCode::AND, InstructionData{"and", arithmeticFormat, type::EXEC}},
    {SecondaryOpCode::NOR, InstructionData{"nor", arithmeticFormat, type::EXEC}},
    {SecondaryOpCode::OR, InstructionData{"or", arithmeticFormat, type::EXEC}},
    {SecondaryOpCode::SLL, InstructionData{"sll", "$%rd, $%rs, %imm", type::EXEC}},
    {SecondaryOpCode::SLT, InstructionData{"slt", arithmeticFormat, type::EXEC}},
    {SecondaryOpCode::SLTU, InstructionData{"sltu", "$%rd, $%rs, %imm", type::EXEC}},
    {SecondaryOpCode::SUB, InstructionData{"sub", arithmeticFormat, type::EXEC}},
    {SecondaryOpCode::SUBU, InstructionData{"subu", arithmeticFormat, type::EXEC}},
    {SecondaryOpCode::XOR, InstructionData{"xor", arithmeticFormat, type::EXEC}},
    {SecondaryOpCode::SRL, InstructionData{"srl", "$%rd, $%rs, %imm", type::EXEC}},
    {SecondaryOpCode::SRA, InstructionData{"sra", "$%rd, $%rs, %imm", type::EXEC}},
    {SecondaryOpCode::SLLV, InstructionData{"sllv", arithmeticFormat, type::EXEC}},
    {SecondaryOpCode::SRLV, InstructionData{"srlv", arithmeticFormat, type::EXEC}},
    {SecondaryOpCode::SRAV, InstructionData{"srav", arithmeticFormat, type::EXEC}},
    {SecondaryOpCode::MULT, InstructionData{"mult", arithmeticFormat, type::EXEC}},
    {SecondaryOpCode::MULTU, InstructionData{"multu", arithmeticFormat, type::EXEC}},
    {SecondaryOpCode::DIV, InstructionData{"div", arithmeticFormat, type::EXEC}},
    {SecondaryOpCode::DIVU, InstructionData{"divu", arithmeticFormat, type::EXEC}},
    {SecondaryOpCode::MFHI, InstructionData{"mfhi", "$%rd", type::EXEC}},
    {SecondaryOpCode::MFLO, InstructionData{"mflo", "$%rd", type::EXEC}},
    {SecondaryOpCode::MTHI, InstructionData{"mthi", "$%rs", type::EXEC}},
    {SecondaryOpCode::MTLO, InstructionData{"mtlo", "$%rs", type::EXEC}},
    {SecondaryOpCode::JR, InstructionData{"jr", "$%rs", type::EXEC}},
    {SecondaryOpCode::JALR, InstructionData{"jalr", "$%rd, $%rs", type::EXEC}},
};

static int16_t computeBranchOffset(Instruction i)
{
    return (int16_t)i.i.immediate << 2;
}

static std::string formatAssembly(uint32_t pc, Instruction i, const InstructionData &data)
{
    std::string result = fmt::format("{:3} {}", data.name, data.asmFormat);
    result = std::regex_replace(result, std::regex("%rs"), registerNames[i.r.rs]);
    result = std::regex_replace(result, std::regex("%rt"), registerNames[i.r.rt]);
    result = std::regex_replace(result, std::regex("%rd"), registerNames[i.r.rd]);
    result = std::regex_replace(result, std::regex("%shamt"), std::to_string(i.r.shamt));
    result = std::regex_replace(result, std::regex("%address"), fmt::format("0x{:x}", (uint32_t)i.j.address));

    auto code = static_cast<PrimaryOpCode>(i.i.opcode);
    uint32_t immediateValue = 0;
    if (code == PrimaryOpCode::BEQ || code == PrimaryOpCode::BNE || code == PrimaryOpCode::BGTZ || code == PrimaryOpCode::BLEZ)
        immediateValue = pc + 4 + computeBranchOffset(i);
    else
        immediateValue = i.i.immediate;

    result = std::regex_replace(result, std::regex("%imm"), fmt::format("0x{:x}", immediateValue));

    // result = fmt::format("0x{:08x}:  {}  {}", pc, formatAsHexBytes(i.raw), result);
    return result;
}

type Disassembler::getInstructionType(Instruction i) {
    auto primary = static_cast<PrimaryOpCode>(i.r.opcode);

    if (primary == PrimaryOpCode::SPECIAL) {
        auto secondary = static_cast<SecondaryOpCode>(i.r.funct);
        if (secondaryData.find(secondary) != secondaryData.end()) {
            return secondaryData.at(secondary).InstructionType;
        }
    }
    if (primary == PrimaryOpCode::BCONDZ) {
        auto bcond = static_cast<BranchOnConditionZero>(i.i.rt);
        switch (bcond) {
            case BranchOnConditionZero::BLTZ:
            case BranchOnConditionZero::BGEZ:
            case BranchOnConditionZero::BLTZAL:
            case BranchOnConditionZero::BGEZAL:
                return type::EXEC;
            default:
                return type::UNKNOWN;
        }
    }
    if (primary == PrimaryOpCode::COP0) {
        auto cop = static_cast<CoprocessorOpcodes>(i.r.rs);
        switch (cop) {
            case CoprocessorOpcodes::MTC:
                return type::WRITE;
            case CoprocessorOpcodes::MFC:
                return type::READ;
            default:
                return type::UNKNOWN;
        }
    }
    if (primaryData.find(primary) != primaryData.end()) {
        return primaryData.at(primary).InstructionType;
    }
    return type::UNKNOWN;
}

static std::string disassembleSpecial(uint32_t pc, Instruction i)
{
    auto secondary = static_cast<SecondaryOpCode>(i.r.funct);

    if (secondaryData.find(secondary) != secondaryData.end())
    {
        auto data = secondaryData.at(secondary);
        return formatAssembly(pc, i, data);
    }
    return std::string("MIPS Disassembler: Illegal instruction");
}

static std::string dissambleBcondZ(uint32_t pc, Instruction i)
{
    auto bcond = static_cast<BranchOnConditionZero>(i.i.rt);

    switch (bcond)
    {
    case BranchOnConditionZero::BLTZ:
        return formatAssembly(pc, i, InstructionData{"bltz", "$%rs, %imm", type::EXEC});
    case BranchOnConditionZero::BGEZ:
        return formatAssembly(pc, i, InstructionData{"bgez", "$%rs, %imm", type::EXEC});
    case BranchOnConditionZero::BLTZAL:
        return formatAssembly(pc, i, InstructionData{"bltzal", "$%rs, %imm", type::EXEC});
    case BranchOnConditionZero::BGEZAL:
        return formatAssembly(pc, i, InstructionData{"bgezal", "$%rs, %imm", type::EXEC});
    default:
        return std::string("MIPS Disassembler: Illegal instruction");
    }
}

static std::string disassembleCop0(uint32_t pc, Instruction i)
{
    auto code = static_cast<CoprocessorOpcodes>(i.r.rs);

    switch (code)
    {
    case CoprocessorOpcodes::MTC:
        return formatAssembly(pc, i, InstructionData{"mtc0", "$%rt, $%rd", type::WRITE}); // type a revoir
    case CoprocessorOpcodes::MFC:
        return formatAssembly(pc, i, InstructionData{"mfc0", "$%rt, $%rd", type::READ}); // type a revoir
    default:
        break;
    }
    return std::string("MIPS Disassembler: Illegal instruction");
}

std::string Disassembler::formatAsHexBytes(uint32_t value)
{
    return fmt::format("{:02x} {:02x} {:02x} {:02x}",
                        (value >> 24) & 0xFF,  // Most significant byte
                        (value >> 16) & 0xFF,
                        (value >> 8) & 0xFF,
                        value & 0xFF);         // Least significant byte
}

std::string Disassembler::disassemble(uint32_t pc, Instruction i)
{
    auto primary = static_cast<PrimaryOpCode>(i.r.opcode);

    if (primary == PrimaryOpCode::SPECIAL)
    {
        return disassembleSpecial(pc, i);
    }
    if (primary == PrimaryOpCode::BCONDZ)
    {
        return dissambleBcondZ(pc, i);
    }
    if (primary == PrimaryOpCode::COP0)
    {
        return disassembleCop0(pc, i);
    }
    if (primaryData.find(primary) != primaryData.end())
    {
        auto data = primaryData.at(primary);
        return formatAssembly(pc, i, data);
    }
    return std::string("MIPS Disassembler: Illegal instruction");
}

std::string Disassembler::disassemble(uint32_t pc, uint32_t word)
{
    Instruction i = {.raw = word};

    return disassemble(pc, i);
}
