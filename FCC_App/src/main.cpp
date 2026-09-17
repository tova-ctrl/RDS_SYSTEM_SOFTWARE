// =============================================================================
// FCC main — NVIDIA Jetson / Linux entry point
//
// Usage:
//   ./fcc_app [can_interface] [gcs_ip] [gcs_port]
//   ./fcc_app can0 192.168.1.100 5200
//
// 2026-09-09: CanBus::open() now brings the interface up itself (bitrate
// 500000 — confirmed correct on real hardware, see IsmCanTransport.cpp's
// FDCAN bit-timing fix; the STM32's actual FDCAN kernel clock is 75MHz
// PLL1Q, not the 8MHz that would justify a 1Mbit "round number" guess), so
// no manual `ip link` step is required before running this anymore.
// =============================================================================

#include "ICanBus.h"
#include "CanBus.h"
#include "EthCanBus.h"
#include "GcsLink.h"
#include "FccController.h"
#include "HttpServer.h"
#include "EthernetServer.h"
#include "Logger.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <csignal>
#include <atomic>
#include <thread>
#include <chrono>
#include <string>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <mutex>
//====Tova 19.7.26======
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>
//======================
CircularLog g_log(1000);

using namespace std::chrono_literals;

// ── Graceful shutdown on Ctrl+C ───────────────────────────────────────────────
//static 
std::atomic<bool> g_shutdown{ false };
static void onSignal(int) { g_shutdown = true; }

// Read default gateway from /proc/net/route — in WSL2 this is the Windows host.
static std::string detectWindowsHostIp() {
    std::ifstream f("/proc/net/route");
    std::string line;
    std::getline(f, line); // skip header
    while (std::getline(f, line)) {
        std::istringstream ss(line);
        std::string iface, dest, gw;
        ss >> iface >> dest >> gw;
        if (dest == "00000000" && gw != "00000000") {
            uint32_t g = static_cast<uint32_t>(std::stoul(gw, nullptr, 16));
            return std::to_string( g        & 0xFF) + "." +
                   std::to_string((g >>  8) & 0xFF) + "." +
                   std::to_string((g >> 16) & 0xFF) + "." +
                   std::to_string((g >> 24) & 0xFF);
        }
    }
    return "127.0.0.1";
}

int configure_uart(int fd) {
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) return -1;

  //  cfsetospeed(&tty, B115200);         //==TOVA TST ==27.7.26==
  //  cfsetispeed(&tty, B115200);         //==TOVA TST ==27.7.26==
    cfsetospeed(&tty, B9600);  //==20.7.26 tova == 
    cfsetispeed(&tty, B9600);  //==20.7.26 tova ==
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8 bits
   //==20.7.26 tova == tty.c_iflag &= ~IGNBRK;                     // disable break processing
    tty.c_iflag = 0;
    tty.c_lflag = 0;                            // no signaling chars, no echo
    tty.c_oflag = 0;                            // no remapping, no delays
    tty.c_cc[VMIN] = 0;                        // non-blocking read
    tty.c_cc[VTIME] = 5;                        // 0.5 seconds read timeout

    tty.c_cflag |= (CLOCAL | CREAD);            // turn on READ & ignore ctrl lines
    tty.c_cflag &= ~(PARENB | PARODD);          // shut off parity
    tty.c_cflag &= ~CSTOPB;                     // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;                    // no flow control

    if (tcsetattr(fd, TCSANOW, &tty) != 0) return -1;
    return 0;
}
//======================
// ── Config defaults ───────────────────────────────────────────────────────────
static constexpr const char* DEFAULT_CAN_IF    = "can0";
static constexpr uint16_t    DEFAULT_GCS_PORT  = 5200;
static constexpr uint16_t    DEFAULT_LISTEN_PORT = 5201;

#define SERIAL_PORT "/dev/ttyTHS1" 
int uart_fd = -1;
std::mutex g_console_mutex;

