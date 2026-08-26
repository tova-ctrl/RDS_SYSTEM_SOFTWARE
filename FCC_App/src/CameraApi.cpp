#include "CameraApi.h"
#include "stkAPI.h"
#include <cstdlib>
#include <unistd.h>
#include <sstream>
#include <iomanip>

// stkAPI.c's global UDP socket fd (Linux branch) — not declared in stkAPI.h,
// but it has external linkage, so we can reach in to close it before re-init.
extern int SockFD;

bool CameraApi::doInitialize() {
    MessagesTransmitEnableStruct enable{};
    enable.EnableTXDayCameraCommand     = true;
    enable.EnableTXDayCameraSetFOV      = true;
    enable.EnableTXThermalCameraCommand = true;
    enable.EnableTXTICameraSetFOV       = true;
    enable.EnableTXRateCommand          = true;
    enable.EnableTXPositionCommand      = true;
    enable.EnableTXCenterMode           = true;
    enable.EnableTXSafeMode             = true;
    enable.EnableTXTrack                = true;
    enable.EnableTXTrackerParameters    = true;
    enable.EnableTXVideoChannelCommand  = true;
    enable.EnableTXKeepAlive            = true;
    enable.EnableTXCameraPowerControl   = true;
    enable.EnableTXVideoChannelRunTimeConfig = true;

    int res = STKInitializeCommunication(enable);
    initialized_ = (res == 0);
    if (!initialized_) last_error_ = "STKInitializeCommunication failed (" + std::to_string(res) + ")";
    return initialized_;
}

bool CameraApi::init() {
    return doInitialize();
}

std::string CameraApi::controlIp()   const { return REMOTE_IP ? std::string(REMOTE_IP) : std::string(); }
uint16_t    CameraApi::controlPort() const { return static_cast<uint16_t>(COMMUNICATION_PORT); }
uint16_t    CameraApi::hostPort()    const { return static_cast<uint16_t>(LOCAL_PORT); }

bool CameraApi::reconfigure(const std::string& camera_ip, uint16_t camera_port, uint16_t host_port) {
    if (initialized_ && SockFD >= 0) {
        ::close(SockFD);
        SockFD = -1;
    }

    ip_storage_    = camera_ip;
    REMOTE_IP      = const_cast<char*>(ip_storage_.c_str());
    COMMUNICATION_PORT = camera_port;
    LOCAL_PORT     = host_port;

    return doInitialize();
}

double CameraApi::extractNum(const std::string& json, const std::string& key, double def) {
    std::string needle = "\"" + key + "\":";
    auto p = json.find(needle);
    if (p == std::string::npos) return def;
    p += needle.size();
    try { return std::stod(json.substr(p)); } catch (...) { return def; }
}

