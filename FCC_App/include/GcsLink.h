#pragma once
// =============================================================================
// GcsLink.h — UDP stub for FCC ↔ GCS (Ground Control Station) communication
//
// Sends ISM state updates as JSON over UDP to the GCS IP:port.
// The Blazor WebApp (RDS_WebApp) or any GCS client listens on this port.
//
// Production upgrade: replace with encrypted UDP (DTLS) or TCP + TLS.
// =============================================================================

#include <string>
#include <cstdint>

class GcsLink {
public:
    GcsLink(const std::string& gcs_ip, uint16_t gcs_port, uint16_t listen_port);
    ~GcsLink();

    bool open();
    void close();
    bool isOpen() const { return send_fd_ >= 0; }

    // Push ISM state to GCS as JSON.
    void sendStateUpdate(uint8_t ism_state, bool torque_enabled,
                         float az, float el);

    // Push fault event to GCS.
    void sendFaultEvent(uint16_t fault_code, uint8_t ism_state,
                        const std::string& message);

    // Non-blocking: check if GCS sent a command (ARM, DISARM, etc.)
    // Returns empty string if nothing pending, otherwise JSON string.
    std::string pollCommand();

    std::string lastError() const { return last_error_; }

private:
    std::string gcs_ip_;
    uint16_t    gcs_port_;
    uint16_t    listen_port_;
    int         send_fd_   = -1;
    int         recv_fd_   = -1;
    std::string last_error_;

    void setError(const std::string& msg);
    std::string buildStateJson(uint8_t ism_state, bool torque_enabled,
                               float az, float el);
};
