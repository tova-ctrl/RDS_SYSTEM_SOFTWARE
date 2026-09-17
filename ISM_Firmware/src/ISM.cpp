// =============================================================================
// ISM.cpp — Independent Safety Monitor (logic layer)
//
// !! PLATFORM NOTE — STM32H7 integration !!
// This file contains ONLY the ISM logic. It has no dependency on any
// hardware peripheral. When porting to STM32H7, create a separate file
// (e.g. stm32h7_hal_glue.cpp) and add the following three hooks:
//
//   1. TIM6 1ms tick → calls ISM::tick()
//      void TIM6_DAC_IRQHandler(void) {
//          HAL_TIM_IRQHandler(&htim6);
//          ism.tick();                       // ← hook here
//      }
//
//   2. FDCAN1 receive → calls ISM::processMessage()
//      void FDCAN1_IT0_IRQHandler(void) {
//          HAL_FDCAN_IRQHandler(&hfdcan1);
//          CAN_Frame f; FDCAN_Read(&f);
//          ism.processMessage(f);            // ← hook here
//      }
//
//   3. GPIO torque-disable line → driven from ISM state after every tick
//      GPIO_Write(TORQUE_PIN, ism.isTorqueEnabled() ? HIGH : LOW); // ← hook here
//
//   4. IWDG hardware watchdog → kick inside the TIM6 handler (before ism.tick)
//      IWDG_Refresh();                       // ← hook here (priority 1)
//
// DO NOT add any STM32 HAL includes or #ifdefs to this file.
// Keep ISM.cpp portable — it must compile and run on Linux/WSL for testing.
// =============================================================================

#include "ISM.h"
#include <sstream>
#include <cmath>
#include <iostream>

// Implemented per-platform:
//   Linux/WSL : ISM_Firmware/src/platform_linux.cpp  (wall-clock HH:MM:SS)
//   STM32H7   : Core/Src/platform_stm32.cpp          (T+Xs uptime)
extern std::string now_str();

ISM::ISM()
    : watchdog_(WATCHDOG_TIMEOUT_MS, [this]() {
          enterFault(FaultCode::COMM_LOSS,
                     "WATCHDOG: Heartbeat timeout — no FCC message within " +
                     std::to_string(WATCHDOG_TIMEOUT_MS) + "ms");
      })
{
    log("ISM powered up. Entering SAFE state. (SWR-SAFE-001)");
    enterSafe("Power-up initialisation");
}

void ISM::tick() {
    // Called every 1ms.
    // On Linux/WSL tests: called manually by the test harness after sleep().
    // On STM32H7: called from TIM6_DAC_IRQHandler() — see platform note above.
    watchdog_.tick();
}

bool ISM::processMessage(const FccMessage& msg) {

    switch (msg.type) {

        case FccCommandType::HEARTBEAT:
            // Heartbeat has its own sequence counter — does not affect cmd_seq.
            watchdog_.kick();
          //  log("HEARTBEAT seq=" + std::to_string(msg.seq) + " from " + msg.source);
            return true;

        case FccCommandType::RECOVERY_RESET:
            // Recovery bypasses seq check — operator override must always work,
            // even if seq got out of sync due to a prior fault or failed injection.
            return requestRecovery(msg.source);

        default:
            break;
    }

    // Sequence gap check for non-heartbeat commands only.
    // seq=0 means "UART command — no sequence tracking", bypass the check.
    if (msg.seq != 0) {
        if (msg.seq != last_seq_ + 1) {
            enterFault(FaultCode::SEQ_ERROR,
                       "SEQ ERROR: expected " + std::to_string(last_seq_ + 1) +
                       " got " + std::to_string(msg.seq) + " from " + msg.source);
            return false;
        }
        last_seq_ = msg.seq;
    }

    switch (msg.type) {

        case FccCommandType::STANDBY:
            if (state_ != SystemState::SAFE) {
                log("STANDBY rejected: current state is " + stateToString(state_)+"\n");
                return false;
            }
            state_ = SystemState::STANDBY;
            // 2026-09-14: dual-channel FIRE interlock (SWR-SAFE-004) — channel A
            // is tied to reaching STANDBY, channel B to reaching ARMED (below),
            // set automatically by the state machine itself, not by separate
            // operator-triggered messages (superseded the 2026-09-09 CAN_ID_
            // SAFETY_CHANNEL_A/B design — see [[mobilicom-controller-mapping]]
            // memory for that history).
            interlock_.setChannelA(true);
            log("State → STANDBY. System ready, torque still off.\n");
            return true;

        case FccCommandType::ARM:
            if (state_ != SystemState::STANDBY) {
                log("ARM rejected: current state is " + stateToString(state_)+"\n");
                return false;
            }
            state_ = SystemState::ARMED;
            torque_enabled_ = true;
            // 2026-09-09: interlock_.arm() was missing entirely — without it,
            // isFirePermitted() (armed_ && channel_a_ && channel_b_) could never
            // return true no matter what the channels were set to, since
            // interlock_'s own armed_ flag never left its default false.
            // 2026-09-14: channel B set here too, automatically — see the
            // STANDBY case above for the paired channel A / full design note.
            interlock_.arm();
            interlock_.setChannelB(true);
            log("State → ARMED. Torque enabled. (SWR-SAFE-002)\n");
            return true;

        case FccCommandType::DISARM:
            if (state_ == SystemState::ARMED||state_ == SystemState::FIRING) {
                state_ = SystemState::STANDBY;
                torque_enabled_ = false;
                interlock_.disarm();
                // 2026-09-14: re-set channel A since we land back in STANDBY —
                // keeps "state STANDBY ⇒ channel A set" true regardless of path
                // (power-up STANDBY vs. DISARM-from-ARMED), per user confirmation.
                interlock_.setChannelA(true);
                log("State → STANDBY (DISARM from ARMED). Torque disabled.\n");
                return true;
            }
            if (state_ == SystemState::STANDBY) {
                enterSafe("DISARM from STANDBY");
                return true;
            }
            log("DISARM rejected: must be ARMED or STANDBY (state=" + stateToString(state_) + ")\n");
            return false;

        case FccCommandType::MOTION_COMMAND:
            return validateMotionCommand(msg);

        case FccCommandType::FIRE_REQUEST:
            if (state_ != SystemState::ARMED) {
                log("FIRE REQUEST rejected: not in ARMED state (state=" + stateToString(state_) + ")\n");
                return false;
            }
            if (!interlock_.isFirePermitted()) {
                log("FIRE REQUEST rejected: interlock not satisfied. " + interlock_.status() + " (SWR-SAFE-004)");
                return false;
            }
            state_ = SystemState::FIRING;
            log("FIRING authorised. " + interlock_.status() + " (SWR-SAFE-004)\n");
            return true;

        case FccCommandType::RECOVERY_RESET:
            return requestRecovery(msg.source);

        case static_cast<FccCommandType>(0xFE):
            enterFault(FaultCode::INTERNAL_ERROR,
                       "INTERNAL FAULT: safety-monitor self-check failure (SWR-SAFE-001)\n");
            return false;

        default:
            enterFault(FaultCode::INVALID_COMMAND,
                       "INVALID COMMAND type=" + std::to_string(static_cast<int>(msg.type)));
            return false;
    }
}

