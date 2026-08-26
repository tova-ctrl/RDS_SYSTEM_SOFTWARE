#include "IsmSimEth.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <FaultCode.h>
#include <ISM.h>

static std::string ts_eth() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{}; localtime_r(&t, &tm);
    char buf[16]; strftime(buf, sizeof(buf), "%H:%M:%S", &tm);
    return buf;
}

IsmSimEth::IsmSimEth(ISM& ism, uint16_t listen_port, uint16_t fcc_port)
    : ism_(ism), listen_port_(listen_port), fcc_port_(fcc_port) {}

IsmSimEth::~IsmSimEth() { close(); }

bool IsmSimEth::open() {
    fd_ = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (fd_ < 0) { setError("socket: " + std::string(strerror(errno))); return false; }

    int opt = 1;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(listen_port_);
    if (::bind(fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        setError("bind port " + std::to_string(listen_port_) + ": " + strerror(errno));
        ::close(fd_); fd_ = -1; return false;
    }

    // Non-blocking
    int flags = fcntl(fd_, F_GETFL, 0);
    fcntl(fd_, F_SETFL, flags | O_NONBLOCK);
    return true;
}

void IsmSimEth::close() {
    if (fd_ >= 0) { ::close(fd_); fd_ = -1; }
}

// ── Transport ──────────────────────────────────────────────────────────────────

bool IsmSimEth::readFrame(uint32_t& id, uint8_t* data, uint8_t& len) {
    uint8_t buf[WIRE_BYTES];
    sockaddr_in sender{};
    socklen_t   slen = sizeof(sender);
    ssize_t n = ::recvfrom(fd_, buf, sizeof(buf), 0,
                           reinterpret_cast<sockaddr*>(&sender), &slen);
    if (n != WIRE_BYTES) return false;

    // Learn FCC address from the first packet received
    if (!fcc_addr_known_) {
        fcc_addr_ = sender;
        fcc_addr_.sin_port = htons(fcc_port_);  // FCC listens on rx_port_, not its send port
        fcc_addr_known_ = true;
        char ip[INET_ADDRSTRLEN];
        ::inet_ntop(AF_INET, &sender.sin_addr, ip, sizeof(ip));
        std::cout << "[ISM-ETH] FCC address learned: " << ip << ":" << fcc_port_ << "\n";
    }

    memcpy(&id, buf,     4);
    len = buf[4];
    memcpy(data, buf + 5, 8);
    return true;
}

bool IsmSimEth::sendFrame(uint32_t id, const uint8_t* data, uint8_t len) {
    if (!fcc_addr_known_) return false;
    uint8_t buf[WIRE_BYTES];
    memcpy(buf,     &id,  4);
    buf[4] = len;
    memcpy(buf + 5, data, 8);
    ssize_t n = ::sendto(fd_, buf, WIRE_BYTES, 0,
                         reinterpret_cast<const sockaddr*>(&fcc_addr_), sizeof(fcc_addr_));
    return n == WIRE_BYTES;
}

// ── processIncoming ────────────────────────────────────────────────────────────

void IsmSimEth::processIncoming() {
    uint32_t id; uint8_t data[8]; uint8_t len;
    while (readFrame(id, data, len))
        dispatch(id, data, len);
}

// ── dispatch — identical logic to IsmSimCan::dispatch ─────────────────────────

void IsmSimEth::dispatch(uint32_t id, const uint8_t* data, uint8_t len) {
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

            if (hb_count_ % 10 == 0) {
                sendHeartbeatAck(seq);
                std::cout << "[ISM-ETH " << ts_eth() << "] ♥ HB seq="
                          << seq << " (×" << hb_count_ << ")"
                          << " state=" << stateToString(ism_.getState()) << "\n";
            }
            if (ism_.getLastFaultCode() == FaultCode::WATCHDOG_TIMEOUT ||
                ism_.getLastFaultCode() == FaultCode::COMM_LOSS) {
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
            std::cout << "[ISM-ETH " << ts_eth() << "] STANDBY " << (ok ? "ACCEPTED" : "REJECTED")
                      << " → state=" << stateToString(ism_.getState()) << "\n";
            sendStateAck(ism_.getState());
            break;
        }

        case SIM_CAN_ID_ARM: {
            uint32_t seq = 0;
            if (len >= 4) memcpy(&seq, data, 4);
            FccMessage msg; msg.type = FccCommandType::ARM; msg.seq = seq; msg.source = "FCC";
            bool ok = ism_.processMessage(msg);
            std::cout << "[ISM-ETH " << ts_eth() << "] ARM " << (ok ? "ACCEPTED" : "REJECTED")
                      << " → state=" << stateToString(ism_.getState()) << "\n";
            sendStateAck(ism_.getState());
            break;
        }

        case SIM_CAN_ID_DISARM: {
            uint32_t seq = 0;
            if (len >= 4) memcpy(&seq, data, 4);
            FccMessage msg; msg.type = FccCommandType::DISARM; msg.seq = seq; msg.source = "FCC";
            ism_.processMessage(msg);
            std::cout << "[ISM-ETH " << ts_eth() << "] DISARM → state=" << stateToString(ism_.getState()) << "\n";
            sendStateAck(ism_.getState());
            break;
        }

        case SIM_CAN_ID_MOTION: {
            // Layout: [seq(2)][az*10(2)][el*10(2)][rate*10(2)]
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
            std::cout << "[ISM-ETH " << ts_eth() << "] MOTION az="
                      << msg.motion.azimuth_deg << " el=" << msg.motion.elevation_deg
                      << " rate=" << msg.motion.rate_dps
                      << (ok ? " OK" : " REJECTED") << "\n";
            if (!ok && ism_.getState() == SystemState::FAULT)
                sendFaultReport(static_cast<uint16_t>(ism_.getLastFaultCode()), ism_.getState());
            break;
        }

        case SIM_CAN_ID_FIRE_REQUEST: {
            uint32_t seq = 0;
            if (len >= 4) memcpy(&seq, data, 4);
            FccMessage msg; msg.type = FccCommandType::FIRE_REQUEST; msg.seq = seq; msg.source = "FCC";
            bool ok = ism_.processMessage(msg);
            std::cout << "[ISM-ETH " << ts_eth() << "] FIRE REQUEST "
                      << (ok ? "AUTHORISED" : "REJECTED") << "\n";
            sendStateAck(ism_.getState());
            break;
        }

        case SIM_CAN_ID_RECOVERY_RESET: {
            uint32_t seq = 0;
            if (len >= 4) memcpy(&seq, data, 4);
            FccMessage msg; msg.type = FccCommandType::RECOVERY_RESET; msg.seq = seq; msg.source = "OPERATOR";
            bool ok = ism_.processMessage(msg);
            std::cout << "[ISM-ETH " << ts_eth() << "] RECOVERY "
                      << (ok ? "OK → SAFE" : "REJECTED") << "\n";
            sendStateAck(ism_.getState());
            break;
        }

        case SIM_CAN_ID_INJECT_RESET: {
            std::cout << "[ISM-ETH " << ts_eth() << "] INJECT: SYSTEM RESET → entering SAFE\n";
            ism_.enterSafe("SYSTEM RESET (injected)");
            ism_.clearLastFaultCode();
            sendStateAck(ism_.getState());
            break;
        }

        case SIM_CAN_ID_INJECT_INTERNAL: {
            std::cout << "[ISM-ETH " << ts_eth() << "] INJECT: INTERNAL FAULT\n";
            uint32_t seq = 0;
            if (len >= 4) memcpy(&seq, data, 4);
            FccMessage msg;
            msg.type   = static_cast<FccCommandType>(0xFE);
            msg.seq    = seq;
            msg.source = "ISM-SELF";
            ism_.processMessage(msg);
            sendFaultReport(static_cast<uint16_t>(ism_.getLastFaultCode()), ism_.getState());
            break;
        }

        default: {
            uint32_t seq = 0;
            if (len >= 4) memcpy(&seq, data, 4);
            std::cout << "[ISM-ETH " << ts_eth() << "] Unknown CAN ID: 0x"
                      << std::hex << id << std::dec << " — injecting INVALID_COMMAND\n";
            FccMessage msg;
            msg.type   = static_cast<FccCommandType>(0xFF);
            msg.seq    = seq;
            msg.source = "FCC";
            bool ok = ism_.processMessage(msg);
            if (!ok) sendFaultReport(static_cast<uint16_t>(ism_.getLastFaultCode()), ism_.getState());
            break;
        }
    }

    if (ism_.getState() != state_before)
        sendStateAck(ism_.getState());
}

