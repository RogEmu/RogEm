#include <gtest/gtest.h>
#include "Utils.h"

TEST(CpuTest, ADDI_1)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = 0xB16B00B5;
    int16_t imm = -1;

    loadImmediate(cpu, 11, value);
    i.i.rs = 11;
    i.i.rt = 8;
    i.i.immediate = imm;
    cpu.addImmediate(i);

    EXPECT_EQ(cpu.m_registers[i.i.rt], 0xB16B00B4);
}

TEST(CpuTest, ADDI_2)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = 123;
    int16_t imm = 23768;

    loadImmediate(cpu, 11, value);
    i.i.rs = 11;
    i.i.rt = 8;
    i.i.immediate = imm;
    cpu.addImmediate(i);

    EXPECT_EQ(cpu.m_registers[i.i.rt], value + imm);
}

TEST(CpuTest, ADDI_Negative_1)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = 0x1;
    int16_t imm = -1;

    loadImmediate(cpu, 11, value);
    i.i.rs = 11;
    i.i.rt = 8;
    i.i.immediate = imm;
    cpu.addImmediate(i);

    EXPECT_EQ(cpu.m_registers[i.i.rt], 0);
}

TEST(CpuTest, ADDI_Negative_Overflow)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = INT32_MAX;
    int16_t imm = 1; // <-- Should overflow only in signed arithmetic

    loadImmediate(cpu, 11, value);
    i.i.rs = 11;
    i.i.rt = 8;
    i.i.immediate = imm;
    uint32_t oldRtValue = cpu.getReg(i.i.rt);
    cpu.addImmediate(i);

    EXPECT_EQ(cpu.m_registers[i.i.rt], oldRtValue);
}

TEST(CpuTest, ADDI_Negative_No_Overflow)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = UINT32_MAX; // <-- Treated as signed (-1)
    int16_t imm = 1; // <-- Should not overflow in signed arithmetic

    loadImmediate(cpu, 11, value);
    i.i.rs = 11;
    i.i.rt = 8;
    i.i.immediate = imm;
    cpu.addImmediate(i);

    EXPECT_EQ(cpu.m_registers[i.i.rt], 0);
}

TEST(CpuTest, ADDI_Negative_No_Underflow)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = 0;
    int16_t imm = -1; // <-- Should not underflow in signed arithmetic

    loadImmediate(cpu, 11, value);
    i.i.rs = 11;
    i.i.rt = 8;
    i.i.immediate = imm;
    cpu.addImmediate(i);

    EXPECT_EQ(cpu.m_registers[i.i.rt], -1);
}

TEST(CpuTest, ADDI_Negative_Underflow)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = (uint32_t)INT32_MIN;
    int16_t imm = -1; // <-- Should underflow in signed arithmetic

    loadImmediate(cpu, 8, value);
    i.i.rs = 8; // $t0
    i.i.rt = 9; // $t1
    i.i.immediate = imm;
    uint32_t oldRtValue = cpu.getReg(i.i.rt);
    cpu.addImmediate(i);

    EXPECT_EQ(cpu.m_registers[i.i.rt], oldRtValue);
}

TEST(CpuTest, ADDIU_1)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = 0xC0FFEE;
    int16_t imm = -1;

    loadImmediate(cpu, 8, value);
    i.i.rs = 8;
    i.i.rt = 9;
    i.i.immediate = imm;
    cpu.addImmediateUnsigned(i);

    EXPECT_EQ(cpu.m_registers[i.i.rt], value + imm);
}

TEST(CpuTest, ADDIU_2)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = 0xBADBABE;
    int16_t imm = INT16_MIN;

    loadImmediate(cpu, 8, value);
    i.i.rs = 8;
    i.i.rt = 9;
    i.i.immediate = imm;
    cpu.addImmediateUnsigned(i);

    EXPECT_EQ(cpu.m_registers[i.i.rt], value + imm);
}

TEST(CpuTest, ADDIU_No_Overflow)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = INT32_MAX;
    int16_t imm = 1;

    loadImmediate(cpu, 8, value);
    i.i.rs = 8;
    i.i.rt = 9;
    i.i.immediate = imm;
    cpu.addImmediateUnsigned(i);

    EXPECT_EQ(cpu.m_registers[i.i.rt], value + imm);
}

TEST(CpuTest, ADDIU_No_Underflow)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = 0;
    int16_t imm = -1;

    loadImmediate(cpu, 8, value);
    i.i.rs = 8;
    i.i.rt = 9;
    i.i.immediate = imm;
    cpu.addImmediateUnsigned(i);

    EXPECT_EQ(cpu.m_registers[i.i.rt], value + imm);
}

TEST(CpuTest, ADDIU_No_Underflow_2)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t value = (uint32_t)INT32_MIN;
    int16_t imm = -1;

    loadImmediate(cpu, 8, value);
    i.i.rs = 8;
    i.i.rt = 9;
    i.i.immediate = imm;
    cpu.addImmediateUnsigned(i);

    EXPECT_EQ(cpu.m_registers[i.i.rt], value + imm);
}

