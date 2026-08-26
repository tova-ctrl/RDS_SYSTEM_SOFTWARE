// Linux/WSL implementations of ISM platform functions.
// STM32 equivalents are in ISM_STM32/Core/Src/platform_stm32.cpp.
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstdint>

uint32_t platform_get_tick_ms() {
    static auto start = std::chrono::steady_clock::now();
    return static_cast<uint32_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start).count());
}

std::string now_str() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    localtime_r(&t, &tm);
    std::ostringstream ss;
    ss << std::put_time(&tm, "%H:%M:%S");
    return ss.str();
}
