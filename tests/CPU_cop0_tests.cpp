#include <gtest/gtest.h>
#include "Utils.h"
#include "BIOS.h"
#include "Bus.h"
#include "CPU.h"

TEST(CpuTest, MTC0_1)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = 0xB16B00B5;
    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t destReg = static_cast<uint8_t>(CP0RegIndex::CAUSE);

    cpu.setReg(srcReg, value);
    i.r.rt = srcReg;
    i.r.rd = destReg;
    cpu.mtc0(i);

    EXPECT_EQ(cpu.getCop0Reg(destReg), value);
}

TEST(CpuTest, MTC0_2)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = 0xB16B00B5;
    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t destReg = static_cast<uint8_t>(CP0RegIndex::SR);

    cpu.setReg(srcReg, value);
    i.r.rt = srcReg;
    i.r.rd = destReg;
    cpu.mtc0(i);

    EXPECT_EQ(cpu.getCop0Reg(destReg), value);
}

TEST(CpuTest, MTC0_Unmapped_Reg)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = 0xB16B00B5;
    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t destReg = static_cast<uint8_t>(CP0RegIndex::BPC) + 1;

    cpu.setReg(srcReg, value);
    i.r.rt = srcReg;
    i.r.rd = destReg;
    cpu.mtc0(i);

    EXPECT_EQ(cpu.getCop0Reg(destReg), value);
}

TEST(CpuTest, MFC0_1)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = 0xB16B00B5;
    uint8_t srcReg = static_cast<uint8_t>(CP0RegIndex::CAUSE);
    uint8_t destReg = static_cast<uint8_t>(GprIndex::T1);

    cpu.setCop0Reg(srcReg, value);
    i.r.rt = destReg;
    i.r.rd = srcReg;
    cpu.mfc0(i);

    EXPECT_EQ(cpu.getReg(destReg), value);
}

TEST(CpuTest, MFC0_2)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = 0xB16B00B5;
    uint8_t srcReg = static_cast<uint8_t>(CP0RegIndex::SR);
    uint8_t destReg = static_cast<uint8_t>(GprIndex::T1);

    cpu.setCop0Reg(srcReg, value);
    i.r.rt = destReg;
    i.r.rd = srcReg;
    cpu.mfc0(i);

    EXPECT_EQ(cpu.getReg(destReg), value);
}

TEST(CpuTest, MFC0_Unmapped_Reg)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = 0xB16B00B5;
    uint8_t srcReg = static_cast<uint8_t>(0);
    uint8_t destReg = static_cast<uint8_t>(GprIndex::T1);

    cpu.setCop0Reg(srcReg, value);
    i.r.rt = destReg;
    i.r.rd = srcReg;
    cpu.mfc0(i);

    EXPECT_EQ(cpu.getReg(destReg), value);
}
