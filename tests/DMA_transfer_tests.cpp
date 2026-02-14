#include <gtest/gtest.h>
#include "Core/DMA.hpp"
#include "Core/Bus.hpp"
#include "Core/RAM.hpp"
#include "Core/GPU.hpp"
#include <memory>

class DMATransferTest : public ::testing::Test {
protected:
    void SetUp() override {
        bus = std::make_unique<Bus>();
        ram = bus->getDevice<RAM>();
        gpu = bus->getDevice<GPU>();
        dma = bus->getDevice<DMA>();
    }

    std::unique_ptr<Bus> bus;
    RAM *ram;
    GPU *gpu;
    DMA *dma;
};

// Test GPU DMA Request mode (Slice) - RAM to GPU transfer
TEST_F(DMATransferTest, GPU_DMA_Request_RamToGpu_SmallTransfer) {
    // GPU channel base addresses
    uint32_t gpu_madr = 0x1F8010A0; // Memory Address
    uint32_t gpu_bcr = 0x1F8010A4;  // Block Control
    uint32_t gpu_chcr = 0x1F8010A8; // Channel Control

    // Prepare real GPU commands in RAM at address 0x80000000
    uint32_t ram_addr = 0x80000000;
    bus->storeWord(ram_addr + 0, 0xE1000000);  // Draw Mode setting
    bus->storeWord(ram_addr + 4, 0xE3000000);  // Set Drawing Area top left (0,0)
    bus->storeWord(ram_addr + 8, 0xE4000000);  // Set Drawing Area bottom right
    bus->storeWord(ram_addr + 12, 0xE5000000); // Set Drawing Offset (0,0)

    // Set DMA registers for GPU channel
    // Memory Address - point to RAM data
    dma->write32(ram_addr, gpu_madr);
    EXPECT_EQ(dma->read32(gpu_madr) & 0xFFFFFF, ram_addr & 0xFFFFFF);

    // Block Control - 4 words in 1 block (size=4, amount=1)
    // Lower 16 bits = block size, upper 16 bits = block amount
    dma->write32(0x00010004, gpu_bcr);
    EXPECT_EQ(dma->read32(gpu_bcr), 0x00010004);

    // Channel Control - start transfer
    // Bit 0: Transfer direction (1 = RAM to Device)
    // Bit 1: Memory address step (0 = increment)
    // Bits 9-10: Sync mode (01b = Request/Slice)
    // Bit 24: Start/Busy (1 = start)
    uint32_t chcr = 0x01000201; // RAM to device, increment, request mode, start
    dma->write32(chcr, gpu_chcr);

    // After transfer, channel should be stopped
    uint32_t chcr_after = dma->read32(gpu_chcr);
    EXPECT_EQ(chcr_after & 0x01000000, 0); // Start bit cleared
}

// Test GPU DMA Request mode with larger transfer
TEST_F(DMATransferTest, GPU_DMA_Request_MultipleBlocks) {
    uint32_t gpu_madr = 0x1F8010A0;
    uint32_t gpu_bcr = 0x1F8010A4;
    uint32_t gpu_chcr = 0x1F8010A8;

    // Prepare real GPU commands in RAM (16 words)
    uint32_t ram_addr = 0x80001000;
    // Setup commands
    bus->storeWord(ram_addr + 0, 0xE1000000);   // Draw Mode
    bus->storeWord(ram_addr + 4, 0xE3000000);   // Drawing Area top left
    bus->storeWord(ram_addr + 8, 0xE4000000);   // Drawing Area bottom right
    bus->storeWord(ram_addr + 12, 0xE5000000);  // Drawing Offset
    // Fill rectangle command (requires 3 words)
    bus->storeWord(ram_addr + 16, 0x02FF0000);  // Fill Rectangle (red)
    bus->storeWord(ram_addr + 20, 0x00000000);  // Top-left (0,0)
    bus->storeWord(ram_addr + 24, 0x00400040);  // Size (64x64)
    // More setup commands
    bus->storeWord(ram_addr + 28, 0xE6000000);  // Mask Bit Setting
    bus->storeWord(ram_addr + 32, 0x00000000);  // NOP
    bus->storeWord(ram_addr + 36, 0x00000000);  // NOP
    bus->storeWord(ram_addr + 40, 0x00000000);  // NOP
    bus->storeWord(ram_addr + 44, 0x00000000);  // NOP
    bus->storeWord(ram_addr + 48, 0x00000000);  // NOP
    bus->storeWord(ram_addr + 52, 0x00000000);  // NOP
    bus->storeWord(ram_addr + 56, 0x00000000);  // NOP
    bus->storeWord(ram_addr + 60, 0x00000000);  // NOP

    // Set DMA registers
    dma->write32(ram_addr, gpu_madr);

    // 16 words total: 4 blocks of 4 words each
    dma->write32(0x00040004, gpu_bcr);

    // Start transfer
    dma->write32(0x01000201, gpu_chcr);

    // Verify transfer completed
    uint32_t chcr_after = dma->read32(gpu_chcr);
    EXPECT_EQ(chcr_after & 0x01000000, 0);
}

