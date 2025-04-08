#include <gtest/gtest.h>
#include "Utils.h"
#include "BIOS.h"
#include "RAM.h"
#include "Bus.h"
#include "CPU.h"

TEST(CpuTest, BEQ_Equal)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;
    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    uint32_t value = 0xB16B00B5;

    int16_t imm = 0x0004;

    loadImmediate(cpu, srcReg, value);
    loadImmediate(cpu, targetReg, value);
    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = imm;
    cpu.branchOnEqual(i);

    EXPECT_EQ(cpu.m_branchSlotAddr, cpu.pc + 4 + (imm << 2));
    EXPECT_EQ(cpu.m_inBranchDelay, true);
}

TEST(CpuTest, BEQ_Not_Equal)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;
    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    uint32_t value = 0xB16B00B5;

    int16_t imm = 0x0004;

    loadImmediate(cpu, srcReg, value);
    loadImmediate(cpu, targetReg, value + 1);
    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = imm;
    cpu.m_branchSlotAddr = 0;
    cpu.branchOnEqual(i);

    EXPECT_EQ(cpu.pc, RESET_VECTOR);
}

TEST(CpuTest, BNE_BranchWhenNotEqual)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);

    cpu.setReg(srcReg, 11);
    cpu.setReg(targetReg, 12);

    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = 0x0004;
    cpu.m_branchSlotAddr = 0;

    cpu.branchOnNotEqual(i);
    EXPECT_EQ(cpu.m_branchSlotAddr, RESET_VECTOR + 4 + ((int16_t)i.i.immediate << 2));
    EXPECT_EQ(cpu.m_inBranchDelay, true);  // Branch taken
}

TEST(CpuTest, BNE_NoBranchWhenEqual)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);

    cpu.setReg(srcReg, 11);
    cpu.setReg(targetReg, 11);  // Same value

    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = 0x0004;
    cpu.m_branchSlotAddr = 0;

    cpu.branchOnNotEqual(i);
    EXPECT_EQ(cpu.m_branchSlotAddr, 0);  // No branch
    EXPECT_EQ(cpu.m_inBranchDelay, false);  // No branch taken
}

TEST(CpuTest, BNE_NoBranch_ZeroRegistersEqual)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);

    cpu.setReg(srcReg, 0);
    cpu.setReg(targetReg, 0);  // Same value

    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = 0x0004;
    cpu.m_branchSlotAddr = 0;

    cpu.branchOnNotEqual(i);
    EXPECT_EQ(cpu.m_branchSlotAddr, 0);  // No branch
    EXPECT_EQ(cpu.m_inBranchDelay, false);  // No branch taken
}

TEST(CpuTest, BNE_MaxPositiveOffset) {
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);

    cpu.setReg(srcReg, 11);
    cpu.setReg(targetReg, 12);

    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = 0x7FFF;  // Maximum positive offset
    cpu.m_branchSlotAddr = 0;

    cpu.branchOnNotEqual(i);
    EXPECT_EQ(cpu.m_branchSlotAddr, RESET_VECTOR + 4 + ((int16_t)i.i.immediate << 2));
    EXPECT_EQ(cpu.m_inBranchDelay, true);  // Branch taken
}

TEST(CpuBranchTests, BLTZ_BranchTaken)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    const uint32_t pc = 0x00100000;
    const uint16_t offset = 0x0020; // 32 instructions = 128 bytes
    const uint32_t branchTarget = pc + 4 + (offset << 2); // 0x00100084

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 0xFFFFFFFF);  // t0 = -1 (branch should be taken)
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0);           // clear t1

    // Write instructions
    bus.storeWord(pc,     0x05000020); // BLTZ t0, +0x20
    bus.storeWord(pc + 4, 0x34091234); // ORI t1, zero, 0x1234 (delay slot)

    // Step 1: Execute BLTZ
    cpu.step();

    EXPECT_TRUE(cpu.m_inBranchDelay);
    EXPECT_EQ(cpu.m_branchSlotAddr, branchTarget);

    // Step 2: Execute delay slot
    cpu.step();

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), branchTarget);
}

