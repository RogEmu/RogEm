#include <gtest/gtest.h>
#include <memory>

#include "Core/SerialInterface.hpp"
#include "Core/Bus.hpp"

// Test fixture for SerialInterface tests
class SerialInterfaceTest : public ::testing::Test {
protected:
    void SetUp() override {
        bus = std::make_unique<Bus>();
        serialInterface = std::make_unique<SerialInterface>(bus.get());
    }

    std::unique_ptr<Bus> bus;
    std::unique_ptr<SerialInterface> serialInterface;

    // SIO0 register addresses (channel 0)
    static constexpr uint32_t SIO0_STAT = 0x1F801044;
    static constexpr uint32_t SIO0_MODE = 0x1F801048;
    static constexpr uint32_t SIO0_CTRL = 0x1F80104A;
    static constexpr uint32_t SIO0_BAUD = 0x1F80104E;

    // SIO1 register addresses (channel 1)
    static constexpr uint32_t SIO1_STAT = 0x1F801054;
    static constexpr uint32_t SIO1_MODE = 0x1F801058;
};

// Test STAT register (read-only)

TEST_F(SerialInterfaceTest, ReadSTATRegister)
{
    // STAT register should return 0x07
    uint16_t result = serialInterface->read16(SIO0_STAT);
    EXPECT_EQ(0x07, result);
}

TEST_F(SerialInterfaceTest, ReadSTATRegisterVia8Bit)
{
    // Reading via 8-bit should also work
    uint8_t result = serialInterface->read8(SIO0_STAT);
    EXPECT_EQ(0x07, result);
}

TEST_F(SerialInterfaceTest, ReadSTATRegisterVia32Bit)
{
    // Reading via 32-bit should also work
    uint32_t result = serialInterface->read32(SIO0_STAT);
    EXPECT_EQ(0x07, result);
}

// Test MODE register (read/write)

TEST_F(SerialInterfaceTest, WriteThenReadMODERegister16Bit)
{
    uint16_t testValue = 0x1234;
    serialInterface->write16(testValue, SIO0_MODE);

    uint16_t result = serialInterface->read16(SIO0_MODE);
    EXPECT_EQ(testValue, result);
}

TEST_F(SerialInterfaceTest, WriteThenReadMODERegister8Bit)
{
    uint8_t testValue = 0xAB;
    serialInterface->write8(testValue, SIO0_MODE);

    uint8_t result = serialInterface->read8(SIO0_MODE);
    EXPECT_EQ(testValue, result);
}

TEST_F(SerialInterfaceTest, WriteThenReadMODERegister32Bit)
{
    uint32_t testValue = 0xDEADBEEF;
    serialInterface->write32(testValue, SIO0_MODE);

    // 32-bit write truncates to 16-bit
    uint32_t result = serialInterface->read32(SIO0_MODE);
    EXPECT_EQ(0xBEEF, result);
}

TEST_F(SerialInterfaceTest, WriteMODEMultipleValues)
{
    serialInterface->write16(0xAAAA, SIO0_MODE);
    EXPECT_EQ(0xAAAA, serialInterface->read16(SIO0_MODE));

    serialInterface->write16(0x5555, SIO0_MODE);
    EXPECT_EQ(0x5555, serialInterface->read16(SIO0_MODE));

    serialInterface->write16(0x0000, SIO0_MODE);
    EXPECT_EQ(0x0000, serialInterface->read16(SIO0_MODE));
}

// Test CTRL register (read/write)

TEST_F(SerialInterfaceTest, WriteThenReadCTRLRegister)
{
    uint16_t testValue = 0xABCD;
    serialInterface->write16(testValue, SIO0_CTRL);

    uint16_t result = serialInterface->read16(SIO0_CTRL);
    EXPECT_EQ(testValue, result);
}

TEST_F(SerialInterfaceTest, WriteCTRLMultipleValues)
{
    serialInterface->write16(0xFFFF, SIO0_CTRL);
    EXPECT_EQ(0xFFFF, serialInterface->read16(SIO0_CTRL));

    serialInterface->write16(0x0001, SIO0_CTRL);
    EXPECT_EQ(0x0001, serialInterface->read16(SIO0_CTRL));

    serialInterface->write16(0x2000, SIO0_CTRL);
    EXPECT_EQ(0x2000, serialInterface->read16(SIO0_CTRL));
}

TEST_F(SerialInterfaceTest, CTRLRegisterPortSelection)
{
    // Test port selection bit (bit 13)
    serialInterface->write16(0x0000, SIO0_CTRL); // Port 0
    EXPECT_EQ(0x0000, serialInterface->read16(SIO0_CTRL));

    serialInterface->write16(0x2000, SIO0_CTRL); // Port 1 (bit 13 set)
    EXPECT_EQ(0x2000, serialInterface->read16(SIO0_CTRL));
}

// Test BAUD register (read/write)

TEST_F(SerialInterfaceTest, WriteThenReadBAUDRegister)
{
    uint16_t testValue = 0x4321;
    serialInterface->write16(testValue, SIO0_BAUD);

    uint16_t result = serialInterface->read16(SIO0_BAUD);
    EXPECT_EQ(testValue, result);
}

