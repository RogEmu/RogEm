/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** DMA
*/

#ifndef DMA_HPP_
#define DMA_HPP_

#include "PsxDevice.hpp"

#include "DMAChannel.hpp"

#define OT_END_TAG 0xFFFFFF

enum class DMAChannelName
{
    MDECin,
    MDECout,
    GPU,
    CDROM,
    SPU,
    PIO,
    OTC
};

class DMA : public PsxDevice
{
    public:
        DMA(Bus *bus);
        ~DMA();

        void reset();

        void write8(uint8_t value, uint32_t address) override;
        void write16(uint16_t value, uint32_t address) override;
        void write32(uint32_t value, uint32_t address) override;

        uint8_t read8(uint32_t address) override;
        uint16_t read16(uint32_t address) override;
        uint32_t read32(uint32_t address) override;

    private:
        DMAChannel &getChannel(DMAChannelName channel);
        const DMAChannel &getChannel(DMAChannelName channel) const;

        void writeCommonRegisters(uint8_t reg, uint32_t value);
        uint32_t readCommonRegisters(uint8_t reg);

        void executeDmaTransfer(uint8_t channel);
        void executeDmaOT();
        void executeDmaGpu();
        void executeDmaGpuLinkedList();
        void executeDmaGpuRequest();
        void executeDmaCdrom();

    private:
        DMAChannel m_channels[7];
        uint32_t m_dpcr;
        uint32_t m_dicr;
};

#endif /* !DMA_HPP_ */
