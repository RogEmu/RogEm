#include <gtest/gtest.h>
#include "Utils.h"
#include "BIOS.h"
#include "Bus.h"
#include "CPU.h"

TEST(CpuTest, ADDI_1)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = 0xB16B00B5;
    int16_t imm = -1;

    loadImmediate(cpu, 11, value);
    i.i.rs = 11;
    i.i.rt = 8;
    i.i.immediate = imm;
    cpu.addImmediate(i);

    EXPECT_EQ(cpu.gpr[i.i.rt], 0xB16B00B4);
}

TEST(CpuTest, ADDI_2)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = 123;
    int16_t imm = 23768;

    loadImmediate(cpu, 11, value);
    i.i.rs = 11;
    i.i.rt = 8;
    i.i.immediate = imm;
    cpu.addImmediate(i);

    EXPECT_EQ(cpu.gpr[i.i.rt], value + imm);
}

TEST(CpuTest, ADDI_Negative_1)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = 0x1;
    int16_t imm = -1;

    loadImmediate(cpu, 11, value);
    i.i.rs = 11;
    i.i.rt = 8;
    i.i.immediate = imm;
    cpu.addImmediate(i);

    EXPECT_EQ(cpu.gpr[i.i.rt], 0);
}

TEST(CpuTest, ADDI_Negative_Overflow)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = INT32_MAX;
    int16_t imm = 1; // <-- Should overflow only in signed arithmetic

    loadImmediate(cpu, 11, value);
    i.i.rs = 11;
    i.i.rt = 8;
    i.i.immediate = imm;
    uint32_t oldRtValue = cpu.getReg(i.i.rt);
    cpu.addImmediate(i);

    EXPECT_EQ(cpu.gpr[i.i.rt], oldRtValue);
}

TEST(CpuTest, ADDI_Negative_No_Overflow)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = UINT32_MAX; // <-- Treated as signed (-1)
    int16_t imm = 1; // <-- Should not overflow in signed arithmetic

    loadImmediate(cpu, 11, value);
    i.i.rs = 11;
    i.i.rt = 8;
    i.i.immediate = imm;
    cpu.addImmediate(i);

    EXPECT_EQ(cpu.gpr[i.i.rt], 0);
}

TEST(CpuTest, ADDI_Negative_No_Underflow)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = 0;
    int16_t imm = -1; // <-- Should not underflow in signed arithmetic

    loadImmediate(cpu, 11, value);
    i.i.rs = 11;
    i.i.rt = 8;
    i.i.immediate = imm;
    cpu.addImmediate(i);

    EXPECT_EQ(cpu.gpr[i.i.rt], -1);
}

TEST(CpuTest, ADDI_Negative_Underflow)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = (uint32_t)INT32_MIN;
    int16_t imm = -1; // <-- Should underflow in signed arithmetic

    loadImmediate(cpu, 8, value);
    i.i.rs = 8; // $t0
    i.i.rt = 9; // $t1
    i.i.immediate = imm;
    uint32_t oldRtValue = cpu.getReg(i.i.rt);
    cpu.addImmediate(i);

    EXPECT_EQ(cpu.gpr[i.i.rt], oldRtValue);
}

TEST(CpuTest, ADDIU_1)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = 0xC0FFEE;
    int16_t imm = -1;

    loadImmediate(cpu, 8, value);
    i.i.rs = 8;
    i.i.rt = 9;
    i.i.immediate = imm;
    cpu.addImmediateUnsigned(i);

    EXPECT_EQ(cpu.gpr[i.i.rt], value + imm);
}

TEST(CpuTest, ADDIU_2)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = 0xBADBABE;
    int16_t imm = INT16_MIN;

    loadImmediate(cpu, 8, value);
    i.i.rs = 8;
    i.i.rt = 9;
    i.i.immediate = imm;
    cpu.addImmediateUnsigned(i);

    EXPECT_EQ(cpu.gpr[i.i.rt], value + imm);
}

TEST(CpuTest, ADDIU_No_Overflow)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = INT32_MAX;
    int16_t imm = 1;

    loadImmediate(cpu, 8, value);
    i.i.rs = 8;
    i.i.rt = 9;
    i.i.immediate = imm;
    cpu.addImmediateUnsigned(i);

    EXPECT_EQ(cpu.gpr[i.i.rt], value + imm);
}

TEST(CpuTest, ADDIU_No_Underflow)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = 0;
    int16_t imm = -1;

    loadImmediate(cpu, 8, value);
    i.i.rs = 8;
    i.i.rt = 9;
    i.i.immediate = imm;
    cpu.addImmediateUnsigned(i);

    EXPECT_EQ(cpu.gpr[i.i.rt], value + imm);
}

