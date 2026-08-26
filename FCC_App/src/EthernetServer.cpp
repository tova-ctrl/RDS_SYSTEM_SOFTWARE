#include "EthernetServer.h"
#include "Logger.h"

extern CircularLog g_log;

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <thread>

// =============================================================================
// EthernetServer — one thread accepts connections, each client gets its own
// short-lived thread for the duration of the session.
// =============================================================================
extern std::atomic<bool> g_shutdown;
EthernetServer::EthernetServer(FccController& fcc, uint16_t port)
    : fcc_(fcc), port_(port) {}

bool EthernetServer::start() {
    server_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) {
        last_error_ = strerror(errno);
        return false;
    }

    int opt = 1;
    ::setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Accept timeout so the loop can check running_ even with no clients
    struct timeval tv{ 1, 0 };
    ::setsockopt(server_fd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(port_);

    if (::bind(server_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        last_error_ = strerror(errno);
        ::close(server_fd_);
        server_fd_ = -1;
        return false;
    }

    if (::listen(server_fd_, 4) < 0) {
        last_error_ = strerror(errno);
        ::close(server_fd_);
        server_fd_ = -1;
        return false;
    }

    running_       = true;
    accept_thread_ = std::thread(&EthernetServer::acceptLoop, this);
    log("TCP server listening on port " + std::to_string(port_));
    return true;
}

void EthernetServer::stop() {
    running_ = false;
    if (server_fd_ >= 0) { ::shutdown(server_fd_, SHUT_RDWR); ::close(server_fd_); server_fd_ = -1; }

    if (accept_thread_.joinable()) accept_thread_.join();
}

// ── Accept loop ───────────────────────────────────────────────────────────────

void EthernetServer::acceptLoop() {
    while (running_ && !g_shutdown) {
        sockaddr_in client_addr{};
        socklen_t   len = sizeof(client_addr);
        int client_fd = ::accept(server_fd_,
                                  reinterpret_cast<sockaddr*>(&client_addr), &len);
        if (client_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) continue; // timeout — check running_
            if (running_) last_error_ = strerror(errno);
            break;
        }

        char ip[INET_ADDRSTRLEN];
        ::inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
        log("Client connected: " + std::string(ip));

        // Each client handled in its own thread (detached — short-lived sessions)
        std::thread([this, client_fd]() {
            handleClient(client_fd);
            ::close(client_fd);
        }).detach();
    }
}

// ── Client handler — read newline-delimited JSON lines ────────────────────────

void EthernetServer::handleClient(int client_fd) {
    std::string buf;
    char tmp[256];

    while (running_ && !g_shutdown) {
        ssize_t n = ::recv(client_fd, tmp, sizeof(tmp) - 1, 0);
   
	if (n < 0) {
		// If non-blocking socket has no data, skip and continue the loop
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Prevent 100% CPU usage
			continue; 
		}
		// Real socket error
		break; 
	}
	else if (n == 0) {
		// n == 0 means the client intentionally closed the connection cleanly
		break; 
	}

        tmp[n] = '\0';
        buf += tmp;

        size_t pos;
        while ((pos = buf.find('\n')) != std::string::npos) {
            std::string line = buf.substr(0, pos);
            buf.erase(0, pos + 1);
            if (!line.empty())
                dispatch(line, client_fd);
        }
    }
    std::string bool_str = running_ ? "true" : "false";
    log (bool_str);
    log("Client disconnected." );
}

// ── Command dispatcher ────────────────────────────────────────────────────────