// Test GPU DMA with decrement step
TEST_F(DMATransferTest, GPU_DMA_Request_DecrementStep) {
    uint32_t gpu_madr = 0x1F8010A0;
    uint32_t gpu_bcr = 0x1F8010A4;
    uint32_t gpu_chcr = 0x1F8010A8;

    // Prepare real GPU commands in RAM (for decrement transfer)
    uint32_t ram_addr = 0x80002000;
    bus->storeWord(ram_addr - 12, 0xE3000000);  // Drawing Area top left
    bus->storeWord(ram_addr - 8, 0xE4000000);   // Drawing Area bottom right
    bus->storeWord(ram_addr - 4, 0xE5000000);   // Drawing Offset
    bus->storeWord(ram_addr, 0xE1000000);       // Draw Mode

    // Set DMA registers
    dma->write32(ram_addr, gpu_madr);
    dma->write32(0x00010004, gpu_bcr); // 4 words in 1 block

    // Channel Control with decrement step (bit 1 = 1)
    uint32_t chcr = 0x01000203; // RAM to device, decrement, request mode
    dma->write32(chcr, gpu_chcr);

    // Verify transfer completed
    EXPECT_EQ(dma->read32(gpu_chcr) & 0x01000000, 0);
}

// Test GPU DMA LinkedList mode
TEST_F(DMATransferTest, GPU_DMA_LinkedList_SinglePacket) {
    uint32_t gpu_madr = 0x1F8010A0;
    uint32_t gpu_bcr = 0x1F8010A4;
    uint32_t gpu_chcr = 0x1F8010A8;

    // Create a simple linked list packet in RAM
    uint32_t ram_addr = 0x80003000;

    // Packet header: size in top byte (3 words), next pointer in lower 24 bits
    // 0x03FFFFFF = 3 words, end marker
    bus->storeWord(ram_addr, 0x03FFFFFF);
    bus->storeWord(ram_addr + 4, 0xE1000000);  // Draw Mode command
    bus->storeWord(ram_addr + 8, 0xE3000000);  // Drawing Area top left
    bus->storeWord(ram_addr + 12, 0xE4000000); // Drawing Area bottom right

    // Set DMA registers
    dma->write32(ram_addr, gpu_madr);
    dma->write32(0, gpu_bcr); // Not used in linked list mode

    // Channel Control for LinkedList mode (syncMode = 10b)
    uint32_t chcr = 0x01000401; // RAM to device, increment, linked list mode
    dma->write32(chcr, gpu_chcr);

    // Verify transfer completed
    EXPECT_EQ(dma->read32(gpu_chcr) & 0x01000000, 0);
}

// Test GPU DMA LinkedList mode with multiple packets
TEST_F(DMATransferTest, GPU_DMA_LinkedList_MultiplePackets) {
    uint32_t gpu_madr = 0x1F8010A0;
    uint32_t gpu_chcr = 0x1F8010A8;

    uint32_t ram_addr = 0x80004000;

    // First packet: 2 words, points to second packet
    bus->storeWord(ram_addr, 0x02000000 | (ram_addr + 0x100));
    bus->storeWord(ram_addr + 4, 0xE1000000);  // Draw Mode
    bus->storeWord(ram_addr + 8, 0xE3000000);  // Drawing Area top left

    // Second packet: 2 words, points to third packet
    bus->storeWord(ram_addr + 0x100, 0x02000000 | (ram_addr + 0x200));
    bus->storeWord(ram_addr + 0x104, 0xE4000000);  // Drawing Area bottom right
    bus->storeWord(ram_addr + 0x108, 0xE5000000);  // Drawing Offset

    // Third packet: 1 word, end marker
    bus->storeWord(ram_addr + 0x200, 0x01FFFFFF);
    bus->storeWord(ram_addr + 0x204, 0x00000000);  // NOP
    dma->write32(ram_addr, gpu_madr);

    // Start linked list transfer
    dma->write32(0x01000401, gpu_chcr);

    // Verify transfer completed
    EXPECT_EQ(dma->read32(gpu_chcr) & 0x01000000, 0);
}

