#include "EthCanBus.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <cerrno>
#include <cstring>

EthCanBus::EthCanBus(const std::string& ism_ip, uint16_t ism_port, uint16_t rx_port)
    : ism_ip_(ism_ip), ism_port_(ism_port), rx_port_(rx_port) {}

EthCanBus::~EthCanBus() { close(); }

bool EthCanBus::open() {
    // TX socket — FCC → ISM
    tx_fd_ = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (tx_fd_ < 0) { setError("tx socket: " + std::string(strerror(errno))); return false; }

    // RX socket — ISM → FCC
    rx_fd_ = ::socket(AF_INET, SOCK_DGRAM, 0); 
    if (rx_fd_ < 0) {
        setError("rx socket: " + std::string(strerror(errno)));
        ::close(tx_fd_); tx_fd_ = -1; return false;
    }
    int opt = 1;
    ::setsockopt(rx_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in rx_addr{};
    rx_addr.sin_family      = AF_INET;
    rx_addr.sin_addr.s_addr = INADDR_ANY;
    rx_addr.sin_port        = htons(rx_port_);
    if (::bind(rx_fd_, reinterpret_cast<sockaddr*>(&rx_addr), sizeof(rx_addr)) < 0) {
        setError("bind port " + std::to_string(rx_port_) + ": " + strerror(errno));
        ::close(tx_fd_); ::close(rx_fd_); tx_fd_ = rx_fd_ = -1; return false;
    }

    // Pre-fill destination address
    dest_.sin_family = AF_INET;
    dest_.sin_port   = htons(ism_port_);
    ::inet_pton(AF_INET, ism_ip_.c_str(), &dest_.sin_addr);

    return true;
}

void EthCanBus::close() {
    if (tx_fd_ >= 0) { ::close(tx_fd_); tx_fd_ = -1; }
    if (rx_fd_ >= 0) { ::close(rx_fd_); rx_fd_ = -1; }
}

bool EthCanBus::send(const CanFrame& frame) {
    uint8_t buf[WIRE_BYTES];
    memcpy(buf,     &frame.id,  4);
    buf[4] = frame.len;
    memcpy(buf + 5, frame.data, 8);

    ssize_t n = ::sendto(tx_fd_, buf, WIRE_BYTES, 0,
                         reinterpret_cast<const sockaddr*>(&dest_), sizeof(dest_));
    if (n != WIRE_BYTES) { setError("sendto: " + std::string(strerror(errno))); return false; }
    return true;
}

bool EthCanBus::receive(CanFrame& out_frame) {
    uint8_t buf[WIRE_BYTES];
    ssize_t n = ::recv(rx_fd_, buf, sizeof(buf), MSG_DONTWAIT);
    if (n != WIRE_BYTES) return false;
    memcpy(&out_frame.id, buf,     4);
    out_frame.len = buf[4];
    memcpy(out_frame.data, buf + 5, 8);
    return true;
}

bool EthCanBus::receiveTimeout(CanFrame& out_frame, int timeout_ms) {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(rx_fd_, &fds);
    timeval tv{ timeout_ms / 1000, (timeout_ms % 1000) * 1000 };
    int r = ::select(rx_fd_ + 1, &fds, nullptr, nullptr, &tv);
    if (r <= 0) return false;
    return receive(out_frame);
}

void EthCanBus::setError(const std::string& msg) { last_error_ = msg; }
