#pragma once
// =============================================================================
// IsmProtocol.h — CAN frame IDs and encoding for FCC ↔ ISM communication
//
// CAN IDs match Communication_Architecture.md
// All values are little-endian packed into the 8-byte CAN data field.
// =============================================================================

#include <cstdint>
#include <cstring>
#include "CanBus.h"

// ── CAN IDs — FCC → ISM ──────────────────────────────────────────────────────
constexpr uint32_t CAN_ID_HEARTBEAT      = 0x100;
constexpr uint32_t CAN_ID_ARM            = 0x101;
constexpr uint32_t CAN_ID_DISARM         = 0x102;
constexpr uint32_t CAN_ID_STANDBY        = 0x103;
constexpr uint32_t CAN_ID_MOTION         = 0x110;
constexpr uint32_t CAN_ID_FIRE_REQUEST   = 0x120;
constexpr uint32_t CAN_ID_RECOVERY_RESET  = 0x130;
// Dual-channel FIRE interlock (SWR-SAFE-004, MIL-STD-882E) — two independent
// safety-catch signals (B5/B6 on the operator controller), sent as two SEPARATE
// messages by design, not combined into one on the PC — a single corrupted/lost
// message must only ever affect ONE channel, never both, or the whole point of
// having two independent channels is defeated. Each carries a 1-byte payload:
// 1 = channel open/authorized, 0 = channel closed/deauthorized.
constexpr uint32_t CAN_ID_SAFETY_CHANNEL_A = 0x140;
constexpr uint32_t CAN_ID_SAFETY_CHANNEL_B = 0x141;
constexpr uint32_t CAN_ID_INJECT_INVALID   = 0x1FF;  // fault injection: INVALID_COMMAND
constexpr uint32_t CAN_ID_INJECT_RESET     = 0x1FE;  // fault injection: system reset → SAFE
constexpr uint32_t CAN_ID_INJECT_INTERNAL  = 0x1FD;  // fault injection: internal fault → FAULT

// ── CAN IDs — ISM → FCC ──────────────────────────────────────────────────────
constexpr uint32_t CAN_ID_STATE_ACK      = 0x200;
constexpr uint32_t CAN_ID_FAULT_REPORT   = 0x210;
constexpr uint32_t CAN_ID_HEARTBEAT_ACK  = 0x211;

// ── ISM State values (1 byte, matches SystemState enum on ISM side) ───────────
constexpr uint8_t ISM_STATE_SAFE      = 0;
constexpr uint8_t ISM_STATE_STANDBY   = 1;
constexpr uint8_t ISM_STATE_ARMED     = 2;
constexpr uint8_t ISM_STATE_FIRING    = 3;
constexpr uint8_t ISM_STATE_FAULT     = 4;
constexpr uint8_t ISM_STATE_COMM_LOSS = 5;  // FCC startup / no ACK received yet

inline const char* ismStateStr(uint8_t s) {
    switch (s) {
        case ISM_STATE_SAFE:      return "SAFE";
        case ISM_STATE_STANDBY:   return "STANDBY";
        case ISM_STATE_ARMED:     return "ARMED";
        case ISM_STATE_FIRING:    return "FIRING";
        case ISM_STATE_FAULT:     return "FAULT";
        case ISM_STATE_COMM_LOSS: return "COMM_LOSS";
        default:                  return "UNKNOWN";
    }
}

// ── Frame builders — FCC → ISM ───────────────────────────────────────────────

inline CanFrame buildHeartbeat(uint32_t seq) {
    CanFrame f{};
    f.id  = CAN_ID_HEARTBEAT;
    f.len = 4;
    memcpy(f.data, &seq, 4);
    return f;
}

inline CanFrame buildStandby(uint32_t seq) {
    CanFrame f{};
    f.id  = CAN_ID_STANDBY;
    f.len = 4;
    memcpy(f.data, &seq, 4);
    return f;
}

inline CanFrame buildArm(uint32_t seq) {
    CanFrame f{};
    f.id  = CAN_ID_ARM;
    f.len = 4;
    memcpy(f.data, &seq, 4);
    return f;
}

