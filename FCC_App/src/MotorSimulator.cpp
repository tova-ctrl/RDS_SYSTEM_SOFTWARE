#include "MotorSimulator.h"
#include <cmath>
#include <algorithm>
#include <thread>
#include <chrono>
#include <iostream>

using namespace std::chrono_literals;
extern std::atomic<bool> g_shutdown;
MotorSimulator::MotorSimulator(PositionCallback on_update)
    : on_update_(on_update) {}

MotorSimulator::~MotorSimulator() {
    stop();
}

void MotorSimulator::start() {
    running_       = true;
    emergency_stop_ = false;
    sim_thread_ = std::thread(&MotorSimulator::simLoop, this);
}

void MotorSimulator::stop() {
    running_ = false;
    if (sim_thread_.joinable())
        sim_thread_.join();
}

void MotorSimulator::setTarget(float az, float el, float rate_dps) {
    // clamp לגבולות פיזיים
    az       = std::clamp(az,       -MAX_AZ_DEG,   MAX_AZ_DEG);
    el       = std::clamp(el,       -MAX_EL_DEG,   MAX_EL_DEG);
    rate_dps = std::clamp(rate_dps,  MIN_RATE_DPS,  MAX_RATE_DPS);

    std::lock_guard<std::mutex> lock(mutex_);
    target_.azimuth_deg   = az;
    target_.elevation_deg = el;
    target_.rate_dps      = rate_dps;
    emergency_stop_       = false;
}

void MotorSimulator::emergencyStop() {
    emergency_stop_ = true;
    std::lock_guard<std::mutex> lock(mutex_);
    target_.azimuth_deg   = pos_.azimuth_deg;    // יעד = מיקום נוכחי → עצירה
    target_.elevation_deg = pos_.elevation_deg;
    target_.rate_dps      = 0.0f;
    pos_.moving           = false;
    pos_.rate_dps         = 0.0f;
}

MotorPosition MotorSimulator::getPosition() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pos_;
}

// ── Simulation loop — מדמה תנועת servo בזמן אמת ─────────────────────────────
// רץ כל 10ms (100Hz) — מספיק לסימולציה, encoder אמיתי ירוץ מהר יותר

void MotorSimulator::simLoop() {
    constexpr float TICK_SEC = 0.01f;   // 10ms per tick

    while (running_ && !g_shutdown) {
        {
            std::lock_guard<std::mutex> lock(mutex_);

            if (!emergency_stop_) {
                float max_step = target_.rate_dps * TICK_SEC;

                float new_az = stepToward(pos_.azimuth_deg,
                                          target_.azimuth_deg, max_step);
                float new_el = stepToward(pos_.elevation_deg,
                                          target_.elevation_deg, max_step);

                bool az_moving = std::abs(new_az - pos_.azimuth_deg)   > 0.001f;
                bool el_moving = std::abs(new_el - pos_.elevation_deg) > 0.001f;

                pos_.azimuth_deg   = new_az;
                pos_.elevation_deg = new_el;
                pos_.moving        = az_moving || el_moving;
                pos_.rate_dps      = pos_.moving ? target_.rate_dps : 0.0f;
            }

            // callback למי שמאזין (FccController)
            if (on_update_) on_update_(pos_);
        }

        std::this_thread::sleep_for(10ms);
    }
}

float MotorSimulator::stepToward(float current, float target, float max_step) const {
    float diff = target - current;
    if (std::abs(diff) <= max_step) return target;
    return current + (diff > 0 ? max_step : -max_step);
}
