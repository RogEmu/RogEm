#include <gtest/gtest.h>
#include <memory>

#include "Core/InterruptController.hpp"
#include "Core/Bus.hpp"
#include "Core/CPU.hpp"

// Test fixture for InterruptController tests
class InterruptControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        bus = std::make_unique<Bus>();
        cpu = std::make_unique<CPU>(bus.get());
        bus->connectCpu(cpu.get());
        irqController = std::make_unique<InterruptController>(bus.get());
    }

    std::unique_ptr<Bus> bus;
    std::unique_ptr<CPU> cpu;
    std::unique_ptr<InterruptController> irqController;

    // Interrupt controller register addresses
    static constexpr uint32_t ISTAT_ADDR = 0x1F801070;
    static constexpr uint32_t IMASK_ADDR = 0x1F801074;
};

// Test ISTAT register read/write

TEST_F(InterruptControllerTest, ReadISTATInitialValue)
{
    // ISTAT should be 0 initially
    uint32_t result = irqController->read32(ISTAT_ADDR);
    EXPECT_EQ(0, result);
}

TEST_F(InterruptControllerTest, WriteAndReadISTAT32Bit)
{
    // Trigger some IRQs to set ISTAT
    irqController->triggerIRQ(DeviceIRQ::VBLANK);
    irqController->triggerIRQ(DeviceIRQ::GPU);

    uint32_t result = irqController->read32(ISTAT_ADDR);
    EXPECT_EQ(0x003, result);  // VBLANK | GPU

    // Acknowledge IRQs (write to clear)
    irqController->write32(~result, ISTAT_ADDR);
    result = irqController->read32(ISTAT_ADDR);
    EXPECT_EQ(0, result);
}

TEST_F(InterruptControllerTest, WriteAndReadISTAT16Bit)
{
    irqController->triggerIRQ(DeviceIRQ::CDROM);

    uint16_t result = irqController->read16(ISTAT_ADDR);
    EXPECT_EQ(0x004, result);

    // Acknowledge with 16-bit write
    irqController->write16(~result, ISTAT_ADDR);
    result = irqController->read16(ISTAT_ADDR);
    EXPECT_EQ(0, result);
}

TEST_F(InterruptControllerTest, WriteAndReadISTAT8Bit)
{
    irqController->triggerIRQ(DeviceIRQ::DMA);

    uint8_t result = irqController->read8(ISTAT_ADDR);
    EXPECT_EQ(0x08, result);

    // Acknowledge with 8-bit write
    irqController->write8(~result, ISTAT_ADDR);
    result = irqController->read8(ISTAT_ADDR);
    EXPECT_EQ(0, result);
}

TEST_F(InterruptControllerTest, ISTATMultipleBits)
{
    // Trigger multiple IRQs
    irqController->triggerIRQ(DeviceIRQ::TIMER0);
    irqController->triggerIRQ(DeviceIRQ::TIMER1);
    irqController->triggerIRQ(DeviceIRQ::TIMER2);

    uint32_t result = irqController->read32(ISTAT_ADDR);
    EXPECT_EQ(0x070, result);  // TIMER0 | TIMER1 | TIMER2
}

// Test IMASK register read/write

TEST_F(InterruptControllerTest, ReadIMASKInitialValue)
{
    // IMASK should be 0 initially
    uint32_t result = irqController->read32(IMASK_ADDR);
    EXPECT_EQ(0, result);
}

TEST_F(InterruptControllerTest, WriteAndReadIMASK32Bit)
{
    uint32_t testValue = 0x12345678;
    irqController->write32(testValue, IMASK_ADDR);

    uint32_t result = irqController->read32(IMASK_ADDR);
    EXPECT_EQ(testValue, result);
}

TEST_F(InterruptControllerTest, WriteAndReadIMASK16Bit)
{
    uint16_t testValue = 0xABCD;
    irqController->write16(testValue, IMASK_ADDR);

    uint16_t result = irqController->read16(IMASK_ADDR);
    EXPECT_EQ(testValue, result);
}

TEST_F(InterruptControllerTest, WriteAndReadIMASK8Bit)
{
    uint8_t testValue = 0xFF;
    irqController->write8(testValue, IMASK_ADDR);

    uint8_t result = irqController->read8(IMASK_ADDR);
    EXPECT_EQ(testValue, result);
}

TEST_F(InterruptControllerTest, IMASKMultipleWrites)
{
    irqController->write32(0x00000001, IMASK_ADDR);
    EXPECT_EQ(0x00000001, irqController->read32(IMASK_ADDR));

    irqController->write32(0x00000080, IMASK_ADDR);
    EXPECT_EQ(0x00000080, irqController->read32(IMASK_ADDR));

    irqController->write32(0xFFFFFFFF, IMASK_ADDR);
    EXPECT_EQ(0xFFFFFFFF, irqController->read32(IMASK_ADDR));
}

