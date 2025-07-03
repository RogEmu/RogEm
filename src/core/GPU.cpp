#include "GPU.hpp"

#include <spdlog/spdlog.h>
#include <algorithm>
#include <cmath>

#include "Bus.h"

Vec2i randomVertex(int width, int height)
{
    return Vec2i{
        rand() % width,
        rand() % height
    };
}

GPU::GPU(Bus *bus) :
    PsxDevice(bus)
{
    m_memoryRange = MemoryMap::GPU_REGISTERS_RANGE;
    reset();
}

GPU::~GPU()
{
}

void GPU::reset()
{
    // m_statRegister = 0x1C000000;
    std::memset(&m_drawArea, 0, sizeof(m_drawArea));
    std::memset(&m_drawOffset, 0, sizeof(m_drawOffset));
    std::memset(&m_displayArea, 0, sizeof(m_displayArea));
    std::memset(&m_gpuStat, 0, sizeof(m_gpuStat));
    m_gpuStat.rdReceiveDmaBlock = true;
    m_gpuStat.rdReceiveCmd = true;
    m_gpuStat.displayEnable = DisplayEnable::Disabled;
    m_gpuStat.interlaceField = true;
    m_gpuStat.rdSendVram = true;

    m_vram.fill(0xFF);
    m_gpuRead = 0;
    m_currentState = GpuState::WaitingForCommand;
    m_currentCmd.reset();
    m_nbExpectedParams = -1;

    // rasterizePoly3(Vec2i{0, 0}, Vec2i{256, 0}, Vec2i{0, 512}, ColorRGBA(0xFF0000FF));
    // rasterizePoly3(Vec2i{0, 0}, Vec2i{100, 100}, Vec2i{0, 100}, ColorRGBA(0x00FF00FF));
}

void GPU::write8(uint8_t /* value */, uint32_t /* address */)
{
    spdlog::warn("GPU: write byte is unhandled");
}

void GPU::write16(uint16_t /* value */, uint32_t /* address */)
{
    spdlog::warn("GPU: write halfword is unhandled");
}

void GPU::write32(uint32_t value, uint32_t address)
{
    spdlog::debug("GPU: Write 0x{:08X} to 0x{:08X}", value, address);
    switch (address)
    {
    case 0x1F801810:
        processGP0(value);
        break;
    case 0x1F801814:
        processGP1(value);
        break;
    default:
        spdlog::error("GPU: Cannot access port at 0x{:08X}", address);
        break;
    }
}


uint8_t GPU::read8(uint32_t /* address */)
{
    spdlog::warn("GPU: read byte is unhandled");
    return 0;
}

uint16_t GPU::read16(uint32_t /* address */)
{
    spdlog::warn("GPU: read halfword is unhandled");
    return 0;
}

uint32_t GPU::read32(uint32_t address)
{
    uint32_t result = 0;

    if (address == 0x1F801814) {
        result = gpuStat();
    } else if (address == 0x1F801810) {
        result = m_gpuRead;
    }
    spdlog::debug("GPU: read from 0x{:08X} = 0x{:08X}", address, result);
    return result;
}

const uint8_t *GPU::getVram() const
{
    return m_vram.data();
}

uint32_t GPU::gpuStat() const
{
    uint32_t result = 0;
    result |= m_gpuStat.texPageBase.x;
    result |= static_cast<uint32_t>(m_gpuStat.texPageBase.y) << 4;
    result |= static_cast<uint32_t>(m_gpuStat.semiTransparency) << 5;
    result |= static_cast<uint32_t>(m_gpuStat.dither) << 9;
    result |= static_cast<uint32_t>(m_gpuStat.drawToDisplayArea) << 10;
    result |= static_cast<uint32_t>(m_gpuStat.setMaskBitWhenDrawing) << 11;
    result |= static_cast<uint32_t>(m_gpuStat.drawPixels) << 12;
    result |= static_cast<uint32_t>(m_gpuStat.interlaceField) << 13;
    result |= static_cast<uint32_t>(m_gpuStat.hRes2) << 16;
    result |= static_cast<uint32_t>(m_gpuStat.hRes1) << 17;
    result |= static_cast<uint32_t>(m_gpuStat.vRes) << 19;
    result |= static_cast<uint32_t>(m_gpuStat.videoMode) << 20;
    result |= static_cast<uint32_t>(m_gpuStat.colorDepth) << 21;
    result |= static_cast<uint32_t>(m_gpuStat.vInterlace) << 22;
    result |= static_cast<uint32_t>(m_gpuStat.displayEnable) << 23;
    result |= static_cast<uint32_t>(m_gpuStat.irq) << 24;
    result |= static_cast<uint32_t>(m_gpuStat.dreq) << 25;
    result |= static_cast<uint32_t>(m_gpuStat.rdReceiveCmd) << 26;
    result |= static_cast<uint32_t>(m_gpuStat.rdSendVram) << 27;
    result |= static_cast<uint32_t>(m_gpuStat.rdReceiveDmaBlock) << 28;
    result |= static_cast<uint32_t>(m_gpuStat.dmaDir) << 29;
    result |= static_cast<uint32_t>(m_gpuStat.interlaceDrawLines) << 31;
    return result;
}

