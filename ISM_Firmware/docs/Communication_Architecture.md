# ISM Communication Architecture
**Drone Remote Weapon Station — Aleppo Systems / RDS**  
MIL-STD-882E | Document ref: Software_general_architecture

---

## 1. Overview

The system spans two physical locations:

- **Weapon Station (on the drone/platform):** FCC + ISM + Actuators — co-located, short-distance wiring
- **Ground Control Station (GCS) / Operator:** GUI / HMI — potentially kilometers away

```
  GROUND CONTROL STATION                         WEAPON STATION (on platform)
  ─────────────────────────                      ──────────────────────────────────────────
                                                                         CAN bus
  ┌──────────────┐              RF / Encrypted  ┌──────────┐  ◄──────────────────────►  ┌──────────┐
  │  GUI / HMI   │ ◄──────────────────────────► │   FCC    │                             │   ISM    │
  │  Operator    │         Ethernet/UDP          │(Medium)  │  ◄──────────────────────►  │  (High)  │
  │  Panel (Low) │                               └──────────┘       CAN bus              └──────────┘
  └──────────────┘                                                                             │
                                                                                          CAN bus
                                                                                             │
                                                                                    ┌────────▼────────┐
                                                                                    │    Actuators    │
                                                                                    │  Torque / Fire  │
                                                                                    │  Hardware I/O   │
                                                                                    └─────────────────┘
```

> **Key invariants:**
> - CAN bus is the **on-platform safety bus**: FCC ↔ ISM commands, and ISM ↔ Actuators torque/fire gating.
> - The GUI is **remote and non-safety (Low integrity)**. It communicates with the FCC only over an encrypted RF/network link. It never reaches the ISM directly.
> - The ISM enforces SAFE state independently of FCC, GUI, and network link status (SWR-SAFE-001, SWR-SAFE-005).

---

## 2. FCC ↔ ISM Communication (on-platform)

### Transport
| Channel | Protocol | Purpose |
|---------|----------|---------|
| Primary | **CAN bus** (1 Mbit/s) | All command, status, and heartbeat frames |
| Backup  | **Dedicated hardware line** | Hard torque-disable signal — asserted by ISM regardless of CAN state |

CAN is the appropriate choice for co-located embedded systems: deterministic timing,  
hardware error detection, multi-master, tolerates single wire faults.  
Specified in the architecture document (§17 — "CAN motion and fault frames").

### Message Table — FCC → ISM

| Message | CAN ID | Rate | Fields |
|---------|--------|------|--------|
| Heartbeat | `0x100` | 10 Hz | `seq` (uint32), `crc` (uint16) |
| ARM | `0x101` | On demand | `seq`, `authority_token` (8 bytes) |
| DISARM | `0x102` | On demand | `seq`, `source` |
| Motion Command | `0x110` | On demand | `seq`, `azimuth_deg` (float), `elevation_deg` (float), `rate_dps` (float), `direction` (uint8) |
| Fire Request | `0x120` | On demand | `seq`, `channel_a_token`, `channel_b_token` |
| Recovery Reset | `0x130` | On demand | `seq`, `authority_id` (8 bytes), `fault_clearance_token` |

### Message Table — ISM → FCC

| Message | CAN ID | Rate | Fields |
|---------|--------|------|--------|
| State Ack | `0x200` | On every transition | `new_state` (uint8), `rejection_reason` (uint8) |
| Fault Report | `0x210` | On fault | `fault_code` (uint16), `state` (uint8), `timestamp` (uint32) |
| Heartbeat Ack | `0x211` | 10 Hz | `seq_echo`, `ism_state` (uint8) |

### Timing & Safety Rules
- ISM expects a Heartbeat from FCC every **100 ms**.
- No Heartbeat within **500 ms** → ISM enters FAULT, torque disabled (SWR-SAFE-001).
- ISM validates sequence numbers on every non-heartbeat frame — gap or replay → FAULT (SWR-SAFE-007).
- ISM **never waits** for FCC acknowledgment before enforcing SAFE or FAULT.

---

## 3. ISM ↔ Actuators (on-platform)

### Transport
| Channel | Protocol | Purpose |
|---------|----------|---------|
| Primary | **CAN bus** | Motion enable frames, fire enable frames |
| Safety override | **Hard-wired torque-disable line** | ISM pulls line LOW → actuators de-energise immediately, regardless of CAN |

This is the final safety barrier. Even if CAN is compromised or FCC crashes,  
the hardware torque-disable line ensures actuators cannot be driven (SWR-SAFE-002).

### Message Table — ISM → Actuators

| Message | CAN ID | Trigger | Fields |
|---------|--------|---------|--------|
| Torque Enable | `0x300` | On ARM | `enabled=1`, `ism_state` |
| Torque Disable | `0x300` | On SAFE/FAULT | `enabled=0`, `ism_state` — AND hardware line asserted LOW |
| Fire Enable | `0x310` | On dual-channel auth | `fire_permitted=1`, `seq` |
| Fire Inhibit | `0x310` | On SAFE/FAULT/interlock clear | `fire_permitted=0` |
| Limit Enforcement | `0x320` | On limit breach | `axis`, `limit_type`, `hard_stop=1` |

