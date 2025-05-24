/*
** EPITECH PROJECT, 2024
** RogEm
** File description:
** GTE
*/

#include "GTE.h"
#include <stdexcept>
#include <iostream>
#include <limits>
#include <spdlog/spdlog.h>

GTE::GTE() {
    reset();
}

void GTE::reset() {
    m_ctrlReg.fill(0);
    m_dataReg.fill(0);
}

uint32_t GTE::mfc(uint8_t reg) {
    if (reg >= 32) {
        spdlog::error("mfc error: Register index out of range");
        return 0;
    }
    return static_cast<uint32_t>(m_dataReg[reg]);
}

uint32_t GTE::cfc(uint8_t reg) {
    if (reg >= 32) {
        spdlog::error("cfc error: Register index out of range");
        return 0;
    }
    return static_cast<uint32_t>(m_ctrlReg[reg]);
}

void GTE::mtc(uint8_t reg, uint32_t value) {
    if (reg >= 32) {
        spdlog::error("mtc error: Register index out of range");
        return;
    }
    m_dataReg[reg] = static_cast<int32_t>(value);
}

void GTE::ctc(uint8_t reg, uint32_t value) {
    if (reg >= 32) {
        spdlog::error("ctc error: Register index out of range");
        return;
    }
    m_ctrlReg[reg] = static_cast<int32_t>(value);
}

void GTE::execute(uint32_t opcode) {
    decodeAndExecute(opcode);
}

void GTE::decodeAndExecute(uint32_t opcode) {
    GTEFunction funct = static_cast<GTEFunction>(opcode & 0x3F);

    switch (funct) {
        case GTEFunction::RTPS:
            executeRTPS();
            break;
        case GTEFunction::NCLIP:
            executeNCLIP();
            break;
        case GTEFunction::OP:
            spdlog::warn("[GTE] OP - Not implemented yet");
            break;
        case GTEFunction::DPCS:
            spdlog::warn("[GTE] DPCS - Not implemented yet");
            break;
        case GTEFunction::INTPL:
            spdlog::warn("[GTE] INTPL - Not implemented yet");
            break;
        case GTEFunction::MVMVA:
            spdlog::warn("[GTE] MVMVA - Not implemented yet");
            break;
        case GTEFunction::NCDS:
            spdlog::warn("[GTE] NCDS - Not implemented yet");
            break;
        case GTEFunction::CDP:
            spdlog::warn("[GTE] CDP - Not implemented yet");
            break;
        case GTEFunction::NCDT:
            spdlog::warn("[GTE] NCDT - Not implemented yet");
            break;
        case GTEFunction::NCCS:
            spdlog::warn("[GTE] NCCS - Not implemented yet");
            break;
        case GTEFunction::CC:
            spdlog::warn("[GTE] CC - Not implemented yet");
            break;
        case GTEFunction::NCS:
            spdlog::warn("[GTE] NCS - Not implemented yet");
            break;
        case GTEFunction::NCT:
            spdlog::warn("[GTE] NCT - Not implemented yet");
            break;
        case GTEFunction::SQR:
            spdlog::warn("[GTE] SQR - Not implemented yet");
            break;
        case GTEFunction::DCPL:
            spdlog::warn("[GTE] DCPL - Not implemented yet");
            break;
        case GTEFunction::DPCT:
            spdlog::warn("[GTE] DPCT - Not implemented yet");
            break;
        case GTEFunction::AVSZ3:
            spdlog::warn("[GTE] AVSZ3 - Not implemented yet");
            break;
        case GTEFunction::AVSZ4:
            spdlog::warn("[GTE] AVSZ4 - Not implemented yet");
            break;
        case GTEFunction::RTPT:
            executeRTPT();
            break;
        case GTEFunction::GPF:
            spdlog::warn("[GTE] GPF - Not implemented yet");
            break;
        case GTEFunction::GPL:
            spdlog::warn("[GTE] GPL - Not implemented yet");
            break;
        case GTEFunction::NCCT:
            spdlog::warn("[GTE] NCCT - Not implemented yet");
            break;
        default:
            spdlog::error("[GTE] Unimplemented or unknown instruction function: 0x{:02X}", opcode & 0x3F);
            break;
    }
}