void GPU::setDisplayMode(uint8_t modeBits)
{
    m_gpuStat.hRes1 = static_cast<HorizontalRes>(modeBits & 3);
    m_gpuStat.vRes = static_cast<VerticalRes>((modeBits >> 2) & 1);
    m_gpuStat.videoMode = static_cast<VideoMode>((modeBits >> 3) & 1);
    m_gpuStat.colorDepth = static_cast<ColorDepth>((modeBits >> 4) & 1);
    m_gpuStat.vInterlace = (modeBits >> 5) & 1;
    m_gpuStat.hRes2 = (modeBits >> 6) & 1;
    // GPUSTAT flip screen horizontally ???
    // I don't know which version between v1 and v2 to use here
    // I use v2 so no screen flip :)
}

void GPU::readInternalRegister(uint8_t reg)
{
    switch (reg)
    {
    case 0x00:
    case 0x01:
    case 0x06:
    case 0x09:
    case 0x0A:
    case 0x0B:
    case 0x0C:
    case 0x0D:
    case 0x0E:
    case 0x0F:
        // Do nothing
        break;
    case 0x02:
        // Read Texture Window setting
        break;
    case 0x03:
        // Read Draw Area top left
        m_gpuRead = m_drawArea.topLeft.x & 0x3FF;
        m_gpuRead |= (m_drawArea.topLeft.y & 0x3FF) << 10;
        break;
    case 0x04:
        // Read Draw Area bottom right
        m_gpuRead = m_drawArea.botRight.x & 0x3FF;
        m_gpuRead |= (m_drawArea.botRight.y & 0x3FF) << 10;
        break;
    case 0x05:
        // Read Draw Offset
        m_gpuRead = m_drawOffset.x & 0x7FF;
        m_gpuRead |= (m_drawOffset.y & 0x7FF) << 11;
        break;
    case 0x07:
        // Read GPU Version
        m_gpuRead = 2;
        break;
    case 0x08:
        m_gpuRead = 0;
        break;
    default:
        break;
    }
}

void GPU::processGP0(uint32_t data)
{
    uint8_t top = data >> 29;

    if (m_currentState == GpuState::ReceivingParameters) {
        m_currentCmd.addParam(cmd);

        if (m_currentCmd.nbParams() == m_nbExpectedParams) {
            // Trigger function call
            if (m_currentCmd.command() == CommandType::DrawPolygon) {
                drawPolygon();
            }
            spdlog::warn("GPU: Must call appropriate command function here!!!");
            m_currentCmd.reset();
            m_currentState = GpuState::WaitingForCommand;
        }
        return;
    }

    switch (top)
    {
    case 0b000:
    case 0b111:
                handleEnvCommand(data);
        break;
    case 0b001: {
                int nbVertices = ((data >> 27) & 1) == 0 ? 3 : 4;
                m_nbExpectedParams = ((data >> 28) & 1) * (nbVertices - 1); // Nb Gouraud Shading vertices
                m_nbExpectedParams += ((data >> 26) & 1) * nbVertices; // Nb UV coordinates
        m_nbExpectedParams += nbVertices;
        m_currentState = GpuState::ReceivingParameters;
                m_currentCmd.setCommand(data);
        break;
    }
    case 0b010:
    case 0b011:
    case 0b100:
    case 0b101:
    case 0b110:
        spdlog::warn("GPU: Current Command = 0b{:03b}", top);
        break;
    default:
                spdlog::warn("GPU: GP0 command 0x{:08X} unsupported", data);
        break;
    }
}

void GPU::processGP1(uint32_t cmd)
{
    uint8_t opcode = (cmd >> 24) & 0xFF;

    switch (opcode)
    {
    case 0x00:
        reset();
        break;
    case 0x01:
        // Reset command buffer
        // Does nothin for now as we don't hava a command FIFO
        break;
    case 0x02:
        m_gpuStat.irq = false;
        break;
    case 0x03:
        m_gpuStat.displayEnable = static_cast<DisplayEnable>(cmd & 1);
        break;
    case 0x04:
        m_gpuStat.dmaDir = static_cast<DMADirection>(cmd & 3);
        break;
    case 0x05:
        // Set start of VRAM display area
        m_displayArea.halfwordAddress = cmd & 0x3FF;
        m_displayArea.scanlineAddress = (cmd >> 10) & 0x1FF;
        break;
    case 0x06:
        // Set horizontal display range
        m_hDisplayRange.v1 = cmd & 0xFFF;
        m_hDisplayRange.v2 = (cmd >> 12) & 0xFFF;
        break;
    case 0x07:
        // Set vertical display range
        m_vDisplayRange.v1 = cmd & 0x3FF;
        m_vDisplayRange.v2 = (cmd >> 10) & 0x3FF;
        break;
    case 0x08:
        setDisplayMode(cmd & 0xFF);
        break;
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x1E:
    case 0x1F:
        // Read internal GPU register
        readInternalRegister(cmd & 0xF);
        break;
    default:
        break;
    }
}

