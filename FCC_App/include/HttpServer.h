#pragma once
// =============================================================================
// HttpServer.h — Simple HTTP REST API for FCC
//
// מאפשר ל-WebApp (Blazor) לשלוח פקודות ל-FCC דרך HTTP POST
// ולקרוא את ה-status דרך HTTP GET.
//
// Endpoints:
//   GET  /api/status          → JSON עם מצב המערכת
//   POST /api/standby         → SAFE → STANDBY
//   POST /api/arm             → STANDBY → ARMED
//   POST /api/disarm          → any → SAFE
//   POST /api/move            → body: {"az":45.0,"el":20.0,"rate":15.0}
//   POST /api/fire            → fire request
//   POST /api/recovery        → FAULT → SAFE
//   POST /api/cam_start       → body: {"ip":"192.168.1.200"}
//   POST /api/cam_stop        → stop camera
//   POST /api/gimbal          → body: {"az":30.0,"el":-10.0}
//   POST /api/zoom_vis        → body: {"factor":5.0}
//   POST /api/zoom_ir         → body: {"factor":2.0}
//
// שימוש מה-WebApp:
//   await http.PostAsync("http://localhost:5300/api/arm", null);
//   var status = await http.GetStringAsync("http://localhost:5300/api/status");
// =============================================================================

#include <string>
#include <cstdint>
#include <atomic>
#include <thread>
#include <functional>
#include <map>

// Forward declaration
class FccController;

class HttpServer {
public:
    static constexpr uint16_t DEFAULT_PORT = 5300;

    explicit HttpServer(FccController& fcc, uint16_t port = DEFAULT_PORT);
    ~HttpServer();

    bool start();
    void stop();
    bool isRunning() const { return running_; }

    std::string lastError() const { return last_error_; }

private:
    FccController& fcc_;
    uint16_t       port_;
    int            server_fd_ = -1;
    std::atomic<bool> running_{ false };
    std::thread    accept_thread_;
    std::string    last_error_;

    // Accept loop
    void acceptLoop();

    // Handle single client connection
    void handleClient(int client_fd);

    // Parse HTTP request
    struct HttpRequest {
        std::string method;   // GET / POST
        std::string path;     // /api/arm
        std::string body;     // JSON body
    };
    HttpRequest parseRequest(const std::string& raw);

    // Route request to FCC command
    std::string route(const HttpRequest& req);

    // Build HTTP responses
    std::string ok(const std::string& body);
    std::string badRequest(const std::string& msg);
    std::string notFound();

    // JSON helpers
    std::string statusJson();
    float extractFloat(const std::string& json, const std::string& key, float def = 0.0f);
    std::string extractString(const std::string& json, const std::string& key,
                              const std::string& def = "");

    void setError(const std::string& msg);
    void log(const std::string& msg);
};
