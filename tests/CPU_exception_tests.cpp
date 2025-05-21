#include <gtest/gtest.h>
#include "BIOS.h"
#include "Bus.h"
#include "RAM.h"
#include "CPU.h"

class CpuExceptionTest : public testing::Test
{
    protected:
        Bus bus;
        CPU cpu;

        CpuExceptionTest() :
            cpu(&bus)
        {
            cpu.setReg(CpuReg::PC, 0x10000);
        }
};

TEST_F(CpuExceptionTest, Syscall)
{
    auto pc = cpu.getReg(CpuReg::PC);
    const uint32_t syscallInstr = 0x0000000C; // SYSCALL opcode

    // Write syscall instruction to RAM
    bus.storeWord(pc, syscallInstr);

    // Clear EPC, Cause, Status
    cpu.setCop0Reg(static_cast<uint8_t>(CP0Reg::CAUSE), 0);
    cpu.setCop0Reg(static_cast<uint8_t>(CP0Reg::EPC), 0);
    cpu.setCop0Reg(static_cast<uint8_t>(CP0Reg::SR), 0x0000000A);

    // Step one instruction (should trigger exception)
    cpu.step();

    // Exception vector
    EXPECT_EQ(cpu.getReg(CpuReg::PC), 0x80000080);

    // EPC should be address of syscall instruction
    EXPECT_EQ(cpu.getCop0Reg(static_cast<uint8_t>(CP0Reg::EPC)), pc);

    // Cause should have ExcCode = 8 (Syscall)
    EXPECT_EQ((cpu.getCop0Reg(static_cast<uint8_t>(CP0Reg::CAUSE)) >> 2) & 0x1F, (uint32_t)ExceptionType::Syscall);

    // Branch Delay bit should be 0
    EXPECT_FALSE(cpu.getCop0Reg(static_cast<uint8_t>(CP0Reg::SR)) >> 31);

    // Bits 6-7 should remain to 0 after shift
    EXPECT_EQ((cpu.getCop0Reg(static_cast<uint8_t>(CP0Reg::SR)) >> 6) & 0x3, 0);

    // Bits 0-5 should be shifted 2 places left
    EXPECT_EQ(cpu.getCop0Reg(static_cast<uint8_t>(CP0Reg::SR)) & 0x3F, 0x28);
}

TEST_F(CpuExceptionTest, Syscall_BranchDelaySlot)
{
    auto pc = cpu.getReg(CpuReg::PC);

    bus.storeWord(pc, 0x1000FFFE); // BEQ $zero, $zero, -1 (dummy branch)
    bus.storeWord(pc + 4, 0x0000000C); // SYSCALL opcode

    // Clear EPC, Cause, Status
    cpu.setCop0Reg(static_cast<uint8_t>(CP0Reg::CAUSE), 0);
    cpu.setCop0Reg(static_cast<uint8_t>(CP0Reg::EPC), 0);
    cpu.setCop0Reg(static_cast<uint8_t>(CP0Reg::SR), 0x0000000A);

    cpu.step(); // Step over branch instruction
    cpu.step(); // Step over syscall instruction (in branch delay slot)

    // Exception vector
    EXPECT_EQ(cpu.getReg(CpuReg::PC), 0x80000080);

    // EPC should be address of branch instruction
    EXPECT_EQ(cpu.getCop0Reg(static_cast<uint8_t>(CP0Reg::EPC)), pc);

    // Cause should have ExcCode = 8 (Syscall)
    EXPECT_EQ((cpu.getCop0Reg(static_cast<uint8_t>(CP0Reg::CAUSE)) >> 2) & 0x1F, (uint32_t)ExceptionType::Syscall);

    // Branch Delay bit should be 0
    EXPECT_TRUE(cpu.getCop0Reg(static_cast<uint8_t>(CP0Reg::CAUSE)) >> 31);

    // Bits 6-7 should remain to 0 after shift
    EXPECT_EQ((cpu.getCop0Reg(static_cast<uint8_t>(CP0Reg::SR)) >> 6) & 0x3, 0);

    // Bits 0-5 should be shifted 2 places left
    EXPECT_EQ(cpu.getCop0Reg(static_cast<uint8_t>(CP0Reg::SR)) & 0x3F, 0x28);
}

TEST_F(CpuExceptionTest, RFE)
{
    auto pc = cpu.getReg(CpuReg::PC);

    bus.storeWord(pc, 0x42000010); // RFE

    // Clear EPC, Cause, Status
    cpu.setCop0Reg(static_cast<uint8_t>(CP0Reg::CAUSE), 0);
    cpu.setCop0Reg(static_cast<uint8_t>(CP0Reg::EPC), 0);
    cpu.setCop0Reg(static_cast<uint8_t>(CP0Reg::SR), 0x00000028);

    cpu.step();

    // Bits 6-7 should remain to 0 after shift
    EXPECT_EQ((cpu.getCop0Reg(static_cast<uint8_t>(CP0Reg::SR)) >> 6) & 0x3, 0);

    // Bits 0-5 should be copied 2 places right and bits 4-5 should be left untouched
    EXPECT_EQ(cpu.getCop0Reg(static_cast<uint8_t>(CP0Reg::SR)) & 0x3F, 0x2A);
}
