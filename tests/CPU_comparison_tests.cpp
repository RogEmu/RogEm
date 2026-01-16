#include <gtest/gtest.h>

#include "Core/BIOS.hpp"
#include "Core/Bus.hpp"
#include "Core/CPU.hpp"
#include "Core/RAM.hpp"

class CpuComparisonTest : public testing::Test
{
    protected:
        Bus bus;
        CPU cpu;

        const uint32_t defaultRegVal = 0xDEADBEEF;

        CpuComparisonTest() :
            cpu(&bus)
        {
            cpu.setReg(CpuReg::PC, 0x10000);
        }

        void runCompTest(SecondaryOpCode opcode, uint32_t rs, uint32_t rt, bool expectedRd)
        {
            Instruction i;
            i.r.opcode = static_cast<uint8_t>(PrimaryOpCode::SPECIAL);
            i.r.rs = static_cast<uint8_t>(CpuReg::T0);
            i.r.rt = static_cast<uint8_t>(CpuReg::T1);
            i.r.rd = static_cast<uint8_t>(CpuReg::T2);
            i.r.funct = static_cast<uint8_t>(opcode);

            auto pc = cpu.getReg(CpuReg::PC);
            bus.store<uint32_t>(pc, i.raw);
            cpu.setReg(CpuReg::T0, rs);
            cpu.setReg(CpuReg::T1, rt);
            cpu.setReg(CpuReg::T2, defaultRegVal);

            cpu.step();

            EXPECT_EQ(cpu.getReg(CpuReg::T2), expectedRd);
        }

        void runCompImmediateTest(PrimaryOpCode opcode, uint32_t rs, uint16_t immediate, bool expectedRt)
        {
            Instruction i;
            i.i.opcode = static_cast<uint8_t>(opcode);
            i.i.rs = static_cast<uint8_t>(CpuReg::T0);
            i.i.rt = static_cast<uint8_t>(CpuReg::T1);
            i.i.immediate = immediate;

            auto pc = cpu.getReg(CpuReg::PC);
            bus.store<uint32_t>(pc, i.raw);
            cpu.setReg(CpuReg::T0, rs);
            cpu.setReg(CpuReg::T1, defaultRegVal);

            cpu.step();

            EXPECT_EQ(cpu.getReg(CpuReg::T1), expectedRt);
        }
};

TEST_F(CpuComparisonTest, SLT_SetSameSign)
{
    uint32_t rs = 139;
    uint32_t rt = 1380;
    runCompTest(SecondaryOpCode::SLT, rs, rt, true);
}

TEST_F(CpuComparisonTest, SLT_SetOppositeSign)
{
    uint32_t rs = static_cast<uint32_t>(-97984);
    uint32_t rt = 5437809;
    runCompTest(SecondaryOpCode::SLT, rs, rt, true);
}

TEST_F(CpuComparisonTest, SLT_SetFromUnsigned)
{
    uint32_t rs = 0xFFFFFFFF; // -1
    uint32_t rt = 1;
    runCompTest(SecondaryOpCode::SLT, rs, rt, true);
}

TEST_F(CpuComparisonTest, SLT_SetBounds)
{
    uint32_t rs = static_cast<uint32_t>(INT32_MIN);
    uint32_t rt = INT32_MAX;
    runCompTest(SecondaryOpCode::SLT, rs, rt, true);
}

TEST_F(CpuComparisonTest, SLT_ClearFromUnsigned)
{
    uint32_t rs = 1;
    uint32_t rt = 0xFFFFFFFF; // -1
    runCompTest(SecondaryOpCode::SLT, rs, rt, false);
}

TEST_F(CpuComparisonTest, SLT_ClearSameSign)
{
    uint32_t rs = 87908;
    uint32_t rt = 7988;
    runCompTest(SecondaryOpCode::SLT, rs, rt, false);
}

TEST_F(CpuComparisonTest, SLT_ClearOppositeSign)
{
    uint32_t rs = 87908;
    uint32_t rt = static_cast<uint32_t>(-7898);
    runCompTest(SecondaryOpCode::SLT, rs, rt, false);
}

TEST_F(CpuComparisonTest, SLT_ClearBounds)
{
    uint32_t rs = static_cast<uint32_t>(INT32_MAX);
    uint32_t rt = static_cast<uint32_t>(INT32_MIN);
    runCompTest(SecondaryOpCode::SLT, rs, rt, false);
}

TEST_F(CpuComparisonTest, SLT_ClearEqual)
{
    uint32_t rs = 0x10;
    uint32_t rt = 0x10;
    runCompTest(SecondaryOpCode::SLT, rs, rt, false);
}

TEST_F(CpuComparisonTest, SLTU_Set)
{
    uint32_t rs = 2480;
    uint32_t rt = 28409;
    runCompTest(SecondaryOpCode::SLTU, rs, rt, true);
}

TEST_F(CpuComparisonTest, SLTU_Clear)
{
    uint32_t rs = 0x1490;
    uint32_t rt = 0xABC;
    runCompTest(SecondaryOpCode::SLTU, rs, rt, false);
}

TEST_F(CpuComparisonTest, SLTU_ClearFromInt)
{
    uint32_t rs = static_cast<uint32_t>(-1); // 0xFFFFFFFF
    uint32_t rt = 1;
    runCompTest(SecondaryOpCode::SLTU, rs, rt, false);
}

TEST_F(CpuComparisonTest, SLTU_ClearEqual)
{
    uint32_t rs = 0x103;
    uint32_t rt = 0x103;
    runCompTest(SecondaryOpCode::SLTU, rs, rt, false);
}

