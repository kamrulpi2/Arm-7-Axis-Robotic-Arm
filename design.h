#ifndef DESIGN_H
#define DESIGN_H

const char HTML_CONTENT[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ArmoNex Industrial Robot Control Panel</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: radial-gradient(circle at top, #1b2735, #090a0f 70%);
            color: #e0e0e0;
            overflow-x: hidden;
            min-height: 100vh;
            padding: 15px;
        }

        .container {
            max-width: 1200px;
            margin: 0 auto;
        }

        header {
            text-align: center;
            margin-bottom: 30px;
            animation: slideDown 0.6s ease;
        }

        @keyframes slideDown {
            from { transform: translateY(-20px); opacity: 0; }
            to { transform: translateY(0); opacity: 1; }
        }

        h1 {
            font-size: 2.5em;
            background: linear-gradient(135deg, #00adb5, #00d4ff);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            margin-bottom: 5px;
            text-shadow: 0 0 20px rgba(0, 173, 181, 0.3);
        }

        .subtitle {
            color: #00adb5;
            font-size: 0.9em;
            opacity: 0.8;
        }

        .main-grid {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 20px;
            margin-bottom: 30px;
        }

        @media (max-width: 1000px) {
            .main-grid {
                grid-template-columns: 1fr;
            }
        }

        .panel {
            background: rgba(30, 30, 40, 0.8);
            backdrop-filter: blur(10px);
            border: 1px solid rgba(0, 173, 181, 0.2);
            border-radius: 18px;
            padding: 25px;
            box-shadow: 0 12px 40px rgba(0,0,0,0.45);
            animation: fadeIn 0.6s ease;
        }

        @keyframes fadeIn {
            from { opacity: 0; }
            to { opacity: 1; }
        }

        .panel h2 {
            color: #00adb5;
            margin-bottom: 20px;
            font-size: 1.4em;
            display: flex;
            align-items: center;
            gap: 10px;
        }

        .robot-arm-container {
            width: 100%;
            height: 400px;
            background: rgba(0, 0, 0, 0.3);
            border-radius: 10px;
            display: flex;
            align-items: center;
            justify-content: center;
            border: 2px solid rgba(0, 173, 181, 0.3);
            overflow: hidden;
        }

        svg.robot-arm {
            width: 100%;
            height: 100%;
            filter: drop-shadow(0 0 10px rgba(0, 173, 181, 0.3));
        }
        
        #j1-base, #j2-shoulder, #j3-elbow, #j4-wrist-pitch, 
        #j5-wrist-roll, #j6-gripper-rotate, #j7-gripper,
        #grip-left, #grip-right {
            transition: transform 0.12s cubic-bezier(.4,0,.2,1);
            transform-box: fill-box;
             transform-origin: center;
        }

        .status-box {
            background: linear-gradient(135deg, rgba(255, 46, 99, 0.1), rgba(0, 173, 181, 0.1));
            border: 2px solid #00adb5;
            border-radius: 10px;
            padding: 15px;
            margin-bottom: 20px;
            text-align: center;
            font-weight: bold;
            color: #00d4ff;
            min-height: 50px;
            display: flex;
            align-items: center;
            justify-content: center;
            animation: pulse 2s infinite;
        }

        @keyframes pulse {
            0%, 100% { opacity: 1; }
            50% { opacity: 0.7; }
        }

        .slider-container {
            margin-bottom: 20px;
        }

        .slider-label {
            display: flex;
            justify-content: space-between;
            font-size: 0.9em;
            margin-bottom: 8px;
            font-weight: 600;
            color: #e0e0e0;
        }

        .slider-label .value {
            background: linear-gradient(135deg, #00adb5, #00d4ff);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            font-weight: bold;
        }

        input[type="range"] {
            width: 100%;
            height: 8px;
            background: linear-gradient(to right, #00adb5, #00d4ff);
            border-radius: 5px;
            outline: none;
            -webkit-appearance: none;
            cursor: pointer;
        }

        input[type="range"]::-webkit-slider-thumb {
            -webkit-appearance: none;
            appearance: none;
            width: 18px;
            height: 18px;
            border-radius: 50%;
            background: #00d4ff;
            cursor: pointer;
            box-shadow: 0 0 10px rgba(0, 212, 255, 0.5);
            transition: all 0.2s;
        }

        input[type="range"]::-webkit-slider-thumb:hover {
            transform: scale(1.2);
            box-shadow: 0 0 20px rgba(0, 212, 255, 0.8);
        }

        input[type="range"]:disabled {
            opacity: 0.5;
            cursor: not-allowed;
        }

        .button-group {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 10px;
            margin-bottom: 20px;
        }

        .button-group.full {
            grid-template-columns: 1fr;
        }

        button {
            padding: 12px 20px;
            border: none;
            border-radius: 8px;
            font-weight: bold;
            cursor: pointer;
            font-size: 0.95em;
            transition: all 0.3s;
            position: relative;
            overflow: hidden;
        }

        button::before {
            content: '';
            position: absolute;
            top: 0;
            left: -100%;
            width: 100%;
            height: 100%;
            background: rgba(255, 255, 255, 0.1);
            transition: left 0.3s;
            z-index: -1;
        }

        button:hover::before {
            left: 100%;
        }

        .btn-primary {
            background: linear-gradient(135deg, #00adb5, #00d4ff);
            color: #000;
            box-shadow: 0 4px 15px rgba(0, 173, 181, 0.3);
        }

        .btn-primary:active {
            transform: scale(0.95);
        }

        .btn-secondary {
            background: linear-gradient(135deg, #0f3460, #16213e);
            color: #00d4ff;
            border: 2px solid #00adb5;
        }

        .btn-secondary:active {
            transform: scale(0.95);
        }

        .btn-danger {
            background: linear-gradient(135deg, #e94560, #ff6b6b);
            color: white;
            box-shadow: 0 4px 15px rgba(233, 69, 96, 0.3);
        }

        .btn-danger:active {
            transform: scale(0.95);
        }

        .btn-warning {
            background: linear-gradient(135deg, #d97706, #f59e0b);
            color: white;
            box-shadow: 0 4px 15px rgba(217, 119, 6, 0.3);
        }

        .relay-grid {
            display: grid;
            grid-template-columns: 1fr;
            gap: 12px;
        }

        .relay-card {
            background: rgba(0, 0, 0, 0.2);
            border: 1px solid rgba(0, 173, 181, 0.2);
            border-radius: 8px;
            padding: 12px;
            display: flex;
            justify-content: space-between;
            align-items: center;
            transition: all 0.3s;
        }

        .relay-card:hover {
            border-color: #00adb5;
            background: rgba(0, 173, 181, 0.05);
        }

        .relay-name {
            font-weight: 600;
            color: #e0e0e0;
        }

        .relay-toggle {
            padding: 8px 16px;
            border-radius: 20px;
            border: none;
            font-weight: bold;
            cursor: pointer;
            transition: all 0.3s;
            min-width: 80px;
        }

        .relay-on {
            background: linear-gradient(135deg, #10b981, #34d399);
            color: white;
            box-shadow: 0 0 15px rgba(16, 185, 129, 0.4);
        }

        .relay-off {
            background: rgba(75, 85, 99, 0.6);
            color: #aaa;
            border: 1px solid rgba(100, 100, 100, 0.3);
        }

        .info-box {
            background: rgba(0, 173, 181, 0.1);
            border-left: 4px solid #00adb5;
            padding: 15px;
            border-radius: 5px;
            margin-top: 20px;
            font-size: 0.9em;
            line-height: 1.6;
        }

        .info-box strong {
            color: #00d4ff;
        }

        .stat-row {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 15px;
            margin-bottom: 15px;
        }

        .stat {
            background: rgba(0, 0, 0, 0.2);
            padding: 12px;
            border-radius: 8px;
            border: 1px solid rgba(0, 173, 181, 0.2);
            text-align: center;
        }

        .stat-label {
            font-size: 0.85em;
            color: #aaa;
            margin-bottom: 5px;
        }

        .stat-value {
            font-size: 1.4em;
            font-weight: bold;
            background: linear-gradient(135deg, #00adb5, #00d4ff);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
        }

        .recording {
            animation: blink 1s infinite;
        }

        @keyframes blink {
            0%, 49% { opacity: 1; }
            50%, 100% { opacity: 0.5; }
        }

        .loading {
            display: inline-block;
            width: 8px;
            height: 8px;
            border-radius: 50%;
            background: #00d4ff;
            margin-left: 5px;
            animation: bounce 1.4s infinite;
        }

        @keyframes bounce {
            0%, 80%, 100% { opacity: 0; }
            40% { opacity: 1; }
        }

        .footer {
            text-align: center;
            margin-top: 40px;
            color: #666;
            font-size: 0.85em;
        }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>ArmoNex | Industrial Robotic Arm</h1>
            <p class="subtitle">ESP32 Dual-Core Real-Time Control System</p>
        </header>

        <div class="status-box" id="status">Ready</div>

        <div class="main-grid">
            <!-- LEFT PANEL: Robot Arm Visualization -->
            <div class="panel">
                <h2>3D Robot Arm Live Visualization</h2>
                <div class="robot-arm-container">
                    <svg class="robot-arm" viewBox="0 0 500 500" xmlns="http://www.w3.org/2000/svg">
                        <defs>
                            <style>
                                .joint-label { font-size: 10px; font-weight: bold; fill: #000; text-anchor: middle; }
                                .segment { stroke-linecap: round; }
                            </style>
                        </defs>
                        
                        <!-- Base Platform -->
                        <ellipse cx="250" cy="450" rx="40" ry="15" fill="#00adb5" opacity="0.8"/>
                        <rect x="220" y="440" width="60" height="15" fill="#1e90ff" opacity="0.6" rx="3"/>
                        
                        <!-- Base/Joint 1 (Servo 1) - Rotation around Z axis -->
                        <g id="j1-base">
                            <circle cx="250" cy="420" r="14" fill="#00d4ff" stroke="#00adb5" stroke-width="2"/>
                            <text class="joint-label" x="250" y="425">S1</text>
                        </g>
                        
                        <!-- Shoulder assembly (Servo 2) - Pitch -->
                        <g id="j2-shoulder" transform="translate(250, 420)">
                            <!-- Segment to shoulder -->
                            <line x1="0" y1="0" x2="0" y2="-75" class="segment" stroke="#1e90ff" stroke-width="14" opacity="0.8"/>
                            
                            <!-- Shoulder joint -->
                            <circle cx="0" cy="-75" r="13" fill="#00d4ff" stroke="#00adb5" stroke-width="2"/>
                            <text class="joint-label" x="0" y="-70">S2</text>
                            
                            <!-- Elbow assembly (Servo 3) - Pitch -->
                            <g id="j3-elbow" transform="translate(0, -75)">
                                <!-- Segment to elbow -->
                                <line x1="0" y1="0" x2="0" y2="-65" class="segment" stroke="#0099ff" stroke-width="13" opacity="0.8"/>
                                
                                <!-- Elbow joint -->
                                <circle cx="0" cy="-65" r="12" fill="#00d4ff" stroke="#00adb5" stroke-width="2"/>
                                <text class="joint-label" x="0" y="-60">S3</text>
                                
                                <!-- Wrist Pitch assembly (Servo 4) -->
                                <g id="j4-wrist-pitch" transform="translate(0, -65)">
                                    <!-- Segment to wrist -->
                                    <line x1="0" y1="0" x2="45" y2="-35" class="segment" stroke="#00ffff" stroke-width="11" opacity="0.8"/>
                                    
                                    <!-- Wrist pitch joint -->
                                    <circle cx="45" cy="-35" r="11" fill="#00d4ff" stroke="#00adb5" stroke-width="2"/>
                                    <text class="joint-label" x="45" y="-30">S4</text>
                                    
                                    <!-- Wrist Roll assembly (Servo 5) -->
                                    <g id="j5-wrist-roll" transform="translate(45, -35)">
                                        <!-- Segment -->
                                        <line x1="0" y1="0" x2="28" y2="-18" class="segment" stroke="#00dd99" stroke-width="9" opacity="0.8"/>
                                        
                                        <!-- Wrist roll joint -->
                                        <circle cx="28" cy="-18" r="10" fill="#00d4ff" stroke="#00adb5" stroke-width="2"/>
                                        <text class="joint-label" x="28" y="-13">S5</text>
                                        
                                        <!-- Gripper Rotate assembly (Servo 6) -->
                                        <g id="j6-gripper-rotate" transform="translate(28, -18)">
                                            <!-- Gripper base joint -->
                                            <circle cx="0" cy="0" r="9" fill="#ff9500" stroke="#00adb5" stroke-width="2"/>
                                            <text class="joint-label" x="0" y="3">S6</text>
                                            
                                            <!-- Gripper (Servo 7) - Opens/closes -->
                                            <g id="j7-gripper" transform="translate(0, 0)">
                                                <!-- Left finger -->
                                                <rect id="grip-left" x="-10" y="-4" width="9" height="8" fill="#ff6b6b" stroke="#ff3333" stroke-width="1.5" rx="2" transform-origin="0 0"/>
                                                <!-- Right finger -->
                                                <rect id="grip-right" x="1" y="-4" width="9" height="8" fill="#ff8888" stroke="#ff3333" stroke-width="1.5" rx="2" transform-origin="0 0"/>
                                                <!-- Center pivot -->
                                                <circle cx="0" cy="0" r="2.5" fill="#ffd700" stroke="#00adb5" stroke-width="1"/>
                                            </g>
                                        </g>
                                    </g>
                                </g>
                            </g>
                        </g>
                    </svg>
                </div>

                <div class="stat-row">
                    <div class="stat">
                        <div class="stat-label">Recording Steps</div>
                        <div class="stat-value" id="stepCount">0 / 50</div>
                    </div>
                    <div class="stat">
                        <div class="stat-label">System Mode</div>
                        <div class="stat-value" id="modeDisplay">IDLE</div>
                    </div>
                </div>
            </div>

            <!-- RIGHT PANEL: Controls & Sliders -->
            <div class="panel">
                <h2>Servo Motion Control</h2>
                <div id="sliders-container"></div>
            </div>
        </div>

        <!-- COMMAND BUTTONS -->
        <div class="panel">
            <h2>Motion Command Center</h2>
            <div class="button-group">
                <button class="btn-primary" onclick="sendCommand('/home')">🏠 Home Position</button>
                <button class="btn-secondary" onclick="sendCommand('/record')">📍 Record Position</button>
                <button class="btn-secondary" onclick="sendCommand('/playOnce')">▶️ Play Once</button>
                <button class="btn-secondary" onclick="sendCommand('/playLoop')">🔄 Play Loop</button>
            </div>
            <div class="button-group">
                <button class="btn-danger" onclick="sendCommand('/stop')">⏹️ Stop Playback</button>
                <button class="btn-warning" onclick="sendCommand('/reset')">🔄 Reset All</button>
            </div>
        </div>

        <!-- RELAY CONTROL -->
        <div class="panel">
            <h2>Peripheral Relay Control</h2>
            <div class="relay-grid" id="relay-container"></div>
        </div>

        <!-- INFO BOX -->
        <div class="panel">
            <h2>System Diagnostics</h2>
            <div class="info-box">
                <strong>RTOS Dual-Core Architecture:</strong><br>
                • <strong>Core 0:</strong> WiFi & Web Server Management<br>
                • <strong>Core 1:</strong> Real-time Servo & Relay Control<br>
                • <strong>Queue-Based Communication:</strong> Safe inter-core messaging<br>
                • <strong>Mutex Protection:</strong> Thread-safe servo & relay access<br>
                <br>
                <strong>Max Recording:</strong> 50 positions per sequence<br>
                <strong>Connected to:</strong> ArmoNex Robot (192.168.4.1)
            </div>
        </div>

        <div class="footer">
            <p>ArmoNex | Industrial Robotic Arm System | Powered by ESP32 RTOS • Live Motion Visualization</p>
        </div>
    </div>

    <script>
        const axisNames = ["1. Base", "2. Shoulder", "3. Elbow", "4. Wrist Pitch", "5. Wrist Roll", "6. Gripper Rotate", "7. Gripper"];
        const relayNames = ["Relay 1", "Relay 2", "Relay 3", "Relay 4"];

        let lastSendTime = 0;
        let recordedCount = 0;
        let isPlaying = false;

        // Create servo sliders
        function initializeSliders() {
            const container = document.getElementById('sliders-container');
            container.innerHTML = '';
            for (let i = 0; i < 7; i++) {
                container.innerHTML += `
                    <div class="slider-container">
                        <div class="slider-label">
                            <span>${axisNames[i]}</span>
                            <span class="value"><span id="val${i}">90</span>°</span>
                        </div>
                        <input type="range" id="s${i}" min="0" max="180" value="90" step="1" 
                               oninput="updateServo(${i}, this.value)">
                    </div>
                `;
            }
        }

        // Create relay buttons
        function initializeRelays() {
            const container = document.getElementById('relay-container');
            container.innerHTML = '';
            for (let i = 0; i < 4; i++) {
                container.innerHTML += `
                    <div class="relay-card">
                        <span class="relay-name">${relayNames[i]}</span>
                        <button class="relay-toggle relay-off" id="r${i}" onclick="toggleRelay(${i})">OFF</button>
                    </div>
                `;
            }
        }

        function updateServo(id, val) {
            document.getElementById('val' + id).innerText = val;
            const now = Date.now();
            if (now - lastSendTime > 15) {
                fetch(`/setServo?id=${id}&val=${val}`);
                lastSendTime = now;
            }
            updateRobotArm();
        }

        function updateRobotArm() {

            const angles = [];

            for(let i=0;i<7;i++){
                angles[i] = parseInt(document.getElementById('s'+i).value);
            }

            let a1 = angles[0]-90;
            let a2 = angles[1]-90;
            let a3 = angles[2]-90;
            let a4 = angles[3]-90;
            let a5 = angles[4]-90;

            let base = document.getElementById("j2-shoulder");
            if(base){
                base.setAttribute("transform",
                    `translate(250 420) rotate(${a1})`);
            }

            let elbow = document.getElementById("j3-elbow");
            if(elbow){
                elbow.setAttribute("transform",
                    `translate(0 -75) rotate(${a2})`);
            }

            let wrist = document.getElementById("j4-wrist-pitch");
            if(wrist){
                wrist.setAttribute("transform",
                    `translate(0 -65) rotate(${a3})`);
            }

            let roll = document.getElementById("j5-wrist-roll");
            if(roll){
                roll.setAttribute("transform",
                    `translate(45 -35) rotate(${a4})`);
            }

            let gripRotate = document.getElementById("j6-gripper-rotate");
            if(gripRotate){
                gripRotate.setAttribute("transform",
                    `translate(28 -18) rotate(${a5})`);
            }

            let left = document.getElementById("grip-left");
            let right = document.getElementById("grip-right");

            if(left && right){
                let open = (angles[6]/180)*15;

                left.setAttribute("x", -10-open);
                right.setAttribute("x", 1+open);
            }
        }

        function toggleRelay(id) {
            fetch(`/setRelay?id=${id}`)
                .then(res => res.json())
                .then(data => updateRelayUI(id, data.state))
                .catch(err => console.error(err));
        }

        function updateRelayUI(id, state) {
            const btn = document.getElementById('r' + id);
            if (state) {
                btn.innerText = "ON";
                btn.className = "relay-toggle relay-on";
            } else {
                btn.innerText = "OFF";
                btn.className = "relay-toggle relay-off";
            }
        }

        function toggleSliders(disable) {
            for (let i = 0; i < 7; i++) {
                document.getElementById('s' + i).disabled = disable;
            }
        }

        function sendCommand(path) {
            fetch(path)
                .then(res => res.text())
                .then(text => {
                    document.getElementById('status').innerText = text;
                })
                .catch(err => console.error(err));
        }

        // Update loop
        setInterval(() => {
            fetch('/getPositions')
                .then(res => res.json())
                .then(data => {
                    // Update sliders
                    for (let i = 0; i < 7; i++) {
                        const angle = Math.round(data.angles[i]);
                        document.getElementById('s' + i).value = angle;
                        document.getElementById('val' + i).innerText = angle;
                    }

                    // Update status
                    document.getElementById('status').innerText = data.status;

                    // Update playing state
                    isPlaying = data.isPlaying;
                    toggleSliders(isPlaying);
                    document.getElementById('modeDisplay').innerText = isPlaying ? '▶️ PLAYING' : '⏸️ IDLE';

                    // Update relays
                    for (let i = 0; i < 4; i++) {
                        updateRelayUI(i, data.relays[i]);
                    }

                    // Update robot visualization
                    updateRobotArm();
                }).catch(err => console.error(err));
        }, 250);

        // Initialize on page load
        window.addEventListener('load', () => {
            initializeSliders();
            initializeRelays();
            updateRobotArm();
        });
    </script>
</body>
</html>
)rawliteral";

#endif // DESIGN_H