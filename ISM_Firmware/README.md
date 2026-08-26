# ISM Firmware & Fire Interlock Logic

**Drone Remote Weapon Station — Independent Safety Monitor**  
Aleppo Systems / RDS · MIL-STD-882E compliant stub

---

## Build & Run (WSL / Linux)

```bash
cd ISM_Firmware
mkdir build && cd build
cmake ..
make -j$(nproc)
./fault_injection_tests
```

---

## Project Structure

```
ISM_Firmware/
├── include/
│   ├── SystemState.h      # SystemState enum (SAFE/STANDBY/ARMED/FIRING/FAULT)
│   ├── Watchdog.h         # Software watchdog timer
│   ├── FireInterlock.h    # Dual-channel fire interlock
│   ├── FccMessage.h       # FCC→ISM message types
│   ├── ISM.h              # Independent Safety Monitor API
│   └── FccStub.h          # Simulated FCC (in-process transport)
├── src/
│   ├── ISM.cpp            # ISM state machine + validation logic
│   └── FccStub.cpp        # FCC stub implementation
├── tests/
│   └── FaultInjectionTests.cpp   # SWR-SAFE-001..008 test harness
└── CMakeLists.txt
```

---

## Requirements Coverage

| Req. ID      | Description                                      | Tested By                          |
|--------------|--------------------------------------------------|------------------------------------|
| SWR-SAFE-001 | SAFE on power-up, watchdog, comm loss            | `test_SAFE001_*`                   |
| SWR-SAFE-002 | FCC cannot energize without ISM gating           | `test_SAFE002_TorqueGating`        |
| SWR-SAFE-003 | Validate range, rate, direction                  | `test_SAFE003_*`                   |
| SWR-SAFE-004 | Dual-channel interlock for fire                  | `test_SAFE004_*`                   |
| SWR-SAFE-005 | Safety-critical SW independent of non-safety     | `test_SAFE005_Independence`        |
| SWR-SAFE-006 | Fault logging with time, state, source           | `test_SAFE006_FaultLogging`        |
| SWR-SAFE-007 | Update/config integrity (sequence check)         | `test_SAFE007_UpdateIntegrity`     |
| SWR-SAFE-008 | Controlled recovery from FAULT                   | `test_SAFE008_RecoveryRequiresClearance` |
