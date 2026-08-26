#pragma once
// =============================================================================
// IsmSimCan.h — CAN transport layer for ISM simulator on Linux/WSL
//
// מחבר בין ISM state machine לבין SocketCAN (vcan0 / can0)
// מקבל פריימים מה-FCC, ממיר ל-FccMessage, מעביר ל-ISM
// שולח STATE_ACK / HEARTBEAT_ACK / FAULT_REPORT בחזרה ל-FCC
// =============================================================================

#include "ISM.h"
#include <string>
#include <cstdint>
#include <linux/can.h>

// CAN IDs — חייב להיות זהה ל-IsmProtocol.h של ה-FCC
constexpr uint32_t SIM_CAN_ID_HEARTBEAT      = 0x100;
constexpr uint32_t SIM_CAN_ID_STANDBY        = 0x103;
constexpr uint32_t SIM_CAN_ID_ARM            = 0x101;
constexpr uint32_t SIM_CAN_ID_DISARM         = 0x102;
constexpr uint32_t SIM_CAN_ID_MOTION         = 0x110;
constexpr uint32_t SIM_CAN_ID_FIRE_REQUEST   = 0x120;
constexpr uint32_t SIM_CAN_ID_RECOVERY_RESET = 0x130;
constexpr uint32_t SIM_CAN_ID_INJECT_INVALID   = 0x1FF;  // fault injection: INVALID_COMMAND
constexpr uint32_t SIM_CAN_ID_INJECT_RESET     = 0x1FE;  // fault injection: system reset → SAFE
constexpr uint32_t SIM_CAN_ID_INJECT_INTERNAL  = 0x1FD;  // fault injection: ISM internal fault → FAULT

constexpr uint32_t SIM_CAN_ID_STATE_ACK      = 0x200;
constexpr uint32_t SIM_CAN_ID_FAULT_REPORT   = 0x210;
constexpr uint32_t SIM_CAN_ID_HEARTBEAT_ACK  = 0x211;

class IsmSimCan {
public:
    IsmSimCan(const std::string& can_if, ISM& ism);
    ~IsmSimCan();

    bool open();
    void close();

    // קרא כל הפריימים הממתינים מה-CAN ועבד אותם
    void processIncoming();

    // הדפס שינוי state אם קרה
    void printStatusIfChanged();

    std::string lastError() const { return last_error_; }

private:
    std::string iface_;
    ISM&        ism_;
    int         fd_ = -1;
    std::string last_error_;

    SystemState last_printed_state_ = SystemState::SAFE;
    uint32_t    hb_count_ = 0;

    // קבל פריים אחד (non-blocking). מחזיר false אם אין
    bool readFrame(uint32_t& id, uint8_t* data, uint8_t& len);

    // שלח פריים
    bool sendFrame(uint32_t id, const uint8_t* data, uint8_t len);

    // dispatch פריים נכנס ל-ISM
    void dispatch(uint32_t id, const uint8_t* data, uint8_t len);

    // בנה ושלח תגובות ל-FCC
    void sendHeartbeatAck(uint32_t seq_echo);
    void sendStateAck(SystemState state);
    void sendFaultReport(uint16_t code, SystemState state);

    void setError(const std::string& msg);
};
