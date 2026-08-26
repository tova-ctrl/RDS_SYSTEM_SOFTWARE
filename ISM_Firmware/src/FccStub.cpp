// FCC ↔ ISM communication stub.
// Simulates a Fire Control Computer sending messages to the ISM over a
// simple in-process queue (no sockets needed for the demo; swap the
// IsmMessageQueue for a real POSIX socket / CAN frame in hardware).

#include "FccStub.h"
#include <iostream>
#include <thread>
#include <chrono>

FccStub::FccStub(ISM& ism) : ism_(ism) {}

void FccStub::sendHeartbeat(uint32_t seq) {
    FccMessage msg;
    msg.type   = FccCommandType::HEARTBEAT;
    msg.seq    = seq;
    msg.source = "FCC-STUB";
    ism_.processMessage(msg);
}

bool FccStub::sendArm(uint32_t seq) {
    FccMessage msg;
    msg.type   = FccCommandType::ARM;
    msg.seq    = seq;
    msg.source = "FCC-STUB";
    return ism_.processMessage(msg);
}

bool FccStub::sendDisarm(uint32_t seq) {
    FccMessage msg;
    msg.type   = FccCommandType::DISARM;
    msg.seq    = seq;
    msg.source = "FCC-STUB";
    return ism_.processMessage(msg);
}

bool FccStub::sendMotion(uint32_t seq, float az, float el, float rate) {
    FccMessage msg;
    msg.type            = FccCommandType::MOTION_COMMAND;
    msg.seq             = seq;
    msg.source          = "FCC-STUB";
    msg.motion.azimuth_deg   = az;
    msg.motion.elevation_deg = el;
    msg.motion.rate_dps      = rate;
    return ism_.processMessage(msg);
}

bool FccStub::sendFireRequest(uint32_t seq) {
    FccMessage msg;
    msg.type   = FccCommandType::FIRE_REQUEST;
    msg.seq    = seq;
    msg.source = "FCC-STUB";
    return ism_.processMessage(msg);
}

bool FccStub::sendRecoveryReset(uint32_t seq, const std::string& authority) {
    FccMessage msg;
    msg.type   = FccCommandType::RECOVERY_RESET;
    msg.seq    = seq;
    msg.source = authority;
    return ism_.processMessage(msg);
}
