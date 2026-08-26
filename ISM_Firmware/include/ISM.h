#pragma once
#include "SystemState.h"
#include "Watchdog.h"
#include "FireInterlock.h"
#include "FccMessage.h"
#include "IsmEmgLink.h"
#include "FaultCode.h"

#include <string>
#include <vector>
#include <functional>

struct IsmLog {
    std::string timestamp;
    std::string message;
};

// Independent Safety Monitor (ISM) — the primary safety authority.
//
// Responsibilities (per SWR-SAFE-001..008):
//   • Enforces SAFE state on power-up, reset, watchdog expiry, comm loss,
//     invalid command, or internal fault.          (SWR-SAFE-001)
//   • Gates all actuator torque enables.           (SWR-SAFE-002)
//   • Validates command range, rate, direction.    (SWR-SAFE-003)
//   • Requires dual-channel interlock for fire.    (SWR-SAFE-004)
//   • Operates independently of FCC / GUI.         (SWR-SAFE-005)
//   • Logs faults with time, state, source.        (SWR-SAFE-006)
//   • Provides controlled recovery from FAULT.     (SWR-SAFE-008)
class ISM {
public:
    // Limits enforced by ISM (SWR-SAFE-003)
    static constexpr float MAX_AZIMUTH_DEG   = 180.0f;
    static constexpr float MAX_ELEVATION_DEG =  90.0f;
    static constexpr float MAX_RATE_DPS      =  45.0f;  // degrees/sec

    // Watchdog timeout: if no heartbeat arrives within this window, ISM → SAFE.
    static constexpr int   WATCHDOG_TIMEOUT_MS = 500;

    explicit ISM();

    // ── EMG Ethernet link ───────────────────────────────────────────────────
    // Call once after network is up. If not called, EMG notifications are silent.
    // Returns false if socket creation failed (EMG will be silently skipped).
    bool initEmgLink(const std::string& webapp_ip,
                     uint16_t port = IsmEmgLink::DEFAULT_EMG_PORT);

    // ── Main API ────────────────────────────────────────────────────────────

    // Called periodically by the run loop to tick the watchdog.
    void tick();

    // Process an incoming FCC message. Returns false if the message was rejected.
    bool processMessage(const FccMessage& msg);

    // Torque enable output — FCC must NOT drive actuators unless this is true.
    bool isTorqueEnabled() const { return torque_enabled_; }

    SystemState getState() const { return state_; }

    // Dual-channel interlock access (test harness can set channels directly).
    FireInterlock& interlock() { return interlock_; }

    // Fault-cleared + authorized reset → transition FAULT → SAFE (SWR-SAFE-008).
    bool requestRecovery(const std::string& authority);

    // Last fault code (SWR-SAFE-006).
    FaultCode getLastFaultCode() const { return last_fault_code_; }

    // Set Last Code
    void clearLastFaultCode()  { last_fault_code_=FaultCode::NONE;state_ = SystemState::SAFE;}
    // Audit log (SWR-SAFE-006).
    const std::vector<IsmLog>& getLogs() const { return logs_; }
    void clearLogs() { logs_.clear(); }
    void enterSafe(const std::string& reason);
private:
    SystemState   state_          = SystemState::SAFE;
    bool          torque_enabled_ = false;
    uint32_t      last_seq_       = 0;
    FireInterlock interlock_;
    Watchdog      watchdog_;
    IsmEmgLink    emg_link_;
    FaultCode     last_fault_code_ = FaultCode::NONE;
    std::vector<IsmLog> logs_;

    //void enterSafe(const std::string& reason);
    void enterFault(FaultCode code, const std::string& reason);
    void log(const std::string& msg);

    bool validateMotionCommand(const FccMessage& msg);
};
