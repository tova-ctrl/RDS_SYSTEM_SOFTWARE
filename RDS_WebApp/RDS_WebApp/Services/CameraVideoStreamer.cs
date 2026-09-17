using System.Buffers.Binary;
using System.Net.WebSockets;
using System.Runtime.InteropServices;
using System.Text;
using FFmpeg.AutoGen;

namespace RDS_WebApp.Services
{
    // Reads the camera's live video — pushed directly to this PC's own address, not
    // through the FCC (see [[project-state]] memory: avoiding an extra relay hop's
    // latency) — and fans it out to every connected WebSocket client.
    //
    // TWO PARALLEL PATHS, auto-selected by the stream's actual codec_id (2026-08-02):
    //   - H.264: demux only, forward raw Annex-B NAL units, browser's own JMuxer/MSE
    //     decodes it — zero server-side decode cost.
    //   - H.265/HEVC: browsers can't play HEVC via MSE at all, so there's no way
    //     around decoding server-side. Decode (hardware first: hevc_cuvid/hevc_qsv,
    //     software fallback) -> cheap YUV->YUV pixel format conversion -> encode to
    //     JPEG with FFmpeg's own mjpeg encoder (kept entirely in native/unmanaged
    //     code, not routed through ImageSharp, to avoid extra managed<->native
    //     copies) -> broadcast JPEG bytes. Client renders as MJPEG-over-WebSocket
    //     (canvas + createImageBitmap), not JMuxer.
    // The camera flips between H.264 and H.265 depending on its own settings (seen
    // happen unprompted mid-session) — this is a real drone requirement, not
    // something we can just force to H.264 and forget (see conversation 2026-08-02).
    //
    // Confirmed via Wireshark (2026-08-01): the camera pushes raw UDP MPEG-TS (not
    // RTSP).
    //
    // ONE shared UDP reader per port, not one per client — a browser refresh/
    // reconnect used to try opening a second bind on the same already-bound port
    // and fail with WSAEADDRINUSE (-10048), since only one socket can receive a
    // given UDP unicast port at a time, even from the same process.
    public class CameraVideoStreamer
    {
        private static bool _ffmpegInitialized = false;
        private static readonly object _initLock = new();

        private static void EnsureFFmpegRegistered()
        {
            lock (_initLock)
            {
                if (_ffmpegInitialized) return;
                ffmpeg.RootPath = AppContext.BaseDirectory;
                DynamicallyLoadedBindings.Initialize();
                _ffmpegInitialized = true;
            }
        }

        private enum StreamMode { Unknown, H264, H265Jpeg }

        private class Subscriber
        {
            public required WebSocket Socket;
            public bool Started; // H.264 path only — has this subscriber received its first SPS+PPS+IDR yet
        }

        private static readonly object _readerLock = new();
        private static readonly List<Subscriber> _subscribers = new();
        private static bool _readerStarted;
        private static StreamMode _mode = StreamMode.Unknown;

        // Latest decoded frame (H.265 path only — see class doc, H.264 is raw
        // passthrough with no server-side decode). Exposed for LiveTrackingService
        // to poll at its own throttled rate — deliberately last-frame-wins, not a
        // queue, so a slow consumer (ONNX inference) always works on the freshest
        // frame instead of falling behind and processing stale ones.
        private static byte[]? _latestH265Jpeg;
        private static readonly object _latestFrameLock = new();

        public static byte[]? GetLatestH265Jpeg()
        {
            lock (_latestFrameLock) { return _latestH265Jpeg; }
        }

        // Cached at the reader level (not per-subscriber) — many H.264 encoders send
        // SPS/PPS only ONCE at the true start of the stream, not before every IDR. A
        // client connecting after that point would otherwise wait forever for a
        // fresh SPS+PPS+IDR combo that never recurs. Confirmed this is what was
        // happening (2026-08-01): the WebSocket connected fine but zero messages
        // ever sent, because our reader joined the already-running MPEG-TS stream
        // well after its one-time SPS/PPS.
        private static byte[]? _cachedSps;
        private static byte[]? _cachedPps;

