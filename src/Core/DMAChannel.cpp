#include "DMAChannel.hpp"
#include "StateBuffer.hpp"

#include <cstring>

DMAChannel::DMAChannel()
{
    std::memset(&m_blockControl, 0, sizeof(m_blockControl));
    std::memset(&m_channelControl, 0, sizeof(m_channelControl));
    m_memoryAddress = 0;
}

DMAChannel::~DMAChannel()
{
}

void DMAChannel::setRegister(DMAChannelReg reg, uint32_t value)
{
    switch (reg)
    {
    case DMAChannelReg::MemoryAddress:
        setMemoryAddress(value);
        break;
    case DMAChannelReg::BlockControl:
        setBlockControl(value);
        break;
    case DMAChannelReg::ChannelControl:
        setChannelControl(value);
    default:
        break;
    }
}

uint32_t DMAChannel::getRegister(DMAChannelReg reg) const
{
    switch (reg)
    {
    case DMAChannelReg::MemoryAddress:
        return memoryAddress();
    case DMAChannelReg::BlockControl:
        return blockControl();
    case DMAChannelReg::ChannelControl:
        return channelControl();
    default:
        break;
    }
    return 0;
}

uint32_t DMAChannel::channelControl() const
{
    uint32_t result = 0;

    result |= static_cast<uint32_t>(m_channelControl.transferDir);
    result |= static_cast<uint32_t>(m_channelControl.step) << 1;
    result |= static_cast<uint32_t>(m_channelControl.enableChop) << 8;
    result |= static_cast<uint32_t>(m_channelControl.syncMode) << 9;
    result |= static_cast<uint32_t>(m_channelControl.chopDmaSize) << 16;
    result |= static_cast<uint32_t>(m_channelControl.chopCpuSize) << 20;
    result |= static_cast<uint32_t>(m_channelControl.transferStatus) << 24;
    result |= static_cast<uint32_t>(m_channelControl.forceTransferStart) << 28;
    // I don't know what to do with bits 29-30

    return result;
}

uint32_t DMAChannel::memoryAddress() const
{
    return m_memoryAddress;
}

uint32_t DMAChannel::blockControl() const
{
    uint32_t res = (m_blockControl.blockAmount << 16) | m_blockControl.block.count;
    return res;
}

DMAChannelControl &DMAChannel::channelControl()
{
    return m_channelControl;
}

DMABlockControl &DMAChannel::blockControl()
{
    return m_blockControl;
}

void DMAChannel::setChannelControl(uint32_t value)
{
    m_channelControl.transferDir = static_cast<DMATransferDirection>(value & 1);
    m_channelControl.step = static_cast<DMAStep>((value >> 1) & 1);
    m_channelControl.enableChop = (value >> 8) & 1;
    m_channelControl.syncMode = static_cast<DMASyncMode>((value >> 9) & 3);
    m_channelControl.chopDmaSize = (value >> 16) & 7;
    m_channelControl.chopCpuSize = (value >> 20) & 7;
    m_channelControl.transferStatus = static_cast<DMATransferStatus>((value >> 24) & 1);
    m_channelControl.forceTransferStart = (value >> 28) & 1;
}

void DMAChannel::setMemoryAddress(uint32_t address)
{
    m_memoryAddress = address & 0xFFFFFF;
}

void DMAChannel::setBlockControl(uint32_t value)
{
    m_blockControl.block.count = value & 0xFFFF;
    m_blockControl.blockAmount = (value >> 16) & 0xFFFF;
}

void DMAChannel::serialize(StateBuffer &buf) const
{
    buf.write(m_memoryAddress);
    buf.write(m_blockControl);
    buf.write(m_channelControl);
}

void DMAChannel::deserialize(StateBuffer &buf)
{
    buf.read(m_memoryAddress);
    buf.read(m_blockControl);
    buf.read(m_channelControl);
}
