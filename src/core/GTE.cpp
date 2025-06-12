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
#include <algorithm>
#include <spdlog/spdlog.h>
#include <fmt/format.h>

/**
 * @brief Constructs the GTE coprocessor and initializes registers.
 */
GTE::GTE() {
    reset();
}

/**
 * @brief Resets all control and data registers of the GTE to 0.
 */
void GTE::reset() {
    m_ctrlReg.fill(0);
    m_dataReg.fill(0);
}

/**
 * @brief Moves a value from the GTE data register to the CPU (MFC2).
 * @param reg Index of the data register (0–31).
 * @return uint32_t The current value of the data register.
 */
uint32_t GTE::mfc(uint8_t reg) {
    if (reg >= 32) {
        spdlog::error("mfc error: Register index out of range");
        return 0;
    }
    return static_cast<uint32_t>(m_dataReg[reg]);
}

/**
 * @brief Moves a value from the GTE control register to the CPU (CFC2).
 *
 * @param reg Index of the control register (0–31).
 * @return uint32_t The current value of the control register.
 */
uint32_t GTE::cfc(uint8_t reg) {
    if (reg >= 32) {
        spdlog::error("cfc error: Register index out of range");
        return 0;
    }
    return static_cast<uint32_t>(m_ctrlReg[reg]);
}

/**
 * @brief Moves a value from the CPU to a GTE data register (MTC2).
 *
 * @param reg Index of the data register (0–31).
 * @param value 32-bit value to store.
 */
void GTE::mtc(uint8_t reg, uint32_t value) {
    if (reg >= 32) {
        spdlog::error("mtc error: Register index out of range");
        return;
    }
    m_dataReg[reg] = static_cast<int32_t>(value);
}

/**
 * @brief Moves a value from the CPU to a GTE control register (CTC2).
 *
 * @param reg Index of the control register (0–31).
 * @param value 32-bit value to store.
 */
void GTE::ctc(uint8_t reg, uint32_t value) {
    if (reg >= 32) {
        spdlog::error("ctc error: Register index out of range");
        return;
    }
    m_ctrlReg[reg] = static_cast<int32_t>(value);
}

/**
 * @brief Executes a GTE instruction by decoding its function code.
 * @param opcode Full 32-bit encoded GTE instruction.
 */
void GTE::execute(uint32_t opcode) {
    decodeAndExecute(opcode);
}

/**
 * @brief Decodes the GTE instruction and dispatches to the appropriate handler.
 * @param opcode The 32-bit instruction word.
 */
void GTE::decodeAndExecute(uint32_t opcode) {
    GTEFunction funct = static_cast<GTEFunction>(opcode & 0x3F);

    switch (funct) {
        case GTEFunction::RTPS:
            std::cout << "[GTE] RTPS - Need to be redone\n";
            break;
        case GTEFunction::NCLIP:
            executeNCLIP();
            break;
        case GTEFunction::OP:{
            bool sf = (opcode >> 19) & 0x1;
            executeOP(sf);
            break;
        }
        case GTEFunction::DPCS:
            spdlog::warn("[GTE] DPCS - Not implemented yet");
            break;
        case GTEFunction::INTPL:
            spdlog::warn("[GTE] INTPL - Not implemented yet");
            break;
        case GTEFunction::MVMVA:
            executeMVMVA(opcode);
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
        case GTEFunction::SQR: {
            bool sf = (opcode >> 19) & 0x1;
            executeSQR(sf);
            break;
        }
        case GTEFunction::DCPL:
            spdlog::warn("[GTE] DCPL - Not implemented yet");
            break;
        case GTEFunction::DPCT:
            spdlog::warn("[GTE] DPCT - Not implemented yet");
            break;
        case GTEFunction::AVSZ3:
            executeAVSZ3();
            break;
        case GTEFunction::AVSZ4:
            executeAVSZ4();
            break;
        case GTEFunction::RTPT:
            std::cout << "[GTE] RTPT - Need to be redone\n";
            break;
        case GTEFunction::GPF:
            executeGPF(opcode);
            break;
        case GTEFunction::GPL:
            executeGPL(opcode, true);
            break;
        case GTEFunction::NCCT:
            spdlog::warn("[GTE] NCCT - Not implemented yet");
            break;
        default:
            spdlog::error("[GTE] Unimplemented or unknown instruction function: 0x{:02X}", opcode & 0x3F);
            break;
    }
}

