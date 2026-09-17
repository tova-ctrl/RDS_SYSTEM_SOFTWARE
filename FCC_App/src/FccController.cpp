#include "FccController.h"
#include "CanBus.h"
#include "UartProtocol.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <thread>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <unistd.h>
#include <endian.h> 
#include <termios.h> 
using namespace std::chrono_literals;
extern std::atomic<bool> g_shutdown;
extern int uart_fd;
const static auto program_start_time = std::chrono::steady_clock::now();
FccController::FccController(ICanBus& can, GcsLink& gcs)
    : can_(can), gcs_(gcs)
    , motor_([this](const MotorPosition& pos) {
          // callback: עדכן state בכל tick של המנוע
          std::lock_guard<std::mutex> lock(state_mutex_);
          state_.azimuth_deg    = pos.azimuth_deg;
          state_.elevation_deg  = pos.elevation_deg;
          state_.motor_rate_dps = pos.rate_dps;
          state_.motor_moving   = pos.moving;
      })
{}

// ── Start / Stop ─────────────────────────────────────────────────────────────

void FccController::start() {
    running_ = true;

    log("FCC starting. CAN=" + can_.interface());

    if (camera_api_.init())
        log("Camera SDK link initialized (Micro300 stkAPI).");
    else
        log("Camera SDK link init FAILED: " + camera_api_.lastError());

    motor_.start();
    heartbeat_thread_      = std::thread(&FccController::heartbeatLoop, this);
    uart_receive_thread_   = std::thread(&FccController::uartReceiveLoop, this);
    can_receive_thread_    = std::thread(&FccController::canReceiveLoop, this);
    gcs_poll_thread_ = std::thread(&FccController::gcsPollLoop, this);
    motor_cmd_thread_ = std::thread(&FccController::motorCmdLoop, this);

    log("FCC running.");

}
void FccController::stop() {
    running_ = false;
    motor_.stop();
    stopCamera(); 
    if (heartbeat_thread_.joinable())      heartbeat_thread_.join();
    if (uart_receive_thread_.joinable())   uart_receive_thread_.join();
    if (can_receive_thread_.joinable())    can_receive_thread_.join();
    if (gcs_poll_thread_.joinable())    gcs_poll_thread_.join();
    if (motor_cmd_thread_.joinable())   motor_cmd_thread_.join();
    log("FCC stopped.");
}

// ── Command senders ───────────────────────────────────────────────────────────

bool FccController::sendStandby() {
    std::lock_guard<std::mutex> lock(state_mutex_);
    if (uart_fd > 0) {
        const char prefix[] = "STBY:";
        uint8_t crc = uart_crc8((const uint8_t*)prefix, (uint8_t)std::strlen(prefix));
        char msg[16];
        int len = std::snprintf(msg, sizeof(msg), "%s%02X:AA\n", prefix, crc);
        ssize_t w = write(uart_fd, msg, (size_t)len);
        log(w > 0 ? "STANDBY sent to ISM (UART)" : "STANDBY send FAILED (UART)");
        return w > 0;
    }
    auto frame = buildStandby(++state_.cmd_seq);
    bool ok = can_.send(frame);
    log(ok ? "STANDBY sent to ISM" : "STANDBY send FAILED: " + can_.lastError());
    return ok;
}

bool FccController::sendArm() {
    std::lock_guard<std::mutex> lock(state_mutex_);
    if (uart_fd > 0) {
        const char prefix[] = "ARM:";
        uint8_t crc = uart_crc8((const uint8_t*)prefix, (uint8_t)std::strlen(prefix));
        char msg[16];
        int len = std::snprintf(msg, sizeof(msg), "%s%02X:AA\n", prefix, crc);
        ssize_t w = write(uart_fd, msg, (size_t)len);
        log(w > 0 ? "ARM sent to ISM (UART)" : "ARM send FAILED (UART)");
        return w > 0;
    }
    auto frame = buildArm(++state_.cmd_seq);
    bool ok = can_.send(frame);
    log(ok ? "ARM sent to ISM" : "ARM send FAILED: " + can_.lastError());
    return ok;
}

