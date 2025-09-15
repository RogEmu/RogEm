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

    m_vram.fill(0);
    m_gpuRead = 0;
    m_currentState = GpuState::WaitingForCommand;
    m_currentCmd.reset();
    m_nbExpectedParams = -1;
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
    spdlog::debug("GPU: Read from 0x{:08X} = 0x{:08X}", address, result);
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

    if (m_currentState == GpuState::ReceivingDataWords) {
        receiveDataWord(data);
    } else if (m_currentState == GpuState::ReceivingParameters) {
        receiveParameter(data);
    } else {
        switch (top)
        {
            case 0b000:
            case 0b111:
                handleEnvCommand(data);
                break;
            case 0b001: {
                int nbVertices = ((data >> 27) & 1) == 0 ? 3 : 4;
                m_nbExpectedParams = ((data >> 28) & 1) * (nbVertices - 1); // Nb Vertex Colors
                m_nbExpectedParams += ((data >> 26) & 1) * nbVertices; // Nb UV coordinates
                m_nbExpectedParams += nbVertices;
                m_currentState = GpuState::ReceivingParameters;
                m_currentCmd.addParam(data & 0xFFFFFF);
                m_currentCmd.setCommand(data);
                m_nbExpectedParams += 1;
                break;
            }
            case 0b010: { // draw Line
                if (((data >> 27) & 1) == 1)
                    m_nbExpectedParams = -1;
                else
                    m_nbExpectedParams = ((data >> 28) & 1) == 0 ? 3 : 4;
                m_currentState = GpuState::ReceivingParameters;
                m_currentCmd.setCommand(data);
                m_currentCmd.addParam(data & 0xFFFFFF);
                break;
            }
            case 0b011: { // Draw Rectangle
                m_nbExpectedParams = 2;
                m_nbExpectedParams += ((data >> 27) & 3) == 0;
                m_currentCmd.addParam(data & 0xFFFFFF);
                m_currentCmd.setCommand(data);
                m_currentState = GpuState::ReceivingParameters;
                break;
            }
            case 0b100: // VRAM to VRAM copy
                m_nbExpectedParams = 3;
                m_currentState = GpuState::ReceivingParameters;
                m_currentCmd.setCommand(data);
                break;
            case 0b101: // CPU to VRAM blitting
                m_nbExpectedParams = 2;
                m_currentState = GpuState::ReceivingParameters;
                m_currentCmd.setCommand(data);
                break;
            case 0b110:
                break;
            default:
                spdlog::warn("GPU: GP0 command 0x{:08X} unsupported", data);
                break;
        }
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
        m_nbExpectedParams = 2;
        m_currentState = GpuState::ReceivingParameters;
        m_currentCmd.setCommand(cmd);
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
        // Set Draw Area top-left
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
    m_gpuStat.texPageBase.x = mode & 0xF;
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
    int nbVerts = ((m_currentCmd.raw() >> 27) & 1) == 0 ? 3 : 4;
    bool shaded = (m_currentCmd.raw() >> 28) & 1;
    bool textured = (m_currentCmd.raw() >> 26) & 1;
    const uint32_t *params = m_currentCmd.params();
    ColorRGBA firstColor;
    firstColor.fromBGR(params[0]);
    Vertex verts[4];

    spdlog::warn("GPU: Draw Polygon with {} vertices", nbVerts);

    int step = 1 + shaded + textured;
    for (int i = 0; i < nbVerts; i++) {
        if (shaded) {
            verts[i].color.fromBGR(params[i * step]);
        }

        verts[i].pos = getVec(params[i * step + 1]);
    }
    if (nbVerts == 4) {
        rasterizePoly4(verts, firstColor);
    } else {
        rasterizePoly3(verts, firstColor);
    }
    m_currentCmd.reset();
    m_currentState = GpuState::WaitingForCommand;
}

void GPU::drawRectangle()
{
    auto params = m_currentCmd.params();
    Vec2i topLeft{(int)(params[1] & 0xFFFF), (int)(params[1] >> 16)};
    ColorRGBA color;
    color.fromBGR(params[0]);
    Vec2i size{};
    int rectSizeFlag = (m_currentCmd.raw() >> 27) & 3;

    if (rectSizeFlag != 0) {
        int scale = (rectSizeFlag - 1) * 8;
        scale = scale ? scale : 1;
        size = {scale, scale};
    } else {
        size = getVec(params[2]);
    }
    rasterizeRectangle({topLeft, color}, size);
    m_currentCmd.reset();
    m_currentState = GpuState::WaitingForCommand;
}

