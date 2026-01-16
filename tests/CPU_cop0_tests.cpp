#include <gtest/gtest.h>

#include "Core/BIOS.hpp"
#include "Core/Bus.hpp"
#include "Core/CPU.hpp"
#include "Core/RAM.hpp"

class CpuCop0Test : public testing::Test
{
    protected:
        Bus bus;
        CPU cpu;

        const uint32_t defaultRegVal = 0xDEADBEEF;

        CpuCop0Test() :
            cpu(&bus)
        {
            cpu.setReg(CpuReg::PC, 0x10000);
        }

        void runMoveFromCop0Test(uint8_t srcReg, CpuReg destReg)
        {
            Instruction i;
            i.r.opcode = static_cast<uint8_t>(PrimaryOpCode::COP0);
            i.r.rs = static_cast<uint8_t>(CoprocessorOpcode::MFC);
            i.r.rt = static_cast<uint8_t>(destReg);
            i.r.rd = srcReg;

            auto pc = cpu.getReg(CpuReg::PC);
            bus.store<uint32_t>(pc, i.raw);
            cpu.setCop0Reg(srcReg, 0xCAFEBABE);
            cpu.setReg(destReg, defaultRegVal);

            cpu.step();

            EXPECT_EQ(cpu.getReg(destReg), 0xCAFEBABE);
        }

        void runMoveToCop0Test(CpuReg srcReg, uint8_t destReg)
        {
            Instruction i;
            i.r.opcode = static_cast<uint8_t>(PrimaryOpCode::COP0);
            i.r.rs = static_cast<uint8_t>(CoprocessorOpcode::MTC);
            i.r.rt = static_cast<uint8_t>(srcReg);
            i.r.rd = destReg;

            auto pc = cpu.getReg(CpuReg::PC);
            bus.store<uint32_t>(pc, i.raw);
            cpu.setReg(srcReg, 0xCAFEBABE);
            cpu.setCop0Reg(destReg, defaultRegVal);

            cpu.step();

            EXPECT_EQ(cpu.getCop0Reg(destReg), 0xCAFEBABE);
        }
};

TEST_F(CpuCop0Test, MTC0_1)
{
    CpuReg srcReg = CpuReg::T0;
    uint8_t destReg = static_cast<uint8_t>(CP0Reg::CAUSE);

    runMoveToCop0Test(srcReg, destReg);
}

TEST_F(CpuCop0Test, MTC0_2)
{
    CpuReg srcReg = CpuReg::T0;
    uint8_t destReg = static_cast<uint8_t>(CP0Reg::SR);

    runMoveToCop0Test(srcReg, destReg);
}

TEST_F(CpuCop0Test, MTC0_Unmapped_Reg)
{
    CpuReg srcReg = CpuReg::T0;
    uint8_t destReg = static_cast<uint8_t>(CP0Reg::BPC) + 1;

    runMoveToCop0Test(srcReg, destReg);
}

TEST_F(CpuCop0Test, MFC0_1)
{
    uint8_t srcReg = static_cast<uint8_t>(CP0Reg::CAUSE);
    CpuReg destReg = CpuReg::T1;

    runMoveFromCop0Test(srcReg, destReg);
}

TEST_F(CpuCop0Test, MFC0_2)
{
    uint8_t srcReg = static_cast<uint8_t>(CP0Reg::SR);
    CpuReg destReg = CpuReg::T1;

    runMoveFromCop0Test(srcReg, destReg);
}

TEST_F(CpuCop0Test, MFC0_Unmapped_Reg)
{
    uint8_t srcReg = static_cast<uint8_t>(0);
    CpuReg destReg = CpuReg::T1;

    runMoveFromCop0Test(srcReg, destReg);
}
