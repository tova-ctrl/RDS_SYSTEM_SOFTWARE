#include "IsmEmgLink.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iostream>
#include <cerrno>
#include <cstring>

IsmEmgLink::~IsmEmgLink() {
    if (ready_) ::close(static_cast<int>(handle_));
}

bool IsmEmgLink::init(const std::string& webapp_ip, uint16_t port) {
    webapp_ip_ = webapp_ip;
    port_      = port;
    int fd = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) return false;
    handle_ = fd;
    ready_  = true;
    return true;
}

void IsmEmgLink::send(SystemState state, FaultCode code, const std::string& reason) {
    if (!ready_) return;
    int fd = static_cast<int>(handle_);

    std::string json = buildJson(state, code, reason);

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port_);
    ::inet_aton(webapp_ip_.c_str(), &addr.sin_addr);

    ssize_t sent = ::sendto(fd, json.c_str(), json.size(), 0,
                            reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (sent < 0)
        std::cout << "[ISM-EMG] sendto FAILED: " << strerror(errno) << "\n";
    else
        std::cout << "[ISM-EMG] UDP sent → " << webapp_ip_ << ":" << port_
                  << "  " << sent << " bytes  payload=" << json << "\n";
}

std::string IsmEmgLink::buildJson(SystemState state, FaultCode code, const std::string& reason) const {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    localtime_r(&t, &tm);
    char ts[16];
    strftime(ts, sizeof(ts), "%H:%M:%S", &tm);

    std::ostringstream ss;
    ss << "{\"type\":\"EMG\""
       << ",\"ism_state\":\""  << stateToString(state)        << "\""
       << ",\"fault_code\":"   << static_cast<uint16_t>(code)
       << ",\"fault_hex\":\""  << faultCodeToHex(code)        << "\""
       << ",\"fault_name\":\"" << faultCodeToString(code)     << "\""
       << ",\"reason\":\""     << reason                      << "\""
       << ",\"ts\":\""         << ts                          << "\"}";
    return ss.str();
}
