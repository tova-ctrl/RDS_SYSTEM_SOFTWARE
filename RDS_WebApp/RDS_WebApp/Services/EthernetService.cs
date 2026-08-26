using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Text;
using System.Text.Json;
using System.Text.Json.Nodes;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.JSInterop;
namespace RDS_WebApp.Services
{
    public enum EthernetStatus { Disconnected, Connecting, Connected, Error }

    public record PositionData(double X, double Y);

    public class EthernetService : IDisposable
    {
        public EthernetStatus Status { get; private set; } = EthernetStatus.Disconnected;
        public string LastError { get; private set; } = "";
        public PositionData? LastPosition { get; private set; }

        public event Action? OnChange;

        private TcpClient? _client;
        private NetworkStream? _stream;
        private readonly string _host;
        private readonly int _port;
        private readonly SemaphoreSlim _lock = new SemaphoreSlim(1, 1);
       

        
        public EthernetService(string host = "192.168.55.1", int port = 5000)  //string host = "127.0.0.1"ת,192.168.137.202
        {
            _host = host;
            _port = port;
            
        }

        public async Task ConnectAsync()
        {
            if (Status == EthernetStatus.Connected && _client?.Connected == true) return;
            if (Status == EthernetStatus.Connecting) return;
            Cleanup(); // clean up any stale connection

            Status = EthernetStatus.Connecting;
            NotifyChange();

            try
            {
                using var connectCts = new CancellationTokenSource(TimeSpan.FromSeconds(3));
                _client = new TcpClient();
                await _client.ConnectAsync(_host, _port, connectCts.Token);
                _stream = _client.GetStream();
                Status = EthernetStatus.Connected;
            }
            catch (Exception ex)
            {
                Status = EthernetStatus.Error;
                LastError = ex.Message.Length > 60 ? ex.Message[..60] : ex.Message;
                Cleanup();
            }

            NotifyChange();
        }

        public void Disconnect()
        {
            Cleanup();
            Status = EthernetStatus.Disconnected;
            NotifyChange();
        }

        // Must be called while holding _lock
        private async Task<bool> SendCommandInternalAsync(string command, object? payload = null)
        {
            if (Status != EthernetStatus.Connected || _stream == null)
            {
                LastError = "Not connected";
                return false;
            }

            try
            {
                var msg = JsonSerializer.Serialize(new { cmd = command, data = payload, ts = DateTime.UtcNow });
                var bytes = Encoding.UTF8.GetBytes(msg + "\n");
                await _stream.WriteAsync(bytes);
                return true;
            }
            catch (Exception ex)
            {
                LastError = ex.Message;
                Status = EthernetStatus.Error;
                Cleanup();
                NotifyChange();
                return false;
            }
        }

        // Must be called while holding _lock. FCC sends exactly one reply line
        // per command it receives — this must always be drained, even when the
        // caller doesn't care about the content, or the leftover bytes corrupt
        // the next SendAndReceiveAsync() read (stale/concatenated JSON).
        private async Task<string?> ReadResponseLineAsync()
        {
            try
            {
                using var cts = new CancellationTokenSource(TimeSpan.FromSeconds(3));
                var buf = new byte[4096];
                var sb = new StringBuilder();

                while (true)
                {
                    int n = await _stream!.ReadAsync(buf, cts.Token);
                    if (n == 0) break;
                    sb.Append(Encoding.UTF8.GetString(buf, 0, n));
                    if (sb.ToString().Contains('\n')) break;
                }

                return sb.ToString().Trim();
            }
            catch { return null; }
        }

        public async Task<bool> SendCommandAsync(string command, object? payload = null)
        {
            await _lock.WaitAsync();
            try
            {
                if (!await SendCommandInternalAsync(command, payload))
                    return false;
                await ReadResponseLineAsync();
                return true;
            }
            finally { _lock.Release(); }
        }

        public async Task<string?> SendAndReceiveAsync(string command, object? payload = null)
        {
            await _lock.WaitAsync();
            try
            {
                if (!await SendCommandInternalAsync(command, payload))
                    return null;
                return await ReadResponseLineAsync();
            }
            finally { _lock.Release(); }
        }

        // ── Camera ───────────────────────────────────────────────────────────
        public async Task<bool> RecoveryResetAsync()
        {
            await ConnectAsync();
            var r = await SendAndReceiveAsync("recovery");
            return r?.Contains("\"ok\":true") == true;
        }

        public async Task<bool> CameraOnAsync()
        {
            var r = await SendAndReceiveAsync("CAM_ON", new { camera = "MICRO300" });
            return r != null;
        }

        public async Task<bool> CameraOffAsync()
        {
            var r = await SendAndReceiveAsync("CAM_OFF", new { camera = "MICRO300" });
            return r != null;
        }

        // ── Position ─────────────────────────────────────────────────────────
        public async Task<PositionData?> GetPositionAsync()
        {
            var response = await SendAndReceiveAsync("GET_POSITION");
            if (response == null) return null;
            try
            {
               
                var lines = response.Split(new[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);

                if (lines.Length == 0) return null;
                /////////////////////////
                // Grab the last valid JSON object in the string
                string latestJson = lines[^1];
                using var doc = JsonDocument.Parse(latestJson);  
                var root = doc.RootElement;
                var x = root.GetProperty("x").GetDouble();
                var y = root.GetProperty("y").GetDouble();
                LastPosition = new PositionData(x, y);
                NotifyChange();
                return LastPosition;
            }
            catch (Exception ex)
            {
                // Shows a pop-up with the error details, an "Error" title, an OK button, and an error icon
               string m=ex.Message;
                return null;
            }
        }

        public async Task<bool> SetPositionAsync(double x, double y)
        {
            var response = await SendAndReceiveAsync("SET_POSITION", new { x, y });
            return response != null;
        }

        // ── Logs ─────────────────────────────────────────────────────────────
        public async Task<List<string>> GetLogsAsync()
        {
            var response = await SendAndReceiveAsync("GET_LOGS");
            if (response == null) return new List<string>();
            try
            {
               // string jsonString = "{\"ok\":true,\"logs\":[\"[FCC STATUS] 118 ISM=SAFE torque=OFF az=0.3° el=0.2° rate=0.0dps [STOP] | CAM=OFF GIM=az0.0° el0.0° hb_seq=2352\"]}";

                // פירוק ה-JSON לאובייקט נוד דינמי
#pragma warning disable CS8600 // Converting null literal or possible null value to non-nullable type.
                JsonNode rootNode = JsonNode.Parse(response);
#pragma warning restore CS8600 // Converting null literal or possible null value to non-nullable type.

                // שליפת המערך מתוך המפתח "logs" והמרתו ישירות ל-List<string>
#pragma warning disable CS8600 // Converting null literal or possible null value to non-nullable type.
                List<string> logs = rootNode["logs"].Deserialize<List<string>>();
#pragma warning restore CS8600 // Converting null literal or possible null value to non-nullable type.
                //var logs = JsonSerializer.Deserialize<List<string>>(response);
                return logs ?? new List<string>();
            }
            catch(Exception ex){
                string m=ex.Message;
                 return new List<string>();
            }
        }

        private void Cleanup()
        {
            _stream?.Dispose();
            _client?.Dispose();
            _stream = null;
            _client = null;
        }

        private void NotifyChange() => OnChange?.Invoke();

        public void Dispose() => Cleanup();
    }
}
