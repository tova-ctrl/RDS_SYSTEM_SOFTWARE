#pragma once
// FDCAN1 transport for ISM — replaces IsmEthTransport.
// PB8 = FDCAN1_RX (AF9), PB9 = FDCAN1_TX (AF9) — see IsmCanTransport.cpp
// header comment for the real pin/clock story (this file's old "PD0/PD1"
// claim was stale/never-true leftover text, unrelated to actual GPIO code).
// 500 kbps @ 75MHz PLL1Q kernel clock, classical CAN (11-bit ID, up to 8 data bytes)

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
