#include <gtest/gtest.h>

#include "Core/BIOS.hpp"
#include "Core/RAM.hpp"
#include "Core/Bus.hpp"
#include "Core/CPU.hpp"

class CpuBranchTest : public testing::Test
{
    protected:
        Bus bus;
        CPU cpu;

        const uint32_t defaultRegVal = 0xDEADBEEF;

        CpuBranchTest() :
            cpu(&bus)
        {
            cpu.setReg(CpuReg::PC, 0x10000);
        }

        void runBranchTest(PrimaryOpCode opcode, uint32_t rs, uint32_t rt, int16_t offset, bool branchTaken)
        {
            Instruction i;
            i.i.opcode = static_cast<uint8_t>(opcode);
            i.i.rs = static_cast<uint8_t>(CpuReg::T0);
            i.i.rt = static_cast<uint8_t>(CpuReg::T1);
            i.i.immediate = offset;

            auto pc = cpu.getReg(CpuReg::PC);
            const uint32_t branchTarget = branchTaken ? pc + 4 + (offset * 4) : pc + 8;

            cpu.setReg(CpuReg::T0, rs);
            cpu.setReg(CpuReg::T1, rt);
            cpu.setReg(CpuReg::T2, 0x10);

            bus.store<uint32_t>(pc, i.raw); // Branch instruction
            bus.store<uint32_t>(pc + 4, 0x254B0020); // ADDIU $t3, $t2, 0x20 (delay slot)

            cpu.step();
            EXPECT_EQ(cpu.getReg(CpuReg::PC), pc + 4);

            cpu.step();
            EXPECT_EQ(cpu.getReg(CpuReg::T3), 0x30);
            EXPECT_EQ(cpu.getReg(CpuReg::PC), branchTarget);
        }

        void runBcondzTest(BranchOnConditionZero opcode, uint32_t rs, int16_t offset, bool branchTaken)
        {
            Instruction i;
            i.i.opcode = static_cast<uint8_t>(PrimaryOpCode::BCONDZ);
            i.i.rs = static_cast<uint8_t>(CpuReg::T0);
            i.i.rt = static_cast<uint8_t>(opcode);
            i.i.immediate = offset;

            auto pc = cpu.getReg(CpuReg::PC);
            const uint32_t branchTarget = branchTaken ? pc + 4 + (offset * 4) : pc + 8;

            cpu.setReg(CpuReg::T0, rs);
            cpu.setReg(CpuReg::T1, 0);
            cpu.setReg(CpuReg::RA, defaultRegVal);

            bus.store<uint32_t>(pc, i.raw);
            bus.store<uint32_t>(pc + 4, 0x34091234); // ORI $t1, $zero, 0x1234 (delay slot)

            cpu.step();
            EXPECT_EQ(cpu.getReg(CpuReg::PC), pc + 4);
            if (opcode == BranchOnConditionZero::BGEZAL || opcode == BranchOnConditionZero::BLTZAL) {
                EXPECT_EQ(cpu.getReg(CpuReg::RA), pc + 8);
            }

            cpu.step();
            EXPECT_EQ(cpu.getReg(CpuReg::T1), 0x1234);
            EXPECT_EQ(cpu.getReg(CpuReg::PC), branchTarget);

            if (opcode == BranchOnConditionZero::BLTZAL || opcode == BranchOnConditionZero::BGEZAL) {
                EXPECT_EQ(cpu.getReg(CpuReg::RA), pc + 8); // RA should always be set to return address
            }
        }
};

TEST_F(CpuBranchTest, BEQ_BranchTaken)
{
    int16_t offset = 0x20;
    uint32_t rs = 10;
    uint32_t rt = 10;
    runBranchTest(PrimaryOpCode::BEQ, rs, rt, offset, true);
}

TEST_F(CpuBranchTest, BEQ_BranchNotTaken)
{
    int16_t offset = 0x20;
    uint32_t rs = 1;
    uint32_t rt = 0;
    runBranchTest(PrimaryOpCode::BEQ, rs, rt, offset, false);
}

TEST_F(CpuBranchTest, BEQ_ZeroOffset)
{
    int16_t offset = 0;
    uint32_t rs = 1;
    uint32_t rt = 1;
    runBranchTest(PrimaryOpCode::BEQ, rs, rt, offset, true);
}

TEST_F(CpuBranchTest, BEQ_MaxOffset)
{
    int16_t offset = INT16_MAX;
    uint32_t rs = 1;
    uint32_t rt = 1;
    runBranchTest(PrimaryOpCode::BEQ, rs, rt, offset, true);
}

TEST_F(CpuBranchTest, BEQ_MinOffset)
{
    int16_t offset = INT16_MIN;
    uint32_t rs = 1;
    uint32_t rt = 1;
    runBranchTest(PrimaryOpCode::BEQ, rs, rt, offset, true);
}

TEST_F(CpuBranchTest, BNE_BranchTaken)
{
    int16_t offset = 0x20;
    uint32_t rs = 1;
    uint32_t rt = 0;
    runBranchTest(PrimaryOpCode::BNE, rs, rt, offset, true);
}

