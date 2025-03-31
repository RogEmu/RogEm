#include <gtest/gtest.h>
#include "Utils.h"
#include "BIOS.h"
#include "Bus.h"
#include "CPU.h"
#include "RAM.h"

TEST(CpuTest, SW_PositiveOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x2000;
    uint32_t store_value = 0xCAFEBABE;
    int16_t offset = 0x0018;  // Positive offset

    cpu.setReg(static_cast<uint8_t>(GprIndex::A0), base_address);
    cpu.setReg(static_cast<uint8_t>(GprIndex::V0), store_value);

    i.i.rs = static_cast<uint8_t>(GprIndex::A0);
    i.i.rt = static_cast<uint8_t>(GprIndex::V0);
    i.i.immediate = offset;

    cpu.storeWord(i);

    EXPECT_EQ(ram.loadWord(base_address + offset), store_value);
}

TEST(CpuTest, SW_NegativeOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x3008;
    uint32_t store_value = 0xFEEDFACE;
    int16_t offset = -4164;  // Negative offset

    cpu.setReg(static_cast<uint8_t>(GprIndex::A1), base_address);
    cpu.setReg(static_cast<uint8_t>(GprIndex::V1), store_value);

    i.i.rs = static_cast<uint8_t>(GprIndex::A1);
    i.i.rt = static_cast<uint8_t>(GprIndex::V1);
    i.i.immediate = offset;

    cpu.storeWord(i);

    EXPECT_EQ(ram.loadWord(base_address + offset), store_value);
}

TEST(CpuTest, SW_ZeroOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x4000;
    uint32_t store_value = 0x12345678;
    int16_t offset = 0;  // Zero offset

    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), base_address);
    cpu.setReg(static_cast<uint8_t>(GprIndex::S1), store_value);

    i.i.rs = static_cast<uint8_t>(GprIndex::T1);
    i.i.rt = static_cast<uint8_t>(GprIndex::S1);
    i.i.immediate = offset;

    cpu.storeWord(i);

    EXPECT_EQ(ram.loadWord(base_address + offset), store_value);
}

TEST(CpuTest, SW_MaxOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x5001;
    uint32_t store_value = 0x89ABCDEF;
    int16_t offset = INT16_MAX;  // Maximum positive offset

    cpu.setReg(static_cast<uint8_t>(GprIndex::S2), base_address);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T2), store_value);

    i.i.rs = static_cast<uint8_t>(GprIndex::S2);
    i.i.rt = static_cast<uint8_t>(GprIndex::T2);
    i.i.immediate = offset;

    cpu.storeWord(i);

    EXPECT_EQ(ram.loadWord(base_address + offset), store_value);
}

TEST(CpuTest, SW_MinOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x16000;
    uint32_t store_value = 0x10203040;
    int16_t offset = INT16_MIN;  // Minimum negative offset

    cpu.setReg(static_cast<uint8_t>(GprIndex::S3), base_address);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T3), store_value);

    i.i.rs = static_cast<uint8_t>(GprIndex::S3);
    i.i.rt = static_cast<uint8_t>(GprIndex::T3);
    i.i.immediate = offset;

    cpu.storeWord(i);

    EXPECT_EQ(ram.loadWord(base_address + offset), store_value);
}
