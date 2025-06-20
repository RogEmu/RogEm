#include "GPUCommand.hpp"

#include <spdlog/spdlog.h>

GPUCommand::GPUCommand()
{
}

GPUCommand::~GPUCommand()
{
}

void GPUCommand::setCommand(uint32_t cmd)
{
    uint8_t top = cmd >> 29;
    m_rawCmd = cmd;

    if (top == 0) {
        if ((cmd >> 24) == 0x02) {
            m_type = CommandType::QuickRectFill;
        } else {
            spdlog::error("GPU Command: Unknown command 0x{:08X}", cmd);
        }
        return;
    }
    m_type = static_cast<CommandType>(top);
}

CommandType GPUCommand::command() const
{
    return m_type;
}

void GPUCommand::addParam(uint32_t param)
{
    m_params.addParam(param);
}

const uint32_t *GPUCommand::params() const
{
    return m_params.data();
}

uint8_t GPUCommand::nbParams() const
{
    return m_params.size();
}

void GPUCommand::reset()
{
    m_params.clear();
    m_type = CommandType::None;
    m_rawCmd = 0;
}

uint32_t GPUCommand::raw() const
{
    return m_rawCmd;
}
