using System.Linq.Expressions;
using System.Runtime.InteropServices;
using System.Threading;

namespace RDS_WebApp.Services
{
    // Talks to the Micro300 camera's control port DIRECTLY from the PC via the
    // vendor's own stkAPI, compiled to a native DLL (native_stkapi/stkapi_native.dll —
    // see stk_bridge.c for why this is a thin wrapper instead of P/Invoking
    // stkAPI's own bitfield structs directly). Bypasses the FCC's TCP relay +
    // CameraApi.cpp dispatch entirely.
    //
    // Built 2026-08-08 specifically to diagnose why zoom commands sent through
    // the normal FCC-relay path (CameraApiService -> EthernetService -> FCC ->
    // CameraApi.cpp -> stkAPI.c) don't visibly affect the camera — confirmed
    // that path is silent, so this direct path is now what the operator-facing
    // camera controls actually use (see [[mobilicom-controller-mapping]] memory).
    // Note: the FCC-relay path exists because in the real deployment, the
    // camera may only be reachable from the FCC/Jetson, not from a remote
    // operator PC (see [[project-state]] memory, 2026-07-26 network topology
    // notes) — don't assume this direct path replaces the FCC path in all
    // deployments without confirming that constraint.
    //
    // THREAD SAFETY (critical, found 2026-08-08 the hard way — a process
    // crash, exit code -1): stkAPI.c is plain C with static/global buffers
    // (RXBuff, TXBuff, the various *_obj report structs, SockFD) — nothing
    // about it is thread-safe. This service gets called from at least three
    // independent threads: the UI/circuit thread (button clicks), a Task.Run
    // worker (telemetry refresh), and a System.Threading.Timer callback
    // (the ~500ms KeepAlive heartbeat below). Without serialization, two of
    // those could enter the native DLL at the same time and race on the same
    // memory — which is almost certainly what crashed the whole process, not
    // a managed exception (a plain exception wouldn't take the process down;
    // AccessViolationException from native memory corruption will). EVERY
    // entry point into the DLL below goes through `lock (_lock)` — do not
    // add a new P/Invoke call anywhere in this file without wrapping it.
    public class StkDirectCameraService : IDisposable
    {
        private const string Dll = "stkapi_native.dll";

        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_Init(string remoteIp, int controlPort, int hostPort);
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern void Stk_Close();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_IsInitialized();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_Sync(int timeoutMs);
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_LastSyncError();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SendKeepAlive();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetRateX(float degPerSec);
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetRateY(float degPerSec);
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetTrack(int x, int y);
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_GetTrackerStatus();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetTrackerGateXSize(int sizePx);
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetTrackerGateYSize(int sizePx);
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetTrackerGateSizeEnable(int enable);
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern float Stk_GetTrackerXError();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern float Stk_GetTrackerYError();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_GetTrackingQuality();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_GetTrackerCurrFOV();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetTrackerAdjust(int enable, int moveX, int moveY);

        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetPrimaryVideoChannel(int camera);
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetBandwidth(int bandwidth);
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_GetPrimaryVideoChannel();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_GetPayloadCommFail();

        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetCenterMode();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetSafeMode();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetSystemInit();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetIDLE();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetDriftCalibration();

        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetDayZoom(int zoomCommand);
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetDaySharpness(int incDec);
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetDayLevel(int incDec);
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetDayGain(int incDec);
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetDayRestoreDefault();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetDayExposureMode(int mode);
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetDayGamma(int incDec);
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetDaySaturation(int incDec);
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetDayExposureTime(int incDec);
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetDayPower(int powerRequest);
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern float Stk_GetDayFOV();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_GetDayWidePowerState();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_GetDayNarrowPowerState();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern float Stk_GetDayGamma();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern float Stk_GetDaySaturation();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern float Stk_GetDayLevel();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern float Stk_GetDayGain();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_GetDayExposureTime();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_GetDayExposureMode();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern float Stk_GetPositionAz();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern float Stk_GetPositionEl();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern float Stk_GetRateAz();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern float Stk_GetRateEl();

        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetThermalZoom(int zoomCommand);
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetThermalSharpness(int incDec);
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetThermalLevel(int incDec);
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetThermalGain(int incDec);
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetThermalRestoreDefault();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_SetThermalPower(int powerRequest);
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern float Stk_GetThermalFOV();
        [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)] private static extern int Stk_GetThermalPowerState();