// Test DMA common registers (DPCR, DICR)
TEST_F(DMATransferTest, DMA_CommonRegisters) {
    uint32_t dpcr_addr = 0x1F8010F0;
    uint32_t dicr_addr = 0x1F8010F4;

    // Test DPCR (DMA Control Register)
    uint32_t dpcr_value = 0x07654321;
    dma->write32(dpcr_value, dpcr_addr);
    EXPECT_EQ(dma->read32(dpcr_addr), dpcr_value);

    // Test DICR (DMA Interrupt Control)
    uint32_t dicr_value = 0x80000000;
    dma->write32(dicr_value, dicr_addr);
    EXPECT_EQ(dma->read32(dicr_addr), dicr_value);
}

// Test DMA channel memory address register (24-bit mask)
TEST_F(DMATransferTest, DMA_MemoryAddress_24BitMask) {
    uint32_t gpu_madr = 0x1F8010A0;

    // Write full 32-bit value
    dma->write32(0xFFFFFFFF, gpu_madr);

    // Should only keep lower 24 bits
    uint32_t read_value = dma->read32(gpu_madr);
    EXPECT_EQ(read_value, 0x00FFFFFF);
}

// Test DMA block control register configuration
TEST_F(DMATransferTest, DMA_BlockControl_Configuration) {
    uint32_t gpu_bcr = 0x1F8010A4;

    // Set block control: 32 words per block, 4 blocks
    // Lower 16 bits = words per block, upper 16 bits = block count
    uint32_t bcr_value = 0x00040020;
    dma->write32(bcr_value, gpu_bcr);
    EXPECT_EQ(dma->read32(gpu_bcr), bcr_value);
}

// Test all DMA channels accessible
TEST_F(DMATransferTest, DMA_AllChannels_Accessible) {
    uint32_t base = 0x1F801080;

    // Test each channel's MADR register
    for (int i = 0; i < 7; i++) {
        uint32_t channel_base = base + (i << 4);
        uint32_t test_value = 0x00100000 + (i << 12);

        dma->write32(test_value, channel_base);
        EXPECT_EQ(dma->read32(channel_base) & 0xFFFFFF, test_value & 0xFFFFFF);
    }
}

// Test GPU DMA with single word transfer
TEST_F(DMATransferTest, GPU_DMA_Request_SingleWord) {
    uint32_t gpu_madr = 0x1F8010A0;
    uint32_t gpu_bcr = 0x1F8010A4;
    uint32_t gpu_chcr = 0x1F8010A8;

    // Prepare single GPU command in RAM
    uint32_t ram_addr = 0x80006000;
    bus->storeWord(ram_addr, 0xE1000000);  // Draw Mode command

    // Set DMA registers
    dma->write32(ram_addr, gpu_madr);
    dma->write32(0x00010001, gpu_bcr); // 1 word in 1 block
    dma->write32(0x01000201, gpu_chcr); // Start transfer

    // Verify completed
    EXPECT_EQ(dma->read32(gpu_chcr) & 0x01000000, 0);
}

// Test GPU DMA LinkedList with empty packet
TEST_F(DMATransferTest, GPU_DMA_LinkedList_EmptyPacket) {
    uint32_t gpu_madr = 0x1F8010A0;
    uint32_t gpu_chcr = 0x1F8010A8;

    uint32_t ram_addr = 0x80007000;

    // Empty packet (0 words) with end marker
    bus->storeWord(ram_addr, 0x00FFFFFF);

    // Set DMA registers
    dma->write32(ram_addr, gpu_madr);
    dma->write32(0x01000401, gpu_chcr); // Start linked list transfer

    // Should complete without error
    EXPECT_EQ(dma->read32(gpu_chcr) & 0x01000000, 0);
}