void GPU::drawLine() {
    const uint32_t *params = m_currentCmd.params();
    bool goraud = (m_currentCmd.raw() >> 28) & 1;
    bool polyline = (m_currentCmd.raw() >> 27) & 1;
    Vertex v0, v1;
    int step = 1 + goraud;

    v0.color.fromBGR(params[0]);
    v0.pos = getVec(params[1]);
    v1.pos = getVec(params[2 + goraud]);
    if (goraud)
        v1.color.fromBGR(params[2]);
    else
        v1.color.fromBGR(params[0]);
    rasterizeLine(v0, v1);
    if (polyline) {
        int iteration = m_currentCmd.nbParams() - 1;
        int numSegments = (iteration - (3 + goraud)) / step;
        for (int i = 0; i < numSegments; i++) {
            v0 = v1;
            if (goraud)
                v1.color.fromBGR(params[(3 + goraud) + step * i]);
            else
                v1.color.fromBGR(params[0]);
            v1.pos = getVec(params[(3 + goraud) + step * i + goraud]);
            rasterizeLine(v0, v1);
        }
    }
    m_currentCmd.reset();
    m_currentState = GpuState::WaitingForCommand;
}

void GPU::startCpuToVramCopy()
{
    spdlog::warn("GPU: Copy data from CPU to VRAM");
    auto params = m_currentCmd.params();
    m_currentState = GpuState::ReceivingDataWords;
    m_vramCopyData.startPos = Vec2i{(int)(params[0] & 0x3FF), (int)((params[0] >> 16) & 0x1FF)};
    m_vramCopyData.size = Vec2i{(int)((params[1] - 1) & 0x3FF) + 1, (int)(((params[1] >> 16) - 1) & 0x1FF)};
    m_vramCopyData.currentPos = Vec2i{0, 0};
}

void GPU::quickRectFill()
{
    auto params = m_currentCmd.params();
    uint32_t raw = m_currentCmd.raw();
    Vec2i topLeft{(int)(params[0] & 0xFFFF), (int)(params[0] >> 16)};
    Vec2i size{(int)(params[1] & 0xFFFF), (int)(params[1] >> 16)};
    ColorRGBA color;

    color.r = raw & 0xFF;
    color.g = (raw >> 8) & 0xFF;
    color.b = (raw >> 16) & 0xFF;
    color.a = 0xFF;

    uint16_t abgr = color.toABGR1555();
    Vec2i pos;

    for (int y = 0; y < size.y; y++) {
        for (int x = 0; x < size.x; x++) {
            for (int pix = 0; pix < 16; pix++) {
                pos.x = topLeft.x + x;
                pos.y = topLeft.y + y;
                setPixel(pos, abgr);
            }
        }
    }
    m_currentState = GpuState::WaitingForCommand;
    m_currentCmd.reset();
}

void GPU::startVramToVramCopy()
{
    // The transfer should be affected by the Mask Bit setting
    auto params = m_currentCmd.params();
    Vec2i sourceCoord{(int)(params[0] & 0xFFFF), (int)(params[0] >> 16)};
    Vec2i destCoord{(int)(params[1] & 0xFFFF), (int)(params[1] >> 16)};
    Vec2i size{(int)(params[2] & 0xFFFF), (int)(params[2] >> 16)};

    for (int y = 0; y < size.y; y++) {
        for (int x = 0; x < size.x; x++) {
            Vec2i currSourceCoord{sourceCoord.x + x, sourceCoord.y + y};
            Vec2i currDestCoord{destCoord.x + x, destCoord.y + y};
            uint16_t color = getPixel(currSourceCoord);
            setPixel(currDestCoord, color);
        }
    }
    m_currentState = GpuState::WaitingForCommand;
    m_currentCmd.reset();
}

void GPU::receiveParameter(uint32_t param)
{
    m_currentCmd.addParam(param);
    if (m_nbExpectedParams == -1 && (param & 0xF000F000) == 0x50005000) {
        drawLine();
    }

    if (m_currentCmd.nbParams() == m_nbExpectedParams) {
        if (m_currentCmd.command() == CommandType::DrawPolygon) {
            drawPolygon();
        } else if (m_currentCmd.command() == CommandType::CpuVramCopy) {
            startCpuToVramCopy();
        } else if (m_currentCmd.command() == CommandType::QuickRectFill) {
            quickRectFill();
        } else if (m_currentCmd.command() == CommandType::VramVramCopy) {
            startVramToVramCopy();
        } else if (m_currentCmd.command() == CommandType::DrawRectangle) {
            drawRectangle();
        } else if (m_currentCmd.command() == CommandType::DrawLine) {
            drawLine();
        }
    }
}

void GPU::receiveDataWord(uint32_t data)
{
    for (int i = 0; i < 2; i++) {
        uint16_t pix = ((data >> (16 * i)) & 0xFFFF) | 0x8000;
        Vec2i pos{
            m_vramCopyData.currentPos.x + m_vramCopyData.startPos.x,
            m_vramCopyData.currentPos.y + m_vramCopyData.startPos.y
        };
        setPixel(pos, pix);
        m_vramCopyData.currentPos.x++;
        if (m_vramCopyData.currentPos.x >= m_vramCopyData.size.x) {
            m_vramCopyData.currentPos.x = 0;
            m_vramCopyData.currentPos.y++;
        }
        if (m_vramCopyData.currentPos.y >  m_vramCopyData.size.y) {
            m_currentState = GpuState::WaitingForCommand;
            m_currentCmd.reset();
            return;
        }
    }
}

