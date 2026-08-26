#include "IsmSimCan.h"

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <FaultCode.h>
#include <ISM.h>

static std::string ts() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{}; localtime_r(&t, &tm);
    char buf[16]; strftime(buf, sizeof(buf), "%H:%M:%S", &tm);
    return buf;
}

IsmSimCan::IsmSimCan(const std::string& can_if, ISM& ism)
    : iface_(can_if), ism_(ism) {}

IsmSimCan::~IsmSimCan() { close(); }

bool IsmSimCan::open() {
    fd_ = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (fd_ < 0) { setError("socket() failed: " + std::string(strerror(errno))); return false; }

    struct ifreq ifr{};
    strncpy(ifr.ifr_name, iface_.c_str(), IFNAMSIZ - 1);
    if (ioctl(fd_, SIOCGIFINDEX, &ifr) < 0) {
        setError("Interface '" + iface_ + "' not found: " + strerror(errno));
        ::close(fd_); fd_ = -1; return false;
    }

    struct sockaddr_can addr{};
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(fd_, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        setError("bind() failed: " + std::string(strerror(errno)));
        ::close(fd_); fd_ = -1; return false;
    }

    // non-blocking
    int flags = fcntl(fd_, F_GETFL, 0);
    fcntl(fd_, F_SETFL, flags | O_NONBLOCK);
    return true;
}

void IsmSimCan::close() {
    if (fd_ >= 0) { ::close(fd_); fd_ = -1; }
}

// ── processIncoming ────────────────────────────────────────────────────────

void IsmSimCan::processIncoming() {
    uint32_t id; uint8_t data[8]; uint8_t len;
    while (readFrame(id, data, len))
        dispatch(id, data, len);
}

void IsmSimCan::dispatch(uint32_t id, const uint8_t* data, uint8_t len) {
    SystemState state_before = ism_.getState();

    switch (id) {

        case SIM_CAN_ID_HEARTBEAT: {
            uint32_t seq = 0;
            if (len >= 4) memcpy(&seq, data, 4);
            hb_count_++;

            FccMessage msg;
            msg.type   = FccCommandType::HEARTBEAT;
            msg.seq    = seq;
            msg.source = "FCC";
            ism_.processMessage(msg);

            // שלח ACK כל 10 heartbeats כדי לא להציף
            if (hb_count_ % 10 == 0) {
                sendHeartbeatAck(seq);
                std::cout << "[ISM-SIM " << ts() << "] ♥ HB seq="
                          << seq << " (×" << hb_count_ << ")"
                          << " state=" << stateToString(ism_.getState()) << "\n";
            }
            
            if (ism_.getLastFaultCode() == FaultCode::WATCHDOG_TIMEOUT || ism_.getLastFaultCode() == FaultCode::COMM_LOSS) // FCC stopped sending heartbeats
            {
                ism_.enterSafe("Power-up initialisation");
                ism_.clearLastFaultCode();
            }
            break;
        }

        case SIM_CAN_ID_STANDBY: {
            uint32_t seq = 0;
            if (len >= 4) memcpy(&seq, data, 4);
            FccMessage msg; msg.type = FccCommandType::STANDBY; msg.seq = seq; msg.source = "FCC";
            bool ok = ism_.processMessage(msg);
            std::cout << "[ISM-SIM " << ts() << "] STANDBY " << (ok ? "ACCEPTED" : "REJECTED")
                      << " → state=" << stateToString(ism_.getState()) << "\n";
            sendStateAck(ism_.getState());
            break;
        }

        case SIM_CAN_ID_ARM: {
            uint32_t seq = 0;
            if (len >= 4) memcpy(&seq, data, 4);
            FccMessage msg; msg.type = FccCommandType::ARM; msg.seq = seq; msg.source = "FCC";
            bool ok = ism_.processMessage(msg);
            std::cout << "[ISM-SIM " << ts() << "] ARM " << (ok ? "ACCEPTED" : "REJECTED")
                      << " → state=" << stateToString(ism_.getState()) << "\n";
            sendStateAck(ism_.getState());
            break;
        }

        case SIM_CAN_ID_DISARM: {
            uint32_t seq = 0;
            if (len >= 4) memcpy(&seq, data, 4);
            FccMessage msg; msg.type = FccCommandType::DISARM; msg.seq = seq; msg.source = "FCC";
            ism_.processMessage(msg);
            std::cout << "[ISM-SIM " << ts() << "] DISARM → state=" << stateToString(ism_.getState()) << "\n";
            sendStateAck(ism_.getState());
            break;
        }

        case SIM_CAN_ID_MOTION: {
            // Layout: [seq(2)] [az_int16*10] [el_int16*10] [rate_int16*10]
            uint16_t seq16 = 0;
            int16_t az_enc = 0, el_enc = 0, rate_enc = 0;
            if (len >= 2) memcpy(&seq16,    data,     2);
            if (len >= 4) memcpy(&az_enc,   data + 2, 2);
            if (len >= 6) memcpy(&el_enc,   data + 4, 2);
            if (len >= 8) memcpy(&rate_enc, data + 6, 2);
            FccMessage msg;
            msg.type = FccCommandType::MOTION_COMMAND;
            msg.seq  = seq16; msg.source = "FCC";
            msg.motion.azimuth_deg   = az_enc   / 10.0f;
            msg.motion.elevation_deg = el_enc   / 10.0f;
            msg.motion.rate_dps      = rate_enc / 10.0f;
            bool ok = ism_.processMessage(msg);
            std::cout << "[ISM-SIM " << ts() << "] MOTION az="
                      << msg.motion.azimuth_deg << " el=" << msg.motion.elevation_deg
                      << " rate=" << msg.motion.rate_dps
                      << (ok ? " OK" : " REJECTED") << "\n";
            // Send FAULT_REPORT only when ISM actually entered FAULT (not a simple rejection)
            if (!ok && ism_.getState() == SystemState::FAULT)
                sendFaultReport(static_cast<uint16_t>(ism_.getLastFaultCode()), ism_.getState());
            break;
        }

        case SIM_CAN_ID_FIRE_REQUEST: {
            uint32_t seq = 0;
            if (len >= 4) memcpy(&seq, data, 4);
            FccMessage msg; msg.type = FccCommandType::FIRE_REQUEST; msg.seq = seq; msg.source = "FCC";
            bool ok = ism_.processMessage(msg);
            std::cout << "[ISM-SIM " << ts() << "] FIRE REQUEST "
                      << (ok ? "AUTHORISED 🔥" : "REJECTED") << "\n";
            sendStateAck(ism_.getState());
            break;
        }

        case SIM_CAN_ID_RECOVERY_RESET: {
            uint32_t seq = 0;
            if (len >= 4) memcpy(&seq, data, 4);
            FccMessage msg; msg.type = FccCommandType::RECOVERY_RESET; msg.seq = seq; msg.source = "OPERATOR";
            bool ok = ism_.processMessage(msg);
            std::cout << "[ISM-SIM " << ts() << "] RECOVERY "
                      << (ok ? "OK → SAFE" : "REJECTED") << "\n";
            sendStateAck(ism_.getState());
            break;
        }

        case SIM_CAN_ID_INJECT_RESET: {
            std::cout << "[ISM-SIM " << ts() << "] INJECT: SYSTEM RESET → entering SAFE\n";
            ism_.enterSafe("SYSTEM RESET (injected)");
            ism_.clearLastFaultCode();
            sendStateAck(ism_.getState());
            break;
        }

        case SIM_CAN_ID_INJECT_INTERNAL: {
            std::cout << "[ISM-SIM " << ts() << "] INJECT: INTERNAL FAULT\n";
            // Use a direct enterFault via a dummy processMessage with INTERNAL_ERROR type.
            // We bypass processMessage (which would do seq checks) and call enterFault directly
            // by simulating an internal hardware check failure.
            uint32_t seq = 0;
            if (len >= 4) memcpy(&seq, data, 4);
            FccMessage msg;
            msg.type   = static_cast<FccCommandType>(0xFE);  // maps to internal-fault path
            msg.seq    = seq;
            msg.source = "ISM-SELF";
            ism_.processMessage(msg);
            sendFaultReport(static_cast<uint16_t>(ism_.getLastFaultCode()), ism_.getState());
            break;
        }

        default: {
            uint32_t seq = 0;
            if (len >= 4) memcpy(&seq, data, 4);
            std::cout << "[ISM-SIM " << ts() << "] Unknown CAN ID: 0x"
                      << std::hex << id << std::dec
                      << " seq=" << std::dec << seq << " — injecting INVALID_COMMAND\n";
            FccMessage msg;
            msg.type   = static_cast<FccCommandType>(0xFF);  // unknown type → enterFault(INVALID_COMMAND)
            msg.seq    = seq;
            msg.source = "FCC";
            bool ok = ism_.processMessage(msg);
            if (!ok) sendFaultReport(static_cast<uint16_t>(ism_.getLastFaultCode()), ism_.getState());
            break;
        }
    }

    // אם המצב השתנה — שלח STATE_ACK
    if (ism_.getState() != state_before)
        sendStateAck(ism_.getState());
}

