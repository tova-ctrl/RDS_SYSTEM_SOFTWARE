#pragma once
// =============================================================================
// EthernetServer.h — TCP JSON command server (RDS WebApp ↔ FCC)
//
// Listens on a TCP port (default 5000).
// Each connected client sends newline-delimited JSON commands:
//
//   {"cmd":"CAM_ON",  "data":{"camera":"MICRO300"}, "ts":"..."}
//   {"cmd":"CAM_OFF", "data":{"camera":"MICRO300"}, "ts":"..."}
//   {"cmd":"SET_POSITION", "data":{"x":45.0,"y":10.0}, "ts":"..."}
//   {"cmd":"GET_POSITION", "ts":"..."}
//
// GET_POSITION responds with: {"x":45.0,"y":10.0}\n
// All other commands respond with: {"ok":true}\n  or {"ok":false}\n
// =============================================================================

#include "FccController.h"
#include <atomic>
#include <string>
#include <thread>

class EthernetServer {
public:
    static constexpr uint16_t DEFAULT_PORT = 5000;

    explicit EthernetServer(FccController& fcc, uint16_t port = DEFAULT_PORT);
    ~EthernetServer() { stop(); }

    bool start();
    void stop();

    std::string lastError() const { return last_error_; }

private:
    FccController&    fcc_;
    uint16_t          port_;
    int               server_fd_ = -1;
    std::atomic<bool> running_{ false };
    std::thread       accept_thread_;
    std::string       last_error_;

    void acceptLoop();
    void handleClient(int client_fd);

    // Parse and dispatch one JSON command line, write response to client_fd.
    void dispatch(const std::string& json, int client_fd);
    
    // Helpers
    static double extractDouble(const std::string& json, const std::string& key, double def = 0.0);
    static void   sendResponse(int fd, const std::string& json);
    static void   log(const std::string& msg);
};
