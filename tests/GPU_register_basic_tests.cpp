#include <gtest/gtest.h>

#include "GPU.hpp"
#include "Bus.hpp"

constexpr uint32_t GP0_ADDR = 0x1F801810;
constexpr uint32_t GP1_ADDR = 0x1F801814;

class GpuRegisterBasicTests : public testing::Test
{
    protected:
        Bus bus;
        GPU gpu;

        GpuRegisterBasicTests() :
            gpu(&bus)
        {
        }
};

TEST_F(GpuRegisterBasicTests, GPUSTAT_Init)
{
    uint32_t val = gpu.read32(GP1_ADDR);
    EXPECT_EQ(val, 0x1C802000);
}

TEST_F(GpuRegisterBasicTests, Reset)
{
    gpu.write32(0x08FFFFFF, GP1_ADDR);
    gpu.reset();
    EXPECT_EQ(gpu.read32(GP1_ADDR), 0x1C802000);
}

TEST_F(GpuRegisterBasicTests, Read_Internal_Registers_Init)
{
    // Also test mirrors of the 0x0 -> 0xF range
    for (int i = 0; i < 0xFFFFFF; i++) {
        gpu.write32(0x10000000 + i, GP1_ADDR);
        if ((i & 0xF) == 7) {
            EXPECT_EQ(gpu.read32(GP0_ADDR), 2);
        } else {
            EXPECT_EQ(gpu.read32(GP0_ADDR), 0);
        }
    }
}

TEST_F(GpuRegisterBasicTests, Enable_Disable_Display)
{
    gpu.write32(0x03000000, GP1_ADDR); // Enable display
    EXPECT_EQ((gpu.read32(GP1_ADDR) >> 23) & 1, 0);
    gpu.write32(0x03000001, GP1_ADDR); // Disable display
    EXPECT_EQ((gpu.read32(GP1_ADDR) >> 23) & 1, 1);
}

TEST_F(GpuRegisterBasicTests, Set_DMA_Direction)
{
    gpu.write32(0x04000000, GP1_ADDR); // DMA Off
    EXPECT_EQ((gpu.read32(GP1_ADDR) >> 29) & 3, 0);
    gpu.write32(0x04000001, GP1_ADDR); // FIFO
    EXPECT_EQ((gpu.read32(GP1_ADDR) >> 29) & 3, 1);
    gpu.write32(0x04000002, GP1_ADDR); // CPUtoGPU0
    EXPECT_EQ((gpu.read32(GP1_ADDR) >> 29) & 3, 2);
    gpu.write32(0x04000003, GP1_ADDR); // GPU0toCPU
    EXPECT_EQ((gpu.read32(GP1_ADDR) >> 29) & 3, 3);
}

TEST_F(GpuRegisterBasicTests, Set_Display_Mode)
{
    gpu.write32(0x0800000D, GP1_ADDR); // 320x480 PAL 15bit color video mode

    uint32_t gpuStat = gpu.read32(GP1_ADDR);
    EXPECT_EQ((gpuStat >> 17) & 3, 1); // Horizontal resolution 1 = 320
    EXPECT_EQ((gpuStat >> 19) & 1, 1); // Vertical resolution = 480
    EXPECT_EQ((gpuStat >> 20) & 1, 1); // PAL video mode
    EXPECT_EQ((gpuStat >> 21) & 1, 0); // Display Color Depth = 15bits
    EXPECT_EQ((gpuStat >> 22) & 1, 0); // Interlacing off
    EXPECT_EQ((gpuStat >> 16) & 1, 0); // Horizontal resolution 2
}

TEST_F(GpuRegisterBasicTests, Set_Drawing_Area)
{
    uint32_t topLeft = (236 << 10) | 643; // Top left corner at 643,236 (x,y)
    uint32_t bottomRight = (500 << 10) | 1012; // Top left corner at 1012,500 (x,y)
    gpu.write32(0xE3000000 | topLeft, GP0_ADDR); // Set Draw Area top-left
    gpu.write32(0xE4000000 | bottomRight, GP0_ADDR); // Set Draw Area bottom-right

    gpu.write32(0x10000003, GP1_ADDR); // Read draw area top-left
    EXPECT_EQ(gpu.read32(GP0_ADDR), topLeft);
    gpu.write32(0x10000004, GP1_ADDR); // Read draw area bottom-right
    EXPECT_EQ(gpu.read32(GP0_ADDR), bottomRight);
}

TEST_F(GpuRegisterBasicTests, Set_Drawing_Offset)
{
    uint32_t offset = (236 << 11) | 643; // Offset at 643,236 (x,y)
    gpu.write32(0xE5000000 | offset, GP0_ADDR); // Set Draw Area bottom-right
    gpu.write32(0x10000005, GP1_ADDR); // Read draw area top-left
    EXPECT_EQ(gpu.read32(GP0_ADDR), offset);
}
