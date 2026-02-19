/*
** EPITECH PROJECT, 2025
** rogem
** File description:
** CDROM
*/

#include "CDROM.hpp"

#include <spdlog/spdlog.h>
#include <fmt/format.h>

#include "MemoryMap.hpp"
#include "Bus.hpp"
#include "InterruptController.hpp"

CDROM::CDROM(Bus *bus) :
    PsxDevice(bus),
    m_index(0),
    m_interruptFlag(0),
    m_interruptEnable(0),
    m_parameterFifo(),
    m_responseFifo(),
    m_dataFifo(),
    m_state(CDROMState::Idle),
    m_currentCommand(0),
    m_delayCounter(0),
    m_secondResponse(),
    m_hasSecondResponse(false),
    m_stat(0x02),
    m_mode(0x00),
    m_seekTarget(),
    m_readPos(),
    m_wantData(false),
    m_volumeRR(0),
    m_volumeRL(0),
    m_volumeLR(0),
    m_volumeLL(0),
    m_readDelayCounter(0),
    m_sectorBuffer()
{
    m_memoryRange = MemoryMap::CDROM_REGISTERS_RANGE;
}

CDROM::~CDROM()
{
}

void CDROM::loadDisc(std::unique_ptr<cuebin::Disc> &disc)
{
    m_disc = std::move(disc);
}

bool CDROM::hasDisc() const
{
    return m_disc != nullptr;
}

uint32_t CDROM::readDataWord()
{
    uint32_t word = 0;
    word |= m_dataFifo.pop();
    word |= static_cast<uint32_t>(m_dataFifo.pop()) << 8;
    word |= static_cast<uint32_t>(m_dataFifo.pop()) << 16;
    word |= static_cast<uint32_t>(m_dataFifo.pop()) << 24;
    return word;
}

void CDROM::readSector()
{
    cuebin::MSF msf {};

    auto sector = m_disc->readSector(msf);
    if (!sector) {
        spdlog::error("CDROM: Failed to read sector at {}:{}:{}", msf.minute, msf.second, msf.frame);
        m_responseFifo.clear();
        pushResponse(CDROMInterrupt::Error, {static_cast<uint8_t>(m_stat | 0x01), 0x40});
        return;
    }

    m_sectorBuffer = sector.value().data;
    // Copy bytes 12-19 into m_lastSectorHeader (for GetLocL)
    for (int i = 0; i < 8; i++)
        m_lastSectorHeader[i] = m_sectorBuffer[12 + i];

    // Fill data FIFO
    m_dataFifo.clear();
    if (m_mode & 0x20) {
        // Whole sector mode: bytes 12..2351 (2340 bytes)
        for (int i = 12; i < 2352; i++)
            m_dataFifo.push(m_sectorBuffer[i]);
    } else {
        // Data only mode: bytes 24..2071 (2048 bytes)
        for (int i = 24; i < 2072; i++)
            m_dataFifo.push(m_sectorBuffer[i]);
    }

    // Deliver INT1 (DataReady) with stat in response FIFO
    m_responseFifo.clear();
    m_responseFifo.push(m_stat);
    deliverInterrupt(CDROMInterrupt::DataReady);

    // Reset read delay based on speed
    m_readDelayCounter = (m_mode & 0x80) ? 225792 : 451584;
}

void CDROM::reset()
{
    m_index = 0;
    m_interruptFlag = 0;
    m_interruptEnable = 0;
    m_parameterFifo.clear();
    m_responseFifo.clear();
    m_dataFifo.clear();
    m_state = CDROMState::Idle;
    m_currentCommand = 0;
    m_delayCounter = 0;
    m_hasSecondResponse = false;
    m_stat = 0x02;
    m_mode = 0x00;
    m_seekTarget = {};
    m_readPos = {};
    m_wantData = false;
    m_volumeRR = 0;
    m_volumeRL = 0;
    m_volumeLR = 0;
    m_volumeLL = 0;
    m_readDelayCounter = 0;
    m_sectorBuffer.fill(0);
}

// --- Status Register ---

uint8_t CDROM::computeStatusRegister() const
{
    uint8_t status = m_index & 0x03;
    // Bit 2: XA-ADPCM fifo empty (always 0 for now)
    if (m_parameterFifo.isEmpty())
        status |= (1 << 3);   // PRMEMPT
    if (!m_parameterFifo.isFull())
        status |= (1 << 4);   // PRMWRDY
    if (!m_responseFifo.isEmpty())
        status |= (1 << 5);   // RSLRRDY
    if (!m_dataFifo.isEmpty())
        status |= (1 << 6);   // DRQSTS
    if (m_state != CDROMState::Idle)
        status |= (1 << 7);   // BUSYSTS
    return status;
}

