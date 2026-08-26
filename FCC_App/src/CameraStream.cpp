#include "CameraStream.h"

#include <iostream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <thread>

using namespace std::chrono_literals;

extern std::mutex g_console_mutex;

// =============================================================================
// Platform selection:
//   WSL / PC:  make                              → STUB MODE (simulated frames)
//   Jetson:    make CXXFLAGS="-DJETSON_PLATFORM" → GStreamer + nvv4l2decoder
// =============================================================================

#ifdef JETSON_PLATFORM
  #include <gst/gst.h>
  #include <gst/app/gstappsink.h>
  // Jetson HW H.265 decoder pipeline:
  //   rtspsrc → rtph265depay → h265parse → nvv4l2decoder → appsink
#endif

static std::string ts() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{}; localtime_r(&t, &tm);
    char buf[16]; strftime(buf, sizeof(buf), "%H:%M:%S", &tm);
    return buf;
}

CameraStream::CameraStream(const std::string& camera_ip,
                           uint16_t rtsp_port,
                           FrameCallback on_frame)
    : camera_ip_(camera_ip)
    , rtsp_port_(rtsp_port)
    , on_frame_(on_frame)
{}

CameraStream::~CameraStream() {
    stop();
}

bool CameraStream::start() {
    log("TRY Connect to Micro300 at rtsp://" + camera_ip_ +
        ":" + std::to_string(rtsp_port_) + "/stream");
    if (streaming_) return true;

    log("Connecting to Micro300 at rtsp://" + camera_ip_ +
        ":" + std::to_string(rtsp_port_) + "/stream");

    running_   = true;
    streaming_ = true;

    {
        std::lock_guard<std::mutex> lock(status_mutex_);
        status_.connected = true;
        status_.streaming = true;
        status_.fps       = 30;
    }

#ifdef JETSON_PLATFORM
    // ── Jetson: GStreamer real stream ─────────────────────────────────────
    log("Stream started (JETSON — GStreamer pipeline: " + buildPipeline() + ")");
    stream_thread_ = std::thread(&CameraStream::streamLoop, this);
#else
    // ── WSL / PC: stub mode ───────────────────────────────────────────────
    log("Stream started (STUB MODE — build with -DJETSON_PLATFORM on Jetson)");
    stream_thread_ = std::thread(&CameraStream::stubLoop, this);
#endif

    return true;
}

void CameraStream::stop() {
    running_   = false;
    streaming_ = false;
    if (stream_thread_.joinable())
        stream_thread_.join();

    std::lock_guard<std::mutex> lock(status_mutex_);
    status_.connected = false;
    status_.streaming = false;
    log("Stream stopped.");
}

CameraStatus CameraStream::getStatus() const {
    std::lock_guard<std::mutex> lock(status_mutex_);
    return status_;
}

// ── Stub loop — מדמה 30fps stream ────────────────────────────────────────────

void CameraStream::stubLoop() {
    uint32_t frame_num = 0;
    auto next = std::chrono::steady_clock::now();
    bool firstPrint = true;
    CameraStatus  prev_CamStatus{};

    while (running_ && !g_shutdown) {
        VideoFrame frame;
        frame.width        = 1920;
        frame.height       = 1080;
        frame.frame_num    = frame_num++;
        frame.timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                 std::chrono::system_clock::now().time_since_epoch()).count();
        frame.valid        = true;
        
        if (on_frame_) on_frame_(frame);

        {
            std::lock_guard<std::mutex> lock(status_mutex_);
            status_.frame_count  = frame_num;
            status_.bitrate_mbps = 8.5f;   // H.265 typical ~8.5 Mbps @ 1080p30
        }

        // לוג כל 150 frames (כל 5 שניות)
        bool changed = firstPrint
            || status_.connected != prev_CamStatus.connected
            || status_.streaming != prev_CamStatus.streaming
            || status_.fps != prev_CamStatus.fps
            || status_.bitrate_mbps != prev_CamStatus.bitrate_mbps
            || status_.error != prev_CamStatus.error;
        const char* ending = changed ? "\n" : "   \r";
      
        if (frame_num % 150 == 0)
            log("Streaming: frame=" + std::to_string(frame_num) +
                " fps=30 bitrate=8.5Mbps (STUB)", ending);

        prev_CamStatus = status_;
        // שמור על 30fps
        next += std::chrono::milliseconds(33);
        std::this_thread::sleep_until(next);
        firstPrint = false;
    }
}

// ── GStreamer stream loop (Jetson only) ───────────────────────────────────────

void CameraStream::streamLoop() {
#ifdef JETSON_PLATFORM
    gst_init(nullptr, nullptr);

    GError* err = nullptr;
    GstElement* pipeline = gst_parse_launch(buildPipeline().c_str(), &err);
    if (!pipeline || err) {
        setError("GStreamer pipeline failed: " + std::string(err ? err->message : "unknown"));
        return;
    }

    GstElement* sink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    uint32_t frame_num = 0;
    while (running_ && !g_shutdown) {
        GstSample* sample = gst_app_sink_try_pull_sample(GST_APP_SINK(sink), GST_SECOND);
        if (!sample) continue;

        VideoFrame frame;
        frame.frame_num    = frame_num++;
        frame.width        = 1920;
        frame.height       = 1080;
        frame.valid        = true;
        frame.timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                 std::chrono::system_clock::now().time_since_epoch()).count();

        if (on_frame_) on_frame_(frame);

        {
            std::lock_guard<std::mutex> lock(status_mutex_);
            status_.frame_count = frame_num;
        }

        gst_sample_unref(sample);
    }

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
#else
    // לא אמור להגיע לכאן על WSL/PC
    log("streamLoop() called without JETSON_PLATFORM — falling back to stub");
    stubLoop();
#endif
}

// ── GStreamer pipeline string ─────────────────────────────────────────────────

std::string CameraStream::buildPipeline() const {
    // TODO: uncomment על Jetson עם GStreamer + nvv4l2decoder
    std::ostringstream ss;
    ss << "rtspsrc location=rtsp://" << camera_ip_ << ":" << rtsp_port_ << "/stream"
       << " latency=100 ! "
       << "rtph265depay ! h265parse ! "
       << "nvv4l2decoder ! "                  // Jetson HW decoder
       << "video/x-raw,format=NV12 ! "
       << "appsink name=sink sync=false";
    return ss.str();
}

void CameraStream::setZoomVis(float zoom_factor) {
    log("VIS zoom → x" + std::to_string(zoom_factor) + " (via GimbalControl)");
}

void CameraStream::setZoomIR(float zoom_factor) {
    log("IR zoom → x" + std::to_string(zoom_factor) + " (via GimbalControl)");
}

void CameraStream::setError(const std::string& msg) {
    last_error_ = msg;
    std::lock_guard<std::mutex> lock(status_mutex_);
    status_.error = msg;
}

void CameraStream::log(const std::string& msg, const char* ending) {
    std::lock_guard<std::mutex> lock(g_console_mutex);
    std::cout << "[CAMERA " << ts() << "] " << msg << ending << std::flush;

}
