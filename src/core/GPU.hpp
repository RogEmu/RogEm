/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** GPU
*/

#ifndef GPU_HPP_
#define GPU_HPP_

#include "PsxDevice.hpp"

#include <array>

#define GPU_VRAM_1MB_SIZE 2048*512 // 512 lines of 2048 bytes (1024 pixels)

struct TexturePageBase
{
    uint8_t x;
    uint8_t y;
};

enum class TexturePageColors
{
    COL_4Bit,
    COL_8Bit,
    COL_15Bit,
    RESERVED
};

enum class HorizontalRes
{
    RES_256,
    RES_320,
    RES_512,
    RES_640
};

enum class VerticalRes
{
    RES_240,
    RES_480
};

enum class VideoMode
{
    NTSC,
    PAL
};

enum class DMADirection
{
    OFF,
    UNKNOWN,
    CPU_GP0,
    GPUREAD_CPU
};

enum class ColorDepth
{
    COLDEP_15bit,
    COLDEP_24bit
};

enum class DisplayEnable
{
    Enabled,
    Disabled
};

struct GPUStat
{
    TexturePageBase texPageBase;
    uint8_t semiTransparency;
    TexturePageColors texPageColors;
    bool dither;
    bool drawToDisplayArea;
    bool setMaskBitWhenDrawing;
    bool drawPixels;
    bool interlaceField;
    bool hRes2;
    HorizontalRes hRes1;
    VerticalRes vRes;
    VideoMode videoMode;
    ColorDepth colorDepth;
    bool vInterlace;
    DisplayEnable displayEnable;
    bool irq;
    bool dreq;
    bool rdReceiveCmd;
    bool rdSendVram;
    bool rdReceiveDmaBlock;
    DMADirection dmaDir;
    bool interlaceDrawLines;
};

struct VramDisplayArea
{
    uint16_t halfwordAddress;
    uint16_t scanlineAddress;
};

struct VramDisplayRange
{
    uint16_t v1;
    uint16_t v2;
};

struct Vec2i
{
    int x;
    int y;
};

struct ColorRGBA
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    ColorRGBA() = default;

    ColorRGBA(uint32_t color) {
        r = (color >> 24) & 0xFF;
        g = (color >> 16) & 0xFF;
        b = (color >> 8) & 0xFF;
        a = color & 0xFF;
    }

    uint16_t toABGR1555() const {
        uint16_t color = 0;
        color |= 0x8000;
        color |= (b >> 3) << 10;
        color |= (g >> 3) << 5;
        color |= (r >> 3);
        return color;
    }
};

struct Vertex
{
    Vec2i position;
    ColorRGBA color;
};

struct TextureRectFlip
{
    bool x;
    bool y;
};

struct VramDrawArea
{
    Vec2i topLeft;
    Vec2i botRight;
};

class GPU : public PsxDevice
{
    public:
        GPU(Bus *bus);
        ~GPU();

        void reset();

        void write8(uint8_t value, uint32_t address) override;
        void write16(uint16_t value, uint32_t address) override;
        void write32(uint32_t value, uint32_t address) override;

        uint8_t read8(uint32_t address) override;
        uint16_t read16(uint32_t address) override;
        uint32_t read32(uint32_t address) override;

        const uint8_t *getVram() const;

    private:
        uint32_t gpuStat() const;
        void setDisplayMode(uint8_t modeBits);
        void readInternalRegister(uint8_t reg);

        void handleGP0Command(uint32_t cmd);
        void handleGP1Command(uint32_t cmd);

        void handleEnvCommand(uint32_t cmd);

        void setDrawMode(uint32_t mode);

        void rasterizeTriangle(const Vec2i &v0, const Vec2i &v1, const Vec2i &v2, const ColorRGBA& color);

    private:
        GPUStat m_gpuStat;
        uint32_t m_gpuRead;
        VramDisplayArea m_displayArea;
        VramDisplayRange m_hDisplayRange;
        VramDisplayRange m_vDisplayRange;
        TextureRectFlip m_textureRectFlip;
        VramDrawArea m_drawArea;
        Vec2i m_drawOffset;

        std::array<uint8_t, GPU_VRAM_1MB_SIZE> m_vram;
};

#endif /* !GPU_HPP_ */