// ── Response helpers ───────────────────────────────────────────────────────────

void IsmSimEth::sendHeartbeatAck(uint32_t seq_echo) {
    uint8_t data[5]{};
    memcpy(data, &seq_echo, 4);
    data[4] = static_cast<uint8_t>(ism_.getState());
    sendFrame(SIM_CAN_ID_HEARTBEAT_ACK, data, 5);
}

void IsmSimEth::sendStateAck(SystemState state) {
    uint8_t data[2]{ static_cast<uint8_t>(state), 0x00 };
    sendFrame(SIM_CAN_ID_STATE_ACK, data, 2);
}

void IsmSimEth::sendFaultReport(uint16_t code, SystemState state) {
    uint8_t data[3]{};
    memcpy(data, &code, 2);
    data[2] = static_cast<uint8_t>(state);
    sendFrame(SIM_CAN_ID_FAULT_REPORT, data, 3);
}

// ── Status print ───────────────────────────────────────────────────────────────

void IsmSimEth::printStatusIfChanged() {
    SystemState cur = ism_.getState();
    if (cur != last_printed_state_) {
        std::cout << "[ISM-ETH " << ts_eth() << "] *** STATE CHANGE: "
                  << stateToString(last_printed_state_)
                  << " → " << stateToString(cur)
                  << " | torque=" << (ism_.isTorqueEnabled() ? "ON" : "OFF") << " ***\n";
        last_printed_state_ = cur;
    }
}

void IsmSimEth::setError(const std::string& msg) { last_error_ = msg; }
