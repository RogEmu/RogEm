#include <gtest/gtest.h>
#include "BIOS.h"
#include "Bus.h"
#include "CPU.h"
#include "RAM.h"

class CpuShiftTest : public testing::Test
{
    protected:
        Bus bus;
        BIOS bios;
        RAM ram;
        CPU cpu;

        const uint32_t defaultRegVal = 0xDEADBEEF;

        CpuShiftTest() :
            bus(&bios, &ram),
            cpu(&bus)
        {
            cpu.reset();
            cpu.setReg(CpuReg::PC, 0x10000);
        }

        void runShift(SecondaryOpCode opcode, CpuReg rd, CpuReg rt, CpuReg rs)
        {
            Instruction i;
            i.r.opcode = static_cast<uint8_t>(PrimaryOpCode::SPECIAL);
            i.r.rd = static_cast<uint8_t>(rd);
            i.r.rt = static_cast<uint8_t>(rt);
            i.r.rs = static_cast<uint8_t>(rs);
            i.r.funct = static_cast<uint8_t>(opcode);

            auto pc = cpu.getReg(CpuReg::PC);
            bus.storeWord(pc, i.raw);
            cpu.setReg(rd, defaultRegVal);
            cpu.step();
        }

        void runShiftImmediate(SecondaryOpCode opcode, CpuReg rd, CpuReg rt, uint8_t shiftAmount)
        {
            Instruction i;
            i.r.opcode = static_cast<uint8_t>(PrimaryOpCode::SPECIAL);
            i.r.rd = static_cast<uint8_t>(rd);
            i.r.rt = static_cast<uint8_t>(rt);
            i.r.shamt = shiftAmount;
            i.r.funct = static_cast<uint8_t>(opcode);

            auto pc = cpu.getReg(CpuReg::PC);
            bus.storeWord(pc, i.raw);
            cpu.setReg(rd, defaultRegVal);
            cpu.step();
        }

        void runLoadUpperImmediate(CpuReg rt, uint16_t imm)
        {
            Instruction i;
            i.i.opcode = static_cast<uint8_t>(PrimaryOpCode::LUI);
            i.i.rt = static_cast<uint8_t>(rt);
            i.i.immediate = imm;

            auto pc = cpu.getReg(CpuReg::PC);
            bus.storeWord(pc, i.raw);
            cpu.setReg(rt, defaultRegVal);
            cpu.step();
        }
};

TEST_F(CpuShiftTest, SLLV_Basic)
{
    uint32_t value = 0x9F0DE23B;
    uint32_t shiftAmount = 7;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;
    CpuReg rd = CpuReg::T2;

    cpu.setReg(rt, value);
    cpu.setReg(rs, shiftAmount);
    runShift(SecondaryOpCode::SLLV, rd, rt, rs);
    EXPECT_EQ(cpu.getReg(rd), value << (shiftAmount & 0x1F));
}

TEST_F(CpuShiftTest, SLLV_MaxShift)
{
    uint32_t value = 0x9F0DE23B;
    uint32_t shiftAmount = 32;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;
    CpuReg rd = CpuReg::T2;

    cpu.setReg(rt, value);
    cpu.setReg(rs, shiftAmount);
    runShift(SecondaryOpCode::SLLV, rd, rt, rs);
    EXPECT_EQ(cpu.getReg(rd), value << (shiftAmount & 0x1F));
}

TEST_F(CpuShiftTest, SLLV_ShiftOverflow)
{
    uint32_t value = 0x9F0DE23B;
    uint32_t shiftAmount = 178; // Bigger shift value should be truncated
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;
    CpuReg rd = CpuReg::T2;

    cpu.setReg(rt, value);
    cpu.setReg(rs, shiftAmount);
    runShift(SecondaryOpCode::SLLV, rd, rt, rs);
    EXPECT_EQ(cpu.getReg(rd), value << (shiftAmount & 0x1F));
}

TEST_F(CpuShiftTest, SLLV_No_Shift)
{
    uint32_t value = 0x9F0DE23B;
    uint32_t shiftAmount = 0;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;
    CpuReg rd = CpuReg::T2;

    cpu.setReg(rt, value);
    cpu.setReg(rs, shiftAmount);
    runShift(SecondaryOpCode::SLLV, rd, rt, rs);
    EXPECT_EQ(cpu.getReg(rd), value);
}

TEST_F(CpuShiftTest, SLL_Basic)
{
    uint32_t value = 0x9F0DE23B;
    uint8_t shiftAmount = 21;
    CpuReg rt = CpuReg::T1;
    CpuReg rd = CpuReg::T2;

    cpu.setReg(rt, value);
    runShiftImmediate(SecondaryOpCode::SLL, rd, rt, shiftAmount);
    EXPECT_EQ(cpu.getReg(rd), value << shiftAmount);
}

