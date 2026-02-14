#include <gtest/gtest.h>
#include "Core/Timers.hpp"
#include "Core/Bus.hpp"
#include "Core/CPU.hpp"
#include "Core/InterruptController.hpp"

class TimersTest : public ::testing::Test {
protected:
    void SetUp() override {
        bus = std::make_unique<Bus>();
        cpu = std::make_unique<CPU>(bus.get());
        irqc = bus->getDevice<InterruptController>();
        timers = bus->getDevice<Timers>();
    }

    std::unique_ptr<Bus> bus;
    std::unique_ptr<CPU> cpu;
    InterruptController *irqc;
    Timers *timers;
};

TEST_F(TimersTest, UnhandledREadsWrites)
{
    timers->write8(0xFF, 0x1F801100);
    timers->read8(0x1F801100);
}

// Test reading/writing timer 0 current value
TEST_F(TimersTest, Timer0_CurrentValue_ReadWrite) {
    uint32_t timer0_current = 0x1F801100;

    // Write value
    timers->write16(0x1234, timer0_current);

    // Read back
    EXPECT_EQ(timers->read16(timer0_current), 0x1234);
}

// Test reading/writing timer 0 mode
TEST_F(TimersTest, Timer0_Mode_ReadWrite) {
    uint32_t timer0_mode = 0x1F801104;

    // Write mode value
    timers->write16(0x0123, timer0_mode);

    // Read back
    EXPECT_EQ(timers->read16(timer0_mode), 0x0123);
}

// Test reading/writing timer 0 target value
TEST_F(TimersTest, Timer0_Target_ReadWrite) {
    uint32_t timer0_target = 0x1F801108;

    // Write target value
    timers->write16(0x5678, timer0_target);

    // Read back
    EXPECT_EQ(timers->read16(timer0_target), 0x5678);
}

// Test reading/writing timer 1 registers
TEST_F(TimersTest, Timer1_Registers_ReadWrite) {
    uint32_t timer1_current = 0x1F801110;
    uint32_t timer1_mode = 0x1F801114;
    uint32_t timer1_target = 0x1F801118;

    timers->write16(0xAAAA, timer1_current);
    timers->write16(0xBBBB, timer1_mode);
    timers->write16(0xCCCC, timer1_target);

    EXPECT_EQ(timers->read16(timer1_current), 0xAAAA);
    EXPECT_EQ(timers->read16(timer1_mode), 0xBBBB);
    EXPECT_EQ(timers->read16(timer1_target), 0xCCCC);
}

// Test reading/writing timer 2 registers
TEST_F(TimersTest, Timer2_Registers_ReadWrite) {
    uint32_t timer2_current = 0x1F801120;
    uint32_t timer2_mode = 0x1F801124;
    uint32_t timer2_target = 0x1F801128;

    timers->write16(0x1111, timer2_current);
    timers->write16(0x2222, timer2_mode);
    timers->write16(0x3333, timer2_target);

    EXPECT_EQ(timers->read16(timer2_current), 0x1111);
    EXPECT_EQ(timers->read16(timer2_mode), 0x2222);
    EXPECT_EQ(timers->read16(timer2_target), 0x3333);
}

// Test timer counting with system clock
TEST_F(TimersTest, Timer0_SystemClock_Counting) {
    uint32_t timer0_current = 0x1F801100;
    uint32_t timer0_mode = 0x1F801104;

    // Set mode: no sync, system clock
    timers->write16(0x0000, timer0_mode);
    timers->write16(0x0000, timer0_current);

    // Update with 100 cycles
    timers->update(100);

    // Timer should increment by 100
    EXPECT_EQ(timers->read16(timer0_current), 100);
}

// Test timer reaching target value
TEST_F(TimersTest, Timer0_ReachTarget) {
    uint32_t timer0_current = 0x1F801100;
    uint32_t timer0_mode = 0x1F801104;
    uint32_t timer0_target = 0x1F801108;

    // Set target to 1000
    timers->write16(1000, timer0_target);
    timers->write16(0, timer0_current);

    // Mode: resetCounter on target (bit 3)
    timers->write16(0x0008, timer0_mode);

    // Update to reach target
    timers->update(1000);

    // Timer should reset to 0
    EXPECT_EQ(timers->read16(timer0_current), 0);

    // Mode should have reachedTarget flag set (bit 11)
    uint16_t mode = timers->read16(timer0_mode);
    EXPECT_NE(mode & 0x0800, 0);
}