TEST(CpuTest, ADDIU_No_Underflow_2)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = (uint32_t)INT32_MIN;
    int16_t imm = -1;

    loadImmediate(cpu, 8, value);
    i.i.rs = 8;
    i.i.rt = 9;
    i.i.immediate = imm;
    cpu.addImmediateUnsigned(i);

    EXPECT_EQ(cpu.gpr[i.i.rt], value + imm);
}

TEST(CpuTest, ADDIU_3)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = 0;
    int16_t imm = 12322;

    loadImmediate(cpu, 0, value);
    i.i.rs = 0;
    i.i.rt = 8;
    i.i.immediate = imm;
    cpu.addImmediateUnsigned(i);

    EXPECT_EQ(cpu.gpr[i.i.rt], value + imm);
}

TEST(CpuTest, ADD_Normal)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value1 = 100;
    uint32_t value2 = 200;

    loadImmediate(cpu, 1, value1);
    loadImmediate(cpu, 2, value2);
    i.r.rs = 1;
    i.r.rt = 2;
    i.r.rd = 3;

    cpu.addWord(i);

    EXPECT_EQ(cpu.gpr[i.r.rd], value1 + value2);
}

TEST(CpuTest, ADD_Zero)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    loadImmediate(cpu, 1, 0);
    loadImmediate(cpu, 2, 0);
    i.r.rs = 1;
    i.r.rt = 2;
    i.r.rd = 3;

    cpu.addWord(i);

    EXPECT_EQ(cpu.gpr[i.r.rd], 0);
}

TEST(CpuTest, ADD_Negative)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    int32_t value1 = -50;
    int32_t value2 = 30;

    loadImmediate(cpu, 1, value1);
    loadImmediate(cpu, 2, value2);
    i.r.rs = 1;
    i.r.rt = 2;
    i.r.rd = 3;

    cpu.addWord(i);

    EXPECT_EQ(cpu.gpr[i.r.rd], value1 + value2);
}

TEST(CpuTest, ADD_Overflow_Positive)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    int32_t value1 = INT32_MAX;
    int32_t value2 = 1;
    int32_t initial_rd_value = 0xDEADBEEF; // Known value for verification

    loadImmediate(cpu, 1, value1);
    loadImmediate(cpu, 2, value2);
    loadImmediate(cpu, 3, initial_rd_value); // Initialize rd
    i.r.rs = 1;
    i.r.rt = 2;
    i.r.rd = 3;

    cpu.addWord(i);

    EXPECT_EQ(cpu.gpr[i.r.rd], initial_rd_value); // rd should remain unchanged
}

TEST(CpuTest, ADD_Overflow_Negative)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    int32_t value1 = INT32_MIN;
    int32_t value2 = -1;
    int32_t initial_rd_value = 0xDEADBEEF; // Known value for verification

    loadImmediate(cpu, 1, value1);
    loadImmediate(cpu, 2, value2);
    loadImmediate(cpu, 3, initial_rd_value); // Initialize rd
    i.r.rs = 1;
    i.r.rt = 2;
    i.r.rd = 3;

    cpu.addWord(i);

    EXPECT_EQ(cpu.gpr[i.r.rd], initial_rd_value); // rd should remain unchanged
}

TEST(CpuTest, SUB_1)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
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

    EXPECT_EQ(cpu.gpr[i.r.rd], left - right);
}

TEST(CpuTest, SUB_Overflow)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
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

    EXPECT_EQ(cpu.gpr[i.r.rd], oldRd);
}

TEST(CpuTest, SUB_Overflow_2)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
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

    EXPECT_EQ(cpu.gpr[i.r.rd], oldRd);
}

TEST(CpuTest, SUB_Underflow)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
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

    EXPECT_EQ(cpu.gpr[i.r.rd], oldRd);
}

TEST(CpuTest, SUB_Underflow_2)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
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

    EXPECT_EQ(cpu.gpr[i.r.rd], oldRd);
}

TEST(CpuTest, SUB_No_Overflow)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
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

    EXPECT_EQ(cpu.gpr[i.r.rd], left - right);
}

TEST(CpuTest, SUB_No_Overflow_2)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
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

    EXPECT_EQ(cpu.gpr[i.r.rd], left - right);
}

TEST(CpuTest, SUBU_1)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
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

    EXPECT_EQ(cpu.gpr[i.r.rd], left - right);
}

TEST(CpuTest, SUBU_2)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
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

    EXPECT_EQ(cpu.gpr[i.r.rd], left - right);
}

TEST(CpuTest, SUBU_3)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
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

    EXPECT_EQ(cpu.gpr[i.r.rd], left - right);
}

TEST(CpuTest, SUBU_4)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
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

    EXPECT_EQ(cpu.gpr[i.r.rd], left - right);
}

TEST(CpuTest, SUBU_No_Overflow)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
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

    EXPECT_EQ(cpu.gpr[i.r.rd], left - right);
}

TEST(CpuTest, SUBU_No_Overflow_2)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
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

    EXPECT_EQ(cpu.gpr[i.r.rd], left - right);
}
