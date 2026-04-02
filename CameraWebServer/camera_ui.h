#ifndef CAMERA_UI_H
#define CAMERA_UI_H

static const char CAMERA_APP_HTML[] = R"HTML(
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
  <title>ESP32 Camera</title>
  <style>
    :root {
      --bg: #0b1117;
      --bg-2: #141d27;
      --card: rgba(18, 28, 39, 0.82);
      --text: #ecf2f9;
      --muted: #9fb4c8;
      --accent: #28c9a7;
      --accent-soft: rgba(40, 201, 167, 0.16);
      --danger: #ff5d73;
      --ring: rgba(255, 255, 255, 0.22);
    }

    * { box-sizing: border-box; }

    body {
      margin: 0;
      color: var(--text);
      font-family: "Trebuchet MS", "Segoe UI", sans-serif;
      background:
        radial-gradient(circle at 20% 0%, #1f3142 0, transparent 55%),
        radial-gradient(circle at 95% 90%, #0c5244 0, transparent 45%),
        linear-gradient(150deg, var(--bg), var(--bg-2));
      min-height: 100vh;
      display: grid;
      place-items: center;
      padding: 12px;
    }

    .shell {
      width: min(1080px, 100%);
      display: grid;
      gap: 12px;
      grid-template-columns: 1.15fr 0.85fr;
    }

    .panel {
      background: var(--card);
      border: 1px solid rgba(255, 255, 255, 0.08);
      border-radius: 20px;
      backdrop-filter: blur(8px);
      box-shadow: 0 18px 40px rgba(0, 0, 0, 0.35);
    }

    .camera {
      padding: 14px;
      position: relative;
      overflow: hidden;
    }

    .topbar {
      display: flex;
      justify-content: space-between;
      align-items: center;
      margin-bottom: 10px;
      font-size: 14px;
    }

    .badge {
      border: 1px solid var(--ring);
      background: rgba(255, 255, 255, 0.08);
      color: var(--muted);
      padding: 4px 10px;
      border-radius: 999px;
    }

    .preview {
      border-radius: 16px;
      overflow: hidden;
      border: 1px solid rgba(255, 255, 255, 0.15);
      min-height: 320px;
      background: #0a0e12;
      display: grid;
      place-items: center;
      position: relative;
    }

    #stream {
      width: 100%;
      height: 100%;
      object-fit: cover;
      display: block;
    }

    .hint {
      position: absolute;
      color: var(--muted);
      letter-spacing: 0.02em;
      text-transform: uppercase;
      font-size: 12px;
      pointer-events: none;
    }

    .controls {
      margin-top: 12px;
      display: grid;
      grid-template-columns: 1fr auto 1fr;
      align-items: center;
      gap: 10px;
    }

    .row {
      display: flex;
      gap: 8px;
      align-items: center;
      flex-wrap: wrap;
    }

    button {
      border: 1px solid rgba(255, 255, 255, 0.12);
      border-radius: 12px;
      color: var(--text);
      background: rgba(255, 255, 255, 0.08);
      padding: 8px 12px;
      cursor: pointer;
      transition: transform 120ms ease, background-color 120ms ease;
    }

    button:hover { transform: translateY(-1px); }
    button:active { transform: translateY(1px) scale(0.99); }
    .primary { background: var(--accent-soft); border-color: rgba(40, 201, 167, 0.55); }
    .danger { background: rgba(255, 93, 115, 0.15); border-color: rgba(255, 93, 115, 0.5); }

    .shutter {
      width: 78px;
      height: 78px;
      border-radius: 50%;
      background: white;
      border: 6px solid rgba(255, 255, 255, 0.4);
      box-shadow: 0 0 0 7px rgba(0, 0, 0, 0.25) inset;
      justify-self: center;
    }

    .settings {
      padding: 14px;
      display: grid;
      gap: 10px;
      align-content: start;
    }

    .settings h2 {
      margin: 0 0 6px;
      font-size: 18px;
      font-weight: 700;
    }

    .field {
      display: grid;
      gap: 5px;
    }

    .line {
      display: flex;
      justify-content: space-between;
      align-items: center;
      font-size: 13px;
      color: var(--muted);
    }

    select, input[type="range"] {
      width: 100%;
      accent-color: var(--accent);
    }

    .toggle-grid {
      display: grid;
      gap: 8px;
      grid-template-columns: repeat(2, minmax(0, 1fr));
    }

    label.toggle {
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 8px;
      border: 1px solid rgba(255, 255, 255, 0.1);
      background: rgba(255, 255, 255, 0.04);
      border-radius: 10px;
      padding: 7px 8px;
      font-size: 13px;
      color: var(--muted);
    }

    #snapshot {
      width: 100%;
      min-height: 120px;
      border-radius: 12px;
      border: 1px dashed rgba(255, 255, 255, 0.2);
      display: grid;
      place-items: center;
      color: var(--muted);
      overflow: hidden;
      background: rgba(0, 0, 0, 0.2);
      text-align: center;
      font-size: 13px;
      padding: 6px;
    }

    #snapshot img {
      width: 100%;
      height: 100%;
      object-fit: cover;
      display: block;
    }

    @media (max-width: 900px) {
      .shell { grid-template-columns: 1fr; }
      .controls { grid-template-columns: 1fr; justify-items: center; }
      .row { justify-content: center; }
      .preview { min-height: 250px; }
    }
  </style>
