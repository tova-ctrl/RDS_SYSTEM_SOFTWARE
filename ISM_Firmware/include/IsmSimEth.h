#pragma once
// =============================================================================
// IsmSimEth.h — UDP transport for ISM simulator (no CAN hardware needed)
//
// Receives CAN frames from FCC over UDP, dispatches through ISM state machine,
// sends STATE_ACK / HEARTBEAT_ACK / FAULT_REPORT back to FCC.
// Reuses same CAN IDs and dispatch logic as IsmSimCan.
//
// FCC → ISM : FCC sends to this machine's listen_port (default 5300)
// ISM → FCC : ISM sends to FCC's rx_port (default 5301), learned from first packet
//
// Usage (main_sim.cpp):
//   IsmSimEth eth(ism, 5300, 5301);
//   eth.open();
//   while (running) { ism.tick(); eth.processIncoming(); }
// =============================================================================

#include "ISM.h"
#include "IsmSimCan.h"  // reuse SIM_CAN_ID_* constants
#include <string>
#include <cstdint>
#include <netinet/in.h>

class IsmSimEth {
public:
    IsmSimEth(ISM& ism,
              uint16_t listen_port = 5300,  // ISM listens here (FCC → ISM)
              uint16_t fcc_port    = 5301); // FCC listens here (ISM → FCC)
    ~IsmSimEth();

    bool open();
    void close();

    void processIncoming();
    void printStatusIfChanged();

    std::string lastError() const { return last_error_; }

private:
    ISM&        ism_;
    uint16_t    listen_port_;
    uint16_t    fcc_port_;
    int         fd_ = -1;
    std::string last_error_;

    // FCC address — learned from first received packet
    sockaddr_in fcc_addr_{};
    bool        fcc_addr_known_ = false;

    SystemState last_printed_state_ = SystemState::SAFE;
    uint32_t    hb_count_           = 0;

    static constexpr int WIRE_BYTES = 13;  // 4+1+8

    bool readFrame(uint32_t& id, uint8_t* data, uint8_t& len);
    bool sendFrame(uint32_t id, const uint8_t* data, uint8_t len);

    void dispatch(uint32_t id, const uint8_t* data, uint8_t len);

    void sendHeartbeatAck(uint32_t seq_echo);
    void sendStateAck(SystemState state);
    void sendFaultReport(uint16_t code, SystemState state);

    void setError(const std::string& msg);
};
