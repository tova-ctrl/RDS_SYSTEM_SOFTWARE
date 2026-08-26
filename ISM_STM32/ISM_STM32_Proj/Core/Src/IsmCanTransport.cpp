// FDCAN1 CAN transport for ISM on STM32H7.
// PB8 = FDCAN1_RX (AF9) — CN7 pin 2. PB9 = FDCAN1_TX (AF9) — CN7 pin 4.
// 500 kbps @ 8 MHz HSE kernel clock (Prescaler=1, TS1=13, TS2=2).
//
// CAN ID space matches IsmProtocol.h on the FCC (Jetson) side.
// The FCC uses Linux SocketCAN (can0), which sends standard 11-bit CAN frames.

#include "IsmCanTransport.h"
#include "FaultCode.h"
#include <cstring>
#include <cstdio>

// ── CAN IDs — must match IsmProtocol.h on the FCC ────────────────────────────
static constexpr uint32_t CAN_ID_HEARTBEAT       = 0x100;
static constexpr uint32_t CAN_ID_ARM             = 0x101;
static constexpr uint32_t CAN_ID_DISARM          = 0x102;
static constexpr uint32_t CAN_ID_STANDBY         = 0x103;
static constexpr uint32_t CAN_ID_MOTION          = 0x110;
static constexpr uint32_t CAN_ID_FIRE_REQUEST    = 0x120;
static constexpr uint32_t CAN_ID_RECOVERY_RESET  = 0x130;
static constexpr uint32_t CAN_ID_INJECT_RESET    = 0x1FE;
static constexpr uint32_t CAN_ID_INJECT_INTERNAL = 0x1FD;
static constexpr uint32_t CAN_ID_STATE_ACK       = 0x200;
static constexpr uint32_t CAN_ID_FAULT_REPORT    = 0x210;
static constexpr uint32_t CAN_ID_HEARTBEAT_ACK   = 0x211;

FDCAN_HandleTypeDef hfdcan1;

static ISM*     s_ism      = nullptr;
static uint32_t s_hb_count = 0;
static uint32_t s_rx_count = 0;

// ── Helpers ───────────────────────────────────────────────────────────────────

// HAL stores DataLength as raw DLC (0-15), NOT shifted.
// See stm32h7xx_hal_fdcan.c line 3084: DataLength = (*RxAddress & MASK_DLC) >> 16
static uint8_t dlc_to_bytes(uint32_t dlc) {
    static const uint8_t tbl[] = {0,1,2,3,4,5,6,7,8,12,16,20,24,32,48,64};
    return (dlc < 16U) ? tbl[dlc] : 8U;
}

static void can_send(uint32_t id, const uint8_t* data, uint8_t len) {
    FDCAN_TxHeaderTypeDef hdr{};
    hdr.Identifier          = id;
    hdr.IdType              = FDCAN_STANDARD_ID;
    hdr.TxFrameType         = FDCAN_DATA_FRAME;
    hdr.DataLength          = static_cast<uint32_t>(len);  // FDCAN_DLC_BYTES_N = N (HAL shifts internally)
    hdr.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    hdr.BitRateSwitch       = FDCAN_BRS_OFF;
    hdr.FDFormat            = FDCAN_CLASSIC_CAN;
    hdr.TxEventFifoControl  = FDCAN_NO_TX_EVENTS;
    hdr.MessageMarker       = 0;

    uint8_t buf[8] = {};
    if (len > 8) len = 8;
    memcpy(buf, data, len);

    HAL_StatusTypeDef r = HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &hdr, buf);
    if (r != HAL_OK) {
        uint32_t psr = hfdcan1.Instance->PSR;  // protocol status register
        printf("[CAN] TX id=0x%03lX len=%d FAIL hal=%d PSR=0x%08lX\r\n",
               id, len, (int)r, psr);
    }
}

static void send_state_ack(SystemState state) {
    uint8_t d[2]{ static_cast<uint8_t>(state), 0 };
    can_send(CAN_ID_STATE_ACK, d, 2);
}

