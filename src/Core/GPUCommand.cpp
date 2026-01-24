#include "GPUCommand.hpp"

#include <spdlog/spdlog.h>

GPUCommand::GPUCommand()
{
}

GPUCommand::~GPUCommand()
{
}

void GPUCommand::set(uint32_t cmd)
{
    parseCommandType(cmd);
    parseRawCommand(cmd);
}

void GPUCommand::addParam(uint32_t param)
{
    m_params.addParam(param);
}

static GPUCommandFlags parsePolygonArgs(uint32_t cmd)
{
    GPUCommandFlags flags{};
    flags.shaded = ((cmd >> 28) & 1) != 0;
    flags.nbVertices = ((cmd >> 27) & 1) == 0 ? 3 : 4;
    flags.textured = ((cmd >> 26) & 1) != 0;
    flags.semiTransparent = ((cmd >> 25) & 1) != 0;
    flags.rawTexture = ((cmd >> 24) & 1) != 0;
    return flags;
}

static GPUCommandFlags parseLineArgs(uint32_t cmd)
{
    GPUCommandFlags flags{};
    flags.shaded = ((cmd >> 28) & 1) != 0;
    flags.semiTransparent = ((cmd >> 25) & 1) != 0;
    flags.polyline = ((cmd >> 24) & 1) != 0;
    return flags;
}

static GPUCommandFlags parseRectangleArgs(uint32_t cmd)
{
    GPUCommandFlags flags{};
    flags.rectFlag = static_cast<GPURectSize>((cmd >> 27) & 3);
    flags.textured = ((cmd >> 26) & 1) != 0;
    flags.semiTransparent = ((cmd >> 25) & 1) != 0;
    flags.rawTexture = ((cmd >> 24) & 1) != 0;

    switch (flags.rectFlag) {
        case GPURectSize::Variable:
            flags.rectSize = {0, 0};
            break;
        case GPURectSize::Size_1x1:
            flags.rectSize = {1, 1};
            break;
        case GPURectSize::Size_8x8:
            flags.rectSize = {8, 8};
            break;
        case GPURectSize::Size_16x16:
            flags.rectSize = {16, 16};
            break;
        default:
            break;
    }
    return flags;
}

void GPUCommand::parseRawCommand(uint32_t cmd)
{
    switch (m_type)
    {
        case GPUCommandType::DrawPolygon:
        {
            m_flags = parsePolygonArgs(cmd);
            m_nbExpectedParams = m_flags.nbVertices * (1 + m_flags.shaded + m_flags.textured) - m_flags.shaded + 1;
            m_params.addParam(cmd & 0xFFFFFF);
            break;
        }
        case GPUCommandType::DrawLine:
        {
            m_flags = parseLineArgs(cmd);
            m_nbExpectedParams = m_flags.polyline ? -1 : (m_flags.shaded ? 4 : 3);
            m_params.addParam(cmd & 0xFFFFFF);
            break;
        }
        case GPUCommandType::DrawRectangle:
        {
            m_flags = parseRectangleArgs(cmd);
            m_nbExpectedParams = 2 + (m_flags.rectFlag == GPURectSize::Variable) + m_flags.textured;
            m_params.addParam(cmd & 0xFFFFFF);
            break;
        }
        case GPUCommandType::CpuVramCopy:
            m_nbExpectedParams = 2;
            break;
        case GPUCommandType::VramVramCopy:
            m_nbExpectedParams = 3;
            break;
        case GPUCommandType::QuickRectFill:
            m_nbExpectedParams = 3;
            m_params.addParam(cmd & 0xFFFFFF);
            break;
        default:
            spdlog::error("GPU Command: Parsing of unsupported command 0x{:08X}", cmd);
            break;
    }
}

static GPUCommandType parseMiscCommandType(uint32_t cmd)
{
    switch ((cmd >> 24) & 0xFF) {
        case 0x00:
            return GPUCommandType::NOP;
        case 0x01:
            return GPUCommandType::ClearCache;
        case 0x02:
            return GPUCommandType::QuickRectFill;
        default:
            spdlog::error("GPU Command: Unknown command 0x{:08X}", cmd);
            return GPUCommandType::None;
    }
}

void GPUCommand::parseCommandType(uint32_t cmd)
{
    uint8_t top = cmd >> 29;

    if (top == 0) {
        m_type = parseMiscCommandType(cmd);
        return;
    }
    m_type = static_cast<GPUCommandType>(top);
}

void GPUCommand::reset()
{
    m_params.clear();
    m_type = GPUCommandType::None;
    m_flags = {};
    m_nbExpectedParams = 0;
}
