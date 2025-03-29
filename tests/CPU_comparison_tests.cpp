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

TEST(CpuTest, SLTU_Set_1)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    uint8_t destReg = static_cast<uint8_t>(GprIndex::AT);

    cpu.setReg(srcReg, 2480);
    cpu.setReg(targetReg, 28409);
    i.r.rs = srcReg;
    i.r.rt = targetReg;
    i.r.rd = destReg;
    cpu.setOnLessThanUnsigned(i);

    EXPECT_EQ(cpu.getReg(destReg), true);
}

TEST(CpuTest, SLTU_Set_From_Int_2)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    uint8_t destReg = static_cast<uint8_t>(GprIndex::AT);

    cpu.setReg(srcReg, INT32_MIN); // equal to 80000000
    cpu.setReg(targetReg, UINT32_MAX); // equal to FFFFFFFF
    i.r.rs = srcReg;
    i.r.rt = targetReg;
    i.r.rd = destReg;
    cpu.setOnLessThanUnsigned(i);

    EXPECT_EQ(cpu.getReg(destReg), true);
}

TEST(CpuTest, SLTU_Set_From_Int_3)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    uint8_t destReg = static_cast<uint8_t>(GprIndex::AT);

    cpu.setReg(srcReg, 1);
    cpu.setReg(targetReg, -2); // equal to FFFFFFFE
    i.r.rs = srcReg;
    i.r.rt = targetReg;
    i.r.rd = destReg;
    cpu.setOnLessThanUnsigned(i);

    EXPECT_EQ(cpu.getReg(destReg), true);
}

TEST(CpuTest, SLTU_No_Set_Equal)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    uint8_t destReg = static_cast<uint8_t>(GprIndex::AT);

    cpu.setReg(srcReg, UINT32_MAX);
    cpu.setReg(targetReg, UINT32_MAX);
    i.r.rs = srcReg;
    i.r.rt = targetReg;
    i.r.rd = destReg;
    cpu.setOnLessThanUnsigned(i);

    EXPECT_EQ(cpu.getReg(destReg), false);
}

TEST(CpuTest, SLTU_No_Set_From_Int)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    uint8_t destReg = static_cast<uint8_t>(GprIndex::AT);

    cpu.setReg(srcReg, -1); // equal to FFFFFFFF
    cpu.setReg(targetReg, -2); // equal to FFFFFFFE
    i.r.rs = srcReg;
    i.r.rt = targetReg;
    i.r.rd = destReg;
    cpu.setOnLessThanUnsigned(i);

    EXPECT_EQ(cpu.getReg(destReg), false);
}

TEST(CpuTest, SLTI_Set_1)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    int16_t imm = 4820;

    cpu.setReg(srcReg, (int32_t)139);
    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = imm;
    cpu.setOnLessThanImmediate(i);

    EXPECT_EQ(cpu.getReg(targetReg), true);
}

TEST(CpuTest, SLTI_Set_2)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    int16_t imm = 4820;

    cpu.setReg(srcReg, (int32_t)-139);
    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = imm;
    cpu.setOnLessThanImmediate(i);

    EXPECT_EQ(cpu.getReg(targetReg), true);
}

TEST(CpuTest, SLTI_Set_3)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    int16_t imm = -4820;

    cpu.setReg(srcReg, -879482);
    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = imm;
    cpu.setOnLessThanImmediate(i);

    EXPECT_EQ(cpu.getReg(targetReg), true);
}

TEST(CpuTest, SLTI_Set_Bounds_1)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    int16_t imm = INT16_MIN;

    cpu.setReg(srcReg, INT32_MIN);
    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = imm;
    cpu.setOnLessThanImmediate(i);

    EXPECT_EQ(cpu.getReg(targetReg), true);
}

TEST(CpuTest, SLTI_Set_Bounds_2)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    int16_t imm = INT16_MAX;

    cpu.setReg(srcReg, INT32_MIN);
    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = imm;
    cpu.setOnLessThanImmediate(i);

    EXPECT_EQ(cpu.getReg(targetReg), true);
}

