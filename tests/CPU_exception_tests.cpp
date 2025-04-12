#include <gtest/gtest.h>
#include "Utils.h"
#include "BIOS.h"
#include "Bus.h"
#include "RAM.h"
#include "CPU.h"

TEST(CPU_Exceptions, Syscall)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    // Address for syscall instruction in RAM
    const uint32_t pc = 0x00010000;
    const uint32_t syscallInstr = 0x0000000C; // SYSCALL opcode

    // Write syscall instruction to RAM
    bus.storeWord(pc, syscallInstr);

    // Set PC
    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);

    // Clear EPC, Cause, Status
    cpu.setCop0Reg(static_cast<uint8_t>(CP0RegIndex::CAUSE), 0);
    cpu.setCop0Reg(static_cast<uint8_t>(CP0RegIndex::EPC), 0);
    cpu.setCop0Reg(static_cast<uint8_t>(CP0RegIndex::SR), 0x0000000A);

    // Step one instruction (should trigger exception)
    cpu.step();

    // Exception vector
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), 0x80000080);

    // EPC should be address of syscall instruction
    EXPECT_EQ(cpu.getCop0Reg(static_cast<uint8_t>(CP0RegIndex::EPC)), pc);

    // Cause should have ExcCode = 8 (Syscall)
    EXPECT_EQ((cpu.getCop0Reg(static_cast<uint8_t>(CP0RegIndex::CAUSE)) >> 2) & 0x1F, (uint32_t)ExceptionType::Syscall);

    // Branch Delay bit should be 0
    EXPECT_FALSE(cpu.getCop0Reg(static_cast<uint8_t>(CP0RegIndex::SR)) >> 31);

    // Bits 6-7 should remain to 0 after shift
    EXPECT_EQ((cpu.getCop0Reg(static_cast<uint8_t>(CP0RegIndex::SR)) >> 6) & 0x3, 0);

    // Bits 0-5 should be shifted 2 places left
    EXPECT_EQ(cpu.getCop0Reg(static_cast<uint8_t>(CP0RegIndex::SR)) & 0x3F, 0x28);
}

TEST(CPU_Exceptions, Syscall_BranchDelaySlot)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    // Address for syscall instruction in RAM
    const uint32_t pc = 0x00010000;
    const uint32_t syscallInstr = 0x0000000C; // SYSCALL opcode
    const uint32_t branchInstr = 0x1000FFFE; // BEQ $zero, $zero, -1 (dummy branch)

    // Write syscall instruction to RAM
    bus.storeWord(pc, branchInstr);
    bus.storeWord(pc + 4, syscallInstr);

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);

    // Clear EPC, Cause, Status
    cpu.setCop0Reg(static_cast<uint8_t>(CP0RegIndex::CAUSE), 0);
    cpu.setCop0Reg(static_cast<uint8_t>(CP0RegIndex::EPC), 0);
    cpu.setCop0Reg(static_cast<uint8_t>(CP0RegIndex::SR), 0x0000000A);

    cpu.step(); // Step over branch instruction
    cpu.step(); // Step over syscall instruction (in branch delay slot)

    // Exception vector
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), 0x80000080);

    // EPC should be address of branch instruction
    EXPECT_EQ(cpu.getCop0Reg(static_cast<uint8_t>(CP0RegIndex::EPC)), pc);

    // Cause should have ExcCode = 8 (Syscall)
    EXPECT_EQ((cpu.getCop0Reg(static_cast<uint8_t>(CP0RegIndex::CAUSE)) >> 2) & 0x1F, (uint32_t)ExceptionType::Syscall);

    // Branch Delay bit should be 0
    EXPECT_TRUE(cpu.getCop0Reg(static_cast<uint8_t>(CP0RegIndex::CAUSE)) >> 31);

    // Bits 6-7 should remain to 0 after shift
    EXPECT_EQ((cpu.getCop0Reg(static_cast<uint8_t>(CP0RegIndex::SR)) >> 6) & 0x3, 0);

    // Bits 0-5 should be shifted 2 places left
    EXPECT_EQ(cpu.getCop0Reg(static_cast<uint8_t>(CP0RegIndex::SR)) & 0x3F, 0x28);
}

TEST(CPU_Exceptions, RFE)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    // Address for syscall instruction in RAM
    const uint32_t pc = 0x00010000;
    const uint32_t refInstruction = 0x42000010; // RFE opcode

    // Write syscall instruction to RAM
    bus.storeWord(pc, refInstruction);

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);

    // Clear EPC, Cause, Status
    cpu.setCop0Reg(static_cast<uint8_t>(CP0RegIndex::CAUSE), 0);
    cpu.setCop0Reg(static_cast<uint8_t>(CP0RegIndex::EPC), 0);
    cpu.setCop0Reg(static_cast<uint8_t>(CP0RegIndex::SR), 0x0000002A);

    cpu.step();

    // Bits 6-7 should remain to 0 after shift
    EXPECT_EQ((cpu.getCop0Reg(static_cast<uint8_t>(CP0RegIndex::SR)) >> 6) & 0x3, 0);

    // Bits 0-5 should be shifted 2 places right
    EXPECT_EQ(cpu.getCop0Reg(static_cast<uint8_t>(CP0RegIndex::SR)) & 0x3F, 0xA);
}
