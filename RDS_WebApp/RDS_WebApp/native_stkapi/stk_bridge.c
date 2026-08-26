// Thin P/Invoke-friendly wrapper around the vendor's stkAPI.c/h (MicroCon
// Micro300 SDK) — deliberately does NOT modify stkAPI.c/h at all, so this
// stays a byte-for-byte copy of FCC_App's own copy, no drift risk.
//
// Why this exists (2026-08-08): stkAPI.c's own command structs (e.g.
// DayCameraCommandStruct) are tightly packed C bitfields (`uint8_t
// ZoomCommand : 2;` etc.) — reproducing that exact bit layout from C# via
// P/Invoke struct marshaling would be fragile and hard to verify. Instead,
// every function exported here takes only plain ints/strings/floats; all
// bitfield struct construction stays entirely on the C side, where the
// native compiler's own layout is authoritative and nothing can drift out
// of sync across the language boundary.
//
// Talks to the camera's control port DIRECTLY from the PC, bypassing the
// FCC's TCP relay + CameraApi.cpp dispatch entirely — that relay path was
// confirmed silent for Zoom (sent, but no visible effect), so this direct
// path is what the operator-facing controls actually use now (see
// StkDirectCameraService.cs and [[mobilicom-controller-mapping]] memory).
#include "stkAPI.h"
#include <stdbool.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <winsock2.h>
#include <mstcpip.h>
#pragma comment(lib, "ws2_32.lib")
// mstcpip.h doesn't always expose this without extra SDK-version defines —
// this is the well-known standalone definition (_WSAIOW(IOC_VENDOR, 12)).
#ifndef SIO_UDP_CONNRESET
#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR, 12)
#endif
// stkAPI.c's own global socket handle (not static there, so it has external
// linkage) — reached from here, not from stkAPI.c itself, to keep that file
// a byte-for-byte untouched copy of the vendor's SDK (see file header above).
extern SOCKET SockFD;
#endif

#ifdef _WIN32
#define STK_EXPORT __declspec(dllexport)
#else
#define STK_EXPORT
#endif

static bool s_initialized = false;
static char s_ip_storage[64];
static int s_last_sync_error = 1; // sentinel "never synced" — not a real SyncLocalErrorEnum value

// 0 (ERR_NO_ERROR) = last Stk_Sync() call actually got and parsed a real
// packet. Anything else (see enum SyncLocalErrorEnum in stkAPI.h: -1
// ERR_UDP_RX, -2 ERR_HEADER_KEY, -3 ERR_LEN_MISSMATCH, -4 ERR_CRC_MISSMATCH)
// means every Get*Report* value read since is stale/default, not real
// telemetry. Callers MUST check this before trusting any Get*Report* read.
STK_EXPORT int Stk_LastSyncError(void)
{
    return s_last_sync_error;
}