// ── printStatusIfChanged ───────────────────────────────────────────────────

void IsmSimCan::printStatusIfChanged() {
    SystemState cur = ism_.getState();
    if (cur != last_printed_state_) {
        std::cout << "[ISM-SIM " << ts() << "] *** STATE CHANGE: "
                  << stateToString(last_printed_state_)
                  << " → " << stateToString(cur)
                  << " | torque=" << (ism_.isTorqueEnabled() ? "ON" : "OFF") << " ***\n";
        last_printed_state_ = cur;
    }
}

// ── Frame send helpers ─────────────────────────────────────────────────────

void IsmSimCan::sendHeartbeatAck(uint32_t seq_echo) {
    uint8_t data[5]{};
    memcpy(data, &seq_echo, 4);
    data[4] = static_cast<uint8_t>(ism_.getState());
    sendFrame(SIM_CAN_ID_HEARTBEAT_ACK, data, 5);
}

void IsmSimCan::sendStateAck(SystemState state) {
    uint8_t data[2]{ static_cast<uint8_t>(state), 0x00 };
    sendFrame(SIM_CAN_ID_STATE_ACK, data, 2);
}

void IsmSimCan::sendFaultReport(uint16_t code, SystemState state) {
    uint8_t data[3]{};
    memcpy(data, &code, 2);
    data[2] = static_cast<uint8_t>(state);
    sendFrame(SIM_CAN_ID_FAULT_REPORT, data, 3);
}

// ── Low-level CAN I/O ──────────────────────────────────────────────────────

bool IsmSimCan::readFrame(uint32_t& id, uint8_t* data, uint8_t& len) {
    struct can_frame cf{};
    ssize_t n = read(fd_, &cf, sizeof(cf));
    if (n < 0) return false;
    id  = cf.can_id & CAN_SFF_MASK;
    len = cf.can_dlc;
    memcpy(data, cf.data, len);
    return true;
}

bool IsmSimCan::sendFrame(uint32_t id, const uint8_t* data, uint8_t len) {
    struct can_frame cf{};
    cf.can_id  = id;
    cf.can_dlc = len;
    memcpy(cf.data, data, len);
    return write(fd_, &cf, sizeof(cf)) == sizeof(cf);
}

void IsmSimCan::setError(const std::string& msg) { last_error_ = msg; }