bool CameraApi::dispatch(const std::string& func, const std::string& data) {
    bool matched = true;

    // ── DayCameraCommand ─────────────────────────────────────────────────
    if (func == "SetDayCameraCommandZoomCommand") {
        if (initialized_) SetDayCameraCommandZoomCommand(
            static_cast<enum ZoomChange>((int)extractNum(data, "ZoomCommand")));
    }
    else if (func == "SetDayCameraCommandGammaCommand") {
        if (initialized_) SetDayCameraCommandGammaCommand(
            static_cast<enum GenericIncDecVal>((int)extractNum(data, "GammaCommand")));
    }
    else if (func == "SetDayCameraCommandSaturationCommand") {
        if (initialized_) SetDayCameraCommandSaturationCommand(
            static_cast<enum GenericIncDecVal>((int)extractNum(data, "SaturationCommand")));
    }
    else if (func == "SetDayCameraCommandLevelCommand") {
        if (initialized_) SetDayCameraCommandLevelCommand(
            static_cast<enum GenericIncDecVal>((int)extractNum(data, "LevelCommand")));
    }
    else if (func == "SetDayCameraCommandGainCommand") {
        if (initialized_) SetDayCameraCommandGainCommand(
            static_cast<enum GenericIncDecVal>((int)extractNum(data, "GainCommand")));
    }
    else if (func == "SetDayCameraCommandExposureMode") {
        if (initialized_) SetDayCameraCommandExposureMode(
            static_cast<enum DTVExposureModeSet>((int)extractNum(data, "ExposureMode")));
    }
    else if (func == "SetDayCameraCommandExposureTime") {
        if (initialized_) SetDayCameraCommandExposureTime(
            static_cast<enum GenericIncDecVal>((int)extractNum(data, "ExposureTime")));
    }

    // ── ThermalCameraCommand ─────────────────────────────────────────────
    else if (func == "SetThermalCameraCommandZoomCommand") {
        if (initialized_) SetThermalCameraCommandZoomCommand(
            static_cast<enum ZoomChange>((int)extractNum(data, "ZoomCommand")));
    }
    else if (func == "SetThermalCameraCommandColorPalette") {
        if (initialized_) SetThermalCameraCommandColorPalette(
            static_cast<enum ThermalCameraColorPalette>((int)extractNum(data, "ColorPalette")));
    }

    // ── CameraPowerControl — per-sensor power ────────────────────────────
    else if (func == "SetCameraPowerControlDayLightCameraPower") {
        if (initialized_) SetCameraPowerControlDayLightCameraPower(
            static_cast<enum PowerStatesRequest>((int)extractNum(data, "DayLightCameraPower")));
    }
    else if (func == "SetCameraPowerControlThermalCameraPower") {
        if (initialized_) SetCameraPowerControlThermalCameraPower(
            static_cast<enum PowerStatesRequest>((int)extractNum(data, "ThermalCameraPower")));
    }

    // ── VideoChannelRunTimeConfig — Channel 0 ("Main") bandwidth only ────
    else if (func == "SetVideoChannelRunTimeConfigBandwidthChannel0Stream") {
        if (initialized_) SetVideoChannelRunTimeConfigBandwidthChannel0Stream(
            (int32_t)extractNum(data, "BandwidthChannel0Stream"));
    }

    // ── RateCommand — gimbal rate, deg/sec ───────────────────────────────
    else if (func == "SetRateCommandAngularVelocities_X") {
        if (initialized_) SetRateCommandAngularVelocities_X((float)extractNum(data, "AngularVelocities_X"));
    }
    else if (func == "SetRateCommandAngularVelocities_Y") {
        if (initialized_) SetRateCommandAngularVelocities_Y((float)extractNum(data, "AngularVelocities_Y"));
    }

    // ── CenterMode / SafeMode ─────────────────────────────────────────────
    else if (func == "SetCenterMode") {
        if (initialized_) SetCenterMode();
    }
    else if (func == "SetSafeMode") {
        if (initialized_) SetSafeMode();
    }

    // ── Track — target coords, pixels relative to screen center ─────────
    else if (func == "SetTrack") {
        if (initialized_) SetTrack((int16_t)extractNum(data, "TargetXCoordinate"),
                                    (int16_t)extractNum(data, "TargetYCoordinate"));
    }

    // ── TrackerParameters — manual gate size, pixels ─────────────────────
    else if (func == "SetTrackerParametersGateXSize") {
        if (initialized_) SetTrackerParametersGateXSize((uint16_t)extractNum(data, "GateXSize"));
    }
    else if (func == "SetTrackerParametersGateYSize") {
        if (initialized_) SetTrackerParametersGateYSize((uint16_t)extractNum(data, "GateYSize"));
    }

    // ── VideoChannelCommand — active sensor toggle ───────────────────────
    else if (func == "SetVideoChannelCommandPrimaryVideoChannel") {
        if (initialized_) SetVideoChannelCommandPrimaryVideoChannel(
            static_cast<enum Camera>((int)extractNum(data, "PrimaryVideoChannel")));
    }

    else {
        matched = false;
    }

    if (matched && !initialized_) {
        last_error_ = "camera link not initialized — command ignored: " + func;
    }
    return matched;
}

std::string CameraApi::getStatusJson() {
    if (!initialized_) return R"({"ok":false,"error":"camera link not initialized"})";

    STKSyncLocal(200);  // pull fresh report values from the camera (200ms timeout)

    // Note: the SDK's own report field names don't line up with the matching
    // Set-command field names — "Level"/"Gain" commands read back as
    // "Contrast"/"Brigthness" (sic) in DayCameraReport. See [[camera-sdk-command-mapping]].
    std::ostringstream j;
    j << std::fixed << std::setprecision(2);
    j << "{\"ok\":true"
      << ",\"posAz\":"       << GetPositionReportRelativeGimbalAngles_X()
      << ",\"posEl\":"       << GetPositionReportRelativeGimbalAngles_Y()
      << ",\"rateAz\":"      << GetRate_GRRAngularVelocities_X()
      << ",\"rateEl\":"      << GetRate_GRRAngularVelocities_Y()
      << ",\"dayFov\":"      << GetDayCameraReportFOV()
      << ",\"gamma\":"       << GetDayCameraReportGammaValue()
      << ",\"saturation\":"  << GetDayCameraReportSaturationValue()
      << ",\"level\":"       << GetDayCameraReportContrastValue()
      << ",\"gain\":"        << GetDayCameraReportBrigthnessValue()
      << ",\"expTime\":"     << GetDayCameraReportExposureTime()
      << ",\"exposureMode\":" << static_cast<int>(GetDayCameraReportExposureMode())
      << ",\"irFov\":"       << GetThermalCameraReportFOV()
      << ",\"primaryChannel\":" << static_cast<int>(GetVideoChannelReportPrimaryVideoChannel())
      << "}";
    return j.str();
}
