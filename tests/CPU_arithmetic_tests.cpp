#include <gtest/gtest.h>

#include "Core/BIOS.hpp"
#include "Core/Bus.hpp"
#include "Core/CPU.hpp"
#include "Core/RAM.hpp"

class CpuArithmeticTest : public testing::Test
{
    protected:
        Bus bus;
        CPU cpu;

        const uint32_t defaultRegVal = 0xDEADBEEF;

        CpuArithmeticTest() :
            cpu(&bus)
        {
            cpu.setReg(CpuReg::PC, 0x10000);
        }

        void runArithmeticTest(SecondaryOpCode opcode, uint32_t left, uint32_t right, uint32_t expected)
        {
            Instruction i;
            i.r.opcode = static_cast<uint8_t>(PrimaryOpCode::SPECIAL);
            i.r.rd = static_cast<uint8_t>(CpuReg::T0);
            i.r.rs = static_cast<uint8_t>(CpuReg::T1);
            i.r.rt = static_cast<uint8_t>(CpuReg::T2);
            i.r.funct = static_cast<uint8_t>(opcode);

            auto pc = cpu.getReg(CpuReg::PC);
            bus.storeWord(pc, i.raw);

            cpu.setReg(CpuReg::T0, defaultRegVal);
            cpu.setReg(CpuReg::T1, left);
            cpu.setReg(CpuReg::T2, right);
            cpu.step();

            EXPECT_EQ(cpu.getReg(CpuReg::T0), expected);
        }

        void runArithmeticImmediateTest(PrimaryOpCode opcode, uint32_t value, uint16_t immediate, uint32_t expected)
        {
            Instruction i;
            i.i.opcode = static_cast<uint8_t>(opcode);
            i.i.rt = static_cast<uint8_t>(CpuReg::T0);
            i.i.rs = static_cast<uint8_t>(CpuReg::T1);
            i.i.immediate = immediate;

            auto pc = cpu.getReg(CpuReg::PC);
            bus.storeWord(pc, i.raw);

            cpu.setReg(CpuReg::T0, defaultRegVal);
            cpu.setReg(CpuReg::T1, value);
            cpu.step();

            EXPECT_EQ(cpu.getReg(CpuReg::T0), expected);
        }
};

TEST_F(CpuArithmeticTest, ADDI_Basic)
{
    uint32_t value = 123;
    int16_t imm = 0x5CD8;
    uint32_t expected = value + imm;
    runArithmeticImmediateTest(PrimaryOpCode::ADDI, value, imm, expected);
}

TEST_F(CpuArithmeticTest, ADDI_Negative_Immediate)
{
    uint32_t value = 0x10;
    int16_t imm = -4189;
    uint32_t expected = value + imm;
    runArithmeticImmediateTest(PrimaryOpCode::ADDI, value, imm, expected);
}

TEST_F(CpuArithmeticTest, ADDI_Immediate_Minimum)
{
    uint32_t value = 0x10;
    int16_t imm = INT16_MIN;
    uint32_t expected = value + imm;
    runArithmeticImmediateTest(PrimaryOpCode::ADDI, value, imm, expected);
}

TEST_F(CpuArithmeticTest, ADDI_Immediate_Maximum)
{
    uint32_t value = 0x10;
    int16_t imm = INT16_MAX;
    uint32_t expected = value + imm;
    runArithmeticImmediateTest(PrimaryOpCode::ADDI, value, imm, expected);
}

TEST_F(CpuArithmeticTest, ADDI_Overflow)
{
    uint32_t value = INT32_MAX;
    int16_t imm = 1;
    uint32_t expected = defaultRegVal; // Overflow expected so default $t0 value expected
    runArithmeticImmediateTest(PrimaryOpCode::ADDI, value, imm, expected);
}

TEST_F(CpuArithmeticTest, ADDI_No_Overflow)
{
    uint32_t value = UINT32_MAX;
    int16_t imm = 1;
    uint32_t expected = value + imm;
    runArithmeticImmediateTest(PrimaryOpCode::ADDI, value, imm, expected);
}

TEST_F(CpuArithmeticTest, ADDI_No_Underflow)
{
    uint32_t value = 0;
    int16_t imm = -1;
    uint32_t expected = value + imm;
    runArithmeticImmediateTest(PrimaryOpCode::ADDI, value, imm, expected);
}

