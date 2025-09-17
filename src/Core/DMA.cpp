#include "DMA.hpp"

#include <spdlog/spdlog.h>

#include "MemoryMap.hpp"
#include "Bus.h"

#define GPU_GP0_ADDR 0x1F801810
#define GPU_GP1_ADDR 0x1F801814
#define GPU_GPUREAD_ADDR GPU_GP0_ADDR
#define GPU_GPUSTAT_ADDR GPU_GP1_ADDR

DMA::DMA(Bus *bus) :
    PsxDevice(bus)
{
    m_memoryRange = MemoryMap::DMA_REGISTERS_RANGE;
    reset();
}

DMA::~DMA()
{
}

void DMA::reset()
{
    m_dpcr = 0x07654321;
    m_dicr = 0;
}

void DMA::write8(uint8_t value, uint32_t address)
{
    spdlog::error("DMA: Write byte 0x{:02X} to 0x{:08X} is unsupported", value, address);
}

void DMA::write16(uint16_t value, uint32_t address)
{
    spdlog::error("DMA: Write halfword 0x{:04X} to 0x{:08X} is unsupported", value, address);
}

void DMA::write32(uint32_t value, uint32_t address)
{
    spdlog::debug("DMA: Write word 0x{:08X} to 0x{:08X}", value, address);

    uint32_t offset = m_memoryRange.remap(address);
    uint8_t channelIndex = (offset >> 4) & 0xF;
    uint8_t channelRegister = (offset & 0xF) >> 2; // Divide offset by 4 to get continuous register indexes

    if (channelIndex == 7) {
        writeCommonRegisters(channelRegister, value);
        return;
    }

    DMAChannel &channel = m_channels[channelIndex];
    channel.setRegister(static_cast<DMAChannelReg>(channelRegister), value);

    DMAChannelControl &chcr = channel.channelControl();
    if (chcr.forceTransferStart || chcr.transferStatus == DMATransferStatus::Started) {
        executeDmaTransfer(channelIndex);
        channel.channelControl().transferStatus = DMATransferStatus::Stopped;
        channel.channelControl().forceTransferStart = false;
    }
}

uint8_t DMA::read8(uint32_t address)
{
    spdlog::error("DMA: Read byte at 0x{:08X} is unsupported", address);
    return 0;
}

uint16_t DMA::read16(uint32_t address)
{
    spdlog::error("DMA: Read halfword at 0x{:08X} is unsupported", address);
    return 0;
}

uint32_t DMA::read32(uint32_t address)
{
    spdlog::debug("DMA: Read word at 0x{:08X}", address);

    uint32_t offset = m_memoryRange.remap(address);
    uint8_t channelIndex = (offset >> 4) & 0xF;
    uint8_t channelRegister = (offset & 0xF) >> 2; // Divide offset by 4 to get continuous register indexes

    if (channelIndex == 7) {
        return readCommonRegisters(channelRegister);
    }

    DMAChannel &channel = m_channels[channelIndex];
    return channel.getRegister(static_cast<DMAChannelReg>(channelRegister));
}

DMAChannel &DMA::getChannel(DMAChannelName channel)
{
    return m_channels[static_cast<uint8_t>(channel)];
}

const DMAChannel &DMA::getChannel(DMAChannelName channel) const
{
    return m_channels[static_cast<uint8_t>(channel)];
}

void DMA::writeCommonRegisters(uint8_t reg, uint32_t value)
{
    switch (reg)
    {
    case 0:
        m_dpcr = value;
        break;
    case 1:
        m_dicr = value;
        break;
    default:
        break;
    }
}

uint32_t DMA::readCommonRegisters(uint8_t reg)
{
    switch (reg)
    {
    case 0:
        return m_dpcr;
    case 1:
        return m_dicr;
    default:
        break;
    }
    return 0;
}

void DMA::executeDmaTransfer(uint8_t channel)
{
    auto channelEnum = static_cast<DMAChannelName>(channel);
    switch (channelEnum)
    {
    case DMAChannelName::OTC:
        executeDmaOT();
        break;
    case DMAChannelName::GPU:
        executeDmaGpu();
        break;
    default:
        spdlog::warn("Currently unsupported DMA transfer to channel {}", channel);
        break;
    }
}

