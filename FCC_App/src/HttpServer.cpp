#include "HttpServer.h"
#include "FccController.h"
#include "IsmProtocol.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <thread>

using namespace std::chrono_literals;
extern std::atomic<bool> g_shutdown;
static std::string ts() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{}; localtime_r(&t, &tm);
    char buf[16]; strftime(buf, sizeof(buf), "%H:%M:%S", &tm);
    return buf;
}

HttpServer::HttpServer(FccController& fcc, uint16_t port)
    : fcc_(fcc), port_(port) {}

HttpServer::~HttpServer() { stop(); }

// ── Start / Stop ──────────────────────────────────────────────────────────────

bool HttpServer::start() {
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) {
        setError("socket() failed: " + std::string(strerror(errno)));
        return false;
    }

    // Allow port reuse
    int opt = 1;
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(port_);

    if (bind(server_fd_, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        setError("bind() failed on port " + std::to_string(port_) +
                 ": " + strerror(errno));
        ::close(server_fd_); server_fd_ = -1;
        return false;
    }

    if (listen(server_fd_, 8) < 0) {
        setError("listen() failed: " + std::string(strerror(errno)));
        ::close(server_fd_); server_fd_ = -1;
        return false;
    }

    running_       = true;
    accept_thread_ = std::thread(&HttpServer::acceptLoop, this);
    log("HTTP API listening on http://0.0.0.0:" + std::to_string(port_));
    return true;
}

void HttpServer::stop() {
    running_ = false;
    if (server_fd_ >= 0) { ::shutdown(server_fd_, SHUT_RDWR);::close(server_fd_); server_fd_ = -1; }
    if (accept_thread_.joinable()) accept_thread_.join();
}

// ── Accept loop ───────────────────────────────────────────────────────────────

void HttpServer::acceptLoop() {
    while (running_ && !g_shutdown) {
        struct sockaddr_in client_addr{};
        socklen_t len = sizeof(client_addr);
        int client_fd = accept(server_fd_,
                               reinterpret_cast<struct sockaddr*>(&client_addr),
                               &len);
        if (client_fd < 0) {
            if (running_) log("accept() error: " + std::string(strerror(errno)));
            continue;
        }

        // Handle each client in a detached thread
        std::thread([this, client_fd]() {
            handleClient(client_fd);
            ::close(client_fd);
        }).detach();
    }
}

// ── Handle client ─────────────────────────────────────────────────────────────

void HttpServer::handleClient(int client_fd) {
    // Set receive timeout
    struct timeval tv{ 2, 0 };
    setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    char buf[4096]{};
    ssize_t n = recv(client_fd, buf, sizeof(buf) - 1, 0);
    if (n <= 0) return;

    std::string raw(buf, n);
    HttpRequest req = parseRequest(raw);
    std::string response = route(req);

    send(client_fd, response.c_str(), response.size(), 0);
}

// ── Parse HTTP request ────────────────────────────────────────────────────────

HttpServer::HttpRequest HttpServer::parseRequest(const std::string& raw) {
    HttpRequest req;
    std::istringstream ss(raw);

    // First line: "POST /api/arm HTTP/1.1"
    std::string method, path, version;
    ss >> method >> path >> version;
    req.method = method;

    // Strip query string from path
    auto q = path.find('?');
    req.path = (q != std::string::npos) ? path.substr(0, q) : path;

    // Find body (after \r\n\r\n)
    auto body_start = raw.find("\r\n\r\n");
    if (body_start != std::string::npos)
        req.body = raw.substr(body_start + 4);

    return req;
}

// ── Router ────────────────────────────────────────────────────────────────────

std::string HttpServer::route(const HttpRequest& req) {
    const auto& p = req.path;

    // ── CORS preflight (for Blazor WebApp) ────────────────────────────────
    if (req.method == "OPTIONS") {
        return "HTTP/1.1 204 No Content\r\n"
               "Access-Control-Allow-Origin: *\r\n"
               "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
               "Access-Control-Allow-Headers: Content-Type\r\n"
               "\r\n";
    }

    // ── GET /api/status ───────────────────────────────────────────────────
    if (req.method == "GET" && p == "/api/status")
        return ok(statusJson());

    // ── POST commands ─────────────────────────────────────────────────────
    if (req.method == "POST") {

        if (p == "/api/standby") {
            bool r = fcc_.sendStandby();
            return ok("{\"result\":\"" + std::string(r ? "ok" : "rejected") + "\"}");
        }
        if (p == "/api/arm") {
            bool r = fcc_.sendArm();
            return ok("{\"result\":\"" + std::string(r ? "ok" : "rejected") + "\"}");
        }
        if (p == "/api/disarm") {
            bool r = fcc_.sendDisarm();
            return ok("{\"result\":\"" + std::string(r ? "ok" : "rejected") + "\"}");
        }
        if (p == "/api/fire") {
            bool r = fcc_.sendFireRequest();
            return ok("{\"result\":\"" + std::string(r ? "ok" : "rejected") + "\"}");
        }
        if (p == "/api/recovery") {
            bool r = fcc_.sendRecoveryReset();
            return ok("{\"result\":\"" + std::string(r ? "ok" : "rejected") + "\"}");
        }
        if (p == "/api/move") {
            float az   = extractFloat(req.body, "az");
            float el   = extractFloat(req.body, "el");
            float rate = extractFloat(req.body, "rate");
            if (rate < 1.0f) rate = 10.0f;
            fcc_.motorMoveTo(az, el, rate);
            return ok("{\"result\":\"ok\",\"az\":" + std::to_string(az) +
                      ",\"el\":" + std::to_string(el) + "}");
        }
        if (p == "/api/stop") {
            fcc_.motorStop();
            return ok("{\"result\":\"ok\"}");
        }
        /*  8.8.26 Tova
        if (p == "/api/cam_start") {
            std::string ip = extractString(req.body, "ip", "192.168.1.200");
            bool r = fcc_.startCamera(ip, 554);
            return ok("{\"result\":\"" + std::string(r ? "ok" : "failed") + "\"}");
        }
        if (p == "/api/cam_stop") {
            fcc_.stopCamera();
            return ok("{\"result\":\"ok\"}");
        }*/
        if (p == "/api/gimbal") {
            float az = extractFloat(req.body, "az");
            float el = extractFloat(req.body, "el");
            fcc_.pointGimbal(az, el);
            return ok("{\"result\":\"ok\"}");
        }
        if (p == "/api/zoom_vis") {
            float f = extractFloat(req.body, "factor", 1.0f);
            fcc_.setZoomVis(f);
            return ok("{\"result\":\"ok\"}");
        }
        if (p == "/api/zoom_ir") {
            float f = extractFloat(req.body, "factor", 1.0f);
            fcc_.setZoomIR(f);
            return ok("{\"result\":\"ok\"}");
        }
    }

    return notFound();
}