// Test timer reaching max value (0xFFFF)
TEST_F(TimersTest, Timer0_ReachMax) {
    uint32_t timer0_current = 0x1F801100;
    uint32_t timer0_mode = 0x1F801104;

    // Start near max
    timers->write16(0xFFF0, timer0_current);
    timers->write16(0x0000, timer0_mode);

    // Update to exceed max
    timers->update(20);

    // Timer should wrap to 0
    EXPECT_LT(timers->read16(timer0_current), 20);

    // Mode should have reachedMax flag set (bit 12)
    uint16_t mode = timers->read16(timer0_mode);
    EXPECT_NE(mode & 0x1000, 0);
}

// Test IRQ on target reach
TEST_F(TimersTest, Timer0_IRQ_OnTarget) {
    uint32_t timer0_current = 0x1F801100;
    uint32_t timer0_mode = 0x1F801104;
    uint32_t timer0_target = 0x1F801108;

    // Set target
    timers->write16(500, timer0_target);
    timers->write16(0, timer0_current);

    // Mode: IRQ on target (bit 4), reset on target (bit 3)
    timers->write16(0x0018, timer0_mode);

    // Update to reach target
    timers->update(500);

    // IRQ flag should be set (bit 10)
    uint16_t mode = timers->read16(timer0_mode);
    EXPECT_NE(mode & 0x0400, 0);
}

// Test IRQ on max reach
TEST_F(TimersTest, Timer0_IRQ_OnMax) {
    uint32_t timer0_current = 0x1F801100;
    uint32_t timer0_mode = 0x1F801104;

    // Start near max
    timers->write16(0xFFF0, timer0_current);

    // Mode: IRQ on max (bit 5)
    timers->write16(0x0020, timer0_mode);

    // Update to reach max
    timers->update(20);

    // IRQ flag should be set (bit 10)
    uint16_t mode = timers->read16(timer0_mode);
    EXPECT_NE(mode & 0x0400, 0);
}

// Test IRQ once mode - only first trigger
TEST_F(TimersTest, Timer0_IRQ_Once) {
    uint32_t timer0_current = 0x1F801100;
    uint32_t timer0_mode = 0x1F801104;
    uint32_t timer0_target = 0x1F801108;

    // Set target to 100
    timers->write16(100, timer0_target);
    timers->write16(0, timer0_current);

    // Mode: IRQ once (bit 6), IRQ on target (bit 4), reset on target (bit 3)
    timers->write16(0x0058, timer0_mode);

    // First reach
    timers->update(100);
    uint16_t mode_after_first = timers->read16(timer0_mode);
    EXPECT_NE(mode_after_first & 0x0400, 0); // IRQ flag set

    // Clear IRQ flag manually for next test
    timers->write16(mode_after_first & ~0x0400, timer0_mode);

    // Second reach - IRQ should not trigger again
    timers->update(100);
    uint16_t mode_after_second = timers->read16(timer0_mode);
    // IRQ flag should stay clear because irqTarget was disabled
    EXPECT_EQ(mode_after_second & 0x0400, 0);
}

// Test timer 1 HBlank clock source
TEST_F(TimersTest, Timer1_HBlank_ClockSource) {
    uint32_t timer1_current = 0x1F801110;
    uint32_t timer1_mode = 0x1F801114;

    // Mode: clock source = HBlank (bit 8)
    timers->write16(0x0100, timer1_mode);
    timers->write16(0, timer1_current);

    // Normal update should not increment
    timers->update(100);
    EXPECT_EQ(timers->read16(timer1_current), 0);

    // HBlank should increment
    timers->onHBlank();
    EXPECT_EQ(timers->read16(timer1_current), 1);

    timers->onHBlank();
    EXPECT_EQ(timers->read16(timer1_current), 2);
}

// Test timer 2 system clock div 8
TEST_F(TimersTest, Timer2_SystemClock_Div8) {
    uint32_t timer2_current = 0x1F801120;
    uint32_t timer2_mode = 0x1F801124;

    // Mode: clock source = system clock / 8 (bit 9)
    timers->write16(0x0200, timer2_mode);
    timers->write16(0, timer2_current);

    // Update with 80 cycles
    timers->update(80);

    // Timer should increment by 10 (80/8)
    EXPECT_EQ(timers->read16(timer2_current), 10);
}