TEST_F(CpuArithmeticTest, ADDI_Underflow)
{
    uint32_t value = static_cast<uint32_t>(INT32_MIN);
    int16_t imm = -1;
    uint32_t expected = defaultRegVal; // Overflow expected so default $t0 value expected
    runArithmeticImmediateTest(PrimaryOpCode::ADDI, value, imm, expected);
}

TEST_F(CpuArithmeticTest, ADDIU_Basic)
{
    uint32_t value = 0xBADBABE;
    int16_t imm = 0x429;
    uint32_t expected = value + imm;
    runArithmeticImmediateTest(PrimaryOpCode::ADDIU, value, imm, expected);
}

TEST_F(CpuArithmeticTest, ADDIU_Immediate_Minimum)
{
    uint32_t value = 0xBADBABE;
    int16_t imm = INT16_MIN;
    uint32_t expected = value + imm;
    runArithmeticImmediateTest(PrimaryOpCode::ADDIU, value, imm, expected);
}

TEST_F(CpuArithmeticTest, ADDIU_Immediate_Maximum)
{
    uint32_t value = 0xBADBABE;
    int16_t imm = INT16_MAX;
    uint32_t expected = value + imm;
    runArithmeticImmediateTest(PrimaryOpCode::ADDIU, value, imm, expected);
}

TEST_F(CpuArithmeticTest, ADDIU_No_Overflow)
{
    uint32_t value = static_cast<uint32_t>(INT32_MAX);
    int16_t imm = 1;
    uint32_t expected = value + imm;
    runArithmeticImmediateTest(PrimaryOpCode::ADDIU, value, imm, expected);
}

TEST_F(CpuArithmeticTest, ADDIU_No_Overflow_2)
{
    uint32_t value = static_cast<uint32_t>(-1);
    int16_t imm = 2;
    uint32_t expected = value + imm;
    runArithmeticImmediateTest(PrimaryOpCode::ADDIU, value, imm, expected);
}

TEST_F(CpuArithmeticTest, ADDIU_No_Underflow)
{
    uint32_t value = 0;
    int16_t imm = -1;
    uint32_t expected = static_cast<uint32_t>(-1);
    runArithmeticImmediateTest(PrimaryOpCode::ADDIU, value, imm, expected);
}

TEST_F(CpuArithmeticTest, ADDIU_No_Underflow_2)
{
    uint32_t value = static_cast<uint32_t>(INT32_MIN);
    int16_t imm = -1;
    uint32_t expected = value + imm;
    runArithmeticImmediateTest(PrimaryOpCode::ADDIU, value, imm, expected);
}

TEST_F(CpuArithmeticTest, ADD_Basic)
{
    uint32_t vleft = 100;
    uint32_t vright = 200;
    uint32_t expected = vleft + vright;
    runArithmeticTest(SecondaryOpCode::ADD, vleft, vright, expected);
}

TEST_F(CpuArithmeticTest, ADD_Zeroed_Registers)
{
    uint32_t vleft = 0;
    uint32_t vright = 0;
    uint32_t expected = 0;
    runArithmeticTest(SecondaryOpCode::ADD, vleft, vright, expected);
}

TEST_F(CpuArithmeticTest, ADD_Negative_Positive)
{
    uint32_t vleft = static_cast<uint32_t>(-50);
    uint32_t vright = 30;
    uint32_t expected = vleft + vright;
    runArithmeticTest(SecondaryOpCode::ADD, vleft, vright, expected);
}

TEST_F(CpuArithmeticTest, ADD_Overflow)
{
    uint32_t vleft = static_cast<uint32_t>(INT32_MAX);
    uint32_t vright = 1;
    uint32_t expected = defaultRegVal;
    runArithmeticTest(SecondaryOpCode::ADD, vleft, vright, expected);
}

TEST_F(CpuArithmeticTest, ADD_Underflow)
{
    uint32_t vleft = static_cast<uint32_t>(INT32_MIN);
    uint32_t vright = static_cast<uint32_t>(-1);
    uint32_t expected = defaultRegVal;
    runArithmeticTest(SecondaryOpCode::ADD, vleft, vright, expected);
}

TEST_F(CpuArithmeticTest, ADDU_Basic)
{
    uint32_t vleft = 100;
    uint32_t vright = 200;
    uint32_t expected = vleft + vright;
    runArithmeticTest(SecondaryOpCode::ADDU, vleft, vright, expected);
}

