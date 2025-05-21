#include "SystemControlCop.hpp"

#include <cstring>

SystemControlCop::SystemControlCop() :
    Coprocessor()
{
    mtc(15, 0x00000002); // Set the Processor ID of the playstation
}

SystemControlCop::~SystemControlCop()
{
}

void SystemControlCop::reset()
{
    std::memset(m_regs, 0, sizeof(m_regs) - sizeof(m_regs[0]));
}

void SystemControlCop::execute(uint32_t opcode)
{
    (void)opcode;
}

uint32_t SystemControlCop::mfc(uint8_t reg)
{
    if (reg >= COP0_NB_REG) {
        return 0;
    }
    return m_regs[reg];
}

void SystemControlCop::mtc(uint8_t reg, uint32_t value)
{
    if (reg >= COP0_NB_REG) {
        return;
    }
    m_regs[reg] = value;
}

uint32_t SystemControlCop::cfc(uint8_t reg)
{
    (void)reg;
    return 0;
}

void SystemControlCop::ctc(uint8_t reg, uint32_t value)
{
    (void)reg;
    (void)value;
}
