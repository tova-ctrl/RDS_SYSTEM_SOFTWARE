#pragma once
#include <cstdint>
#include <string>

// =============================================================================
// FaultCode.h — ISM fault codes aligned with SWR-SAFE requirements
//
// Format: 0xSSFF
//   SS = SWR-SAFE requirement number (01, 03, 04 ...)
//   FF = specific sub-fault within that requirement
// =============================================================================

enum class FaultCode : uint16_t {
    NONE              = 0x0000,

    // ── SWR-SAFE-001: SAFE state on power-up / reset / watchdog / comm loss ──
    WATCHDOG_TIMEOUT  = 0x0101,   // FCC stopped sending heartbeats
    COMM_LOSS         = 0x0102,   // No FCC present at all
    INVALID_COMMAND   = 0x0103,   // Unknown / malformed command type
    SEQ_ERROR         = 0x0104,   // Sequence number gap or replay
    INTERNAL_ERROR    = 0x0105,   // Safety-monitor internal fault (injected or HW check)

    // ── SWR-SAFE-003: Command range, rate, direction validation ──────────────
    AZ_OUT_OF_RANGE   = 0x0301,   // Azimuth exceeds ±180°
    EL_OUT_OF_RANGE   = 0x0302,   // Elevation exceeds ±90°
    RATE_EXCEEDED     = 0x0303,   // Rate exceeds 45 dps

    // ── SWR-SAFE-004: Dual-channel interlock for fire ────────────────────────
    FIRE_NO_INTERLOCK = 0x0401,   // Fire requested without interlock satisfied
};

inline std::string faultCodeToString(FaultCode c) {
    switch (c) {
        case FaultCode::NONE:              return "NONE";
        case FaultCode::WATCHDOG_TIMEOUT:  return "WATCHDOG_TIMEOUT";
        case FaultCode::COMM_LOSS:         return "COMM_LOSS";
        case FaultCode::INVALID_COMMAND:   return "INVALID_COMMAND";
        case FaultCode::SEQ_ERROR:         return "SEQ_ERROR";
        case FaultCode::INTERNAL_ERROR:    return "INTERNAL_ERROR";
        case FaultCode::AZ_OUT_OF_RANGE:   return "AZ_OUT_OF_RANGE";
        case FaultCode::EL_OUT_OF_RANGE:   return "EL_OUT_OF_RANGE";
        case FaultCode::RATE_EXCEEDED:     return "RATE_EXCEEDED";
        case FaultCode::FIRE_NO_INTERLOCK: return "FIRE_NO_INTERLOCK";
        default:                           return "UNKNOWN";
    }
}

inline std::string faultCodeToHex(FaultCode c) {
    char buf[7];
    snprintf(buf, sizeof(buf), "0x%04X", static_cast<uint16_t>(c));
    return buf;
}