bool FccController::sendDisarm() {
    std::lock_guard<std::mutex> lock(state_mutex_);
    if (uart_fd > 0) {
        const char prefix[] = "SAFE:";
        uint8_t crc = uart_crc8((const uint8_t*)prefix, (uint8_t)std::strlen(prefix));
        char msg[16];
        int len = std::snprintf(msg, sizeof(msg), "%s%02X:AA\n", prefix, crc);
        ssize_t w = write(uart_fd, msg, (size_t)len);
        log(w > 0 ? "SAFE sent to ISM (UART)" : "SAFE send FAILED (UART)");
        return w > 0;
    }
    auto frame = buildDisarm(++state_.cmd_seq);
    bool ok = can_.send(frame);
    log(ok ? "DISARM sent to ISM" : "DISARM send FAILED: " + can_.lastError());
    return ok;
}

bool FccController::sendMotion(float az, float el, float rate) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    auto frame = buildMotion(++state_.cmd_seq, az, el, rate);
    bool ok = can_.send(frame);
    if (ok) {
        state_.azimuth_deg   = az;
        state_.elevation_deg = el;
    }
    log(ok ? "MOTION sent: az=" + std::to_string(az) +
             " el=" + std::to_string(el)
           : "MOTION send FAILED");
    return ok;
}

bool FccController::sendFireRequest() {
    std::lock_guard<std::mutex> lock(state_mutex_);
    if (uart_fd > 0) {
        const char prefix[] = "FIRE:";
        uint8_t crc = uart_crc8((const uint8_t*)prefix, (uint8_t)std::strlen(prefix));
        char msg[16];
        int len = std::snprintf(msg, sizeof(msg), "%s%02X:AA\n", prefix, crc);
        ssize_t w = write(uart_fd, msg, (size_t)len);
        log(w > 0 ? "FIRE REQUEST sent to ISM (UART)" : "FIRE REQUEST send FAILED (UART)");
        return w > 0;
    }
    auto frame = buildFireRequest(++state_.cmd_seq);
    bool ok = can_.send(frame);
    log(ok ? "FIRE REQUEST sent to ISM" : "FIRE REQUEST send FAILED");
    return ok;
}

bool FccController::sendRecoveryReset() {
    std::lock_guard<std::mutex> lock(state_mutex_);
    if (uart_fd > 0) {
        const char prefix[] = "SAFE:";
        uint8_t crc = uart_crc8((const uint8_t*)prefix, (uint8_t)std::strlen(prefix));
        char msg[16];
        int len = std::snprintf(msg, sizeof(msg), "%s%02X:AA\n", prefix, crc);
        ssize_t w = write(uart_fd, msg, (size_t)len);
        log(w > 0 ? "SAFE sent to ISM (UART)" : "SAFE send FAILED (UART)");
        return w > 0;
    }
    auto frame = buildRecoveryReset(++state_.cmd_seq);
    bool ok = can_.send(frame);
    if (ok) {
        state_.cmd_seq = 0;  // ISM resets last_seq_=0 on recovery — stay in sync

    }
    log(ok ? "RECOVERY RESET sent to ISM (cmd_seq reset)" : "RECOVERY RESET send FAILED");
    return ok;
}

bool FccController::sendSafetyChannelA(bool open) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    auto frame = buildSafetyChannelA(open);
    bool ok = can_.send(frame);
    log(ok ? (std::string("SAFETY CHANNEL A ") + (open ? "OPEN" : "CLOSED") + " sent to ISM")
           : "SAFETY CHANNEL A send FAILED");
    return ok;
}

bool FccController::sendSafetyChannelB(bool open) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    auto frame = buildSafetyChannelB(open);
    bool ok = can_.send(frame);
    log(ok ? (std::string("SAFETY CHANNEL B ") + (open ? "OPEN" : "CLOSED") + " sent to ISM")
           : "SAFETY CHANNEL B send FAILED");
    return ok;
}

