#pragma once
#include <string>

// Dual-channel fire interlock (MIL-STD-882E).
// Both Channel A and Channel B must independently authorize before firing is permitted.
// Any single channel denial is sufficient to inhibit fire.
class FireInterlock {
public:
    FireInterlock() = default;

    // Each channel is set independently (simulates two separate hardware paths).
    void setChannelA(bool authorized) { channel_a_ = authorized; }
    void setChannelB(bool authorized) { channel_b_ = authorized; }

    // Fire is only permitted when BOTH channels authorize AND the interlock is armed.
    bool isFirePermitted() const {
        return armed_ && channel_a_ && channel_b_;
    }

    void arm()    { armed_ = true; }
    void disarm() { armed_ = false; channel_a_ = false; channel_b_ = false; }

    std::string status() const {
        return std::string("Interlock[armed=") + (armed_ ? "Y" : "N") +
               " chA=" + (channel_a_ ? "1" : "0") +
               " chB=" + (channel_b_ ? "1" : "0") + "]";
    }

private:
    bool armed_     = false;
    bool channel_a_ = false;
    bool channel_b_ = false;
};