// --- Read Registers ---

uint8_t CDROM::read8(uint32_t address)
{
    uint32_t offset = address & 0x3;

    switch (offset) {
        case 0: {
            uint8_t status = computeStatusRegister();
            return status;
        }
        case 1:
            return m_responseFifo.pop();
        case 2:
            return m_dataFifo.pop();
        case 3:
            if (m_index == 0 || m_index == 2)
                return 0xE0 | (m_interruptEnable & 0x1F);
            else
                return 0xE0 | (m_interruptFlag & 0x1F);
        default:
            spdlog::warn("CDROM: Unexpected read8 at offset {}", offset);
            return 0;
    }
}

uint16_t CDROM::read16(uint32_t address)
{
    uint16_t lo = read8(address);
    uint16_t hi = read8(address + 1);
    return lo | (hi << 8);
}

uint32_t CDROM::read32(uint32_t address)
{
    uint32_t b0 = read8(address);
    uint32_t b1 = read8(address + 1);
    uint32_t b2 = read8(address + 2);
    uint32_t b3 = read8(address + 3);
    return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
}

// --- Write Registers ---

void CDROM::write8(uint8_t value, uint32_t address)
{
    uint32_t offset = address & 0x3;

    switch (offset) {
        case 0:
            m_index = value & 0x3;
            break;
        case 1:
            switch (m_index) {
                case 0:
                    startCommand(value);
                    break;
                case 1:
                    spdlog::debug("CDROM: Sound Map Data Out = 0x{:02X} (ignored)", value);
                    break;
                case 2:
                    spdlog::debug("CDROM: Sound Map Coding Info = 0x{:02X} (ignored)", value);
                    break;
                case 3:
                    m_volumeRR = value;
                    break;
            }
            break;
        case 2:
            switch (m_index) {
                case 0:
                    m_parameterFifo.push(value);
                    break;
                case 1:
                    m_interruptEnable = value & 0x1F;
                    break;
                case 2:
                    m_volumeLL = value;
                    break;
                case 3:
                    m_volumeRL = value;
                    break;
            }
            break;
        case 3:
            switch (m_index) {
                case 0:
                    // Request register
                    if (!(value & 0x80))
                        m_parameterFifo.clear();
                    m_wantData = (value & 0x80) != 0;
                    break;
                case 1:
                    // Interrupt flag acknowledge
                    m_interruptFlag &= ~(value & 0x1F);
                    if (value & 0x40)
                        m_parameterFifo.clear();
                    if (value & 0x1F)
                        m_responseFifo.clear();
                    break;
                case 2:
                    m_volumeLR = value;
                    break;
                case 3:
                    spdlog::debug("CDROM: Apply volume changes = 0x{:02X} (ignored)", value);
                    break;
            }
            break;
        default:
            spdlog::warn("CDROM: Unexpected write8 at offset {} = 0x{:02X}", offset, value);
            break;
    }
}

void CDROM::write16(uint16_t value, uint32_t address)
{
    write8(static_cast<uint8_t>(value & 0xFF), address);
    write8(static_cast<uint8_t>((value >> 8) & 0xFF), address + 1);
}

void CDROM::write32(uint32_t value, uint32_t address)
{
    write8(static_cast<uint8_t>(value & 0xFF), address);
    write8(static_cast<uint8_t>((value >> 8) & 0xFF), address + 1);
    write8(static_cast<uint8_t>((value >> 16) & 0xFF), address + 2);
    write8(static_cast<uint8_t>((value >> 24) & 0xFF), address + 3);
}

// --- Update (cycle-driven state machine) ---

void CDROM::update(int cycles)
{
    switch (m_state) {
        case CDROMState::Idle:
            return;

        case CDROMState::WaitingFirstResponse:
            m_delayCounter -= cycles;
            if (m_delayCounter <= 0)
                executeCommand();
            break;

        case CDROMState::WaitingSecondResponse:
            m_delayCounter -= cycles;
            if (m_delayCounter <= 0 && m_interruptFlag == 0)
                deliverSecondResponse();
            break;

        case CDROMState::Reading:
            if (m_interruptFlag != 0)
                return;
            m_readDelayCounter -= cycles;
            if (m_readDelayCounter <= 0) {
                if (hasDisc()) {
                    readSector();
                } else {
                    m_responseFifo.clear();
                    pushResponse(CDROMInterrupt::Error, {static_cast<uint8_t>(m_stat | 0x01), 0x40});
                }
            }
            break;
    }
}

