#include <gtest/gtest.h>
#include "Core/DMA.hpp"

TEST(DMA, unsupportedWrites)
{
    DMA dma(nullptr);

    dma.write8(0, 0);
    dma.write16(0, 0);
}

TEST(DMA, unsupportedReads)
{
    DMA dma(nullptr);

    EXPECT_EQ(dma.read8(0), 0);
    EXPECT_EQ(dma.read16(0), 0);
}

TEST(DMA, readWriteCommonRegisters)
{
    DMA dma(nullptr);
    uint32_t value = 0xCAFEBABE;

    dma.write32(value, 0x1F8010F0); // DPCR register
    EXPECT_EQ(dma.read32(0x1F8010F0), value);
    dma.write32(value, 0x1F8010F4); // DICR register
    EXPECT_EQ(dma.read32(0x1F8010F4), value);
    dma.write32(value, 0x1F8010F8); // Unknown register
    EXPECT_EQ(dma.read32(0x1F8010F8), 0);
}

TEST(DMA, ReadWriteBaseAddressChannel)
{
    DMA dma(nullptr);
    uint32_t value = 0xCAFEBABE;
    uint32_t base = 0x1F801000;

    dma.write32(value, base + 0x80); // MDECin channel
    EXPECT_EQ(dma.read32(base + 0x80), value & 0xFFFFFF);

    dma.write32(value, base + 0x90); // MDECout channel
    EXPECT_EQ(dma.read32(base + 0x90), value & 0xFFFFFF);

    dma.write32(value, base + 0xA0); // GPU channel
    EXPECT_EQ(dma.read32(base + 0xA0), value & 0xFFFFFF);

    dma.write32(value, base + 0xB0); // CDROM channel
    EXPECT_EQ(dma.read32(base + 0xB0), value & 0xFFFFFF);

    dma.write32(value, base + 0xC0); // SPU channel
    EXPECT_EQ(dma.read32(base + 0xC0), value & 0xFFFFFF);

    dma.write32(value, base + 0xD0); // PIO channel
    EXPECT_EQ(dma.read32(base + 0xD0), value & 0xFFFFFF);

    dma.write32(value, base + 0xE0); // OTC channel
    EXPECT_EQ(dma.read32(base + 0xE0), value & 0xFFFFFF);
}
