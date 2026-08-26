using System.IO;
using System.Text.Json;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Hosting;

namespace RDS_WebApp.Services
{
    // Everything the SystemConfig page needs to survive a WebApp restart when
    // FCC isn't reachable to ask directly. NETWORK tab fields are genuine
    // configuration; the camera-settings fields are a "last known" snapshot,
    // always re-confirmed via GetConfigAsync()/GetStatusAsync() when FCC is
    // reachable — FCC's live values win over this file (see [[project-state]]).
    public class RdsPersistedConfig
    {
        // Network
        public string FccIp { get; set; } = "192.168.55.1";
        public int FccPort { get; set; } = 5000;
        public string IsmIp { get; set; } = "192.168.55.3";
        public string UartDev { get; set; } = "/dev/ttyTHS0";
        public int UartBaud { get; set; } = 115200;
        public string CamCtrlIp { get; set; } = "1.1.1.3";
        public int CamCtrlPort { get; set; } = 1024;
        public int CamHostPort { get; set; } = 1025;
        public string CamStreamIp { get; set; } = "1.1.1.2";
        public int StreamPort1 { get; set; } = 51000;
        public int StreamPort2 { get; set; } = 51001;
        public int StreamPort3 { get; set; } = 51002;

        // Last-known camera settings snapshot (telemetry, not really "config")
        public float DayFov { get; set; }
        public float Gamma { get; set; }
        public float Saturation { get; set; }
        public float Level { get; set; }
        public float Gain { get; set; }
        public int ExpTime { get; set; }
        public int ExposureMode { get; set; }
        public float IrFov { get; set; }
        public int PrimaryChannel { get; set; }
    }

    public class ConfigPersistenceService
    {
        private readonly string _filePath;
        private readonly SemaphoreSlim _lock = new(1, 1);

        public ConfigPersistenceService(IWebHostEnvironment env)
        {
            var dir = Path.Combine(env.ContentRootPath, "App_Data");
            Directory.CreateDirectory(dir);
            _filePath = Path.Combine(dir, "rds_config.json");
        }

        public async Task<RdsPersistedConfig> LoadAsync()
        {
            await _lock.WaitAsync();
            try
            {
                if (!File.Exists(_filePath)) return new RdsPersistedConfig();
                var json = await File.ReadAllTextAsync(_filePath);
                return JsonSerializer.Deserialize<RdsPersistedConfig>(json) ?? new RdsPersistedConfig();
            }
            catch { return new RdsPersistedConfig(); }
            finally { _lock.Release(); }
        }

        public async Task SaveAsync(RdsPersistedConfig config)
        {
            await _lock.WaitAsync();
            try
            {
                var json = JsonSerializer.Serialize(config, new JsonSerializerOptions { WriteIndented = true });
                await File.WriteAllTextAsync(_filePath, json);
            }
            catch { /* best-effort persistence — a failed disk write shouldn't break the UI */ }
            finally { _lock.Release(); }
        }
    }
}