// ── Status JSON ───────────────────────────────────────────────────────────────

std::string HttpServer::statusJson() {
    FccState      s   = fcc_.getState();
    CameraStatus  cam = fcc_.getCameraStatus();
    GimbalStatus  gim = fcc_.getGimbalStatus();

    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "{"
       << "\"ism_state\":"      << static_cast<int>(s.ism_state)           << ","
       << "\"ism_state_str\":\"" << ismStateStr(s.ism_state)               << "\","
       << "\"torque_enabled\":" << (s.torque_enabled ? "true" : "false")   << ","
       << "\"azimuth_deg\":"    << s.azimuth_deg                           << ","
       << "\"elevation_deg\":"  << s.elevation_deg                         << ","
       << "\"motor_rate_dps\":" << s.motor_rate_dps                        << ","
       << "\"motor_moving\":"   << (s.motor_moving ? "true" : "false")     << ","
       << "\"hb_seq\":"         << s.heartbeat_seq                         << ","
       << "\"can_ok\":"         << (s.can_ok ? "true" : "false")           << ","
       << "\"camera_on\":"      << (cam.streaming ? "true" : "false")      << ","
       << "\"camera_frames\":"  << cam.frame_count                         << ","
       << "\"gimbal_az\":"      << gim.az_deg                              << ","
       << "\"gimbal_el\":"      << gim.el_deg                              << ","
       << "\"zoom_vis\":"       << gim.zoom_vis                            << ","
       << "\"zoom_ir\":"        << gim.zoom_ir
       << "}";
    return ss.str();
}

// ── HTTP response builders ────────────────────────────────────────────────────

std::string HttpServer::ok(const std::string& body) {
    std::string resp = "HTTP/1.1 200 OK\r\n"
                       "Content-Type: application/json\r\n"
                       "Access-Control-Allow-Origin: *\r\n"
                       "Content-Length: " + std::to_string(body.size()) + "\r\n"
                       "Connection: close\r\n"
                       "\r\n" + body;
    return resp;
}

std::string HttpServer::badRequest(const std::string& msg) {
    std::string body = "{\"error\":\"" + msg + "\"}";
    return "HTTP/1.1 400 Bad Request\r\n"
           "Content-Type: application/json\r\n"
           "Access-Control-Allow-Origin: *\r\n"
           "Content-Length: " + std::to_string(body.size()) + "\r\n"
           "Connection: close\r\n\r\n" + body;
}

std::string HttpServer::notFound() {
    std::string body = "{\"error\":\"not found\"}";
    return "HTTP/1.1 404 Not Found\r\n"
           "Content-Type: application/json\r\n"
           "Access-Control-Allow-Origin: *\r\n"
           "Content-Length: " + std::to_string(body.size()) + "\r\n"
           "Connection: close\r\n\r\n" + body;
}

// ── JSON helpers ──────────────────────────────────────────────────────────────

float HttpServer::extractFloat(const std::string& json,
                                const std::string& key, float def) {
    auto p = json.find("\"" + key + "\":");
    if (p == std::string::npos) return def;
    try { return std::stof(json.substr(p + key.size() + 3)); }
    catch (...) { return def; }
}

std::string HttpServer::extractString(const std::string& json,
                                       const std::string& key,
                                       const std::string& def) {
    auto p = json.find("\"" + key + "\":\"");
    if (p == std::string::npos) return def;
    p += key.size() + 4;
    auto end = json.find("\"", p);
    if (end == std::string::npos) return def;
    return json.substr(p, end - p);
}

void HttpServer::setError(const std::string& msg) { last_error_ = msg; }

void HttpServer::log(const std::string& msg) {
    std::cout << "[HTTP " << ts() << "] " << msg << "\n";
}