        private readonly object _lock = new();
        private bool _initialized;
        private string? _lastError;
        private Thread? _keepAliveThread;
        private volatile bool _disposed;

        public string? LastError => _lastError;
        public bool Initialized => _initialized;

        // Callers already hold _lock. Not itself locking (unlike the old
        // version) — every public entry point below takes the lock ONCE for
        // its whole body (init-check + native call together), since C#'s
        // lock is reentrant per-thread but there is no reason to release and
        // reacquire it mid-method.
        private bool EnsureInitializedLocked(string remoteIp = "1.1.1.3", int controlPort = 1024, int hostPort = 1025)
        {
            if (_initialized) return true;
            try
            {
                int res = Stk_Init(remoteIp, controlPort, hostPort);
                _initialized = res == 0;
                _lastError = _initialized ? null : $"Stk_Init returned {res}";

                // Per the SDK's own doc comment on SetKeepAlive: "Need to send to
                // system every ~500 mSec". User reported the live picture freezes
                // after ~30-60 minutes (2026-08-08) — plausibly this, since
                // neither the FCC relay nor this direct path ever sent one before.
                // Started once, for the app's lifetime.
                //
                // 2026-08-12: moved off System.Threading.Timer onto a dedicated
                // background Thread — Timer callbacks run on the shared .NET
                // ThreadPool, which EthernetService's own async TCP I/O for the
                // ISM also depends on; starving that pool has caused real
                // operational COMM_LOSS disconnects before (established project
                // convention — see LiveTrackingService for the same pattern).
                // Piling multiple ThreadPool timers on top of each other (this
                // one, plus ones added/removed earlier today) was a live
                // suspect during today's camera-control-freeze investigation
                // (see [[mobilicom-controller-mapping]] memory) once an actual
                // ISM COMM_LOSS was observed mid-session.
                if (_initialized && _keepAliveThread == null)
                {
                    _keepAliveThread = new Thread(() =>
                    {
                        while (!_disposed)
                        {
                            Thread.Sleep(500);
                            if (_disposed) break;
                            lock (_lock) { try { Stk_SendKeepAlive(); } catch { /* best-effort */ } }
                        }
                    })
                    { IsBackground = true, Name = "StkKeepAlive" };
                    _keepAliveThread.Start();
                }
            }
            catch (Exception ex)
            {
                _initialized = false;
                _lastError = ex.Message;
            }
            return _initialized;
        }

        public bool EnsureInitialized()
        {
            lock (_lock) { return EnsureInitializedLocked(); }
        }

        // ── One telemetry snapshot per call — the fix for a real bug found
        // 2026-08-08: the previous design had each field (FOV, primary
        // channel, power state, link) do its OWN Stk_Sync internally, i.e.
        // FOUR separate blocking receives per refresh, each one consuming a
        // DIFFERENT incoming packet from the camera. Not every packet
        // necessarily contains every report type, so that risked reading
        // fields that were moments apart or stale, and checking the sync
        // error only after the LAST call could miss an earlier one failing.
        // Now: sync exactly once, then read everything from that one parsed
        // snapshot, all inside one lock.
        public readonly record struct CamTelemetry(int SyncError, float Fov, int PrimaryChannel, int PowerState, bool? LinkFail);

        public CamTelemetry RefreshTelemetry(bool thermal, int timeoutMs = 200)
        {
            lock (_lock)
            {
                if (!EnsureInitializedLocked())
                    return new CamTelemetry(1, -1f, -1, -1, null);
                int syncErr = -1;

                try
                {
                    syncErr = Stk_Sync(timeoutMs);
                }
                catch (Exception ex) { };
                if (syncErr != 0)
                    return new CamTelemetry(syncErr, -1f, -1, -1, null);

                float fov = thermal ? Stk_GetThermalFOV() : Stk_GetDayFOV();
                int primary = Stk_GetPrimaryVideoChannel();
                int power;
                if (thermal)
                {
                    power = Stk_GetThermalPowerState();
                }
                else
                {
                    // Day/VIS power is reported as TWO separate states (Wide DTV /
                    // Narrow DTV) — not clear which drives the live picture, so
                    // treat "on" as either reporting on rather than guessing.
                    int wide = Stk_GetDayWidePowerState();
                    int narrow = Stk_GetDayNarrowPowerState();
                    power = (wide < 0 && narrow < 0) ? -1 : ((wide == 1 || narrow == 1) ? 1 : 0);
                }
                bool linkFail = Stk_GetPayloadCommFail() == 1;
                return new CamTelemetry(0, fov, primary, power, linkFail);
            }
        }

