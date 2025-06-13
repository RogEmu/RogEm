#include "GPU.hpp"

#include <spdlog/spdlog.h>

#include "Bus.h"

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
    std::memset(&m_displayArea, 0, sizeof(m_displayArea));
    std::memset(&m_gpuStat, 0, sizeof(m_gpuStat));
    m_gpuStat.rdReceiveDmaBlock = true;
    m_gpuStat.rdReceiveCmd = true;
    m_gpuStat.displayEnable = DisplayEnable::Disabled;
    m_gpuStat.interlaceField = true;

    m_gpuStat.rdSendVram = true;

    m_gpuRead = 0;
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
        //handle GP0 commands
        spdlog::warn("GPU: GP0 command not suppported");
        break;
    case 0x1F801814:
        handleGP1Command(value);
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
        break;
    case 0x04:
        // Read Draw Area bottom right
        break;
    case 0x05:
        // Read Draw Offset
        break;
    case 0x07:
        m_gpuRead = 2;
        break;
    case 0x08:
        m_gpuRead = 0;
        break;
    default:
        break;
    }
}

void GPU::handleGP1Command(uint32_t cmd)
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