TEST(CpuBranchTests, BLTZ_BranchNotTaken_Zero)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    const uint32_t pc = 0x00100000;
    const uint32_t nextPC = pc + 8; // no branch, normal PC increment

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 0); // t0 = 0 (not less than 0)
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0);

    bus.storeWord(pc,     0x05000020); // BLTZ t0, +0x20
    bus.storeWord(pc + 4, 0x34091234); // ORI t1, zero, 0x1234

    // Step 1: Execute BLTZ
    cpu.step();

    EXPECT_FALSE(cpu.m_inBranchDelay);

    // Step 2: Execute delay slot
    cpu.step();

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), nextPC);
}

TEST(CpuBranchTests, BLTZ_BranchNotTaken)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    const uint32_t pc = 0x00100000;
    const uint32_t nextPC = pc + 8; // no branch, normal PC increment

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 0x24);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0);

    bus.storeWord(pc,     0x05000020); // BLTZ t0, +0x20
    bus.storeWord(pc + 4, 0x34091234); // ORI t1, zero, 0x1234

    // Step 1: Execute BLTZ
    cpu.step();

    EXPECT_FALSE(cpu.m_inBranchDelay);

    // Step 2: Execute delay slot
    cpu.step();

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), nextPC);
}

TEST(CpuBranchTests, BLTZ_ZeroOffset)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    const uint32_t pc = 0x00100000;
    const uint32_t branchTarget = pc + 4; // no offset, same as pc + 4 = 0x00100004

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 0xFFFFFFFF);  // branch taken
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0);

    bus.storeWord(pc,     0x05000000); // BLTZ t0, +0x00
    bus.storeWord(pc + 4, 0x34091234); // ORI t1, zero, 0x1234

    cpu.step();

    EXPECT_TRUE(cpu.m_inBranchDelay);
    EXPECT_EQ(cpu.m_branchSlotAddr, branchTarget);

    cpu.step();

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), branchTarget);
}

TEST(CpuBranchTests, BLTZ_MaxPositiveOffset)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    const uint32_t pc = 0x00100000;
    const int16_t offset = INT16_MAX;
    const uint32_t branchTarget = pc + 4 + (offset << 2);

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 0xFFFFFFFF);  // branch taken
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0);

    bus.storeWord(pc,     0x05007FFF); // BLTZ t0, +0x7FFF
    bus.storeWord(pc + 4, 0x34091234); // ORI t1, zero, 0x1234

    cpu.step();

    EXPECT_TRUE(cpu.m_inBranchDelay);
    EXPECT_EQ(cpu.m_branchSlotAddr, branchTarget);

    cpu.step();

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), branchTarget);
}

TEST(CpuBranchTests, BLTZ_BackwardBranch)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    uint32_t pc = 0x00100080;
    int16_t offset = -0x10; // -16 instructions = -64 bytes
    uint32_t branchTarget = pc + 4 + (offset << 2); // 0x00100044

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 0xFFFFFFFF);  // branch taken
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0);

    bus.storeWord(pc,     0x0500FFF0); // BLTZ t0, -0x10
    bus.storeWord(pc + 4, 0x34091234); // ORI t1, zero, 0x1234

    cpu.step();

    EXPECT_TRUE(cpu.m_inBranchDelay);
    EXPECT_EQ(cpu.m_branchSlotAddr, branchTarget);

    cpu.step();

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), branchTarget);
}

TEST(CpuBranchTests, BLTZ_MaxNegativeOffset)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    uint32_t pc = 0x00110000;
    int16_t offset = INT16_MIN;
    uint32_t branchTarget = pc + 4 + (offset << 2); // pc + 4 - 0x20000 = 0x000F0004

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 0xFFFFFFFF); // branch should be taken
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0);

    bus.storeWord(pc,     0x05008000); // BLTZ T0, -0x8000
    bus.storeWord(pc + 4, 0x34091234); // ORI T1, ZERO, 0x1234

    // Step 1: Execute BLTZ
    cpu.step();

    EXPECT_TRUE(cpu.m_inBranchDelay);
    EXPECT_EQ(cpu.m_branchSlotAddr, branchTarget);

    // Step 2: Execute delay slot
    cpu.step();

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), branchTarget);
}

TEST(CpuBranchTests, BGTZ_BranchTaken)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    const uint32_t pc = 0x00100000;
    const uint16_t offset = 0x0020; // 32 instructions = 128 bytes
    const uint32_t branchTarget = pc + 4 + (offset << 2); // 0x00100084

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 1);     // t0 = 1 (branch should be taken)
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0);             // clear t1

    // Write instructions
    bus.storeWord(pc,     0x1D000020); // BGTZ t0, +0x20
    bus.storeWord(pc + 4, 0x34091234); // ORI t1, zero, 0x1234 (delay slot)

    // Step 1: Execute BLTZ
    cpu.step();

    EXPECT_TRUE(cpu.m_inBranchDelay);
    EXPECT_EQ(cpu.m_branchSlotAddr, branchTarget);

    // Step 2: Execute delay slot
    cpu.step();

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), branchTarget);
}