// Test DMA reset functionality
TEST_F(DMATransferTest, DMA_Reset) {
    // Write some values to DMA registers
    dma->write32(0x12345678, 0x1F8010F0); // DPCR
    dma->write32(0xABCDEF00, 0x1F8010F4); // DICR

    // Reset DMA
    dma->reset();

    // DPCR should reset to 0x07654321
    EXPECT_EQ(dma->read32(0x1F8010F0), 0x07654321);

    // DICR should reset to 0
    EXPECT_EQ(dma->read32(0x1F8010F4), 0);
}

// Test GPU DMA with maximum block size
TEST_F(DMATransferTest, GPU_DMA_Request_MaxBlockSize) {
    uint32_t gpu_madr = 0x1F8010A0;
    uint32_t gpu_bcr = 0x1F8010A4;
    uint32_t gpu_chcr = 0x1F8010A8;

    uint32_t ram_addr = 0x80008000;

    // Fill RAM with GPU commands (64 words)
    // Mix of setup commands and NOPs
    bus->storeWord(ram_addr + 0, 0xE1000000);   // Draw Mode
    bus->storeWord(ram_addr + 4, 0xE3000000);   // Drawing Area top left
    bus->storeWord(ram_addr + 8, 0xE4000000);   // Drawing Area bottom right
    bus->storeWord(ram_addr + 12, 0xE5000000);  // Drawing Offset
    bus->storeWord(ram_addr + 16, 0xE6000000);  // Mask Bit Setting
    // Fill remainder with NOPs
    for (int i = 5; i < 64; i++) {
        bus->storeWord(ram_addr + i * 4, 0x00000000);  // NOP
    }

    // Set DMA registers
    dma->write32(ram_addr, gpu_madr);
    dma->write32(0x00010040, gpu_bcr); // 64 words in 1 block
    dma->write32(0x01000201, gpu_chcr);

    // Verify completed
    EXPECT_EQ(dma->read32(gpu_chcr) & 0x01000000, 0);
}

// Test OTC with single entry
TEST_F(DMATransferTest, OTC_DMA_SingleEntry) {
    uint32_t otc_madr = 0x1F8010E0;
    uint32_t otc_bcr = 0x1F8010E4;
    uint32_t otc_chcr = 0x1F8010E8;

    uint32_t ot_addr = 0x80009000;

    // Initialize RAM area
    bus->storeWord(ot_addr, 0x00000000);

    // Set DMA registers for single entry
    dma->write32(ot_addr, otc_madr);
    dma->write32(0x00000001, otc_bcr); // 1 entry
    dma->write32(0x01000002, otc_chcr);

    // Verify completed
    EXPECT_EQ(dma->read32(otc_chcr) & 0x01000000, 0);

    // Single entry should have end marker
    EXPECT_EQ(bus->loadWord(ot_addr), 0x00FFFFFF);
}

// Test GPU DMA LinkedList with maximum packet size
TEST_F(DMATransferTest, GPU_DMA_LinkedList_MaxPacketSize) {
    uint32_t gpu_madr = 0x1F8010A0;
    uint32_t gpu_chcr = 0x1F8010A8;

    uint32_t ram_addr = 0x8000A000;

    // Maximum packet size is 255 words (FF in top byte)
    bus->storeWord(ram_addr, 0xFFFFFFFF); // 255 words, end marker

    // Fill with GPU commands (setup commands + NOPs)
    bus->storeWord(ram_addr + 4, 0xE1000000);   // Draw Mode
    bus->storeWord(ram_addr + 8, 0xE3000000);   // Drawing Area top left
    bus->storeWord(ram_addr + 12, 0xE4000000);  // Drawing Area bottom right
    bus->storeWord(ram_addr + 16, 0xE5000000);  // Drawing Offset
    bus->storeWord(ram_addr + 20, 0xE6000000);  // Mask Bit Setting
    // Fill remainder with NOPs
    for (int i = 5; i < 255; i++)
        bus->storeWord(ram_addr + 4 + i * 4, 0x00000000);  // NOP
    dma->write32(ram_addr, gpu_madr);
    dma->write32(0x01000401, gpu_chcr);

    // Verify completed
    EXPECT_EQ(dma->read32(gpu_chcr) & 0x01000000, 0);
}
