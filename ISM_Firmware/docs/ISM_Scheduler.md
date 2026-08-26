# ISM Bare-Metal Scheduler Design
**Drone Remote Weapon Station — Aleppo Systems / RDS**  
MIL-STD-882E | Hardware: STM32H7

---

## 1. Philosophy

The ISM runs **no operating system**. There is no kernel, no memory manager,  
no heap allocator, and no task switcher. Instead, a **fixed-cycle bare-metal  
scheduler** drives all ISM functions from a hardware timer interrupt.

This is intentional and mandatory:

| Property | Bare-Metal Scheduler | RTOS |
|----------|---------------------|------|
| Worst-case execution time | Fully deterministic | Depends on scheduler + ISR latency |
| Attack surface | Zero (no kernel) | Kernel + drivers |
| Certifiability (MIL-STD-882E High) | Straightforward | Requires OS qualification |
| Stack overflow risk | Statically bounded | Per-task, harder to bound |
| Interrupt latency | Hardware-only | Kernel adds overhead |
| Heap usage | **None — zero dynamic allocation** | Typically present |

> The ISM never calls `malloc`, `new`, or any dynamic allocator.  
> All buffers, queues, and state are statically allocated at compile time.

---

## 2. Hardware Timer — The Scheduler Clock

**Timer:** TIM6 (basic timer, dedicated — not shared with any other peripheral)  
**Tick period:** 1 ms (1 kHz)  
**Interrupt priority:** Highest (NVIC priority 0)

TIM6 fires every 1 ms and calls the ISM tick dispatcher. Everything the ISM  
does is anchored to this tick.

```
TIM6 IRQ (1 ms)
    │
    └──► ISM_SchedulerTick()   ← single entry point, runs to completion
```

The tick handler always runs to completion within the 1 ms window.  
If it ever overruns, the hardware watchdog (IWDG) resets the MCU → SAFE.

---

## 3. Task Priority Table

Tasks execute in fixed priority order within each 1 ms tick.  
Higher priority = executed first = shorter response latency.

| Priority | Task | Period | Max Duration | Description |
|----------|------|--------|-------------|-------------|
| **1** | Hardware watchdog kick | Every tick (1 ms) | < 1 µs | Kick IWDG — if this is ever skipped the MCU resets to SAFE |
| **2** | Torque disable GPIO assert | Every tick (1 ms) | < 1 µs | Re-assert the torque-disable line based on current state — cannot be overridden by any lower-priority task |
| **3** | CAN receive | Every tick (1 ms) | < 50 µs | Read all pending CAN frames from FDCAN FIFO, validate, pass to state machine |
| **4** | ISM state machine tick | Every tick (1 ms) | < 100 µs | Process validated messages, run watchdog timer, evaluate state transitions |
| **5** | Fault log write | On event only | < 20 µs | Write timestamped fault entry to static ring buffer |
| **6** | CAN transmit — state ack | On state change | < 20 µs | Send State Ack / Fault Report frame to FCC |
| **7** | Heartbeat ack transmit | Every 100 ms | < 10 µs | Echo FCC heartbeat seq back to FCC with current ISM state |

---

## 4. Scheduler Tick — Pseudocode

```c
// Called by TIM6 IRQ every 1 ms. Runs to completion. No blocking, no waiting.
void ISM_SchedulerTick(void) {

    // ── Priority 1: Hardware watchdog ────────────────────────────────────
    IWDG_Refresh();   // must always execute — if skipped → MCU reset → SAFE

    // ── Priority 2: Torque disable output ────────────────────────────────
    // Re-assert every tick regardless of other logic.
    // This means even a single bad tick cannot leave torque accidentally enabled.
    bool torque_ok = (ism_state == ARMED || ism_state == FIRING);
    GPIO_Write(TORQUE_ENABLE_PIN, torque_ok ? HIGH : LOW);

    // ── Priority 3: CAN receive ───────────────────────────────────────────
    CAN_Frame frame;
    while (FDCAN_Read(&frame)) {          // drain FIFO — non-blocking
        ISM_ValidateAndEnqueue(&frame);   // validate CRC + seq, push to static queue
    }

    // ── Priority 4: ISM state machine ────────────────────────────────────
    ISM_Tick();   // process queued messages, run watchdog timer, transition states

    // ── Priority 5–7: Events and periodic transmit ────────────────────────
    if (fault_pending) {
        ISM_LogFault();
        ISM_TransmitFaultReport();
        fault_pending = false;
    }

    if (state_changed) {
        ISM_TransmitStateAck();
        state_changed = false;
    }

    if (tick_counter % 100 == 0) {   // every 100 ms
        ISM_TransmitHeartbeatAck();
    }

    tick_counter++;
}
```