/* OBSOLETE TO REDO
void GTE::executeRTPS() {
    // Read V0 (input vector)
    int16_t vx = extractSigned16(m_dataReg[0], false); // VXY0 low
    int16_t vy = extractSigned16(m_dataReg[0], true);  // VXY0 high
    int16_t vz = extractSigned16(m_dataReg[1], false); // VZ0 low only

    // Read rotation Mat3x3 R (fixed-point 12.20 format)
    int32_t r11 = static_cast<int16_t>(m_ctrlReg[0] >> 16) << 4;
    int32_t r12 = static_cast<int16_t>(m_ctrlReg[0] & 0xFFFF) << 4;
    int32_t r13 = static_cast<int16_t>(m_ctrlReg[1] >> 16) << 4;

    int32_t r21 = static_cast<int16_t>(m_ctrlReg[1] & 0xFFFF) << 4;
    int32_t r22 = static_cast<int16_t>(m_ctrlReg[2] >> 16) << 4;
    int32_t r23 = static_cast<int16_t>(m_ctrlReg[2] & 0xFFFF) << 4;

    int32_t r31 = static_cast<int16_t>(m_ctrlReg[3] >> 16) << 4;
    int32_t r32 = static_cast<int16_t>(m_ctrlReg[3] & 0xFFFF) << 4;
    int32_t r33 = static_cast<int16_t>(m_ctrlReg[4] >> 16) << 4;

    // Translation vector T
    int32_t trx = m_ctrlReg[5];
    int32_t try_ = m_ctrlReg[6];
    int32_t trz = m_ctrlReg[7];

    // Perspective projection parameters
    int32_t ofx = m_ctrlReg[24];
    int32_t ofy = m_ctrlReg[25];
    int32_t h = m_ctrlReg[26];

    // Perform Mat3x3 * vector (with >>12 shift for fixed point)
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
    int32_t sx = ((m_dataReg[9] * h) / zc + ofx);
    int32_t sy = ((m_dataReg[10] * h) / zc + ofy);

    sx = std::clamp(sx, 0, 0xFFFF);
    sy = std::clamp(sy, 0, 0xFFFF);

    // Output to screen XY register SXY2
    m_dataReg[14] = (sy << 16) | (sx & 0xFFFF);

    // Depth register SZ3 and OTZ
    m_dataReg[19] = z;             // SZ3
    m_dataReg[7] = z & 0xFFFF;     // OTZ (used for ordering table)
}

void GTE::executeRTPT()
{
    // Process V0, V1, V2 in sequence
    const int vectorRegs[3][2] = { {0, 1}, {2, 3}, {4, 5} }; // VXY0/VZ0, VXY1/VZ1, VXY2/VZ2
    const int sxyRegs[3] = { 12, 13, 14 }; // SXY0, SXY1, SXY2
    const int szRegs[3] = { 16, 17, 18 };  // SZ1, SZ2, SZ3

    int32_t ofx = m_ctrlReg[24];
    int32_t ofy = m_ctrlReg[25];
    int32_t h = m_ctrlReg[26];

    int32_t r11 = static_cast<int16_t>(m_ctrlReg[0] >> 16) << 4;
    int32_t r12 = static_cast<int16_t>(m_ctrlReg[0] & 0xFFFF) << 4;
    int32_t r13 = static_cast<int16_t>(m_ctrlReg[1] >> 16) << 4;
    int32_t r21 = static_cast<int16_t>(m_ctrlReg[1] & 0xFFFF) << 4;
    int32_t r22 = static_cast<int16_t>(m_ctrlReg[2] >> 16) << 4;
    int32_t r23 = static_cast<int16_t>(m_ctrlReg[2] & 0xFFFF) << 4;
    int32_t r31 = static_cast<int16_t>(m_ctrlReg[3] >> 16) << 4;
    int32_t r32 = static_cast<int16_t>(m_ctrlReg[3] & 0xFFFF) << 4;
    int32_t r33 = static_cast<int16_t>(m_ctrlReg[4] >> 16) << 4;

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
        int32_t sx = ((m_dataReg[9] * h) / zc + ofx);
        int32_t sy = ((m_dataReg[10] * h) / zc + ofy);

        sx = std::clamp(sx, 0, 0xFFFF);
        sy = std::clamp(sy, 0, 0xFFFF);

        m_dataReg[sxyRegs[i]] = (sy << 16) | (sx & 0xFFFF);
        m_dataReg[szRegs[i]] = z;
    }

    // Set SZ3 and OTZ to final Z
    m_dataReg[19] = m_dataReg[18]; // SZ3 = last depth
    m_dataReg[7] = m_dataReg[18] & 0xFFFF; // OTZ
}
    */

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

