using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.Json;
using RDS_WebApp.Services;

namespace RDS_WebApp.Services
{
    public record IsmEmgEvent(string IsmState, string Reason, string Ts);

    /// <summary>
    /// Listens on UDP :5202 for ISM emergency datagrams (send-only from ISM).
    /// Raises OnEmergency and forwards FAULT/SAFE transitions to SystemControlService.
    /// </summary>
    public class IsmEmgService : BackgroundService
    {
        public const int EMG_PORT = 5202;

        public event Action<IsmEmgEvent>? OnEmergency;

        private readonly SystemControlService _sys;
        private readonly ILogger<IsmEmgService> _logger;

        public IsmEmgService(SystemControlService sys, ILogger<IsmEmgService> logger)
        {
            _sys    = sys;
            _logger = logger;
        }

        protected override async Task ExecuteAsync(CancellationToken ct)
        {
            using var udp = new UdpClient(new IPEndPoint(IPAddress.Any, EMG_PORT));
            _logger.LogInformation("ISM EMG listener ready on UDP :{Port}", EMG_PORT);

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
                    _logger.LogError(ex, "ISM EMG listener error");
                }
            }
        }

        private void HandleDatagram(string json)
        {
            try
            {
                using var doc  = JsonDocument.Parse(json);
                var root       = doc.RootElement;
                var ismState   = root.GetProperty("ism_state").GetString() ?? "UNKNOWN";
                var reason     = root.GetProperty("reason").GetString()    ?? "";
                var ts         = root.GetProperty("ts").GetString()        ?? "";

                _logger.LogWarning("ISM EMG → state={State} reason={Reason}", ismState, reason);

                var evt = new IsmEmgEvent(ismState, reason, ts);
                OnEmergency?.Invoke(evt);

                // mirror into SystemControlService so the UI reflects ISM state
                var sysState = ismState switch
                {
                    "FAULT"   => SystemState.FAULT,
                    "SAFE"    => SystemState.SAFE,
                    "STANDBY" => SystemState.STANDBY,
                    "ARMED"   => SystemState.ARMED,
                    "FIRING"  => SystemState.FIRING,
                    _         => SystemState.FAULT
                };
                _sys.UpdateState(sysState, $"[ISM-EMG] {ismState}: {reason}");
            }
            catch (Exception ex)
            {
                _logger.LogWarning(ex, "Failed to parse ISM EMG datagram: {Json}", json);
            }
        }
    }
}