        // Direct-path replacement for CameraApiService.GetStatusAsync() (2026-08-16,
        // migrating SystemConfig.razor off the FCC relay entirely for anything
        // camera-related — user's explicit request). Same one-Sync-then-read-
        // everything pattern as RefreshTelemetry above. Level/Gain read from
        // Stk_GetDayLevel/Stk_GetDayGain, which are deliberately the SDK's
        // ContrastValue/BrigthnessValue respectively — see stk_bridge.c's own
        // note on why (mirrors CameraApiService.cs's already-established mapping,
        // not a new decision made here).
        public readonly record struct CamFullStatus(
            int SyncError, float PosAz, float PosEl, float RateAz, float RateEl,
            float DayFov, float Gamma, float Saturation, float Level, float Gain,
            int ExpTime, int ExposureMode, float IrFov, int PrimaryChannel);

        public CamFullStatus RefreshFullStatus(int timeoutMs = 200)
        {
            lock (_lock)
            {
                if (!EnsureInitializedLocked())
                    return new CamFullStatus(1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1);

                int syncErr;
                try { syncErr = Stk_Sync(timeoutMs); }
                catch { syncErr = 1; }
                if (syncErr != 0)
                    return new CamFullStatus(syncErr, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1);

                return new CamFullStatus(
                    0,
                    Stk_GetPositionAz(), Stk_GetPositionEl(),
                    Stk_GetRateAz(), Stk_GetRateEl(),
                    Stk_GetDayFOV(), Stk_GetDayGamma(), Stk_GetDaySaturation(),
                    Stk_GetDayLevel(), Stk_GetDayGain(),
                    Stk_GetDayExposureTime(), Stk_GetDayExposureMode(),
                    Stk_GetThermalFOV(), Stk_GetPrimaryVideoChannel());
            }
        }

        // J1 (joystick left stick) — continuous gimbal pan/tilt rate, °/sec.
        // See [[mobilicom-controller-mapping]] memory.
        public int SetRateX(float degPerSec) { lock (_lock) { return EnsureInitializedLocked() ? Stk_SetRateX(degPerSec) : -100; } }
        public int SetRateY(float degPerSec) { lock (_lock) { return EnsureInitializedLocked() ? Stk_SetRateY(degPerSec) : -100; } }

        // T1 (LB) — camera lock. x,y are pixels relative to screen center
        // (0,0 = lock onto whatever's at the crosshair, matching the vendor
        // demo's own plain "Track" button — see [[camera-sdk-command-mapping]]).
        public int SetTrack(short x, short y)
        {
            lock (_lock) { return EnsureInitializedLocked() ? Stk_SetTrack(x, y) : -100; }
        }

        // Must be sent BEFORE SetTrack — without an explicit gate size +
        // GateSizeEnable(true), TrackerParameters_obj.GateSizeEnable defaults
        // to false on the camera and Track drops within ~1 second even on a
        // high-contrast target (found by stepping through the vendor's own
        // STK300cpp demo — see stk_bridge.c's Stk_SetTrackerGate* comment).
        //
        // 2026-08-12: small pacing delays added between these three sends (and
        // between this call and the following SetTrack, at the Home.razor call
        // site) — LB was observed to occasionally wedge the camera's own
        // CONTROL port (not just the onboard tracker), requiring a manual
        // "Save Settings" on the camera's MicroAccess page to clear (see
        // [[mobilicom-controller-mapping]] memory). Working theory: 4 UDP Set
        // commands fired back-to-back with zero spacing may overrun a small
        // receive queue on the camera's firmware side. This pacing is a
        // low-risk mitigation attempt, not a confirmed fix — Thread.Sleep here
        // is deliberate (this only runs once per LB press, not in a hot loop).
        public int ConfigureTrackerGate(int sizePx = 20, int sizePy = 20)
        {
            lock (_lock)
            {
                if (!EnsureInitializedLocked()) return -100;
                Stk_SetTrackerGateXSize(sizePx);
                Thread.Sleep(30);
                Stk_SetTrackerGateYSize(sizePy);
                Thread.Sleep(30);
                return Stk_SetTrackerGateSizeEnable(1);
            }
        }

