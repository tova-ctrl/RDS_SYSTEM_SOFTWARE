using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.Json;

namespace RDS_WebApp.Services
{
    /// <summary>
    /// Listens on UDP :5200 for FCC state datagrams (sent by GcsLink::sendStateUpdate).
    /// JSON format: {"type":"state","ism_state":5,"torque_enabled":false,"azimuth_deg":0.0,"elevation_deg":0.0,"timestamp":"..."}
    /// </summary>
    public class GcsStateService : BackgroundService
    {
        public const int GCS_PORT = 5200;

        private readonly SystemControlService _sys;
        private readonly ILogger<GcsStateService> _logger;

        public GcsStateService(SystemControlService sys, ILogger<GcsStateService> logger)
        {
            _sys    = sys;
            _logger = logger;
        }

        protected override async Task ExecuteAsync(CancellationToken ct)
        {
            using var udp = new UdpClient(new IPEndPoint(IPAddress.Any, GCS_PORT));
            _logger.LogInformation("GCS state listener ready on UDP :{Port}", GCS_PORT);

            while (!ct.IsCancellationRequested)
            {
                try
                {
                    var result = await udp.ReceiveAsync(ct);
                    var json   = Encoding.UTF8.GetString(result.Buffer);
                    HandleDatagram(json);
                }
                catch (OperationCanceledException) { break; }
                catch (Exception ex)
                {
                    _logger.LogError(ex, "GCS state listener error");
                }
            }
        }

        private void HandleDatagram(string json)
        {
            try
            {
                using var doc   = JsonDocument.Parse(json);
                var root        = doc.RootElement;
                var type        = root.TryGetProperty("type", out var t) ? t.GetString() : "";

                if (type != "state" && type != "fault") return;

                int ismStateInt = root.GetProperty("ism_state").GetInt32();

                var sysState = ismStateInt switch
                {
                    0 => SystemState.SAFE,
                    1 => SystemState.STANDBY,
                    2 => SystemState.ARMED,
                    3 => SystemState.FIRING,
                    4 => SystemState.FAULT,
                    5 => SystemState.COMM_LOSS,   // ISM_STATE_COMM_LOSS
                    _ => SystemState.FAULT
                };

                _logger.LogInformation("GCS state → {State} (raw={Raw})", sysState, ismStateInt);
                _sys.UpdateState(sysState, $"[FCC-GCS] ISM={sysState}");
            }
            catch (Exception ex)
            {
                _logger.LogWarning(ex, "Failed to parse GCS datagram: {Json}", json);
            }
        }
    }
}