// Blocks up to timeoutMs DRAINING as many incoming telemetry packets as
// arrive from the camera in that window, parsing each one. STKSyncLocal is
// NOT a request/response call — it never transmits anything, it just
// receives+parses whatever the camera's own periodic broadcast sends next
// (see stkAPI.c), ONE packet per call. Call Stk_Sync() ONCE per refresh,
// then read as many Get*Report* values as needed from whatever's been
// parsed so far — added 2026-08-08 after finding the old per-field
// Stk_SyncAndGetXxx functions did a SEPARATE blocking receive for each
// field, consuming a different incoming packet per call.
//
// EXTENDED 2026-08-10: a single STKSyncLocal call only updates whichever
// report structs happen to be bundled in THAT ONE packet — not every
// message type is necessarily present in every packet (the parser walks a
// variable list of message IDs per packet, see STKSyncLocal's switch
// statement). User found this concretely: VideoChannelReport_obj (message
// 52851, read by GetVideoChannelReportPrimaryVideoChannel) wasn't updating
// after a channel switch because the ONE packet a single STKSyncLocal call
// happened to consume didn't include it that time — not a bug in the
// getter, just bad luck on which packet got caught. Now drains repeatedly
// (short per-call timeout so we don't overshoot the budget) until either
// the packet queue looks empty (an attempt returns nothing) after we've
// already gotten at least one, or the overall timeoutMs budget runs out —
// whichever first. This makes it far more likely that whatever report type
// a caller actually wants was refreshed within this one Stk_Sync() call.
STK_EXPORT int Stk_Sync(int timeoutMs)
{
    if (!s_initialized) { s_last_sync_error = 1; return 1; }

    clock_t start = clock();
    int last_err = 1;
    int got_any = 0;
    const int per_call_timeout_ms = 60; // short, so we can drain several packets within timeoutMs

    while (1)
    {
        long elapsed_ms = (long)((clock() - start) * 1000L / CLOCKS_PER_SEC);
        long remaining_ms = (long)timeoutMs - elapsed_ms;
        if (remaining_ms <= 0) break;

        int this_timeout = (remaining_ms < per_call_timeout_ms) ? (int)remaining_ms : per_call_timeout_ms;
        int err = (int)STKSyncLocal((uint16_t)this_timeout);

        if (err == 0)
        {
            got_any = 1;
            last_err = 0;
            continue; // a packet just arrived — more may already be queued, keep draining
        }

        // Nothing available right now. If we already got at least one
        // packet this call, call it a success and stop early rather than
        // burning the rest of the budget waiting for more. Otherwise keep
        // retrying until the overall timeout budget is used up.
        if (got_any) break;
        last_err = err;
    }

    s_last_sync_error = last_err;
    return s_last_sync_error;
}

// Must be called once before any other Stk_* function. Mirrors FCC_App's own
// CameraApi::doInitialize() enable-flag set exactly (see CameraApi.cpp) —
// though under SEND_ON_SET (stkAPI.h) these flags only gate the separate
// bulk-resync path (STKUpdateRemote), not the immediate per-Set sends this
// bridge actually uses.
STK_EXPORT int Stk_Init(const char* remote_ip, int control_port, int host_port)
{
    strncpy(s_ip_storage, remote_ip, sizeof(s_ip_storage) - 1);
    s_ip_storage[sizeof(s_ip_storage) - 1] = '\0';
    REMOTE_IP = s_ip_storage;
    COMMUNICATION_PORT = control_port;
    LOCAL_PORT = host_port;

    struct MessagesTransmitEnableStruct enable;
    memset(&enable, 0, sizeof(enable));
    enable.EnableTXDayCameraCommand          = true;
    enable.EnableTXDayCameraSetFOV           = true;
    enable.EnableTXThermalCameraCommand      = true;
    enable.EnableTXTICameraSetFOV            = true;
    enable.EnableTXRateCommand               = true;
    enable.EnableTXPositionCommand           = true;
    enable.EnableTXCenterMode                = true;
    enable.EnableTXSafeMode                  = true;
    enable.EnableTXTrack                     = true;
    enable.EnableTXTrackerParameters         = true;
    enable.EnableTXVideoChannelCommand       = true;
    enable.EnableTXVideoChannelRunTimeConfig = true;
    enable.EnableTXCameraPowerControl        = true;
    enable.EnableTXKeepAlive                 = true;

    int res = STKInitializeCommunication(enable);
    s_initialized = (res == 0);

#ifdef _WIN32
    // 2026-08-12: Wireshark caught our own PC replying ICMP "Port Unreachable"
    // to the camera's own UDP sends TO us on the host port — meaning nothing
    // was listening there anymore, mid-session, despite Stk_Init succeeding
    // only once. Windows' default UDP behavior is to fail a socket's pending
    // recvfrom with WSAECONNRESET (and related odd behavior) the moment ANY
    // ICMP Port Unreachable arrives for it — a well-known Windows Sockets
    // pitfall for UDP "servers". SIO_UDP_CONNRESET disables that. Applied
    // here (not in stkAPI.c) to keep that file an untouched vendor copy.
    if (s_initialized)
    {
        BOOL newBehavior = FALSE;
        DWORD bytesReturned = 0;
        WSAIoctl(SockFD, SIO_UDP_CONNRESET, &newBehavior, sizeof(newBehavior), NULL, 0, &bytesReturned, NULL, NULL);
    }
#endif

    return res;
}