        // Registers this client as a subscriber to the shared reader for udpPort
        // (starting the reader if it isn't already running), then blocks until the
        // client's WebSocket closes or is cancelled.
        public async Task RunClientAsync(int udpPort, WebSocket socket, CancellationToken clientCt)
        {
            EnsureFFmpegRegistered();

            var sub = new Subscriber { Socket = socket };
            StreamMode modeAtJoinTime;
            lock (_readerLock)
            {
                _subscribers.Add(sub);
                modeAtJoinTime = _mode;
                if (!_readerStarted)
                {
                    _readerStarted = true;
                    // Deliberately a raw background Thread, NOT Task.Run/ThreadPool.
                    // This loop runs for the app's entire lifetime (see the "never
                    // tear down" note below) doing blocking reads + per-packet work
                    // (NAL scanning, or full decode+encode for H.265) — parking that
                    // on the shared .NET ThreadPool starved it of worker threads,
                    // which in turn delayed the FCC TCP link's own async I/O
                    // continuations (EthernetService) enough to look like
                    // dropped/disconnected Ethernet comms. Confirmed 2026-08-02.
                    // A dedicated OS thread, at BelowNormal priority, can't do that —
                    // it never competes with the ThreadPool for a worker slot.
                    var thread = new Thread(() => ReaderLoop(udpPort))
                    {
                        IsBackground = true,
                        Priority = ThreadPriority.BelowNormal,
                        Name = "CameraVideoReader"
                    };
                    thread.Start();
                }
            }

            // If the reader already knows the codec (this client joined after an
            // earlier one), tell it immediately so it picks the right rendering path
            // without waiting for the next mode-change broadcast (there may not be one).
            if (modeAtJoinTime != StreamMode.Unknown)
                await SendModeAsync(socket, modeAtJoinTime).ConfigureAwait(false);

            try
            {
                while (socket.State == WebSocketState.Open && !clientCt.IsCancellationRequested)
                    await Task.Delay(500, clientCt).ConfigureAwait(false);
            }
            catch (OperationCanceledException) { /* client disconnected */ }
            finally
            {
                // Deliberately NOT tearing down the reader when subscriber count hits
                // zero. av_read_frame blocks synchronously and only notices
                // cancellation between reads — Blazor Server routinely opens a second
                // WebSocket moments after the first closes (prerender circuit, then
                // the real interactive one), and a still-unwinding old reader would
                // still hold the UDP socket when the new one tries to bind, causing
                // WSAEADDRINUSE (-10048). Confirmed this exact race (2026-08-01).
                // This is a single always-on home screen — keeping one idle reader
                // alive between viewers is a non-issue.
                lock (_readerLock) { _subscribers.Remove(sub); }
            }
        }

        private static async Task SendModeAsync(WebSocket socket, StreamMode mode)
        {
            try
            {
                if (socket.State != WebSocketState.Open) return;
                string json = mode == StreamMode.H264 ? "{\"codec\":\"h264\"}" : "{\"codec\":\"h265\"}";
                var bytes = Encoding.UTF8.GetBytes(json);
                await socket.SendAsync(new ArraySegment<byte>(bytes), WebSocketMessageType.Text, true, CancellationToken.None).ConfigureAwait(false);
            }
            catch { /* client may already be gone */ }
        }

        private static void BroadcastModeToAllSubscribers(StreamMode mode)
        {
            List<Subscriber> snapshot;
            lock (_readerLock) { snapshot = _subscribers.ToList(); }
            foreach (var sub in snapshot)
                _ = SendModeAsync(sub.Socket, mode);
        }