static int edgeFunction(const Vec2i& a, const Vec2i& b, const Vec2i& c)
{
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

static ColorRGBA interpolateColor(const ColorRGBA& c0, const ColorRGBA& c1, const ColorRGBA& c2, float alpha, float beta, float gamma)
{
    ColorRGBA color;
    color.r = static_cast<uint8_t>(c0.r * alpha + c1.r * beta + c2.r * gamma);
    color.g = static_cast<uint8_t>(c0.g * alpha + c1.g * beta + c2.g * gamma);
    color.b = static_cast<uint8_t>(c0.b * alpha + c1.b * beta + c2.b * gamma);
    color.a = static_cast<uint8_t>(c0.a * alpha + c1.a * beta + c2.a * gamma);
    return color;
}

void GPU::rasterizeLine(const Vertex& v0, const Vertex& v1)
{
    int x0 = v0.pos.x;
    int y0 = v0.pos.y;
    int x1 = v1.pos.x;
    int y1 = v1.pos.y;

    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    float steps = static_cast<float>(std::max(dx, dy));
    if (steps == 0.0f)
        steps = 1.0f;

    float dr = (v1.color.r - v0.color.r) / steps;
    float dg = (v1.color.g - v0.color.g) / steps;
    float db = (v1.color.b - v0.color.b) / steps;
    float da = (v1.color.a - v0.color.a) / steps;
    ColorRGBA c = v0.color;

    while (true) {
        setPixel(Vec2i(x0, y0), c.toABGR1555());
        if (x0 == x1 && y0 == y1)
            break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }

        c.r = static_cast<uint8_t>(std::clamp(c.r + dr, 0.0f, 255.0f));
        c.g = static_cast<uint8_t>(std::clamp(c.g + dg, 0.0f, 255.0f));
        c.b = static_cast<uint8_t>(std::clamp(c.b + db, 0.0f, 255.0f));
        c.a = static_cast<uint8_t>(std::clamp(c.a + da, 0.0f, 255.0f));
    }
}

void GPU::rasterizePoly3(const Vertex *verts, const ColorRGBA &color)
{
    uint16_t argbColor = color.toABGR1555();
    bool shaded = (m_currentCmd.raw() >> 28) & 1;

    int minX = std::max(0, std::min({verts[0].pos.x, verts[1].pos.x, verts[2].pos.x}));
    int maxX = std::min(GPU_VRAM_WIDTH - 1, std::max({verts[0].pos.x, verts[1].pos.x, verts[2].pos.x}));
    int minY = std::max(0, std::min({verts[0].pos.y, verts[1].pos.y, verts[2].pos.y}));
    int maxY = std::min(GPU_VRAM_HEIGHT - 1, std::max({verts[0].pos.y, verts[1].pos.y, verts[2].pos.y}));

    int area = edgeFunction(verts[0].pos, verts[1].pos, verts[2].pos);
    float invArea = 1.0f / area;

    if (area == 0)
        return;

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            Vec2i p = {x, y};
            int w0 = edgeFunction(verts[1].pos, verts[2].pos, p);
            int w1 = edgeFunction(verts[2].pos, verts[0].pos, p);
            int w2 = edgeFunction(verts[0].pos, verts[1].pos, p);

            if (shaded) {
                float alpha = w0 * invArea;
                float beta = w1 * invArea;
                float gamma = w2 * invArea;
                argbColor = interpolateColor(verts[0].color, verts[1].color, verts[2].color, alpha, beta, gamma).toABGR1555();
            }

            int pos = (w0 | w1 | w2) >= 0;
            int neg = (w0 & w1 & w2) < 0;
            if (pos | neg) {
                setPixel(p, argbColor);
            }
        }
    }
}

void GPU::rasterizePoly4(const Vertex *verts, const ColorRGBA &color)
{
    rasterizePoly3(verts, color);
    rasterizePoly3(verts + 1, color);
}

void GPU::rasterizeRectangle(const Vertex &vert, const Vec2i &size)
{
    uint16_t color = vert.color.toABGR1555();
    for (int y = 0; y < size.y; y++) {
        for (int x = 0; x < size.x; x++) {
            Vec2i pos{vert.pos.x + x, vert.pos.y + y};
            setPixel(pos, color);
        }
    }
}

void GPU::setPixel(const Vec2i &pos, uint16_t color)
{
    int index = (pos.y * 1024 + pos.x) * 2;
    m_vram[index] = color & 0xFF;
    m_vram[index + 1] = color >> 8;
}

uint16_t GPU::getPixel(const Vec2i &pos)
{
    int index = (pos.y * 1024 + pos.x) * 2;
    uint16_t color = 0;
    color = m_vram[index];
    color |= m_vram[index + 1] << 8;
    return color;
}
