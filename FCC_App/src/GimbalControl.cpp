#include "GimbalControl.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

// =============================================================================
// NOTE — MavLink integration
//
// על Jetson האמיתי עם SDK MavLink:
//   #include <mavlink/common/mavlink.h>
//
// ה-message שישמש:
//   GIMBAL_MANAGER_SET_ATTITUDE (msg id 282)
//   fields: gimbal_device_id, flags, q[4] (quaternion), ang_vel_x/y/z
//
// כרגע: stub mode — מדמה תגובה בלי חומרה.
// =============================================================================

static std::string ts() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{}; localtime_r(&t, &tm);
    char buf[16]; strftime(buf, sizeof(buf), "%H:%M:%S", &tm);
    return buf;
}

GimbalControl::GimbalControl(const std::string& camera_ip, uint16_t mavlink_port)
    : camera_ip_(camera_ip), mavlink_port_(mavlink_port) {}

GimbalControl::~GimbalControl() {
    close();
}

bool GimbalControl::open() {
    // TODO (Jetson): פתח UDP socket אמיתי ל-MavLink
    // udp_fd_ = socket(AF_INET, SOCK_DGRAM, 0);

    // כרגע stub mode
    connected_ = true;
    {
        std::lock_guard<std::mutex> lock(status_mutex_);
        status_.connected  = true;
        status_.stabilized = true;
    }
    log("GimbalControl connected to " + camera_ip_ +
        ":" + std::to_string(mavlink_port_) + " (STUB MODE)");
    return true;
}

void GimbalControl::close() {
    if (udp_fd_ >= 0) { ::close(udp_fd_); udp_fd_ = -1; }
    connected_ = false;
}

bool GimbalControl::pointTo(float az_deg, float el_deg) {
    // clamp לגבולות הגימבל של ה-Micro300
    // Elevation: -140° to +90° | Azimuth: ±175°
    if (az_deg >  175.0f) az_deg =  175.0f;
    if (az_deg < -175.0f) az_deg = -175.0f;
    if (el_deg >   90.0f) el_deg =   90.0f;
    if (el_deg < -140.0f) el_deg = -140.0f;

    // TODO (Jetson): sendMavlinkGimbalCmd(az_deg, el_deg);
    stubPointTo(az_deg, el_deg);
    return true;
}

bool GimbalControl::moveRelative(float delta_az, float delta_el) {
    std::lock_guard<std::mutex> lock(status_mutex_);
    return pointTo(status_.az_deg + delta_az,
                   status_.el_deg + delta_el);
}

bool GimbalControl::setZoomVis(float factor) {
    if (factor < 1.0f)  factor = 1.0f;
    if (factor > 20.0f) factor = 20.0f;
    // TODO (Jetson): sendMavlinkZoomCmd(1, factor);  // camera_id=1 = VIS
    std::lock_guard<std::mutex> lock(status_mutex_);
    status_.zoom_vis = factor;
    log("VIS zoom → x" + std::to_string(factor));
    return true;
}

bool GimbalControl::setZoomIR(float factor) {
    if (factor < 1.0f) factor = 1.0f;
    if (factor > 4.0f) factor = 4.0f;
    // TODO (Jetson): sendMavlinkZoomCmd(2, factor);  // camera_id=2 = IR
    std::lock_guard<std::mutex> lock(status_mutex_);
    status_.zoom_ir = factor;
    log("IR zoom → x" + std::to_string(factor));
    return true;
}

bool GimbalControl::stopMovement() {
    std::lock_guard<std::mutex> lock(status_mutex_);
    log("Gimbal STOP at az=" + std::to_string(status_.az_deg) +
        " el=" + std::to_string(status_.el_deg));
    return true;
}

GimbalStatus GimbalControl::getStatus() const {
    std::lock_guard<std::mutex> lock(status_mutex_);
    return status_;
}

// ── Stub ──────────────────────────────────────────────────────────────────────

void GimbalControl::stubPointTo(float az, float el) {
    std::lock_guard<std::mutex> lock(status_mutex_);
    status_.az_deg = az;
    status_.el_deg = el;
    log("Gimbal → az=" + std::to_string(az) +
        "° el=" + std::to_string(el) + "° (STUB)");
}

// ── MavLink senders (TODO on Jetson) ─────────────────────────────────────────

bool GimbalControl::sendMavlinkGimbalCmd(float az, float el) {
    // TODO: בנה GIMBAL_MANAGER_SET_ATTITUDE MavLink packet ושלח ב-UDP
    // mavlink_message_t msg;
    // mavlink_msg_gimbal_manager_set_attitude_pack(...);
    // uint8_t buf[256]; int len = mavlink_msg_to_send_buffer(buf, &msg);
    // sendto(udp_fd_, buf, len, 0, ...);
    (void)az; (void)el;
    return true;
}

bool GimbalControl::sendMavlinkZoomCmd(uint8_t camera_id, float factor) {
    // TODO: בנה CAMERA_ZOOM MavLink packet
    (void)camera_id; (void)factor;
    return true;
}

void GimbalControl::setError(const std::string& msg) { last_error_ = msg; }

void GimbalControl::log(const std::string& msg) {
    std::cout << "[GIMBAL " << ts() << "] " << msg << "\n";
}