bool ISM::validateMotionCommand(const FccMessage& msg) {
    // SWR-SAFE-003: validate range, rate, direction, current state, limit status.
    if (state_ != SystemState::ARMED) {
        log("MOTION rejected: not ARMED (state=" + stateToString(state_) + ")");
        return false;
    }

    const auto& m = msg.motion;

    if (std::abs(m.azimuth_deg) > MAX_AZIMUTH_DEG) {
        enterFault(FaultCode::AZ_OUT_OF_RANGE,
                   "MOTION: azimuth out of range (" + std::to_string(m.azimuth_deg) +
                   "° > ±" + std::to_string(MAX_AZIMUTH_DEG) + "°) (SWR-SAFE-003)");
        return false;
    }
    if (std::abs(m.elevation_deg) > MAX_ELEVATION_DEG) {
        enterFault(FaultCode::EL_OUT_OF_RANGE,
                   "MOTION: elevation out of range (" + std::to_string(m.elevation_deg) +
                   "° > ±" + std::to_string(MAX_ELEVATION_DEG) + "°) (SWR-SAFE-003)");
        return false;
    }
    if (m.rate_dps > MAX_RATE_DPS) {
        enterFault(FaultCode::RATE_EXCEEDED,
                   "MOTION: rate exceeds limit (" + std::to_string(m.rate_dps) +
                   " dps > " + std::to_string(MAX_RATE_DPS) + " dps) (SWR-SAFE-003)");
        return false;
    }

    log("MOTION accepted: az=" + std::to_string(m.azimuth_deg) +
        "° el=" + std::to_string(m.elevation_deg) +
        "° rate=" + std::to_string(m.rate_dps) + "dps");
    return true;
}

bool ISM::requestRecovery(const std::string& authority) {
    // SWR-SAFE-008: controlled recovery — operator may command SAFE from any state.
    if (state_ == SystemState::SAFE) {
        log("RECOVERY: already in SAFE state");
        return true;
    }
    if (authority.empty()) {
        log("RECOVERY rejected: no authority provided");
        return false;
    }
    log("RECOVERY authorised by '" + authority + "'. " + stateToString(state_) + " → SAFE. (SWR-SAFE-008)");
    last_seq_ = 0;
    watchdog_.kick();
    enterSafe("RECOVERY by " + authority);
    return true;
}

bool ISM::initEmgLink(const std::string& webapp_ip, uint16_t port) {
    return emg_link_.init(webapp_ip, port);
}

void ISM::enterSafe(const std::string& reason) {
    bool was_fault = (state_ == SystemState::FAULT);
    state_ = SystemState::SAFE;
    torque_enabled_ = false;
    interlock_.disarm();
    log("→ SAFE: " + reason + " (SWR-SAFE-001)");
    // Notify WebApp only on recovery from FAULT — not on routine DISARM or power-up.
    if (was_fault) {
        emg_link_.send(SystemState::SAFE, FaultCode::NONE, "RECOVERY: " + reason);
    }
}

void ISM::enterFault(FaultCode code, const std::string& reason) {
    last_fault_code_ = code;
    state_           = SystemState::FAULT;
    torque_enabled_  = false;
    interlock_.disarm();
    log("→ FAULT [" + faultCodeToHex(code) + " " + faultCodeToString(code) +
        "]: " + reason + " (SWR-SAFE-006)");
    emg_link_.send(SystemState::FAULT, code, reason);
}

void ISM::log(const std::string& msg) {
    std::string ts = now_str();
    logs_.push_back({ ts, msg });
    std::cout << "[ISM " << ts << "] " << msg << "\n";
    if (logs_.size() > 200) logs_.erase(logs_.begin());
}