void EthernetServer::dispatch(const std::string& json, int client_fd) {
    if (json.find("PING") == std::string::npos)
        log("CMD:" + json);

    // Extract "cmd" value — simple search without full JSON parser
    auto extractStr = [&](const std::string& key) -> std::string {
        std::string needle = "\"" + key + "\":\"";
        auto p = json.find(needle);
        if (p == std::string::npos) return "";
        p += needle.size();
        auto end = json.find('"', p);
        return (end == std::string::npos) ? "" : json.substr(p, end - p);
    };

    std::string cmd = extractStr("cmd");

    // ── PING — keepalive, no response needed ─────────────────────────────────
    if (cmd == "PING") {
        return;
    }

    // ── CAM_ON ───────────────────────────────────────────────────────────────
    if (cmd == "CAM_ON") {
        /* 8.8.26  Tova
        g_log.push("CAM_ON video=" + fcc_.videoStreamIp() + ":" + std::to_string(fcc_.videoStreamPort()));
        // Run startCamera in a detached thread — it may block on network I/O
        std::thread([this]() {
            fcc_.startCamera(fcc_.videoStreamIp(), fcc_.videoStreamPort());
        }).detach();*/
        sendResponse(client_fd, R"({"ok":true})");
        return;
    }

    // ── CAM_OFF ──────────────────────────────────────────────────────────────
    if (cmd == "CAM_OFF") {
        g_log.push("CAM_OFF");
        fcc_.stopCamera();
        sendResponse(client_fd, R"({"ok":true})");
        return;
    }

    // ── GET_POSITION ─────────────────────────────────────────────────────────
    if (cmd == "GET_POSITION") {
        FccState s = fcc_.getState();
        std::ostringstream logEntry;
        logEntry << std::fixed << std::setprecision(2)
                 << "GET_POSITION az=" << s.azimuth_deg
                 << " el=" << s.elevation_deg;
        g_log.push(logEntry.str());
        std::ostringstream resp("");
        resp << std::fixed << std::setprecision(2)
             << "{\"x\":" << s.azimuth_deg
             << ",\"y\":" << s.elevation_deg << "}";
        sendResponse(client_fd, resp.str());
        return;
    }

    // ── SET_POSITION ─────────────────────────────────────────────────────────
    if (cmd == "SET_POSITION") {
        double x = extractDouble(json, "x");
        double y = extractDouble(json, "y");
        fcc_.motorMoveTo(static_cast<float>(x), static_cast<float>(y));
        bool ok = fcc_.pointGimbal(static_cast<float>(x), static_cast<float>(y));
        std::ostringstream logEntry;
        logEntry << std::fixed << std::setprecision(2)
                 << "SET_POSITION x=" << x << " y=" << y
                 << (ok ? " ok" : " fail");
        g_log.push(logEntry.str());
        sendResponse(client_fd, ok ? R"({"ok":true})" : R"({"ok":false})");
        return;
    }

    // ── GET_LOGS ─────────────────────────────────────────────────────────────
    if (cmd == "GET_LOGS") {
        std::string lastStatus = g_log.lastStatus();
        auto entries = g_log.snapshot();
        std::ostringstream resp;
        resp << "{\"ok\":true,\"logs\":[";
        bool first = true;
        if (!lastStatus.empty()) {
            resp << "\"" << lastStatus << "\"";
            first = false;
        }
        for (const auto& e : entries) {
            if (!first) resp << ",";
            resp << "\"" << e << "\"";
            first = false;
        }
        resp << "]}";
        sendResponse(client_fd, resp.str());
        return;
    }

    // ── ISM STATE COMMANDS ────────────────────────────────────────────────────
    if (cmd == "standby") {
        bool ok = fcc_.sendStandby();
        sendResponse(client_fd, ok ? R"({"ok":true})" : R"({"ok":false,"error":"standby failed"})");
        return;
    }
    if (cmd == "arm") {
        bool ok = fcc_.sendArm();
        sendResponse(client_fd, ok ? R"({"ok":true})" : R"({"ok":false,"error":"arm failed"})");
        return;
    }
    if (cmd == "disarm") {
        bool ok = fcc_.sendDisarm();
        sendResponse(client_fd, ok ? R"({"ok":true})" : R"({"ok":false,"error":"disarm failed"})");
        return;
    }

    // ── FIRE — 2026-08-20: was never wired here at all. sendFireRequest()
    // was already fully implemented (UART + CAN) and even already used by
    // gcsPollLoop's separate UDP command channel, but this TCP dispatch —
    // the channel RDS_WebApp's EthernetService actually talks to — had no
    // "fire" case, so the WebApp's FIRE button/joystick trigger never sent
    // anything to the ISM at all. Same pattern as standby/arm/disarm above.
    if (cmd == "fire") {
        bool ok = fcc_.sendFireRequest();
        sendResponse(client_fd, ok ? R"({"ok":true})" : R"({"ok":false,"error":"fire failed"})");
        return;
    }

    // ── SAFE — go to SAFE from any state (uses recovery path) ───────────────
    if (cmd == "safe") {
        bool ok = fcc_.sendRecoveryReset();
        sendResponse(client_fd, ok ? R"({"ok":true})" : R"({"ok":false,"error":"safe command failed"})");
        return;
    }

    // ── RECOVERY_RESET ───────────────────────────────────────────────────────
    if (cmd == "recovery") {
        bool ok = fcc_.sendRecoveryReset();
        sendResponse(client_fd, ok ? R"({"ok":true})" : R"({"ok":false,"error":"recovery failed"})");
        return;
    }

    // ── INJECT_FAULT ─────────────────────────────────────────────────────────
    if (cmd == "inject_fault") {
        std::string type = extractStr("type");
        bool ok = fcc_.injectFault(type);
        sendResponse(client_fd, ok ? R"({"ok":true})" : R"({"ok":false,"error":"inject failed"})");
        return;
    }

    // ── CONFIG_CAM_CTRL — re-point the camera UDP link ───────────────────────
    if (cmd == "CONFIG_CAM_CTRL") {
        std::string ip   = extractStr("ip");
        double      port = extractDouble(json, "port", 1024);
        double      host = extractDouble(json, "host_port", 1025);
        bool ok = !ip.empty() &&
                  fcc_.reconfigureCameraLink(ip, static_cast<uint16_t>(port), static_cast<uint16_t>(host));
        g_log.push("CONFIG_CAM_CTRL ip=" + ip + " port=" + std::to_string((int)port) +
                    " host_port=" + std::to_string((int)host) + (ok ? " ok" : " failed"));
        sendResponse(client_fd, ok ? R"({"ok":true})" : R"({"ok":false,"error":"camera reconfigure failed"})");
        return;
    }

    // ── CONFIG_CAM_STREAM — re-point the "Main" video receiver target ────────
    // Not an stkAPI command — the camera streams to fixed ports regardless;
    // this only tells our own GStreamer receiver where to listen. 2nd/3rd
    // ports are accepted but not stored yet — nothing in the UI displays them.
    if (cmd == "CONFIG_CAM_STREAM") {
        std::string ip = extractStr("ip");
        double      p1 = extractDouble(json, "p1", 51000);
        bool ok = !ip.empty();
        if (ok) fcc_.configureVideoStream(ip, static_cast<uint16_t>(p1));
        g_log.push("CONFIG_CAM_STREAM ip=" + ip + " p1=" + std::to_string((int)p1) + (ok ? " ok" : " failed"));
        sendResponse(client_fd, ok ? R"({"ok":true})" : R"({"ok":false,"error":"invalid ip"})");
        return;
    }

    // ── GET_CONFIG — current camera control link + video stream target ───────
    // For the WebApp's "load config from FCC on start" — reflects whatever is
    // actually running now, not necessarily compiled-in defaults.
    if (cmd == "GET_CONFIG") {
        sendResponse(client_fd, fcc_.getConfigJson());
        return;
    }

    // ── GET_CAMERA_STATUS — manual-refresh telemetry pull (STKSyncLocal) ──────
    if (cmd == "GET_CAMERA_STATUS") {
        std::string resp = fcc_.getCameraStatusJson();
        log("GET_CAMERA_STATUS -> " + resp);
        sendResponse(client_fd, resp);
        return;
    }

    // ── Micro300 camera SDK passthrough ──────────────────────────────────────
    // cmd matches an stkAPI.h function name exactly (see CameraApiService.cs
    // on the PC side); "data" holds its arguments as plain JSON numbers.
    if (fcc_.dispatchCameraCommand(cmd, json)) {
        sendResponse(client_fd, R"({"ok":true})");
        return;
    }

    // ── Unknown ───────────────────────────────────────────────────────────────
    log("Unknown command: " + cmd);
    sendResponse(client_fd, R"({"ok":false,"error":"unknown command"})");
}

