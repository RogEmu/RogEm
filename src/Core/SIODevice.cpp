/*
** EPITECH PROJECT, 2026
** rogem
** File description:
** SIODevice
*/

#include "SIODevice.hpp"
#include "StateBuffer.hpp"

void SIODevice::serialize(StateBuffer &buf) const
{
    buf.write(m_stat);
    buf.write(m_mode);
    buf.write(m_ctrl);
    buf.write(m_baud);
    buf.write(m_baudTimer);
}

void SIODevice::deserialize(StateBuffer &buf)
{
    buf.read(m_stat);
    buf.read(m_mode);
    buf.read(m_ctrl);
    buf.read(m_baud);
    buf.read(m_baudTimer);
}