/**
 * @brief Executes the OP (Cross Product) GTE instruction.
 *
 * This instruction calculates the 3D cross product between two vectors:
 * - The first vector is read from the intermediate registers IR1, IR2, IR3
 * - The second vector uses the diagonal elements of the rotation matrix:
 *     D1 = RT11 (ctrlReg[0] upper 16 bits)
 *     D2 = RT22 (ctrlReg[2] upper 16 bits)
 *     D3 = RT33 (ctrlReg[4] upper 16 bits)
 *   Although these are matrix elements, here they're "misused" as a vector.
 *
 * The formula used is the standard cross product:
 *   MAC1 = IR3 * D2 - IR2 * D3
 *   MAC2 = IR1 * D3 - IR3 * D1
 *   MAC3 = IR2 * D1 - IR1 * D2
 *
 * After calculating the MAC values, the result is scaled depending on the 'sf' flag:
 * - If sf == 1, the values are shifted right by 12 bits (SAR 12)
 *   This operation preserves the sign of negative numbers (arithmetic shift)
 * - If sf == 0, the full values are kept without shifting
 *
 * The final values (MAC1–MAC3) are:
 * - Stored in the MAC registers (m_dataReg[25–27])
 * - Clamped to 16-bit signed range and stored in IR1–IR3 (m_dataReg[9–11])
 */
void GTE::executeOP(bool sf) {
    // Read IR1, IR2, IR3
    int32_t ir1 = m_dataReg[9];
    int32_t ir2 = m_dataReg[10];
    int32_t ir3 = m_dataReg[11];

    // Read D1, D2, D3 from RT11, RT22, RT33
    int16_t d1 = static_cast<int16_t>(m_ctrlReg[0] >> 16); // RT11
    int16_t d2 = static_cast<int16_t>(m_ctrlReg[2] >> 16); // RT22
    int16_t d3 = static_cast<int16_t>(m_ctrlReg[4] >> 16); // RT33

    // Cross product computation
    int64_t mac1 = (static_cast<int64_t>(ir3) * d2 - static_cast<int64_t>(ir2) * d3) >> (sf * 12);
    int64_t mac2 = (static_cast<int64_t>(ir1) * d3 - static_cast<int64_t>(ir3) * d1) >> (sf * 12);
    int64_t mac3 = (static_cast<int64_t>(ir2) * d1 - static_cast<int64_t>(ir1) * d2) >> (sf * 12);

    // Store to MACs
    m_dataReg[25] = static_cast<int32_t>(mac1);
    m_dataReg[26] = static_cast<int32_t>(mac2);
    m_dataReg[27] = static_cast<int32_t>(mac3);

    // Clamp and store to IRs
    m_dataReg[9]  = clampMAC(mac1, IR_LIMIT_HIGH, IR_LIMIT_LOW, 0x0001, 0x0001); // IR1
    m_dataReg[10] = clampMAC(mac2, IR_LIMIT_HIGH, IR_LIMIT_LOW, 0x0002, 0x0002); // IR2
    m_dataReg[11] = clampMAC(mac3, IR_LIMIT_HIGH, IR_LIMIT_LOW, 0x0004, 0x0004); // IR3
}

