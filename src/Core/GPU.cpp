#include "GPU.hpp"

#include <spdlog/spdlog.h>
#include <algorithm>
#include <cmath>

#include "Bus.hpp"
#include "InterruptController.hpp"

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

static constexpr float NTSC_CLOCK_MULTIPLIER = 11.0f/7.0f;
static constexpr uint32_t NTSC_VRES_HEIGHT = 240;
static constexpr uint32_t NTSC_SCANLINES = 263;
static constexpr uint32_t NTSC_HCYCLES = 3413;

void GPU::update(int cycles)
{
    static float cycleCount = 0;
    static uint32_t scanline = 0;

    cycleCount += cycles * NTSC_CLOCK_MULTIPLIER;
    if (cycleCount >= NTSC_HCYCLES) {
        cycleCount = 0.0f;
        if (m_gpuStat.vInterlace || m_gpuStat.interlaceField) {
            m_gpuStat.interlaceDrawLines = !m_gpuStat.interlaceDrawLines;
        }
        scanline++;
        if (scanline >= NTSC_SCANLINES) {
            scanline = 0;
            m_gpuStat.interlaceDrawLines = false;
            auto irqc = m_bus->getDevice<InterruptController>();
            irqc->triggerIRQ(DeviceIRQ::VBLANK);
        }
    }
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

uint8_t *GPU::getVram()
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
    if (m_currentState == GpuState::ReceivingDataWords) {
        receiveDataWord(data);
        return;
    }
    if (m_currentState == GpuState::ReceivingParameters) {
        receiveParameter(data);
        return;
    }

    switch (data >> 29) {
        case 0b000: // Misc commands
            executeMiscCommand(data);
            break;
        case 0b111:
            executeEnvCommand(data);
            break;
        case 0b001: // Draw Polygon
        case 0b010: // Draw Line
        case 0b011: // Draw Rectangle
        case 0b100: // VRAM to VRAM copy
        case 0b101: // CPU to VRAM blitting
            m_currentState = GpuState::ReceivingParameters;
            m_currentCmd.set(data);
            break;
        default:
            spdlog::error("GPU: Unknown GP0 command 0x{:08X}", data);
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

void GPU::executeEnvCommand(uint32_t cmd)
{
    switch (cmd >> 24) {
        case 0xE1:
            setDrawMode(cmd & 0xFFFFFF);
            break;
        case 0xE2: {
            m_textureWindow.offsetX = cmd & 0x1F;
            m_textureWindow.offsetY = (cmd >> 5) & 0x1F;
            m_textureWindow.maskX = (cmd >> 10) & 0x1F;
            m_textureWindow.maskY = (cmd >> 15) & 0x1F;
            break;
        }
        case 0xE3:
            m_drawArea.topLeft.x = cmd & 0x3FF;
            m_drawArea.topLeft.y = (cmd >> 10) & 0x3FF;
            break;
        case 0xE4:
            m_drawArea.botRight.x = cmd & 0x3FF;
            m_drawArea.botRight.y = (cmd >> 10) & 0x3FF;
            break;
        case 0xE5:
            m_drawOffset.x = cmd & 0x7FF;
            m_drawOffset.y = (cmd >> 11) & 0x7FF;
            break;
        case 0xE6:
            m_gpuStat.setMaskBitWhenDrawing = cmd & 1;
            m_gpuStat.drawPixels = (cmd >> 1) & 1;
            break;
        default:
            spdlog::error("GPU: Unknown GP0 environment command 0x{:08X}", cmd);
            break;
        }
}

void GPU::executeMiscCommand(uint32_t cmd)
{
    switch (cmd >> 24) {
        case 0x00:
            // NOP
            break;
        case 0x01:
            // Clear Cache
            break;
        case 0x02:
            m_currentCmd.set(cmd);
            m_currentState = GpuState::ReceivingParameters;
            break;
        default:
            spdlog::error("GPU: Unknown GP0 misc command 0x{:08X}", cmd);
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
    auto &flags = m_currentCmd.flags();
    auto &params = m_currentCmd.params();
    ColorRGBA firstColor;
    firstColor.fromBGR(params.data()[0]);
    Vertex verts[4];
    TextureInfo texInfo;

    int step = 1 + flags.shaded + flags.textured;
    for (int i = 0; i < flags.nbVertices; i++) {
        int paramIndex = i * step;

        // Parse color (if shaded)
        if (flags.shaded) {
            verts[i].color.fromBGR(params.data()[paramIndex]);
        }

        // Parse position
        verts[i].pos = getVec(params.data()[paramIndex + 1]);
        verts[i].pos.x += m_drawOffset.x;
        verts[i].pos.y += m_drawOffset.y;

        // Parse texture coordinates (if textured)
        if (flags.textured) {
            uint32_t texData = params.data()[paramIndex + 2];
            verts[i].u = texData & 0xFF;          // U coordinate (bits 0-7)
            verts[i].v = (texData >> 8) & 0xFF;   // V coordinate (bits 8-15)

            // First vertex: extract CLUT position (upper 16 bits)
            if (i == 0) {
                uint16_t clutInfo = (texData >> 16) & 0xFFFF;
                texInfo.clutX = (clutInfo & 0x3F) * 16;        // Bits 0-5: X in 16-pixel units
                texInfo.clutY = (clutInfo >> 6) & 0x1FF;       // Bits 6-14: Y (0-511)
            }
            // Second vertex: extract texture page info (upper 16 bits)
            else if (i == 1) {
                uint16_t texPageInfo = (texData >> 16) & 0xFFFF;
                texInfo.texPageX = texPageInfo & 0xF;           // Bits 0-3: X in 64-pixel units
                texInfo.texPageY = (texPageInfo >> 4) & 0x1;    // Bit 4: Y in 256-pixel units
                texInfo.colorMode = static_cast<TexturePageColors>((texPageInfo >> 7) & 0x3); // Bits 7-8: color mode

                // Note: Bits 5-6 contain semi-transparency mode (already in GPU status)
                // Bit 9 contains texture disable bit (for rectangles)
            }
        }
    }

    if (flags.nbVertices == 4) {
        rasterizePoly4(verts, firstColor, texInfo);
    } else {
        rasterizePoly3(verts, firstColor, texInfo);
    }
    m_currentCmd.reset();
    m_currentState = GpuState::WaitingForCommand;
}

void GPU::drawRectangle()
{
    auto &flags = m_currentCmd.flags();
    auto &params = m_currentCmd.params();
    ColorRGBA color;
    color.fromBGR(params.data()[0]);
    Vec2i size{};
    Vec2i topLeft = getVec(params.data()[1]);
    TextureInfo texInfo{};
    topLeft.x += m_drawOffset.x;
    topLeft.y += m_drawOffset.y;
    Vertex vert{topLeft, color, 0, 0};

    if (flags.textured) {
        uint32_t texData = params.data()[2];
        vert.u = texData & 0xFF;
        vert.v = (texData >> 8) & 0xFF;

        uint16_t clutInfo = (texData >> 16);
        texInfo.clutX = (clutInfo & 0x3F) * 16;
        texInfo.clutY = (clutInfo >> 6) & 0x1FF;
        texInfo.texPageX = m_gpuStat.texPageBase.x;
        texInfo.texPageY = m_gpuStat.texPageBase.y;
        texInfo.colorMode = m_gpuStat.texPageColors;
    }

    if (flags.rectFlag != GPURectSize::Variable) {
        size.x = flags.rectSize.width;
        size.y = flags.rectSize.height;
    } else {
        size = getVec(params.data()[2 + flags.textured]);
    }

    rasterizeRectangle(vert, size, texInfo);
    m_currentCmd.reset();
    m_currentState = GpuState::WaitingForCommand;
}

void GPU::drawLine() {
    auto &params = m_currentCmd.params();
    auto &flags = m_currentCmd.flags();
    Vertex v0, v1;
    int step = 1 + flags.shaded;

    v0.color.fromBGR(params.data()[0]);
    v0.pos = getVec(params.data()[1]);
    v1.pos = getVec(params.data()[2 + flags.shaded]);
    if (flags.shaded)
        v1.color.fromBGR(params.data()[2]);
    else
        v1.color.fromBGR(params.data()[0]);
    rasterizeLine(v0, v1);
    if (flags.polyline) {
        int iteration = params.size() - 1;
        int numSegments = (iteration - (3 + flags.shaded)) / step;
        for (int i = 0; i < numSegments; i++) {
            v0 = v1;
            if (flags.shaded)
                v1.color.fromBGR(params.data()[(3 + flags.shaded) + step * i]);
            else
                v1.color.fromBGR(params.data()[0]);
            v1.pos = getVec(params.data()[(3 + flags.shaded) + step * i + flags.shaded]);
            rasterizeLine(v0, v1);
        }
    }
    m_currentCmd.reset();
    m_currentState = GpuState::WaitingForCommand;
}

void GPU::startCpuToVramCopy()
{
    auto &params = m_currentCmd.params();
    m_currentState = GpuState::ReceivingDataWords;
    m_vramCopyData.startPos = Vec2i{(int)(params.data()[0] & 0x3FF), (int)((params.data()[0] >> 16) & 0x1FF)};
    m_vramCopyData.size = Vec2i{(int)((params.data()[1] - 1) & 0x3FF) + 1, (int)(((params.data()[1] >> 16) - 1) & 0x1FF)};
    m_vramCopyData.currentPos = Vec2i{0, 0};
}

void GPU::quickRectFill()
{
    auto &params = m_currentCmd.params();
    Vec2i topLeft{(int)(params.data()[1] & 0xFFFF), (int)(params.data()[1] >> 16)};
    Vec2i size{(int)(params.data()[2] & 0xFFFF), (int)(params.data()[2] >> 16)};
    ColorRGBA color;
    color.fromBGR(params.data()[0]);
    uint16_t abgr = color.toABGR1555();

    for (int y = 0; y < size.y; y++) {
        for (int x = 0; x < size.x; x++) {
            for (int pix = 0; pix < 16; pix++) {
                Vec2i pos{topLeft.x + x, topLeft.y + y};
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
    auto &params = m_currentCmd.params();
    Vec2i sourceCoord{(int)(params.data()[0] & 0xFFFF), (int)(params.data()[0] >> 16)};
    Vec2i destCoord{(int)(params.data()[1] & 0xFFFF), (int)(params.data()[1] >> 16)};
    Vec2i size{(int)(params.data()[2] & 0xFFFF), (int)(params.data()[2] >> 16)};
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

    if (m_currentCmd.expectedParams() == -1 && (param & 0xF000F000) == 0x50005000) {
        drawLine();
    }

    if (m_currentCmd.params().size() == m_currentCmd.expectedParams()) {
        switch (m_currentCmd.type()) {
            case GPUCommandType::DrawPolygon:
                drawPolygon();
                break;
            case GPUCommandType::DrawRectangle:
                drawRectangle();
                break;
            case GPUCommandType::DrawLine:
                drawLine();
                break;
            case GPUCommandType::QuickRectFill:
                quickRectFill();
                break;
            case GPUCommandType::CpuVramCopy:
                startCpuToVramCopy();
                break;
            case GPUCommandType::VramVramCopy:
                startVramToVramCopy();
                break;
            default:
                break;
        }
    }
}

void GPU::receiveDataWord(uint32_t data)
{
    for (int i = 0; i < 2; i++) {
        uint16_t pix = ((data >> (16 * i)) & 0xFFFF);
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

void GPU::rasterizePoly3(const Vertex *verts, const ColorRGBA &color, const TextureInfo& texInfo)
{
    auto &flags = m_currentCmd.flags();

    int minX = std::max(0, std::min({verts[0].pos.x, verts[1].pos.x, verts[2].pos.x}));
    int maxX = std::min(GPU_VRAM_WIDTH - 1, std::max({verts[0].pos.x, verts[1].pos.x, verts[2].pos.x}));
    int minY = std::max(0, std::min({verts[0].pos.y, verts[1].pos.y, verts[2].pos.y}));
    int maxY = std::min(GPU_VRAM_HEIGHT - 1, std::max({verts[0].pos.y, verts[1].pos.y, verts[2].pos.y}));

    int area = edgeFunction(verts[0].pos, verts[1].pos, verts[2].pos);
    float invArea = 1.0f / area;

    if (area == 0)
        return;

    ColorRGBA finalColor;
    for (int y = minY; y < maxY; y++) {
        for (int x = minX; x < maxX; x++) {
            Vec2i p = {x, y};
            int w0 = edgeFunction(verts[1].pos, verts[2].pos, p);
            int w1 = edgeFunction(verts[2].pos, verts[0].pos, p);
            int w2 = edgeFunction(verts[0].pos, verts[1].pos, p);

            int pos = (w0 >= 0) && (w1 >= 0) && (w2 >= 0);
            int neg = (w0 <= 0) && (w1 <= 0) && (w2 <= 0);

            if (pos || neg) {
                float alpha = w0 * invArea;
                float beta = w1 * invArea;
                float gamma = w2 * invArea;

                finalColor = color;
                if (flags.shaded) {
                    finalColor = interpolateColor(verts[0].color, verts[1].color, verts[2].color, alpha, beta, gamma);
                }
                if (flags.textured) {
                    float u = alpha * verts[0].u + beta * verts[1].u + gamma * verts[2].u;
                    float v = alpha * verts[0].v + beta * verts[1].v + gamma * verts[2].v;

                    uint16_t texColor = sampleTexture(static_cast<uint8_t>(u), static_cast<uint8_t>(v), texInfo);

                    if (!texColor) {
                        continue;
                    }

                    if (!flags.rawTexture) {
                        uint16_t texR = (texColor & 0x1F) << 3;
                        uint16_t texG = ((texColor >> 5) & 0x1F) << 3;
                        uint16_t texB = ((texColor >> 10) & 0x1F) << 3;

                        finalColor.r = static_cast<uint8_t>((texR * finalColor.r) / 128);
                        finalColor.g = static_cast<uint8_t>((texG * finalColor.g) / 128);
                        finalColor.b = static_cast<uint8_t>((texB * finalColor.b) / 128);
                    } else {
                        finalColor.r = (texColor & 0x1F) << 3;
                        finalColor.g = ((texColor >> 5) & 0x1F) << 3;
                        finalColor.b = ((texColor >> 10) & 0x1F) << 3;
                    }
                }
                setPixel(p, finalColor.toABGR1555());
            }
        }
    }
}

void GPU::rasterizePoly4(const Vertex *verts, const ColorRGBA &color, const TextureInfo& texInfo)
{
    rasterizePoly3(verts, color, texInfo);
    rasterizePoly3(verts + 1, color, texInfo);
}

void GPU::rasterizeRectangle(const Vertex &vert, const Vec2i &size, const TextureInfo& texInfo)
{
    auto &flags = m_currentCmd.flags();
    uint16_t color = vert.color.toABGR1555();

    for (uint16_t y = 0; y < size.y; y++) {
        for (uint16_t x = 0; x < size.x; x++) {
            Vec2i pos{vert.pos.x + x, vert.pos.y + y};
            uint16_t pixelColor = color;

            if (flags.textured) {
                uint8_t u = static_cast<uint8_t>(vert.u + x);
                uint8_t v = static_cast<uint8_t>(vert.v + y);

                pixelColor = sampleTexture(u, v, texInfo);

                if (!pixelColor) {
                    continue;
                }

                if (!flags.rawTexture) {
                    uint8_t texR = (pixelColor & 0x1F) << 3;
                    uint8_t texG = ((pixelColor >> 5) & 0x1F) << 3;
                    uint8_t texB = ((pixelColor >> 10) & 0x1F) << 3;

                    texR = (texR * vert.color.r) >> 7;
                    texG = (texG * vert.color.g) >> 7;
                    texB = (texB * vert.color.b) >> 7;

                    pixelColor = 0x8000 | ((texB >> 3) << 10) | ((texG >> 3) << 5) | (texR >> 3);
                }
            }
            setPixel(pos, pixelColor);
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

uint16_t GPU::sampleTexture(uint8_t u, uint8_t v, const TextureInfo& texInfo)
{
    uint16_t texPageBaseY = texInfo.texPageY * 256;
    uint16_t texPageBaseX = texInfo.texPageX * 64;  // Base in VRAM 16-bit words

    switch (texInfo.colorMode) {
        case TexturePageColors::COL_4Bit: {
            uint16_t texelX = texPageBaseX + (u / 4);  // 4 texels per 16-bit word
            uint16_t texelY = texPageBaseY + v;
            uint16_t texData = getPixel({static_cast<int>(texelX), static_cast<int>(texelY)});
            uint8_t index = (texData >> ((u % 4) * 4)) & 0xF;
            return getPixel({texInfo.clutX + index, texInfo.clutY});
        }

        case TexturePageColors::COL_8Bit: {
            uint16_t texelX = texPageBaseX + (u / 2);  // 2 texels per 16-bit word
            uint16_t texelY = texPageBaseY + v;
            uint16_t texData = getPixel({static_cast<int>(texelX), static_cast<int>(texelY)});
            uint8_t index = (texData >> ((u % 2) * 8)) & 0xFF;
            return getPixel({texInfo.clutX + index, texInfo.clutY});
        }

        case TexturePageColors::COL_15Bit: {
            uint16_t texelX = texPageBaseX + u;
            uint16_t texelY = texPageBaseY + v;
            return getPixel({static_cast<int>(texelX), static_cast<int>(texelY)});
        }
        default:
            return 0x8000; // Transparent
    }
}
