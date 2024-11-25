#include <gtest/gtest.h>
#include "Utils.h"

TEST(CpuTest, SLLV_1)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = 0x9F0DE23B;
    uint8_t shiftAmount = 7;
    loadImmediate(cpu, 8, value); // Value that will be shifted in register $8
    loadImmediate(cpu, 9, shiftAmount); // Shift amount
    i.r.rs = 9;
    i.r.rt = 8;
    i.r.rd = 8;
    cpu.shiftLeftLogicalVariable(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], value << (shiftAmount & 0x1F));
}

TEST(CpuTest, SLLV_2)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = 0x9F0DE23B;
    uint8_t shiftAmount = 178; // Bigger shift value should be truncated
    loadImmediate(cpu, 8, value); // Value that will be shifted in register $8
    loadImmediate(cpu, 9, shiftAmount); // Shift amount
    i.r.rs = 9;
    i.r.rt = 8;
    i.r.rd = 8;
    cpu.shiftLeftLogicalVariable(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], value << (shiftAmount & 0x1F));
}

TEST(CpuTest, SLLV_No_Shift)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = 0x9F0DE23B;
    uint8_t shiftAmount = 0;
    loadImmediate(cpu, 8, value); // Value that will be shifted in register $8
    loadImmediate(cpu, 9, shiftAmount); // Shift amount
    i.r.rs = 9;
    i.r.rt = 8;
    i.r.rd = 8;
    cpu.shiftLeftLogicalVariable(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], value);
}

TEST(CpuTest, SLL_1)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    // sll  rd,rt,imm         rd = rt SHL (00h..1Fh)
    uint32_t value = 0x9F0DE23B;
    uint8_t shiftAmount = 21;
    loadImmediate(cpu, 8, value); // Value that will be shifted in register $8
    i.r.rd = 9;
    i.r.rt = 8;
    i.r.shamt = shiftAmount;
    cpu.shiftLeftLogical(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], value << shiftAmount);
}

TEST(CpuTest, SLL_Large_Shift)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    // sll  rd,rt,imm         rd = rt SHL (00h..1Fh)
    uint32_t value = 0x9F0DE23B;
    uint8_t shiftAmount = 43;
    loadImmediate(cpu, 8, value); // Value that will be shifted in register $8
    i.r.rd = 9;
    i.r.rt = 8;
    i.r.shamt = shiftAmount;
    cpu.shiftLeftLogical(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], value << (shiftAmount & 0x1F));
}

TEST(CpuTest, SRL_1)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = 0xFFFFAAAA;
    uint8_t shiftAmount = 12;
    loadImmediate(cpu, 8, value); // Value that will be shifted in register $8
    i.r.rt = 8;
    i.r.rd = 9;
    i.r.shamt = shiftAmount;
    cpu.shiftRightLogical(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], value >> (shiftAmount & 0x1F));
}

TEST(CpuTest, SRL_Large_Shift)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = 0xFFFFAAAA;
    uint8_t shiftAmount = 78;
    loadImmediate(cpu, 8, value); // Value that will be shifted in register $8
    i.r.rt = 8;
    i.r.rd = 9;
    i.r.shamt = shiftAmount;
    cpu.shiftRightLogical(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], value >> (shiftAmount & 0x1F));
}