STK_EXPORT int Stk_IsInitialized(void)
{
    return s_initialized ? 1 : 0;
}

// Closes the UDP control socket. Nothing in this codebase ever called this
// before (2026-08-16) -- the socket just lived until the OS reclaimed it on
// process exit. Correct regardless: any app that opens a socket must close
// it on shutdown, not rely on the OS doing it implicitly. Called from
// StkDirectCameraService's IDisposable on app shutdown (DI singleton,
// disposed by the host) -- covers Cancel/graceful stop; an outright kill
// still has the OS close the handle either way.
STK_EXPORT void Stk_Close(void)
{
    if (!s_initialized) return;
#ifdef _WIN32
    closesocket(SockFD);
    WSACleanup();
#endif
    s_initialized = false;
}

// "Sensor ON/OFF" — see StkDirectCameraService.cs for the full story of how
// this was pinned down (three wrong-ish hypotheses, then the real answer:
// the actual power command for both directions, plus PrimaryVideoChannel on
// the way up to route it to the output). camera: 1=TI (thermal), 2=DTV (day).
STK_EXPORT int Stk_SetPrimaryVideoChannel(int camera)
{
    if (!s_initialized) return -100;
    return SetVideoChannelCommandPrimaryVideoChannel((enum Camera)camera);
}

// 2026-08-16: SystemConfig.razor's "BITRATE" control — Channel 0 ("Main")
// only, per stkAPI.h (no equivalent for the 2nd/3rd streams).
STK_EXPORT int Stk_SetBandwidth(int bandwidth)
{
    if (!s_initialized) return -100;
    return SetVideoChannelRunTimeConfigBandwidthChannel0Stream(bandwidth);
}

// Read-only — does NOT sync. Call Stk_Sync() first.
STK_EXPORT int Stk_GetPrimaryVideoChannel(void)
{
    return (int)GetVideoChannelReportPrimaryVideoChannel();
}

// T1 (LB) — camera lock. Matches the vendor demo's own plain "Track" button
// (see [[camera-sdk-command-mapping]] memory: click = SetTrack(0,0)) — locks
// the tracker onto whatever is at screen center (this RWS's own crosshair is
// drawn there). Coordinates are pixels relative to screen center, per the
// SDK's own doc comment on SetTrack.
STK_EXPORT int Stk_SetTrack(int x, int y)
{
    if (!s_initialized) return -100;
    return SetTrack((int16_t)x, (int16_t)y);
}

// Gate size (KeyID 6510, TrackerParameters) — must be sent BEFORE Stk_SetTrack.
// Found by stepping through the vendor's own STK300cpp demo: without an
// explicit gate size + GateSizeEnable(true), TrackerParameters_obj.GateSizeEnable
// defaults to false (see stkAPI.c struct default), and Track drops within
// ~1 second even on a high-contrast target. Call all three together.
STK_EXPORT int Stk_SetTrackerGateXSize(int sizePx)
{
    if (!s_initialized) return -100;
    return SetTrackerParametersGateXSize((uint16_t)sizePx);
}

STK_EXPORT int Stk_SetTrackerGateYSize(int sizePx)
{
    if (!s_initialized) return -100;
    return SetTrackerParametersGateYSize((uint16_t)sizePx);
}

STK_EXPORT int Stk_SetTrackerGateSizeEnable(int enable)
{
    if (!s_initialized) return -100;
    return SetTrackerParametersGateSizeEnable(enable != 0);
}

