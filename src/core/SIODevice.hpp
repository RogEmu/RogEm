#pragma once
#include <cstdint>

// Base interface for anything that sits on the PSX SIO0 bus (pads, memcards)
class SIODevice {

    public:
    virtual ~SIODevice() = default;

    // Called when the console sends one byte while /CS is asserted.
    // Put one reply byte in rx. If the device wants to /ACK now, set ackNow=true.
    // ackDelayCycles can be 0 for MVP (immediate); we ignore it here.
    // Return true if the device expects more bytes in this transaction.
    virtual bool onByteFromConsole(uint8_t tx, uint8_t& rx, bool& ackNow, int& ackDelayCycles) = 0;

    // /CS deasserted: reset internal transfer state
    virtual void endTransaction() = 0;

    // Hotplug presence (always true for now)
    virtual bool isPresent() const { return true; }
};
