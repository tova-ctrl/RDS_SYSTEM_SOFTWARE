#pragma once
// =============================================================================
// CameraApi.h — FCC-side receiving end of the PC's CameraApiService.
//
// The PC (RDS_WebApp) has no direct route to the Micro300 camera (it lives on
// the Hive network, currently 1.1.1.x, only reachable from the Jetson). So the
// PC never calls stkAPI directly — it sends {cmd:"SetXxx", data:{...}} over the
// existing TCP link (EthernetServer), where "SetXxx" is the exact stkAPI.h
// function name and "data" holds its arguments as plain JSON numbers (C#
// enums serialize as their underlying int, matching the stkAPI enum values
// 1:1 — see RDS_WebApp/Services/CameraApiService.cs).
//
// This class is the lookup table that turns that JSON back into the real
// stkAPI Set* call, which then sends the actual UDP packet to the camera.
// =============================================================================

#include <cstdint>
#include <string>

class CameraApi {
public:
    // Opens the UDP link to the camera using stkAPI's built-in defaults
    // (REMOTE_IP/COMMUNICATION_PORT/LOCAL_PORT in stkAPI.c — currently
    // 1.1.1.3:1024, host port 1025).
    bool init();

    // Re-point the camera link at a new IP/port (from the WebApp NETWORK tab's
    // CONFIG_CAM_CTRL command) and re-run STKInitializeCommunication so the
    // new endpoint actually takes effect. Closes the previous UDP socket first.
    bool reconfigure(const std::string& camera_ip, uint16_t camera_port, uint16_t host_port);

    // Dispatch one command by stkAPI function name; data_json is the raw
    // JSON line received from the PC (searched directly for "key": fields,
    // same simple-substring approach EthernetServer already uses).
    // Returns false only if func is not a recognized stkAPI function name —
    // callers should fall through to their own "unknown command" handling.
    bool dispatch(const std::string& func, const std::string& data_json);

    // Pulls fresh values from the camera (STKSyncLocal) and returns a flat JSON
    // status object for the WebApp's "Manual Sync Local"-style REFRESH button.
    std::string getStatusJson();

    bool isInitialized() const { return initialized_; }
    std::string lastError() const { return last_error_; }

    // Current camera control link — reads stkAPI's own REMOTE_IP/COMMUNICATION_PORT/
    // LOCAL_PORT globals, so this always reflects whatever init()/reconfigure() last set.
    std::string controlIp()   const;
    uint16_t    controlPort() const;
    uint16_t    hostPort()    const;

private:
    bool        initialized_ = false;
    std::string last_error_;
    std::string ip_storage_;  // REMOTE_IP is a raw char*; stkAPI keeps using this pointer, so it must outlive the connection

    bool doInitialize();
    static double extractNum(const std::string& json, const std::string& key, double def = 0.0);
};
