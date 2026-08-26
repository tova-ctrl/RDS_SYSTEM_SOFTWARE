#pragma once
#include <string>

enum class SystemState {
    SAFE,
    STANDBY,
    ARMED,
    FIRING,
    FAULT
};

inline std::string stateToString(SystemState s) {
    switch (s) {
        case SystemState::SAFE:    return "SAFE";
        case SystemState::STANDBY: return "STANDBY";
        case SystemState::ARMED:   return "ARMED";
        case SystemState::FIRING:  return "FIRING";
        case SystemState::FAULT:   return "FAULT";
        default:                   return "UNKNOWN";
    }
}