inline CanFrame buildDisarm(uint32_t seq) {
    CanFrame f{};
    f.id  = CAN_ID_DISARM;
    f.len = 4;
    memcpy(f.data, &seq, 4);
    return f;
}

// Motion: [seq(2)] [az_int16 * 10] [el_int16 * 10] [rate_int16 * 10]
// seq truncated to uint16 (lower 16 bits) to free room for rate in the 8-byte CAN frame.
inline CanFrame buildMotion(uint32_t seq, float az, float el, float rate) {
    CanFrame f{};
    f.id  = CAN_ID_MOTION;
    f.len = 8;
    uint16_t seq16    = static_cast<uint16_t>(seq & 0xFFFF);
    int16_t  az_enc   = static_cast<int16_t>(az   * 10.0f);
    int16_t  el_enc   = static_cast<int16_t>(el   * 10.0f);
    int16_t  rate_enc = static_cast<int16_t>(rate  * 10.0f);
    memcpy(f.data,     &seq16,    2);
    memcpy(f.data + 2, &az_enc,   2);
    memcpy(f.data + 4, &el_enc,   2);
    memcpy(f.data + 6, &rate_enc, 2);
    return f;
}

inline CanFrame buildFireRequest(uint32_t seq) {
    CanFrame f{};
    f.id  = CAN_ID_FIRE_REQUEST;
    f.len = 4;
    memcpy(f.data, &seq, 4);
    return f;
}

inline CanFrame buildInjectInvalidCmd(uint32_t seq) {
    CanFrame f{}; f.id = CAN_ID_INJECT_INVALID; f.len = 4;
    memcpy(f.data, &seq, 4); return f;
}
inline CanFrame buildInjectReset(uint32_t seq) {
    CanFrame f{}; f.id = CAN_ID_INJECT_RESET; f.len = 4;
    memcpy(f.data, &seq, 4); return f;
}
inline CanFrame buildInjectInternal(uint32_t seq) {
    CanFrame f{}; f.id = CAN_ID_INJECT_INTERNAL; f.len = 4;
    memcpy(f.data, &seq, 4); return f;
}

inline CanFrame buildSafetyChannelA(bool open) {
    CanFrame f{};
    f.id  = CAN_ID_SAFETY_CHANNEL_A;
    f.len = 1;
    f.data[0] = open ? 1 : 0;
    return f;
}

inline CanFrame buildSafetyChannelB(bool open) {
    CanFrame f{};
    f.id  = CAN_ID_SAFETY_CHANNEL_B;
    f.len = 1;
    f.data[0] = open ? 1 : 0;
    return f;
}

inline CanFrame buildRecoveryReset(uint32_t seq) {
    CanFrame f{};
    f.id  = CAN_ID_RECOVERY_RESET;
    f.len = 4;
    memcpy(f.data, &seq, 4);
    return f;
}

// ── Frame parsers — ISM → FCC ────────────────────────────────────────────────

struct IsmStateAck {
    uint8_t new_state;
    uint8_t rejection_reason;
};

struct IsmFaultReport {
    uint16_t fault_code;
    uint8_t  state;
    uint32_t timestamp_ms;
};

struct IsmHeartbeatAck {
    uint32_t seq_echo;
    uint8_t  ism_state;
    uint8_t ism_save_state;
};

inline IsmStateAck parseStateAck(const CanFrame& f) {
    IsmStateAck r{};
    if (f.len >= 1) r.new_state         = f.data[0];
    if (f.len >= 2) r.rejection_reason  = f.data[1];
    return r;
}

inline IsmFaultReport parseFaultReport(const CanFrame& f) {
    IsmFaultReport r{};
    if (f.len >= 2) memcpy(&r.fault_code,    f.data,     2);
    if (f.len >= 3) r.state = f.data[2];
    if (f.len >= 7) memcpy(&r.timestamp_ms,  f.data + 3, 4);
    return r;
}

inline IsmHeartbeatAck parseHeartbeatAck(const CanFrame& f) {
    IsmHeartbeatAck r{};
    if (f.len >= 4) memcpy(&r.seq_echo,  f.data,     4);
    if (f.len >= 5) r.ism_state = f.data[4];
    return r;
}
