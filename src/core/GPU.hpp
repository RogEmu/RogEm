/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** GPU
*/

#ifndef GPU_HPP_
#define GPU_HPP_

#include <array>

#include "PsxDevice.hpp"
#include "GPUCommand.hpp"

#define GPU_VRAM_WIDTH 2048 // 2048 bytes (1024 pixels)
#define GPU_VRAM_HEIGHT 512 // 512 lines
#define GPU_VRAM_1MB_SIZE (GPU_VRAM_WIDTH * GPU_VRAM_HEIGHT) // 512 lines of 1024 pixels

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

    void fromRGB(uint32_t rgb)
    {
        r = (rgb >> 16) & 0xFF;
        g = (rgb >> 8) & 0xFF;
        b = rgb & 0xFF;
    }

    void fromBGR(uint32_t rgb)
    {
        b = (rgb >> 16) & 0xFF;
        g = (rgb >> 8) & 0xFF;
        r = rgb & 0xFF;
    }
};

struct Vertex
{
    Vec2i pos;
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

enum class GpuState
{
    WaitingForCommand,
    ReceivingParameters,
    ReceivingDataWords,
};

struct VramCopyData
{
    Vec2i size;
    Vec2i startPos;
    Vec2i currentPos;
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
        void readInternalRegister(uint8_t reg);

        void processGP0(uint32_t data);
        void processGP1(uint32_t cmd);

        void setDisplayMode(uint8_t modeBits);
        void handleEnvCommand(uint32_t cmd);
        void setDrawMode(uint32_t mode);

        // GP0 commands
        void drawPolygon();
        void drawRectangle();
        void startCpuToVramCopy();
        void quickRectFill();
        void startVramToVramCopy();

        void receiveParameter(uint32_t param);
        void receiveDataWord(uint32_t data);

        // Rasterization methods
        void rasterizePoly3(const Vertex *verts, const ColorRGBA& color);
        void rasterizePoly4(const Vertex *verts, const ColorRGBA& color);
        void rasterizeRectangle(const Vertex &vert, const Vec2i &size);

        void setPixel(const Vec2i &pos, uint16_t color);
        uint16_t getPixel(const Vec2i &pos);

    private:
        GPUStat m_gpuStat;
        uint32_t m_gpuRead;
        VramDisplayArea m_displayArea;
        VramDisplayRange m_hDisplayRange;
        VramDisplayRange m_vDisplayRange;
        TextureRectFlip m_textureRectFlip;
        VramDrawArea m_drawArea;
        Vec2i m_drawOffset;

        GpuState m_currentState;
        int m_nbExpectedParams;
        GPUCommand m_currentCmd;

        VramCopyData m_vramCopyData;

        std::array<uint8_t, GPU_VRAM_1MB_SIZE> m_vram;
};

#endif /* !GPU_HPP_ */
