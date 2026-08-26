#include "GcsLink.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

GcsLink::GcsLink(const std::string& gcs_ip, uint16_t gcs_port, uint16_t listen_port)
    : gcs_ip_(gcs_ip), gcs_port_(gcs_port), listen_port_(listen_port) {}

GcsLink::~GcsLink() {
    close();
}

bool GcsLink::open() {
    // ── Send socket (FCC → GCS) ───────────────────────────────────────────
    send_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (send_fd_ < 0) {
        setError("send socket() failed: " + std::string(strerror(errno)));
        return false;
    }

    // ── Receive socket (GCS → FCC commands) ──────────────────────────────
    recv_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (recv_fd_ < 0) {
        setError("recv socket() failed: " + std::string(strerror(errno)));
        ::close(send_fd_); send_fd_ = -1;
        return false;
    }

    struct sockaddr_in listen_addr{};
    listen_addr.sin_family      = AF_INET;
    listen_addr.sin_addr.s_addr = INADDR_ANY;
    listen_addr.sin_port        = htons(listen_port_);

    if (bind(recv_fd_, reinterpret_cast<struct sockaddr*>(&listen_addr),
             sizeof(listen_addr)) < 0) {
        setError("bind() failed on port " + std::to_string(listen_port_) +
                 ": " + strerror(errno));
        ::close(send_fd_); send_fd_ = -1;
        ::close(recv_fd_); recv_fd_ = -1;
        return false;
    }

    // Set recv socket non-blocking
    int flags = fcntl(recv_fd_, F_GETFL, 0);
    fcntl(recv_fd_, F_SETFL, flags | O_NONBLOCK);

    return true;
}

void GcsLink::close() {
    if (send_fd_ >= 0) { ::close(send_fd_); send_fd_ = -1; }
    if (recv_fd_ >= 0) { ::close(recv_fd_); recv_fd_ = -1; }
}

void GcsLink::sendStateUpdate(uint8_t ism_state, bool torque_enabled,
                               float az, float el) {
    std::string json = buildStateJson(ism_state, torque_enabled, az, el);

    struct sockaddr_in dest{};
    dest.sin_family = AF_INET;
    dest.sin_port   = htons(gcs_port_);
    inet_pton(AF_INET, gcs_ip_.c_str(), &dest.sin_addr);

    sendto(send_fd_, json.c_str(), json.size(), 0,
           reinterpret_cast<struct sockaddr*>(&dest), sizeof(dest));
}

void GcsLink::sendFaultEvent(uint16_t fault_code, uint8_t ism_state,
                              const std::string& message) {
    std::ostringstream ss;
    ss << "{"
       << "\"type\":\"fault\","
       << "\"fault_code\":" << fault_code << ","
       << "\"ism_state\":" << static_cast<int>(ism_state) << ","
       << "\"message\":\"" << message << "\""
       << "}";

    std::string json = ss.str();
    struct sockaddr_in dest{};
    dest.sin_family = AF_INET;
    dest.sin_port   = htons(gcs_port_);
    inet_pton(AF_INET, gcs_ip_.c_str(), &dest.sin_addr);

    sendto(send_fd_, json.c_str(), json.size(), 0,
           reinterpret_cast<struct sockaddr*>(&dest), sizeof(dest));
}

std::string GcsLink::pollCommand() {
    char buf[512]{};
    ssize_t n = recv(recv_fd_, buf, sizeof(buf) - 1, 0);
    if (n <= 0) return "";
    return std::string(buf, n);
}

std::string GcsLink::buildStateJson(uint8_t ism_state, bool torque_enabled,
                                     float az, float el) {
    // Timestamp
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    localtime_r(&t, &tm);
    char ts[32];
    strftime(ts, sizeof(ts), "%H:%M:%S", &tm);

    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "{"
       << "\"type\":\"state\","
       << "\"ism_state\":" << static_cast<int>(ism_state) << ","
       << "\"torque_enabled\":" << (torque_enabled ? "true" : "false") << ","
       << "\"azimuth_deg\":"    << az << ","
       << "\"elevation_deg\":"  << el << ","
       << "\"timestamp\":\"" << ts << "\""
       << "}";
    return ss.str();
}

void GcsLink::setError(const std::string& msg) {
    last_error_ = msg;
}