// Test triggering IRQs

TEST_F(InterruptControllerTest, TriggerSingleIRQ)
{
    irqController->triggerIRQ(DeviceIRQ::VBLANK);

    uint32_t istat = irqController->read32(ISTAT_ADDR);
    EXPECT_EQ(0x001, istat);
}

TEST_F(InterruptControllerTest, TriggerMultipleIRQs)
{
    irqController->triggerIRQ(DeviceIRQ::GPU);
    irqController->triggerIRQ(DeviceIRQ::CDROM);
    irqController->triggerIRQ(DeviceIRQ::DMA);

    uint32_t istat = irqController->read32(ISTAT_ADDR);
    EXPECT_EQ(0x00E, istat);  // GPU | CDROM | DMA
}

TEST_F(InterruptControllerTest, TriggerAllIRQs)
{
    irqController->triggerIRQ(DeviceIRQ::VBLANK);
    irqController->triggerIRQ(DeviceIRQ::GPU);
    irqController->triggerIRQ(DeviceIRQ::CDROM);
    irqController->triggerIRQ(DeviceIRQ::DMA);
    irqController->triggerIRQ(DeviceIRQ::TIMER0);
    irqController->triggerIRQ(DeviceIRQ::TIMER1);
    irqController->triggerIRQ(DeviceIRQ::TIMER2);
    irqController->triggerIRQ(DeviceIRQ::CONTROLLER_MEMCARD);
    irqController->triggerIRQ(DeviceIRQ::SIO);
    irqController->triggerIRQ(DeviceIRQ::SPU);
    irqController->triggerIRQ(DeviceIRQ::LIGHTPEN);

    uint32_t istat = irqController->read32(ISTAT_ADDR);
    EXPECT_EQ(0x7FF, istat);  // All 11 IRQ bits set
}

// Test COP0 interaction - IRQ updates CAUSE register

TEST_F(InterruptControllerTest, COP0UpdateWhenIRQEnabledAndTriggered)
{
    // Enable VBLANK interrupt in mask
    irqController->write32(0x001, IMASK_ADDR);

    // Check initial CAUSE register state (bit 10 should be 0)
    uint32_t cause = cpu->getCop0Reg(13);  // CAUSE is register 13
    EXPECT_EQ(0, cause & 0x400);

    // Trigger VBLANK IRQ
    irqController->triggerIRQ(DeviceIRQ::VBLANK);

    // Check that CAUSE register bit 10 is set
    cause = cpu->getCop0Reg(13);
    EXPECT_NE(0, cause & 0x400);
}

TEST_F(InterruptControllerTest, COP0NotUpdatedWhenIRQDisabled)
{
    // Don't enable any interrupts in mask (IMASK = 0)
    irqController->write32(0x000, IMASK_ADDR);

    // Check initial CAUSE register state
    uint32_t cause = cpu->getCop0Reg(13);
    EXPECT_EQ(0, cause & 0x400);

    // Trigger VBLANK IRQ
    irqController->triggerIRQ(DeviceIRQ::VBLANK);

    // Check that CAUSE register bit 10 is still 0 (not set)
    cause = cpu->getCop0Reg(13);
    EXPECT_EQ(0, cause & 0x400);
}

TEST_F(InterruptControllerTest, COP0UpdateWithMultipleIRQsEnabled)
{
    // Enable multiple interrupts in mask
    irqController->write32(0x00F, IMASK_ADDR);  // VBLANK, GPU, CDROM, DMA

    // Trigger one of the enabled IRQs
    irqController->triggerIRQ(DeviceIRQ::GPU);

    // Check that CAUSE register bit 10 is set
    uint32_t cause = cpu->getCop0Reg(13);
    EXPECT_NE(0, cause & 0x400);
}

TEST_F(InterruptControllerTest, COP0ClearedWhenIRQAcknowledged)
{
    // Enable CDROM interrupt
    irqController->write32(0x004, IMASK_ADDR);

    // Trigger CDROM IRQ
    irqController->triggerIRQ(DeviceIRQ::CDROM);

    // Verify CAUSE bit is set
    uint32_t cause = cpu->getCop0Reg(13);
    EXPECT_NE(0, cause & 0x400);

    // Acknowledge the IRQ
    uint32_t istat = irqController->read32(ISTAT_ADDR);
    irqController->write32(~istat, ISTAT_ADDR);

    // Verify CAUSE bit is cleared
    cause = cpu->getCop0Reg(13);
    EXPECT_EQ(0, cause & 0x400);
}