        // Runs on its own dedicated background OS thread (see RunClientAsync) — never
        // torn down once started, so there's no cancellation token; av_read_frame
        // blocks synchronously, which is normal/expected for FFmpeg's I/O model.
        //
        // Outer retry loop added 2026-08-12: a single corrupt UDP packet (PES
        // packet size mismatch / stream corrupt) makes av_read_frame return an
        // error, which used to permanently end this whole thread — and since
        // _readerStarted is never reset, no future WebSocket subscriber could
        // ever restart it either, so the ONLY recovery was restarting the whole
        // app. Confirmed hit in the field (2026-08-12), unrelated to J1/J2/T1 —
        // a pre-existing gap in a video pipeline that otherwise has no
        // reconnect story at all. Now retries indefinitely with a short
        // backoff, matching this reader's existing "never torn down, lives for
        // the app's lifetime" design.
        private static unsafe void ReaderLoop(int udpPort)
        {
            while (true)
            {
                try
                {
                    RunOneReadAttempt(udpPort);
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"[VideoStream] Reader attempt threw: {ex.GetType().Name}: {ex.Message}");
                }

                Console.WriteLine("[VideoStream] Reconnecting in 2s...");
                Thread.Sleep(2000);
            }
        }

        private static unsafe void RunOneReadAttempt(int udpPort)
        {
            _cachedSps = null;
            _cachedPps = null;
            _mode = StreamMode.Unknown;
            // Don't let a stale pre-disconnect frame keep looking "live" to
            // LiveTrackingService (or anything else polling it) during an outage.
            lock (_latestFrameLock) { _latestH265Jpeg = null; }

            AVFormatContext* fmtCtx = ffmpeg.avformat_alloc_context();
           // string url = $"udp://@:{udpPort}";
            string url = "udp://1.1.1.2:51000";
            //==Tova ===2.8.26 ==============================
            AVDictionary* options = null;

            // הגדלת באפר הקליטה של ה-Socket (בבייטים) כדי למנוע נפילת חבילות UDP
           ffmpeg.av_dict_set(&options, "buffer_size", "10485760", 0); // 10MB Buffer

            // 2026-08-23: buffer_size above is only the OS socket buffer — FFmpeg's
            // udp:// protocol keeps its OWN separate internal circular buffer
            // (fifo_size, default ~28672 packets), which was never set. The
            // "Circular buffer overrun" log line literally names this option as
            // the fix ("To avoid, increase fifo_size URL option"), confirmed
            // recurring in production (repeated overrun -> corrupt packets ->
            // av_read_frame(-5) -> full 2s reconnect cycles). Bumped well above
            // default for this stream's bursty HEVC traffic; overrun_nonfatal
            // means a transient overrun drops the oldest buffered packets instead
            // of failing the whole read (same log line's second suggestion).
            // 2026-09-14: bumped 1,000,000 -> 4,000,000 — "PES packet size
            // mismatch"/"Packet corrupt" (the same failure pattern this fifo_size
            // fix was originally for) still recurring on the ISHLAT operator
            // console, observed correlating with a LiveTrackingService
            // auto-unlock event (max missed cycles) — consistent with the
            // console being under more CPU pressure than the original dev
            // machine, needing more buffer margin to absorb a stall.
            ffmpeg.av_dict_set(&options, "fifo_size", "4000000", 0);
            ffmpeg.av_dict_set(&options, "overrun_nonfatal", "1", 0);

            // הגדרות לצמצום שיהוי (Latency) בזמן הפתיחה
           ffmpeg.av_dict_set(&options, "probesize", "32", 0);
           ffmpeg.av_dict_set(&options, "analyzeduration", "0", 0);
            // הגדרת Timeout של 5 שניות (5,000,000 מיקרו-שניות) לחיבור ה-UDP
            ffmpeg.av_dict_set(&options, "timeout", "5000000", 0);
            // פתיחת הקלט
            int openResult = ffmpeg.avformat_open_input(&fmtCtx, url, null, &options);
            ffmpeg.av_dict_free(&options); // caller owns this dict regardless of success/failure — was only freed on the failure path before, which leaked on every successful (re)connect
            if (openResult < 0)
            {
                Console.WriteLine($"[VideoStream] avformat_open_input failed: {openResult}");
                return;
            }
            //===============================================

            try
            {
                if (ffmpeg.avformat_find_stream_info(fmtCtx, null) < 0)
                {
                    Console.WriteLine("[VideoStream] avformat_find_stream_info failed");
                    return;
                }

                int videoStreamIndex = -1;
                for (int i = 0; i < fmtCtx->nb_streams; i++)
                {
                    if (fmtCtx->streams[i]->codecpar->codec_type == AVMediaType.AVMEDIA_TYPE_VIDEO)
                    {
                        videoStreamIndex = i;
                        break;
                    }
                }
                if (videoStreamIndex == -1)
                {
                    Console.WriteLine("[VideoStream] No video stream found in UDP-TS input");
                    return;
                }

                var codecId = fmtCtx->streams[videoStreamIndex]->codecpar->codec_id;
                bool isHevc = codecId == AVCodecID.AV_CODEC_ID_HEVC;
                _mode = isHevc ? StreamMode.H265Jpeg : StreamMode.H264;
                Console.WriteLine($"[VideoStream] Detected codec: {(isHevc ? "H.265/HEVC (will decode+JPEG)" : "H.264 (raw NAL passthrough)")}");
                BroadcastModeToAllSubscribers(_mode);

                var pkt = ffmpeg.av_packet_alloc();
                try
                {
                    if (isHevc)
                        RunHevcDecodeLoop(fmtCtx, videoStreamIndex, pkt);
                    else
                        RunH264PassthroughLoop(fmtCtx, videoStreamIndex, pkt);
                }
                finally
                {
                    ffmpeg.av_packet_free(&pkt);
                }
            }
            finally
            {
                ffmpeg.avformat_close_input(&fmtCtx);
                Console.WriteLine("[VideoStream] Reader loop stopped.");
            }
        }