/**
 * @brief Executes the SQR (Square Vector) GTE instruction.
 *
 * This instruction takes IR1, IR2, and IR3, squares them,
 * and stores the results into MAC1, MAC2, and MAC3.
 *
 * Then, depending on the value of the 'sf' bit (Shift Fraction flag),
 * the function optionally divides each result by 4096 (which is 2^12).
 *
 * The sf flag is found at bit 19 of the 32-bit GTE instruction.
 * - If sf == 0: keep full precision, do not shift
 * - If sf == 1: divide results by 4096 using a bitwise right shift (>> 12)
 *   This scaling is used to convert from fixed-point to normal-sized integers.
 *
 * After this, each result is clamped to a signed 16-bit integer range (-32768 to 32767)
 * and stored back into the IR1, IR2, and IR3 registers.
 */
void GTE::executeSQR(bool sf) {
    // Read input vector
    int32_t ir1 = m_dataReg[9];
    int32_t ir2 = m_dataReg[10];
    int32_t ir3 = m_dataReg[11];

    // Calculate MACs
    int64_t mac1 = (static_cast<int64_t>(ir1) * ir1) >> (sf * 12);
    int64_t mac2 = (static_cast<int64_t>(ir2) * ir2) >> (sf * 12);
    int64_t mac3 = (static_cast<int64_t>(ir3) * ir3) >> (sf * 12);

    // Store MACs
    m_dataReg[25] = static_cast<int32_t>(mac1);
    m_dataReg[26] = static_cast<int32_t>(mac2);
    m_dataReg[27] = static_cast<int32_t>(mac3);

    // Clamp and store in IRs
    m_dataReg[9]  = clampMAC(mac1, IR_LIMIT_HIGH, IR_LIMIT_LOW, 0x0001, 0x0001); // IR1
    m_dataReg[10] = clampMAC(mac2, IR_LIMIT_HIGH, IR_LIMIT_LOW, 0x0002, 0x0002); // IR2
    m_dataReg[11] = clampMAC(mac3, IR_LIMIT_HIGH, IR_LIMIT_LOW, 0x0004, 0x0002); // IR3
}

Mat3x3 GTE::getMat3x3FromMX(uint8_t mx)
{
    Mat3x3 m;
    if (mx == 3)
    {
        m.r11 = -0x60;
        m.r12 = 0x60;
        m.r13 = extractSigned16(m_dataReg[8], false);
        m.r21 = extractSigned16(m_ctrlReg[1], true);
        m.r22 = extractSigned16(m_ctrlReg[1], true);
        m.r23 = extractSigned16(m_ctrlReg[1], true);
        m.r31 = extractSigned16(m_ctrlReg[2], true);
        m.r32 = extractSigned16(m_ctrlReg[2], true);
        m.r33 = extractSigned16(m_ctrlReg[2], true);
    } else
        extractMat3x3(mx * 8, m);
    return m;
}

Vector3<int16_t> GTE::getVector3FromV(uint8_t v)
{
    Vector3<int16_t> vec;
    if (v == 3)
    {
        vec.x = extractSigned16(m_dataReg[9], false);
        vec.y = extractSigned16(m_dataReg[10], false);
        vec.z = extractSigned16(m_dataReg[11], false);
    } else {
        vec.x = extractSigned16(m_dataReg[v * 2], false);
        vec.y = extractSigned16(m_dataReg[v * 2], true);
        vec.z = extractSigned16(m_dataReg[v * 2 + 1], false);
    }
    return vec;
}

Flag GTE::getFlags(uint32_t opcode)
{
    Flag f;
    f.sf    = (opcode >> 19) & 0x01;   // Shift flag (bit 19)
    f.mx = (opcode >> 17) & 0x03;   // Mat3x3 selection (bits 18-17)
    f.cv = (opcode >> 13) & 0x03;   // Translation vector (bits 14-13)
    f.v  = (opcode >> 15) & 0x03;   // Vector source (bits 16-15)
    f.lm    = (opcode >> 10) & 0x01;   // Limit mode (bit 10)
    return f;
}

