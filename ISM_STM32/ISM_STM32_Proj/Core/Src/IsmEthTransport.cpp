// STM32H7 LwIP raw-API CAN-over-UDP transport for ISM.
// Mirrors the dispatch logic of ISM_Firmware/src/IsmSimEth.cpp.
//
// Wire format: [id:4LE][len:1][data:8] = 13 bytes per datagram
// FCC → ISM : listen_port (5300)
// ISM → FCC : fcc_port    (5301), learned from first received packet
#include "IsmEthTransport.h"
#include "FaultCode.h"
#include <cstdio>

extern "C" {
#include "lwip/udp.h"
#include "lwip/pbuf.h"
#include "lwip/ip_addr.h"
#include "stm32h7xx_hal.h"
}

#include <cstring>

// CAN IDs — must match IsmProtocol.h on the FCC side
static constexpr uint32_t CAN_ID_HEARTBEAT       = 0x100;
static constexpr uint32_t CAN_ID_ARM             = 0x101;
static constexpr uint32_t CAN_ID_DISARM          = 0x102;
static constexpr uint32_t CAN_ID_STANDBY         = 0x103;
static constexpr uint32_t CAN_ID_MOTION          = 0x110;
static constexpr uint32_t CAN_ID_FIRE_REQUEST    = 0x120;
static constexpr uint32_t CAN_ID_RECOVERY_RESET  = 0x130;
static constexpr uint32_t CAN_ID_SAFETY_CHANNEL_A = 0x140;  // B5/B6 dual-channel FIRE interlock, SWR-SAFE-004
static constexpr uint32_t CAN_ID_SAFETY_CHANNEL_B = 0x141;  // sent as two separate messages, see FCC_App's IsmProtocol.h
static constexpr uint32_t CAN_ID_INJECT_RESET    = 0x1FE;
static constexpr uint32_t CAN_ID_INJECT_INTERNAL = 0x1FD;
static constexpr uint32_t CAN_ID_STATE_ACK       = 0x200;
static constexpr uint32_t CAN_ID_FAULT_REPORT    = 0x210;
static constexpr uint32_t CAN_ID_HEARTBEAT_ACK   = 0x211;
static constexpr int      WIRE_BYTES             = 13;

static ISM*            s_ism        = nullptr;
static struct udp_pcb* s_send_pcb   = nullptr;
static ip_addr_t       s_fcc_addr{};
static uint16_t        s_fcc_port   = 5301;
static bool            s_fcc_known  = false;
static uint32_t        s_hb_count   = 0;
static uint32_t        s_rx_count   = 0;

// ── Send helpers ──────────────────────────────────────────────────────────────

static void send_frame(uint32_t id, const uint8_t* data, uint8_t len) {
    if (!s_fcc_known || !s_send_pcb) return;
    uint8_t buf[WIRE_BYTES];
    memcpy(buf,     &id,  4);
    buf[4] = len;
    memcpy(buf + 5, data, 8);
    struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, WIRE_BYTES, PBUF_RAM);
    if (!p) {
        printf("[ETH] pbuf_alloc failed — id=0x%03lX (pool exhausted?)\r\n", (unsigned long)id);
        return;
    }
    memcpy(p->payload, buf, WIRE_BYTES);
	err_t e = udp_sendto(s_send_pcb, p, &s_fcc_addr, s_fcc_port);
    if (e != ERR_OK)
        printf("[ETH] udp_sendto err=%d id=0x%03lX\r\n", (int)e, (unsigned long)id);
    pbuf_free(p);
}

static void send_state_ack(SystemState state) {
    uint8_t d[2]{ static_cast<uint8_t>(state), 0 };
    send_frame(CAN_ID_STATE_ACK, d, 2);
}

static void send_fault_report(uint16_t code, SystemState state) {
    uint8_t d[3]{};
    memcpy(d, &code, 2);
    d[2] = static_cast<uint8_t>(state);
    send_frame(CAN_ID_FAULT_REPORT, d, 3);
}

static void send_heartbeat_ack(uint32_t seq) {
    uint8_t d[5]{};
    memcpy(d, &seq, 4);
    d[4] = static_cast<uint8_t>(s_ism->getState());
    send_frame(CAN_ID_HEARTBEAT_ACK, d, 5);
}

// ── Dispatch — mirrors IsmSimEth::dispatch() ──────────────────────────────────