// Read-only — does NOT sync, call Stk_Sync() first. 2026-08-12: confirmed via
// QBee (the vendor's own operator app) that the camera draws its OWN onboard
// tracker gate reticle directly into the video signal (baked into the
// pixels, an OSD feature of the camera itself) — completely independent of
// anything this app draws. Our own separately-computed crosshair (from an
// independent YOLO pass on our side) could never coincide with it, since
// they're two unrelated tracking results. These expose the CAMERA's own
// TrackerTargetErrorReport (KeyID-reported gate-vs-boresight offset, in
// pixels) so our crosshair can instead reflect what the camera itself is
// actually tracking.
STK_EXPORT float Stk_GetTrackerXError(void) { return GetTrackerTargetErrorReportTrackerXError(); }
STK_EXPORT float Stk_GetTrackerYError(void) { return GetTrackerTargetErrorReportTrackerYError(); }
STK_EXPORT int   Stk_GetTrackingQuality(void) { return (int)GetTrackerTargetErrorReportTrackingQuality(); }
STK_EXPORT int   Stk_GetTrackerCurrFOV(void) { return (int)GetTrackerTargetErrorReportCurrFOV(); }

// J1/J2 while locked (2026-08-12): the camera appears to ignore RateCommand
// while its own internal Track mode is engaged (SetTrack was sent at lock
// time) — SetTrackerAdjustControls is the SDK's dedicated "nudge the gate
// while tracking" command instead (pixels, not deg/sec; positive = right,
// down per the SDK's own doc comment on the underlying function).
STK_EXPORT int Stk_SetTrackerAdjust(int enable, int moveX, int moveY)
{
    if (!s_initialized) return -100;
    return SetTrackerAdjustControls(enable != 0, (int16_t)moveX, (int16_t)moveY);
}

// Read-only — does NOT sync. Call Stk_Sync() first. Real tracker state
// (NoTrack=0, TrackerOK=1, Prediction=2, TrackLoss=3 — enum TrackerStatus)
// so the UI's LOCKED indicator reflects reality, not just "we sent the
// command" — same lesson as everything else that turned out to need real
// telemetry instead of an assumed state this session.
STK_EXPORT int Stk_GetTrackerStatus(void)
{
    return (int)GetTrackerTargetErrorReportTrackerGeneralStatus();
}

// J1 (joystick left stick) — continuous gimbal pan/tilt rate, degrees/sec.
// See [[mobilicom-controller-mapping]] memory. Units per stkAPI.h's own doc
// comment: "[°/Sec]".
STK_EXPORT int Stk_SetRateX(float degPerSec)
{
    if (!s_initialized) return -100;
    return SetRateCommandAngularVelocities_X(degPerSec);
}

STK_EXPORT int Stk_SetRateY(float degPerSec)
{
    if (!s_initialized) return -100;
    return SetRateCommandAngularVelocities_Y(degPerSec);
}

// Per the SDK's own doc comment: "Need to send to system every ~500 mSec".
// Added 2026-08-08 — user reported the live picture freezes after ~30-60
// minutes, and we were never sending this at all (neither via the FCC relay
// nor this direct path). Caller is responsible for the ~500ms cadence (see
// StkDirectCameraService's own Timer) — this function just fires one signal.
STK_EXPORT int Stk_SendKeepAlive(void)
{
    if (!s_initialized) return -100;
    return SetKeepAlive();
}

// Real payload (camera) communication-failure flag from the gimbal's own
// BIT status — closer to "is the camera actually alive/talking to us" than
// the per-sensor power state below. Read-only — call Stk_Sync() first.
STK_EXPORT int Stk_GetPayloadCommFail(void)
{
    return GetGimbalMasterBITStatusPayloadcommfail() ? 1 : 0;
}

// ── Gimbal operating modes (2026-08-08) — no-arg SDK commands, same pattern
// as everything else here. Added through the direct path (not the FCC
// relay) from the start, since that relay was already proven unreliable for
// Zoom/Sensor this session — no reason to repeat that debugging cycle here.
STK_EXPORT int Stk_SetCenterMode(void)       { if (!s_initialized) return -100; return SetCenterMode(); }
STK_EXPORT int Stk_SetSafeMode(void)         { if (!s_initialized) return -100; return SetSafeMode(); }
STK_EXPORT int Stk_SetSystemInit(void)       { if (!s_initialized) return -100; return SetSystemInit(); }
STK_EXPORT int Stk_SetIDLE(void)             { if (!s_initialized) return -100; return SetIDLE(); }
STK_EXPORT int Stk_SetDriftCalibration(void) { if (!s_initialized) return -100; return SetDriftCalibration(); }