// ── Helpers ───────────────────────────────────────────────────────────────────

double EthernetServer::extractDouble(const std::string& json,
                                     const std::string& key, double def) {
    std::string needle = "\"" + key + "\":";
    auto p = json.find(needle);
    if (p == std::string::npos) return def;
    try { return std::stod(json.substr(p + needle.size())); }
    catch (...) { return def; }
}

void EthernetServer::sendResponse(int fd, const std::string& json) {
    std::string msg = json + "\n";
    ::send(fd, msg.c_str(), msg.size(), MSG_NOSIGNAL);
}
/*void EthernetServer::stop()
{
    g_shutdown = true; // מעדכן את הדגל

    if (m_server_fd != -1) {
        // shutdown פוסל את ה-Socket מלקבל או לשלוח מידע ומעיר פונקציות חסומות
        ::shutdown(m_server_fd, SHUT_RDWR);
        ::close(m_server_fd); // סגירת ה-File Descriptor בלינוקס/WSL
        m_server_fd = -1;
    }
}*/


void EthernetServer::log(const std::string& msg) {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{}; ::localtime_r(&t, &tm);
    char ts[16]; ::strftime(ts, sizeof(ts), "%H:%M:%S", &tm);
   // std::cout << "[ETH " << ts << "] " << msg << "\n";
    // 1. \33[2K מוחק את שורת הסטטוס הישנה, \r מחזיר להתחלה
    // 2. מדפיסים את הלוג של ה-Ethernet ויורדים שורה כרגיל
    std::cout << "\33[2K\r[ETH " << ts << "] " << msg << "\n";
}
