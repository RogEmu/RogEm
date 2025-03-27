#include <gtest/gtest.h>
#include "Utils.h"
#include "BIOS.h"
#include "Bus.h"
#include "CPU.h"

TEST(CpuTest, ORI_1)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);

    // Or immediate value 0x1F1F with $0 into $5
    Instruction i;
    i.i.rs = 0x00;
    i.i.rt = 0x05;
    i.i.immediate = 0x1F1F;

    cpu.orImmediateWord(i);

    EXPECT_EQ(cpu.gpr[i.i.rt], i.i.immediate);
}

TEST(CpuTest, ORI_2)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);

    // Or immediate value 0x1F1F with $0 into $1
    Instruction i;
    i.i.rs = 0x00;
    i.i.rt = 0x01;
    i.i.immediate = 0x1F1F;

    cpu.orImmediateWord(i);
    EXPECT_EQ(cpu.gpr[i.i.rt], i.i.immediate);

    // Or immediate value 0x1234 with $0 into $2
    i.i.rt = 0x02;
    i.i.immediate = 0x1234;
    cpu.orImmediateWord(i);
    EXPECT_EQ(cpu.gpr[i.i.rt], i.i.immediate);

    // Or immediate value 0x1234 with $1 into $2
    i.i.rt = 0x02;
    i.i.rs = 0x01;
    i.i.immediate = 0x1F3F;
    cpu.orImmediateWord(i);
    EXPECT_EQ(cpu.gpr[i.i.rt], i.i.immediate);

}

TEST(CpuTest, ANDI_1)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    loadImmediate(cpu, 8, 0x42424242);
    i.i.rs = 8;
    i.i.rt = 8;
    i.i.immediate = 0xBEEF;
    cpu.andImmediateWord(i);

    EXPECT_EQ(cpu.gpr[i.i.rt], 0x42424242 & 0xBEEF);
}

TEST(CpuTest, ANDI_2)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = 0x89FD3EAB;
    uint16_t imm = 0x9A4F;

    loadImmediate(cpu, 8, value);
    i.i.rs = 8;
    i.i.rt = 12;
    i.i.immediate = imm;
    cpu.andImmediateWord(i);

    EXPECT_EQ(cpu.gpr[i.i.rt], value & imm);
}

TEST(CpuTest, ANDI_3)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = 0x89FD3EAB;
    uint16_t imm = 0x1;

    loadImmediate(cpu, 8, value);
    i.i.rs = 8;
    i.i.rt = 9;
    i.i.immediate = imm;
    cpu.andImmediateWord(i);

    EXPECT_EQ(cpu.gpr[i.i.rt], value & imm);
}

TEST(CpuTest, XORI_1)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t value = 0x89FD3EAB;
    uint16_t imm = 0x0;

    loadImmediate(cpu, 8, value);
    i.i.rs = 8;
    i.i.rt = 9;
    i.i.immediate = imm;
    cpu.xorImmediateWord(i);

    EXPECT_EQ(cpu.gpr[i.i.rt], value ^ imm);
}

TEST(CpuTest, AND_1)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t rsVal = 0x12345678;
    uint32_t rtVal = 0x89FD3EAB;

    loadImmediate(cpu, 8, rsVal);
    loadImmediate(cpu, 9, rtVal);
    i.r.rs = 8;
    i.r.rt = 9;
    i.r.rd = 10;
    cpu.andWord(i);

    EXPECT_EQ(cpu.gpr[i.r.rd], rsVal & rtVal);
}


TEST(CpuTest, AND_2)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t rsVal = 0xFFFFFFFF;
    uint32_t rtVal = 0x89FD3EAB;

    loadImmediate(cpu, 8, rsVal);
    loadImmediate(cpu, 9, rtVal);
    i.r.rs = 8;
    i.r.rt = 9;
    i.r.rd = 10;
    cpu.andWord(i);

    EXPECT_EQ(cpu.gpr[i.r.rd], rsVal & rtVal);
}

