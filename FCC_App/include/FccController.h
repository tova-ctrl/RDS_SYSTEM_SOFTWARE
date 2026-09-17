#pragma once
// =============================================================================
// FccController.h — Main FCC logic
//
// Responsibilities:
//   • Send heartbeat to ISM every 100ms over CAN
//   • Receive and dispatch ISM state ack / fault reports from CAN
//   • Forward operator commands from GCS to ISM over CAN
//   • Push ISM state updates to GCS over UDP
// =============================================================================

#include "ICanBus.h"
#include "GcsLink.h"
#include "IsmProtocol.h"
#include "MotorSimulator.h"
#include "CameraStream.h"
#include "GimbalControl.h"
#include "CameraApi.h"
#include <atomic>
#include <chrono>
#include <thread>
#include <mutex>
#include <memory>
#include <string>
#include <cstdint>
#include <sstream>

struct FccState {
    uint8_t  ism_state      = ISM_STATE_COMM_LOSS;  // unknown until first ACK from ISM
    uint8_t  ism_save_state = ISM_STATE_SAFE;
    bool     torque_enabled = false;
    float    azimuth_deg    = 0.0f;
    float    elevation_deg  = 0.0f;
    float    motor_rate_dps = 0.0f;
    bool     motor_moving   = false;
    uint32_t heartbeat_seq  = 0;
    uint32_t cmd_seq        = 0;
    bool     can_ok         = false;
    bool     gcs_ok         = false;
};

class FccController {
public:
    FccController(ICanBus& can, GcsLink& gcs);
    ~FccController() { stop(); }

    // Start heartbeat thread and CAN receive thread.
    void start();

    // Stop all threads gracefully.
    void stop();

    // Send STANDBY command to ISM — SAFE → STANDBY.
    bool sendStandby();

    // Send ARM command to ISM — STANDBY → ARMED.
    bool sendArm();

    // Send DISARM command to ISM.
    bool sendDisarm();

    // Send motion command to ISM (manual).
    bool sendMotion(float az, float el, float rate);

    // Start motor moving to target — FCC will send motion commands automatically.
    void motorMoveTo(float az, float el, float rate_dps = 10.0f);

    // Emergency stop motor (on ISM SAFE/FAULT).
    void motorStop();

    // ── Camera / Gimbal ───────────────────────────────────────────────────
    // Start Micro300 video stream.
    bool startCamera(const std::string& camera_ip, uint16_t rtsp_port = 554);

    // Stop camera stream.
    void stopCamera();

    // Re-point the "Main" video stream target (from the WebApp NETWORK tab's
    // CONFIG_CAM_STREAM) — the video IP/port aren't sent to the camera via any
    // stkAPI command; they're purely receiver-side (the camera streams to
    // fixed ports, this just tells our GStreamer receiver where to listen).
    void configureVideoStream(const std::string& ip, uint16_t port) { video_ip_ = ip; video_port_ = port; }
    std::string videoStreamIp()   const { return video_ip_; }
    uint16_t    videoStreamPort() const { return video_port_; }

    // Point gimbal to absolute angle.
    bool pointGimbal(float az_deg, float el_deg);

    // Set VIS zoom (x1-x20).
    bool setZoomVis(float factor);

    // Set IR zoom (x1-x4).
    bool setZoomIR(float factor);

    // Get camera status.
    CameraStatus getCameraStatus() const;

    // Get gimbal status.
    GimbalStatus getGimbalStatus() const;

    // Send fire request to ISM.
    bool sendFireRequest();

    // Send recovery reset to ISM.
    bool sendRecoveryReset();

    // Send dual-channel FIRE interlock safety-catch state to ISM (B5/B6 on the
    // operator controller — SWR-SAFE-004). Two independent calls, not combined.
    bool sendSafetyChannelA(bool open);
    bool sendSafetyChannelB(bool open);

    // Dispatch a Micro300 SDK command from the PC (see CameraApi.h). Returns
    // false only if func isn't a recognized stkAPI function name.
    bool dispatchCameraCommand(const std::string& func, const std::string& data_json) {
        return camera_api_.dispatch(func, data_json);
    }