void GTE::executeMVMVA(uint32_t opcode)
{
    Flag f = getFlags(opcode);
    Mat3x3 m = getMat3x3FromMX(f.mx);
    Vector3<int16_t> vec = getVector3FromV(f.v);
    Vector3<int32_t> translation = extractTranslation(5 + f.cv * 8);

    int64_t mac1, mac2, mac3;
    int fc = f.cv != 2;

    mac1 = (static_cast<int64_t>(m.r11) * vec.x * fc + static_cast<int64_t>(m.r12) * vec.y * fc + static_cast<int64_t>(m.r13) * vec.z + translation.x * fc) >> (12 * f.sf);
    mac2 = (static_cast<int64_t>(m.r21) * vec.x * fc + static_cast<int64_t>(m.r22) * vec.y * fc + static_cast<int64_t>(m.r23) * vec.z + translation.y * fc) >> (12 * f.sf);
    mac3 = (static_cast<int64_t>(m.r31) * vec.x * fc + static_cast<int64_t>(m.r32) * vec.y * fc + static_cast<int64_t>(m.r33) * vec.z + translation.z * fc) >> (12 * f.sf);

    m_dataReg[25] = static_cast<int32_t>(mac1);
    m_dataReg[26] = static_cast<int32_t>(mac2);
    m_dataReg[27] = static_cast<int32_t>(mac3);

    m_dataReg[9]  = clampMAC(mac1, IR_LIMIT_HIGH, f.lm ? IR_LIMIT_MODE : IR_LIMIT_LOW, 1 << 24, 1 << 24);
    m_dataReg[10] = clampMAC(mac2, IR_LIMIT_HIGH, f.lm ? IR_LIMIT_MODE : IR_LIMIT_LOW, 1 << 23, 1 << 23);
    m_dataReg[11] = clampMAC(mac3, IR_LIMIT_HIGH, f.lm ? IR_LIMIT_MODE : IR_LIMIT_LOW, 1 << 22, 1 << 22);
}

void GTE::extractMat3x3(int32_t base, Mat3x3 &Mat3x3)
{
    Mat3x3.r11 = extractSigned16(m_ctrlReg[base], true);
    Mat3x3.r12 = extractSigned16(m_ctrlReg[base], false);
    Mat3x3.r13 = extractSigned16(m_ctrlReg[base +1], true);
    Mat3x3.r21 = extractSigned16(m_ctrlReg[base +1], false);
    Mat3x3.r22 = extractSigned16(m_ctrlReg[base +2], true);
    Mat3x3.r23 = extractSigned16(m_ctrlReg[base +2], false);
    Mat3x3.r31 = extractSigned16(m_ctrlReg[base +3], true);
    Mat3x3.r32 = extractSigned16(m_ctrlReg[base +3], false);
    Mat3x3.r33 = extractSigned16(m_ctrlReg[base +4], true);
}

void GTE::extractVector3(int32_t base, Vector3<int16_t> &vector)
{
    vector.x = extractSigned16(m_dataReg[base], false);
    vector.y = extractSigned16(m_dataReg[base], true);
    vector.z = extractSigned16(m_dataReg[base + 1], false);
}

Vector3<int32_t> GTE::extractTranslation(int32_t base)
{
    Vector3<int32_t> translation;
    translation.x = m_ctrlReg[base];
    translation.y = m_ctrlReg[base + 1];
    translation.z = m_ctrlReg[base + 2];
    return translation;
}

/**
 * @brief Computes the average of SZ depths and stores it in OTZ (Ordering Table Z).
 *
 * This function is shared by both AVSZ3 and AVSZ4. It sums a number of depth values
 * (SZ1 to SZ3 or SZ0 to SZ3), multiplies the total by a scale factor (ZSF3 or ZSF4),
 * shifts the result down by 12 bits (which is like dividing by 4096), clamps it to the
 * 16-bit unsigned range [0, 65535], and stores the final value in the OTZ register (dataReg[7]).
 */
void GTE::executeAVSZ(int szCount, int zsfRegister) {
    int64_t szSum = 0;
    for (int i = 0; i < szCount; ++i) {
        szSum += m_dataReg[18 - i]; // Start from SZ3 and go backward
    }

    int32_t zsf = m_ctrlReg[zsfRegister] & 0xFFFF;

    int64_t mac0 = szSum * zsf;
    int64_t shifted = mac0 >> 12;

    int64_t clamped = std::min<int64_t>(std::max<int64_t>(shifted, 0), 0xFFFF);
    uint16_t otz = static_cast<uint16_t>(clamped);
    m_dataReg[7] = (m_dataReg[7] & 0xFFFF0000) | otz;

    m_dataReg[24] = static_cast<int32_t>(mac0); // Store MAC0 for debugging/inspection
}