bool FccController::injectFault(const std::string& type) {
    if (type == "INVALID_CMD") {
        std::lock_guard<std::mutex> lock(state_mutex_);
        auto frame = buildInjectInvalidCmd(++state_.cmd_seq);
        bool ok = can_.send(frame);
        log(ok ? "INJECT INVALID_CMD sent (CAN 0x1FF)" : "INJECT INVALID_CMD FAILED");
        return ok;
    }
    if (type == "RESET") {
        std::lock_guard<std::mutex> lock(state_mutex_);
        auto frame = buildInjectReset(++state_.cmd_seq);
        bool ok = can_.send(frame);
        if (ok) state_.cmd_seq = 0;  // ISM resets seq on reset, stay in sync
        log(ok ? "INJECT RESET sent (CAN 0x1FE)" : "INJECT RESET FAILED");
        return ok;
    }
    if (type == "INTERNAL_FAULT") {
        std::lock_guard<std::mutex> lock(state_mutex_);
        auto frame = buildInjectInternal(++state_.cmd_seq);
        bool ok = can_.send(frame);
        log(ok ? "INJECT INTERNAL_FAULT sent (CAN 0x1FD)" : "INJECT INTERNAL_FAULT FAILED");
        return ok;
    }
    if (type == "WATCHDOG" || type == "COMM_LOSS") {
        // Pause heartbeats for 600ms — ISM watchdog (500ms) fires → FAULT(COMM_LOSS)
        log("INJECT " + type + ": pausing heartbeats for 600ms");
        std::thread([this, type]() {
            pause_heartbeats_ = true;
            std::this_thread::sleep_for(std::chrono::milliseconds(600));
            pause_heartbeats_ = false;
            log("INJECT " + type + ": heartbeats resumed");
        }).detach();
        return true;
    }
    // ── SWR-SAFE-003: motion boundary-value injection (ISM must be ARMED) ──────
    if (type == "AZ_RANGE" || type == "EL_RANGE" || type == "RATE_EXCEED") {
        bool need_wait = false;
        {
            std::lock_guard<std::mutex> lock(state_mutex_);
            if (state_.ism_state == ISM_STATE_STANDBY) {
                auto arm = buildArm(++state_.cmd_seq);
                can_.send(arm);
                log("INJECT " + type + ": auto-ARM from STANDBY, waiting 300ms...");
                need_wait = true;
            } else if (state_.ism_state != ISM_STATE_ARMED) {
                log("INJECT " + type + ": ISM must be STANDBY or ARMED (state=" +
                    std::string(ismStateStr(state_.ism_state)) + ")");
                return false;
            }
        }
        if (need_wait)
            std::this_thread::sleep_for(std::chrono::milliseconds(300));

        std::lock_guard<std::mutex> lock(state_mutex_);
        CanFrame frame;
        if      (type == "AZ_RANGE")    frame = buildMotion(++state_.cmd_seq, 200.0f, 0.0f,  10.0f);
        else if (type == "EL_RANGE")    frame = buildMotion(++state_.cmd_seq, 0.0f,   100.0f, 10.0f);
        else                             frame = buildMotion(++state_.cmd_seq, 0.0f,   0.0f,   60.0f);
        bool ok = can_.send(frame);
        log(ok ? "INJECT " + type + ": sent (CAN 0x110)"
               : "INJECT " + type + " FAILED");
        return ok;
    }
    if (type == "NOT_ARMED") {
        // Ensure ISM is not ARMED before injecting — auto-disarm if needed
        {
            std::lock_guard<std::mutex> lock(state_mutex_);
            if (state_.ism_state == ISM_STATE_ARMED) {
                auto frame = buildRecoveryReset(++state_.cmd_seq);
                can_.send(frame);
                state_.cmd_seq = 0;
                log("INJECT NOT_ARMED: auto-safe (ISM was ARMED), waiting 300ms...");
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        std::lock_guard<std::mutex> lock(state_mutex_);
        auto frame = buildMotion(++state_.cmd_seq, 0.0f, 0.0f, 10.0f);
        bool ok = can_.send(frame);
        log(ok ? "INJECT NOT_ARMED: MOTION sent while ISM not ARMED — expect rejection (log only)"
               : "INJECT NOT_ARMED FAILED");
        return ok;
    }
    log("injectFault: unknown type '" + type + "'");
    return false;
}

FccState FccController::getState() const {
    std::lock_guard<std::mutex> lock(state_mutex_);
    return state_;
}

// ── Camera / Gimbal ───────────────────────────────────────────────────────────

bool FccController::startCamera(const std::string& camera_ip, uint16_t rtsp_port) {
    camera_ = std::make_unique<CameraStream>(
        camera_ip, rtsp_port,
        [this](const VideoFrame& frame) {
            // callback: כל frame מה-Micro300
            // כאן אפשר להוסיף: overlay az/el, forward ל-GCS, שמירה לדיסק
            (void)frame;
        });

    gimbal_ = std::make_unique<GimbalControl>(camera_ip);
    gimbal_->open();

    bool ok = camera_->start();
    if (ok)
        log("Micro300 stream started at " + camera_ip +
            ":" + std::to_string(rtsp_port));
    else
        log("Micro300 stream FAILED: " + camera_->lastError());
    return ok;
}

void FccController::stopCamera() {
    if (camera_) { camera_->stop(); camera_.reset(); }
    if (gimbal_) { gimbal_->close(); gimbal_.reset(); }
    log("Camera stopped.");
}

bool FccController::pointGimbal(float az_deg, float el_deg) {
    if (!gimbal_) { log("Gimbal not initialized"); return false; }
    return gimbal_->pointTo(az_deg, el_deg);
}

bool FccController::setZoomVis(float factor) {
    if (!gimbal_) return false;
    return gimbal_->setZoomVis(factor);
}

bool FccController::setZoomIR(float factor) {
    if (!gimbal_) return false;
    return gimbal_->setZoomIR(factor);
}

CameraStatus FccController::getCameraStatus() const {
    if (!camera_) return CameraStatus{};
    return camera_->getStatus();
}

GimbalStatus FccController::getGimbalStatus() const {
    if (!gimbal_) return GimbalStatus{};
    return gimbal_->getStatus();
}

// ── Motor control ────────────────────────────────────────────────────────────

void FccController::motorMoveTo(float az, float el, float rate_dps) {
    motor_.setTarget(az, el, rate_dps);
    log("MOTOR target → az=" + std::to_string(az) +
        " el=" + std::to_string(el) +
        " rate=" + std::to_string(rate_dps) + "dps");
}

void FccController::motorStop() {
    motor_.emergencyStop();
    log("MOTOR emergency stop.");
}

// ── Motor command loop — שולח motion command לISM כל 100ms כשהמנוע זז ────────

void FccController::motorCmdLoop() {
    while (running_ && !g_shutdown) {
        {
            std::lock_guard<std::mutex> lock(state_mutex_);

            // שלח motion command רק כשה-ISM ב-ARMED והמנוע זז
            if (state_.ism_state == ISM_STATE_ARMED && state_.motor_moving) {
                auto frame = buildMotion(++state_.cmd_seq,
                                         state_.azimuth_deg,
                                         state_.elevation_deg,
                                         state_.motor_rate_dps);
                can_.send(frame);
            }
        }
        std::this_thread::sleep_for(100ms);
    }
}

// ── Heartbeat loop (100ms) ────────────────────────────────────────────────────
std::string to_debug_string(std::chrono::steady_clock::time_point tp) {
    //calculate ms passed from the startup.
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tp - program_start_time).count();
    return std::to_string(ms) + " ms";
}
#define SWAP_UINT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))
std::string FccController::read_serial_line(int uart_fd) {
    std::string line = "";
    char c;
    int bytes;

    // קרא תו בודד בכל פעם עד שנגמר הצינור או שמגיעים ל-\n
    while ((bytes = read(uart_fd, &c, 1)) > 0) {
        if (c == '\n') {
            break;
        }
        if (c != '\r') { // התעלם מתווי קצה של ווינדוס במידה ויש
            line += c;
        }
    }
    return line;
}
/*
bool FccController::heartbeatRead()
{
    // 2. קריאת השורה שחוזרת בטקסט נקי מה-STM32
    std::string response = read_serial_line(uart_fd);

    if (!response.empty()) {
      //  std::cout << "[RX] Response received: " << response << "\n";

        // בדיקה האם ה-ACK המבוקש נמצא בתוך הטקסט
        if (response.find("ID_HEARTBEAT_ACK 0x211") != std::string::npos) {
          //  std::cout << ">> Heartbeat Verified Successfully! <<\n";
            return true;
        }
    }
    else {
       // std::cout << "[WARN] Timeout - No response from ISM!\n";
    }
 
    return false;
}*/
void FccController::heartbeatLoop() {
    while (running_ && !g_shutdown) {
        {
            std::lock_guard<std::mutex> lock(state_mutex_);

            if (!pause_heartbeats_) {
                auto frame = buildHeartbeat(state_.heartbeat_seq++);
                if (uart_fd > 0) {
                    uint16_t seq = (uint16_t)(state_.heartbeat_seq & 0xFFFFu);
                    char prefix[16];
                    std::snprintf(prefix, sizeof(prefix), "HB:%04X:", seq);  
                    uint8_t crc = uart_crc8((const uint8_t*)prefix, (uint8_t)std::strlen(prefix));
                   
                    char msg[24];
                    int len = std::snprintf(msg, sizeof(msg), "%s%02X:AA\n", prefix, crc); 
                   // std::cout << "CRC Decimal: " << static_cast<int>(crc) << std::endl;
                    ssize_t bytes_written = write(uart_fd, msg, (size_t)len);
                    if (bytes_written < 0)
                        std::cout << "[TX] Sent Heartbeat FAILED\n";
                   //==23.7.26=Tova== else std::cout << "msg :" << msg << std::to_string(len)<<std::to_string(bytes_written)<<"\n";
                } else {
                    state_.can_ok = can_.send(frame);
                    if (!state_.can_ok)
                        log("HEARTBEAT send failed: " + can_.lastError());
                }
            }

            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - last_ism_ack_time).count();
            if (elapsed > 6000 && state_.ism_state != ISM_STATE_COMM_LOSS) {
                state_.ism_save_state = state_.ism_state;
                state_.ism_state = ISM_STATE_COMM_LOSS;
                log("ISM ACK timeout — COMM_LOSS reported to GCS");
                gcs_.sendStateUpdate(ISM_STATE_COMM_LOSS, false,
                                     state_.azimuth_deg, state_.elevation_deg);
            }
        }
        std::this_thread::sleep_for(500ms);
    }
}