// ── Day/VIS channel ─────────────────────────────────────────────────────
// zoomCommand/incDec: 0=Nochange, 1=Zoomout/Decrease, 2=Zoomin/Increase
// (enum ZoomChange / enum GenericIncDecVal in stkAPI.h — same 0/1/2 shape).
STK_EXPORT int Stk_SetDayZoom(int zoomCommand)
{
    if (!s_initialized) return -100;
    return SetDayCameraCommandZoomCommand((enum ZoomChange)zoomCommand);
}

STK_EXPORT int Stk_SetDaySharpness(int incDec)
{
    if (!s_initialized) return -100;
    return SetDayCameraCommandSharpnessCommand((enum GenericIncDecVal)incDec);
}

STK_EXPORT int Stk_SetDayLevel(int incDec)
{
    if (!s_initialized) return -100;
    return SetDayCameraCommandLevelCommand((enum GenericIncDecVal)incDec);
}

STK_EXPORT int Stk_SetDayGain(int incDec)
{
    if (!s_initialized) return -100;
    return SetDayCameraCommandGainCommand((enum GenericIncDecVal)incDec);
}

// 2026-08-16: added for SystemConfig.razor's migration off the FCC-relay
// (CameraApiService) onto this direct path, per the user's explicit request
// that all CONTROL commands go straight PC->camera. Same 0/1/2 shape as the
// Level/Gain/Sharpness setters above.
STK_EXPORT int Stk_SetDayGamma(int incDec)
{
    if (!s_initialized) return -100;
    return SetDayCameraCommandGammaCommand((enum GenericIncDecVal)incDec);
}

STK_EXPORT int Stk_SetDaySaturation(int incDec)
{
    if (!s_initialized) return -100;
    return SetDayCameraCommandSaturationCommand((enum GenericIncDecVal)incDec);
}

STK_EXPORT int Stk_SetDayExposureTime(int incDec)
{
    if (!s_initialized) return -100;
    return SetDayCameraCommandExposureTime((enum GenericIncDecVal)incDec);
}

STK_EXPORT int Stk_SetDayRestoreDefault(void)
{
    if (!s_initialized) return -100;
    return SetDayCameraCommandRestoreToDefault(true);
}

// mode: 0=SetAutomaticExposureMode, 1=SetAutomaticGainMode, 2=SetManualExposureMode
STK_EXPORT int Stk_SetDayExposureMode(int mode)
{
    if (!s_initialized) return -100;
    return SetDayCameraCommandExposureMode((enum DTVExposureModeSet)mode);
}

// powerRequest: 0=NoChange, 1=On, 2=Off (enum PowerStatesRequest)
STK_EXPORT int Stk_SetDayPower(int powerRequest)
{
    if (!s_initialized) return -100;
    return SetCameraPowerControlDayLightCameraPower((enum PowerStatesRequest)powerRequest);
}

// Read-only — call Stk_Sync() first.
STK_EXPORT float Stk_GetDayFOV(void)
{
    return GetDayCameraReportFOV();
}