        // ── H.264 path — unchanged: demux only, forward raw NAL units ────────────
        private static unsafe void RunH264PassthroughLoop(AVFormatContext* fmtCtx, int videoStreamIndex, AVPacket* pkt)
        {
            Console.WriteLine("[VideoStream] H.264 passthrough loop started, entering av_read_frame...");
            while (true)
            {
                int readResult = ffmpeg.av_read_frame(fmtCtx, pkt);
                if (readResult < 0)
                {
                    Console.WriteLine($"[VideoStream] av_read_frame returned {readResult}, stopping.");
                    break;
                }

                if (pkt->stream_index == videoStreamIndex && pkt->size > 0)
                {
                    var data = new byte[pkt->size];
                    Marshal.Copy((IntPtr)pkt->data, data, 0, pkt->size);
                    BroadcastH264(data);
                }
                ffmpeg.av_packet_unref(pkt);
            }
        }

        // Fans one demuxed H.264 packet out to every subscriber. A subscriber that
        // hasn't started yet gets fed the first IDR it sees, with the reader's cached
        // SPS/PPS synthetically prepended — since the encoder likely won't send them
        // again on its own (see _cachedSps/_cachedPps comment above).
        private static void BroadcastH264(byte[] data)
        {
            var (sps, pps, hasIdr) = ScanNals(data);
            if (sps != null) _cachedSps = sps;
            if (pps != null) _cachedPps = pps;

            List<Subscriber> snapshot;
            lock (_readerLock) { snapshot = _subscribers.ToList(); }

            foreach (var sub in snapshot)
            {
                byte[] payload = data;

                if (!sub.Started)
                {
                    if (!hasIdr) continue; // wait for an IDR
                    if (_cachedSps == null || _cachedPps == null) continue; // no parameter sets seen yet at all

                    payload = new byte[_cachedSps.Length + _cachedPps.Length + data.Length];
                    Buffer.BlockCopy(_cachedSps, 0, payload, 0, _cachedSps.Length);
                    Buffer.BlockCopy(_cachedPps, 0, payload, _cachedSps.Length, _cachedPps.Length);
                    Buffer.BlockCopy(data, 0, payload, _cachedSps.Length + _cachedPps.Length, data.Length);
                    sub.Started = true;
                }

                SendBinary(sub, payload);
            }
        }