TEST(CpuTest, AND_3)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t rsVal = 0x11111111;
    uint32_t rtVal = 0x89FD3EAB;

    loadImmediate(cpu, 8, rsVal);
    loadImmediate(cpu, 9, rtVal);
    i.r.rs = 8;
    i.r.rt = 9;
    i.r.rd = 8;
    cpu.andWord(i);

    EXPECT_EQ(cpu.gpr[i.r.rd], rsVal & rtVal);
}

TEST(CpuTest, OR_1)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t rsVal = 0xFFFF0000;
    uint32_t rtVal = 0x0000FFFF;

    loadImmediate(cpu, 8, rsVal);
    loadImmediate(cpu, 9, rtVal);
    i.r.rs = 8;
    i.r.rt = 9;
    i.r.rd = 10;
    cpu.orWord(i);

    EXPECT_EQ(cpu.gpr[i.r.rd], rsVal | rtVal);
}

TEST(CpuTest, OR_2)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t rsVal = 0x0;
    uint32_t rtVal = 0x0000FFFF;

    loadImmediate(cpu, 8, rsVal);
    loadImmediate(cpu, 9, rtVal);
    i.r.rs = 8;
    i.r.rt = 9;
    i.r.rd = 8;
    cpu.orWord(i);

    EXPECT_EQ(cpu.gpr[i.r.rd], rsVal | rtVal);
}


TEST(CpuTest, OR_3)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t rsVal = 0x78492B32;
    uint32_t rtVal = 0x127F98BA;

    loadImmediate(cpu, 10, rsVal);
    loadImmediate(cpu, 12, rtVal);
    i.r.rs = 10;
    i.r.rt = 12;
    i.r.rd = 15;
    cpu.orWord(i);

    EXPECT_EQ(cpu.gpr[i.r.rd], rsVal | rtVal);
}

TEST(CpuTest, XOR_1)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t rsVal = 0xFFA43EBC;
    uint32_t rtVal = 0x0000FFFF;

    loadImmediate(cpu, 8, rsVal);
    loadImmediate(cpu, 9, rtVal);
    i.r.rs = 8;
    i.r.rt = 9;
    i.r.rd = 8;
    cpu.xorWord(i);

    EXPECT_EQ(cpu.gpr[i.r.rd], rsVal ^ rtVal);
}

TEST(CpuTest, XOR_2)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint32_t rsVal = 0x0;
    uint32_t rtVal = 0x0;

    loadImmediate(cpu, 8, rsVal);
    loadImmediate(cpu, 9, rtVal);
    i.r.rs = 8;
    i.r.rt = 9;
    i.r.rd = 8;
    cpu.xorWord(i);

    EXPECT_EQ(cpu.gpr[i.r.rd], rsVal ^ rtVal);
}

TEST(CpuTest, NOR_1)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t rs = 11;
    uint8_t rt = 12;
    uint8_t rd = 18;
    uint32_t rsVal = 0x1F1F1F1F;
    uint32_t rtVal = 0x78563412;

    loadImmediate(cpu, rs, rsVal);
    loadImmediate(cpu, rt, rtVal);
    i.r.rs = rs;
    i.r.rt = rt;
    i.r.rd = rd;
    cpu.norWord(i);

    EXPECT_EQ(cpu.gpr[rd], ~(rsVal | rtVal));
}

TEST(CpuTest, NOR_2)
{
    auto bios = BIOS();
    auto bus = Bus(&bios, nullptr);
    CPU cpu(&bus);
    Instruction i;

    uint8_t rs = 11;
    uint8_t rt = 12;
    uint8_t rd = 18;
    uint32_t rsVal = 0x11111111;
    uint32_t rtVal = 0x0000FFFF;

    loadImmediate(cpu, rs, rsVal);
    loadImmediate(cpu, rt, rtVal);
    i.r.rs = rs;
    i.r.rt = rt;
    i.r.rd = rd;
    cpu.norWord(i);

    EXPECT_EQ(cpu.gpr[rd], ~(rsVal | rtVal));
}
