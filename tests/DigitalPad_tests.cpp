#include <gtest/gtest.h>
#include "Core/DigitalPad.hpp"

class DigitalPadTest : public ::testing::Test {
protected:
    void SetUp() override {
        pad = std::make_unique<DigitalPad>();
    }

    std::unique_ptr<DigitalPad> pad;
};

// Test initial state: disconnected controller returns 0xFFFF
TEST_F(DigitalPadTest, DisconnectedControllerReturnsHighZ) {
    pad->tx(0x01);
    EXPECT_EQ(pad->rx(), 0xFFFF);

    pad->tx(0x42);
    EXPECT_EQ(pad->rx(), 0xFFFF);
}

// Test connect/disconnect functionality
TEST_F(DigitalPadTest, ConnectDisconnect) {
    pad->connect();
    EXPECT_TRUE(pad->isConnected());

    pad->disconnect();
    EXPECT_FALSE(pad->isConnected());
}

// Test full communication sequence: Address byte (0x01)
TEST_F(DigitalPadTest, CommunicationSequence_AddressByte) {
    pad->connect();

    // Send address byte 0x01, expect Hi-Z (0xFF)
    pad->tx(0x01);
    EXPECT_EQ(pad->rx(), 0xFF);
}

// Test full communication sequence: Command byte (0x42)
TEST_F(DigitalPadTest, CommunicationSequence_CommandByte) {
    pad->connect();

    // Send address byte
    pad->tx(0x01);
    pad->rx();

    // Send command byte 0x42 (Read), expect ID low byte (0x41)
    pad->tx(0x42);
    EXPECT_EQ(pad->rx(), 0x41);
}

// Test full communication sequence: ID hi byte
TEST_F(DigitalPadTest, CommunicationSequence_IDHiByte) {
    pad->connect();

    // Send address and command
    pad->tx(0x01);
    pad->rx();
    pad->tx(0x42);
    pad->rx();

    // Expect ID high byte (0x5A)
    EXPECT_EQ(pad->rx(), 0x5A);
}

// Test full communication sequence: Button data low byte
TEST_F(DigitalPadTest, CommunicationSequence_ButtonDataLow) {
    pad->connect();

    // Complete sequence to MOT0 state
    pad->tx(0x01);
    pad->rx();
    pad->tx(0x42);
    pad->rx();
    pad->rx(); // IDHI

    // Expect button data low byte (all released = 0xFF)
    EXPECT_EQ(pad->rx(), 0xFF);
}

// Test full communication sequence: Button data high byte
TEST_F(DigitalPadTest, CommunicationSequence_ButtonDataHigh) {
    pad->connect();

    // Complete sequence to MOT1 state
    pad->tx(0x01);
    pad->rx();
    pad->tx(0x42);
    pad->rx();
    pad->rx(); // IDHI
    pad->rx(); // MOT0

    // Expect button data high byte (all released = 0xFF)
    EXPECT_EQ(pad->rx(), 0xFF);
}

// Test state machine reset to HighZ after complete sequence
TEST_F(DigitalPadTest, StateResetAfterCompleteSequence) {
    pad->connect();

    // Complete full sequence
    pad->tx(0x01);
    pad->rx();
    pad->tx(0x42);
    pad->rx();
    pad->rx(); // IDHI
    pad->rx(); // MOT0
    pad->rx(); // MOT1

    // State should reset to HighZ, next address byte should work
    pad->tx(0x01);
    EXPECT_EQ(pad->rx(), 0xFF);
}

// Test Select button (bit 0)
TEST_F(DigitalPadTest, SelectButtonPressed) {
    pad->connect();
    pad->updateButtons(static_cast<uint16_t>(~0x0001)); // Bit 0 = 0 (pressed)

    // Navigate to button data
    pad->tx(0x01);
    pad->rx();
    pad->tx(0x42);
    pad->rx();
    pad->rx(); // IDHI

    uint16_t button_lo = pad->rx();
    EXPECT_EQ(button_lo & 0x01, 0); // Select pressed
}

// Test Start button (bit 3)
TEST_F(DigitalPadTest, StartButtonPressed) {
    pad->connect();
    pad->updateButtons(static_cast<uint16_t>(~0x0008)); // Bit 3 = 0 (pressed)

    // Navigate to button data
    pad->tx(0x01);
    pad->rx();
    pad->tx(0x42);
    pad->rx();
    pad->rx(); // IDHI

    uint16_t button_lo = pad->rx();
    EXPECT_EQ(button_lo & 0x08, 0); // Start pressed
}

