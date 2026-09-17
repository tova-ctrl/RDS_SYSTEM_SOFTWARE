#pragma once
// STM32H7 LwIP UDP transport for ISM ↔ FCC communication.
// Receives CAN-over-UDP frames from FCC (port 5300), dispatches through ISM,
// sends STATE_ACK / HEARTBEAT_ACK / FAULT_REPORT back to FCC (port 5301).
#include "ISM.h"
#include <cstdint>

// Call once after LwIP is initialized (from ism_glue.cpp / main.c USER CODE).
void ism_eth_transport_init(ISM& ism,
                            uint16_t listen_port = 5300,
                            uint16_t fcc_port    = 5301);

// Call every iteration of main's while(1) — periodic diagnostic + unconditional
// heartbeat-ack (see IsmEthTransport.cpp for details). extern "C" so main.c
// (plain C) can call it with the same linkage as its definition.
#ifdef __cplusplus
extern "C" {
#endif
void ism_eth_tick(void);
uint32_t ism_eth_get_rx_count(void);
#ifdef __cplusplus
}
#endif