TEST_F(CpuShiftTest, SLL_MaxShift)
{
    uint32_t value = 0x9F0DE23B;
    uint8_t shiftAmount = 31;
    CpuReg rt = CpuReg::T1;
    CpuReg rd = CpuReg::T2;

    cpu.setReg(rt, value);
    runShiftImmediate(SecondaryOpCode::SLL, rd, rt, shiftAmount);
    EXPECT_EQ(cpu.getReg(rd), value << shiftAmount);
}

TEST_F(CpuShiftTest, SLL_ZeroShift)
{
    uint32_t value = 0x9F0DE23B;
    uint8_t shiftAmount = 0;
    CpuReg rt = CpuReg::T1;
    CpuReg rd = CpuReg::T2;

    cpu.setReg(rt, value);
    runShiftImmediate(SecondaryOpCode::SLL, rd, rt, shiftAmount);
    EXPECT_EQ(cpu.getReg(rd), value);
}


TEST_F(CpuShiftTest, SRL_Basic)
{
    uint32_t value = 0xFFFFAAAA;
    uint8_t shiftAmount = 12;
    CpuReg rt = CpuReg::T1;
    CpuReg rd = CpuReg::T2;

    cpu.setReg(rt, value);
    runShiftImmediate(SecondaryOpCode::SRL, rd, rt, shiftAmount);
    EXPECT_EQ(cpu.getReg(rd), value >> (shiftAmount & 0x1F));
}

TEST_F(CpuShiftTest, SRL_MaxShift)
{
    uint32_t value = 0xFFFFAAAA;
    uint8_t shiftAmount = 31;
    CpuReg rt = CpuReg::T1;
    CpuReg rd = CpuReg::T2;

    cpu.setReg(rt, value);
    runShiftImmediate(SecondaryOpCode::SRL, rd, rt, shiftAmount);
    EXPECT_EQ(cpu.getReg(rd), value >> shiftAmount);
}

TEST_F(CpuShiftTest, SRL_ZeroShift)
{
    uint32_t value = 0xFFFFAAAA;
    uint8_t shiftAmount = 0;
    CpuReg rt = CpuReg::T1;
    CpuReg rd = CpuReg::T2;

    cpu.setReg(rt, value);
    runShiftImmediate(SecondaryOpCode::SRL, rd, rt, shiftAmount);
    EXPECT_EQ(cpu.getReg(rd), value);
}

TEST_F(CpuShiftTest, SRLV_Basic)
{
    uint32_t value = 0x9F0DE23B;
    uint32_t shiftAmount = 7;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;
    CpuReg rd = CpuReg::T2;

    cpu.setReg(rt, value);
    cpu.setReg(rs, shiftAmount);
    runShift(SecondaryOpCode::SRLV, rd, rt, rs);
    EXPECT_EQ(cpu.getReg(rd), value >> (shiftAmount & 0x1F));
}

TEST_F(CpuShiftTest, SRLV_MaxShift)
{
    uint32_t value = 0x9F0DE23B;
    uint32_t shiftAmount = 32;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;
    CpuReg rd = CpuReg::T2;

    cpu.setReg(rt, value);
    cpu.setReg(rs, shiftAmount);
    runShift(SecondaryOpCode::SRLV, rd, rt, rs);
    EXPECT_EQ(cpu.getReg(rd), value >> (shiftAmount & 0x1F));
}

TEST_F(CpuShiftTest, SRLV_ShiftOverflow)
{
    uint32_t value = 0x9F0DE23B;
    uint32_t shiftAmount = 178; // Bigger shift value should be truncated
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;
    CpuReg rd = CpuReg::T2;

    cpu.setReg(rt, value);
    cpu.setReg(rs, shiftAmount);
    runShift(SecondaryOpCode::SRLV, rd, rt, rs);
    EXPECT_EQ(cpu.getReg(rd), value >> (shiftAmount & 0x1F));
}

TEST_F(CpuShiftTest, SRLV_No_Shift)
{
    uint32_t value = 0x9F0DE23B;
    uint32_t shiftAmount = 0;
    CpuReg rs = CpuReg::T0;
    CpuReg rt = CpuReg::T1;
    CpuReg rd = CpuReg::T2;

    cpu.setReg(rt, value);
    cpu.setReg(rs, shiftAmount);
    runShift(SecondaryOpCode::SRLV, rd, rt, rs);
    EXPECT_EQ(cpu.getReg(rd), value);
}

TEST_F(CpuShiftTest, SRA_Basic)
{
    uint32_t value = 0x7898;
    uint8_t shiftAmount = 5;
    CpuReg rt = CpuReg::T1;
    CpuReg rd = CpuReg::T2;

    cpu.setReg(rt, value);
    runShiftImmediate(SecondaryOpCode::SRA, rd, rt, shiftAmount);
    EXPECT_EQ(cpu.getReg(rd),  0x3C4);
}

TEST_F(CpuShiftTest, SRA_MSB_Negative)
{
    uint32_t value = 0x80000000;
    uint8_t shiftAmount = 31; // Should shift and get all ones
    CpuReg rt = CpuReg::T1;
    CpuReg rd = CpuReg::T2;

    cpu.setReg(rt, value);
    runShiftImmediate(SecondaryOpCode::SRA, rd, rt, shiftAmount);
    EXPECT_EQ(cpu.getReg(rd), 0xFFFFFFFF);
}