        // Single pass over the packet's Annex-B start codes (00 00 01 / 00 00 00 01),
        // extracting SPS(7)/PPS(8) NAL ranges and IDR(5) presence together — replaces
        // what used to be 2-3 redundant full-buffer scans per packet (ExtractNal x2 +
        // ContainsNalType), which added up under sustained frame rate and contributed
        // to the CPU load that starved the FCC TCP link's ThreadPool (2026-08-02).
        private static (byte[]? sps, byte[]? pps, bool hasIdr) ScanNals(byte[] data)
        {
            var starts = new List<int>();
            for (int i = 0; i + 2 < data.Length; i++)
                if (data[i] == 0 && data[i + 1] == 0 && data[i + 2] == 1)
                    starts.Add(i);

            byte[]? sps = null, pps = null;
            bool hasIdr = false;

            for (int s = 0; s < starts.Count; s++)
            {
                int nalByteIdx = starts[s] + 3;
                if (nalByteIdx >= data.Length) continue;
                int nalType = data[nalByteIdx] & 0x1F;
                int end = (s + 1 < starts.Count) ? starts[s + 1] : data.Length;

                switch (nalType)
                {
                    case 5: hasIdr = true; break;
                    case 7: sps = data[starts[s]..end]; break;
                    case 8: pps = data[starts[s]..end]; break;
                }
            }

            return (sps, pps, hasIdr);
        }

        // 2026-08-18: `avcodec_find_decoder_by_name("hevc_cuvid")` returns non-null as
        // long as FFmpeg was BUILT with NVDEC support — it says nothing about whether
        // THIS machine actually has a working NVIDIA GPU/driver. On a machine without
        // one (confirmed: the operator console's Windows 11 box, QBee still showed
        // video fine there since it doesn't share this decode path), hevc_cuvid is
        // "found" but `avcodec_open2` fails at runtime — and the old code just gave up
        // at that point instead of falling through to hevc_qsv/software, so this
        // machine got NO video at all. Isolated per-attempt so a found-but-unopenable
        // hardware decoder no longer blocks the software fallback.
        private static unsafe AVCodecContext* TryOpenHevcDecoder(AVCodec* decoder, AVFormatContext* fmtCtx, int videoStreamIndex, bool isCuvid)
        {
            AVCodecContext* ctx = ffmpeg.avcodec_alloc_context3(decoder);
            ffmpeg.avcodec_parameters_to_context(ctx, fmtCtx->streams[videoStreamIndex]->codecpar);

            // Low-latency tuning (2026-08-05): a diagnostic (packet-in to frame-out,
            // tracked via pts round-trip below) showed the decoder itself is the
            // likely place most of our extra latency vs QBee lives — hardware
            // decoders like hevc_cuvid/NVDEC hold several frames in an internal
            // pipeline for throughput by default, invisible to simple per-call
            // timing since the pipeline is already primed once steady-state.
            // AV_CODEC_FLAG_LOW_DELAY tells the decoder not to hold frames for
            // reordering beyond what's strictly required; "surfaces" (cuvid-only)
            // caps how many decode surfaces NVDEC keeps in flight — fewer means
            // less pipeline depth, at some cost to throughput headroom our light
            // per-frame CPU load (~14ms) can easily absorb.
            ctx->flags |= ffmpeg.AV_CODEC_FLAG_LOW_DELAY;

            AVDictionary* decoderOpts = null;
            if (isCuvid)
            {
                ffmpeg.av_dict_set(&decoderOpts, "surfaces", "4", 0);
            }

            if (ffmpeg.avcodec_open2(ctx, decoder, &decoderOpts) < 0)
            {
                ffmpeg.av_dict_free(&decoderOpts);
                ffmpeg.avcodec_free_context(&ctx);
                return null;
            }
            ffmpeg.av_dict_free(&decoderOpts);
            return ctx;
        }