TEST_F(CpuComparisonTest, SLTI_Set)
{
    int16_t imm = 4820;
    uint32_t rs = 139;
    runCompImmediateTest(PrimaryOpCode::SLTI, rs, imm, true);
}

TEST_F(CpuComparisonTest, SLTI_SetRsNegative)
{
    int16_t imm = 4820;
    uint32_t rs = static_cast<uint32_t>(-139);
    runCompImmediateTest(PrimaryOpCode::SLTI, rs, imm, true);
}

TEST_F(CpuComparisonTest, SLTI_SetBothNegative)
{
    int16_t imm = -4820;
    uint32_t rs = static_cast<uint32_t>(-879482);
    runCompImmediateTest(PrimaryOpCode::SLTI, rs, imm, true);
}

TEST_F(CpuComparisonTest, SLTI_SetImmMin)
{
    int16_t imm = INT16_MIN;
    uint32_t rs = static_cast<uint32_t>(INT32_MIN);
    runCompImmediateTest(PrimaryOpCode::SLTI, rs, imm, true);
}

TEST_F(CpuComparisonTest, SLTI_ClearImmMin)
{
    int16_t imm = INT16_MIN;
    uint32_t rs = 0;
    runCompImmediateTest(PrimaryOpCode::SLTI, rs, imm, false);
}

TEST_F(CpuComparisonTest, SLTI_SetImmMax)
{
    int16_t imm = INT16_MAX;
    uint32_t rs = 0;
    runCompImmediateTest(PrimaryOpCode::SLTI, rs, imm, true);
}

TEST_F(CpuComparisonTest, SLTI_ClearImmMax)
{
    int16_t imm = INT16_MAX;
    uint32_t rs = INT32_MAX;
    runCompImmediateTest(PrimaryOpCode::SLTI, rs, imm, false);
}

TEST_F(CpuComparisonTest, SLTI_Clear)
{
    int16_t imm = 820;
    uint32_t rs = 1789;
    runCompImmediateTest(PrimaryOpCode::SLTI, rs, imm, false);
}

TEST_F(CpuComparisonTest, SLTI_ClearEqual)
{
    int16_t imm = 0x23;
    uint32_t rs = 0x23;
    runCompImmediateTest(PrimaryOpCode::SLTI, rs, imm, false);
}

TEST_F(CpuComparisonTest, SLTI_SetRsUintMax)
{
    int16_t imm = 10;
    uint32_t rs = UINT32_MAX; // Interpreted as -1
    runCompImmediateTest(PrimaryOpCode::SLTI, rs, imm, true);
}

TEST_F(CpuComparisonTest, SLTI_ClearUnsigned)
{
    int16_t imm = (int16_t)UINT16_MAX;
    uint32_t rs = 10;
    runCompImmediateTest(PrimaryOpCode::SLTI, rs, imm, false);
}

TEST_F(CpuComparisonTest, SLTIU_Set)
{
    uint16_t imm = 42;
    uint32_t rs = 10;
    runCompImmediateTest(PrimaryOpCode::SLTIU, rs, imm, true);
}

TEST_F(CpuComparisonTest, SLTIU_Clear)
{
    uint16_t imm = 190;
    uint32_t rs = 8794;
    runCompImmediateTest(PrimaryOpCode::SLTIU, rs, imm, false);
}

TEST_F(CpuComparisonTest, SLTIU_SetImmSignedMax)
{
    // Here the immediate value will be sign extended to 0x00007FFF
    // We set the value of rs to be 1-less so the comparison is true
    uint16_t imm = INT16_MAX;
    uint32_t rs = imm - 1;
    runCompImmediateTest(PrimaryOpCode::SLTIU, rs, imm, true);
}

TEST_F(CpuComparisonTest, SLTIU_SetImmSignedMin)
{
    // Here the immediate value will be sign extended to 0xFFFF8000
    // We set the value of rs to be the original immediate value + 1 meaning the
    // value of rs is 0x8001. In normal conditions the comparison should be false
    // but we expect the comparison to be true because of sign extension
    uint16_t imm = static_cast<uint16_t>(INT16_MIN);
    uint32_t rs = imm + 1;
    runCompImmediateTest(PrimaryOpCode::SLTIU, rs, imm, true);
}

TEST_F(CpuComparisonTest, SLTIU_SetImmUnsignedMax)
{
    // Here the immediate value will be sign extended to 0xFFFFFFFF
    // We set the value of rs to be the original immediate value + 1 meaning the
    // value of rs is 0x10000. In normal conditions the comparison should be
    // false but we expect the comparison to be true because of sign extension
    uint16_t imm = UINT16_MAX;
    uint32_t rs = imm + 1;
    runCompImmediateTest(PrimaryOpCode::SLTIU, rs, imm, true);
}

TEST_F(CpuComparisonTest, SLTIU_ClearImmZero)
{
    uint16_t imm = 0;
    uint32_t rs = 0x10;
    runCompImmediateTest(PrimaryOpCode::SLTIU, rs, imm, false);
}

TEST_F(CpuComparisonTest, SLTIU_ClearMax)
{
    // With sign extension of the immediate value rs and imm are both 0xFFFFFFFF
    // By being equal the comparison result should be false
    uint16_t imm = UINT16_MAX;
    uint32_t rs = UINT32_MAX;
    runCompImmediateTest(PrimaryOpCode::SLTIU, rs, imm, false);
}