void GPU::handleEnvCommand(uint32_t cmd)
{
    uint8_t top = cmd >> 24;

    switch (top)
    {
    case 0x00:
        // NOP
        break;
    case 0x01:
        // Clear Cache
        break;
    case 0x02:
        // Quick Rectangle Fill
        break;
    case 0x1F:
        // IRQ
        break;
    case 0xE1:
        // Draw Mode setting
        setDrawMode(cmd & 0xFFFFFF);
        break;
    case 0xE2:
        // Texture Window setting
        break;
    case 0xE3:
        // Set Draw Area bottom-right
        m_drawArea.topLeft.x = cmd & 0x3FF;
        m_drawArea.topLeft.y = (cmd >> 10) & 0x3FF;
        break;
    case 0xE4:
        // Set Draw Area bottom-right
        m_drawArea.botRight.x = cmd & 0x3FF;
        m_drawArea.botRight.y = (cmd >> 10) & 0x3FF;
        break;
    case 0xE5:
        // Set Drawing Offset
        m_drawOffset.x = cmd & 0x7FF;
        m_drawOffset.y = (cmd >> 11) & 0x7FF;
        break;
    case 0xE6:
        // Mask bit setting
        m_gpuStat.setMaskBitWhenDrawing = cmd & 1;
        m_gpuStat.drawPixels = (cmd >> 1) & 1;
        break;
    default:
        spdlog::error("GPU: Unknow GP0 command 0x{:08X}", cmd);
        break;
    }
}

void GPU::setDrawMode(uint32_t mode)
{
    m_gpuStat.texPageBase.x = mode & 7;
    m_gpuStat.texPageBase.y = (mode >> 4) & 1;
    m_gpuStat.semiTransparency = (mode >> 5) & 3;
    m_gpuStat.texPageColors = static_cast<TexturePageColors>((mode >> 7) & 3);
    m_gpuStat.dither = (mode >> 9) & 1;
    m_gpuStat.drawToDisplayArea = (mode >> 10) & 1;
    m_textureRectFlip.x = (mode >> 12) & 1;
    m_textureRectFlip.y = (mode >> 13) & 1;
}

static Vec2i getVec(uint32_t param)
{
    Vec2i vec{.x = (int)(param & 0xFFFF), .y = (int)((param >> 16) & 0xFFFF)};
    return vec;
}

void GPU::drawPolygon()
{
    int nbVerts = (m_currentCmd.raw() >> 27) & 1 == 0 ? 3 : 4;
    ColorRGBA color(m_currentCmd.raw() & 0xFFFFFF);
    const uint32_t *params = m_currentCmd.params();
    Vec2i verts[4];

    for (int i = 0; i < nbVerts; i++) {
        verts[i] = getVec(params[i]);
    }
    rasterizePoly4(verts, color);
}

static int edgeFunction(const Vec2i& a, const Vec2i& b, const Vec2i& c)
{
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

void GPU::rasterizePoly3(const Vec2i &v0, const Vec2i &v1, const Vec2i &v2, const ColorRGBA &color)
{
    uint16_t argbColor = color.toABGR1555();

    int minX = std::max(0, std::min({v0.x, v1.x, v2.x}));
    int maxX = std::min(1023, std::max({v0.x, v1.x, v2.x}));
    int minY = std::max(0, std::min({v0.y, v1.y, v2.y}));
    int maxY = std::min(511, std::max({v0.y, v1.y, v2.y}));

    int area = edgeFunction(v0, v1, v2);
    if (area == 0)
        return;

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            Vec2i p = {x, y};
            int w0 = edgeFunction(v1, v2, p);
            int w1 = edgeFunction(v2, v0, p);
            int w2 = edgeFunction(v0, v1, p);

            bool test = w0 <= 0 && w1 <= 0 && w2 <= 0;
            int num = w0 | w1 | w2;
            if (test) {
                setPixel(p, argbColor);
                num += 1;
            }
        }
    }
}

void GPU::rasterizePoly4(const Vec2i *verts, const ColorRGBA &color)
{
    rasterizePoly3(verts[0], verts[1], verts[2], color);
    rasterizePoly3(verts[1], verts[3], verts[2], color);
}

void GPU::setPixel(const Vec2i &pos, uint16_t color)
{
    int index = (pos.y * 1024 + pos.x) * 2;
    m_vram[index] = color & 0xFF;
    m_vram[index + 1] = color >> 8;
}