        // ── H.265/HEVC path — decode (hardware first) -> YUV->YUV convert -> JPEG ──
        private static unsafe void RunHevcDecodeLoop(AVFormatContext* fmtCtx, int videoStreamIndex, AVPacket* pkt)
        {
            AVCodecContext* decCtx = null;
            string decoderName = "";

            AVCodec* decoder = ffmpeg.avcodec_find_decoder_by_name("hevc_cuvid");
            if (decoder != null)
            {
                decCtx = TryOpenHevcDecoder(decoder, fmtCtx, videoStreamIndex, isCuvid: true);
                decoderName = "hevc_cuvid (NVIDIA hardware)";
                if (decCtx == null)
                    Console.WriteLine("[VideoStream] hevc_cuvid found but avcodec_open2 failed (no NVIDIA GPU/driver on this machine?) — falling back.");
            }

            if (decCtx == null)
            {
                decoder = ffmpeg.avcodec_find_decoder_by_name("hevc_qsv");
                if (decoder != null)
                {
                    decCtx = TryOpenHevcDecoder(decoder, fmtCtx, videoStreamIndex, isCuvid: false);
                    decoderName = "hevc_qsv (Intel hardware)";
                    if (decCtx == null)
                        Console.WriteLine("[VideoStream] hevc_qsv found but avcodec_open2 failed — falling back to software.");
                }
            }

            if (decCtx == null)
            {
                decoder = ffmpeg.avcodec_find_decoder(AVCodecID.AV_CODEC_ID_HEVC);
                if (decoder != null)
                {
                    decCtx = TryOpenHevcDecoder(decoder, fmtCtx, videoStreamIndex, isCuvid: false);
                    decoderName = "software";
                }
            }

            if (decCtx == null)
            {
                Console.WriteLine("[VideoStream] No HEVC decoder could be opened (tried hardware + software) — no video will be available.");
                return;
            }
            Console.WriteLine($"[VideoStream] Using HEVC decoder: {decoderName}");

            AVCodec* jpegEncoder = ffmpeg.avcodec_find_encoder(AVCodecID.AV_CODEC_ID_MJPEG);
            if (jpegEncoder == null)
            {
                Console.WriteLine("[VideoStream] MJPEG encoder not available.");
                ffmpeg.avcodec_free_context(&decCtx);
                return;
            }

            AVFrame* decodedFrame = ffmpeg.av_frame_alloc();
            AVFrame* yuvFrame = ffmpeg.av_frame_alloc();
            AVPacket* jpegPkt = ffmpeg.av_packet_alloc();
            AVCodecContext* encCtx = null;
            SwsContext* swsCtx = null;
            long frameNumber = 0;

            try
            {
                while (true)
                {
                    int readResult = ffmpeg.av_read_frame(fmtCtx, pkt);
                    if (readResult < 0)
                    {
                        Console.WriteLine($"[VideoStream] av_read_frame returned {readResult}, stopping.");
                        break;
                    }

                    if (pkt->stream_index != videoStreamIndex)
                    {
                        // Non-video packets (PCR/PAT/PMT etc. in the MPEG-TS mux).
                        ffmpeg.av_packet_unref(pkt);
                        continue;
                    }

                    if (ffmpeg.avcodec_send_packet(decCtx, pkt) < 0)
                    {
                        ffmpeg.av_packet_unref(pkt);
                        continue;
                    }
                    ffmpeg.av_packet_unref(pkt);

                    while (ffmpeg.avcodec_receive_frame(decCtx, decodedFrame) == 0)
                    {
                        // Lazily create the encoder + scaler once we know the decoded
                        // frame's actual dimensions/pixel format (varies by which
                        // decoder — hardware vs software — actually got used).
                        if (encCtx == null)
                        {
                            encCtx = ffmpeg.avcodec_alloc_context3(jpegEncoder);
                            encCtx->width = decodedFrame->width;
                            encCtx->height = decodedFrame->height;
                            encCtx->pix_fmt = AVPixelFormat.AV_PIX_FMT_YUVJ420P;
                            encCtx->time_base = new AVRational { num = 1, den = 25 };
                            encCtx->flags |= ffmpeg.AV_CODEC_FLAG_QSCALE;
                            encCtx->global_quality = 5 * ffmpeg.FF_QP2LAMBDA; // ~good quality, small enough for WebSocket frames
                            if (ffmpeg.avcodec_open2(encCtx, jpegEncoder, null) < 0)
                            {
                                Console.WriteLine("[VideoStream] avcodec_open2 (mjpeg encoder) failed.");
                                return;
                            }

                            yuvFrame->format = (int)AVPixelFormat.AV_PIX_FMT_YUVJ420P;
                            yuvFrame->width = decodedFrame->width;
                            yuvFrame->height = decodedFrame->height;
                            ffmpeg.av_frame_get_buffer(yuvFrame, 32);

                            swsCtx = ffmpeg.sws_getContext(
                                decodedFrame->width, decodedFrame->height, (AVPixelFormat)decodedFrame->format,
                                decodedFrame->width, decodedFrame->height, AVPixelFormat.AV_PIX_FMT_YUVJ420P,
                                ffmpeg.SWS_BILINEAR, null, null, null);
                        }

                        ffmpeg.sws_scale(swsCtx, decodedFrame->data, decodedFrame->linesize, 0, decodedFrame->height,
                                          yuvFrame->data, yuvFrame->linesize);
                        yuvFrame->pts = frameNumber++;

                        if (ffmpeg.avcodec_send_frame(encCtx, yuvFrame) == 0)
                        {
                            while (ffmpeg.avcodec_receive_packet(encCtx, jpegPkt) == 0)
                            {
                                var jpegBytes = new byte[jpegPkt->size];
                                Marshal.Copy((IntPtr)jpegPkt->data, jpegBytes, 0, jpegPkt->size);
                                BroadcastJpeg(jpegBytes);
                                ffmpeg.av_packet_unref(jpegPkt);
                            }
                        }

                        ffmpeg.av_frame_unref(decodedFrame);
                    }
                }
            }
            finally
            {
                if (swsCtx != null) ffmpeg.sws_freeContext(swsCtx);
                if (encCtx != null) ffmpeg.avcodec_free_context(&encCtx);
                ffmpeg.av_frame_free(&decodedFrame);
                ffmpeg.av_frame_free(&yuvFrame);
                ffmpeg.av_packet_free(&jpegPkt);
                ffmpeg.avcodec_free_context(&decCtx);
            }
        }

