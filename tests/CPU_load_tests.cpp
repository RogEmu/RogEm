#include <gtest/gtest.h>
#include "Utils.h"
#include "BIOS.h"
#include "RAM.h"
#include "Bus.h"
#include "CPU.h"

#include <sstream>

namespace testing {
    template <>
    std::string PrintToString<uint32_t>(const uint32_t& value) {
        std::stringstream ss;
        ss << "0x" << std::hex << value;
        return ss.str();
    }
}

TEST(CpuTest, LW_AlignedAddress)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1000;
    uint32_t offset = 0x4;
    uint32_t address = base_address + offset;
    uint32_t expected_value = 0xDEADBEEF;

    ram.storeWord(address, expected_value); // Store value in RAM

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = offset;

    cpu.loadWord(i);

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), expected_value); // Compare T0 register value
}

TEST(CpuTest, LW_UnalignedAddress)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1000;
    uint32_t offset = 0x3; // Unaligned
    uint32_t initial_rt_value = 0xBADF00D;

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), initial_rt_value); // Set initial value in T0

    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = offset;

    cpu.loadWord(i);

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), initial_rt_value); // Register should remain unchanged
}

TEST(CpuTest, LW_NegativeOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x2008;
    uint32_t offset = -4; // Negative offset
    uint32_t target_address = base_address + offset;
    uint32_t expected_value = 0x12345678;

    ram.storeWord(target_address, expected_value); // Store word at target address

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = offset;

    cpu.loadWord(i);

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), expected_value); // Verify loaded value
}

TEST(CpuTest, LW_MaxAddress)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1FFFC;
    uint32_t offset = 0x0; // No offset
    uint32_t address = base_address + offset;
    uint32_t expected_value = 0xAABBCCDD;

    ram.storeWord(address, expected_value);

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = offset;

    cpu.loadWord(i);

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), expected_value); // Verify loaded value
}

TEST(CpuTest, LW_ZeroRegister)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1000;
    uint32_t offset = 0x4;
    uint32_t address = base_address + offset;
    uint32_t value = 0xCAFEBABE;

    ram.storeWord(address, value);

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::ZERO); // Writing to $zero
    i.i.immediate = offset;

    cpu.loadWord(i);

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::ZERO)), 0); // $zero must remain 0
}

TEST(CpuTest, LH_BasicLoad_PositiveOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1000;  // Address in valid RAM space (KUSEG)
    uint16_t halfword_value = 0x1234;  // A simple 16-bit value
    uint16_t offset = 4;  // A simple 16-bit value

    ram.storeHalfWord(base_address + offset, halfword_value); // Store the halfword with positive offset

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = offset; // Positive offset

    cpu.loadHalfWord(i); // Execute LH instruction

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), static_cast<int32_t>(halfword_value)); // Verify the value is correctly loaded
}

TEST(CpuTest, LH_BasicLoad_NegativeOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1000;  // Address in valid RAM space (KUSEG)
    uint16_t halfword_value = 0x5678;  // Another simple 16-bit value
    int16_t offset = -4;  // Negative 16-bit value

    ram.storeHalfWord(base_address + offset, halfword_value); // Store the halfword with negative offset

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = (uint16_t)offset; // Negative offset

    cpu.loadHalfWord(i); // Execute LH instruction

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), static_cast<int32_t>(halfword_value)); // Verify the value is correctly loaded
}

TEST(CpuTest, LH_NegativeLoad_PositiveOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1000;  // Address in valid RAM space (KUSEG)
    int16_t halfword_value = -32768;  // Smallest possible 16-bit signed value (-32768 = 0x8000)
    int16_t offset = 2;  // Positive offset

    ram.storeHalfWord(base_address + offset, halfword_value); // Store the negative value with positive offset

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = (uint16_t)offset;

    cpu.loadHalfWord(i); // Execute LH instruction

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), static_cast<int32_t>(halfword_value)); // Verify the negative value is correctly sign-extended
}