void DMA::executeDmaOT()
{
    if (!m_bus) {
        spdlog::error("DMA: No bus reference!");
        return;
    }

    auto &channel = getChannel(DMAChannelName::OTC);
    uint16_t transferSize = channel.blockControl().block.count;
    uint32_t startAddr = channel.getRegister(DMAChannelReg::MemoryAddress) & OT_END_TAG;

    for (uint32_t i = transferSize - 1; i > 0; i--) {
        m_bus->storeWord(startAddr, startAddr - 4);
        startAddr -= 4;
    }

    m_bus->storeWord(startAddr, OT_END_TAG);
    channel.channelControl().transferStatus = DMATransferStatus::Stopped;
    channel.channelControl().forceTransferStart = false;
}

void DMA::executeDmaGpu()
{
    if (!m_bus) {
        spdlog::error("DMA: no bus reference");
        return;
    }
    auto syncMode = getChannel(DMAChannelName::GPU).channelControl().syncMode;

    switch (syncMode)
    {
    case DMASyncMode::LinkedList:
        executeDmaGpuLinkedList();
        break;
    case DMASyncMode::Slice:
        executeDmaGpuRequest();
        break;
    default:
        spdlog::error("DMA: GPU SyncMode not supported yet {}", static_cast<uint8_t>(syncMode));
        break;
    }
}

void DMA::executeDmaGpuLinkedList()
{
    if (!m_bus) {
        spdlog::error("DMA: no bus reference");
        return;
    }

    auto &channel = getChannel(DMAChannelName::GPU);
    uint32_t currentAddr = channel.getRegister(DMAChannelReg::MemoryAddress);
    bool transfer = true;

    while (transfer) {
        uint32_t currentPacket = m_bus->loadWord(currentAddr);
        uint8_t packetSize = currentPacket >> 24;

        for (uint8_t i = 0; i < packetSize; i++) {
            currentAddr += channel.channelControl().step == DMAStep::Increment ? 4 : -4;
            uint32_t command = m_bus->loadWord(currentAddr);
            m_bus->storeWord(0x1F801810, command);
            channel.setRegister(DMAChannelReg::MemoryAddress, currentAddr);
        }
        currentAddr = currentPacket & 0xFFFFFF;
        if (currentAddr == OT_END_TAG) {
            transfer = false;
        }
    }
}

// Normally, the sent data is split into blocks and only sent once DREQ is received
// but since I don't know what DREQ (Data REQuest obviously, but how and what it does)
// I'm going to transfer everything at once I guess
void DMA::executeDmaGpuRequest()
{
    if (!m_bus) {
        spdlog::error("DMA: No bus reference!");
        return;
    }
    auto &channel = getChannel(DMAChannelName::GPU);
    auto &blockControl = channel.blockControl();
    auto &channelControl = channel.channelControl();

    uint32_t transferSize = blockControl.block.size * blockControl.blockAmount;
    uint32_t startAddr = channel.getRegister(DMAChannelReg::MemoryAddress) & OT_END_TAG;
    int step = channelControl.step == DMAStep::Increment ? 4 : -4;
    // uint32_t srcAddr = channelControl.transferDir == DMATransferDirection::RamToDevice ? GPU_GP0_ADDR : startAddr;

    if (channelControl.transferDir == DMATransferDirection::RamToDevice) {
        for (uint32_t i = 0; i < transferSize; i++) {
            uint32_t word = m_bus->loadWord(startAddr);
            m_bus->storeWord(GPU_GP0_ADDR, word);
            startAddr += step;
            channel.setRegister(DMAChannelReg::MemoryAddress, startAddr);
        }
    } else {
        spdlog::error("DMA: GPU to RAM Request transfer mode not supported");
    }
    channelControl.transferStatus = DMATransferStatus::Stopped;
    channelControl.forceTransferStart = false;
}