---

## 5. Watchdog Architecture — Two Layers

The ISM uses **two independent watchdog layers** for defense in depth:

```
Layer 1 — Software watchdog (Watchdog.h, in ISM state machine)
    Monitors: FCC heartbeat arrival
    Timeout:  500 ms
    Action:   ISM state → FAULT, torque disabled via Priority-2 task

Layer 2 — Hardware watchdog (IWDG, independent RC oscillator)
    Monitors: ISM_SchedulerTick() actually executing
    Timeout:  ~26 ms (IWDG prescaler configured at boot)
    Action:   Full MCU reset → power-up → ISM enters SAFE (SWR-SAFE-001)
```

Layer 2 catches cases that Layer 1 cannot — infinite loop, hard fault exception,  
stack overflow, or a bug that prevents the tick from running entirely.

```
Normal operation:
  TIM6 → Tick() → IWDG_Refresh() → ... repeats every 1 ms ✓

Scheduler hangs (bug / hard fault):
  TIM6 stops firing (or Tick() loops forever)
  IWDG not refreshed within 26 ms
  IWDG resets MCU
  ISM boots → SAFE → torque disable line asserted within 50 ms ✓
```

---

## 6. Memory Model — No Dynamic Allocation

All ISM data structures are statically sized and allocated at compile time:

```c
// Static ring buffer for fault log — no malloc
#define ISM_LOG_SIZE 200
static IsmLog log_buffer[ISM_LOG_SIZE];
static uint8_t log_head = 0;

// Static CAN receive queue — sized to drain in one tick
#define CAN_QUEUE_SIZE 16
static CAN_Frame can_queue[CAN_QUEUE_SIZE];
static uint8_t can_queue_head = 0, can_queue_tail = 0;

// ISM state — single instance, statically allocated
static ISM_State ism;
```

Stack usage is bounded and verified at link time using the linker map.  
The build will fail if the stack exceeds the MCU's TCM (Tightly Coupled Memory) allocation.

---

## 7. Interrupt Priority Map (STM32H7 NVIC)

| Priority | Interrupt | Reason |
|----------|-----------|--------|
| 0 (highest) | TIM6 — scheduler tick | ISM must never be preempted by lower-priority IRQs |
| 1 | FDCAN1 FIFO full warning | Alert before CAN messages are lost |
| 2 | Hard fault / bus fault | Force safe state on exception |
| 3 | FDCAN1 receive | Secondary CAN notification |
| 15 (lowest) | SysTick | Used only for delay_us, not scheduler |

> TIM6 at priority 0 means the ISM tick **cannot be preempted** by any other  
> interrupt except NMI (Non-Maskable Interrupt) and HardFault.

---

## 8. Timing Budget per 1 ms Tick

```
1000 µs available per tick
├──   1 µs  — IWDG kick
├──   1 µs  — GPIO torque assert
├──  50 µs  — CAN FIFO drain (worst case: 16 frames × ~3 µs each)
├── 100 µs  — ISM state machine tick
├──  20 µs  — fault log write (if event pending)
├──  20 µs  — CAN state ack transmit (if state changed)
├──  10 µs  — heartbeat ack (if 100 ms boundary)
└── 798 µs  — margin (79.8% headroom)
```

The 79.8% margin means the tick loop completes well within 1 ms under  
all modelled conditions. If measured execution ever exceeds 800 µs in testing,  
investigate immediately — the IWDG timeout of 26 ms provides a safety net  
but timing margin erosion is a warning sign.

---

## 9. Boot Sequence

```
Power applied (T=0)
    │
    T+0 ms   — MCU clock init, flash wait states configured
    T+2 ms   — IWDG started (26 ms timeout armed immediately)
    T+5 ms   — GPIO torque-disable line driven LOW (safe by default)
    T+10 ms  — FDCAN1 peripheral init, filter configured
    T+15 ms  — TIM6 started → scheduler begins ticking
    T+15 ms  — ISM state machine init → state = SAFE
    T+50 ms  — ISM fully operational, waiting for FCC heartbeat on CAN
```

> The torque-disable line is driven LOW at **T+5 ms** — before the CAN  
> peripheral or state machine is even initialised. The system is safe  
> from the moment power is stable.

---

## 10. References
- STMicroelectronics — *STM32H7 Reference Manual (RM0433)*, TIM6, FDCAN, IWDG chapters
- MIL-STD-882E — *Standard Practice for System Safety*
- IEC 61508-3 — *Software requirements for safety-related systems*
- *Making Embedded Systems* — Elecia White (bare-metal scheduler patterns)
