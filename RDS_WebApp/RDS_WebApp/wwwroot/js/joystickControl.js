// Reads the operator's controller and calls back into Home.razor for each
// mapped logical role (T1/T2/B2/B3/B4/B5/B6/S3/J1/J2 — see
// [[mobilicom-controller-mapping]] memory for the full role list). S3 (zoom),
// J1 (aim rate), T1 (camera lock), J2 (fine aim), T2 (fire system open/close),
// and B2 (fire) are actually wired to a Home.razor handler so far; B4/B5/B6
// have index slots reserved below but no dispatch logic yet (their backend
// commands aren't scoped — see memory).
//
// 2026-08-18: DEVICE PROFILES — added so the real Mobilicom hand controller
// can be supported WITHOUT touching the Logitech Dual Action mapping that's
// already proven working on real hardware (explicit user requirement: don't
// risk regressing the working version). Every role is looked up through
// `activeProfile`, keyed by role name — swapping the active profile is the
// only thing that changes when a different controller is connected; the
// edge-detection/dispatch code itself never changes.
//
// 2026-08-18, same day, after real-hardware calibration: the Mobilicom
// reports its digital roles very differently from the Logitech. S3 (zoom)
// comes through as an AXIS, not the Logitech's dedicated buttons — confirmed
// 2026-08-19 to still be a genuine continuous analog range on J1/J2
// themselves (the "D-PAD as AXIS" calibration notes only meant the
// calibration pass tested the 4 extremes, not that the hardware is
// digital-only). So a plain button-index number isn't enough to describe
// every control anymore — each digital role is now a small descriptor: either
// `{type:'button', index}` (a real button, .pressed is authoritative) or
// `{type:'axis-digital', index, direction}` (a digital control multiplexed
// onto an axis — "pressed" means that axis has crossed AXIS_DIGITAL_THRESHOLD
// in `direction`'s sign). J1/J2's continuous axes also gained a per-axis
// `invert` flag: the Logitech's Y axes report up=negative (W3C standard,
// hence the old unconditional `-y`), but the Mobilicom's calibration showed
// up=positive already on both its J1/J2 vertical axes — negating those would
// invert the real controller's up/down. `invert` makes that a per-device
// property instead of a hardcoded assumption in the dispatch code.
const AXIS_DIGITAL_THRESHOLD = 0.5;

function isControlActive(gp, control) {
    if (!control) return false;
    if (control.type === 'axis-digital') {
        const v = gp.axes[control.index];
        if (v === undefined) return false;
        return control.direction > 0 ? v >= AXIS_DIGITAL_THRESHOLD : v <= -AXIS_DIGITAL_THRESHOLD;
    }
    const b = gp.buttons[control.index];
    return !!(b && b.pressed);
}

// Returns the axis value in "logical" convention (positive = right / positive
// = up), applying the profile's own `invert` flag — or null if this role has
// no axis assigned yet (Mobilicom placeholders) or the gamepad doesn't
// report that many axes.
function readAxis(gp, axisSpec) {
    if (!axisSpec || axisSpec.index == null || gp.axes.length <= axisSpec.index) return null;
    const raw = gp.axes[axisSpec.index];
    return axisSpec.invert ? -raw : raw;
}

const LOGITECH_PROFILE = {
    name: 'LOGITECH_DUAL_ACTION',
    // Matches the `id` string Chrome reports for this exact gamepad
    // (vendor 046d, product c216) — confirmed empirically 2026-08-05, see memory.
    idMatch: /046d|c216|dual action/i,
    buttons: {
        S3_UP:   { type: 'button', index: 12 },
        S3_DOWN: { type: 'button', index: 13 },
        T1:      { type: 'button', index: 4 },
        T2:      { type: 'button', index: 5 },
        B2_FIRE: { type: 'button', index: 7 },
        B3:      { type: 'button', index: 3 },
        B4:      { type: 'button', index: 2 },
        B5:      { type: 'button', index: 8 },
        B6:      { type: 'button', index: 9 },
    },
    axes: {
        J1_X: { index: 0, invert: false },
        J1_Y: { index: 1, invert: true },
        J2_X: { index: 2, invert: false },
        J2_Y: { index: 3, invert: true },
    },
};

