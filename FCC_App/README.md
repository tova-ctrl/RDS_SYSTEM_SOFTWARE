# FCC Application — NVIDIA Jetson

**Fire Control Computer — Aleppo Systems / RDS**  
C++17 | SocketCAN | UDP/JSON GCS link

---

## Build (Jetson / Linux)

```bash
cd FCC_App
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Setup CAN Interface

```bash
sudo ip link set can0 type can bitrate 1000000
sudo ip link set can0 up
ip link show can0   # verify UP state
```

## Run

```bash
./fcc_app can0 192.168.1.100 5200
# arg1: CAN interface (default: can0)
# arg2: GCS IP        (default: 127.0.0.1)
# arg3: GCS port      (default: 5200)
```

---

## Architecture

```
main.cpp
  └── FccController
        ├── heartbeatLoop()    → CAN HEARTBEAT → ISM every 100ms
        ├── canReceiveLoop()   → reads STATE_ACK / FAULT_REPORT from ISM
        └── gcsPollLoop()      → reads commands from GCS over UDP
              ↕
        CanBus (SocketCAN)     → can0 on Jetson carrier board
        GcsLink (UDP)          → JSON to/from GCS / Blazor WebApp
```

## Project Structure

```
FCC_App/
├── include/
│   ├── CanBus.h          # SocketCAN wrapper
│   ├── IsmProtocol.h     # CAN IDs, frame builders, parsers
│   ├── GcsLink.h         # UDP JSON link to GCS
│   └── FccController.h   # Main FCC logic
├── src/
│   ├── CanBus.cpp
│   ├── GcsLink.cpp
│   ├── FccController.cpp
│   └── main.cpp
└── CMakeLists.txt
```

## GCS Command Format (UDP JSON)

Send to FCC listen port (5201):
```json
{"cmd":"arm"}
{"cmd":"disarm"}
{"cmd":"fire"}
{"cmd":"recovery"}
```

## FCC → GCS State Update Format

Received on GCS port (5200):
```json
{"type":"state","ism_state":2,"torque_enabled":true,"azimuth_deg":15.50,"elevation_deg":5.00,"timestamp":"14:32:01"}
{"type":"fault","fault_code":1,"ism_state":4,"message":"ISM fault reported"}
```