TEST_F(CpuArithmeticTest, ADDU_Zeroed_Registers)
{
    uint32_t vleft = 0;
    uint32_t vright = 0;
    uint32_t expected = 0;
    runArithmeticTest(SecondaryOpCode::ADDU, vleft, vright, expected);
}

TEST_F(CpuArithmeticTest, ADDU_Negative)
{
    uint32_t vleft = static_cast<uint32_t>(-50);
    uint32_t vright = 30;
    uint32_t expected = vleft + vright;
    runArithmeticTest(SecondaryOpCode::ADDU, vleft, vright, expected);
}

TEST_F(CpuArithmeticTest, ADDU_No_Overflow)
{
    uint32_t vleft = UINT32_MAX;
    uint32_t vright = 1;
    uint32_t expected = 0;
    runArithmeticTest(SecondaryOpCode::ADDU, vleft, vright, expected);
}

TEST_F(CpuArithmeticTest, SUB_Basic)
{
    uint32_t vleft = 36178;
    uint32_t vright = 9403875;
    uint32_t expected = vleft - vright;
    runArithmeticTest(SecondaryOpCode::SUB, vleft, vright, expected);
}

TEST_F(CpuArithmeticTest, SUB_Overflow)
{
    uint32_t vleft = static_cast<uint32_t>(INT32_MAX);
    uint32_t vright = static_cast<uint32_t>(-10);
    uint32_t expected = defaultRegVal;
    runArithmeticTest(SecondaryOpCode::SUB, vleft, vright, expected);
}

TEST_F(CpuArithmeticTest, SUB_Underflow)
{
    uint32_t vleft = static_cast<uint32_t>(INT32_MIN);
    uint32_t vright = static_cast<uint32_t>(10);
    uint32_t expected = defaultRegVal;
    runArithmeticTest(SecondaryOpCode::SUB, vleft, vright, expected);
}

TEST_F(CpuArithmeticTest, SUB_No_Underflow)
{
    uint32_t vleft = 1;
    uint32_t vright = 2;
    uint32_t expected = static_cast<uint32_t>(-1);
    runArithmeticTest(SecondaryOpCode::SUB, vleft, vright, expected);
}

TEST_F(CpuArithmeticTest, SUB_No_Overflow)
{
    uint32_t vleft = static_cast<uint32_t>(-1);
    uint32_t vright = static_cast<uint32_t>(-2);
    uint32_t expected = 1;
    runArithmeticTest(SecondaryOpCode::SUB, vleft, vright, expected);
}

TEST_F(CpuArithmeticTest, SUBU_Basic)
{
    uint32_t vleft = static_cast<uint32_t>(986648768);
    uint32_t vright = static_cast<uint32_t>(-8790870);
    uint32_t expected = vleft - vright;
    runArithmeticTest(SecondaryOpCode::SUBU, vleft, vright, expected);
}

TEST_F(CpuArithmeticTest, SUBU_Basic_Reverse_Sign)
{
    uint32_t vleft = static_cast<uint32_t>(-986648768);
    uint32_t vright = static_cast<uint32_t>(8790870);
    uint32_t expected = vleft - vright;
    runArithmeticTest(SecondaryOpCode::SUBU, vleft, vright, expected);
}

TEST_F(CpuArithmeticTest, SUBU_Both_Negative)
{
    uint32_t vleft = static_cast<uint32_t>(-891840298);
    uint32_t vright = static_cast<uint32_t>(-8790870);
    uint32_t expected = vleft - vright;
    runArithmeticTest(SecondaryOpCode::SUBU, vleft, vright, expected);
}

TEST_F(CpuArithmeticTest, SUBU_No_Overflow)
{
    uint32_t vleft = static_cast<uint32_t>(INT32_MAX);
    uint32_t vright = static_cast<uint32_t>(-1);
    uint32_t expected = vleft - vright;
    runArithmeticTest(SecondaryOpCode::SUBU, vleft, vright, expected);
}

TEST_F(CpuArithmeticTest, SUBU_No_Underflow)
{
    uint32_t vleft = static_cast<uint32_t>(INT32_MIN);
    uint32_t vright = 1;
    uint32_t expected = vleft - vright;
    runArithmeticTest(SecondaryOpCode::SUBU, vleft, vright, expected);
}