void FccController::uartReceiveLoop() {
    while (running_ && !g_shutdown) {
        if (uart_fd <= 0) {
            std::this_thread::sleep_for(100ms);
            continue;
        }

        std::string line = read_serial_line(uart_fd);
        //std::cout << "1-ISM send ACK -" << line << "\n";
        if (line.empty()) continue;

        // Validate CRC: everything up to and including the last ':' is the CRC input
        auto crc_pos = line.rfind(':');
       if (crc_pos == std::string::npos || crc_pos + 1 >= line.size()) continue;

        std::string prefix = line.substr(0, crc_pos + 1);
        uint8_t expected = uart_crc8((const uint8_t*)prefix.c_str(), (uint8_t)prefix.size());
        uint8_t received = (uint8_t)std::strtoul(line.c_str() + crc_pos + 1, nullptr, 16);
       if (expected != received) continue;

        if (line.compare(0, 8, "ACK:211:") == 0) {
            std::lock_guard<std::mutex> lock(state_mutex_);
            last_ism_ack_time = std::chrono::steady_clock::now();
           //===23.7.Tova state_.ism_state = ISM_STATE_SAFE;
            if (state_.ism_state== ISM_STATE_COMM_LOSS) state_.ism_state = state_.ism_save_state;
        } else if (line.compare(0, 8, "ACK:103:") == 0) {
            {
                std::lock_guard<std::mutex> lock(state_mutex_);
                state_.ism_state = ISM_STATE_STANDBY;
            }
            log("ISM -> STANDBY (UART ACK)");
            std::cout << "1-ISM send STANDBY ACK -" << line << "\n";
            gcs_.sendStateUpdate(ISM_STATE_STANDBY, false,
                                 state_.azimuth_deg, state_.elevation_deg);
        } else if (line.compare(0, 8, "ACK:101:") == 0) {
            {
                std::lock_guard<std::mutex> lock(state_mutex_);
                state_.ism_state = ISM_STATE_ARMED;
                state_.torque_enabled = true;
            }
            log("ISM -> ARMED (UART ACK)");
            gcs_.sendStateUpdate(ISM_STATE_ARMED, true,
                                 state_.azimuth_deg, state_.elevation_deg);
        } else if (line.compare(0, 8, "ACK:102:") == 0) {
            // DISARM result depends on prior state:
            //   was ARMED   → ISM moved to STANDBY
            //   was STANDBY → ISM moved to SAFE
            uint8_t new_state;
            {
                std::lock_guard<std::mutex> lock(state_mutex_);
                new_state = (state_.ism_state == ISM_STATE_ARMED)
                            ? ISM_STATE_STANDBY : ISM_STATE_SAFE;
                state_.ism_save_state = state_.ism_state;
                state_.ism_state    = new_state;
                state_.torque_enabled = false;
            }
            log(std::string("ISM -> ") + ismStateStr(new_state) + " (UART DISARM ACK)");
            gcs_.sendStateUpdate(new_state, false,
                                 state_.azimuth_deg, state_.elevation_deg);
        } else if (line.compare(0, 8, "ACK:120:") == 0) {
            {
                std::lock_guard<std::mutex> lock(state_mutex_);
                state_.ism_state = ISM_STATE_FIRING;
                state_.torque_enabled = true;
            }
            log("ISM -> FIRING (UART ACK)");
            gcs_.sendStateUpdate(ISM_STATE_FIRING, true,
                                 state_.azimuth_deg, state_.elevation_deg);
        }
    }
}

