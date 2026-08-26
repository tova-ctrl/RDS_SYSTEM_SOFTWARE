#pragma once
// =============================================================================
// GimbalControl.h — Micro300 gimbal control via MavLink over UDP
//
// ה-Micro300 תומך ב-MavLink (Optional) לשליטה על הגימבל.
// שולח פקודות pan/tilt/zoom למצלמה דרך UDP.
//
// MavLink messages used:
//   GIMBAL_MANAGER_SET_ATTITUDE  — set gimbal angles
//   GIMBAL_DEVICE_SET_ATTITUDE   — direct gimbal control
//   CAMERA_ZOOM_MESSAGE          — zoom control
//
// אם ה-SDK של Microcon מספק API אחר — מחליפים רק את sendCommand()
// =============================================================================

#include <string>
#include <cstdint>
#include <atomic>
#include <mutex>

struct GimbalStatus {
    float az_deg        = 0.0f;   // azimuth נוכחי
    float el_deg        = 0.0f;   // elevation נוכחי
    float zoom_vis      = 1.0f;   // זום VIS (x1-x20)
    float zoom_ir       = 1.0f;   // זום IR  (x1-x4)
    bool  stabilized    = false;
    bool  connected     = false;
};

class GimbalControl {
public:
    // camera_ip:    כתובת IP של ה-Micro300
    // mavlink_port: פורט MavLink UDP (ברירת מחדל 14550)
    GimbalControl(const std::string& camera_ip, uint16_t mavlink_port = 14550);
    ~GimbalControl();

    bool open();
    void close();
    bool isConnected() const { return connected_; }

    // כוון את הגימבל לזווית מוחלטת
    bool pointTo(float az_deg, float el_deg);

    // כוון הגימבל יחסית למיקום נוכחי
    bool moveRelative(float delta_az, float delta_el);

    // שליטת זום
    bool setZoomVis(float factor);   // x1.0 - x20.0
    bool setZoomIR(float factor);    // x1.0 - x4.0

    // עצור תנועה מיידית
    bool stopMovement();

    // קרא status נוכחי
    GimbalStatus getStatus() const;

    std::string lastError() const { return last_error_; }

private:
    std::string  camera_ip_;
    uint16_t     mavlink_port_;
    int          udp_fd_    = -1;
    bool         connected_ = false;
    GimbalStatus status_;
    mutable std::mutex status_mutex_;
    std::string  last_error_;

    // שלח MavLink UDP packet
    bool sendMavlinkGimbalCmd(float az, float el);
    bool sendMavlinkZoomCmd(uint8_t camera_id, float factor);

    // Stub: מדמה תגובה כשאין חומרה
    void stubPointTo(float az, float el);

    void setError(const std::string& msg);
    void log(const std::string& msg);
};