static void dispatch(uint32_t id, const uint8_t* data, uint8_t len) {
    SystemState state_before = s_ism->getState();

    switch (id) {

        case CAN_ID_HEARTBEAT: {
            uint32_t seq = 0;
            if (len >= 4) memcpy(&seq, data, 4);
            s_hb_count++;
            FccMessage msg;
            msg.type   = FccCommandType::HEARTBEAT;
            msg.seq    = seq;
            msg.source = "FCC";
            s_ism->processMessage(msg);
            if (s_hb_count % 10 == 0) send_heartbeat_ack(seq);
            if (s_ism->getLastFaultCode() == FaultCode::WATCHDOG_TIMEOUT ||
                s_ism->getLastFaultCode() == FaultCode::COMM_LOSS) {
                s_ism->enterSafe("Power-up initialisation");
                s_ism->clearLastFaultCode();
            }
            break;
        }
        case CAN_ID_STANDBY: {
            uint32_t seq = 0;
            if (len >= 4) memcpy(&seq, data, 4);
            FccMessage msg; msg.type = FccCommandType::STANDBY; msg.seq = seq; msg.source = "FCC";
            s_ism->processMessage(msg);
            send_state_ack(s_ism->getState());
            break;
        }
        case CAN_ID_ARM: {
            uint32_t seq = 0;
            if (len >= 4) memcpy(&seq, data, 4);
            FccMessage msg; msg.type = FccCommandType::ARM; msg.seq = seq; msg.source = "FCC";
            s_ism->processMessage(msg);
            send_state_ack(s_ism->getState());
            break;
        }
        case CAN_ID_DISARM: {
            uint32_t seq = 0;
            if (len >= 4) memcpy(&seq, data, 4);
            FccMessage msg; msg.type = FccCommandType::DISARM; msg.seq = seq; msg.source = "FCC";
            s_ism->processMessage(msg);
            send_state_ack(s_ism->getState());
            break;
        }
        case CAN_ID_MOTION: {
            uint16_t seq16 = 0;
            int16_t az = 0, el = 0, rate = 0;
            if (len >= 2) memcpy(&seq16, data,     2);
            if (len >= 4) memcpy(&az,    data + 2, 2);
            if (len >= 6) memcpy(&el,    data + 4, 2);
            if (len >= 8) memcpy(&rate,  data + 6, 2);
            FccMessage msg;
            msg.type = FccCommandType::MOTION_COMMAND;
            msg.seq  = seq16; msg.source = "FCC";
            msg.motion.azimuth_deg   = az   / 10.0f;
            msg.motion.elevation_deg = el   / 10.0f;
            msg.motion.rate_dps      = rate / 10.0f;
            bool ok = s_ism->processMessage(msg);
            if (!ok && s_ism->getState() == SystemState::FAULT)
                send_fault_report(static_cast<uint16_t>(s_ism->getLastFaultCode()), s_ism->getState());
            break;
        }
        case CAN_ID_FIRE_REQUEST: {
            uint32_t seq = 0;
            if (len >= 4) memcpy(&seq, data, 4);
            FccMessage msg; msg.type = FccCommandType::FIRE_REQUEST; msg.seq = seq; msg.source = "FCC";
            s_ism->processMessage(msg);
            send_state_ack(s_ism->getState());
            break;
        }
        case CAN_ID_RECOVERY_RESET: {
            uint32_t seq = 0;
            if (len >= 4) memcpy(&seq, data, 4);
            FccMessage msg; msg.type = FccCommandType::RECOVERY_RESET; msg.seq = seq; msg.source = "OPERATOR";
            s_ism->processMessage(msg);
            send_state_ack(s_ism->getState());
            break;
        }
        case CAN_ID_SAFETY_CHANNEL_A: {
            bool open = (len >= 1) && (data[0] != 0);
            s_ism->interlock().setChannelA(open);
            printf("[ETH-CAN] safety channel A -> %s\r\n", open ? "OPEN" : "CLOSED");
            send_state_ack(s_ism->getState());
            break;
        }
        case CAN_ID_SAFETY_CHANNEL_B: {
            bool open = (len >= 1) && (data[0] != 0);
            s_ism->interlock().setChannelB(open);
            printf("[ETH-CAN] safety channel B -> %s\r\n", open ? "OPEN" : "CLOSED");
            send_state_ack(s_ism->getState());
            break;
        }
        case CAN_ID_INJECT_RESET: {
            s_ism->enterSafe("SYSTEM RESET (injected)");
            s_ism->clearLastFaultCode();
            send_state_ack(s_ism->getState());
            break;
        }
        case CAN_ID_INJECT_INTERNAL: {
            uint32_t seq = 0;
            if (len >= 4) memcpy(&seq, data, 4);
            FccMessage msg;
            msg.type   = static_cast<FccCommandType>(0xFE);
            msg.seq    = seq;
            msg.source = "ISM-SELF";
            s_ism->processMessage(msg);
            send_fault_report(static_cast<uint16_t>(s_ism->getLastFaultCode()), s_ism->getState());
            break;
        }
        default: {
            uint32_t seq = 0;
            if (len >= 4) memcpy(&seq, data, 4);
            FccMessage msg;
            msg.type   = static_cast<FccCommandType>(0xFF);
            msg.seq    = seq;
            msg.source = "FCC";
            bool ok = s_ism->processMessage(msg);
            if (!ok)
                send_fault_report(static_cast<uint16_t>(s_ism->getLastFaultCode()), s_ism->getState());
            break;
        }
    }

    if (s_ism->getState() != state_before)
        send_state_ack(s_ism->getState());
}

