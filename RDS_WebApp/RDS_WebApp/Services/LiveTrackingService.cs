using SixLabors.ImageSharp;

namespace RDS_WebApp.Services
{
    // T1 (camera lock) continuation: SetTrack (StkDirectCameraService) does the
    // initial lock onto the crosshair, but the camera's own onboard tracker is
    // primitive and drops within ~1 second even on a high-contrast target (see
    // [[project-state]] memory, 2026-08-11) — confirmed not just our bug by
    // matching the vendor's own STK300cpp demo byte-for-byte. This service
    // takes over from there: polls the live H.265 JPEG feed and re-detects the
    // target with the existing YOLO model, continuously updating deltaAz/deltaEl
    // and the crosshair (via DetectionResultService) so "locked" stays accurate
    // as the target moves in frame.
    //
    // Deliberately does NOT send any rate/gimbal commands to physically move the
    // camera — confirmed explicitly (2026-08-11): "לא מה שמתבקש, צריך רק
    // להינעל על המטרה" (not what's required, it only needs to lock onto the
    // target). This is a software/visual lock only.
    //
    // Dedicated background Thread, not Task.Run/ThreadPool — established
    // project convention (shared ThreadPool starvation has caused real
    // operational disconnects before, see FCC comms code).
    public class LiveTrackingService
    {
        private readonly ImageDetectorService _detector;
        private readonly DetectionResultService _detection;

        private const float CameraHFov = 25.0f;  // degrees — matches Home.razor's CAMERA_HFOV
        private const float CameraVFov = 19.0f;
        private const int LoopIntervalMs = 1500;        // ~0.67Hz (2026-08-12: was 500ms/~2Hz, before that 300ms/~3Hz — RDS_WebApp.exe measured at 36.5% CPU (of 44% system total) while locked, correlated with real FCC↔ISM ACK-timeout COMM_LOSS on the actual weapon system. This process's CPU load competing with FCC_App's own real-time-ish comm needs is a genuine safety concern, not just a performance nuisance — slowed down hard as the first mitigation attempt.)
        private const int MaxMissedCycles = 4;           // ~6s of no matching detection before auto-unlock — kept roughly the same wall-clock budget after LoopIntervalMs tripled
        private const double MaxJumpFraction = 0.25;      // nearest-neighbor gate, as a fraction of frame width

        private volatile bool _active;
        private Thread? _thread;
        private bool _haveLastPos;
        private double _lastX, _lastY;
        private int _missedCycles;
        private byte[]? _lastProcessedFrame;

        public bool IsActive => _active;

        public LiveTrackingService(ImageDetectorService detector, DetectionResultService detection)
        {
            _detector = detector;
            _detection = detection;
        }

        public void Start()
        {
            if (_active) return;
            _active = true;
            _haveLastPos = false;
            _missedCycles = 0;
            _lastProcessedFrame = null;
            Console.WriteLine("[LiveTracking] Start()");
            _thread = new Thread(RunLoop) { IsBackground = true, Name = "LiveTracking" };
            _thread.Start();
        }

        public void Stop()
        {
            Console.WriteLine("[LiveTracking] Stop()");
            _active = false;
            _detection.ClearTarget();
        }

        private void RunLoop()
        {
            int noFrameLogCounter = 0;
            while (_active)
            {
                var frame = CameraVideoStreamer.GetLatestH265Jpeg();
                if (frame is null || ReferenceEquals(frame, _lastProcessedFrame))
                {
                    if (frame is null && ++noFrameLogCounter % 20 == 1)
                        Console.WriteLine("[LiveTracking] no H.265 JPEG frame available yet (still waiting)");
                    Thread.Sleep(50);
                    continue;
                }
                _lastProcessedFrame = frame;

                DetectionResult result;
                try
                {
                    result = _detector.DetectPeopleInBytes(frame, contrast: 1.0f);
                }
                catch (Exception ex)
                {
                    // Bad/partial JPEG frame, or transient decode issue — skip this
                    // cycle rather than tearing down the whole loop over one frame.
                    Console.WriteLine($"[LiveTracking] DetectPeopleInBytes threw: {ex.GetType().Name}: {ex.Message}");
                    Thread.Sleep(LoopIntervalMs);
                    continue;
                }

                var best = PickNearest(result.Boxes, result.OriginalWidth, result.OriginalHeight);
                Console.WriteLine($"[LiveTracking] frame {result.OriginalWidth}x{result.OriginalHeight}, {result.Boxes.Count} box(es), match={(best is null ? "none" : "yes")}, missedCycles={_missedCycles}");
                if (best is null)
                {
                    _missedCycles++;
                    if (_missedCycles >= MaxMissedCycles)
                    {
                        Console.WriteLine("[LiveTracking] max missed cycles reached, auto-unlocking");
                        Stop();
                        break;
                    }
                }
                else
                {
                    _missedCycles = 0;
                    double centerX = best.Value.X + best.Value.Width  / 2.0;
                    double centerY = best.Value.Y + best.Value.Height / 2.0;
                    _lastX = centerX;
                    _lastY = centerY;
                    _haveLastPos = true;

                    double deltaAz =  (centerX - result.OriginalWidth  / 2.0) / (result.OriginalWidth  / 2.0) * (CameraHFov / 2.0);
                    double deltaEl = -(centerY - result.OriginalHeight / 2.0) / (result.OriginalHeight / 2.0) * (CameraVFov / 2.0);
                    double svgX = centerX / result.OriginalWidth  * 800.0;
                    double svgY = centerY / result.OriginalHeight * 500.0;
                    Console.WriteLine($"[LiveTracking] picked box=({best.Value.X:F1},{best.Value.Y:F1},{best.Value.Width:F1}x{best.Value.Height:F1}) center=({centerX:F1},{centerY:F1}) -> svg=({svgX:F1},{svgY:F1})");

                  _detection.SetTarget(Convert.ToBase64String(result.AnnotatedImage), deltaAz, deltaEl, svgX, svgY);
                  
                }

                Thread.Sleep(LoopIntervalMs);
            }
        }

        // Picks the detected box closest to the last known target position (or
        // frame center, on the first cycle right after SetTrack) — not simply
        // the highest-confidence box. A per-frame max-confidence pick would let
        // the "lock" jump to an unrelated object the moment it scores higher
        // than the real target. Rejects a match that's implausibly far away
        // (MaxJumpFraction) rather than snapping onto it — that cycle counts as
        // a miss instead.
        private RectangleF? PickNearest(List<RectangleF> boxes, int width, int height)
        {
            if (boxes.Count == 0) return null;

            double refX = _haveLastPos ? _lastX : width  / 2.0;
            double refY = _haveLastPos ? _lastY : height / 2.0;
            double maxDist = MaxJumpFraction * width;

            RectangleF? best = null;
            double bestDist = double.MaxValue;
            foreach (var b in boxes)
            {
                double cx = b.X + b.Width  / 2.0;
                double cy = b.Y + b.Height / 2.0;
                double dist = Math.Sqrt((cx - refX) * (cx - refX) + (cy - refY) * (cy - refY));
                if (dist < bestDist) { bestDist = dist; best = b; }
            }

            return bestDist <= maxDist ? best : null;
        }
    }
}