// ── CAN receive loop ──────────────────────────────────────────────────────────

void FccController::canReceiveLoop() {
    while (running_ && !g_shutdown) {
        CanFrame frame;
        // Block up to 50ms waiting for a frame, then loop to check running_.
        if (can_.receiveTimeout(frame, 50)) {
           /* std::ostringstream ss;
            ss << std::hex << frame.id;
            log("ISM before start state → " + ss.str()); 
            */
            dispatchCanFrame(frame);
        }
        
    }
}

// ── GCS poll loop ─────────────────────────────────────────────────────────────

void FccController::gcsPollLoop() {
    while (running_ && !g_shutdown) {
        std::string cmd = gcs_.pollCommand();
        if (!cmd.empty()) {
            log("GCS command received: " + cmd);
            // Simple keyword dispatch — replace with JSON parser in production.
            if (cmd.find("\"cmd\":\"standby\"")   != std::string::npos) sendStandby();
            if (cmd.find("\"cmd\":\"arm\"")      != std::string::npos) sendArm();
            if (cmd.find("\"cmd\":\"disarm\"")   != std::string::npos) sendDisarm();
            if (cmd.find("\"cmd\":\"fire\"")     != std::string::npos) sendFireRequest();
            if (cmd.find("\"cmd\":\"recovery\"") != std::string::npos) sendRecoveryReset();
            if (cmd.find("\"cmd\":\"move\"")     != std::string::npos) {
                // פורמט: {"cmd":"move","az":45.0,"el":20.0,"rate":15.0}
                auto extract = [&](const std::string& key) -> float {
                    auto p = cmd.find("\"" + key + "\":");
                    if (p == std::string::npos) return 0.0f;
                    return std::stof(cmd.substr(p + key.size() + 3));
                };
                float az   = extract("az");
                float el   = extract("el");
                float rate = extract("rate");
                if (rate < 1.0f) rate = 10.0f;
                motorMoveTo(az, el, rate);
            }
            if (cmd.find("\"cmd\":\"stop\"") != std::string::npos) motorStop();

            // ── Camera / Gimbal commands ──────────────────────────────────
            if (cmd.find("\"cmd\":\"cam_start\"") != std::string::npos) {
                auto extract_str = [&](const std::string& key) -> std::string {
                    auto p = cmd.find("\"" + key + "\":\"");
                    if (p == std::string::npos) return "";
                    p += key.size() + 4;
                    auto end = cmd.find("\"", p);
                    return cmd.substr(p, end - p);
                };
                std::string ip = extract_str("ip");
                if (ip.empty()) ip = "192.168.1.200";  // Micro300 default IP
                startCamera(ip, 554);
            }
            if (cmd.find("\"cmd\":\"cam_stop\"")  != std::string::npos) stopCamera();
            if (cmd.find("\"cmd\":\"gimbal\"")    != std::string::npos) {
                auto extract = [&](const std::string& key) -> float {
                    auto p = cmd.find("\"" + key + "\":");
                    if (p == std::string::npos) return 0.0f;
                    return std::stof(cmd.substr(p + key.size() + 3));
                };
                pointGimbal(extract("az"), extract("el"));
            }
            if (cmd.find("\"cmd\":\"zoom_vis\"")  != std::string::npos) {
                auto p = cmd.find("\"factor\":");
                if (p != std::string::npos)
                    setZoomVis(std::stof(cmd.substr(p + 9)));
            }
            if (cmd.find("\"cmd\":\"zoom_ir\"")   != std::string::npos) {
                auto p = cmd.find("\"factor\":");
                if (p != std::string::npos)
                    setZoomIR(std::stof(cmd.substr(p + 9)));
            }
        }
        std::this_thread::sleep_for(20ms);
    }
}