static void send_fault_report(uint16_t code, SystemState state) {
    uint8_t d[3]{};
    memcpy(d, &code, 2);
    d[2] = static_cast<uint8_t>(state);
    can_send(CAN_ID_FAULT_REPORT, d, 3);
}

static void send_heartbeat_ack(uint32_t seq) {
    uint8_t d[5]{};
    memcpy(d, &seq, 4);
    d[4] = static_cast<uint8_t>(s_ism->getState());
    can_send(CAN_ID_HEARTBEAT_ACK, d, 5);
}

// ── Dispatch — identical logic to IsmEthTransport ────────────────────────────

static void dispatch(uint32_t id, const uint8_t* data, uint8_t len) {
    SystemState state_before = s_ism->getState();

    switch (id) {
        case CAN_ID_HEARTBEAT: {
            uint32_t seq = 0;
            if (len >= 4) memcpy(&seq, data, 4);
            s_hb_count++;
            FccMessage msg;
            msg.type = FccCommandType::HEARTBEAT; msg.seq = seq; msg.source = "FCC";
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
            msg.seq    = seq; msg.source = "ISM-SELF";
            s_ism->processMessage(msg);
            send_fault_report(static_cast<uint16_t>(s_ism->getLastFaultCode()), s_ism->getState());
            break;
        }
        default: {
            uint32_t seq = 0;
            if (len >= 4) memcpy(&seq, data, 4);
            FccMessage msg;
            msg.type   = static_cast<FccCommandType>(0xFF);
            msg.seq    = seq; msg.source = "FCC";
            bool ok = s_ism->processMessage(msg);
            if (!ok)
                send_fault_report(static_cast<uint16_t>(s_ism->getLastFaultCode()), s_ism->getState());
            break;
        }
    }

    if (s_ism->getState() != state_before)
        send_state_ack(s_ism->getState());
}

// RX is handled by polling in ism_can_tick() — no interrupt callback needed.

// ── FDCAN1 peripheral init ────────────────────────────────────────────────────

extern "C" void MX_FDCAN1_Init(void) {
    // GPIO: PB8 = FDCAN1_RX (AF9), PB9 = FDCAN1_TX (AF9)
    // On NUCLEO-H753ZI: CN7 pin 2 = D15/SCL = PB8, CN7 pin 4 = D14/SDA = PB9.
    // PA11/PA12 are FDCAN1-capable on the chip but wired to USB OTG on the NUCLEO board.
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef g{};
    g.Pin       = GPIO_PIN_8 | GPIO_PIN_9;
    g.Mode      = GPIO_MODE_AF_PP;
    g.Pull      = GPIO_NOPULL;
    g.Speed     = GPIO_SPEED_FREQ_HIGH;
    g.Alternate = GPIO_AF9_FDCAN1;
    HAL_GPIO_Init(GPIOB, &g);

    __HAL_RCC_FDCAN_CLK_ENABLE();

    // 500 kbps @ 8 MHz HSE kernel clock
    // Prescaler=1, TS1=13, TS2=2, SJW=1 → 8e6/(1*(1+13+2)) = 500 000 bps
    hfdcan1.Instance                  = FDCAN1;
    hfdcan1.Init.FrameFormat          = FDCAN_FRAME_CLASSIC;
    hfdcan1.Init.Mode                 = FDCAN_MODE_NORMAL;  //===TOVA 19.7.26 ==FDCAN_MODE_NORMAL;
    hfdcan1.Init.AutoRetransmission   = DISABLE;
    hfdcan1.Init.TransmitPause        = DISABLE;
    hfdcan1.Init.ProtocolException    = DISABLE;
    hfdcan1.Init.NominalPrescaler     = 1;
    hfdcan1.Init.NominalSyncJumpWidth = 1;
    hfdcan1.Init.NominalTimeSeg1      = 13;
    hfdcan1.Init.NominalTimeSeg2      = 2;
    hfdcan1.Init.MessageRAMOffset     = 0;
    hfdcan1.Init.StdFiltersNbr        = 1;
    hfdcan1.Init.ExtFiltersNbr        = 0;
    hfdcan1.Init.RxFifo0ElmtsNbr     = 8;
    hfdcan1.Init.RxFifo0ElmtSize      = FDCAN_DATA_BYTES_8;
    hfdcan1.Init.RxFifo1ElmtsNbr     = 0;
    hfdcan1.Init.RxFifo1ElmtSize      = FDCAN_DATA_BYTES_8;
    hfdcan1.Init.TxEventsNbr          = 0;
    hfdcan1.Init.TxBuffersNbr         = 0;
    hfdcan1.Init.TxFifoQueueElmtsNbr  = 4;
    hfdcan1.Init.TxFifoQueueMode      = FDCAN_TX_FIFO_OPERATION;
    hfdcan1.Init.TxElmtSize           = FDCAN_DATA_BYTES_8;

    if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK) {
        printf("[CAN] FDCAN1 init FAILED\r\n");
        return;
    }

    // Accept all standard 11-bit frames into RX FIFO0
    FDCAN_FilterTypeDef f{};
    f.IdType       = FDCAN_STANDARD_ID;
    f.FilterIndex  = 0;
    f.FilterType   = FDCAN_FILTER_MASK;
    f.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    f.FilterID1    = 0x000;
    f.FilterID2    = 0x000;  // mask=0 → accept all
    HAL_FDCAN_ConfigFilter(&hfdcan1, &f);

    HAL_FDCAN_ConfigGlobalFilter(&hfdcan1,
        FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE,
        FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);

    // RX is polled in ism_can_tick() — no interrupt needed.

    if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK) {
        printf("[CAN] FDCAN1 start FAILED\r\n");
        return;
    }
    printf("[CAN] FDCAN1 ready: NORMAL 500kbps PB8/PB9\r\n");
}