// Filled in from the user's own real-hardware calibration passes
// (2026-08-18/19) — every role below is a confirmed real index, not a guess.
const MOBILICOM_PROFILE = {
    name: 'MOBILICOM',
    // 2026-08-18: the Mobilicom hand controller's USB HID reports as its
    // underlying component manufacturer, not the outer "Mobilicom" branding —
    // confirmed via calibration: `gp.id` = "APEM USB KIT (Vendor: 068e
    // Product: 0144)". APEM makes industrial/defense joystick & HMI modules;
    // the hand controller is evidently built around one of their off-the-
    // shelf USB kits. Matching on vendor/product hex (068e/0144) as the
    // primary signal, same pattern as LOGITECH_PROFILE above, "apem" as a
    // human-readable fallback in case the hex ever shows up differently.
    idMatch: /068e|0144|apem/i,
    buttons: {
        S3_UP:      { type: 'axis-digital', index: 4, direction: +1 },
        S3_DOWN: { type: 'axis-digital', index: 4, direction: -1 },
        S4_UP: { type: 'axis-digital', index: 5, direction: +1 },
        S4_DOWN: { type: 'axis-digital', index: 5, direction: -1 },
        // T1 is System_Power Enable - its role is to open/close all switches and buttons,
        T1:         { type: 'button', index: 7, mode: 'maintained' },
        T2: { type: 'button', index: 15, mode: 'maintained' },
        B0_CAM:     { type: 'button', index: 0 },
        B14_FIRE:    { type: 'button', index: 14 },
        B2_STDBY:   { type: 'button', index: 2 },
        B3_ARM:     { type: 'button', index: 3 },
        B4:         { type: 'button', index: 1 },
        B5:         { type: 'button', index: 2 },
        B6_LOCK:    { type: 'button', index: 6 },      
    },
    axes: {
        J1_X: { index: 3, invert: false },
        J1_Y: { index: 2, invert: false },
        J2_X: { index: 0, invert: false },
        J2_Y: { index: 1, invert: false },
    },
};

const PROFILES = [MOBILICOM_PROFILE, LOGITECH_PROFILE]; // checked in order; LOGITECH_PROFILE is also the fallback default

let _activeProfile = LOGITECH_PROFILE;
let _loggedProfileForId = null; // avoid re-logging every frame for the same connected gamepad

function selectProfile(gp) {
    for (const profile of PROFILES) {
        if (profile.idMatch.test(gp.id)) return profile;
    }
    return LOGITECH_PROFILE; // unrecognized device — default to the proven mapping rather than guessing
}

let _dotNetRef = null;
let _rafId = null;
let _prevDpadUp = false;
let _prevDpadDown = false;
let _prevCam = false;
let _prevB_Lock = false;
let _prevb2Stdby = false;
let _prevb3Arm = false;
let _prevb14Fire = false;
// J1 — continuous gimbal pan/tilt rate (SetRateCommandAngularVelocities_X/Y,
// °/sec). Unlike S3's edge-triggered buttons, this is an analog value that
// needs repeated updates while held, but sending on every
// requestAnimationFrame tick (~60Hz) would flood both the SignalR circuit
// and the native calls. Throttled: send when the value changes meaningfully,
// or at least every RATE_SEND_INTERVAL_MS as a heartbeat (also covers the
// "stick released back to center" case — always send the final zero so the
// gimbal doesn't coast on a stale rate command).
//
const RATE_DEADZONE = 0.08;         // ignore drift/noise near stick center
const RATE_MAX_DEG_PER_SEC = 15;    // conservative default — this drives a weapon mount; raise only after confirming on real hardware
const RATE_SEND_INTERVAL_MS = 100;  // ~10Hz heartbeat ceiling
const RATE_CHANGE_THRESHOLD = 0.3;  // °/sec — send early if it moved more than this since the last send
let _lastRateSendTime = 0;
let _lastSentRateAz = 0;
let _lastSentRateEl = 0;

// J2 — fine-tune aim after lock (2026-08-11: same underlying camera command
// as J1 — StkDirectCameraService.SetRateX/Y — user confirmed this is about
// the camera, not the weapon mount). Same mechanism as J1, just a much
// smaller max rate for precise adjustment; both ultimately call the same
// Home.razor OnJoystickRate handler, so this is a coarse/fine choice between
// the two sticks, not a combined/summed input — meant to be used one at a
// time, matching "J1 to get on target, J2 to fine-tune" operator intent.
const RATE_FINE_MAX_DEG_PER_SEC = 3;
let _lastFineRateSendTime = 0;
let _lastSentFineRateAz = 0;
let _lastSentFineRateEl = 0;