void GTE::executeAVSZ3() {
    executeAVSZ(3, 20);
}

void GTE::executeAVSZ4() {
    executeAVSZ(4, 21);
}

void GTE::executeGPF(uint32_t opcode)
{
    executeGPL(opcode, false);
}

void GTE::executeGPL(uint32_t opcode, bool base)
{
    int32_t mac1, mac2, mac3;
    Flag f = getFlags(opcode);

    mac1 = base * (m_dataReg[25] << (f.sf * 12));
    mac2 = base * (m_dataReg[26] << (f.sf * 12));
    mac3 = base * (m_dataReg[27] << (f.sf * 12));

    int64_t temp_mac = static_cast<int64_t>(m_dataReg[9]) * m_dataReg[8] + mac1;
    checkMACOverflow(0, temp_mac);
    mac1 = static_cast<int32_t>(temp_mac >> (f.sf * 12));

    temp_mac = static_cast<int64_t>(m_dataReg[10]) * m_dataReg[8] + mac2;
    checkMACOverflow(1, temp_mac);
    mac2 = static_cast<int32_t>(temp_mac >> (f.sf * 12));

    temp_mac = static_cast<int64_t>(m_dataReg[11]) * m_dataReg[8] + mac3;
    checkMACOverflow(2, temp_mac);
    mac3 = static_cast<int32_t>(temp_mac >> (f.sf * 12));

    m_dataReg[9] = mac1;
    m_dataReg[10] = mac2;
    m_dataReg[11] = mac3;

    m_dataReg[25] = mac1;
    m_dataReg[26] = mac2;
    m_dataReg[27] = mac3;

    uint8_t r = clampMAC(mac1 / 16, FIFO_LIMIT_HIGH, FIFO_LIMIT_LOW, 1 << 21, 1 << 21);
    uint8_t g = clampMAC(mac2 / 16, FIFO_LIMIT_HIGH, FIFO_LIMIT_LOW, 1 << 20, 1 << 20);
    uint8_t b = clampMAC(mac3 / 16, FIFO_LIMIT_HIGH, FIFO_LIMIT_LOW, 1 << 19, 1 << 19);

    Rgbc fifo(r & 0xFF, g & 0xFF, b & 0xFF, (m_dataReg[20] >> 24) & 0xFF);
    m_dataReg[20] = (fifo.c << 24) | (fifo.r << 16) | (fifo.g << 8) | fifo.b;
}


void GTE::checkMACOverflow(int macIndex, int64_t value) {
    if (value >= MAC_LIMIT)
        m_ctrlReg[31] |= (1 << (30 - macIndex)); // bits 30, 29, 28
    else if (value <= -MAC_LIMIT)
        m_ctrlReg[31] |= (1 << (27 - macIndex)); // bits 27, 26, 25
}

/**
 * @brief Extracts a 16-bit signed integer from a 32-bit packed register.
 *
 * @param value The packed 32-bit value.
 * @param upper True to extract the high 16 bits, false for low 16 bits.
 * @return int16_t The extracted signed value.
 */
int16_t GTE::extractSigned16(uint32_t value, bool upper) {
    return static_cast<int16_t>(upper ? (value >> 16) : (value & 0xFFFF));
}

/**
 * @brief Clamps a MAC result to IR range and sets the GTE FLAG register on overflow/underflow.
 *
 * @param value Computed MAC value.
 * @param limitHigh Upper limit (typically 0x7FFF).
 * @param limitLow Lower limit (typically -0x8000).
 * @param flagBit Bitmask to set in FLAG register if clamped.
 * @return int32_t The clamped value.
 */

/**
 * @brief Sets overflow/underflow bits in the FLAG register based on the value's range.
 *
 * @param value The integer to check.
 * @param limitHigh Upper bound.
 * @param limitLow Lower bound.
 * @param flagBit Bit to set if value is out of range.
 */
void GTE::updateFlags(int32_t value, int limitHigh, int limitLow, uint32_t flagBit) {
    if (value > limitHigh || value < limitLow) {
        m_ctrlReg[31] |= flagBit; // Set overflow or underflow bit in FLAG register
    }
}
