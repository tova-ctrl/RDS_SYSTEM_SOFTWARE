// =============================================================================
// main_sim.cpp — ISM Simulator on Linux/WSL
//
// מדמה את ה-STM32H7: מריץ את ה-ISM האמיתי מעל SocketCAN (vcan0)
// מקבל הודעות מה-FCC, מעבד דרך ISM state machine, שולח תגובות בחזרה.
//
// Usage:
//   ./ism_sim [can_interface]
//   ./ism_sim vcan0
// =============================================================================

#include "ISM.h"
#include "IsmSimCan.h"
#include "IsmSimEth.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
#include <string>
#include <cstdint>

using namespace std::chrono_literals;

static std::atomic<bool> g_shutdown{ false };
static void onSignal(int) { g_shutdown = true; }

// In WSL2 the Windows host is the default gateway (not the DNS nameserver).
// Parse /proc/net/route: find the row with destination 00000000 and decode the gateway.
static std::string detectWindowsHostIp() {
    std::ifstream f("/proc/net/route");
    std::string line;
    std::getline(f, line); // skip header
    while (std::getline(f, line)) {
        std::istringstream ss(line);
        std::string iface, dest, gw;
        ss >> iface >> dest >> gw;
        if (dest == "00000000" && gw != "00000000") {
            // Gateway is a 32-bit value in little-endian hex (host byte order on x86).
            uint32_t g = static_cast<uint32_t>(std::stoul(gw, nullptr, 16));
            return std::to_string( g        & 0xFF) + "." +
                   std::to_string((g >>  8) & 0xFF) + "." +
                   std::to_string((g >> 16) & 0xFF) + "." +
                   std::to_string((g >> 24) & 0xFF);
        }
    }
    return "127.0.0.1";
}

// Usage:
//   ./ism_sim [can_if]  [webapp_ip] [emg_port]
//   ./ism_sim vcan0     192.168.10.1 5202        ← CAN mode (default)
//   ./ism_sim eth       192.168.10.1 5202        ← UDP mode (no CAN hardware)
int main(int argc, char* argv[]) {
    std::string can_if    = (argc > 1) ? argv[1] : "vcan0";
    std::string webapp_ip = (argc > 2) ? argv[2] : detectWindowsHostIp();
    uint16_t    emg_port  = (argc > 3) ? static_cast<uint16_t>(std::stoi(argv[3]))
                                       : 5202;

    const bool use_eth = (can_if == "eth");

    std::cout << "=== ISM Simulator — Aleppo Systems / RDS ===\n";
    std::cout << "Transport     : " << (use_eth ? "UDP (eth)" : "SocketCAN (" + can_if + ")") << "\n";
    std::cout << "EMG UDP target: " << webapp_ip << ":" << emg_port << "\n";
    if (use_eth)
        std::cout << "ETH listen    : UDP :5300 (FCC → ISM)\n"
                  << "ETH reply     : UDP :5301 (ISM → FCC, learned from first packet)\n";
    std::cout << "Press Ctrl+C to stop.\n\n";

    std::signal(SIGINT,  onSignal);
    std::signal(SIGTERM, onSignal);

    ISM ism;
    if (ism.initEmgLink(webapp_ip, emg_port))
        std::cout << "[ISM-SIM] EMG link ready → " << webapp_ip << ":" << emg_port << "\n";
    else
        std::cout << "[ISM-SIM] WARNING: EMG link failed to open socket\n";

    if (use_eth) {
        // ── ETH mode ─────────────────────────────────────────────────────────
        IsmSimEth transport(ism);
        if (!transport.open()) {
            std::cerr << "[ISM-SIM] ERROR: " << transport.lastError() << "\n";
            return 1;
        }
        std::cout << "[ISM-SIM] Running (ETH). State=SAFE, waiting for FCC heartbeat...\n\n";
        while (!g_shutdown) {
            ism.tick();
            transport.processIncoming();
            transport.printStatusIfChanged();
            std::this_thread::sleep_for(1ms);
        }
        std::cout << "\n[ISM-SIM] Shutdown.\n";
        transport.close();
    } else {
        // ── CAN mode ─────────────────────────────────────────────────────────
        IsmSimCan transport(can_if, ism);
        if (!transport.open()) {
            std::cerr << "[ISM-SIM] ERROR: " << transport.lastError() << "\n";
            return 1;
        }
        std::cout << "[ISM-SIM] Running (CAN). State=SAFE, waiting for FCC heartbeat...\n\n";
        while (!g_shutdown) {
            ism.tick();
            transport.processIncoming();
            transport.printStatusIfChanged();
            std::this_thread::sleep_for(1ms);
        }
        std::cout << "\n[ISM-SIM] Shutdown.\n";
        transport.close();
    }

    return 0;
}