// Test timer 0 HBlank sync mode 0 (pause during HBlank)
TEST_F(TimersTest, Timer0_HBlank_Sync_Mode0) {
    uint32_t timer0_current = 0x1F801100;
    uint32_t timer0_mode = 0x1F801104;

    // Mode: sync enable (bit 0), sync mode = 0 (bits 1-2)
    timers->write16(0x0001, timer0_mode);
    timers->write16(0, timer0_current);

    // Update - should count normally
    timers->update(50);
    EXPECT_EQ(timers->read16(timer0_current), 50);

    // HBlank - should pause
    timers->onHBlank();
    timers->update(50);
    EXPECT_EQ(timers->read16(timer0_current), 50); // Still 50, paused

    // HBlankEnd - should resume
    timers->onHBlankEnd();
    timers->update(50);
    EXPECT_EQ(timers->read16(timer0_current), 100);
}

// Test timer 0 HBlank sync mode 1 (reset on HBlank)
TEST_F(TimersTest, Timer0_HBlank_Sync_Mode1) {
    uint32_t timer0_current = 0x1F801100;
    uint32_t timer0_mode = 0x1F801104;

    // Mode: sync enable (bit 0), sync mode = 1 (bits 1-2)
    timers->write16(0x0003, timer0_mode);
    timers->write16(100, timer0_current);

    // HBlank - should reset counter
    timers->onHBlank();
    EXPECT_EQ(timers->read16(timer0_current), 0);
}

// Test timer 0 HBlank sync mode 2 (reset and pause outside HBlank)
TEST_F(TimersTest, Timer0_HBlank_Sync_Mode2) {
    uint32_t timer0_current = 0x1F801100;
    uint32_t timer0_mode = 0x1F801104;

    // Mode: sync enable (bit 0), sync mode = 2 (bits 1-2)
    timers->write16(0x0005, timer0_mode);
    timers->write16(100, timer0_current);

    // HBlank - should reset and unpause
    timers->onHBlank();
    EXPECT_EQ(timers->read16(timer0_current), 0);

    // Should count during HBlank
    timers->update(50);
    EXPECT_EQ(timers->read16(timer0_current), 50);

    // HBlankEnd - should pause
    timers->onHBlankEnd();
    timers->update(50);
    EXPECT_EQ(timers->read16(timer0_current), 50); // Still 50, paused
}

// Test timer 0 HBlank sync mode 3 (wait for HBlank then free run)
TEST_F(TimersTest, Timer0_HBlank_Sync_Mode3) {
    uint32_t timer0_current = 0x1F801100;
    uint32_t timer0_mode = 0x1F801104;

    // Mode: sync enable (bit 0), sync mode = 3 (bits 1-2)
    timers->write16(0x0007, timer0_mode);
    timers->write16(0, timer0_current);

    // Should be paused initially (waiting for HBlank)
    timers->update(50);
    EXPECT_EQ(timers->read16(timer0_current), 0);

    // HBlank - should unpause and disable sync
    timers->onHBlank();

    // Should now count freely
    timers->update(50);
    EXPECT_EQ(timers->read16(timer0_current), 50);

    // Additional HBlanks should have no effect
    timers->onHBlank();
    timers->update(50);
    EXPECT_EQ(timers->read16(timer0_current), 100);
}

// Test timer 1 VBlank sync mode 0 (pause during VBlank)
TEST_F(TimersTest, Timer1_VBlank_Sync_Mode0) {
    uint32_t timer1_current = 0x1F801110;
    uint32_t timer1_mode = 0x1F801114;

    // Mode: sync enable (bit 0), sync mode = 0 (bits 1-2)
    timers->write16(0x0001, timer1_mode);
    timers->write16(0, timer1_current);

    // Update - should count normally
    timers->update(50);
    EXPECT_EQ(timers->read16(timer1_current), 50);

    // VBlank - should pause
    timers->onVBlank();
    timers->update(50);
    EXPECT_EQ(timers->read16(timer1_current), 50); // Still 50, paused

    // VBlankEnd - should resume
    timers->onVBlankEnd();
    timers->update(50);
    EXPECT_EQ(timers->read16(timer1_current), 100);
}