// --- Command Handling ---

void CDROM::startCommand(uint8_t cmd)
{
    logCommand(cmd);
    m_currentCommand = cmd;
    m_state = CDROMState::WaitingFirstResponse;
    m_delayCounter = 50000;  // ~1.5ms
    m_hasSecondResponse = false;
}

void CDROM::executeCommand()
{
    m_responseFifo.clear();

    switch (m_currentCommand) {
        case 0x01: cmdGetStat();  break;
        case 0x02: cmdSetLoc();   break;
        case 0x03: cmdPlay();     break;
        case 0x06: cmdReadN();    break;
        case 0x07: cmdMotorOn();  break;
        case 0x08: cmdStop();     break;
        case 0x09: cmdPause();    break;
        case 0x0A: cmdInit();     break;
        case 0x0B: cmdMute();     break;
        case 0x0C: cmdDemute();   break;
        case 0x0D: cmdSetFilter(); break;
        case 0x0E: cmdSetMode();  break;
        case 0x10: cmdGetLocL();  break;
        case 0x11: cmdGetLocP();  break;
        case 0x13: cmdGetTN();    break;
        case 0x14: cmdGetTD();    break;
        case 0x15: cmdSeekL();    break;
        case 0x19: cmdTest();     break;
        case 0x1A: cmdGetID();    break;
        case 0x1B: cmdReadS();    break;
        case 0x1E: cmdReadTOC();  break;
        default:
            spdlog::warn("CDROM: Unknown command 0x{:02X}", m_currentCommand);
            pushResponse(CDROMInterrupt::Error, {static_cast<uint8_t>(m_stat | 0x01), 0x40});
            break;
    }

    m_parameterFifo.clear();
}

// --- Interrupt & Response Helpers ---

void CDROM::deliverInterrupt(uint8_t type)
{
    m_interruptFlag = type & 0x1F;
    if (m_interruptEnable & m_interruptFlag) {
        auto irqc = m_bus->getDevice<InterruptController>();
        if (irqc)
            irqc->triggerIRQ(DeviceIRQ::CDROM);
    }
}

void CDROM::pushResponse(uint8_t type, std::initializer_list<uint8_t> bytes)
{
    for (auto b : bytes)
        m_responseFifo.push(b);
    deliverInterrupt(type);

    if (m_hasSecondResponse)
        m_state = CDROMState::WaitingSecondResponse;
    else
        m_state = CDROMState::Idle;
}

void CDROM::setSecondResponse(uint8_t type, std::initializer_list<uint8_t> bytes, int delay)
{
    m_secondResponse.data.clear();
    for (auto b : bytes)
        m_secondResponse.data.push(b);
    m_secondResponse.interruptType = type;
    m_hasSecondResponse = true;
    m_delayCounter = delay;
}

void CDROM::deliverSecondResponse()
{
    m_responseFifo.clear();
    while (!m_secondResponse.data.isEmpty())
        m_responseFifo.push(m_secondResponse.data.pop());
    deliverInterrupt(m_secondResponse.interruptType);
    m_hasSecondResponse = false;
    m_state = CDROMState::Idle;
}

// --- Logging ---

const char* CDROM::commandName(uint8_t cmd)
{
    switch (cmd) {
        case 0x01: return "GetStat";
        case 0x02: return "SetLoc";
        case 0x03: return "Play";
        case 0x06: return "ReadN";
        case 0x07: return "MotorOn";
        case 0x08: return "Stop";
        case 0x09: return "Pause";
        case 0x0A: return "Init";
        case 0x0B: return "Mute";
        case 0x0C: return "Demute";
        case 0x0D: return "SetFilter";
        case 0x0E: return "SetMode";
        case 0x10: return "GetLocL";
        case 0x11: return "GetLocP";
        case 0x13: return "GetTN";
        case 0x14: return "GetTD";
        case 0x15: return "SeekL";
        case 0x19: return "Test";
        case 0x1A: return "GetID";
        case 0x1B: return "ReadS";
        case 0x1E: return "ReadTOC";
        default:   return "Unknown";
    }
}

