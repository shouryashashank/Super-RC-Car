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

    body.lock-scroll {
      overflow: hidden;
      height: 100dvh;
    }

    body.allow-scroll {
      overflow-y: auto;
      height: auto;
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
      height: 320px;
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
      width: 100%;
      grid-template-columns: 1fr auto 1fr;
      align-items: center;
      column-gap: 12px;
      row-gap: 8px;
    }

    .capture-row {
      display: flex;
      align-items: center;
      gap: 10px;
      justify-content: center;
      justify-self: center;
    }

    .control-left {
      justify-self: start;
    }

    .control-right {
      justify-self: end;
    }

    .lastshot-btn {
      width: 70px;
      height: 70px;
      border-radius: 50%;
      padding: 0;
      border: 1px solid rgba(255, 255, 255, 0.18);
      overflow: hidden;
      position: relative;
      display: grid;
      place-items: center;
      background: rgba(0, 0, 0, 0.35);
    }

    .lastshot-btn img {
      width: 100%;
      height: 100%;
      object-fit: cover;
      display: block;
    }

    .lastshot-empty {
      font-size: 10px;
      color: var(--muted);
      text-transform: uppercase;
      letter-spacing: 0.04em;
      text-align: center;
      line-height: 1.1;
    }

    .download-chip {
      position: absolute;
      right: 5px;
      bottom: 5px;
      width: 22px;
      height: 22px;
      border-radius: 999px;
      display: grid;
      place-items: center;
      font-size: 14px;
      background: rgba(0, 0, 0, 0.58);
      color: #eaffff;
      border: 1px solid rgba(255, 255, 255, 0.35);
    }

    .touch-btn {
      min-height: 42px;
      min-width: 86px;
      font-size: 14px;
      font-weight: 600;
    }

    .startstop-btn {
      width: 70px;
      height: 70px;
      min-width: 70px;
      min-height: 70px;
      border-radius: 50%;
      padding: 0;
      font-size: 13px;
      line-height: 1;
    }

    .row {
      display: flex;
      gap: 8px;
      align-items: center;
      flex-wrap: wrap;
      justify-content: center;
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

    .icon-btn {
      width: 54px;
      height: 54px;
      border-radius: 50%;
      padding: 0;
      font-size: 22px;
      display: grid;
      place-items: center;
      background: rgba(255, 255, 255, 0.08);
    }

    .icon-btn.is-on {
      border-color: rgba(255, 208, 94, 0.9);
      background: rgba(255, 208, 94, 0.2);
      box-shadow: 0 0 0 4px rgba(255, 208, 94, 0.15);
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

    .line-actions {
      display: flex;
      align-items: center;
      gap: 8px;
    }

    .mini-btn {
      min-height: 28px;
      min-width: 58px;
      padding: 4px 8px;
      font-size: 12px;
      border-radius: 8px;
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

    .settings-collapse {
      border: 1px solid rgba(255, 255, 255, 0.12);
      border-radius: 12px;
      background: rgba(255, 255, 255, 0.03);
      overflow: hidden;
    }

    .settings-collapse summary {
      list-style: none;
      cursor: pointer;
      user-select: none;
      padding: 10px 12px;
      font-size: 14px;
      font-weight: 700;
      color: var(--text);
      background: rgba(255, 255, 255, 0.04);
    }

    .settings-collapse summary::-webkit-details-marker {
      display: none;
    }

    .settings-body {
      padding: 12px;
      display: grid;
      gap: 10px;
    }

    .drive-mode {
      display: grid;
      gap: 8px;
    }

    .mode-toggle {
      display: grid;
      grid-template-columns: repeat(3, minmax(0, 1fr));
      gap: 8px;
    }

    .mode-toggle button.active {
      background: var(--accent-soft);
      border-color: rgba(40, 201, 167, 0.55);
    }

    .merged-grid {
      display: grid;
      grid-template-columns: repeat(2, minmax(0, 1fr));
      gap: 8px;
    }

    .merged-grid .motor-card {
      min-width: 0;
    }

    .joystick-well {
      position: relative;
      width: min(100%, clamp(210px, 22vw, 270px));
      aspect-ratio: 1 / 1;
      border-radius: 22px;
      border: 1px solid rgba(255, 255, 255, 0.12);
      background:
        radial-gradient(circle at 50% 50%, rgba(132, 226, 255, 0.16) 0%, rgba(132, 226, 255, 0.03) 44%, transparent 72%),
        linear-gradient(180deg, rgba(255, 255, 255, 0.06), rgba(255, 255, 255, 0.01));
      overflow: hidden;
      display: grid;
      place-items: center;
      justify-self: center;
      box-shadow: inset 0 2px 10px rgba(0, 0, 0, 0.35);
    }

    .joystick-guide-v,
    .joystick-guide-h {
      position: absolute;
      pointer-events: none;
      opacity: 0.38;
    }

    .joystick-guide-v {
      width: 4px;
      height: 78%;
      border-radius: 999px;
      background: linear-gradient(180deg, transparent 0%, rgba(43, 226, 197, 0.9) 50%, transparent 100%);
    }

    .joystick-guide-h {
      width: 78%;
      height: 4px;
      border-radius: 999px;
      background: linear-gradient(90deg, transparent 0%, rgba(43, 226, 197, 0.9) 50%, transparent 100%);
    }

    .joystick-input {
      position: absolute;
      z-index: 4;
      appearance: none;
      -webkit-appearance: none;
      background: transparent;
      touch-action: none;
      cursor: pointer;
    }

    .joystick-input::-webkit-slider-thumb {
      appearance: none;
      -webkit-appearance: none;
      width: 86px;
      height: 86px;
      border-radius: 50%;
      background: transparent;
      border: none;
    }

    .joystick-input::-moz-range-thumb {
      width: 86px;
      height: 86px;
      border-radius: 50%;
      background: transparent;
      border: none;
    }

    .joystick-input::-webkit-slider-runnable-track,
    .joystick-input::-moz-range-track {
      background: transparent;
      border: none;
    }

    .joystick-input-vertical {
      width: 72%;
      height: 72%;
      transform: rotate(-90deg);
    }

    .joystick-input-horizontal {
      width: 72%;
      height: 72%;
    }

    .stick-dot {
      width: 74px;
      height: 74px;
      border-radius: 50%;
      z-index: 2;
      pointer-events: none;
      background: radial-gradient(circle at 34% 28%, #d8f9ff 0%, #77e9ff 40%, #23c5df 100%);
      border: 2px solid rgba(255, 255, 255, 0.84);
      box-shadow:
        0 10px 20px rgba(0, 0, 0, 0.35),
        0 0 0 8px rgba(35, 197, 223, 0.12);
      transition: transform 60ms linear;
      display: grid;
      place-items: center;
    }

    .stick-dot::after {
      content: "";
      width: 24px;
      height: 24px;
      border-radius: 50%;
      background: rgba(4, 34, 41, 0.78);
    }

    .hidden {
      display: none !important;
    }

    .motor-panel {
      margin-top: 6px;
      padding-top: 8px;
      border-top: 1px solid rgba(255, 255, 255, 0.12);
      display: grid;
      gap: 8px;
    }

    .motor-grid {
      display: grid;
      grid-template-columns: repeat(2, minmax(0, 1fr));
      gap: 8px;
    }

    .motor-card {
      border: 1px solid rgba(255, 255, 255, 0.1);
      border-radius: 10px;
      padding: 8px;
      background: rgba(255, 255, 255, 0.03);
      display: grid;
      gap: 5px;
    }

    .motor-card .line {
      font-size: 12px;
    }

    .motor-actions {
      display: flex;
      gap: 8px;
      flex-wrap: wrap;
    }

    @media (max-width: 900px) {
      body {
        padding: 6px;
      }
      .shell {
        grid-template-columns: 1fr;
        grid-template-rows: auto 1fr;
        height: calc(100dvh - 12px);
        gap: 8px;
      }
      .camera,
      .settings {
        padding: 10px;
      }
      .controls {
        grid-template-columns: 1fr auto 1fr;
        width: 100%;
        gap: 8px;
      }
      .row { justify-content: center; }
      .preview {
        min-height: 350px;
        max-height: 350px;
      }
      #stream {
        object-fit: contain;
        background: #02060a;
      }
      .touch-btn {
        min-height: 40px;
        min-width: 82px;
        font-size: 13px;
      }
      .shutter {
        width: 58px;
        height: 58px;
      }
      .icon-btn {
        width: 42px;
        height: 42px;
        font-size: 18px;
      }
      #saveBtn {
        min-width: unset;
      }
      .lastshot-btn {
        width: 52px;
        height: 52px;
      }
      .startstop-btn {
        width: 52px;
        height: 52px;
        min-width: 52px;
        min-height: 52px;
        font-size: 11px;
      }
      .download-chip {
        width: 18px;
        height: 18px;
        font-size: 12px;
      }
      #snapshot {
        display: none;
      }
      .merged-grid {
        grid-template-columns: repeat(2, minmax(0, 1fr));
      }
      .joystick-well {
        width: min(100%, clamp(145px, 41vw, 180px));
      }
      .joystick-input-vertical,
      .joystick-input-horizontal {
        width: 112px;
        height: 112px;
      }
      .stick-dot {
        width: 52px;
        height: 52px;
      }
      .stick-dot::after {
        width: 18px;
        height: 18px;
      }
      .settings h2 {
        margin-bottom: 2px;
        font-size: 16px;
      }
      .motor-actions {
        gap: 6px;
      }
      .motor-actions .touch-btn {
        min-width: 72px;
        min-height: 36px;
        padding: 6px 8px;
      }
      .line {
        font-size: 12px;
      }
      .settings-collapse summary {
        padding: 8px 10px;
      }
      .settings-collapse {
        margin-top: 2px;
      }
      .settings {
        overflow: hidden;
      }
      .motor-panel {
        margin-top: 2px;
        padding-top: 6px;
      }
      .motor-card {
        padding: 6px;
      }
      .motor-grid {
        grid-template-columns: repeat(2, minmax(0, 1fr));
      }
      .drive-mode {
        gap: 6px;
      }
      .topbar {
        margin-bottom: 6px;
      }
      .controls {
        margin-top: 8px;
      }
      .camera {
        overflow: hidden;
      }
      .hint {
        font-size: 11px;
      }
      .badge {
        padding: 3px 8px;
      }
      .motor-card .line {
        font-size: 11px;
      }
      .mode-toggle {
        gap: 6px;
      }
      .mode-toggle .touch-btn {
        min-height: 38px;
      }
      .settings .line span:last-child {
        margin-left: 6px;
      }
      .motor-panel:last-child {
        display: none;
      }
      .camera .row {
        gap: 6px;
        justify-content: flex-start;
      }
      .camera .row .touch-btn {
        min-width: 78px;
      }
      .preview img {
        object-fit: contain;
      }
      .settings-collapse[open] {
        max-height: 42dvh;
        overflow-y: auto;
      }
      .settings-collapse:not([open]) {
        max-height: 42px;
      }
      .settings {
        gap: 6px;
      }
      .field {
        gap: 4px;
      }
      .toggle-grid {
        gap: 6px;
      }
      label.toggle {
        padding: 6px 7px;
      }
      .merged-grid,
      .motor-grid {
        gap: 6px;
      }
    }

    @media (min-width: 901px) and (max-width: 1100px) {
      .joystick-well {
        width: min(100%, clamp(210px, 30vw, 300px));
      }
      .joystick-input-vertical,
      .joystick-input-horizontal {
        width: 74%;
        height: 74%;
      }
      .stick-dot {
        width: 66px;
        height: 66px;
      }
      .stick-dot::after {
        width: 22px;
        height: 22px;
      }
    }
  </style>
</head>
<body>
  <main class="shell">
    <section class="panel camera">
      <div class="topbar">
        <strong>Super RC Car</strong>
        <span id="state" class="badge">Loading</span>
      </div>
      <div class="preview">
        <img id="stream" alt="Camera stream">
        <div id="hint" class="hint">Tap Start Live</div>
      </div>
      <div class="controls">
        <div class="row control-left">
          <button id="startStopBtn" class="primary startstop-btn" type="button">Start</button>
        </div>
        <div class="capture-row">
          <button id="snapBtn" class="shutter" type="button" aria-label="Capture"></button>
          <button id="flashToggle" class="icon-btn" type="button" title="Flash Off" aria-label="Flash Off"><span aria-hidden="true">&#128161;</span></button>
        </div>
        <button id="lastShotBtn" class="lastshot-btn control-right" type="button" title="Download last capture" aria-label="Download last capture">
          <span id="lastShotEmpty" class="lastshot-empty">Last</span>
          <img id="lastShotImg" alt="Last capture" style="display:none;">
          <span class="download-chip" aria-hidden="true">&#8681;</span>
        </button>
      </div>
    </section>

    <aside class="panel settings">
      <h2>Drive Controls</h2>

      <section class="motor-panel">
        <div class="drive-mode">
          <div class="line"><span>Control Mode</span><span id="driveModeOut">Merged (2 controls)</span></div>
          <div class="mode-toggle">
            <button id="modeMerged" class="active touch-btn" type="button">Car</button>
            <button id="modeTank" class="touch-btn" type="button">Tank</button>
            <button id="modeIndependent" class="touch-btn" type="button">4 Wheels</button>
          </div>
        </div>

        <div id="mergedControl" class="merged-grid">
          <div class="motor-card">
            <div class="line"><span>Throttle</span><span id="throttleOut">0</span></div>
            <div class="joystick-well">
              <input id="throttle" class="joystick-input joystick-input-vertical" type="range" min="-100" max="100" step="1" value="0">
              <div class="joystick-guide-v"></div>
              <div id="throttleStick" class="stick-dot"></div>
            </div>
          </div>
          <div class="motor-card">
            <div class="line"><span>Steering</span><span id="steeringOut">0</span></div>
            <div class="joystick-well">
              <input id="steering" class="joystick-input joystick-input-horizontal" type="range" min="-100" max="100" step="1" value="0">
              <div class="joystick-guide-h"></div>
              <div id="steeringStick" class="stick-dot"></div>
            </div>
          </div>
        </div>

        <div id="independentControl" class="motor-grid hidden">
          <div class="motor-card">
            <div class="line"><span>Front Left</span><span id="flOut">0</span></div>
            <input id="fl" type="range" min="-100" max="100" step="1" value="0">
          </div>
          <div class="motor-card">
            <div class="line"><span>Front Right</span><span id="frOut">0</span></div>
            <input id="fr" type="range" min="-100" max="100" step="1" value="0">
          </div>
          <div class="motor-card">
            <div class="line"><span>Rear Left</span><span id="rlOut">0</span></div>
            <input id="rl" type="range" min="-100" max="100" step="1" value="0">
          </div>
          <div class="motor-card">
            <div class="line"><span>Rear Right</span><span id="rrOut">0</span></div>
            <input id="rr" type="range" min="-100" max="100" step="1" value="0">
          </div>
        </div>

        <div id="tankControl" class="merged-grid hidden">
          <div class="motor-card">
            <div class="line"><span>Left Track</span><span id="tankLeftOut">0</span></div>
            <div class="joystick-well">
              <input id="tankLeft" class="joystick-input joystick-input-vertical" type="range" min="-100" max="100" step="1" value="0">
              <div class="joystick-guide-v"></div>
              <div id="tankLeftStick" class="stick-dot"></div>
            </div>
          </div>
          <div class="motor-card">
            <div class="line"><span>Right Track</span><span id="tankRightOut">0</span></div>
            <div class="joystick-well">
              <input id="tankRight" class="joystick-input joystick-input-vertical" type="range" min="-100" max="100" step="1" value="0">
              <div class="joystick-guide-v"></div>
              <div id="tankRightStick" class="stick-dot"></div>
            </div>
          </div>
        </div>

        <div class="line"><span>Computed Wheels</span><span id="wheelPreview">FL 0 | FR 0 | RL 0 | RR 0</span></div>

        <div class="motor-actions">
          <button id="motorSend" class="primary touch-btn" type="button">Send</button>
          <button id="motorCenter" class="touch-btn" type="button">Center</button>
          <button id="motorReverse" class="touch-btn" type="button">Reverse</button>
          <button id="motorStop" class="danger touch-btn" type="button">Motor Stop</button>
        </div>
      </section>

      <div id="snapshot">No snapshot yet</div>

      <details class="settings-collapse" id="cameraSettings">
        <summary>Camera Settings</summary>
        <div class="settings-body">
          <div class="field">
            <div class="line">
              <span>Stream Height</span>
              <div class="line-actions">
                <span id="previewHeightOut">320px</span>
                <button id="previewHeightReset" class="mini-btn" type="button">Reset</button>
              </div>
            </div>
            <input id="preview_height" type="range" min="200" max="700" step="10" value="320">
          </div>

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
            <div class="line"><span>Flash Level</span><span id="flashOut">-</span></div>
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
        </div>
      </details>

      <section class="motor-panel">
        <div class="line"><span>Tip</span><span>Use Merged mode for easy touch driving</span></div>
      </section>
    </aside>
  </main>

  <script>
    const stateEl = document.getElementById('state');
    const hintEl = document.getElementById('hint');
    const streamEl = document.getElementById('stream');
    const snapshotEl = document.getElementById('snapshot');
    const lastShotBtn = document.getElementById('lastShotBtn');
    const lastShotImg = document.getElementById('lastShotImg');
    const lastShotEmpty = document.getElementById('lastShotEmpty');
    const startStopBtn = document.getElementById('startStopBtn');
    let isStreaming = false;
    let lastBlobUrl = '';
    let motorSeq = 0;
    let motorDirty = false;
    let driveMode = 'merged';
    let flashEnabled = false;
    let hasFlash = true;
    let mergedDragActive = false;

    const fields = {
      preview_height: document.getElementById('preview_height'),
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

    const motors = {
      fl: document.getElementById('fl'),
      fr: document.getElementById('fr'),
      rl: document.getElementById('rl'),
      rr: document.getElementById('rr')
    };

    const merged = {
      throttle: document.getElementById('throttle'),
      steering: document.getElementById('steering')
    };

    const tank = {
      left: document.getElementById('tankLeft'),
      right: document.getElementById('tankRight')
    };

    const motorOut = {
      fl: document.getElementById('flOut'),
      fr: document.getElementById('frOut'),
      rl: document.getElementById('rlOut'),
      rr: document.getElementById('rrOut')
    };

    const mergedOut = {
      throttle: document.getElementById('throttleOut'),
      steering: document.getElementById('steeringOut')
    };

    const tankOut = {
      left: document.getElementById('tankLeftOut'),
      right: document.getElementById('tankRightOut')
    };

    const stickVisual = {
      throttle: document.getElementById('throttleStick'),
      steering: document.getElementById('steeringStick'),
      tankLeft: document.getElementById('tankLeftStick'),
      tankRight: document.getElementById('tankRightStick')
    };

    const flashToggleBtn = document.getElementById('flashToggle');
    const driveModeOut = document.getElementById('driveModeOut');
    const wheelPreview = document.getElementById('wheelPreview');
    const cameraSettingsEl = document.getElementById('cameraSettings');
    const previewHeightResetBtn = document.getElementById('previewHeightReset');

    const DEFAULT_PREVIEW_HEIGHT = 320;

    const out = {
      preview_height: document.getElementById('previewHeightOut'),
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

    function applyPreviewHeight(pxValue) {
      const clamped = clamp(Number(pxValue), 200, 700);
      const px = `${clamped}px`;
      const preview = document.querySelector('.preview');
      preview.style.height = px;
      preview.style.minHeight = px;
      preview.style.maxHeight = px;
      out.preview_height.textContent = px;
    }

    function clamp(v, min, max) {
      return Math.max(min, Math.min(max, v));
    }

    function computeWheelValues() {
      if (driveMode === 'tank') {
        const left = Number(tank.left.value);
        const right = Number(tank.right.value);
        return {
          fl: clamp(left, -100, 100),
          fr: clamp(right, -100, 100),
          rl: clamp(left, -100, 100),
          rr: clamp(right, -100, 100)
        };
      }

      if (driveMode === 'independent') {
        return {
          fl: Number(motors.fl.value),
          fr: Number(motors.fr.value),
          rl: Number(motors.rl.value),
          rr: Number(motors.rr.value)
        };
      }

      const throttle = Number(merged.throttle.value);
      const steering = Number(merged.steering.value);
      return {
        fl: clamp(throttle + steering, -100, 100),
        fr: clamp(throttle - steering, -100, 100),
        rl: clamp(throttle + steering, -100, 100),
        rr: clamp(throttle - steering, -100, 100)
      };
    }

    function motorPayload() {
      const wheels = computeWheelValues();
      return {
        fl: wheels.fl,
        fr: wheels.fr,
        rl: wheels.rl,
        rr: wheels.rr,
        seq: ++motorSeq,
        ttl_ms: 300
      };
    }

    async function sendMotors() {
      const payload = motorPayload();
      await fetch('/api/v1/motors', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(payload)
      });
    }

    async function stopMotors() {
      await fetch('/api/v1/stop', { method: 'POST' });
    }

    function updateReadouts() {
      const wheels = computeWheelValues();
      out.framesize.textContent = frameLabels[fields.framesize.value] || fields.framesize.value;
      out.quality.textContent = fields.quality.value;
      out.brightness.textContent = fields.brightness.value;
      out.contrast.textContent = fields.contrast.value;
      out.saturation.textContent = fields.saturation.value;
      out.led_intensity.textContent = fields.led_intensity.value;
      out.preview_height.textContent = `${fields.preview_height.value}px`;

      mergedOut.throttle.textContent = merged.throttle.value;
      mergedOut.steering.textContent = merged.steering.value;
      tankOut.left.textContent = tank.left.value;
      tankOut.right.textContent = tank.right.value;

      const throttleOffset = Math.round((Number(merged.throttle.value) / 100) * 46);
      const steeringOffset = Math.round((Number(merged.steering.value) / 100) * 46);
      const tankLeftOffset = Math.round((Number(tank.left.value) / 100) * 46);
      const tankRightOffset = Math.round((Number(tank.right.value) / 100) * 46);
      stickVisual.throttle.style.transform = `translate3d(0, ${-throttleOffset}px, 0)`;
      stickVisual.steering.style.transform = `translate3d(${steeringOffset}px, 0, 0)`;
      stickVisual.tankLeft.style.transform = `translate3d(0, ${-tankLeftOffset}px, 0)`;
      stickVisual.tankRight.style.transform = `translate3d(0, ${-tankRightOffset}px, 0)`;

      motorOut.fl.textContent = String(wheels.fl);
      motorOut.fr.textContent = String(wheels.fr);
      motorOut.rl.textContent = String(wheels.rl);
      motorOut.rr.textContent = String(wheels.rr);

      wheelPreview.textContent = `FL ${wheels.fl} | FR ${wheels.fr} | RL ${wheels.rl} | RR ${wheels.rr}`;

      flashEnabled = Number(fields.led_intensity.value) > 0;
      flashToggleBtn.classList.toggle('is-on', flashEnabled);
      flashToggleBtn.title = flashEnabled ? 'Flash On' : 'Flash Off';
      flashToggleBtn.setAttribute('aria-label', flashEnabled ? 'Flash On' : 'Flash Off');
    }

    function recenterMergedSliders() {
      if (driveMode !== 'merged' && driveMode !== 'tank') return;

      let changed = false;

      if (driveMode === 'merged') {
        if (merged.throttle.value !== '0') {
          merged.throttle.value = '0';
          changed = true;
        }
        if (merged.steering.value !== '0') {
          merged.steering.value = '0';
          changed = true;
        }
      }

      if (driveMode === 'tank') {
        if (tank.left.value !== '0') {
          tank.left.value = '0';
          changed = true;
        }
        if (tank.right.value !== '0') {
          tank.right.value = '0';
          changed = true;
        }
      }

      if (changed) {
        motorDirty = true;
        updateReadouts();
      }
    }

    async function loadStatus() {
      try {
        const status = await fetch('/status').then((r) => r.json());
        for (const key of Object.keys(fields)) {
          if (key === 'preview_height') continue;
          const el = fields[key];
          if (!(key in status)) continue;
          if (el.type === 'checkbox') {
            el.checked = Number(status[key]) === 1;
          } else {
            el.value = String(status[key]);
          }
        }

        hasFlash = Number(status.led_intensity) >= 0;
        document.getElementById('flashWrap').style.display = hasFlash ? '' : 'none';
        flashToggleBtn.style.display = hasFlash ? '' : 'none';
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
      startStopBtn.textContent = active ? 'Stop' : 'Start';
      startStopBtn.classList.toggle('danger', active);
      startStopBtn.classList.toggle('primary', !active);
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

    function updateMobileScrollLock() {
      const isMobile = window.matchMedia('(max-width: 900px)').matches;
      if (!isMobile) {
        document.body.classList.remove('lock-scroll', 'allow-scroll');
        return;
      }

      if (cameraSettingsEl.open) {
        document.body.classList.remove('lock-scroll');
        document.body.classList.add('allow-scroll');
      } else {
        document.body.classList.remove('allow-scroll');
        document.body.classList.add('lock-scroll');
      }
    }

    async function snap() {
      try {
        const blob = await fetch('/capture').then((r) => r.blob());
        if (lastBlobUrl) URL.revokeObjectURL(lastBlobUrl);
        lastBlobUrl = URL.createObjectURL(blob);
        snapshotEl.innerHTML = `<img src="${lastBlobUrl}" alt="Snapshot">`;
        lastShotImg.src = lastBlobUrl;
        lastShotImg.style.display = 'block';
        lastShotEmpty.style.display = 'none';
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

    startStopBtn.addEventListener('click', () => {
      if (isStreaming) {
        stopStream();
      } else {
        startStream();
      }
    });
    document.getElementById('snapBtn').addEventListener('click', snap);
    lastShotBtn.addEventListener('click', downloadLast);

    flashToggleBtn.addEventListener('click', () => {
      if (!hasFlash) return;
      const nextValue = flashEnabled ? 0 : 120;
      fields.led_intensity.value = String(nextValue);
      updateReadouts();
      sendControl('led_intensity', nextValue).catch(() => {});
    });

    function setDriveMode(mode) {
      driveMode = mode;
      document.getElementById('modeMerged').classList.toggle('active', mode === 'merged');
      document.getElementById('modeTank').classList.toggle('active', mode === 'tank');
      document.getElementById('modeIndependent').classList.toggle('active', mode === 'independent');
      document.getElementById('mergedControl').classList.toggle('hidden', mode !== 'merged');
      document.getElementById('tankControl').classList.toggle('hidden', mode !== 'tank');
      document.getElementById('independentControl').classList.toggle('hidden', mode !== 'independent');
      if (mode === 'merged') {
        driveModeOut.textContent = 'Merged (2 controls)';
      } else if (mode === 'tank') {
        driveModeOut.textContent = 'Tank (left/right tracks)';
      } else {
        driveModeOut.textContent = 'Independent (4 wheels)';
      }
      motorDirty = true;
      updateReadouts();
    }

    document.getElementById('modeMerged').addEventListener('click', () => setDriveMode('merged'));
    document.getElementById('modeTank').addEventListener('click', () => setDriveMode('tank'));
    document.getElementById('modeIndependent').addEventListener('click', () => setDriveMode('independent'));
    cameraSettingsEl.addEventListener('toggle', updateMobileScrollLock);
    window.addEventListener('resize', updateMobileScrollLock);

    document.getElementById('motorSend').addEventListener('click', () => {
      sendMotors().catch(() => {});
    });

    document.getElementById('motorCenter').addEventListener('click', () => {
      Object.values(motors).forEach((el) => {
        el.value = '0';
      });
      merged.throttle.value = '0';
      merged.steering.value = '0';
      tank.left.value = '0';
      tank.right.value = '0';
      updateReadouts();
      sendMotors().catch(() => {});
    });

    document.getElementById('motorReverse').addEventListener('click', () => {
      if (driveMode === 'merged') {
        merged.throttle.value = String(-Number(merged.throttle.value));
      } else if (driveMode === 'tank') {
        tank.left.value = String(-Number(tank.left.value));
        tank.right.value = String(-Number(tank.right.value));
      } else {
        Object.values(motors).forEach((el) => {
          el.value = String(-Number(el.value));
        });
      }
      updateReadouts();
      sendMotors().catch(() => {});
    });

    document.getElementById('motorStop').addEventListener('click', () => {
      Object.values(motors).forEach((el) => {
        el.value = '0';
      });
      merged.throttle.value = '0';
      merged.steering.value = '0';
      tank.left.value = '0';
      tank.right.value = '0';
      updateReadouts();
      stopMotors().catch(() => {});
    });

    [merged.throttle, merged.steering].forEach((el) => {
      ['pointerdown', 'mousedown', 'touchstart'].forEach((evt) => {
        el.addEventListener(evt, () => {
          if (driveMode === 'merged') {
            mergedDragActive = true;
          }
        }, { passive: true });
      });

      el.addEventListener('input', () => {
        motorDirty = true;
        updateReadouts();
      });
    });

    [tank.left, tank.right].forEach((el) => {
      ['pointerdown', 'mousedown', 'touchstart'].forEach((evt) => {
        el.addEventListener(evt, () => {
          if (driveMode === 'tank') {
            mergedDragActive = true;
          }
        }, { passive: true });
      });

      el.addEventListener('input', () => {
        motorDirty = true;
        updateReadouts();
      });
    });

    ['pointerup', 'mouseup', 'touchend', 'touchcancel', 'blur'].forEach((evt) => {
      window.addEventListener(evt, () => {
        if (!mergedDragActive) return;
        mergedDragActive = false;
        recenterMergedSliders();
      }, { passive: true });
    });

    ['framesize', 'quality', 'brightness', 'contrast', 'saturation', 'led_intensity'].forEach((k) => {
      fields[k].addEventListener('input', () => {
        updateReadouts();
        sendControl(k, fields[k].value).catch(() => {});
      });
    });

    fields.preview_height.addEventListener('input', () => {
      applyPreviewHeight(fields.preview_height.value);
      localStorage.setItem('rc_preview_height', fields.preview_height.value);
      updateReadouts();
    });

    previewHeightResetBtn.addEventListener('click', () => {
      fields.preview_height.value = String(DEFAULT_PREVIEW_HEIGHT);
      applyPreviewHeight(DEFAULT_PREVIEW_HEIGHT);
      localStorage.setItem('rc_preview_height', String(DEFAULT_PREVIEW_HEIGHT));
      updateReadouts();
    });

    ['awb', 'agc', 'aec', 'vflip', 'hmirror', 'colorbar'].forEach((k) => {
      fields[k].addEventListener('change', () => {
        sendControl(k, fields[k].checked ? 1 : 0).catch(() => {});
      });
    });

    Object.values(motors).forEach((el) => {
      el.addEventListener('input', () => {
        motorDirty = true;
        updateReadouts();
      });
    });

    setInterval(() => {
      if (!motorDirty) return;
      motorDirty = false;
      sendMotors().catch(() => {});
    }, 50);

    window.addEventListener('beforeunload', () => {
      stopMotors().catch(() => {});
    });

    loadStatus();
    const savedPreviewHeight = localStorage.getItem('rc_preview_height');
    if (savedPreviewHeight) {
      fields.preview_height.value = String(clamp(Number(savedPreviewHeight), 200, 700));
    } else {
      fields.preview_height.value = String(DEFAULT_PREVIEW_HEIGHT);
    }
    applyPreviewHeight(fields.preview_height.value);
    setDriveMode('merged');
    updateMobileScrollLock();
    updateReadouts();
  </script>
</body>
</html>
)HTML";

#endif
