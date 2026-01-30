/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** MemoryMap
*/

#ifndef MEMORYMAP_HPP_
#define MEMORYMAP_HPP_

#include <cstdint>

namespace MemoryMap
{

struct MemRange
{
    uint32_t start;
    uint32_t length;

    bool contains(uint32_t addr) const
    {
        return (addr >= start && addr < start + length);
    }

    uint32_t remap(uint32_t addr) const
    {
        return addr - start;
    }
};

enum class MemorySegments : uint32_t
{
    KUSEG = 0x0,
    KSEG0 = 0x80000000,
    KSEG1 = 0xA0000000,
    KSEG2 = 0xFFFE0000
};

// All possible RAM base addresses
constexpr uint32_t RAM_BASE_KUSEG = 0x0;
constexpr uint32_t RAM_BASE_KSEG0 = 0x80000000;
constexpr uint32_t RAM_BASE_KSEG1 = 0xA0000000;

// All possible BIOS base addresses
constexpr uint32_t BIOS_BASE_KUSEG = 0x1FC00000;
constexpr uint32_t BIOS_BASE_KSEG0 = 0x9FC00000;
constexpr uint32_t BIOS_BASE_KSEG1 = 0xBFC00000;

// When adding address spaces, use physical addresses
// These ranges represent the top-level memory segments of the PS1
// Sub ranges/mappings are defined below
constexpr MemRange BIOS_RANGE =              {BIOS_BASE_KUSEG, 512 * 1024};
constexpr MemRange RAM_RANGE =               {RAM_BASE_KUSEG, 2048 * 1024};
constexpr MemRange SCRATCHPAD_RANGE =        {0x1F800000, 1024};
constexpr MemRange IO_PORTS_RANGE =          {0x1F801000, 4096};
constexpr MemRange EXP2_RANGE =              {0x1F802000, 8 * 1024};
constexpr MemRange CACHE_CONTROL_RANGE =     {0xFFFE0000, 512};

// I/O ports mappings within I/O ports range
constexpr MemRange MEMORY_CONTROL_1_RANGE =  {0x1F801000, 36};
constexpr MemRange SERIAL_PORT_RANGE =       {0x1F801040, 0x20};
constexpr MemRange MEMORY_CONTROL_2_RANGE =  {0x1F801060, 4};
constexpr MemRange INTERRUPT_CONTROL_RANGE = {0x1F801070, 0x8};
constexpr MemRange DMA_REGISTERS_RANGE =     {0x1F801080, 0x80};
constexpr MemRange TIMERS_RANGE =            {0x1F801100, 0x2F};
constexpr MemRange CDROM_REGISTERS_RANGE =   {0x1F801800, 0x4};
constexpr MemRange GPU_REGISTERS_RANGE =     {0x1F801810, 0x10};
constexpr MemRange SPU_CONTROL_REGS_RANGE =  {0x1F801C00, 0x200};

constexpr MemorySegments AddressSegments[] = {
    MemorySegments::KUSEG,
    MemorySegments::KUSEG,
    MemorySegments::KUSEG,
    MemorySegments::KUSEG,
    MemorySegments::KSEG0,
    MemorySegments::KSEG1,
    MemorySegments::KSEG2,
    MemorySegments::KSEG2,
};

uint32_t mapAddress(uint32_t addr);

};

#endif /* !MEMORYMAP_HPP_ */