TEST(CpuBranchTests, BGTZ_BranchNotTaken_Zero)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    const uint32_t pc = 0x00100000;
    const uint32_t nextPC = pc + 8; // no branch, normal PC increment

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 0); // t0 = 0 (not less than 0)
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0);

    bus.storeWord(pc,     0x1D000020); // BGTZ t0, +0x20
    bus.storeWord(pc + 4, 0x34091234); // ORI t1, zero, 0x1234

    // Step 1: Execute BLTZ
    cpu.step();

    EXPECT_FALSE(cpu.m_inBranchDelay);

    // Step 2: Execute delay slot
    cpu.step();

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), nextPC);
}

TEST(CpuBranchTests, BGTZ_BranchNotTaken)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    const uint32_t pc = 0x00100000;
    const uint32_t nextPC = pc + 8; // no branch, normal PC increment

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 0xFFFFFFFF); // t0 = -1 (not less than 0)
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0x24);

    bus.storeWord(pc,     0x1D000020); // BGTZ t0, +0x20
    bus.storeWord(pc + 4, 0x34091234); // ORI t1, zero, 0x1234

    // Step 1: Execute BLTZ
    cpu.step();

    EXPECT_FALSE(cpu.m_inBranchDelay);

    // Step 2: Execute delay slot
    cpu.step();

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), nextPC);
}

TEST(CpuBranchTests, BGTZ_ZeroOffset)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    const uint32_t pc = 0x00100000;
    const uint32_t branchTarget = pc + 4; // no offset, same as pc + 4 = 0x00100004

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 1);  // branch taken
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0);

    bus.storeWord(pc,     0x1D000000); // BGTZ t0, +0x20
    bus.storeWord(pc + 4, 0x34091234); // ORI t1, zero, 0x1234

    cpu.step();

    EXPECT_TRUE(cpu.m_inBranchDelay);
    EXPECT_EQ(cpu.m_branchSlotAddr, branchTarget);

    cpu.step();

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), branchTarget);
}

TEST(CpuBranchTests, BGTZ_MaxPositiveOffset)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    const uint32_t pc = 0x00100000;
    const int16_t offset = INT16_MAX;
    const uint32_t branchTarget = pc + 4 + (offset << 2);

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 1);  // branch taken
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0);

    bus.storeWord(pc,     0x1D007FFF); // BGTZ t0, +0x7FFF
    bus.storeWord(pc + 4, 0x34091234); // ORI t1, zero, 0x1234

    cpu.step();

    EXPECT_TRUE(cpu.m_inBranchDelay);
    EXPECT_EQ(cpu.m_branchSlotAddr, branchTarget);

    cpu.step();

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), branchTarget);
}

TEST(CpuBranchTests, BGTZ_BackwardBranch)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    uint32_t pc = 0x00100080;
    int16_t offset = -0x10; // -16 instructions = -64 bytes
    uint32_t branchTarget = pc + 4 + (offset << 2); // 0x00100044

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 1);  // branch taken
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0);

    bus.storeWord(pc,     0x1D00FFF0); // BGTZ t0, 0x10
    bus.storeWord(pc + 4, 0x34091234); // ORI t1, zero, 0x1234

    cpu.step();

    EXPECT_TRUE(cpu.m_inBranchDelay);
    EXPECT_EQ(cpu.m_branchSlotAddr, branchTarget);

    cpu.step();

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), branchTarget);
}

TEST(CpuBranchTests, BGTZ_MaxNegativeOffset)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    uint32_t pc = 0x00110000;
    int16_t offset = INT16_MIN;
    uint32_t branchTarget = pc + 4 + (offset << 2); // pc + 4 - 0x20000 = 0x000F0004

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 1); // branch should be taken
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0);

    bus.storeWord(pc,     0x1D008000); // BGTZ t0, -0x8000
    bus.storeWord(pc + 4, 0x34091234); // ORI T1, ZERO, 0x1234

    // Step 1: Execute BLTZ
    cpu.step();

    EXPECT_TRUE(cpu.m_inBranchDelay);
    EXPECT_EQ(cpu.m_branchSlotAddr, branchTarget);

    // Step 2: Execute delay slot
    cpu.step();

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), branchTarget);
}

