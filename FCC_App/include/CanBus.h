#pragma once
// =============================================================================
// CanBus.h — SocketCAN wrapper for Jetson (Linux)
//
// Uses the standard Linux SocketCAN API (PF_CAN / SOCK_RAW).
// Works on any Jetson carrier board that exposes a CAN controller as can0/can1
// (Auvidea J20, Connect Tech Quasar, Seeed A203, etc.)
//
// To bring up the interface before running FCC:
//   sudo ip link set can0 type can bitrate 1000000
//   sudo ip link set can0 up
// =============================================================================

#include "ICanBus.h"
#include <string>

class CanBus : public ICanBus {
public:
    explicit CanBus(const std::string& iface = "can0");
    ~CanBus() override;

    bool open();
    void close();

    bool send(const CanFrame& frame) override;
    bool receive(CanFrame& out_frame) override;
    bool receiveTimeout(CanFrame& out_frame, int timeout_ms) override;

    bool        isOpen()    const override { return fd_ >= 0; }
    std::string interface() const override { return iface_; }
    std::string lastError() const override { return last_error_; }

private:
    std::string iface_;
    int         fd_ = -1;
    std::string last_error_;

    void setError(const std::string& msg);
};