TEST_F(CpuShiftTest, SRA_MSB_Positive)
{
    uint32_t value = 0x40000000;
    uint8_t shiftAmount = 31; // Should shift all zeroes
    CpuReg rt = CpuReg::T1;
    CpuReg rd = CpuReg::T2;

    cpu.setReg(rt, value);
    runShiftImmediate(SecondaryOpCode::SRA, rd, rt, shiftAmount);
    EXPECT_EQ(cpu.getReg(rd), 0);
}

TEST_F(CpuShiftTest, SRA_MSB_Positive_2)
{
    uint32_t value = 0x40000000;
    uint8_t shiftAmount = 30; // Should shift and result in 1
    CpuReg rt = CpuReg::T1;
    CpuReg rd = CpuReg::T2;

    cpu.setReg(rt, value);
    runShiftImmediate(SecondaryOpCode::SRA, rd, rt, shiftAmount);
    EXPECT_EQ(cpu.getReg(rd), 1);
}

TEST_F(CpuShiftTest, SRAV_Basic)
{
    uint32_t value = 0x7986;
    uint32_t shiftAmount = 10;
    CpuReg rt = CpuReg::T1;
    CpuReg rd = CpuReg::T2;
    CpuReg rs = CpuReg::T3;

    cpu.setReg(rt, value);
    cpu.setReg(rs, shiftAmount);
    runShift(SecondaryOpCode::SRAV, rd, rt, rs);
    EXPECT_EQ(cpu.getReg(rd), 0x1E);
}

TEST_F(CpuShiftTest, SRAV_MSB_Negative)
{
    uint32_t value = 0x80000000;
    uint32_t shiftAmount = 31; // Should shift all 1s
    CpuReg rt = CpuReg::T1;
    CpuReg rd = CpuReg::T2;
    CpuReg rs = CpuReg::T3;

    cpu.setReg(rt, value);
    cpu.setReg(rs, shiftAmount);
    runShift(SecondaryOpCode::SRAV, rd, rt, rs);
    EXPECT_EQ(cpu.getReg(rd), 0xFFFFFFFF);
}

TEST_F(CpuShiftTest, SRAV_MSB_Positive)
{
    uint32_t value = 0x40000000;
    uint32_t shiftAmount = 30; // Should shift all 0s
    CpuReg rt = CpuReg::T1;
    CpuReg rd = CpuReg::T2;
    CpuReg rs = CpuReg::T3;

    cpu.setReg(rt, value);
    cpu.setReg(rs, shiftAmount);
    runShift(SecondaryOpCode::SRAV, rd, rt, rs);
    EXPECT_EQ(cpu.getReg(rd), 1);
}

TEST_F(CpuShiftTest, SRAV_MSB_Positive_ShiftOverflow)
{
    uint32_t value = 0x40000000;
    uint32_t shiftAmount = 769; // Equivalent to shift 1 right
    CpuReg rt = CpuReg::T1;
    CpuReg rd = CpuReg::T2;
    CpuReg rs = CpuReg::T3;

    cpu.setReg(rt, value);
    cpu.setReg(rs, shiftAmount);
    runShift(SecondaryOpCode::SRAV, rd, rt, rs);
    EXPECT_EQ(cpu.getReg(rd), 0x20000000);
}

TEST_F(CpuShiftTest, SRAV_MSB_Negative_ShiftOverflow)
{
    uint32_t value = 0xF0000000;
    uint32_t shiftAmount = 769; // Equivalent to shift 1 right
    CpuReg rt = CpuReg::T1;
    CpuReg rd = CpuReg::T2;
    CpuReg rs = CpuReg::T3;

    cpu.setReg(rt, value);
    cpu.setReg(rs, shiftAmount);
    runShift(SecondaryOpCode::SRAV, rd, rt, rs);
    EXPECT_EQ(cpu.getReg(rd), 0xF8000000);
}

TEST_F(CpuShiftTest, LUI_1)
{
    CpuReg rt = CpuReg::T0;
    uint16_t immediate = 0xAAAA;

    runLoadUpperImmediate(rt, immediate);
    EXPECT_EQ(cpu.getReg(rt), static_cast<uint32_t>(immediate) << 16);
}

TEST_F(CpuShiftTest, LUI_2)
{
    CpuReg rt = CpuReg::T0;
    uint16_t immediate = 0xFF40;

    runLoadUpperImmediate(rt, immediate);
    EXPECT_EQ(cpu.getReg(rt), static_cast<uint32_t>(immediate) << 16);
}

TEST_F(CpuShiftTest, LUI_3)
{
    CpuReg rt = CpuReg::T0;
    uint16_t immediate = 0xDEAD;

    runLoadUpperImmediate(rt, immediate);
    EXPECT_EQ(cpu.getReg(rt), static_cast<uint32_t>(immediate) << 16);
}