TEST(CpuTest, LH_NegativeLoad_NegativeOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1000;  // Address in valid RAM space (KUSEG)
    int16_t halfword_value = -32768;  // Smallest possible 16-bit signed value (-32768 = 0x8000)
    int16_t offset = -2;  // Negative offset

    ram.storeHalfWord(base_address + offset, halfword_value); // Store the negative value with negative offset

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = (uint16_t)offset; // Negative offset

    cpu.loadHalfWord(i); // Execute LH instruction

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), static_cast<int32_t>(halfword_value)); // Verify the negative value is correctly sign-extended
}

TEST(CpuTest, LH_UnalignedLoad_PositiveOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1001;  // Unaligned base address
    int16_t offset = 130;  // Positive offset

    // Store an initial value in the destination register (T0) to check if it's untouched
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 0xDEADBEEF);  // Set T0 to a known value

    // Set the base register (SP) to the unaligned address
    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = static_cast<int16_t>(offset); // Positive unaligned offset

    // Attempt to execute LH instruction with an unaligned base address
    cpu.loadHalfWord(i);  // Execute LH instruction

    // Check if the value of T0 is unchanged (it should remain 0xDEADBEEF)
    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), 0xDEADBEEF);
}

TEST(CpuTest, LH_UnalignedLoad_NegativeOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1001;  // Unaligned base address
    int16_t offset = -130;  // Positive offset

    // Store an initial value in the destination register (T0) to check if it's untouched
    cpu.setReg(static_cast<uint8_t>(GprIndex::T0), 0xDEADBEEF);  // Set T0 to a known value

    // Set the base register (SP) to the unaligned address
    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = static_cast<int16_t>(offset); // Positive unaligned offset

    // Attempt to execute LH instruction with an unaligned base address
    cpu.loadHalfWord(i);  // Execute LH instruction

    // Check if the value of T0 is unchanged (it should remain 0xDEADBEEF)
    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), 0xDEADBEEF);
}

TEST(CpuTest, LH_MaxPositiveOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1001;  // Address in valid RAM space (KUSEG)
    uint16_t halfword_value = 0x1234;  // A simple 16-bit value

    // Store with the max positive offset (INT16_MAX)
    ram.storeHalfWord(base_address + INT16_MAX, halfword_value);

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = (uint16_t)INT16_MAX; // Maximum positive offset

    cpu.loadHalfWord(i); // Execute LH instruction

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), static_cast<int32_t>(halfword_value)); // Verify the value is correctly loaded
}

TEST(CpuTest, LH_MaxNegativeOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1FFFF0;  // Address in valid RAM space (KUSEG)
    uint16_t halfword_value = 0x5678;  // Another simple 16-bit value

    // Store with the max negative offset (INT16_MIN)
    ram.storeHalfWord(base_address + INT16_MIN, halfword_value);

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = (uint16_t)INT16_MIN; // Maximum negative offset

    cpu.loadHalfWord(i); // Execute LH instruction

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), static_cast<int32_t>(halfword_value)); // Verify the value is correctly loaded
}

TEST(CpuTest, LH_NegativeOffset_SignExtension)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1000;  // Address in valid RAM space (KUSEG)
    uint16_t halfword_value = 0xFABC;  // A negative value in 16-bit (sign bit set)
    int16_t offset = -2424;  // A negative value in 16-bit (sign bit set)

    // Store a negative value with a negative offset
    ram.storeHalfWord(base_address + offset, halfword_value);

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address); // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = (uint16_t)offset; // Maximum negative offset

    cpu.loadHalfWord(i); // Execute LH instruction

    // Check if the value was correctly sign-extended into a 32-bit register
    // The 16-bit value 0xFABC should be sign-extended to 0xFFFFFABC (32-bit value)
    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), 0xFFFFFABC); // Verify the sign extension
}