// Test D-pad Up button (bit 4)
TEST_F(DigitalPadTest, DPadUpPressed) {
    pad->connect();
    pad->updateButtons(static_cast<uint16_t>(~0x0010)); // Bit 4 = 0 (pressed)

    // Navigate to button data
    pad->tx(0x01);
    pad->rx();
    pad->tx(0x42);
    pad->rx();
    pad->rx(); // IDHI

    uint16_t button_lo = pad->rx();
    EXPECT_EQ(button_lo & 0x10, 0); // Up pressed
}

// Test D-pad Right button (bit 5)
TEST_F(DigitalPadTest, DPadRightPressed) {
    pad->connect();
    pad->updateButtons(static_cast<uint16_t>(~0x0020)); // Bit 5 = 0 (pressed)

    // Navigate to button data
    pad->tx(0x01);
    pad->rx();
    pad->tx(0x42);
    pad->rx();
    pad->rx(); // IDHI

    uint16_t button_lo = pad->rx();
    EXPECT_EQ(button_lo & 0x20, 0); // Right pressed
}

// Test D-pad Down button (bit 6)
TEST_F(DigitalPadTest, DPadDownPressed) {
    pad->connect();
    pad->updateButtons(static_cast<uint16_t>(~0x0040)); // Bit 6 = 0 (pressed)

    // Navigate to button data
    pad->tx(0x01);
    pad->rx();
    pad->tx(0x42);
    pad->rx();
    pad->rx(); // IDHI

    uint16_t button_lo = pad->rx();
    EXPECT_EQ(button_lo & 0x40, 0); // Down pressed
}

// Test D-pad Left button (bit 7)
TEST_F(DigitalPadTest, DPadLeftPressed) {
    pad->connect();
    pad->updateButtons(static_cast<uint16_t>(~0x0080)); // Bit 7 = 0 (pressed)

    // Navigate to button data
    pad->tx(0x01);
    pad->rx();
    pad->tx(0x42);
    pad->rx();
    pad->rx(); // IDHI

    uint16_t button_lo = pad->rx();
    EXPECT_EQ(button_lo & 0x80, 0); // Left pressed
}

// Test L2 button (bit 8)
TEST_F(DigitalPadTest, L2ButtonPressed) {
    pad->connect();
    pad->updateButtons(static_cast<uint16_t>(~0x0100)); // Bit 8 = 0 (pressed)

    // Navigate to button data high byte
    pad->tx(0x01);
    pad->rx();
    pad->tx(0x42);
    pad->rx();
    pad->rx(); // IDHI
    pad->rx(); // MOT0

    uint16_t button_hi = pad->rx();
    EXPECT_EQ(button_hi & 0x01, 0); // L2 pressed
}

// Test R2 button (bit 9)
TEST_F(DigitalPadTest, R2ButtonPressed) {
    pad->connect();
    pad->updateButtons(static_cast<uint16_t>(~0x0200)); // Bit 9 = 0 (pressed)

    // Navigate to button data high byte
    pad->tx(0x01);
    pad->rx();
    pad->tx(0x42);
    pad->rx();
    pad->rx(); // IDHI
    pad->rx(); // MOT0

    uint16_t button_hi = pad->rx();
    EXPECT_EQ(button_hi & 0x02, 0); // R2 pressed
}

// Test L1 button (bit 10)
TEST_F(DigitalPadTest, L1ButtonPressed) {
    pad->connect();
    pad->updateButtons(static_cast<uint16_t>(~0x0400)); // Bit 10 = 0 (pressed)

    // Navigate to button data high byte
    pad->tx(0x01);
    pad->rx();
    pad->tx(0x42);
    pad->rx();
    pad->rx(); // IDHI
    pad->rx(); // MOT0

    uint16_t button_hi = pad->rx();
    EXPECT_EQ(button_hi & 0x04, 0); // L1 pressed
}

// Test R1 button (bit 11)
TEST_F(DigitalPadTest, R1ButtonPressed) {
    pad->connect();
    pad->updateButtons(static_cast<uint16_t>(~0x0800)); // Bit 11 = 0 (pressed)

    // Navigate to button data high byte
    pad->tx(0x01);
    pad->rx();
    pad->tx(0x42);
    pad->rx();
    pad->rx(); // IDHI
    pad->rx(); // MOT0

    uint16_t button_hi = pad->rx();
    EXPECT_EQ(button_hi & 0x08, 0); // R1 pressed
}