TEST_F(CpuBranchTest, BNE_BranchNotTaken)
{
    int16_t offset = 0;
    uint32_t rs = 1;
    uint32_t rt = rs;
    runBranchTest(PrimaryOpCode::BNE, rs, rt, offset, false);
}

TEST_F(CpuBranchTest, BNE_ZeroOffset)
{
    int16_t offset = 0;
    uint32_t rs = 1;
    uint32_t rt = 0;
    runBranchTest(PrimaryOpCode::BNE, rs, rt, offset, true);
}

TEST_F(CpuBranchTest, BNE_MaxOffset)
{
    int16_t offset = INT16_MAX;
    uint32_t rs = 1;
    uint32_t rt = 0;
    runBranchTest(PrimaryOpCode::BNE, rs, rt, offset, true);
}

TEST_F(CpuBranchTest, BNE_MinOffset)
{
    int16_t offset = INT16_MIN;
    uint32_t rs = 1;
    uint32_t rt = 0;
    runBranchTest(PrimaryOpCode::BNE, rs, rt, offset, true);
}

TEST_F(CpuBranchTest, BLTZ_BranchTaken)
{
    int16_t offset = 0x20;
    uint32_t rs = static_cast<uint32_t>(-1);
    runBcondzTest(BranchOnConditionZero::BLTZ, rs, offset, true);
}

TEST_F(CpuBranchTest, BLTZ_BranchNotTaken_Zero)
{
    int16_t offset = 0x20;
    uint32_t rs = 0;
    runBcondzTest(BranchOnConditionZero::BLTZ, rs, offset, false);
}

TEST_F(CpuBranchTest, BLTZ_BranchNotTaken)
{
    int16_t offset = 0x20;
    uint32_t rs = 10;
    runBcondzTest(BranchOnConditionZero::BLTZ, rs, offset, false);
}

TEST_F(CpuBranchTest, BLTZ_ZeroOffset)
{
    int16_t offset = 0;
    uint32_t rs = static_cast<uint32_t>(-1);
    runBcondzTest(BranchOnConditionZero::BLTZ, rs, offset, true);
}

TEST_F(CpuBranchTest, BLTZ_MaxOffset)
{
    int16_t offset = INT16_MAX;
    uint32_t rs = static_cast<uint32_t>(-1);
    runBcondzTest(BranchOnConditionZero::BLTZ, rs, offset, true);
}

TEST_F(CpuBranchTest, BLTZ_MinOffset)
{
    int16_t offset = INT16_MIN;
    uint32_t rs = static_cast<uint32_t>(-1);
    runBcondzTest(BranchOnConditionZero::BLTZ, rs, offset, true);
}

TEST_F(CpuBranchTest, BGTZ_BranchTaken)
{
    int16_t offset = 0x20;
    uint32_t rs = 0x10;
    runBranchTest(PrimaryOpCode::BGTZ, rs, 0, offset, true);
}

TEST_F(CpuBranchTest, BGTZ_BranchNotTaken)
{
    int16_t offset = 0x20;
    uint32_t rs = static_cast<uint32_t>(-1);
    runBranchTest(PrimaryOpCode::BGTZ, rs, 0, offset, false);
}

TEST_F(CpuBranchTest, BGTZ_BranchNotTaken_Zero)
{
    int16_t offset = 0x20;
    uint32_t rs = 0;
    runBranchTest(PrimaryOpCode::BGTZ, rs, 0, offset, false);
}

TEST_F(CpuBranchTest, BGTZ_ZeroOffset)
{
    int16_t offset = 0;
    uint32_t rs = 0x10;
    runBranchTest(PrimaryOpCode::BGTZ, rs, 0, offset, true);
}

TEST_F(CpuBranchTest, BGTZ_MaxOffset)
{
    int16_t offset = INT16_MAX;
    uint32_t rs = 0x10;
    runBranchTest(PrimaryOpCode::BGTZ, rs, 0, offset, true);
}

TEST_F(CpuBranchTest, BGTZ_MinOffset)
{
    int16_t offset = INT16_MIN;
    uint32_t rs = 0x10;
    runBranchTest(PrimaryOpCode::BGTZ, rs, 0, offset, true);
}

TEST_F(CpuBranchTest, BGEZ_BranchTaken)
{
    int16_t offset = 0x20;
    uint32_t rs = 0x10;
    runBcondzTest(BranchOnConditionZero::BGEZ, rs, offset, true);
}

TEST_F(CpuBranchTest, BGEZ_BranchTaken_Zero)
{
    int16_t offset = 0x20;
    uint32_t rs = 0;
    runBcondzTest(BranchOnConditionZero::BGEZ, rs, offset, true);
}

TEST_F(CpuBranchTest, BGEZ_BranchNotTaken)
{
    int16_t offset = 0x20;
    uint32_t rs = static_cast<uint32_t>(-1);
    runBcondzTest(BranchOnConditionZero::BGEZ, rs, offset, false);
}

