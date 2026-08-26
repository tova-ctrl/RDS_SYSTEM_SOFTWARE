// ISM global instance and C-callable entry points for STM32 integration.
//
// Call from main.c (USER CODE sections):
//   ism_init()   — once, after MX_LWIP_Init()
//   ism_tick()   — every ms, driven from main loop using HAL_GetTick()
//
// The LwIP UDP receive callback (in IsmEthTransport.cpp) is invoked
// automatically by MX_LWIP_Process() when a UDP packet arrives from the FCC.
#include "ISM.h"
#include "IsmEthTransport.h"
#include "IsmEmgLink.h"
#include "IsmCanTransport.h"

// WebApp PC address for EMG (emergency) notifications on port 5202.
static constexpr const char* WEBAPP_IP = "192.168.55.5";

static ISM s_ism;

extern "C" void ism_init(void) {
    s_ism.initEmgLink(WEBAPP_IP, IsmEmgLink::DEFAULT_EMG_PORT);
    ism_eth_transport_init(s_ism);  // FCC<->ISM over UDP (port 5300/5301)
    MX_FDCAN1_Init();
    ism_can_transport_init(s_ism);
}

extern "C" void ism_tick(void) {
    s_ism.tick();
}

extern "C" void ism_cmd_standby(uint32_t seq) {
    FccMessage msg;
    msg.type = FccCommandType::STANDBY;
    msg.seq  = seq;
    s_ism.processMessage(msg);
}

extern "C" void ism_cmd_arm(uint32_t seq) {
    FccMessage msg;
    msg.type = FccCommandType::ARM;
    msg.seq  = seq;
    s_ism.processMessage(msg);
}

extern "C" void ism_cmd_disarm(uint32_t seq) {
    FccMessage msg;
    msg.type = FccCommandType::DISARM;
    msg.seq  = seq;
    s_ism.processMessage(msg);
}

extern "C" void ism_cmd_fire(uint32_t seq) {
    FccMessage msg;
    msg.type = FccCommandType::FIRE_REQUEST;
    msg.seq  = seq;
    s_ism.processMessage(msg);
}