TEST(CpuBranchTests, BGEZ_BranchTaken_Zero)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    const uint32_t pc = 0x00100000;
    const uint16_t offset = 0x0020;
    const uint32_t branchTarget = pc + 4 + (offset << 2); // 0x00100084

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 0); // t0 = 0
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0);

    bus.storeWord(pc,     0x05010020); // BGEZ T0, +0x20
    bus.storeWord(pc + 4, 0x34091234); // ORI T1, ZERO, 0x1234

    cpu.step();
    EXPECT_TRUE(cpu.m_inBranchDelay);
    EXPECT_EQ(cpu.m_branchSlotAddr, branchTarget);

    cpu.step();
    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), branchTarget);
}

TEST(CpuBranchTests, BGEZ_BranchTaken_Positive)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    const uint32_t pc = 0x00100000;
    const uint16_t offset = 0x0020;
    const uint32_t branchTarget = pc + 4 + (offset << 2); // 0x00100084

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 0x00000001); // t0 = 1
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0);

    bus.storeWord(pc,     0x05010020); // BGEZ T0, +0x20
    bus.storeWord(pc + 4, 0x34091234); // ORI T1, ZERO, 0x1234

    cpu.step();
    EXPECT_TRUE(cpu.m_inBranchDelay);
    EXPECT_EQ(cpu.m_branchSlotAddr, branchTarget);

    cpu.step();
    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), branchTarget);
}

TEST(CpuBranchTests, BGEZ_BranchNotTaken)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    const uint32_t pc = 0x00100000;
    const uint32_t nextPc = pc + 8;

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 0xFFFFFFFF); // t0 = -1
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0);

    bus.storeWord(pc,     0x05010020); // BGEZ T0, +0x20
    bus.storeWord(pc + 4, 0x34091234); // ORI T1, ZERO, 0x1234

    cpu.step();
    EXPECT_FALSE(cpu.m_inBranchDelay);

    cpu.step();
    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), nextPc);
}

TEST(CpuBranchTests, BGEZ_ZeroOffset)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    uint32_t pc = 0x00100000;
    uint32_t branchTarget = pc + 4; // offset = 0 -> PC + 4

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 0); // t0 = 0
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0);

    bus.storeWord(pc,     0x05010000); // BGEZ T0, 0
    bus.storeWord(pc + 4, 0x34091234); // ORI T1, ZERO, 0x1234

    cpu.step();
    EXPECT_TRUE(cpu.m_inBranchDelay);
    EXPECT_EQ(cpu.m_branchSlotAddr, branchTarget);

    cpu.step();
    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), branchTarget);
}

TEST(CpuBranchTests, BGEZ_MaxPositiveOffset)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    uint32_t pc = 0x00100000;
    uint16_t offset = INT16_MAX;
    uint32_t branchTarget = pc + 4 + (offset << 2); // pc + 4 + 131068

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 1); // t0 > 0
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0);

    bus.storeWord(pc,     0x05017FFF); // BGEZ T0, +0x7FFF
    bus.storeWord(pc + 4, 0x34091234); // ORI T1, ZERO, 0x1234

    cpu.step();
    EXPECT_TRUE(cpu.m_inBranchDelay);
    EXPECT_EQ(cpu.m_branchSlotAddr, branchTarget);

    cpu.step();
    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), branchTarget);
}

TEST(CpuBranchTests, BGEZ_MaxNegativeOffset)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    uint32_t pc = 0x00120000;
    int16_t offset = INT16_MIN;
    uint32_t branchTarget = pc + 4 + (offset << 2); // pc + 4 - 131072

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 0); // t0 = 0 (branch taken)
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0);

    bus.storeWord(pc,     0x05018000); // BGEZ T0, -0x8000
    bus.storeWord(pc + 4, 0x34091234); // ORI T1, ZERO, 0x1234

    cpu.step();
    EXPECT_TRUE(cpu.m_inBranchDelay);
    EXPECT_EQ(cpu.m_branchSlotAddr, branchTarget);

    cpu.step();
    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), branchTarget);
}