TEST_F(InterruptControllerTest, COP0StateWithPartialMask)
{
    // Enable only TIMER0 and TIMER2
    irqController->write32(0x050, IMASK_ADDR);

    // Trigger TIMER1 (not enabled in mask)
    irqController->triggerIRQ(DeviceIRQ::TIMER1);

    // CAUSE bit should not be set
    uint32_t cause = cpu->getCop0Reg(13);
    EXPECT_EQ(0, cause & 0x400);

    // Trigger TIMER0 (enabled in mask)
    irqController->triggerIRQ(DeviceIRQ::TIMER0);

    // Now CAUSE bit should be set
    cause = cpu->getCop0Reg(13);
    EXPECT_NE(0, cause & 0x400);
}

TEST_F(InterruptControllerTest, COP0UpdateWhenMaskChanges)
{
    // Trigger an IRQ without mask enabled
    irqController->triggerIRQ(DeviceIRQ::SPU);

    // CAUSE bit should not be set
    uint32_t cause = cpu->getCop0Reg(13);
    EXPECT_EQ(0, cause & 0x400);

    // Now enable the SPU interrupt in mask
    irqController->write32(0x200, IMASK_ADDR);

    // CAUSE bit should now be set (because ISTAT still has SPU bit)
    cause = cpu->getCop0Reg(13);
    EXPECT_NE(0, cause & 0x400);
}

// Test reset functionality

TEST_F(InterruptControllerTest, ResetClearsRegisters)
{
    // Set some values
    irqController->write32(0xFFFFFFFF, IMASK_ADDR);
    irqController->triggerIRQ(DeviceIRQ::VBLANK);
    irqController->triggerIRQ(DeviceIRQ::GPU);

    // Verify they're set
    EXPECT_NE(0, irqController->read32(ISTAT_ADDR));
    EXPECT_NE(0, irqController->read32(IMASK_ADDR));

    // Reset
    irqController->reset();

    // Verify everything is cleared
    EXPECT_EQ(0, irqController->read32(ISTAT_ADDR));
    EXPECT_EQ(0, irqController->read32(IMASK_ADDR));
}

// Test edge cases

TEST_F(InterruptControllerTest, MultipleTriggersOfSameIRQ)
{
    // Trigger the same IRQ multiple times
    irqController->triggerIRQ(DeviceIRQ::DMA);
    irqController->triggerIRQ(DeviceIRQ::DMA);
    irqController->triggerIRQ(DeviceIRQ::DMA);

    // ISTAT should still show only one bit set
    uint32_t istat = irqController->read32(ISTAT_ADDR);
    EXPECT_EQ(0x008, istat);
}

TEST_F(InterruptControllerTest, PartialAcknowledgement)
{
    // Trigger multiple IRQs
    irqController->triggerIRQ(DeviceIRQ::VBLANK);
    irqController->triggerIRQ(DeviceIRQ::GPU);
    irqController->triggerIRQ(DeviceIRQ::CDROM);

    // Acknowledge only VBLANK
    irqController->write32(~0x001, ISTAT_ADDR);

    // ISTAT should still have GPU and CDROM
    uint32_t istat = irqController->read32(ISTAT_ADDR);
    EXPECT_EQ(0x006, istat);
}

TEST_F(InterruptControllerTest, ISTATReadDifferentOffsets)
{
    irqController->triggerIRQ(DeviceIRQ::CONTROLLER_MEMCARD);  // 0x80

    // Read as 32-bit
    EXPECT_EQ(0x80, irqController->read32(ISTAT_ADDR));

    // Read as 16-bit
    EXPECT_EQ(0x80, irqController->read16(ISTAT_ADDR));

    // Read as 8-bit
    EXPECT_EQ(0x80, irqController->read8(ISTAT_ADDR));
}

TEST_F(InterruptControllerTest, IMASKReadDifferentOffsets)
{
    irqController->write32(0x12345678, IMASK_ADDR);

    // Read different bytes
    EXPECT_EQ(0x78, irqController->read8(IMASK_ADDR));
    EXPECT_EQ(0x56, irqController->read8(IMASK_ADDR + 1));
    EXPECT_EQ(0x34, irqController->read8(IMASK_ADDR + 2));
    EXPECT_EQ(0x12, irqController->read8(IMASK_ADDR + 3));

    // Read halfwords
    EXPECT_EQ(0x5678, irqController->read16(IMASK_ADDR));
    EXPECT_EQ(0x1234, irqController->read16(IMASK_ADDR + 2));
}