---

## 4. GUI ↔ FCC Communication (remote link)

### Transport
| Channel | Protocol | Purpose |
|---------|----------|---------|
| Primary | **Encrypted UDP / TCP over RF link** | Telemetry stream, state updates, operator commands |
| Encoding | **JSON** (demo) or **Protobuf** (production) | Structured messages, versioned |

The GUI is located at the Ground Control Station — potentially kilometers away.  
The link goes over an encrypted RF datalink (or LAN in bench testing).  
Specified in the architecture document (§17 — "GCS↔FCC Ethernet messages").

### Topology — GUI never reaches ISM

```
  GCS                               Weapon Station
  ───────────────────────────────   ────────────────────────────
  GUI  ──[ARM / operator cmd]──►   FCC  ──[validates + forwards]──►  ISM
  GUI  ◄──[state / telemetry]──    FCC  ◄──[state ack / fault]────   ISM
```

The FCC is the relay and trust boundary between the remote non-safety GUI  
and the safety-critical on-platform ISM. This preserves SWR-SAFE-005.

### Message Table — FCC → GUI

| Message | Rate | Fields |
|---------|------|--------|
| System State Update | On change | `state`, `torque_enabled`, `timestamp` |
| Telemetry Stream | 10 Hz | `azimuth_deg`, `elevation_deg`, `rate_dps`, `fcc_health`, `ism_health` |
| Fault / Event Log | On event | `timestamp`, `source`, `fault_code`, `message`, `mitigation` |
| Interlock Status | On change | `armed`, `channel_a`, `channel_b`, `fire_permitted` |
| Link Quality | 1 Hz | `rtt_ms`, `packet_loss_pct` |

### Message Table — GUI → FCC

| Message | Fields | Notes |
|---------|--------|-------|
| ARM Request | `authority_id`, `operator_pin` | FCC validates then forwards to ISM via CAN |
| DISARM Request | `authority_id` | Always forwarded; ISM always accepts |
| Fire Intent | `authority_id`, `target_confirm` | FCC checks interlock before forwarding |
| Recovery Reset | `authority_id`, `fault_clearance` | Dual GUI confirmation required (SWR-SAFE-008) |

### Remote Link Safety Rules
- **RF link loss** → FCC stops receiving heartbeats from GCS → FCC stops forwarding commands to ISM → ISM watchdog eventually fires → SAFE (SWR-SAFE-001).
- GUI **disconnect has zero effect** on ISM torque-disable or fire-inhibit state (SWR-SAFE-005).
- All RF traffic is **encrypted and authenticated** to prevent spoofing (SWR-SAFE-007, §15 Cybersecurity/RMF).
- GUI displays ISM state as relayed — it does **not** infer or simulate state locally.
- Fire requires **dual confirmation** in GUI before Fire Intent is sent (SWR-SAFE-004).

---

## 5. Full Message Flow — Nominal ARM + FIRE Sequence

```
  GUI              FCC              ISM            Actuators
   │                │                │                 │
   │──ARM Request──►│                │                 │
   │                │──CAN ARM──────►│                 │
   │                │                │ validates       │
   │                │◄──State Ack────│ (ARMED)         │
   │                │──CAN Torque───────────────────── ►│ enable
   │◄──State Update─│                │                 │
   │                │                │                 │
   │──Fire Intent──►│                │                 │
   │  (dual confirm)│──CAN Fire Req─►│                 │
   │                │                │ dual-channel    │
   │                │                │ interlock check │
   │                │                │──CAN Fire Ena──►│ fire
   │◄──State Update─│◄──State Ack────│ (FIRING)        │
```

---

## 6. Software Integrity Levels & Communication Role

| Software Element | Integrity Level | Communication Role |
|------------------|-----------------|--------------------|
| ISM Firmware | **High** | CAN bus (FCC↔ISM, ISM↔Actuators) + hardware torque line |
| Fire Interlock Logic | **High** | Internal to ISM; dual-channel tokens over CAN |
| FCC Control Software | **Medium** | CAN bridge on-platform; encrypted RF link to GCS |
| Operator GUI / HMI | **Low** | Encrypted RF/Ethernet to FCC only; no ISM access |

---

## 7. Current Codebase vs. Production Transport

| Layer | Current (stub) | Production upgrade |
|-------|---------------|--------------------|
| FCC ↔ ISM | In-process `FccStub` direct calls | SocketCAN (`PF_CAN`, `can0`) with `struct can_frame` |
| ISM ↔ Actuators | `isTorqueEnabled()` bool | CAN frame `0x300` + dedicated GPIO torque-disable line |
| FCC ↔ GUI | ASP.NET `SystemController` HTTP API | Encrypted UDP/TCP server; JSON or Protobuf; WebApp via SignalR |
| Watchdog backup | Software timer (`Watchdog.h`) | Hardware watchdog IC + dedicated torque-disable line |

---

## 8. References
- Aleppo Systems — *Software Architecture and Safety Classification (MIL-STD-882E)*
- Aleppo Systems — *Upgraded Dual-Channel + ISM Safety Architecture (Integrated)*
- MIL-STD-882E — *Standard Practice for System Safety*
