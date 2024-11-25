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

TEST(CpuTest, SRLV_1)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = 0x9F0DE23B;
    uint8_t shiftAmount = 13; // Bigger shift value should be truncated
    loadImmediate(cpu, 8, value); // Value that will be shifted in register $8
    loadImmediate(cpu, 9, shiftAmount); // Shift amount
    i.r.rs = 9;
    i.r.rt = 8;
    i.r.rd = 8;
    cpu.shiftRightLogicalVariable(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], value >> (shiftAmount & 0x1F));
}

TEST(CpuTest, SRLV_Large_Shift)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = 0x9F0DE23B;
    uint32_t shiftAmount = 78127942; // Bigger shift value should be truncated
    loadImmediate(cpu, 8, value); // Value that will be shifted in register $8
    loadImmediate(cpu, 9, shiftAmount); // Shift amount
    i.r.rs = 9;
    i.r.rt = 8;
    i.r.rd = 8;
    cpu.shiftRightLogicalVariable(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], value >> (shiftAmount & 0x1F));
}

TEST(CpuTest, SRLV_No_Shift)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = 0x9F0DE23B;
    uint32_t shiftAmount = 0; // Bigger shift value should be truncated
    loadImmediate(cpu, 8, value); // Value that will be shifted in register $8
    loadImmediate(cpu, 9, shiftAmount); // Shift amount
    i.r.rs = 9;
    i.r.rt = 8;
    i.r.rd = 8;
    cpu.shiftRightLogicalVariable(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], value);
}

TEST(CpuTest, SRA_MSB_Negative)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = 0x80000000;
    uint32_t shiftAmount = 31; // Should shift and get all ones
    loadImmediate(cpu, 8, value); // Value that will be shifted in register $8
    i.r.rt = 8;
    i.r.rd = 8;
    i.r.shamt = shiftAmount;
    cpu.shiftRightArithmetic(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], (uint32_t)(((int32_t)value) >> shiftAmount));
}


TEST(CpuTest, SRA_MSB_Positive)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = 0x40000000;
    uint32_t shiftAmount = 31; // Should shift all zeroes
    loadImmediate(cpu, 8, value); // Value that will be shifted in register $8
    i.r.rt = 8;
    i.r.rd = 8;
    i.r.shamt = shiftAmount;
    cpu.shiftRightArithmetic(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], 0);
}

TEST(CpuTest, SRA_MSB_Positive_2)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = 0x40000000;
    uint32_t shiftAmount = 30; // Should shift and result in 1
    loadImmediate(cpu, 8, value); // Value that will be shifted in register $8
    i.r.rt = 8;
    i.r.rd = 8;
    i.r.shamt = shiftAmount;
    cpu.shiftRightArithmetic(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], 1);
}

TEST(CpuTest, SRAV_MSB_Negative)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = 0x80000000;
    uint32_t shiftAmount = 31; // Should shift all 1s
    loadImmediate(cpu, 8, value); // Value that will be shifted in register $8
    loadImmediate(cpu, 9, shiftAmount); // Shift amount
    i.r.rs = 9;
    i.r.rt = 8;
    i.r.rd = 8;
    cpu.shiftRightArithmeticVariable(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], 0xFFFFFFFF);
}

TEST(CpuTest, SRAV_MSB_Positive)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = 0x40000000;
    uint32_t shiftAmount = 30; // Should shift all 1s
    loadImmediate(cpu, 8, value); // Value that will be shifted in register $8
    loadImmediate(cpu, 9, shiftAmount); // Shift amount
    i.r.rs = 9;
    i.r.rt = 8;
    i.r.rd = 8;
    cpu.shiftRightArithmeticVariable(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], 1);
}
