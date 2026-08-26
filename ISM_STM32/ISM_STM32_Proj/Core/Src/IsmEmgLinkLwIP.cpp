// STM32H7 implementation of IsmEmgLink using LwIP raw UDP API.
// The Linux/POSIX implementation is in ISM_Firmware/src/IsmEmgLink.cpp.
// Both files implement the same IsmEmgLink class (different .cpp, same header).
#include "IsmEmgLink.h"

extern uint32_t platform_get_tick_ms();

extern "C" {
#include "lwip/udp.h"
#include "lwip/ip_addr.h"
#include "lwip/pbuf.h"
}

#include <cstdio>
#include <cstring>

IsmEmgLink::~IsmEmgLink() {
    if (ready_) {
        udp_remove(reinterpret_cast<struct udp_pcb*>(handle_));
        ready_  = false;
        handle_ = -1;
    }
}

bool IsmEmgLink::init(const std::string& webapp_ip, uint16_t port) {
    webapp_ip_ = webapp_ip;
    port_      = port;
    struct udp_pcb* pcb = udp_new();
    if (!pcb) return false;
    handle_ = reinterpret_cast<intptr_t>(pcb);
    ready_  = true;
    return true;
}

void IsmEmgLink::send(SystemState state, FaultCode code, const std::string& reason) {
    if (!ready_) return;

    std::string json = buildJson(state, code, reason);

    ip_addr_t dest;
    if (!ipaddr_aton(webapp_ip_.c_str(), &dest)) return;

    struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, static_cast<u16_t>(json.size()), PBUF_RAM);
    if (!p) return;
    memcpy(p->payload, json.c_str(), json.size());

    auto* pcb = reinterpret_cast<struct udp_pcb*>(handle_);
    udp_sendto(pcb, p, &dest, port_);
    pbuf_free(p);
}

std::string IsmEmgLink::buildJson(SystemState state, FaultCode code, const std::string& reason) const {
    char ts[16];
    snprintf(ts, sizeof(ts), "T+%lus", (unsigned long)(platform_get_tick_ms() / 1000));

    char buf[512];
    snprintf(buf, sizeof(buf),
        "{\"type\":\"EMG\""
        ",\"ism_state\":\"%s\""
        ",\"fault_code\":%u"
        ",\"fault_hex\":\"%s\""
        ",\"fault_name\":\"%s\""
        ",\"reason\":\"%s\""
        ",\"ts\":\"%s\"}",
        stateToString(state).c_str(),
        static_cast<unsigned>(code),
        faultCodeToHex(code).c_str(),
        faultCodeToString(code).c_str(),
        reason.c_str(),
        ts);
    return std::string(buf);
}
