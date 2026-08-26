#include "CanBus.h"

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <poll.h>
#include <string>

CanBus::CanBus(const std::string& interface)
    : iface_(interface) {}

CanBus::~CanBus() {
    close();
}

bool CanBus::open() {
    // Create raw CAN socket
    fd_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (fd_ < 0) {
        setError("socket() failed: " + std::string(strerror(errno)));
        return false;
    }

    // Bind to the named CAN interface (e.g. "can0")
    struct ifreq ifr{};
    strncpy(ifr.ifr_name, iface_.c_str(), IFNAMSIZ - 1);
    if (ioctl(fd_, SIOCGIFINDEX, &ifr) < 0) {
        setError("ioctl(SIOCGIFINDEX) failed for '" + iface_ +
                 "': " + strerror(errno) +
                 " — run: sudo ip link set " + iface_ +
                 " type can bitrate 1000000 && sudo ip link set " + iface_ + " up");
        ::close(fd_);
        fd_ = -1;
        return false;
    }

    struct sockaddr_can addr{};
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(fd_, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        setError("bind() failed: " + std::string(strerror(errno)));
        ::close(fd_);
        fd_ = -1;
        return false;
    }

    // Set non-blocking
    int flags = fcntl(fd_, F_GETFL, 0);
    fcntl(fd_, F_SETFL, flags | O_NONBLOCK);

    return true;
}

void CanBus::close() {
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
}

bool CanBus::send(const CanFrame& frame) {
    if (fd_ < 0) return false;  // stub mode — silent

    struct can_frame cf{};
    cf.can_id  = frame.id;
    cf.can_dlc = frame.len;
    memcpy(cf.data, frame.data, frame.len);

    ssize_t n = write(fd_, &cf, sizeof(cf));
    if (n != sizeof(cf)) {
        setError("write() failed: " + std::string(strerror(errno)));
        return false;
    }
    return true;
}

bool CanBus::receive(CanFrame& out) {
    if (fd_ < 0) return false;  // stub mode — silent

    struct can_frame cf{};
    ssize_t n = read(fd_, &cf, sizeof(cf));
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return false;
        setError("read() failed: " + std::string(strerror(errno)));
        return false;
    }
    out.id  = cf.can_id & CAN_SFF_MASK;
    out.len = cf.can_dlc;
    memcpy(out.data, cf.data, cf.can_dlc);
    return true;
}

bool CanBus::receiveTimeout(CanFrame& out, int timeout_ms) {
    if (fd_ < 0) return false;  // stub mode — silent

    struct pollfd pfd{ fd_, POLLIN, 0 };
    int ret = poll(&pfd, 1, timeout_ms);
    if (ret <= 0) return false;
    return receive(out);
}

void CanBus::setError(const std::string& msg) {
    last_error_ = msg;
}
