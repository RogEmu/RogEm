/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** DMAChannel
*/

#ifndef DMACHANNEL_HPP_
#define DMACHANNEL_HPP_

#include <cstdint>

enum class DMASyncMode
{
    Burst,
    Slice,
    LinkedList,
    Reserved
};

enum class DMAStep
{
    Increment,
    Decrement
};

enum class DMATransferDirection
{
    DeviceToRam,
    RamToDevice
};

enum class DMATransferStatus
{
    Stopped,
    Started
};

struct DMABlockControl
{
    union Block
    {
        uint16_t count;
        uint16_t size;
    } block;
    uint16_t blockAmount;
};

struct DMAChannelControl
{
    DMATransferDirection transferDir;
    bool enableChop;
    DMAStep step;
    DMASyncMode syncMode;
    uint8_t chopDmaSize;
    uint8_t chopCpuSize;
    DMATransferStatus transferStatus;
    bool forceTransferStart;
};

enum class DMAChannelReg
{
    MemoryAddress,
    BlockControl,
    ChannelControl
};

class DMAChannel {
    public:
        DMAChannel();
        ~DMAChannel();

        void setRegister(DMAChannelReg reg, uint32_t value);
        uint32_t getRegister(DMAChannelReg reg) const;

        DMAChannelControl &channelControl();
        DMABlockControl &blockControl();

    private:
        void setChannelControl(uint32_t value);
        void setMemoryAddress(uint32_t address);
        void setBlockControl(uint32_t value);

        uint32_t channelControl() const;
        uint32_t memoryAddress() const;
        uint32_t blockControl() const;

    private:
        uint32_t m_memoryAddress;
        DMABlockControl m_blockControl;
        DMAChannelControl m_channelControl;
};

#endif /* !DMACHANNEL_HPP_ */
