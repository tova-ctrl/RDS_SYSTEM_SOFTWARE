#pragma once
#include <string>
#include <cstdint>

enum class FccCommandType {
    HEARTBEAT,
    STANDBY,          // SAFE → STANDBY (system ready, torque still off)
    ARM,              // STANDBY → ARMED (torque enabled)
    DISARM,           // any → SAFE
    MOTION_COMMAND,   // azimuth + elevation
    FIRE_REQUEST,
    RECOVERY_RESET
};

struct MotionParams {
    float azimuth_deg   = 0.0f;
    float elevation_deg = 0.0f;
    float rate_dps      = 0.0f;  // degrees per second
};

struct FccMessage {
    FccCommandType type = FccCommandType::HEARTBEAT;
    uint32_t       seq  = 0;      // sequence number for CRC/ordering checks
    MotionParams   motion{};
    std::string    source = "FCC";
};
