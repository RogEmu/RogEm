#include <gtest/gtest.h>
#include "Utils.h"
#include "BIOS.h"
#include "Bus.h"
#include "CPU.h"

TEST(CpuTest, SLT_Set_Same_Sign)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    uint8_t destReg = static_cast<uint8_t>(GprIndex::AT);

    cpu.setReg(srcReg, (int32_t)139);
    cpu.setReg(targetReg, (int32_t)1380);
    i.r.rs = srcReg;
    i.r.rt = targetReg;
    i.r.rd = destReg;
    cpu.setOnLessThan(i);

    EXPECT_EQ(cpu.getReg(destReg), true);
}

TEST(CpuTest, SLT_Set_Different_Sign)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    uint8_t destReg = static_cast<uint8_t>(GprIndex::AT);

    cpu.setReg(srcReg, (int32_t)-97984);
    cpu.setReg(targetReg, (int32_t)5437809);
    i.r.rs = srcReg;
    i.r.rt = targetReg;
    i.r.rd = destReg;
    cpu.setOnLessThan(i);

    EXPECT_EQ(cpu.getReg(destReg), true);
}

TEST(CpuTest, SLT_Set_Int_Bounds)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    uint8_t destReg = static_cast<uint8_t>(GprIndex::AT);

    cpu.setReg(srcReg, INT32_MIN);
    cpu.setReg(targetReg, INT32_MAX);
    i.r.rs = srcReg;
    i.r.rt = targetReg;
    i.r.rd = destReg;
    cpu.setOnLessThan(i);

    EXPECT_EQ(cpu.getReg(destReg), true);
}

TEST(CpuTest, SLT_No_Set_Same_Sign)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    uint8_t destReg = static_cast<uint8_t>(GprIndex::AT);

    cpu.setReg(srcReg, (int32_t)87908);
    cpu.setReg(targetReg, (int32_t)7898);
    i.r.rs = srcReg;
    i.r.rt = targetReg;
    i.r.rd = destReg;
    cpu.setOnLessThan(i);

    EXPECT_EQ(cpu.getReg(destReg), false);
}

TEST(CpuTest, SLT_No_Set_Different_Sign)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    uint8_t destReg = static_cast<uint8_t>(GprIndex::AT);

    cpu.setReg(srcReg, (int32_t)87908);
    cpu.setReg(targetReg, (int32_t)-7898);
    i.r.rs = srcReg;
    i.r.rt = targetReg;
    i.r.rd = destReg;
    cpu.setOnLessThan(i);

    EXPECT_EQ(cpu.getReg(destReg), false);
}

TEST(CpuTest, SLT_No_Set_Int_Bounds)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    uint8_t destReg = static_cast<uint8_t>(GprIndex::AT);

    cpu.setReg(srcReg, INT32_MAX);
    cpu.setReg(targetReg, INT32_MIN);
    i.r.rs = srcReg;
    i.r.rt = targetReg;
    i.r.rd = destReg;
    cpu.setOnLessThan(i);

    EXPECT_EQ(cpu.getReg(destReg), false);
}

TEST(CpuTest, SLT_Set_From_Unsigned)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    uint8_t destReg = static_cast<uint8_t>(GprIndex::AT);

    cpu.setReg(srcReg, UINT32_MAX); // equal to -1
    cpu.setReg(targetReg, 1);
    i.r.rs = srcReg;
    i.r.rt = targetReg;
    i.r.rd = destReg;
    cpu.setOnLessThan(i);

    EXPECT_EQ(cpu.getReg(destReg), true);
}

TEST(CpuTest, SLT_Set_Unsigned_From_Pass)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    uint8_t destReg = static_cast<uint8_t>(GprIndex::AT);

    cpu.setReg(srcReg, 1);
    cpu.setReg(targetReg, UINT32_MAX); // equal to -1
    i.r.rs = srcReg;
    i.r.rt = targetReg;
    i.r.rd = destReg;
    cpu.setOnLessThan(i);

    EXPECT_EQ(cpu.getReg(destReg), false);
}

TEST(CpuTest, SLT_No_Set_Equal)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    uint8_t destReg = static_cast<uint8_t>(GprIndex::AT);

    cpu.setReg(srcReg, 1);
    cpu.setReg(targetReg, 1); // equal to -1
    i.r.rs = srcReg;
    i.r.rt = targetReg;
    i.r.rd = destReg;
    cpu.setOnLessThan(i);

    EXPECT_EQ(cpu.getReg(destReg), false);
}