TEST(CpuTest, SUB_1)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t left = 36178;
    uint32_t right = 9403875;

    loadImmediate(cpu, 8, left);
    loadImmediate(cpu, 9, right);
    // rd = rs - rt;
    i.r.rd = 10;
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.substractWord(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], left - right);
}

TEST(CpuTest, SUB_Overflow)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    // Should overflow into negative integers
    uint32_t left = INT32_MAX;
    uint32_t right = (uint32_t)-1;

    uint32_t oldRd = cpu.getReg(10);
    loadImmediate(cpu, 8, left);
    loadImmediate(cpu, 9, right);
    // rd = rs - rt;
    i.r.rd = 10;
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.substractWord(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], oldRd);
}

TEST(CpuTest, SUB_Overflow_2)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    // Should overflow into negative integers
    uint32_t left = INT32_MAX;
    uint32_t right = (uint32_t)-79879;

    uint32_t oldRd = cpu.getReg(10);
    loadImmediate(cpu, 8, left);
    loadImmediate(cpu, 9, right);
    // rd = rs - rt;
    i.r.rd = 10;
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.substractWord(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], oldRd);
}

TEST(CpuTest, SUB_Underflow)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    // Should overflow into negative integers
    uint32_t left = (uint32_t)INT32_MIN;
    uint32_t right = 1;

    uint32_t oldRd = cpu.getReg(10);
    loadImmediate(cpu, 8, left);
    loadImmediate(cpu, 9, right);
    // rd = rs - rt;
    i.r.rd = 10;
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.substractWord(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], oldRd);
}

TEST(CpuTest, SUB_Underflow_2)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    // Should overflow into negative integers
    uint32_t left = (uint32_t)INT32_MIN;
    uint32_t right = 8790870;

    uint32_t oldRd = cpu.getReg(10);
    loadImmediate(cpu, 8, left);
    loadImmediate(cpu, 9, right);
    // rd = rs - rt;
    i.r.rd = 10;
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.substractWord(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], oldRd);
}

TEST(CpuTest, SUB_No_Overflow)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    // Should overflow into negative integers
    uint32_t left = 1;
    uint32_t right = 2;

    loadImmediate(cpu, 8, left);
    loadImmediate(cpu, 9, right);
    // rd = rs - rt;
    i.r.rd = 10;
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.substractWord(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], left - right);
}

TEST(CpuTest, SUB_No_Overflow_2)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    // Should overflow into negative integers
    uint32_t left = (uint32_t)-1;
    uint32_t right = (uint32_t)-2;

    loadImmediate(cpu, 8, left);
    loadImmediate(cpu, 9, right);
    // rd = rs - rt;
    i.r.rd = 10;
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.substractWord(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], left - right);
}

TEST(CpuTest, SUBU_1)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t left = 986648768;
    uint32_t right = (uint32_t)-8790870;

    loadImmediate(cpu, 8, left);
    loadImmediate(cpu, 9, right);
    // rd = rs - rt;
    i.r.rd = 10;
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.substractWordUnsigned(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], left - right);
}

TEST(CpuTest, SUBU_2)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t left = (uint32_t)-891840298;
    uint32_t right = 8790870;

    loadImmediate(cpu, 8, left);
    loadImmediate(cpu, 9, right);
    // rd = rs - rt;
    i.r.rd = 10;
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.substractWordUnsigned(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], left - right);
}

TEST(CpuTest, SUBU_3)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t left = (uint32_t)-891840298;
    uint32_t right = (uint32_t)-8790870;

    loadImmediate(cpu, 8, left);
    loadImmediate(cpu, 9, right);
    // rd = rs - rt;
    i.r.rd = 10;
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.substractWordUnsigned(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], left - right);
}

TEST(CpuTest, SUBU_4)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t left = (uint32_t)-891840298;
    uint32_t right = 8790870;

    loadImmediate(cpu, 8, left);
    loadImmediate(cpu, 9, right);
    // rd = rs - rt;
    i.r.rd = 10;
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.substractWordUnsigned(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], left - right);
}

TEST(CpuTest, SUBU_No_Overflow)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t left = INT32_MAX;
    uint32_t right = (uint32_t)-1;

    loadImmediate(cpu, 8, left);
    loadImmediate(cpu, 9, right);
    // rd = rs - rt;
    i.r.rd = 10;
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.substractWordUnsigned(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], left - right);
}

TEST(CpuTest, SUBU_No_Overflow_2)
{
    BIOS bios;
    Bus bus(bios);
    CPU cpu(bus);
    Instruction i;

    uint32_t left = (uint32_t)INT32_MIN;
    uint32_t right = 1;

    loadImmediate(cpu, 8, left);
    loadImmediate(cpu, 9, right);
    // rd = rs - rt;
    i.r.rd = 10;
    i.r.rs = 8;
    i.r.rt = 9;
    cpu.substractWordUnsigned(i);

    EXPECT_EQ(cpu.m_registers[i.r.rd], left - right);
}