void GTE::executeRTPS() {
    // Read V0 (input vector)
    int16_t vx = extractSigned16(m_dataReg[0], false); // VXY0 low
    int16_t vy = extractSigned16(m_dataReg[0], true);  // VXY0 high
    int16_t vz = extractSigned16(m_dataReg[1], false); // VZ0 low only

    // Read rotation matrix R (fixed-point 12.20 format)
    int32_t r11 = static_cast<int16_t>(m_ctrlReg[0] >> 16);
    int32_t r12 = static_cast<int16_t>(m_ctrlReg[0] & 0xFFFF);
    int32_t r13 = static_cast<int16_t>(m_ctrlReg[1] >> 16);

    int32_t r21 = static_cast<int16_t>(m_ctrlReg[1] & 0xFFFF);
    int32_t r22 = static_cast<int16_t>(m_ctrlReg[2] >> 16);
    int32_t r23 = static_cast<int16_t>(m_ctrlReg[2] & 0xFFFF);

    int32_t r31 = static_cast<int16_t>(m_ctrlReg[3] >> 16);
    int32_t r32 = static_cast<int16_t>(m_ctrlReg[3] & 0xFFFF);
    int32_t r33 = static_cast<int16_t>(m_ctrlReg[4] >> 16);

    // Translation vector T
    int32_t trx = m_ctrlReg[5];
    int32_t try_ = m_ctrlReg[6];
    int32_t trz = m_ctrlReg[7];

    // Perspective projection parameters
    int32_t ofx = m_ctrlReg[24];
    int32_t ofy = m_ctrlReg[25];
    int32_t h = m_ctrlReg[26];

    // Perform matrix * vector (with >>12 shift for fixed point)
    int64_t mac1 = ((int64_t)r11 * vx + (int64_t)r12 * vy + (int64_t)r13 * vz) >> 12;
    int64_t mac2 = ((int64_t)r21 * vx + (int64_t)r22 * vy + (int64_t)r23 * vz) >> 12;
    int64_t mac3 = ((int64_t)r31 * vx + (int64_t)r32 * vy + (int64_t)r33 * vz) >> 12;

    // Add translation vector
    mac1 += trx;
    mac2 += try_;
    mac3 += trz;

    // Clamp MACs to 32-bit signed int (we emulate overflows in IR later)
    m_dataReg[25] = static_cast<int32_t>(mac1); // MAC1
    m_dataReg[26] = static_cast<int32_t>(mac2); // MAC2
    m_dataReg[27] = static_cast<int32_t>(mac3); // MAC3

    // Clamp MACs into IR1-IR3 with overflow/underflow detection
    m_dataReg[9] = clampMAC(mac1, IR_LIMIT_HIGH, IR_LIMIT_LOW, 0x0001); // IR1
    m_dataReg[10] = clampMAC(mac2, IR_LIMIT_HIGH, IR_LIMIT_LOW, 0x0002); // IR2
    m_dataReg[11] = clampMAC(mac3, IR_LIMIT_HIGH, IR_LIMIT_LOW, 0x0004); // IR3

    // Perspective divide using IR3 as Z
    int32_t z = m_dataReg[11];
    int32_t zc = (z > 0) ? z : 1; // Avoid divide by 0 or negative
    int32_t sx = ((m_dataReg[9] * h) / zc + ofx) & 0xFFFF;
    int32_t sy = ((m_dataReg[10] * h) / zc + ofy) & 0xFFFF;

    // Output to screen XY register SXY2
    m_dataReg[14] = (sy << 16) | (sx & 0xFFFF);

    // Depth register SZ3 and OTZ
    m_dataReg[19] = z;             // SZ3
    m_dataReg[7] = z & 0xFFFF;     // OTZ (used for ordering table)
}