</head>
<body>
  <main class="shell">
    <section class="panel camera">
      <div class="topbar">
        <strong>ESP32 Camera</strong>
        <span id="state" class="badge">Loading</span>
      </div>
      <div class="preview">
        <img id="stream" alt="Camera stream">
        <div id="hint" class="hint">Tap Start Live</div>
      </div>
      <div class="controls">
        <div class="row">
          <button id="startBtn" class="primary" type="button">Start Live</button>
          <button id="stopBtn" class="danger" type="button">Stop</button>
        </div>
        <button id="snapBtn" class="shutter" type="button" aria-label="Capture"></button>
        <div class="row" style="justify-content:flex-end;">
          <button id="saveBtn" type="button">Download Last</button>
        </div>
      </div>
    </section>

    <aside class="panel settings">
      <h2>Quick Settings</h2>

      <div class="field">
        <div class="line"><span>Resolution</span><span id="resVal">-</span></div>
        <select id="framesize">
          <option value="10">UXGA 1600x1200</option>
          <option value="9">SXGA 1280x1024</option>
          <option value="8">XGA 1024x768</option>
          <option value="7">SVGA 800x600</option>
          <option value="6">VGA 640x480</option>
          <option value="5">CIF 400x296</option>
          <option value="4">QVGA 320x240</option>
          <option value="3">HQVGA 240x176</option>
          <option value="0">QQVGA 160x120</option>
        </select>
      </div>

      <div class="field">
        <div class="line"><span>Quality</span><span id="qualityOut">-</span></div>
        <input id="quality" type="range" min="10" max="63" step="1">
      </div>

      <div class="field">
        <div class="line"><span>Brightness</span><span id="brightnessOut">-</span></div>
        <input id="brightness" type="range" min="-2" max="2" step="1">
      </div>

      <div class="field">
        <div class="line"><span>Contrast</span><span id="contrastOut">-</span></div>
        <input id="contrast" type="range" min="-2" max="2" step="1">
      </div>

      <div class="field">
        <div class="line"><span>Saturation</span><span id="saturationOut">-</span></div>
        <input id="saturation" type="range" min="-2" max="2" step="1">
      </div>

      <div id="flashWrap" class="field">
        <div class="line"><span>Flash</span><span id="flashOut">-</span></div>
        <input id="led_intensity" type="range" min="0" max="255" step="1">
      </div>

      <div class="toggle-grid">
        <label class="toggle">Auto White Balance <input id="awb" type="checkbox"></label>
        <label class="toggle">Auto Gain <input id="agc" type="checkbox"></label>
        <label class="toggle">Auto Exposure <input id="aec" type="checkbox"></label>
        <label class="toggle">Vertical Flip <input id="vflip" type="checkbox"></label>
        <label class="toggle">Mirror <input id="hmirror" type="checkbox"></label>
        <label class="toggle">Color Bar <input id="colorbar" type="checkbox"></label>
      </div>

      <div id="snapshot">No snapshot yet</div>
    </aside>
  </main>

  <script>
    const stateEl = document.getElementById('state');
    const hintEl = document.getElementById('hint');
    const streamEl = document.getElementById('stream');
    const snapshotEl = document.getElementById('snapshot');
    const saveBtn = document.getElementById('saveBtn');
    let isStreaming = false;
    let lastBlobUrl = '';

    const fields = {
      framesize: document.getElementById('framesize'),
      quality: document.getElementById('quality'),
      brightness: document.getElementById('brightness'),
      contrast: document.getElementById('contrast'),
      saturation: document.getElementById('saturation'),
      awb: document.getElementById('awb'),
      agc: document.getElementById('agc'),
      aec: document.getElementById('aec'),
      vflip: document.getElementById('vflip'),
      hmirror: document.getElementById('hmirror'),
      colorbar: document.getElementById('colorbar'),
      led_intensity: document.getElementById('led_intensity')
    };

    const out = {
      framesize: document.getElementById('resVal'),
      quality: document.getElementById('qualityOut'),
      brightness: document.getElementById('brightnessOut'),
      contrast: document.getElementById('contrastOut'),
      saturation: document.getElementById('saturationOut'),
      led_intensity: document.getElementById('flashOut')
    };

    const frameLabels = {
      10: 'UXGA', 9: 'SXGA', 8: 'XGA', 7: 'SVGA', 6: 'VGA', 5: 'CIF', 4: 'QVGA', 3: 'HQVGA', 0: 'QQVGA'
    };

    async function sendControl(name, value) {
      await fetch(`/control?var=${encodeURIComponent(name)}&val=${encodeURIComponent(value)}`);
    }

    function updateReadouts() {
      out.framesize.textContent = frameLabels[fields.framesize.value] || fields.framesize.value;
      out.quality.textContent = fields.quality.value;
      out.brightness.textContent = fields.brightness.value;
      out.contrast.textContent = fields.contrast.value;
      out.saturation.textContent = fields.saturation.value;
      out.led_intensity.textContent = fields.led_intensity.value;
    }

    async function loadStatus() {
      try {
        const status = await fetch('/status').then((r) => r.json());
        for (const key of Object.keys(fields)) {
          const el = fields[key];
          if (!(key in status)) continue;
          if (el.type === 'checkbox') {
            el.checked = Number(status[key]) === 1;
          } else {
            el.value = String(status[key]);
          }
        }

        const hasFlash = Number(status.led_intensity) >= 0;
        document.getElementById('flashWrap').style.display = hasFlash ? '' : 'none';
        updateReadouts();
        stateEl.textContent = 'Ready';
      } catch (_err) {
        stateEl.textContent = 'Offline';
      }
    }

    function setStreaming(active) {
      isStreaming = active;
      stateEl.textContent = active ? 'Live' : 'Ready';
      hintEl.style.display = active ? 'none' : 'block';
    }

    function getStreamUrl() {
      const url = new URL(window.location.href);
      const currentPort = Number(url.port || (url.protocol === 'https:' ? 443 : 80));
      url.port = String(currentPort + 1);
      url.pathname = '/stream';
      url.search = '';
      url.hash = '';
      return url.toString();
    }

    function startStream() {
      streamEl.src = `${getStreamUrl()}?_cb=${Date.now()}`;
      setStreaming(true);
    }

    function stopStream() {
      streamEl.src = '';
      setStreaming(false);
    }

    async function snap() {
      try {
        const blob = await fetch('/capture').then((r) => r.blob());
        if (lastBlobUrl) URL.revokeObjectURL(lastBlobUrl);
        lastBlobUrl = URL.createObjectURL(blob);
        snapshotEl.innerHTML = `<img src="${lastBlobUrl}" alt="Snapshot">`;
      } catch (_err) {
        snapshotEl.textContent = 'Capture failed';
      }
    }

    function downloadLast() {
      if (!lastBlobUrl) return;
      const a = document.createElement('a');
      a.href = lastBlobUrl;
      a.download = `capture-${Date.now()}.jpg`;
      a.click();
    }

    document.getElementById('startBtn').addEventListener('click', startStream);
    document.getElementById('stopBtn').addEventListener('click', stopStream);
    document.getElementById('snapBtn').addEventListener('click', snap);
    saveBtn.addEventListener('click', downloadLast);

    ['framesize', 'quality', 'brightness', 'contrast', 'saturation', 'led_intensity'].forEach((k) => {
      fields[k].addEventListener('input', () => {
        updateReadouts();
        sendControl(k, fields[k].value).catch(() => {});
      });
    });

    ['awb', 'agc', 'aec', 'vflip', 'hmirror', 'colorbar'].forEach((k) => {
      fields[k].addEventListener('change', () => {
        sendControl(k, fields[k].checked ? 1 : 0).catch(() => {});
      });
    });

    loadStatus();
  </script>
</body>
</html>
)HTML";

#endif