window.startJoystickControl = function (dotNetRef) {
    window.stopJoystickControl();
    _dotNetRef = dotNetRef;
    _prevDpadUp = false;
    _prevDpadDown = false;
    _prevB_Lock = false;
    _prevb2Stdby = false;
    _prevb3Arm = false;
    _prevb14Fire = false;
    _prevCam = false;
    _lastRateSendTime = 0;
    _lastSentRateAz = 0;
    _lastSentRateEl = 0;
    _lastFineRateSendTime = 0;
    _lastSentFineRateAz = 0;
    _lastSentFineRateEl = 0;
    _activeProfile = LOGITECH_PROFILE;
    _loggedProfileForId = null;
    tick();
};

/* T1 — camera lock. Logitech's LB (btn.T1 set): single momentary
// button, edge-triggered software toggle (OnJoystickLock decides
// Engage/Disengage via liveTracking.IsActive) — unchanged.
//
// Mobilicom's T1 (btn.T1_LOCK set instead): a maintained switch keyed
// off #23 alone (backward=locked; forward/center=unlocked, confirmed
// by the user 2026-08-20 — see the long comment on T1_LOCK's
// descriptor above for how this superseded two earlier same-day
// designs) — same maintained-switch pattern as T2 below.*/
function tick() {
    const pads = navigator.getGamepads ? navigator.getGamepads() : [];
    let gp = null;
    for (let i = 0; i < pads.length; i++) {
        if (pads[i]) { gp = pads[i]; break; }
    }
  
    if (gp && _dotNetRef) {
        if (_loggedProfileForId !== gp.id) {
            _activeProfile = selectProfile(gp);
            _loggedProfileForId = gp.id;
            console.log(`[joystickControl] gamepad "${gp.id}" -> profile ${_activeProfile.name}`);
        }
        const btn = _activeProfile.buttons;
        const axis = _activeProfile.axes;
        //T1_POWER is System_Power Enable - its role is to open / close all switches and buttons
        let T1_POWER = isControlActive(gp, btn.T1); 
        if (T1_POWER) {
            // S3 — zoom — Up/Down = zoom in/out. Edge-triggered (only fires on
            // press/release), matching the same hold-to-zoom semantics as the
            // existing on-screen buttons in SystemConfig.razor (@onmousedown/
            // @onmouseup -> Zoomin/Zoomout/Nochange).
            const up = isControlActive(gp, btn.S4_UP) || isControlActive(gp, btn.S3_UP);
            const down = isControlActive(gp, btn.S4_DOWN) || isControlActive(gp, btn.S3_DOWN);

            if (up && !_prevDpadUp) {
                _dotNetRef.invokeMethodAsync('OnJoystickZoomIn');
            } else if (!up && _prevDpadUp) {
                _dotNetRef.invokeMethodAsync('OnJoystickZoomStop');
            }

            if (down && !_prevDpadDown) {
                _dotNetRef.invokeMethodAsync('OnJoystickZoomOut');
            } else if (!down && _prevDpadDown) {
                _dotNetRef.invokeMethodAsync('OnJoystickZoomStop');
            }

            _prevDpadUp = up;
            _prevDpadDown = down;
            // ===  LOCK On Target Button 6 
            const B_Lock = isControlActive(gp, btn.B6_LOCK);
            if ((B_Lock && !_prevB_Lock) ){
                _dotNetRef.invokeMethodAsync('OnJoystickLock');
            }
            _prevB_Lock = B_Lock;
            

            // B0 — activate thermal camera. Edge-triggered, one-shot — an
            // explicit switch TO thermal (not a toggle), matching the SDK's own
            // Camera enum (TI=1). Reuses the same SetPrimaryVideoChannel command
            // as the existing manual "TOGGLE ACTIVE CAMERA" button — see
            // Home.razor's OnJoystickThermalActivate.
            const camToggle = isControlActive(gp, btn.B0_CAM);
            if (camToggle && !_prevCam) {
                _dotNetRef.invokeMethodAsync('ToggleActiveCamera');
            }
            _prevCam = camToggle;
            //STAND BY
            const b2Stdby = isControlActive(gp, btn.B2_STDBY);
            if (b2Stdby && !_prevb2Stdby) {
                _dotNetRef.invokeMethodAsync('onJoystickStndBy');
            }
            _prevb2Stdby = b2Stdby;

            //ARM
            const b3Arm = isControlActive(gp, btn.B3_ARM);
            if (b3Arm && !_prevb3Arm) {
                _dotNetRef.invokeMethodAsync('onJoystickArm');
            }
            _prevb3Arm = b3Arm;
            // B14 — fire. Edge-triggered, one-shot on press only — matches the
            // existing on-screen FIRE button (Home.razor's @onclick="Fire"),
            // which is a single discrete action, not something to repeat-fire
            // while held.
            const b14Fire = isControlActive(gp, btn.B14_FIRE);
            if (b14Fire && !_prevb14Fire) {
                const B_Lock = isControlActive(gp, btn.B6_LOCK);
             //   if (B_Lock)
                    _dotNetRef.invokeMethodAsync('OnJoystickFire');
            }
            _prevb14Fire = b14Fire;
            
            // B4/B5/B6 — index slots reserved in each profile above, but no
            // dispatch here yet: their backend commands aren't scoped (laser
            // rangefinder / weapon safety catch / engine safety catch — see
            // [[mobilicom-controller-mapping]] memory). Add an edge-triggered
            // block here, same pattern as B3 above, once each has a real
            // Home.razor handler to call.

            // J1 — logical convention: positive X = right, positive Y = up
            // (readAxis() already applies each profile's own invert flag so the
            // dispatch code below never needs to know which raw polarity a given
            // device actually reports).
            const j1x = readAxis(gp, axis.J1_X);
            const j1y = readAxis(gp, axis.J1_Y);
            if (j1x !== null && j1y !== null) {
                const x = Math.abs(j1x) < RATE_DEADZONE ? 0 : j1x;
                const y = Math.abs(j1y) < RATE_DEADZONE ? 0 : j1y;
                const rateAz = x * RATE_MAX_DEG_PER_SEC;
                const rateEl = y * RATE_MAX_DEG_PER_SEC;

                const now = performance.now();
                const changed =
                    Math.abs(rateAz - _lastSentRateAz) > RATE_CHANGE_THRESHOLD ||
                    Math.abs(rateEl - _lastSentRateEl) > RATE_CHANGE_THRESHOLD;
                const dueForHeartbeat = (now - _lastRateSendTime) >= RATE_SEND_INTERVAL_MS;
                // 2026-08-12: the heartbeat used to fire forever, even at rest — a
                // Wireshark capture (sticks confirmed untouched) showed a
                // continuous, uninterrupted flood of RateCommand UDP packets to
                // the camera's CONTROL port, implicated in wedging that port over
                // time (see [[mobilicom-controller-mapping]] memory). Once both
                // the current and last-sent rate are already (0,0), there's
                // nothing left to protect against a dropped packet for — stop
                // heartbeating; `changed` alone still catches the stick moving
                // away from center immediately.
                const atRest = rateAz === 0 && rateEl === 0 && _lastSentRateAz === 0 && _lastSentRateEl === 0;

                if (changed || (dueForHeartbeat && !atRest)) {
                    _dotNetRef.invokeMethodAsync('OnJoystickRate', rateAz, rateEl);
                    _lastSentRateAz = rateAz;
                    _lastSentRateEl = rateEl;
                    _lastRateSendTime = now;
                }
            }

            // J2 — same deadzone/throttle pattern as J1, just a finer max rate.
            const j2x = readAxis(gp, axis.J2_X);
            const j2y = readAxis(gp, axis.J2_Y);
            if (j2x !== null && j2y !== null) {
                const x = Math.abs(j2x) < RATE_DEADZONE ? 0 : j2x;
                const y = Math.abs(j2y) < RATE_DEADZONE ? 0 : j2y;
                const rateAz = x * RATE_FINE_MAX_DEG_PER_SEC;
                const rateEl = y * RATE_FINE_MAX_DEG_PER_SEC;

                const now = performance.now();
                const changed =
                    Math.abs(rateAz - _lastSentFineRateAz) > RATE_CHANGE_THRESHOLD ||
                    Math.abs(rateEl - _lastSentFineRateEl) > RATE_CHANGE_THRESHOLD;
                const dueForHeartbeat = (now - _lastFineRateSendTime) >= RATE_SEND_INTERVAL_MS;
                const atRest = rateAz === 0 && rateEl === 0 && _lastSentFineRateAz === 0 && _lastSentFineRateEl === 0;

                if (changed || (dueForHeartbeat && !atRest)) {
                    _dotNetRef.invokeMethodAsync('OnJoystickRate', rateAz, rateEl);
                    _lastSentFineRateAz = rateAz;
                    _lastSentFineRateEl = rateEl;
                    _lastFineRateSendTime = now;
                }
            }
        }
    }
    _rafId = requestAnimationFrame(tick);
}

window.stopJoystickControl = function () {
    if (_rafId) {
        cancelAnimationFrame(_rafId);
        _rafId = null;
    }
    _dotNetRef = null;
};
