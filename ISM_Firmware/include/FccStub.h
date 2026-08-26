#pragma once
#include "ISM.h"
#include <string>
#include <cstdint>

// Simulated FCC that sends messages directly to the ISM in-process.
// In production this would be replaced by a real CAN / Ethernet transport.
class FccStub {
public:
    explicit FccStub(ISM& ism);

    void sendHeartbeat(uint32_t seq);
    bool sendArm(uint32_t seq);
    bool sendDisarm(uint32_t seq);
    bool sendMotion(uint32_t seq, float az, float el, float rate);
    bool sendFireRequest(uint32_t seq);
    bool sendRecoveryReset(uint32_t seq, const std::string& authority);

private:
    ISM& ism_;
};
