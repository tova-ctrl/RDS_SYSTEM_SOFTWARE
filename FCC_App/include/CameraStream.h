#pragma once
// =============================================================================
// CameraStream.h — Micro300 H.265 video stream receiver
//
// מקבל H.265 stream מה-Micro300 דרך RTSP over Ethernet.
// משתמש ב-GStreamer pipeline על Jetson.
//
// ממשק המצלמה (מתוך מפרט Micro300):
//   Video Output: H.265 over Ethernet
//   Power:        24-32 VDC
//   IP interface: Ethernet
//
// GStreamer pipeline:
//   rtspsrc → rtph265depay → h265parse → nvv4l2decoder (Jetson HW) → appsink
//
// כשה-SDK של Microcon יגיע — מחליפים רק את connectToCamera()
// =============================================================================

#include <string>
#include <functional>
#include <atomic>
#include <thread>
#include <cstdint>
#include <mutex>
#include <atomic>

struct VideoFrame {
    uint32_t width       = 0;
    uint32_t height      = 0;
    uint32_t frame_num   = 0;
    uint64_t timestamp_ms = 0;
    bool     valid       = false;
    // בפרודקשן: pointer לנתוני הפריים (YUV / RGB)
    // כרגע stub — מכיל מטא-דאטה בלבד
};

struct CameraStatus {
    bool     connected    = false;
    bool     streaming    = false;
    uint32_t fps          = 0;
    uint32_t frame_count  = 0;
    float    bitrate_mbps = 0.0f;
    std::string error;
};
extern std::atomic<bool> g_shutdown;
class CameraStream {
public:
    using FrameCallback = std::function<void(const VideoFrame&)>;

    // camera_ip: כתובת IP של ה-Micro300
    // rtsp_port: פורט RTSP (ברירת מחדל 554)
    // on_frame:  callback שנקרא על כל פריים
    CameraStream(const std::string& camera_ip,
                 uint16_t rtsp_port,
                 FrameCallback on_frame = nullptr);
    ~CameraStream();

    // התחבר למצלמה והתחל לקבל stream
    bool start();

    // עצור stream
    void stop();

    bool isStreaming()  const { return streaming_; }
    CameraStatus getStatus() const;

    // פקודות זום (ישלחו דרך GimbalControl / MavLink)
    void setZoomVis(float zoom_factor);   // VIS: x1 - x20
    void setZoomIR(float zoom_factor);    // IR:  x1 - x4

    std::string lastError() const { return last_error_; }

private:
    std::string    camera_ip_;
    uint16_t       rtsp_port_;
    FrameCallback  on_frame_;

    std::atomic<bool>   streaming_{ false };
    std::atomic<bool>   running_{ false };
    std::thread         stream_thread_;
    CameraStatus        status_;
    mutable std::mutex  status_mutex_;
    std::string         last_error_;

    // GStreamer pipeline string
    std::string buildPipeline() const;

    // Stream receive loop
    void streamLoop();

    // Stub: מדמה קבלת frames כשאין חומרה
    void stubLoop();

    void setError(const std::string& msg);
    void log(const std::string& msg, const char* ending = "\n");
};