// Test Triangle button (bit 12)
TEST_F(DigitalPadTest, TriangleButtonPressed) {
    pad->connect();
    pad->updateButtons(static_cast<uint16_t>(~0x1000)); // Bit 12 = 0 (pressed)

    // Navigate to button data high byte
    pad->tx(0x01);
    pad->rx();
    pad->tx(0x42);
    pad->rx();
    pad->rx(); // IDHI
    pad->rx(); // MOT0

    uint16_t button_hi = pad->rx();
    EXPECT_EQ(button_hi & 0x10, 0); // Triangle pressed
}

// Test Circle button (bit 13)
TEST_F(DigitalPadTest, CircleButtonPressed) {
    pad->connect();
    pad->updateButtons(static_cast<uint16_t>(~0x2000)); // Bit 13 = 0 (pressed)

    // Navigate to button data high byte
    pad->tx(0x01);
    pad->rx();
    pad->tx(0x42);
    pad->rx();
    pad->rx(); // IDHI
    pad->rx(); // MOT0

    uint16_t button_hi = pad->rx();
    EXPECT_EQ(button_hi & 0x20, 0); // Circle pressed
}

// Test Cross button (bit 14)
TEST_F(DigitalPadTest, CrossButtonPressed) {
    pad->connect();
    pad->updateButtons(static_cast<uint16_t>(~0x4000)); // Bit 14 = 0 (pressed)

    // Navigate to button data high byte
    pad->tx(0x01);
    pad->rx();
    pad->tx(0x42);
    pad->rx();
    pad->rx(); // IDHI
    pad->rx(); // MOT0

    uint16_t button_hi = pad->rx();
    EXPECT_EQ(button_hi & 0x40, 0); // Cross pressed
}

// Test Square button (bit 15)
TEST_F(DigitalPadTest, SquareButtonPressed) {
    pad->connect();
    pad->updateButtons(static_cast<uint16_t>(~0x8000)); // Bit 15 = 0 (pressed)

    // Navigate to button data high byte
    pad->tx(0x01);
    pad->rx();
    pad->tx(0x42);
    pad->rx();
    pad->rx(); // IDHI
    pad->rx(); // MOT0

    uint16_t button_hi = pad->rx();
    EXPECT_EQ(button_hi & 0x80, 0); // Square pressed
}

// Test multiple buttons pressed simultaneously
TEST_F(DigitalPadTest, MultipleButtonsPressed) {
    pad->connect();
    // Press Start (bit 3) and Cross (bit 14)
    pad->updateButtons(~(0x0008 | 0x4000));

    // Navigate to button data
    pad->tx(0x01);
    pad->rx();
    pad->tx(0x42);
    pad->rx();
    pad->rx(); // IDHI

    uint16_t button_lo = pad->rx();
    uint16_t button_hi = pad->rx();

    EXPECT_EQ(button_lo & 0x08, 0); // Start pressed
    EXPECT_EQ(button_hi & 0x40, 0); // Cross pressed
}

// Test all buttons released (default state)
TEST_F(DigitalPadTest, AllButtonsReleased) {
    pad->connect();
    pad->updateButtons(0xFFFF); // All released

    // Navigate to button data
    pad->tx(0x01);
    pad->rx();
    pad->tx(0x42);
    pad->rx();
    pad->rx(); // IDHI

    uint16_t button_lo = pad->rx();
    uint16_t button_hi = pad->rx();

    EXPECT_EQ(button_lo, 0xFF);
    EXPECT_EQ(button_hi, 0xFF);
}

// Test reset functionality
TEST_F(DigitalPadTest, ResetFunctionality) {
    pad->connect();

    // Navigate to middle of sequence
    pad->tx(0x01);
    pad->rx();
    pad->tx(0x42);
    pad->rx();

    // Reset should return to HighZ state
    pad->reset();

    // Next transmission should start from beginning
    pad->tx(0x01);
    EXPECT_EQ(pad->rx(), 0xFFFF);
}

// Test incorrect sequence: missing address byte
TEST_F(DigitalPadTest, IncorrectSequence_MissingAddress) {
    pad->connect();

    // Try to send command byte without address byte
    pad->tx(0x42);
    // Should not transition to IDLO state
    EXPECT_NE(pad->rx(), 0x41);
}

// Test tx() stores value correctly
TEST_F(DigitalPadTest, TxStoresValue) {
    pad->connect();

    // tx() should store the value
    pad->tx(0x01);
    pad->rx();
    pad->tx(0x42);

    // State machine should have recognized the sequence
    uint16_t id_lo = pad->rx();
    EXPECT_EQ(id_lo, 0x41);
}