TEST(Cpu_Branch, BLEZ_ZeroOffset)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    uint32_t pc = 0x00100000;
    uint32_t branchTarget = pc + 4;

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 0xFFFFFFFF); // t0 = -1
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0);

    bus.storeWord(pc,     0x19000000); // BLEZ T0, 0
    bus.storeWord(pc + 4, 0x34091234); // ORI T1, ZERO, 0x1234

    cpu.step();
    EXPECT_TRUE(cpu.m_inBranchDelay);
    EXPECT_EQ(cpu.m_branchSlotAddr, branchTarget);

    cpu.step();
    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), branchTarget);
}

TEST(Cpu_Branch, BLEZ_BranchTaken_Negative)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    uint32_t pc = 0x00100000;
    uint16_t offset = 0x0020;
    uint32_t branchTarget = pc + 4 + (offset << 2); // 0x00100084

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 0xFFFFFFFF); // t0 = -1
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0);

    bus.storeWord(pc,     0x19000020); // BLEZ T0, 0x20
    bus.storeWord(pc + 4, 0x34091234); // ORI T1, ZERO, 0x1234

    cpu.step();
    EXPECT_TRUE(cpu.m_inBranchDelay);
    EXPECT_EQ(cpu.m_branchSlotAddr, branchTarget);

    cpu.step();
    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), branchTarget);
}

TEST(Cpu_Branch, BLEZ_BranchTaken_Zero)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    uint32_t pc = 0x00100000;
    uint16_t offset = 0x0020;
    uint32_t branchTarget = pc + 4 + (offset << 2);

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 0); // t0 = 0
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0);

    bus.storeWord(pc,     0x19000020); // BLEZ T0, 0x20
    bus.storeWord(pc + 4, 0x34091234); // ORI T1, ZERO, 0x1234

    cpu.step();
    EXPECT_TRUE(cpu.m_inBranchDelay);
    EXPECT_EQ(cpu.m_branchSlotAddr, branchTarget);

    cpu.step();
    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), branchTarget);
}

TEST(Cpu_Branch, BLEZ_BranchNotTaken_Positive)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    uint32_t pc = 0x00100000;
    uint32_t branchTarget = pc + 8; // Not taken -> PC + 8

    cpu.m_branchSlotAddr = 0xDEADBEEF;
    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 123); // t0 = 123 (not taken)
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0);

    bus.storeWord(pc,     0x19000020); // BLEZ T0, 0x20
    bus.storeWord(pc + 4, 0x34091234); // ORI T1, ZERO, 0x1234

    cpu.step();
    EXPECT_FALSE(cpu.m_inBranchDelay);
    EXPECT_EQ(cpu.m_branchSlotAddr, 0xDEADBEEF);

    cpu.step();
    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), branchTarget);
}

TEST(Cpu_Branch, BLEZ_MaxPositiveOffset)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    uint32_t pc = 0x00100000;
    uint16_t offset = INT16_MAX;
    uint32_t branchTarget = pc + 4 + (offset << 2); // 0x00100000 + 4 + 131068 = 0x00120000

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 0xFFFFFFFF); // t0 = -1
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0);

    bus.storeWord(pc,     0x19007FFF); // BLEZ T0, 0x7FFF
    bus.storeWord(pc + 4, 0x34091234); // ORI T1, ZERO, 0x1234

    cpu.step();
    EXPECT_TRUE(cpu.m_inBranchDelay);
    EXPECT_EQ(cpu.m_branchSlotAddr, branchTarget);

    cpu.step();
    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), branchTarget);
}

TEST(Cpu_Branch, BLEZ_MaxNegativeOffset)
{
    BIOS bios;
    RAM ram;
    Bus bus(&bios, &ram);
    CPU cpu(&bus);

    uint32_t pc = 0x00120000;
    int16_t offset = INT16_MIN;
    uint32_t branchTarget = pc + 4 + (offset << 2); // pc + 4 - 131072 = 0x00100004

    cpu.setSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC), pc);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 0); // t0 = 0 (branch taken)
    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), 0);

    bus.storeWord(pc,     0x19008000); // BLEZ T0, 0x8000
    bus.storeWord(pc + 4, 0x34091234); // ORI T1, ZERO, 0x1234

    cpu.step();
    EXPECT_TRUE(cpu.m_inBranchDelay);
    EXPECT_EQ(cpu.m_branchSlotAddr, branchTarget);

    cpu.step();
    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T1)), 0x1234);
    EXPECT_EQ(cpu.getSpecialReg(static_cast<uint8_t>(SpecialRegIndex::PC)), branchTarget);
}