TEST_F(SerialInterfaceTest, WriteBAUDMultipleValues)
{
    serialInterface->write16(0x00FF, SIO0_BAUD);
    EXPECT_EQ(0x00FF, serialInterface->read16(SIO0_BAUD));

    serialInterface->write16(0xFF00, SIO0_BAUD);
    EXPECT_EQ(0xFF00, serialInterface->read16(SIO0_BAUD));

    serialInterface->write16(0xDEAD, SIO0_BAUD);
    EXPECT_EQ(0xDEAD, serialInterface->read16(SIO0_BAUD));
}

// Test combined operations

TEST_F(SerialInterfaceTest, MultipleRegisterWrites)
{
    // Write to multiple registers
    serialInterface->write16(0x1111, SIO0_MODE);
    serialInterface->write16(0x2222, SIO0_CTRL);
    serialInterface->write16(0x3333, SIO0_BAUD);

    // Verify all values are stored correctly
    EXPECT_EQ(0x1111, serialInterface->read16(SIO0_MODE));
    EXPECT_EQ(0x2222, serialInterface->read16(SIO0_CTRL));
    EXPECT_EQ(0x3333, serialInterface->read16(SIO0_BAUD));
}

TEST_F(SerialInterfaceTest, SequentialRegisterAccess)
{
    // Test reading and writing in sequence
    serialInterface->write16(0xBEEF, SIO0_MODE);
    EXPECT_EQ(0xBEEF, serialInterface->read16(SIO0_MODE));

    serialInterface->write16(0xCAFE, SIO0_CTRL);
    EXPECT_EQ(0xCAFE, serialInterface->read16(SIO0_CTRL));

    // Previous values should be preserved
    EXPECT_EQ(0xBEEF, serialInterface->read16(SIO0_MODE));
}

TEST_F(SerialInterfaceTest, MixedAccessSizes)
{
    // Write with 16-bit, read with 8-bit
    serialInterface->write16(0x1234, SIO0_MODE);
    EXPECT_EQ(0x34, serialInterface->read8(SIO0_MODE));

    // Write with 32-bit, read with 16-bit
    serialInterface->write32(0xABCD1234, SIO0_CTRL);
    EXPECT_EQ(0x1234, serialInterface->read16(SIO0_CTRL));

    // Write with 8-bit, read with 32-bit
    serialInterface->write8(0xFF, SIO0_BAUD);
    EXPECT_EQ(0xFF, serialInterface->read32(SIO0_BAUD));
}

// Test SIO1 (for coverage purposes)

TEST_F(SerialInterfaceTest, SIO1ReadSTAT)
{
    // Reading from SIO1 STAT should return 0x7 (as per readSIO1)
    uint16_t result = serialInterface->read16(SIO1_STAT);
    EXPECT_EQ(0x07, result);
}

TEST_F(SerialInterfaceTest, SIO1ReadMODE)
{
    // Reading from SIO1 MODE should return 0x7 (as per readSIO1)
    uint16_t result = serialInterface->read16(SIO1_MODE);
    EXPECT_EQ(0x07, result);
}

TEST_F(SerialInterfaceTest, SIO1WriteSTAT)
{
    // Writing to SIO1 should not crash (just logs an error)
    EXPECT_NO_THROW(serialInterface->write16(0x1234, SIO1_STAT));
}

TEST_F(SerialInterfaceTest, SIO1WriteMODE)
{
    // Writing to SIO1 should not crash (just logs an error)
    EXPECT_NO_THROW(serialInterface->write16(0xABCD, SIO1_MODE));
}

TEST_F(SerialInterfaceTest, SIO1Mixed8BitAccess)
{
    // Test 8-bit access to SIO1
    EXPECT_NO_THROW(serialInterface->write8(0x42, SIO1_STAT));
    uint8_t result = serialInterface->read8(SIO1_STAT);
    EXPECT_EQ(0x07, result);
}

TEST_F(SerialInterfaceTest, SIO1Mixed32BitAccess)
{
    // Test 32-bit access to SIO1
    EXPECT_NO_THROW(serialInterface->write32(0xDEADBEEF, SIO1_MODE));
    uint32_t result = serialInterface->read32(SIO1_MODE);
    EXPECT_EQ(0x07, result);
}

// Test interaction between SerialInterface and SIO0

TEST_F(SerialInterfaceTest, GetPadReferences)
{
    // Test that getting pad references doesn't crash
    EXPECT_NO_THROW(serialInterface->getPad(0));
    EXPECT_NO_THROW(serialInterface->getPad(1));
}

TEST_F(SerialInterfaceTest, ResetFunction)
{
    // Write some values
    serialInterface->write16(0x1234, SIO0_MODE);
    serialInterface->write16(0x5678, SIO0_CTRL);
    serialInterface->write16(0xABCD, SIO0_BAUD);

    // Reset should clear the SIO0 state
    EXPECT_NO_THROW(serialInterface->reset());
}

TEST_F(SerialInterfaceTest, UpdateFunction)
{
    // Update should not crash
    EXPECT_NO_THROW(serialInterface->update(100));
    EXPECT_NO_THROW(serialInterface->update(1000));
}