    // Re-point the camera UDP link (from the WebApp NETWORK tab's CONFIG_CAM_CTRL).
    bool reconfigureCameraLink(const std::string& camera_ip, uint16_t camera_port, uint16_t host_port) {
        return camera_api_.reconfigure(camera_ip, camera_port, host_port);
    }

    // Camera telemetry for the WebApp's REFRESH button (GET_CAMERA_STATUS).
    std::string getCameraStatusJson() { return camera_api_.getStatusJson(); }

    // Current FCC-side config for the WebApp's NETWORK/CAMERA tabs on startup
    // (GET_CONFIG) — camera control link + video stream target, whatever they
    // actually are right now (not necessarily the compiled-in defaults, since
    // either may have been changed at runtime via CONFIG_CAM_CTRL/STREAM).
    std::string getConfigJson() {
        std::ostringstream j;
        j << "{\"ok\":true"
          << ",\"controlIp\":\""   << camera_api_.controlIp()   << "\""
          << ",\"controlPort\":"   << camera_api_.controlPort()
          << ",\"hostPort\":"      << camera_api_.hostPort()
          << ",\"streamIp\":\""    << video_ip_ << "\""
          << ",\"streamPort1\":"   << video_port_
          << "}";
        return j.str();
    }

    // ── Fault injection (testing only) ────────────────────────────────────
    bool injectFault(const std::string& type);

    // Thread-safe read of current FCC state.
    FccState getState() const;

    bool isRunning() const { return running_; }

private:
    ICanBus&  can_;
    GcsLink&  gcs_;

    FccState          state_;
    mutable std::mutex state_mutex_;
    std::atomic<bool>  running_{ false };
    std::atomic<bool>  pause_heartbeats_{ false };

    MotorSimulator                motor_;
    std::unique_ptr<CameraStream> camera_;
    std::unique_ptr<GimbalControl> gimbal_;
    CameraApi                     camera_api_;

    // Main video stream receiver target — real Micro300 defaults, not the old
    // 192.168.1.200:554 simulation value. Real-hardware testing (2026-07-27)
    // confirmed the camera is a single device at 1.1.1.3, responding to both
    // control (stkAPI REMOTE_IP) and — presumed, not yet stream-confirmed —
    // video. 1.1.1.2 is NOT the camera; it's the SDK's HOST_IP convention for
    // the local receiving machine's own address (confirmed via the PC's own
    // NIC being statically set to 1.1.1.2 to match the vendor demo app).
    std::string video_ip_   = "1.1.1.3";
    uint16_t    video_port_ = 51000;

    std::atomic<int64_t> last_ism_ack_ms_{ -1 };  // -1 = never received
    static constexpr int ISM_ACK_TIMEOUT_MS = 1000;

    std::thread heartbeat_thread_;
    std::thread uart_receive_thread_;
    std::thread can_receive_thread_;
    std::thread gcs_poll_thread_;
    std::thread motor_cmd_thread_;

    // for ISM timeout == 23.6.26=====
    std::chrono::steady_clock::time_point last_ism_ack_time= std::chrono::steady_clock::now();
    //================================
    // Thread functions
    void heartbeatLoop();      // sends heartbeat every 500ms
    void uartReceiveLoop();    // reads UART continuously, updates state on ACK
    void canReceiveLoop();     // reads ISM frames from CAN
    void gcsPollLoop();      // polls GCS for operator commands
    void motorCmdLoop();     // sends motor position to ISM every 100ms when ARMED

    // CAN frame dispatcher
    void dispatchCanFrame(const CanFrame& frame);
    void onStateAck(const IsmStateAck& ack);
    void onFaultReport(const IsmFaultReport& report);
    void onHeartbeatAck(const IsmHeartbeatAck& ack);
    //USART2
    //bool heartbeatRead();
    std::string read_serial_line(int uart_fd);
    // Logging
    void log(const std::string& msg);
    std::string to_debug_string(std::chrono::steady_clock::time_point tp);
};