// ── LwIP UDP receive callback ─────────────────────────────────────────────────

static void udp_recv_cb(void* /*arg*/, struct udp_pcb* /*pcb*/, struct pbuf* p,
                        const ip_addr_t* addr, u16_t /*port*/) {
    if (!p) return;
    s_rx_count++;

    if (!s_fcc_known) {
        s_fcc_addr  = *addr;
        s_fcc_known = true;
        printf("[ETH-CAN] first packet from FCC — learned peer address\r\n");
    }

    printf("[ETH-CAN] udp_recv_cb FIRED, tot_len=%u\r\n", p->tot_len);
    if (p->tot_len >= WIRE_BYTES) {
        uint8_t buf[WIRE_BYTES];
        pbuf_copy_partial(p, buf, WIRE_BYTES, 0);

        uint32_t id;
        memcpy(&id, buf, 4);
        uint8_t  len  = buf[4];
        dispatch(id, buf + 5, len);
    }

    pbuf_free(p);
}

// ── Init ──────────────────────────────────────────────────────────────────────

void ism_eth_transport_init(ISM& ism, uint16_t listen_port, uint16_t fcc_port) {
    s_ism      = &ism;
    s_fcc_port = fcc_port;

    struct udp_pcb* listen_pcb = udp_new();
    if (!listen_pcb) {
        printf("[ETH-CAN] udp_new() FAILED for listen_pcb — PCB pool exhausted?\r\n");
        return;
    }
    err_t berr = udp_bind(listen_pcb, IP_ADDR_ANY, listen_port);
    if (berr != ERR_OK) {
        printf("[ETH-CAN] udp_bind FAILED on port %u, err=%d\r\n", listen_port, (int)berr);
    } else {
        printf("[ETH-CAN] listening on UDP port %u (FCC->ISM)\r\n", listen_port);
    }
    udp_recv(listen_pcb, udp_recv_cb, nullptr);

    s_send_pcb = udp_new();
    if (!s_send_pcb)
        printf("[ETH-CAN] udp_new() FAILED for s_send_pcb\r\n");
    printf("[ETH-CAN] init done, fcc_port=%u\r\n", fcc_port);
}

// ── Periodic ETH diagnostic/heartbeat tick ────────────────────────────────────
// Call from main loop (every ms via HAL_GetTick comparison) — mirrors
// ism_can_tick()'s unconditional health-check pattern. Unlike CAN (a shared
// bus, no destination needed), UDP needs a known peer address to send to, so
// this can only actually transmit once s_fcc_known is true (i.e. at least one
// packet has ever arrived from the FCC) — but it ALWAYS prints a diagnostic
// line every second regardless, so "is anything arriving at all" doesn't
// depend on 10 real heartbeats lining up first (dispatch()'s CAN_ID_HEARTBEAT
// case only sends an ACK every 10th received heartbeat).
extern "C" uint32_t ism_eth_get_rx_count(void) { return s_rx_count; }

extern "C" void ism_eth_tick(void) {
    if (!s_ism) return;
    static uint32_t s_last_tick_ms = 0;
    uint32_t now = HAL_GetTick();
    if (now - s_last_tick_ms < 1000U) return;
    s_last_tick_ms = now;

    if (s_fcc_known) send_heartbeat_ack(s_hb_count);

    printf("[ETH-CAN] tick: fcc_known=%d rx_total=%lu hb_rx=%lu\r\n",
           (int)s_fcc_known, (unsigned long)s_rx_count, (unsigned long)s_hb_count);

    // Dump lwIP's global UDP PCB list EVERY tick (not just once) — confirms
    // whether our listen_pcb (port 5300) is STILL registered at each point in
    // time, not just at startup. If it ever disappears or changes, something
    // later is corrupting/removing it.
    {
        printf("[ETH-CAN] udp_pcbs dump:\r\n");
        int n = 0;
        for (struct udp_pcb* p = udp_pcbs; p != nullptr; p = p->next) {
            printf("  [%d] local_port=%u remote_port=%u recv_cb=%p local_ip=0x%08lX\r\n",
                   n++, p->local_port, p->remote_port, (void*)p->recv,
                   (unsigned long)ip_2_ip4(&p->local_ip)->addr);
        }
        if (n == 0) printf("  (empty — no PCBs registered at all!)\r\n");
    }
}