void CDROM::logCommand(uint8_t cmd)
{
    if (m_parameterFifo.isEmpty()) {
        spdlog::info("CDROM: Command {} (0x{:02X}) [no params]", commandName(cmd), cmd);
    } else {
        std::string params;
        // Read params non-destructively
        uint16_t pos = m_parameterFifo.readPos;
        for (uint16_t i = 0; i < m_parameterFifo.count; i++) {
            if (i > 0)
                params += ", ";
            params += fmt::format("{:02X}", m_parameterFifo.data[pos]);
            pos = (pos + 1) % 16;
        }
        spdlog::info("CDROM: Command {} (0x{:02X}) [params: {}]", commandName(cmd), cmd, params);
    }
}

// --- Command Handlers ---

void CDROM::cmdGetStat()
{
    pushResponse(CDROMInterrupt::Acknowledge, {m_stat});
}

void CDROM::cmdSetLoc()
{
    m_seekTarget.minute = m_parameterFifo.pop();
    m_seekTarget.second = m_parameterFifo.pop();
    m_seekTarget.frame  = m_parameterFifo.pop();
    pushResponse(CDROMInterrupt::Acknowledge, {m_stat});
}

void CDROM::cmdPlay()
{
    // Optional track parameter (ignored — no disc)
    if (!m_parameterFifo.isEmpty())
        m_parameterFifo.pop();
    m_stat |= 0x80;  // Set Play bit
    pushResponse(CDROMInterrupt::Acknowledge, {m_stat});
}

void CDROM::cmdReadN()
{
    m_stat |= 0x20;  // Set Read bit
    if (hasDisc()) {
        m_readPos = m_seekTarget;
        m_readDelayCounter = (m_mode & 0x80) ? 225792 : 451584;
        pushResponse(CDROMInterrupt::Acknowledge, {m_stat});
        m_state = CDROMState::Reading;  // override Idle set by pushResponse
    } else {
        setSecondResponse(CDROMInterrupt::Error,
            {static_cast<uint8_t>(m_stat | 0x01), 0x40}, 1000000);
        pushResponse(CDROMInterrupt::Acknowledge, {m_stat});
    }
}

void CDROM::cmdMotorOn()
{
    m_stat |= 0x02;  // Motor on
    setSecondResponse(CDROMInterrupt::Complete, {m_stat}, 1000000);
    pushResponse(CDROMInterrupt::Acknowledge, {m_stat});
}

void CDROM::cmdStop()
{
    m_stat = 0x00;  // Clear all mode bits
    setSecondResponse(CDROMInterrupt::Complete, {m_stat}, 2000000);
    pushResponse(CDROMInterrupt::Acknowledge, {m_stat});
}

void CDROM::cmdPause()
{
    m_stat &= ~0x20;  // Clear Read bit
    setSecondResponse(CDROMInterrupt::Complete, {m_stat}, 1000000);
    pushResponse(CDROMInterrupt::Acknowledge, {m_stat});
}

void CDROM::cmdInit()
{
    m_mode = 0x00;
    m_stat &= ~(0x20 | 0x40 | 0x80);  // Clear Read/Seek/Play bits
    m_stat |= 0x02;                    // Ensure motor on
    setSecondResponse(CDROMInterrupt::Complete, {m_stat}, 30000);
    pushResponse(CDROMInterrupt::Acknowledge, {m_stat});
}

void CDROM::cmdMute()
{
    pushResponse(CDROMInterrupt::Acknowledge, {m_stat});
}

void CDROM::cmdDemute()
{
    pushResponse(CDROMInterrupt::Acknowledge, {m_stat});
}

void CDROM::cmdSetFilter()
{
    // file, channel params — just consume
    if (!m_parameterFifo.isEmpty()) m_parameterFifo.pop();
    if (!m_parameterFifo.isEmpty()) m_parameterFifo.pop();
    pushResponse(CDROMInterrupt::Acknowledge, {m_stat});
}

void CDROM::cmdSetMode()
{
    if (!m_parameterFifo.isEmpty())
        m_mode = m_parameterFifo.pop();
    pushResponse(CDROMInterrupt::Acknowledge, {m_stat});
}

void CDROM::cmdGetLocL()
{
    if (hasDisc()) {
        pushResponse(CDROMInterrupt::Acknowledge, {
            m_lastSectorHeader[0], m_lastSectorHeader[1],
            m_lastSectorHeader[2], m_lastSectorHeader[3],
            m_lastSectorHeader[4], m_lastSectorHeader[5],
            m_lastSectorHeader[6], m_lastSectorHeader[7]
        });
    } else {
        pushResponse(CDROMInterrupt::Error, {static_cast<uint8_t>(m_stat | 0x01), 0x80});
    }
}

