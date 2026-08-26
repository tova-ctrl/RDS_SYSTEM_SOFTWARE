#pragma once
// =============================================================================
// EthCanBus.h — CAN-over-UDP transport for FCC ↔ ISM without physical CAN
//
// Tunnels CanFrame structs over two UDP sockets:
//   FCC → ISM : sendto ism_ip:ism_port   (default 5300)
//   ISM → FCC : recvfrom 0.0.0.0:rx_port (default 5301)
//
// Frame on wire: [id:4LE][len:1][data:8] = 13 bytes per datagram
//
// Usage (FCC main.cpp):
//   EthCanBus can("192.168.10.1");   // ISM is on laptop at 192.168.10.1
//   can.open();
//   FccController fcc(can, gcs);
// =============================================================================

#include "ICanBus.h"
#include <netinet/in.h>

class EthCanBus : public ICanBus {
public:
    EthCanBus(const std::string& ism_ip,
              uint16_t ism_port = 5300,   // ISM listens here  (FCC → ISM)
              uint16_t rx_port  = 5301);  // FCC listens here  (ISM → FCC)
    ~EthCanBus() override;

    bool open();
    void close();

    bool send(const CanFrame& frame) override;
    bool receive(CanFrame& out_frame) override;
    bool receiveTimeout(CanFrame& out_frame, int timeout_ms) override;

    bool        isOpen()    const override { return tx_fd_ >= 0 && rx_fd_ >= 0; }
    std::string interface() const override { return "eth:" + ism_ip_; }
    std::string lastError() const override { return last_error_; }

private:
    std::string ism_ip_;
    uint16_t    ism_port_;
    uint16_t    rx_port_;
    int         tx_fd_ = -1;
    int         rx_fd_ = -1;
    sockaddr_in dest_{};
    std::string last_error_;

    static constexpr int WIRE_BYTES = 13;  // 4+1+8

    void setError(const std::string& msg);
};