        // Individual gate-size setters (2026-08-16) — for SystemConfig.razor's
        // separate GATE X / GATE Y buttons, which set one axis at a time
        // (matching the old FCC-relay CameraApiService.SetTrackerParametersGateXSize/
        // YSize behavior exactly, no GateSizeEnable side effect). ConfigureTrackerGate
        // above is T1's own all-in-one helper — this is deliberately narrower.
        public int SetTrackerGateX(int sizePx)
        {
            lock (_lock) { return EnsureInitializedLocked() ? Stk_SetTrackerGateXSize(sizePx) : -100; }
        }

        public int SetTrackerGateY(int sizePx)
        {
            lock (_lock) { return EnsureInitializedLocked() ? Stk_SetTrackerGateYSize(sizePx) : -100; }
        }

        // T1 unlock (2026-08-12): the SDK has no explicit "cancel track" command
        // at all (confirmed earlier this session — nothing in stkAPI.h) — so
        // unlocking previously only stopped LiveTrackingService locally and
        // never told the camera anything. Disabling the gate is the closest
        // thing to a cancel available; unverified whether the camera treats
        // this as actually ending its internal Track mode. Being tried as a
        // candidate fix for the CONTROL-port freeze (see
        // [[mobilicom-controller-mapping]] memory) — repeated SetTrack calls
        // without ever properly closing out the previous one is a live theory.
        public int DisableTrackerGate()
        {
            lock (_lock) { return EnsureInitializedLocked() ? Stk_SetTrackerGateSizeEnable(0) : -100; }
        }

        // Real tracker state (enum TrackerStatus: 0=NoTrack, 1=TrackerOK,
        // 2=Prediction, 3=TrackLoss) — one sync + read, so callers can confirm
        // a lock attempt actually took instead of just assuming success the
        // moment SetTrack was sent. Returns (syncError, status); status is -1
        // if sync failed (don't trust it then — see CamTelemetry's own note).
        public (int SyncError, int Status) SyncTrackerStatus(int timeoutMs = 200)
        {
            lock (_lock)
            {
                if (!EnsureInitializedLocked()) return (1, -1);
                int syncErr = Stk_Sync(timeoutMs);
                if (syncErr != 0) return (syncErr, -1);
                return (0, Stk_GetTrackerStatus());
            }
        }

        // The camera's OWN onboard tracker gate — the reticle it draws directly
        // into the video signal itself (confirmed via QBee, 2026-08-12), separate
        // from anything computed on this side. XError/YError are the gate's
        // offset from boresight, in pixels, per the SDK's own doc comment —
        // reference resolution not documented, needs empirical calibration
        // against what's actually visible on screen.
        public (int SyncError, float XError, float YError, int Quality, int Status) SyncTrackerError(int timeoutMs = 200)
        {
            lock (_lock)
            {
                if (!EnsureInitializedLocked()) return (1, 0, 0, -1, -1);
                int syncErr = Stk_Sync(timeoutMs);
                if (syncErr != 0) return (syncErr, 0, 0, -1, -1);
                return (0, Stk_GetTrackerXError(), Stk_GetTrackerYError(), Stk_GetTrackingQuality(), Stk_GetTrackerStatus());
            }
        }

        // J1/J2 while locked — nudges the camera's own onboard tracker gate
        // (pixels), used instead of SetRateX/Y because the camera appears to
        // ignore RateCommand while its internal Track mode is engaged.
        public int SetTrackerAdjust(bool enable, short moveX, short moveY)
        {
            lock (_lock) { return EnsureInitializedLocked() ? Stk_SetTrackerAdjust(enable ? 1 : 0, moveX, moveY) : -100; }
        }

        // "Sensor ON/OFF" — see stk_bridge.c / [[project-state]] memory for the
        // full story of how this was pinned down. camera: 1=TI, 2=DTV, 4=SeeSpot.
        public int SetPrimaryVideoChannel(int camera)
        {
            lock (_lock) { return EnsureInitializedLocked() ? Stk_SetPrimaryVideoChannel(camera) : -100; }
        }

        // Channel 0 ("Main") bitrate only — no SDK equivalent for the 2nd/3rd streams.
        public int SetBandwidth(int bandwidth)
        {
            lock (_lock) { return EnsureInitializedLocked() ? Stk_SetBandwidth(bandwidth) : -100; }
        }