TEST(CpuTest, LB_RegularLoad)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1000;  // Aligned base address
    uint8_t byte_value = 0x17;  // Byte value to store
    int16_t offset = 0x131;

    ram.storeByte(base_address + offset, byte_value);  // Store the byte at the base address

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address);  // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = (uint16_t)offset;

    cpu.loadByte(i);  // Execute LB instruction

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), 0x00000017);  // Verify byte value is correctly loaded
}

TEST(CpuTest, LB_SignExtension_NegativeValue)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1000;
    uint8_t byte_value = 0xAB;  // Negative byte value
    int16_t offset = 0x87;

    ram.storeByte(base_address + offset, byte_value);  // Store negative byte

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address);  // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = (uint16_t)offset;

    cpu.loadByte(i);  // Execute LB instruction

    // The byte -0x10 (0xF0) should be sign-extended to 32 bits
    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), static_cast<int32_t>(0xFFFFFFAB));  // Verify sign extension
}

TEST(CpuTest, LB_SignExtension_PositiveValue)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1000;
    uint8_t byte_value = 0x7F;  // Positive byte value
    int16_t offset = 0xAB;

    ram.storeByte(base_address + offset, byte_value);  // Store positive byte

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address);  // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = (uint16_t)offset;

    cpu.loadByte(i);  // Execute LB instruction

    // The byte 0x7F should be zero-extended to 32 bits
    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), static_cast<int32_t>(0x0000007F));  // Verify zero extension
}

TEST(CpuTest, LB_SignExtension_NegativeOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x1005;
    uint8_t byte_value = 0xAB;
    int16_t offset = -0x131;

    ram.storeByte(base_address + offset, byte_value);  // Store byte at base address

    cpu.setReg(static_cast<uint8_t>(GprIndex::SP), base_address);  // Set base register (SP)
    i.i.rs = static_cast<uint8_t>(GprIndex::SP);  // rs = SP
    i.i.rt = static_cast<uint8_t>(GprIndex::T0); // rt = T0
    i.i.immediate = (uint16_t)offset;  // Negative offset

    cpu.loadByte(i);  // Execute LB instruction

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T0)), static_cast<int32_t>(0xFFFFFFAB));  // Verify the byte is correctly loaded
}

TEST(CpuTest, LHU_PositiveOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x2000;
    uint16_t halfword_value = 0x1234;
    int16_t offset = 0x1368; // Positive aligned offset

    ram.storeHalfWord(base_address + offset, halfword_value);

    cpu.setReg(static_cast<uint8_t>(GprIndex::A0), base_address);

    i.i.rs = static_cast<uint8_t>(GprIndex::A0);
    i.i.rt = static_cast<uint8_t>(GprIndex::V0);
    i.i.immediate = offset;

    cpu.loadHalfWordUnsigned(i);

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::V0)), static_cast<uint32_t>(halfword_value));
}

TEST(CpuTest, LHU_NegativeOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x3004;
    uint16_t halfword_value = 0x5678;
    int16_t offset = -1738; // Negative offset, ensuring correct address calculation

    ram.storeHalfWord(base_address + offset, halfword_value);

    cpu.setReg(static_cast<uint8_t>(GprIndex::A1), base_address);

    i.i.rs = static_cast<uint8_t>(GprIndex::A1);
    i.i.rt = static_cast<uint8_t>(GprIndex::V1);
    i.i.immediate = offset;

    cpu.loadHalfWordUnsigned(i);

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::V1)), static_cast<uint32_t>(halfword_value));
}

TEST(CpuTest, LHU_ZeroOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x4000;
    uint16_t halfword_value = 0x9ABC;
    int16_t offset = 0; // Zero offset

    ram.storeHalfWord(base_address + offset, halfword_value);

    cpu.setReg(static_cast<uint8_t>(GprIndex::T1), base_address);

    i.i.rs = static_cast<uint8_t>(GprIndex::T1);
    i.i.rt = static_cast<uint8_t>(GprIndex::S1);
    i.i.immediate = offset;

    cpu.loadHalfWordUnsigned(i);

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::S1)), static_cast<uint32_t>(halfword_value));
}