        // Fans one encoded JPEG frame out to every subscriber (H.265 path). No
        // SPS/PPS/keyframe gating needed here — every message IS a complete,
        // independently-decodable image.
        private static void BroadcastJpeg(byte[] jpegBytes)
        {
            lock (_latestFrameLock) { _latestH265Jpeg = jpegBytes; }

            List<Subscriber> snapshot;
            lock (_readerLock) { snapshot = _subscribers.ToList(); }
            foreach (var sub in snapshot)
            {
                sub.Started = true; // no gating concept for JPEG frames — mark so state stays consistent
                SendBinary(sub, jpegBytes);
            }
        }

        private static void SendBinary(Subscriber sub, byte[] payload)
        {
            if (sub.Socket.State != WebSocketState.Open) return;

            // Prefix every frame with an 8-byte big-endian send timestamp (ms since
            // Unix epoch) so the client can compute wire-to-glass latency: our own
            // processing + network transit + the browser's own decode/paint time
            // (see videoStream.js). Only meaningful when server and client clocks
            // are close — true for this app's normal case (same machine, or same
            // LAN) but not corrected for skew if that ever stops being true.
            var framed = new byte[8 + payload.Length];
            BinaryPrimitives.WriteInt64BigEndian(framed, DateTimeOffset.UtcNow.ToUnixTimeMilliseconds());
            Buffer.BlockCopy(payload, 0, framed, 8, payload.Length);

            // Fire-and-forget — never let one slow/stuck client block the reader
            // loop or the other subscribers.
            _ = sub.Socket.SendAsync(new ArraySegment<byte>(framed), WebSocketMessageType.Binary, true, CancellationToken.None)
                .ContinueWith(t =>
                {
                    if (t.Exception != null)
                        Console.WriteLine($"[VideoStream] send error: {t.Exception.InnerException?.Message}");
                }, TaskContinuationOptions.OnlyOnFaulted);
        }
    }
}
