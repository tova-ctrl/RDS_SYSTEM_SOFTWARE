#pragma once
// FDCAN1 transport for ISM — replaces IsmEthTransport.
// PD0 = FDCAN1_RX (AF9), PD1 = FDCAN1_TX (AF9)
// 500 kbps, classical CAN (11-bit ID, up to 8 data bytes)

// C++ only: ISM class + ism_can_transport_init declaration
#ifdef __cplusplus
#include "ISM.h"
extern "C" {
#endif

#include "stm32h7xx_hal.h"

extern FDCAN_HandleTypeDef hfdcan1;
void MX_FDCAN1_Init(void);
void ism_can_tick(void);

#ifdef __cplusplus
} // extern "C"
void ism_can_transport_init(ISM& ism);
#endif