void CDROM::cmdGetLocP()
{
    if (hasDisc()) {
        pushResponse(CDROMInterrupt::Acknowledge, {
            0x01, 0x01,
            m_readPos.minute, m_readPos.second, m_readPos.frame,
            m_readPos.minute, m_readPos.second, m_readPos.frame
        });
    } else {
        pushResponse(CDROMInterrupt::Acknowledge, {0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00});
    }
}

void CDROM::cmdGetTN()
{
    if (hasDisc()) {
        pushResponse(CDROMInterrupt::Acknowledge, {m_stat, 0x01, 0x01});
    } else {
        pushResponse(CDROMInterrupt::Error, {static_cast<uint8_t>(m_stat | 0x01), 0x40});
    }
}

void CDROM::cmdGetTD()
{
    uint8_t track = 0;
    if (!m_parameterFifo.isEmpty())
        track = fromBcd(m_parameterFifo.pop());
    if (!hasDisc()) {
        pushResponse(CDROMInterrupt::Error, {static_cast<uint8_t>(m_stat | 0x01), 0x40});
        return;
    }
    if (track == 0) {
        uint8_t mm, ss, ff;
        lbaToMsf(m_disc->totalSectors(), mm, ss, ff);
        pushResponse(CDROMInterrupt::Acknowledge, {m_stat, toBcd(mm), toBcd(ss)});
    } else {
        pushResponse(CDROMInterrupt::Acknowledge, {m_stat, 0x00, 0x02});
    }
}

void CDROM::cmdSeekL()
{
    m_stat |= 0x40;  // Set Seek bit
    m_readPos = m_seekTarget;
    setSecondResponse(CDROMInterrupt::Complete, {static_cast<uint8_t>(m_stat & ~0x40u)}, 1000000);
    pushResponse(CDROMInterrupt::Acknowledge, {m_stat});
}

void CDROM::cmdTest()
{
    uint8_t subFunction = 0;
    if (!m_parameterFifo.isEmpty())
        subFunction = m_parameterFifo.pop();

    switch (subFunction) {
        case 0x20:
            // Return BIOS date/version: 97h, 01h, 10h, C2h
            pushResponse(CDROMInterrupt::Acknowledge, {0x97, 0x01, 0x10, 0xC2});
            break;
        default:
            spdlog::warn("CDROM: Test sub-function 0x{:02X} not implemented", subFunction);
            pushResponse(CDROMInterrupt::Error, {static_cast<uint8_t>(m_stat | 0x01), 0x40});
            break;
    }
}

void CDROM::cmdGetID()
{
    if (hasDisc()) {
        setSecondResponse(CDROMInterrupt::Complete,
            {m_stat, 0x00, 0x20, 0x00, 'S', 'C', 'E', 'A'}, 1000000);
    } else {
        setSecondResponse(CDROMInterrupt::Error,
            {0x08, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 1000000);
    }
    pushResponse(CDROMInterrupt::Acknowledge, {m_stat});
}

void CDROM::cmdReadS()
{
    // Same as ReadN
    cmdReadN();
}

void CDROM::cmdReadTOC()
{
    setSecondResponse(CDROMInterrupt::Complete, {m_stat}, 16000000);
    pushResponse(CDROMInterrupt::Acknowledge, {m_stat});
}

uint32_t CDROM::msfToLba(uint8_t minute, uint8_t second, uint8_t frame)
{
    return (minute * 60 + second) * 75 + frame - 150;
}

void CDROM::lbaToMsf(uint32_t lba, uint8_t& minute, uint8_t& second, uint8_t& frame)
{
    uint32_t absolute = lba + 150;
    minute = static_cast<uint8_t>(absolute / (60 * 75));
    absolute %= (60 * 75);
    second = static_cast<uint8_t>(absolute / 75);
    frame = static_cast<uint8_t>(absolute % 75);
}

uint8_t CDROM::toBcd(uint8_t value)
{
    return static_cast<uint8_t>((value / 10) << 4 | (value % 10));
}

uint8_t CDROM::fromBcd(uint8_t bcd)
{
    return static_cast<uint8_t>((bcd >> 4) * 10 + (bcd & 0x0F));
}
