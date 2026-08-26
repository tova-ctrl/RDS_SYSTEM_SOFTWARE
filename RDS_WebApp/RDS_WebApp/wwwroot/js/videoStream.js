// Live camera video — connects to the camera's video stream via a local WebSocket
// relay (server-side: CameraVideoStreamer.cs). Two rendering paths, auto-selected
// by a small JSON control message the server sends first ({"codec":"h264"} or
// {"codec":"h265"}) — added 2026-08-02 because the camera genuinely needs to run
// H.265/HEVC sometimes (real drone requirement, not just a switchable default),
// and browsers can't play HEVC via MediaSource Extensions at all:
//   - h264: server forwards raw NAL units, JMuxer remuxes to fragmented MP4 for
//     the <video> element's MediaSource Extensions.
//   - h265: server decodes + re-encodes each frame to JPEG, sent as one binary
//     WebSocket message per frame; drawn directly onto a <canvas> (MJPEG-style).
//
// LATENCY: every binary message is prefixed by the server with an 8-byte
// big-endian send timestamp (ms since Unix epoch) — see SendBinary in
// CameraVideoStreamer.cs. Added 2026-08-05 as a software wire-to-glass latency
// measurement: this can only measure from the moment our server first has the
// frame to the moment the browser paints it (server processing + network +
// browser decode/paint) — NOT the camera's own internal capture-to-encode time,
// which no software on this side can see. Only valid when server/client clocks
// are close, true here since both normally run on/near the same machine.
let _jmuxer = null;
let _socket = null;
let _mode = null; // 'h264' | 'h265'
let _videoEl = null;
let _canvasEl = null;
let _canvasCtx = null;
let _latencyHud = null;
let _lastLatencyReportAt = 0;

window.startVideoStream = function (videoElementId, canvasElementId, wsPort) {
    window.stopVideoStream();

    _videoEl = document.getElementById(videoElementId);
    _canvasEl = document.getElementById(canvasElementId);
    _canvasCtx = _canvasEl ? _canvasEl.getContext('2d') : null;
    _latencyHud = document.getElementById('videoLatencyHud');
    _mode = null;

    const wsProtocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
    _socket = new WebSocket(`${wsProtocol}//${window.location.host}/stream-video?port=${wsPort}`);
    _socket.binaryType = 'arraybuffer';

    _socket.onmessage = function (event) {
        if (typeof event.data === 'string') {
            try {
                const msg = JSON.parse(event.data);
                if (msg.codec === 'h264' || msg.codec === 'h265') {
                    setMode(msg.codec, videoElementId);
                }
            } catch (e) {
                console.error('Bad video control message:', event.data, e);
            }
            return;
        }

        if (_mode === null) {
            // No control message seen yet (shouldn't normally happen — server sends
            // it before any frame data) — assume H.264, the far more common case.
            setMode('h264', videoElementId);
        }

        if (event.data.byteLength < 8) return;
        const view = new DataView(event.data);
        const sendTs = Number(view.getBigInt64(0, false)); // big-endian
        const payload = new Uint8Array(event.data, 8);
        const networkAndServerMs = Date.now() - sendTs;

        if (_mode === 'h265') {
            renderJpegFrame(payload, sendTs);
        } else if (_jmuxer) {
            _jmuxer.feed({ video: payload });
            // One WS message here isn't necessarily one displayed video frame (MSE
            // buffers/decodes asynchronously), so add the MSE backlog still sitting
            // in the buffer as an estimate of remaining decode/display delay, on
            // top of the network+server time this specific message just measured.
            reportLatency(networkAndServerMs + h264BufferBacklogMs());
        }
    };

    _socket.onerror = function (err) {
        console.error('Camera video stream WebSocket error:', err);
    };

    _socket.onclose = function () {
        console.log('Camera video stream WebSocket closed.');
    };
};

function setMode(mode, videoElementId) {
    if (_mode === mode) return;
    _mode = mode;

    if (mode === 'h264') {
        if (!_jmuxer) {
            _jmuxer = new JMuxer({
                node: videoElementId,
                mode: 'video',
                flushingTime: 0,
                clearBuffer: true,
                onError: function (err) { console.error('JMuxer error:', err); }
            });
        }
        if (_videoEl) _videoEl.style.display = '';
        if (_canvasEl) _canvasEl.style.display = 'none';
    } else {
        if (_videoEl) _videoEl.style.display = 'none';
        if (_canvasEl) _canvasEl.style.display = '';
    }
}

function h264BufferBacklogMs() {
    if (!_videoEl || _videoEl.buffered.length === 0) return 0;
    const end = _videoEl.buffered.end(_videoEl.buffered.length - 1);
    return Math.max(0, (end - _videoEl.currentTime) * 1000);
}

function renderJpegFrame(bytes, sendTs) {
    if (!_canvasCtx) return;
    // createImageBitmap decodes straight off the Blob — no object URL to leak/revoke.
    const blob = new Blob([bytes], { type: 'image/jpeg' });
    createImageBitmap(blob).then(function (bitmap) {
        if (_canvasEl.width !== bitmap.width) _canvasEl.width = bitmap.width;
        if (_canvasEl.height !== bitmap.height) _canvasEl.height = bitmap.height;
        _canvasCtx.drawImage(bitmap, 0, 0);
        bitmap.close();
        // Every JPEG message is exactly one displayed frame, so this is an exact
        // total (server send -> actually painted), not an estimate like H.264's.
        reportLatency(Date.now() - sendTs);
    }).catch(function (e) {
        console.error('JPEG frame decode error:', e);
    });
}

function reportLatency(ms) {
    const now = performance.now();
    if (now - _lastLatencyReportAt < 250) return; // throttle HUD updates to 4/sec
    _lastLatencyReportAt = now;
    if (_latencyHud) _latencyHud.textContent = 'LAT: ~' + Math.max(0, Math.round(ms)) + 'ms';
}

window.stopVideoStream = function () {
    if (_socket) {
        _socket.close();
        _socket = null;
    }
    if (_jmuxer) {
        _jmuxer.destroy();
        _jmuxer = null;
    }
    _mode = null;
    _videoEl = null;
    _canvasEl = null;
    _canvasCtx = null;
    _latencyHud = null;
};
