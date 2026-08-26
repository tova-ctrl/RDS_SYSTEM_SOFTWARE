#pragma once
// =============================================================================
// ICanBus.h — Abstract CAN transport interface
//
// Implemented by:
//   CanBus     — Linux SocketCAN (physical CAN or vcan0)
//   EthCanBus  — UDP tunnel (CAN frames over Ethernet, no CAN hardware needed)
// =============================================================================

#include <string>
#include <cstdint>

struct CanFrame {
    uint32_t id;
    uint8_t  len;
    uint8_t  data[8];
};

class ICanBus {
public:
    virtual ~ICanBus() = default;

    virtual bool send(const CanFrame& frame) = 0;
    virtual bool receive(CanFrame& out_frame) = 0;
    virtual bool receiveTimeout(CanFrame& out_frame, int timeout_ms) = 0;

    virtual bool        isOpen()    const = 0;
    virtual std::string interface() const = 0;
    virtual std::string lastError() const = 0;
};
