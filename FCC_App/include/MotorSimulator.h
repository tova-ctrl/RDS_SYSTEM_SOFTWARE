#pragma once
// =============================================================================
// MotorSimulator.h — Simulated Az/El servo motor for FCC testing
//
// מדמה מנוע servo עם:
//   - מיקום נוכחי (azimuth + elevation)
//   - מהירות תנועה (rate_dps)
//   - גבולות פיזיים (±180° az, ±90° el)
//   - האצה/עצירה הדרגתית
//
// על החומרה האמיתית: יוחלף בקריאת encoder דרך CAN (0x300)
// =============================================================================

#include <atomic>
#include <thread>
#include <mutex>
#include <functional>

struct MotorPosition {
    float azimuth_deg   = 0.0f;
    float elevation_deg = 0.0f;
    float rate_dps      = 0.0f;   // מהירות נוכחית
    bool  moving        = false;
};

struct MotorTarget {
    float azimuth_deg   = 0.0f;
    float elevation_deg = 0.0f;
    float rate_dps      = 10.0f;  // מהירות רצויה
};

class MotorSimulator {
public:
    // גבולות פיזיים — חייב להיות זהה ל-ISM::MAX_*
    static constexpr float MAX_AZ_DEG   = 180.0f;
    static constexpr float MAX_EL_DEG   =  90.0f;
    static constexpr float MAX_RATE_DPS =  45.0f;
    static constexpr float MIN_RATE_DPS =   1.0f;

    // callback שנקרא כל פעם שהמיקום מתעדכן
    using PositionCallback = std::function<void(const MotorPosition&)>;

    explicit MotorSimulator(PositionCallback on_update = nullptr);
    ~MotorSimulator();

    // הפעל את ה-simulation loop
    void start();
    void stop();

    // קבע יעד תנועה חדש
    void setTarget(float az, float el, float rate_dps);

    // עצור תנועה מיידית (על פקודת ISM SAFE/FAULT)
    void emergencyStop();

    // קרא מיקום נוכחי (thread-safe)
    MotorPosition getPosition() const;

    bool isRunning() const { return running_; }

private:
    MotorPosition           pos_;
    MotorTarget             target_;
    mutable std::mutex      mutex_;
    std::atomic<bool>       running_{ false };
    std::atomic<bool>       emergency_stop_{ false };
    std::thread             sim_thread_;
    PositionCallback        on_update_;

    void simLoop();
    float stepToward(float current, float target, float max_step) const;
};
