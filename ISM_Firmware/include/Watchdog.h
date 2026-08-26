#pragma once
#include <cstdint>
#include <functional>

// Platform tick provider — implemented per platform:
//   Linux/WSL : ISM_Firmware/src/platform_linux.cpp  (std::chrono)
//   STM32H7   : Core/Src/platform_stm32.cpp          (HAL_GetTick)
uint32_t platform_get_tick_ms();

// Software watchdog: must be kicked within timeout_ms or the expire callback fires.
class Watchdog {
public:
    explicit Watchdog(int timeout_ms, std::function<void()> on_expire)
        : timeout_ms_(timeout_ms), on_expire_(on_expire) {
        kick();
    }

    void kick() {
        last_kick_ms_ = platform_get_tick_ms();
        expired_ = false;
    }

    // Call periodically from the main ISM tick (every 1ms).
    // On STM32H7: called from main loop driven by HAL_GetTick().
    // On Linux/WSL: called manually by test harness.
    void tick() {
        if (expired_) return;
        uint32_t elapsed = platform_get_tick_ms() - last_kick_ms_;
        if (elapsed >= static_cast<uint32_t>(timeout_ms_)) {
            expired_ = true;
            on_expire_();
        }
    }

    bool hasExpired() const { return expired_; }

private:
    int      timeout_ms_;
    std::function<void()> on_expire_;
    uint32_t last_kick_ms_ = 0;
    bool     expired_      = false;
};
