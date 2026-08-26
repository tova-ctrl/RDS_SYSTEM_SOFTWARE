// STM32H7 implementations of ISM platform functions.
// Linux equivalents are in ISM_Firmware/src/platform_linux.cpp.
#include "stm32h7xx_hal.h"
#include <cstdint>
#include <cstdio>
#include <string>

uint32_t platform_get_tick_ms() {
    return HAL_GetTick();
}

// Returns uptime string "T+Xs" instead of wall-clock time.
std::string now_str() {
    uint32_t ms = HAL_GetTick();
    char buf[16];
    snprintf(buf, sizeof(buf), "T+%lus", (unsigned long)(ms / 1000));
    return std::string(buf);
}
