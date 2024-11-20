/*
** EPITECH PROJECT, 2024
** rogem
** File description:
** Disassembler
*/

#include "Disassembler.h"

#include <unordered_map>
#include <regex>
#include <fmt/core.h>

using namespace Disassembler;

static const std::string arithmeticFormat = "$%rd, $%rs, $%rt";
static const std::string arithmeticImmediateFormat = "$%rt, $%rs, %imm";
static const std::string loadStoreFormat = "$%rt, %imm($%rs)";

static const std::unordered_map<PrimaryOpCode, InstructionData> primaryData = {
    {PrimaryOpCode::ADDIU, (InstructionData){"addiu", arithmeticImmediateFormat}},
    {PrimaryOpCode::ANDI, (InstructionData){"andi", arithmeticImmediateFormat}},
    {PrimaryOpCode::J, (InstructionData){"jump", "%address"}},
    {PrimaryOpCode::LB, (InstructionData){"lb", loadStoreFormat}},
    {PrimaryOpCode::LBU, (InstructionData){"lbu", loadStoreFormat}},
    {PrimaryOpCode::LH, (InstructionData){"lh", loadStoreFormat}},
    {PrimaryOpCode::LHU, (InstructionData){"lhu", loadStoreFormat}},
    {PrimaryOpCode::LUI, (InstructionData){"lui", "$%rt, %imm"}},
    {PrimaryOpCode::LW, (InstructionData){"lw", loadStoreFormat}},
    {PrimaryOpCode::LWL, (InstructionData){"lwl", loadStoreFormat}},
    {PrimaryOpCode::LWR, (InstructionData){"lwr", loadStoreFormat}},
    {PrimaryOpCode::ORI, (InstructionData){"ori", arithmeticImmediateFormat}},
    {PrimaryOpCode::SB, (InstructionData){"sb", loadStoreFormat}},
    {PrimaryOpCode::SH, (InstructionData){"lh", loadStoreFormat}},
    {PrimaryOpCode::SLTI, (InstructionData){"slti", arithmeticImmediateFormat}},
    {PrimaryOpCode::SLTIU, (InstructionData){"sltiu", arithmeticImmediateFormat}},
    {PrimaryOpCode::SW, (InstructionData){"sw", loadStoreFormat}},
    {PrimaryOpCode::SWL, (InstructionData){"swl", loadStoreFormat}},
    {PrimaryOpCode::SWR, (InstructionData){"swr", loadStoreFormat}},
    {PrimaryOpCode::XORI, (InstructionData){"xori", arithmeticImmediateFormat}}
};

static const std::unordered_map<SecondaryOpCode, InstructionData> secondaryData = {
    {SecondaryOpCode::ADD, (InstructionData){"add", arithmeticFormat}},
    {SecondaryOpCode::ADDU, (InstructionData){"addu", arithmeticFormat}},
    {SecondaryOpCode::AND, (InstructionData){"and", arithmeticFormat}},
    {SecondaryOpCode::NOR, (InstructionData){"nor", arithmeticFormat}},
    {SecondaryOpCode::OR, (InstructionData){"or", arithmeticFormat}},
    {SecondaryOpCode::SLL, (InstructionData){"sll", "$%rd, $%rs, %imm"}},
    {SecondaryOpCode::SLT, (InstructionData){"slt", arithmeticFormat}},
    {SecondaryOpCode::SLTU, (InstructionData){"sltu", "$%rd, $%rs, %imm"}},
    {SecondaryOpCode::SUB, (InstructionData){"sub", arithmeticFormat}},
    {SecondaryOpCode::SUBU, (InstructionData){"subu", arithmeticFormat}},
    {SecondaryOpCode::XOR, (InstructionData){"xor", arithmeticFormat}},
};

static std::string formatAsHexBytes(uint32_t value) {
    return fmt::format("{:02x} {:02x} {:02x} {:02x}",
                        (value >> 24) & 0xFF,  // Most significant byte
                        (value >> 16) & 0xFF,
                        (value >> 8) & 0xFF,
                        value & 0xFF);         // Least significant byte
}

static std::string formatAssembly(uint32_t pc, Instruction i, const InstructionData &data)
{
    std::string result = data.name + " " + data.asmFormat;
    result = std::regex_replace(result, std::regex("%rs"), std::to_string(i.r.rs));
    result = std::regex_replace(result, std::regex("%rt"), std::to_string(i.r.rt));
    result = std::regex_replace(result, std::regex("%rd"), std::to_string(i.r.rd));
    result = std::regex_replace(result, std::regex("%shamt"), std::to_string(i.r.shamt));
    result = std::regex_replace(result, std::regex("%imm"), fmt::format("0x{:x}", (uint16_t)i.i.immediate));
    result = std::regex_replace(result, std::regex("%address"), fmt::format("0x{:x}", (uint32_t)i.j.address));

    result = fmt::format("0x{:08x}:  ", pc) + formatAsHexBytes(i.raw) + "  " + result;
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

std::string Disassembler::disassemble(uint32_t pc, Instruction i)
{
    auto primary = static_cast<PrimaryOpCode>(i.r.opcode);

    if (primary == PrimaryOpCode::SPECIAL)
    {
        return disassembleSpecial(pc, i);
    }
    if (primaryData.find(primary) != primaryData.end())
    {
        auto data = primaryData.at(primary);
        return formatAssembly(pc, i, data);
    }
    return std::string("MIPS Disassembler: Illegal instruction");
}