// 2026-08-16: added to migrate SystemConfig.razor's GetStatusAsync (FCC-relay)
// off onto this direct path too — user's explicit request that everything
// camera-related bypass the FCC entirely, not just Set commands. All
// read-only — call Stk_Sync() first, same as Stk_GetDayFOV above.
//
// Level/Gain are deliberately swapped vs. their SDK field names — this
// mirrors CameraApiService.cs's own documented mapping (its CameraStatus
// record's doc comment): "Level" = GetDayCameraReportContrastValue,
// "Gain" = GetDayCameraReportBrigthnessValue. Not a mistake introduced here —
// keeping the existing, already-verified convention so the UI's meaning
// doesn't silently change under whoever reads "Level"/"Gain" in the app.
STK_EXPORT float Stk_GetDayGamma(void)      { return GetDayCameraReportGammaValue(); }
STK_EXPORT float Stk_GetDaySaturation(void) { return GetDayCameraReportSaturationValue(); }
STK_EXPORT float Stk_GetDayLevel(void)      { return GetDayCameraReportContrastValue(); }
STK_EXPORT float Stk_GetDayGain(void)       { return GetDayCameraReportBrigthnessValue(); }
STK_EXPORT int   Stk_GetDayExposureTime(void) { return GetDayCameraReportExposureTime(); }
STK_EXPORT int   Stk_GetDayExposureMode(void) { return (int)GetDayCameraReportExposureMode(); }

STK_EXPORT float Stk_GetPositionAz(void) { return GetPositionReportRelativeGimbalAngles_X(); }
STK_EXPORT float Stk_GetPositionEl(void) { return GetPositionReportRelativeGimbalAngles_Y(); }
STK_EXPORT float Stk_GetRateAz(void)     { return GetRate_GRRAngularVelocities_X(); }
STK_EXPORT float Stk_GetRateEl(void)     { return GetRate_GRRAngularVelocities_Y(); }

// Real (not locally-assumed) power state, per sensor. PowerReportOff=0,
// PowerReportOn=1 (enum PowerStatesReport). Read-only — call Stk_Sync() first.
//
// NOTE (2026-08-08): the SDK reports Day/VIS power as TWO separate states —
// Wide DTV and Narrow DTV — not one. SetCameraPowerControlDayLightCameraPower's
// own doc comment just says "Enables/Disables DTV Sensor" (singular), so it's
// not clear from the header alone which physical path is actually driving the
// live video we display, or whether the Set command even affects both. Report
// BOTH here rather than guessing; caller ORs them (see StkDirectCameraService).
STK_EXPORT int Stk_GetDayWidePowerState(void)
{
    return (int)GetCameraPowerReportWideDTVState();
}

STK_EXPORT int Stk_GetDayNarrowPowerState(void)
{
    return (int)GetCameraPowerReportNarrowDTVState();
}

// ── Thermal channel ─────────────────────────────────────────────────────
STK_EXPORT int Stk_SetThermalZoom(int zoomCommand)
{
    if (!s_initialized) return -100;
    return SetThermalCameraCommandZoomCommand((enum ZoomChange)zoomCommand);
}

STK_EXPORT int Stk_SetThermalSharpness(int incDec)
{
    if (!s_initialized) return -100;
    return SetThermalCameraCommandSharpnessCommand((enum GenericIncDecVal)incDec);
}

STK_EXPORT int Stk_SetThermalLevel(int incDec)
{
    if (!s_initialized) return -100;
    return SetThermalCameraCommandLevelCommand((enum GenericIncDecVal)incDec);
}

STK_EXPORT int Stk_SetThermalGain(int incDec)
{
    if (!s_initialized) return -100;
    return SetThermalCameraCommandGainCommand((enum GenericIncDecVal)incDec);
}

STK_EXPORT int Stk_SetThermalRestoreDefault(void)
{
    if (!s_initialized) return -100;
    return SetThermalCameraCommandRestoreToDefault(true);
}

STK_EXPORT int Stk_SetThermalPower(int powerRequest)
{
    if (!s_initialized) return -100;
    return SetCameraPowerControlThermalCameraPower((enum PowerStatesRequest)powerRequest);
}

// Read-only — call Stk_Sync() first.
STK_EXPORT float Stk_GetThermalFOV(void)
{
    return GetThermalCameraReportFOV();
}

// TIState = Thermal Imager power state. Read-only — call Stk_Sync() first.
STK_EXPORT int Stk_GetThermalPowerState(void)
{
    return (int)GetCameraPowerReportTIStateState();
}