TEST_F(CpuBranchTest, BGEZ_ZeroOffset)
{
    int16_t offset = 0;
    uint32_t rs = 0x10;
    runBcondzTest(BranchOnConditionZero::BGEZ, rs, offset, true);
}

TEST_F(CpuBranchTest, BGEZ_MaxOffset)
{
    int16_t offset = INT16_MAX;
    uint32_t rs = 0x10;
    runBcondzTest(BranchOnConditionZero::BGEZ, rs, offset, true);
}

TEST_F(CpuBranchTest, BGEZ_MinOffset)
{
    int16_t offset = INT16_MIN;
    uint32_t rs = 0x10;
    runBcondzTest(BranchOnConditionZero::BGEZ, rs, offset, true);
}

TEST_F(CpuBranchTest, BLEZ_BranchTaken)
{
    int16_t offset = 0x20;
    uint32_t rs = static_cast<uint32_t>(-1);
    runBranchTest(PrimaryOpCode::BLEZ, rs, 0, offset, true);
}

TEST_F(CpuBranchTest, BLEZ_BranchTaken_Zero)
{
    int16_t offset = 0x20;
    uint32_t rs = 0;
    runBranchTest(PrimaryOpCode::BLEZ, rs, 0, offset, true);
}

TEST_F(CpuBranchTest, BLEZ_BranchNotTaken)
{
    int16_t offset = 0x20;
    uint32_t rs = 0x10;
    runBranchTest(PrimaryOpCode::BLEZ, rs, 0, offset, false);
}

TEST_F(CpuBranchTest, BLEZ_ZeroOffset)
{
    int16_t offset = 0;
    uint32_t rs = 0;
    runBranchTest(PrimaryOpCode::BLEZ, rs, 0, offset, true);
}

TEST_F(CpuBranchTest, BLEZ_MaxOffset)
{
    int16_t offset = INT16_MAX;
    uint32_t rs = 0;
    runBranchTest(PrimaryOpCode::BLEZ, rs, 0, offset, true);
}

TEST_F(CpuBranchTest, BLEZ_MinOffset)
{
    int16_t offset = INT16_MIN;
    uint32_t rs = 0;
    runBranchTest(PrimaryOpCode::BLEZ, rs, 0, offset, true);
}

TEST_F(CpuBranchTest, BLTZAL_BranchTaken)
{
    int16_t offset = 0x20;
    uint32_t rs = static_cast<uint32_t>(-1);
    runBcondzTest(BranchOnConditionZero::BLTZAL, rs, offset, true);
}

TEST_F(CpuBranchTest, BLTZAL_BranchNotTaken)
{
    int16_t offset = 0x20;
    uint32_t rs = 1;
    runBcondzTest(BranchOnConditionZero::BLTZAL, rs, offset, false);
}

TEST_F(CpuBranchTest, BLTZAL_BranchNotTaken_Zero)
{
    int16_t offset = 0x20;
    uint32_t rs = 0;
    runBcondzTest(BranchOnConditionZero::BLTZAL, rs, offset, false);
}

TEST_F(CpuBranchTest, BLTZAL_MaxOffset)
{
    int16_t offset = INT16_MAX;
    uint32_t rs = static_cast<uint32_t>(-1);
    runBcondzTest(BranchOnConditionZero::BLTZAL, rs, offset, true);
}

TEST_F(CpuBranchTest, BLTZAL_MinOffset)
{
    int16_t offset = INT16_MIN;
    uint32_t rs = static_cast<uint32_t>(-1);
    runBcondzTest(BranchOnConditionZero::BLTZAL, rs, offset, true);
}

TEST_F(CpuBranchTest, BGEZAL_BranchTaken)
{
    int16_t offset = 0x20;
    uint32_t rs = 1;
    runBcondzTest(BranchOnConditionZero::BGEZAL, rs, offset, true);
}

TEST_F(CpuBranchTest, BGEZAL_BranchTaken_Zero)
{
    int16_t offset = 0x20;
    uint32_t rs = 0;
    runBcondzTest(BranchOnConditionZero::BGEZAL, rs, offset, true);
}

TEST_F(CpuBranchTest, BGEZAL_BranchNotTaken)
{
    int16_t offset = 0x20;
    uint32_t rs = static_cast<uint32_t>(-1);
    runBcondzTest(BranchOnConditionZero::BGEZAL, rs, offset, false);
}

TEST_F(CpuBranchTest, BGEZAL_MaxOffset)
{
    int16_t offset = INT16_MAX;
    uint32_t rs = 0x10;
    runBcondzTest(BranchOnConditionZero::BGEZAL, rs, offset, true);
}

TEST_F(CpuBranchTest, BGEZAL_MaxNegativeOffset)
{
    int16_t offset = INT16_MIN;
    uint32_t rs = 0x10;
    runBcondzTest(BranchOnConditionZero::BGEZAL, rs, offset, true);
}