// ── Periodic CAN heartbeat from STM32 → FCC ──────────────────────────────────
// Call from main loop (every ms via HAL_GetTick comparison).
// Sends CAN_ID_HEARTBEAT_ACK with current ISM state every 1000 ms.
extern "C" void ism_can_tick(void) {
    if (!s_ism) return;
    static uint32_t s_last_hb_ms   = 0;
    static uint32_t s_last_diag_ms = 0;
    uint32_t now = HAL_GetTick();

    // Poll RX FIFO0 — drains every frame that arrived since last tick.
    {
        FDCAN_RxHeaderTypeDef hdr{};
        uint8_t data[8] = {};
		while (HAL_FDCAN_GetRxFifoFillLevel(&hfdcan1, FDCAN_RX_FIFO0) > 0) {
            if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &hdr, data) != HAL_OK)
                break;
            if (hdr.IdType != FDCAN_STANDARD_ID) continue;
            uint8_t len = dlc_to_bytes(hdr.DataLength);
            s_rx_count++;
            printf("[CAN RX] id=0x%03lX len=%d\r\n", hdr.Identifier, len);
            if (hdr.Identifier < 0x200U)
                dispatch(hdr.Identifier, data, len);
        }
    }

    if (now - s_last_hb_ms >= 1000U) {
        s_last_hb_ms = now;
        uint32_t seq = s_hb_count++;
        send_heartbeat_ack(seq);
        printf("[CAN HB] seq=%lu state=%d\r\n", seq, (int)s_ism->getState());
    }

    if (now - s_last_diag_ms >= 5000U) {
        s_last_diag_ms = now;
        uint32_t psr = hfdcan1.Instance->PSR;
        uint8_t  lec = psr & 0x7U;
        uint8_t  ep  = (psr >> 5) & 1U;
        uint8_t  bo  = (psr >> 7) & 1U;
        printf("[CAN DIAG] PSR=0x%08lX LEC=%d EP=%d BO=%d rx_total=%lu\r\n",
               psr, lec, ep, bo, s_rx_count);
    }
}

// ── Transport init ────────────────────────────────────────────────────────────

void ism_can_transport_init(ISM& ism) {
    s_ism = &ism;
}