int main(int argc, char* argv[]) {
    // ── Parse args ────────────────────────────────────────────────────────
    std::string can_if    = (argc > 1) ? argv[1] : DEFAULT_CAN_IF;
    std::string gcs_ip    = (argc > 2) ? argv[2] : detectWindowsHostIp();
    uint16_t    gcs_port  = (argc > 3) ? static_cast<uint16_t>(std::stoi(argv[3]))
                                       : DEFAULT_GCS_PORT;

    std::cout << "=== FCC Application — Aleppo Systems / RDS ===\n";
    std::cout << "CAN interface : " << can_if  << "\n";
    std::cout << "GCS address   : " << gcs_ip  << ":" << gcs_port << "\n";
    std::cout << "Listen port   : " << DEFAULT_LISTEN_PORT << "\n";
    std::cout << "HTTP API      : http://localhost:" << HttpServer::DEFAULT_PORT << "/api/status\n";
    std::cout << "Press Ctrl+C to stop.\n\n";

    // ── Signal handler ────────────────────────────────────────────────────
    std::signal(SIGINT,  onSignal);
    std::signal(SIGTERM, onSignal);

    // ── Open CAN/ETH transport ────────────────────────────────────────────
    // If can_if starts with "eth:", use UDP tunnel (no CAN hardware needed).
    // e.g.:  ./fcc_app eth:192.168.10.1 192.168.10.1 5200
    const bool use_eth = (can_if.rfind("eth:", 0) == 0);
    std::unique_ptr<CanBus>    can_sock;
    std::unique_ptr<EthCanBus> can_eth;
    ICanBus* can_ptr = nullptr;

    if (use_eth) {
        std::string ism_ip = can_if.substr(4);
        can_eth = std::make_unique<EthCanBus>(ism_ip);
        if (!can_eth->open()) {
            std::cerr << "[FCC] ERROR: Cannot open ETH CAN: " << can_eth->lastError() << "\n";
            return 1;
        }
        can_ptr = can_eth.get();
        std::cout << "[FCC] ETH CAN transport → ISM at " << ism_ip << ":5300, rx :5301\n";
    } else {
        can_sock = std::make_unique<CanBus>(can_if);
        if (!can_sock->open()) {
            std::string msg = "[FCC] ERROR: Cannot open CAN bus: " + can_sock->lastError();
            std::cerr << msg << "\n";
            g_log.push(msg);
            std::cerr << "[FCC] Hint:  sudo ip link set " << can_if
                      << " type can bitrate 500000 && sudo ip link set "
                      << can_if << " up\n";
            return 1;
        }
        can_ptr = can_sock.get();
        std::cout << "[FCC] CAN bus open on " << can_if << "\n";
    }
    ICanBus& can = *can_ptr;

    // ── Open GCS link ─────────────────────────────────────────────────────
    GcsLink gcs(gcs_ip, gcs_port, DEFAULT_LISTEN_PORT);
    if (!gcs.open()) {
        std::string msg = "[FCC] ERROR: Cannot open GCS link: " + gcs.lastError();
        std::cerr << msg << "\n";
        g_log.push(msg);
        return 1;
    }
    std::cout << "[FCC] GCS UDP link open → " << gcs_ip << ":" << gcs_port << "\n";

    // ── Start FCC controller ──────────────────────────────────────────────
    FccController fcc(can, gcs);
    fcc.start();

    // ── Start HTTP API server ─────────────────────────────────────────────
    HttpServer http(fcc);
    if (!http.start()) {
        std::string msg = "[FCC] WARNING: HTTP server failed: " + http.lastError();
        std::cerr << msg << "\n";
        g_log.push(msg);
    }

    // ── Start Ethernet TCP server (RDS WebApp ↔ FCC) ──────────────────────
    EthernetServer eth(fcc, EthernetServer::DEFAULT_PORT);
    if (!eth.start()) {
        std::string msg = "[FCC] WARNING: Ethernet server failed: " + eth.lastError();
        std::cerr << msg << "\n";
        g_log.push(msg);
    } else {
        std::cout << "[FCC] Ethernet TCP server on port " << EthernetServer::DEFAULT_PORT << "\n";
    }
    // ── USART init ──────────────────────19.7.26 Tova
    // 2026-09-03: every command sender in FccController (sendStandby/sendArm/
    // sendDisarm/sendFireRequest/sendRecoveryReset/heartbeatLoop) checks
    // `if (uart_fd > 0)` FIRST and only falls through to the CAN/ETH transport
    // chosen on the command line if that check fails — this is intentional
    // and correct on the original Jetson (real UART wired straight to that
    // ISM, carries the safety-critical HB/ARM/DISARM/FIRE protocol regardless
    // of what CAN/ETH carries). But it means /dev/ttyTHS1 merely EXISTING and
    // opening successfully silently overrides an explicit `can0`/`eth:`
    // choice on ANY OTHER Jetson too — confirmed: a second Jetson wired to a
    // different ISM over CAN kept showing COMM_LOSS because its own
    // /dev/ttyTHS1 also opens fine (same SoC/carrier board family) but isn't
    // wired to anything, so heartbeats were silently going out over UART into
    // nothing instead of the CAN bus that was actually explicitly requested.
    // FCC_NO_UART=1 is a new, purely additive opt-out — unset by default, so
    // every existing deployment (including the original UART-wired Jetson)
    // behaves identically to before; only a machine that explicitly sets this
    // env var skips the UART open entirely and lets the CLI-selected
    // transport (can0/eth:) actually be used for every command.
    bool skip_uart = std::getenv("FCC_NO_UART") != nullptr;
    if (skip_uart) {
        std::cout << "[FCC] FCC_NO_UART set — skipping UART, using CLI-selected transport only.\n";
        uart_fd = -1;
    } else {
        uart_fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_SYNC);
        if (uart_fd < 0) {
            std::cerr << "Error opening UART port! Try running with sudo.\n";
           // return -1;
        }

        if (configure_uart(uart_fd) < 0) {
            std::cerr << "Error configuring UART port!\n";
            close(uart_fd);
            //return -1;
        }else std::cout << "\nSucess on configuring UART port!\n";
    }

    // ── Main loop — print status every 2 seconds ──────────────────────────
    uint64_t statusCount = 0;
    FccState   prevS{};
    CameraStatus prevCam{};
    GimbalStatus prevGim{};
    bool firstPrint = true;

    while (!g_shutdown) {
        std::this_thread::sleep_for(2s);
        FccState     s   = fcc.getState();
        CameraStatus cam = fcc.getCameraStatus();
        GimbalStatus gim = fcc.getGimbalStatus();
        ++statusCount;

        bool changed = firstPrint
            || s.ism_state       != prevS.ism_state
            || s.torque_enabled  != prevS.torque_enabled
            || s.motor_moving    != prevS.motor_moving
            || s.azimuth_deg     != prevS.azimuth_deg
            || s.elevation_deg   != prevS.elevation_deg
            || s.motor_rate_dps  != prevS.motor_rate_dps
            || cam.streaming     != prevCam.streaming
            //|| cam.frame_count   != prevCam.frame_count
            || gim.az_deg        != prevGim.az_deg
            || gim.el_deg        != prevGim.el_deg
            ||true;

        const char* ending = changed ? "\n" : "\r";

        std::ostringstream statusLine;
        statusLine << "[FCC STATUS] " << statusCount << " ISM=" << ismStateStr(s.ism_state)
            << " torque=" << (s.torque_enabled ? "ON" : "OFF")
            << " az=" << std::fixed << std::setprecision(1) << s.azimuth_deg << "°"
            << " el=" << s.elevation_deg << "°"
            << " rate=" << s.motor_rate_dps << "dps"
            << (s.motor_moving ? " [MOVING]" : " [STOP]")
            << " | CAM=" << (cam.streaming ? "ON f=" + std::to_string(cam.frame_count) : "OFF")
            << " GIM=az" << std::setprecision(1) << gim.az_deg << "°"
            << " el" << gim.el_deg << "°"
            << " hb_seq=" << s.heartbeat_seq;

        g_log.setLastStatus(statusLine.str());
        g_log.push(statusLine.str());
        
        // חוסם את חוט המצלמה מלהתערבב באמצע ההדפסה הזו
        {
            std::lock_guard<std::mutex> lock(g_console_mutex);
            if (!changed)
            std::cout << "\n"           // 1. פותח שורה חדשה בתחתית עבור הסטטוס
                << "\33[2K"       // 2. מוחק את שורת הסטטוס הקודמת לחלוטין
                << statusLine.str() // 3. מדפיס את הסטטוס המעודכן
                << "\33[1F"       // 4. קוד ANSI: מחזיר את הסמן שורה אחת למעלה!
                << std::flush;
            else // הדפסת הסטטוס
                std::cout << statusLine.str() << ending << std::flush;
            
        }
        prevS   = s;
        prevCam = cam;
        prevGim = gim;
        firstPrint = false;

    }

    // ── Shutdown ──────────────────────────────────────────────────────────
    std::cout << "\n[FCC] Shutting down...\n";
    eth.stop();
    std::cout << "\n[FCC] Ethernet shutting down...\n";
    http.stop();
    std::cout << "\n[FCC] HTTP shutting down...\n";
    fcc.stop();
    if (can_sock) can_sock->close();
    if (can_eth)  can_eth->close();
    gcs.close();
    std::cout << "[FCC] Goodbye.\n";
    return 0;
}