// ── CAN frame dispatcher ──────────────────────────────────────────────────────

void FccController::dispatchCanFrame(const CanFrame& frame) {
    switch (frame.id) {
        case CAN_ID_STATE_ACK:
            onStateAck(parseStateAck(frame));
            break;
        case CAN_ID_FAULT_REPORT:
            onFaultReport(parseFaultReport(frame));
            break;
        case CAN_ID_HEARTBEAT_ACK: {
            onHeartbeatAck(parseHeartbeatAck(frame));
          
        }
        break;
        default:
            log("Unknown CAN ID: 0x" + [&](){
                std::ostringstream ss;
                ss << std::hex << frame.id;
                return ss.str();
            }());
    }
}

void FccController::onStateAck(const IsmStateAck& ack) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    last_ism_ack_time = std::chrono::steady_clock::now();
    state_.ism_state     = ack.new_state;
    state_.torque_enabled = (ack.new_state == ISM_STATE_ARMED ||
                             ack.new_state == ISM_STATE_FIRING);
    log("ISM state → " + std::string(ismStateStr(ack.new_state)));
    gcs_.sendStateUpdate(state_.ism_state, state_.torque_enabled,
                         state_.azimuth_deg, state_.elevation_deg);
    
}

void FccController::onFaultReport(const IsmFaultReport& report) {
    std::lock_guard<std::mutex> lock(state_mutex_);
    state_.ism_state = report.state;
    log("ISM FAULT: code=0x" + [&](){
        std::ostringstream ss; ss << std::hex << report.fault_code; return ss.str();
    }() + " state=" + ismStateStr(report.state));
    // sendStateUpdate (type=state) so GcsStateService picks it up immediately
    gcs_.sendStateUpdate(state_.ism_state, state_.torque_enabled,
                         state_.azimuth_deg, state_.elevation_deg);
}

void FccController::onHeartbeatAck(const IsmHeartbeatAck& ack) {
    uint8_t old_state;
    bool recovering;
    {
        std::lock_guard<std::mutex> lock(state_mutex_);
        old_state  = state_.ism_state;
        recovering = (old_state == ISM_STATE_COMM_LOSS);
        state_.ism_state      = ack.ism_state;
        state_.torque_enabled = (ack.ism_state == ISM_STATE_ARMED ||
                                 ack.ism_state == ISM_STATE_FIRING);
        last_ism_ack_time = std::chrono::steady_clock::now();
    }
    if (recovering)
        log("ISM recovered from COMM_LOSS → " + std::string(ismStateStr(ack.ism_state)));
    if (recovering || ack.ism_state != old_state)
        gcs_.sendStateUpdate(ack.ism_state, state_.torque_enabled,
                             state_.azimuth_deg, state_.elevation_deg);
}

// ── Logging ───────────────────────────────────────────────────────────────────

void FccController::log(const std::string& msg) {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    localtime_r(&t, &tm);
    char ts[16];
    strftime(ts, sizeof(ts), "%H:%M:%S", &tm);
    std::cout << "[FCC " << ts << "] " << msg << "\n";
}
