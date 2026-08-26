using System.Text.Json;
using System.Threading.Tasks;

namespace RDS_WebApp.Services
{
    // =========================================================================
    // CameraApiService — PC-side mirror of the Micro300 SDK (SDK_11.4/SDK/stkAPI.h).
    //
    // The camera is on the Hive network (1.1.1.x) which only the FCC (Jetson) can
    // reach — the PC has no direct link to it. So this service does NOT call
    // stkAPI itself; instead each method mirrors an stkAPI "Set..." function
    // 1:1 by name and argument shape, and forwards it to the FCC over the
    // existing TCP command channel (EthernetService). The FCC is expected to
    // receive {cmd:"SetXxx", data:{...}} and call the real stkAPI Set function,
    // which then sends the UDP packet to the camera at 1.1.1.3:1024.
    //
    // Keeping the PC-side names identical to stkAPI.h means the FCC dispatcher
    // is a straight lookup table — no translation layer to maintain.
    // =========================================================================

    // ── enum ZoomChange (stkAPI.h) ──────────────────────────────────────────
    public enum ZoomChange { Nochange = 0, Zoomout = 1, Zoomin = 2 }

    // ── enum GenericIncDecVal (stkAPI.h) ────────────────────────────────────
    public enum GenericIncDecVal { NoChange = 0, DecreaseValue = 1, IncreaseValue = 2 }

    // ── enum DTVExposureModeSet (stkAPI.h) ──────────────────────────────────
    public enum DTVExposureModeSet { SetAutomaticExposureMode = 0, SetAutomaticGainMode = 1, SetManualExposureMode = 2 }

    // ── enum DTVExposureMode (stkAPI.h) — report-side mode readback ────────
    public enum DTVExposureMode { ManualExposureMode = 0, AutomaticGainMode = 9, AutomaticExposureMode = 12 }

    // ── enum Camera (stkAPI.h) — renamed CameraSensor to avoid clashing with the domain name "camera" ──
    public enum CameraSensor { TI = 1, DTV = 2, SeeSpot = 4 }

    // ── enum ThermalCameraColorPalette (stkAPI.h) ───────────────────────────
    public enum ThermalCameraColorPalette
    {
        SetColorPaletteNC = 0,
        SetColorPaletteWhiteHot = 1,
        SetColorPaletteBlackHot = 2,
        SetColorPaletteRainbow = 3,
        SetColorPaletteRainbowHC = 4,
        SetColorPaletteIronBow = 5
    }

    // ── enum PowerStatesRequest (stkAPI.h) ──────────────────────────────────
    public enum PowerStatesRequest { PowerRequestNC = 0, PowerRequestOn = 1, PowerRequestOff = 2 }

    // ── GET_CAMERA_STATUS response — mirrors CameraApi::getStatusJson() on the FCC ──
    public record CameraStatus(
        bool Ok,
        float PosAz, float PosEl,
        float RateAz, float RateEl,
        // Level/Gain mirror GetDayCameraReportContrastValue/BrigthnessValue — both
        // floats in the SDK (unlike ExposureTime, a genuine int32_t) — must stay
        // float here or System.Text.Json throws deserializing "0.00" into an int.
        float DayFov, float Gamma, float Saturation, float Level, float Gain, int ExpTime, int ExposureMode,
        float IrFov,
        int PrimaryChannel);

    // ── GET_CONFIG response — mirrors FccController::getConfigJson() on the FCC ──
    public record CameraConfig(
        bool Ok,
        string ControlIp, int ControlPort, int HostPort,
        string StreamIp, int StreamPort1);

    public class CameraApiService
    {
        private readonly EthernetService _eth;

        public CameraApiService(EthernetService eth)
        {
            _eth = eth;
        }

        // Every call is fire-and-forget over the existing FCC TCP command
        // channel; SendCommandAsync already no-ops (returns false) when not
        // connected, so callers don't need to guard on connection state.
        private Task<bool> Send(string func, object? args) =>
            _eth.SendCommandAsync(func, args);

        // ── DayCameraCommand (5600) ─────────────────────────────────────────
        public Task<bool> SetDayCameraCommandZoomCommand(ZoomChange ZoomCommand) =>
            Send(nameof(SetDayCameraCommandZoomCommand), new { ZoomCommand });

        public Task<bool> SetDayCameraCommandGammaCommand(GenericIncDecVal GammaCommand) =>
            Send(nameof(SetDayCameraCommandGammaCommand), new { GammaCommand });

        public Task<bool> SetDayCameraCommandSaturationCommand(GenericIncDecVal SaturationCommand) =>
            Send(nameof(SetDayCameraCommandSaturationCommand), new { SaturationCommand });

        public Task<bool> SetDayCameraCommandLevelCommand(GenericIncDecVal LevelCommand) =>
            Send(nameof(SetDayCameraCommandLevelCommand), new { LevelCommand });