// Test timer 1 VBlank sync mode 1 (reset on VBlank)
TEST_F(TimersTest, Timer1_VBlank_Sync_Mode1) {
    uint32_t timer1_current = 0x1F801110;
    uint32_t timer1_mode = 0x1F801114;

    // Mode: sync enable (bit 0), sync mode = 1 (bits 1-2)
    timers->write16(0x0003, timer1_mode);
    timers->write16(200, timer1_current);

    // VBlank - should reset counter
    timers->onVBlank();
    EXPECT_EQ(timers->read16(timer1_current), 0);
}

// Test timer 1 VBlank sync mode 2 (reset and pause outside VBlank)
TEST_F(TimersTest, Timer1_VBlank_Sync_Mode2) {
    uint32_t timer1_current = 0x1F801110;
    uint32_t timer1_mode = 0x1F801114;

    // Mode: sync enable (bit 0), sync mode = 2 (bits 1-2)
    timers->write16(0x0005, timer1_mode);
    timers->write16(200, timer1_current);

    // VBlank - should reset and unpause
    timers->onVBlank();
    EXPECT_EQ(timers->read16(timer1_current), 0);

    // Should count during VBlank
    timers->update(50);
    EXPECT_EQ(timers->read16(timer1_current), 50);

    // VBlankEnd - should pause
    timers->onVBlankEnd();
    timers->update(50);
    EXPECT_EQ(timers->read16(timer1_current), 50); // Still 50, paused
}

// Test timer 1 VBlank sync mode 3 (wait for VBlank then free run)
TEST_F(TimersTest, Timer1_VBlank_Sync_Mode3) {
    uint32_t timer1_current = 0x1F801110;
    uint32_t timer1_mode = 0x1F801114;

    // Mode: sync enable (bit 0), sync mode = 3 (bits 1-2)
    timers->write16(0x0007, timer1_mode);
    timers->write16(0, timer1_current);

    // Should be paused initially (waiting for VBlank)
    timers->update(50);
    EXPECT_EQ(timers->read16(timer1_current), 0);

    // VBlank - should unpause and disable sync
    timers->onVBlank();

    // Should now count freely
    timers->update(50);
    EXPECT_EQ(timers->read16(timer1_current), 50);

    // Additional VBlanks should have no effect
    timers->onVBlank();
    timers->update(50);
    EXPECT_EQ(timers->read16(timer1_current), 100);
}

// Test 32-bit write/read
TEST_F(TimersTest, Timer0_32bit_Write_Read) {
    uint32_t timer0_current = 0x1F801100;

    // Write 32-bit value (only lower 16 bits should be stored)
    timers->write32(0xABCD1234, timer0_current);

    // Read back as 32-bit
    uint32_t value = timers->read32(timer0_current);
    EXPECT_EQ(value & 0xFFFF, 0x1234);
}

// Test all three timers running simultaneously
TEST_F(TimersTest, AllTimers_Simultaneous) {
    uint32_t timer0_current = 0x1F801100;
    uint32_t timer1_current = 0x1F801110;
    uint32_t timer2_current = 0x1F801120;

    // Initialize all timers
    timers->write16(0, timer0_current);
    timers->write16(0, timer1_current);
    timers->write16(0, timer2_current);

    // Set modes - all system clock, no div
    timers->write16(0x0000, 0x1F801104);
    timers->write16(0x0000, 0x1F801114);
    timers->write16(0x0000, 0x1F801124);

    // Update all
    timers->update(100);

    // All should increment
    EXPECT_EQ(timers->read16(timer0_current), 100);
    EXPECT_EQ(timers->read16(timer1_current), 100);
    EXPECT_EQ(timers->read16(timer2_current), 100);
}

// Test timer with no reset on target (wraps at 0xFFFF)
TEST_F(TimersTest, Timer0_NoReset_OnTarget) {
    uint32_t timer0_current = 0x1F801100;
    uint32_t timer0_mode = 0x1F801104;
    uint32_t timer0_target = 0x1F801108;

    // Set target to 100
    timers->write16(100, timer0_target);
    timers->write16(0, timer0_current);

    // Mode: IRQ on target (bit 4), NO reset on target (bit 3 = 0)
    timers->write16(0x0010, timer0_mode);

    // Update to pass target
    timers->update(200);

    // Timer should be at 200 (not reset)
    EXPECT_EQ(timers->read16(timer0_current), 200);

    // ReachedTarget flag should still be set
    uint16_t mode = timers->read16(timer0_mode);
    EXPECT_NE(mode & 0x0800, 0);
}