void GTE::executeRTPT() {
    // Process V0, V1, V2 in sequence
    const int vectorRegs[3][2] = { {0, 1}, {2, 3}, {4, 5} }; // VXY0/VZ0, VXY1/VZ1, VXY2/VZ2
    const int sxyRegs[3] = { 12, 13, 14 }; // SXY0, SXY1, SXY2
    const int szRegs[3] = { 16, 17, 18 };  // SZ1, SZ2, SZ3

    int32_t ofx = m_ctrlReg[24];
    int32_t ofy = m_ctrlReg[25];
    int32_t h = m_ctrlReg[26];

    int32_t r11 = static_cast<int16_t>(m_ctrlReg[0] >> 16);
    int32_t r12 = static_cast<int16_t>(m_ctrlReg[0] & 0xFFFF);
    int32_t r13 = static_cast<int16_t>(m_ctrlReg[1] >> 16);
    int32_t r21 = static_cast<int16_t>(m_ctrlReg[1] & 0xFFFF);
    int32_t r22 = static_cast<int16_t>(m_ctrlReg[2] >> 16);
    int32_t r23 = static_cast<int16_t>(m_ctrlReg[2] & 0xFFFF);
    int32_t r31 = static_cast<int16_t>(m_ctrlReg[3] >> 16);
    int32_t r32 = static_cast<int16_t>(m_ctrlReg[3] & 0xFFFF);
    int32_t r33 = static_cast<int16_t>(m_ctrlReg[4] >> 16);

    int32_t trx = m_ctrlReg[5];
    int32_t try_ = m_ctrlReg[6];
    int32_t trz = m_ctrlReg[7];

    for (int i = 0; i < 3; ++i) {
        int16_t vx = extractSigned16(m_dataReg[vectorRegs[i][0]], false);
        int16_t vy = extractSigned16(m_dataReg[vectorRegs[i][0]], true);
        int16_t vz = extractSigned16(m_dataReg[vectorRegs[i][1]], false);

        int64_t mac1 = ((int64_t)r11 * vx + (int64_t)r12 * vy + (int64_t)r13 * vz) >> 12;
        int64_t mac2 = ((int64_t)r21 * vx + (int64_t)r22 * vy + (int64_t)r23 * vz) >> 12;
        int64_t mac3 = ((int64_t)r31 * vx + (int64_t)r32 * vy + (int64_t)r33 * vz) >> 12;

        mac1 += trx;
        mac2 += try_;
        mac3 += trz;

        m_dataReg[25] = static_cast<int32_t>(mac1); // MAC1
        m_dataReg[26] = static_cast<int32_t>(mac2); // MAC2
        m_dataReg[27] = static_cast<int32_t>(mac3); // MAC3

        m_dataReg[9] = clampMAC(mac1, IR_LIMIT_HIGH, IR_LIMIT_LOW, 0x0001); // IR1
        m_dataReg[10] = clampMAC(mac2, IR_LIMIT_HIGH, IR_LIMIT_LOW, 0x0002); // IR2
        m_dataReg[11] = clampMAC(mac3, IR_LIMIT_HIGH, IR_LIMIT_LOW, 0x0004); // IR3

        int32_t z = m_dataReg[11];
        int32_t zc = (z > 0) ? z : 1;
        int32_t sx = ((m_dataReg[9] * h) / zc + ofx) & 0xFFFF;
        int32_t sy = ((m_dataReg[10] * h) / zc + ofy) & 0xFFFF;

        m_dataReg[sxyRegs[i]] = (sy << 16) | (sx & 0xFFFF);
        m_dataReg[szRegs[i]] = z;
    }

    // Set SZ3 and OTZ to final Z
    m_dataReg[19] = m_dataReg[18]; // SZ3 = last depth
    m_dataReg[7] = m_dataReg[18] & 0xFFFF; // OTZ
}

void GTE::executeNCLIP()
{
    // get r12, r13 and 14
    int32_t r12 = m_dataReg[12];
    int32_t r13 = m_dataReg[13];
    int32_t r14 = m_dataReg[14];

    // extract x and y from r12, r13 and r14
    int16_t sx0 = static_cast<int16_t>(r12 & 0xFFFF);
    int16_t sy0 = static_cast<int16_t>(r12 >> 16);
    int16_t sx1 = static_cast<int16_t>(r13 & 0xFFFF);
    int16_t sy1 = static_cast<int16_t>(r13 >> 16);
    int16_t sx2 = static_cast<int16_t>(r14 & 0xFFFF);
    int16_t sy2 = static_cast<int16_t>(r14 >> 16);

    int64_t mac0 = static_cast<int64_t>(sx0) * sy1 +
                    static_cast<int64_t>(sx1) * sy2 +
                    static_cast<int64_t>(sx2) * sy0 -
                    static_cast<int64_t>(sx0) * sy2 -
                    static_cast<int64_t>(sx1) * sy0 -
                    static_cast<int64_t>(sx2) * sy1;

    // Store in MAC0
    m_dataReg[24] = static_cast<int32_t>(mac0);  // MAC0
}

int16_t GTE::extractSigned16(uint32_t value, bool upper) {
    return static_cast<int16_t>(upper ? (value >> 16) : (value & 0xFFFF));
}

int32_t GTE::clampMAC(int64_t value, int limitHigh, int limitLow, uint32_t flagBit) {
    if (value > limitHigh) {
        m_ctrlReg[31] |= flagBit; // Set overflow flag bit
        return limitHigh;
    }
    if (value < limitLow) {
        m_ctrlReg[31] |= flagBit; // Set underflow flag bit
        return limitLow;
    }
    return static_cast<int32_t>(value);
}

void GTE::updateFlags(int32_t value, int limitHigh, int limitLow, uint32_t flagBit) {
    if (value > limitHigh || value < limitLow) {
        m_ctrlReg[31] |= flagBit; // Set overflow or underflow bit in FLAG register
    }
}