        public Task<bool> SetDayCameraCommandGainCommand(GenericIncDecVal GainCommand) =>
            Send(nameof(SetDayCameraCommandGainCommand), new { GainCommand });

        public Task<bool> SetDayCameraCommandExposureMode(DTVExposureModeSet ExposureMode) =>
            Send(nameof(SetDayCameraCommandExposureMode), new { ExposureMode });

        public Task<bool> SetDayCameraCommandExposureTime(GenericIncDecVal ExposureTime) =>
            Send(nameof(SetDayCameraCommandExposureTime), new { ExposureTime });

        // ── ThermalCameraCommand ────────────────────────────────────────────
        public Task<bool> SetThermalCameraCommandZoomCommand(ZoomChange ZoomCommand) =>
            Send(nameof(SetThermalCameraCommandZoomCommand), new { ZoomCommand });

        public Task<bool> SetThermalCameraCommandColorPalette(ThermalCameraColorPalette ColorPalette) =>
            Send(nameof(SetThermalCameraCommandColorPalette), new { ColorPalette });

        // ── RateCommand — gimbal rate, deg/sec ──────────────────────────────
        public Task<bool> SetRateCommandAngularVelocities_X(float AngularVelocities_X) =>
            Send(nameof(SetRateCommandAngularVelocities_X), new { AngularVelocities_X });

        public Task<bool> SetRateCommandAngularVelocities_Y(float AngularVelocities_Y) =>
            Send(nameof(SetRateCommandAngularVelocities_Y), new { AngularVelocities_Y });

        // ── CenterMode / SafeMode ────────────────────────────────────────────
        public Task<bool> SetCenterMode() => Send(nameof(SetCenterMode), null);
        public Task<bool> SetSafeMode()   => Send(nameof(SetSafeMode), null);

        // ── Track — target coords in pixels, relative to screen center ─────
        public Task<bool> SetTrack(short TargetXCoordinate, short TargetYCoordinate) =>
            Send(nameof(SetTrack), new { TargetXCoordinate, TargetYCoordinate });

        // ── TrackerParameters — manual gate size, pixels ────────────────────
        public Task<bool> SetTrackerParametersGateXSize(ushort GateXSize) =>
            Send(nameof(SetTrackerParametersGateXSize), new { GateXSize });

        public Task<bool> SetTrackerParametersGateYSize(ushort GateYSize) =>
            Send(nameof(SetTrackerParametersGateYSize), new { GateYSize });

        // ── VideoChannelCommand — active sensor toggle ──────────────────────
        public Task<bool> SetVideoChannelCommandPrimaryVideoChannel(CameraSensor PrimaryVideoChannel) =>
            Send(nameof(SetVideoChannelCommandPrimaryVideoChannel), new { PrimaryVideoChannel });

        // ── CameraPowerControl — per-sensor power, not a single unified on/off ──
        public Task<bool> SetCameraPowerControlDayLightCameraPower(PowerStatesRequest DayLightCameraPower) =>
            Send(nameof(SetCameraPowerControlDayLightCameraPower), new { DayLightCameraPower });

        public Task<bool> SetCameraPowerControlThermalCameraPower(PowerStatesRequest ThermalCameraPower) =>
            Send(nameof(SetCameraPowerControlThermalCameraPower), new { ThermalCameraPower });

        // ── VideoChannelRunTimeConfig — Channel 0 ("Main") only, no per-channel
        // equivalent exists in the SDK for the 2nd/3rd streams. "leave 0 if no
        // change required" per stkAPI.h; units not documented in the header.
        public Task<bool> SetVideoChannelRunTimeConfigBandwidthChannel0Stream(int BandwidthChannel0Stream) =>
            Send(nameof(SetVideoChannelRunTimeConfigBandwidthChannel0Stream), new { BandwidthChannel0Stream });

        // ── GET_CAMERA_STATUS — manual-refresh telemetry pull (STKSyncLocal) ──
        // Mirrors the vendor demo app's own "Manual Sync Local" button.
        public async Task<CameraStatus?> GetStatusAsync()
        {
            var resp = await _eth.SendAndReceiveAsync("GET_CAMERA_STATUS");
            if (string.IsNullOrEmpty(resp)) return null;
            try
            {
                return JsonSerializer.Deserialize<CameraStatus>(resp,
                    new JsonSerializerOptions { PropertyNameCaseInsensitive = true });
            }
            catch { return null; }
        }

        // ── GET_CONFIG — current FCC-side camera control link + video stream target ──
        public async Task<CameraConfig?> GetConfigAsync()
        {
            var resp = await _eth.SendAndReceiveAsync("GET_CONFIG");
            if (string.IsNullOrEmpty(resp)) return null;
            try
            {
                return JsonSerializer.Deserialize<CameraConfig>(resp,
                    new JsonSerializerOptions { PropertyNameCaseInsensitive = true });
            }
            catch { return null; }
        }
    }
}
