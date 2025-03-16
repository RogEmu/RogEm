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

static const std::unordered_map<PrimaryOpCode, InstructionData> primaryData = {
    {PrimaryOpCode::ADDI, InstructionData{"addi", arithmeticImmediateFormat}},
    {PrimaryOpCode::ADDIU, InstructionData{"addiu", arithmeticImmediateFormat}},
    {PrimaryOpCode::ANDI, InstructionData{"andi", arithmeticImmediateFormat}},
    {PrimaryOpCode::J, InstructionData{"j", "%address"}},
    {PrimaryOpCode::JAL, InstructionData{"jal", "%address"}},
    {PrimaryOpCode::BEQ, InstructionData{"beq", arithmeticImmediateFormat}},
    {PrimaryOpCode::BNE, InstructionData{"bne", arithmeticImmediateFormat}},
    {PrimaryOpCode::BGTZ, InstructionData{"bgtz", arithmeticImmediateFormat}},
    {PrimaryOpCode::BLEZ, InstructionData{"blez", arithmeticImmediateFormat}},
    {PrimaryOpCode::COP0, InstructionData{"cop0", "Incomplete"}},
    {PrimaryOpCode::LB, InstructionData{"lb", loadStoreFormat}},
    {PrimaryOpCode::LBU, InstructionData{"lbu", loadStoreFormat}},
    {PrimaryOpCode::LH, InstructionData{"lh", loadStoreFormat}},
    {PrimaryOpCode::LHU, InstructionData{"lhu", loadStoreFormat}},
    {PrimaryOpCode::LUI, InstructionData{"lui", "$%rt, %imm"}},
    {PrimaryOpCode::LW, InstructionData{"lw", loadStoreFormat}},
    {PrimaryOpCode::LWL, InstructionData{"lwl", loadStoreFormat}},
    {PrimaryOpCode::LWR, InstructionData{"lwr", loadStoreFormat}},
    {PrimaryOpCode::ORI, InstructionData{"ori", arithmeticImmediateFormat}},
    {PrimaryOpCode::SB, InstructionData{"sb", loadStoreFormat}},
    {PrimaryOpCode::SH, InstructionData{"lh", loadStoreFormat}},
    {PrimaryOpCode::SLTI, InstructionData{"slti", arithmeticImmediateFormat}},
    {PrimaryOpCode::SLTIU, InstructionData{"sltiu", arithmeticImmediateFormat}},
    {PrimaryOpCode::SW, InstructionData{"sw", loadStoreFormat}},
    {PrimaryOpCode::SWL, InstructionData{"swl", loadStoreFormat}},
    {PrimaryOpCode::SWR, InstructionData{"swr", loadStoreFormat}},
    {PrimaryOpCode::XORI, InstructionData{"xori", arithmeticImmediateFormat}}
};

static const std::unordered_map<SecondaryOpCode, InstructionData> secondaryData = {
    {SecondaryOpCode::ADD, InstructionData{"add", arithmeticFormat}},
    {SecondaryOpCode::ADDU, InstructionData{"addu", arithmeticFormat}},
    {SecondaryOpCode::AND, InstructionData{"and", arithmeticFormat}},
    {SecondaryOpCode::NOR, InstructionData{"nor", arithmeticFormat}},
    {SecondaryOpCode::OR, InstructionData{"or", arithmeticFormat}},
    {SecondaryOpCode::SLL, InstructionData{"sll", "$%rd, $%rs, %imm"}},
    {SecondaryOpCode::SLT, InstructionData{"slt", arithmeticFormat}},
    {SecondaryOpCode::SLTU, InstructionData{"sltu", "$%rd, $%rs, %imm"}},
    {SecondaryOpCode::SUB, InstructionData{"sub", arithmeticFormat}},
    {SecondaryOpCode::SUBU, InstructionData{"subu", arithmeticFormat}},
    {SecondaryOpCode::XOR, InstructionData{"xor", arithmeticFormat}},
    {SecondaryOpCode::SRL, InstructionData{"srl", "$%rd, $%rs, %imm"}},
    {SecondaryOpCode::SRA, InstructionData{"sra", "$%rd, $%rs, %imm"}},
    {SecondaryOpCode::SLLV, InstructionData{"sllv", arithmeticFormat}},
    {SecondaryOpCode::SRLV, InstructionData{"srlv", arithmeticFormat}},
    {SecondaryOpCode::SRAV, InstructionData{"srav", arithmeticFormat}},
    {SecondaryOpCode::MULT, InstructionData{"mult", arithmeticFormat}},
    {SecondaryOpCode::MULTU, InstructionData{"multu", arithmeticFormat}},
    {SecondaryOpCode::DIV, InstructionData{"div", arithmeticFormat}},
    {SecondaryOpCode::DIVU, InstructionData{"divu", arithmeticFormat}},
    {SecondaryOpCode::MFHI, InstructionData{"mfhi", "$%rd"}},
    {SecondaryOpCode::MFLO, InstructionData{"mflo", "$%rd"}},
    {SecondaryOpCode::MTHI, InstructionData{"mthi", "$%rs"}},
    {SecondaryOpCode::MTLO, InstructionData{"mtlo", "$%rs"}},
    {SecondaryOpCode::JR, InstructionData{"jr", "$%rs"}},
    {SecondaryOpCode::JALR, InstructionData{"jalr", "$%rd, $%rs"}},
};

static std::string formatAsHexBytes(uint32_t value) {
    return fmt::format("{:02x} {:02x} {:02x} {:02x}",
                        (value >> 24) & 0xFF,  // Most significant byte
                        (value >> 16) & 0xFF,
                        (value >> 8) & 0xFF,
                        value & 0xFF);         // Least significant byte
}

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

    result = fmt::format("0x{:08x}:  {}  {}", pc, formatAsHexBytes(i.raw), result);
    return result;
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
        return formatAssembly(pc, i, InstructionData{"bltz", "$%rs, %imm"});
    case BranchOnConditionZero::BGEZ:
        return formatAssembly(pc, i, InstructionData{"bgez", "$%rs, %imm"});
    case BranchOnConditionZero::BLTZAL:
        return formatAssembly(pc, i, InstructionData{"bltzal", "$%rs, %imm"});
    case BranchOnConditionZero::BGEZAL:
        return formatAssembly(pc, i, InstructionData{"bgezal", "$%rs, %imm"});
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
        return formatAssembly(pc, i, InstructionData{"mtc0", "$%rt, $%rd"});
    default:
        break;
    }
    return std::string();
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

void Disassembler::debugState(uint32_t pc, const uint32_t *registers)
{
    std::cout << "CPU Register State:\n";
    std::cout << fmt::format("PC: 0x{:08x}\n", pc);
    for (int i = 0; i < 32; i++) {
        std::cout << fmt::format("{:>5}: 0x{:08x}", registerNames[i], registers[i]);
        if (i % 4 == 3) std::cout << "\n";
        else std::cout << "    ";
    }
}
