using Microsoft.AspNetCore.Mvc;
using RDS_WebApp.Components;
using RDS_WebApp.Controllers;
using RDS_WebApp.Services;

var builder = WebApplication.CreateBuilder(args);

// Add services to the container.
builder.Services.AddRazorComponents()
    .AddInteractiveServerComponents();
builder.Services.AddControllers(); // הוסף את זה אם אתה משתמש ב-Controllers
builder.Services.AddSingleton<SystemControlService>();
builder.Services.AddSingleton<EthernetService>();
builder.Services.AddSingleton<CameraApiService>();
builder.Services.AddSingleton<StkDirectCameraService>();
builder.Services.AddSingleton<ConfigPersistenceService>();
builder.Services.AddHostedService<IsmEmgService>();
builder.Services.AddHostedService<GcsStateService>();
builder.Services.AddSingleton<ImageDetectorService>();
builder.Services.AddSingleton<DetectionResultService>();
builder.Services.AddSingleton<LiveTrackingService>();
var app = builder.Build();


// Configure the HTTP request pipeline.
if (!app.Environment.IsDevelopment())
{
    //app.UseExceptionHandler("/Error", createScopeForErrors: true);
    // The default HSTS value is 30 days. You may want to change this for production scenarios, see https://aka.ms/aspnetcore-hsts.
    //app.UseHsts();
    app.UseExceptionHandler("/Error");
}

app.UseStatusCodePagesWithReExecute("/not-found", createScopeForStatusCodePages: true);
app.UseHttpsRedirection();

app.UseAntiforgery();

app.UseWebSockets();

// Live camera video relay — see CameraVideoStreamer.cs. Demuxes the camera's UDP
// MPEG-TS push. H.264: forwards raw NAL units, JMuxer remuxes to fragmented MP4
// for MediaSource Extensions. H.265/HEVC (a genuine hard requirement of this
// camera/drone, browsers can't play it via MSE): decoded server-side and
// re-encoded to JPEG, drawn onto a <canvas> client-side instead.
app.Map("/stream-video", async (HttpContext context) =>
{
    if (!context.WebSockets.IsWebSocketRequest)
    {
        context.Response.StatusCode = StatusCodes.Status400BadRequest;
        return;
    }

    int port = context.Request.Query.TryGetValue("port", out var p) && int.TryParse(p, out var parsed)
        ? parsed
        : 51000; // confirmed via Wireshark (2026-08-01) — DTV video lands here, not 51001

    using var socket = await context.WebSockets.AcceptWebSocketAsync();
    try
    {
        await new RDS_WebApp.Services.CameraVideoStreamer().RunClientAsync(port, socket, context.RequestAborted);
    }
    catch (Exception ex)
    {
        Console.WriteLine($"[VideoStream] error: {ex.Message}");
    }
});

app.MapStaticAssets();
app.MapRazorComponents<App>()
    .AddInteractiveServerRenderMode();
// דוגמה ל-GET: מחזיר את מצב המערכת ב-JSON
/*app.MapGet("/api/status", (SystemController service) => {
    return Results.Ok(new
    {
        State = service.GetStatus().ToString(),    //CurrentState.ToString(),
        LastUpdate = DateTime.Now
    });  
});

app.MapGet("/api/range", ([FromQuery] double el, SystemControlService service) => {
    service.InjectRangeFault(el);
    return Results.Ok();
});
app.MapPost("/api/inject-fault", (SystemControlService service) => {
    service.UpdateState(SystemState.FAULT, "API: Remote Fault Injection triggered");
    return Results.Accepted();
});*/
//sbuilder.Services.AddControllers(); // בתוך ה-Services
app.MapControllers();              // לפני ה-Run
app.UseStaticFiles();

app.MapRazorComponents<App>().AddInteractiveServerRenderMode();
app.Run();