TEST(CpuTest, LHU_MaxOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x5001;
    uint16_t halfword_value = 0xDEAD;
    int16_t offset = INT16_MAX; // Maximum positive offset

    ram.storeHalfWord(base_address + offset, halfword_value);

    cpu.setReg(static_cast<uint8_t>(GprIndex::S2), base_address);

    i.i.rs = static_cast<uint8_t>(GprIndex::S2);
    i.i.rt = static_cast<uint8_t>(GprIndex::T2);
    i.i.immediate = offset;

    cpu.loadHalfWordUnsigned(i);

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T2)), static_cast<uint32_t>(halfword_value));
}

TEST(CpuTest, LHU_MinOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0xF080;
    uint16_t halfword_value = 0xBEEF;
    int16_t offset = INT16_MIN; // Minimum negative offset

    ram.storeHalfWord(base_address + offset, halfword_value);

    cpu.setReg(static_cast<uint8_t>(GprIndex::S3), base_address);

    i.i.rs = static_cast<uint8_t>(GprIndex::S3);
    i.i.rt = static_cast<uint8_t>(GprIndex::T3);
    i.i.immediate = offset;

    cpu.loadHalfWordUnsigned(i);

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T3)), static_cast<uint32_t>(halfword_value));
}

TEST(CpuTest, LHU_UnalignedAddress_PositiveOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x7008;
    int16_t offset = 0xB;
    uint16_t halfword_value = 0x1234;

    ram.storeHalfWord(base_address + offset, halfword_value);

    cpu.setReg(static_cast<uint8_t>(GprIndex::S4), base_address);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T4), 0xFFFFFFFF); // Ensure the value is not modified

    i.i.rs = static_cast<uint8_t>(GprIndex::S4);
    i.i.rt = static_cast<uint8_t>(GprIndex::T4);
    i.i.immediate = offset;

    cpu.loadHalfWordUnsigned(i);

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T4)), 0xFFFFFFFF); // Should remain unchanged
}

TEST(CpuTest, LHU_UnalignedAddress_NegativeOffset)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x7008;
    int16_t offset = -0xB;
    uint16_t halfword_value = 0x1234;

    ram.storeHalfWord(base_address + offset, halfword_value);

    cpu.setReg(static_cast<uint8_t>(GprIndex::S4), base_address);
    cpu.setReg(static_cast<uint8_t>(GprIndex::T4), 0xFFFFFFFF); // Ensure the value is not modified

    i.i.rs = static_cast<uint8_t>(GprIndex::S4);
    i.i.rt = static_cast<uint8_t>(GprIndex::T4);
    i.i.immediate = offset;

    cpu.loadHalfWordUnsigned(i);

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::T4)), 0xFFFFFFFF); // Should remain unchanged
}

TEST(CpuTest, LHU_ZeroExtension)
{
    auto bios = BIOS();
    auto ram = RAM();
    auto bus = Bus(&bios, &ram);
    CPU cpu(&bus);
    Instruction i;

    uint32_t base_address = 0x8000;
    uint16_t halfword_value = 0xABCD;
    int16_t offset = 2;

    ram.storeHalfWord(base_address + offset, halfword_value);

    cpu.setReg(static_cast<uint8_t>(GprIndex::T5), base_address);

    i.i.rs = static_cast<uint8_t>(GprIndex::T5);
    i.i.rt = static_cast<uint8_t>(GprIndex::S5);
    i.i.immediate = offset;

    cpu.loadHalfWordUnsigned(i);

    EXPECT_EQ(cpu.getReg(static_cast<uint8_t>(GprIndex::S5)), static_cast<uint32_t>(halfword_value));
    // Must be zero-extended: 0x0000ABCD
}
