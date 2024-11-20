#include "Utils.h"

void loadImmediate(CPU &cpu, uint8_t reg, uint32_t value)
{
    Instruction i;

    i.i.rt = reg;
    i.i.immediate = value >> 16;
    cpu.loadUpperImmediate(i);
    i.i.immediate = value & 0xFFFF;
    i.i.rs = reg;
    cpu.orImmediateWord(i);
}