        // ── Gimbal operating modes ────────────────────────────────────────
        public int SetCenterMode()       { lock (_lock) { return EnsureInitializedLocked() ? Stk_SetCenterMode() : -100; } }
        public int SetSafeMode()         { lock (_lock) { return EnsureInitializedLocked() ? Stk_SetSafeMode() : -100; } }
        public int SetSystemInit()       { lock (_lock) { return EnsureInitializedLocked() ? Stk_SetSystemInit() : -100; } }
        public int SetIDLE()             { lock (_lock) { return EnsureInitializedLocked() ? Stk_SetIDLE() : -100; } }
        public int SetDriftCalibration() { lock (_lock) { return EnsureInitializedLocked() ? Stk_SetDriftCalibration() : -100; } }

        // ── Day/VIS channel ──────────────────────────────────────────────
        // incDec/zoomCommand: 0=Nochange, 1=Decrease/Zoomout, 2=Increase/Zoomin
        public int SetDayZoom(int zoomCommand)  { lock (_lock) { return EnsureInitializedLocked() ? Stk_SetDayZoom(zoomCommand) : -100; } }
        public int SetDaySharpness(int incDec)  { lock (_lock) { return EnsureInitializedLocked() ? Stk_SetDaySharpness(incDec) : -100; } }
        public int SetDayLevel(int incDec)      { lock (_lock) { return EnsureInitializedLocked() ? Stk_SetDayLevel(incDec) : -100; } }
        public int SetDayGain(int incDec)       { lock (_lock) { return EnsureInitializedLocked() ? Stk_SetDayGain(incDec) : -100; } }
        public int SetDayGamma(int incDec)      { lock (_lock) { return EnsureInitializedLocked() ? Stk_SetDayGamma(incDec) : -100; } }
        public int SetDaySaturation(int incDec) { lock (_lock) { return EnsureInitializedLocked() ? Stk_SetDaySaturation(incDec) : -100; } }
        public int SetDayExposureTime(int incDec) { lock (_lock) { return EnsureInitializedLocked() ? Stk_SetDayExposureTime(incDec) : -100; } }
        public int RestoreDayDefault()          { lock (_lock) { return EnsureInitializedLocked() ? Stk_SetDayRestoreDefault() : -100; } }
        // mode: 0=AutoExposure, 1=AutoGain, 2=Manual
        public int SetDayExposureMode(int mode) { lock (_lock) { return EnsureInitializedLocked() ? Stk_SetDayExposureMode(mode) : -100; } }
        // powerRequest: 0=NoChange, 1=On, 2=Off
        public int SetDayPower(int powerRequest) { lock (_lock) { return EnsureInitializedLocked() ? Stk_SetDayPower(powerRequest) : -100; } }

        // ── Thermal channel ──────────────────────────────────────────────
        public int SetThermalZoom(int zoomCommand)   { lock (_lock) { return EnsureInitializedLocked() ? Stk_SetThermalZoom(zoomCommand) : -100; } }
        public int SetThermalSharpness(int incDec)   { lock (_lock) { return EnsureInitializedLocked() ? Stk_SetThermalSharpness(incDec) : -100; } }
        public int SetThermalLevel(int incDec)       { lock (_lock) { return EnsureInitializedLocked() ? Stk_SetThermalLevel(incDec) : -100; } }
        public int SetThermalGain(int incDec)        { lock (_lock) { return EnsureInitializedLocked() ? Stk_SetThermalGain(incDec) : -100; } }
        public int RestoreThermalDefault()           { lock (_lock) { return EnsureInitializedLocked() ? Stk_SetThermalRestoreDefault() : -100; } }
        public int SetThermalPower(int powerRequest) { lock (_lock) { return EnsureInitializedLocked() ? Stk_SetThermalPower(powerRequest) : -100; } }

        // 2026-08-16: this service is a DI singleton (Program.cs), so the ASP.NET
        // Core host disposes it automatically on any graceful shutdown (Ctrl+C,
        // Stop Debugging's clean path, service stop) -- an app that opens a
        // socket must close it on shutdown, not just let the OS reclaim it.
        // Signal the KeepAlive thread to stop BEFORE closing the socket (it
        // must not try to send on a closed handle), then Stk_Close under the
        // same lock as every other native call.
        public void Dispose()
        {
            _disposed = true;
            _keepAliveThread?.Join(1000);
            lock (_lock)
            {
                if (_initialized)
                {
                    try { Stk_Close(); } catch { /* best-effort on shutdown */ }
                    _initialized = false;
                }
            }
        }
    }
}
