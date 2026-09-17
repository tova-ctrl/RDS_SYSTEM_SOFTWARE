#include "CanBus.h"

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <poll.h>
#include <string>

CanBus::CanBus(const std::string& interface, int bitrate)
    : iface_(interface), bitrate_(bitrate) {}

CanBus::~CanBus() {
    close();
}

bool CanBus::bringUp() {
    // Best-effort — down first (ignore failure, e.g. if already down), then
    // set bitrate and bring up. Requires passwordless sudo for `ip link` (the
    // FCC service already assumes this for the hint message below anyway).
    std::string down_cmd = "sudo ip link set " + iface_ + " down >/dev/null 2>&1";
    std::system(down_cmd.c_str());

    std::string cfg_cmd = "sudo ip link set " + iface_ + " type can bitrate " +
                           std::to_string(bitrate_) + " >/dev/null 2>&1";
    if (std::system(cfg_cmd.c_str()) != 0) {
        setError("auto bring-up: 'ip link set " + iface_ + " type can bitrate " +
                  std::to_string(bitrate_) + "' failed");
        return false;
    }

    std::string up_cmd = "sudo ip link set " + iface_ + " up >/dev/null 2>&1";
    if (std::system(up_cmd.c_str()) != 0) {
        setError("auto bring-up: 'ip link set " + iface_ + " up' failed");
        return false;
    }
    return true;
}

bool CanBus::open() {
    // Bring the interface up ourselves first — no manual `ip link` step needed
    // before launching FCC. Best-effort: if this fails (e.g. no sudo, or the
    // interface is already correctly configured by something else), fall
    // through to the normal open attempt anyway rather than giving up here.
    bringUp();

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
                 " type can bitrate " + std::to_string(bitrate_) +
                 " && sudo ip link set " + iface_ + " up");
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