TEST(CpuTest, SLTI_No_Set_Bounds_1)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    int16_t imm = INT16_MAX;

    cpu.setReg(srcReg, INT32_MAX);
    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = imm;
    cpu.setOnLessThanImmediate(i);

    EXPECT_EQ(cpu.getReg(targetReg), false);
}

TEST(CpuTest, SLTI_No_Set_Bounds_2)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    int16_t imm = INT16_MIN;

    cpu.setReg(srcReg, INT32_MAX);
    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = imm;
    cpu.setOnLessThanImmediate(i);

    EXPECT_EQ(cpu.getReg(targetReg), false);
}

TEST(CpuTest, SLTI_No_Set_1)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    int16_t imm = -4820;

    cpu.setReg(srcReg, 1789);
    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = imm;
    cpu.setOnLessThanImmediate(i);

    EXPECT_EQ(cpu.getReg(targetReg), false);
}

TEST(CpuTest, SLTI_No_Set_Equal)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    int16_t imm = 0;

    cpu.setReg(srcReg, (int32_t)0);
    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = imm;
    cpu.setOnLessThanImmediate(i);

    EXPECT_EQ(cpu.getReg(targetReg), false);
}

TEST(CpuTest, SLTI_Set_Unsigned)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    int16_t imm = 10;

    cpu.setReg(srcReg, UINT32_MAX);
    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = imm;
    cpu.setOnLessThanImmediate(i);

    EXPECT_EQ(cpu.getReg(targetReg), true);
}

TEST(CpuTest, SLTI_No_Set_Unsigned)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    int16_t imm = (int16_t)UINT16_MAX;

    cpu.setReg(srcReg, 10);
    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = imm;
    cpu.setOnLessThanImmediate(i);

    EXPECT_EQ(cpu.getReg(targetReg), false);
}


TEST(CpuTest, SLTIU_Set_1)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    uint16_t imm = UINT16_MAX;

    cpu.setReg(srcReg, 10);
    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = imm;
    cpu.setOnLessThanImmediateUnsigned(i);

    EXPECT_EQ(cpu.getReg(targetReg), true);
}

TEST(CpuTest, SLTIU_Set_2)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    uint16_t imm = 1980;

    cpu.setReg(srcReg, 139);
    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = imm;
    cpu.setOnLessThanImmediateUnsigned(i);

    EXPECT_EQ(cpu.getReg(targetReg), true);
}

TEST(CpuTest, SLTIU_No_Set_1)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    uint16_t imm = 190;

    cpu.setReg(srcReg, 879482);
    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = imm;
    cpu.setOnLessThanImmediateUnsigned(i);

    EXPECT_EQ(cpu.getReg(targetReg), false);
}

TEST(CpuTest, SLTIU_Bounds_1)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    uint16_t imm = 0;

    cpu.setReg(srcReg, INT32_MAX);
    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = imm;
    cpu.setOnLessThanImmediateUnsigned(i);

    EXPECT_EQ(cpu.getReg(targetReg), false);
}

TEST(CpuTest, SLTIU_Bounds_2)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    uint16_t imm = UINT16_MAX;

    cpu.setReg(srcReg, 0);
    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = imm;
    cpu.setOnLessThanImmediateUnsigned(i);

    EXPECT_EQ(cpu.getReg(targetReg), true);
}

TEST(CpuTest, SLTIU_Bounds_3)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    uint16_t imm = UINT16_MAX;

    cpu.setReg(srcReg, UINT32_MAX);
    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = imm;
    cpu.setOnLessThanImmediateUnsigned(i);

    EXPECT_EQ(cpu.getReg(targetReg), false);
}

TEST(CpuTest, SLTIU_No_Set_Equal)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t srcReg = static_cast<uint8_t>(GprIndex::T0);
    uint8_t targetReg = static_cast<uint8_t>(GprIndex::T1);
    uint16_t imm = 0;

    cpu.setReg(srcReg, 0);
    i.i.rs = srcReg;
    i.i.rt = targetReg;
    i.i.immediate = imm;
    cpu.setOnLessThanImmediateUnsigned(i);

    EXPECT_EQ(cpu.getReg(targetReg), false);
}
