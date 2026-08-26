#pragma once
// =============================================================================
// IsmEmgLink.h — ISM → WebApp emergency notification (UDP, send-only)
//
// One-way channel: the ISM can only SEND. There is no bind(), no recvfrom(),
// no accept(). The WebApp listens on DEFAULT_EMG_PORT for incoming datagrams.
//
// Sent on every FAULT entry and on recovery back to SAFE.
//
// Platform note:
//   Linux/WSL : IsmEmgLink.cpp  (POSIX sendto)
//   STM32H7   : IsmEmgLink_lwip.cpp  (lwIP udp_sendto — same class, different .cpp)
//   The header itself has NO POSIX or HAL includes — stays portable.
// =============================================================================

#include "SystemState.h"
#include "FaultCode.h"
#include <string>
#include <cstdint>

class IsmEmgLink {
public:
    static constexpr uint16_t DEFAULT_EMG_PORT = 5202;

    IsmEmgLink() = default;
    ~IsmEmgLink();

    // Open the send socket. Call once after network is up.
    // Returns false on failure; send() becomes a no-op until init() succeeds.
    bool init(const std::string& webapp_ip, uint16_t port = DEFAULT_EMG_PORT);

    // Send an EMG datagram to the WebApp. Non-blocking, fire-and-forget.
    // No-op if init() was never called or failed.
    void send(SystemState state, FaultCode code, const std::string& reason);

    bool isReady() const { return ready_; }

private:
    intptr_t    handle_   = -1;   // POSIX: socket fd;  LwIP: (intptr_t)udp_pcb*
    bool        ready_    = false;
    std::string webapp_ip_;
    uint16_t    port_     = DEFAULT_EMG_PORT;

    std::string buildJson(SystemState state, FaultCode code, const std::string& reason) const;
};
