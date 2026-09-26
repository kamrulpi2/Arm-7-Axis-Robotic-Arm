# 🤖 Tech Bee Robot - Dual-Core RTOS Control System

## Overview
This is an advanced ESP32-based robotic arm controller using **FreeRTOS dual-core architecture** with a stunning modern web UI featuring real-time robot arm visualization.

### Key Features
✅ **Dual-Core RTOS Architecture**
- **Core 0:** WiFi & Web Server (responsive UI serving)
- **Core 1:** Real-time servo & relay control (deterministic timing)
- Inter-core communication via FreeRTOS queues
- Thread-safe access with mutexes

✅ **7-Axis Robotic Arm Control**
- 7 servo motors with smooth motion control
- Real-time position synchronization
- Home position functionality
- Speed-controlled servo movements

✅ **Advanced Recording & Playback**
- Record up to 50 positions per sequence
- Play once or continuous loop mode
- Step-by-step execution with configurable hold time

✅ **4-Channel Relay Control**
- Independent relay activation/deactivation
- Active-LOW/Active-HIGH configuration support
- Real-time state feedback

✅ **Premium Web UI**
- Modern gradient design with glassmorphism effects
- Real-time robot arm visualization (SVG-based)
- Live servo position display
- Responsive design (desktop & mobile compatible)
- Status indicators and system information

---

## Hardware Requirements

### Microcontroller
- **ESP32 Development Board** (dual-core)
- USB cable for programming

### Servos
- **7x Servo Motors** (e.g., MG996R, SG90, etc.)
- Servo pins: 13, 12, 14, 27, 26, 25, 33

### Relays
- **4-Channel Relay Module** (Active-LOW or Active-HIGH)
- Relay pins: 16, 17, 18, 19

### Power Supply
- **5V/10A PSU** for servos
- **3.3V PSU** for ESP32 (or use USB)
- **GND** common across all components

### Connections
```
Servo 1 (Base)           → GPIO 13
Servo 2 (Shoulder)       → GPIO 12
Servo 3 (Elbow)          → GPIO 14
Servo 4 (Wrist Pitch)    → GPIO 27
Servo 5 (Wrist Roll)     → GPIO 26
Servo 6 (Gripper Rotate) → GPIO 25
Servo 7 (Gripper)        → GPIO 33

Relay 1 → GPIO 16
Relay 2 → GPIO 17
Relay 3 → GPIO 18
Relay 4 → GPIO 19

GND → Common ground
```

---

## Software Requirements

### Libraries
Install these libraries via Arduino IDE Library Manager:
```
1. WiFi (built-in)
2. DNSServer (built-in)
3. WebServer (built-in)
4. ESP32Servo (by John K. Bennett)
```

### Installation Steps
1. Open Arduino IDE
2. Go to **Sketch → Include Library → Manage Libraries**
3. Search and install "ESP32Servo"
4. Select **ESP32 Dev Module** as board
5. Set upload speed to **115200 baud**

---

## File Structure

```
project/
├── code.ino          # Main dual-core RTOS implementation
├── design.h          # Web UI design & HTML/CSS/JS
└── README.md         # This file
```

### To Use:
1. Create a new Arduino sketch folder: `ArmoNexRobot`
2. Copy `code.ino` into `ArmoNexRobot.ino`
3. Create new file `design.h` in same folder
4. Paste contents from provided `design.h`
5. Upload to ESP32

---

## Architecture Details

### Core 0: WiFi Task (wifiTask)
```
┌─────────────────────────────────┐
│   WiFi & Web Server Task        │
│   - DNS Server                  │
│   - HTTP Request Handling       │
│   - Route Processing            │
│   - Client Communication        │
└──────────────┬──────────────────┘
               │
        ┌──────┴──────┐
        │   Queue     │
        │  (Commands) │
        └──────┬──────┘
               │
        ┌──────▼──────────────┐
        │   Core 1: Control   │
        │      Task           │
        └─────────────────────┘
```

### Core 1: Control Task (controlTask)
```
┌─────────────────────────────────┐
│   Servo & Relay Control Task    │
│                                 │
│   1. Read Queue                 │
│   2. Execute Commands           │
│   3. Smooth Servo Motion        │
│   4. Playback Logic             │
│   5. Relay State Management     │
└─────────────────────────────────┘
```

### Inter-Core Communication
**Command Queue Structure:**
```cpp
typedef struct {
  int type;   // Command type (0-7)
  int id;     // Servo/Relay ID
  int value;  // Angle value or state
} CommandMessage;
```

**Command Types:**
- `0` - Set Servo
- `1` - Toggle Relay
- `2` - Play Once
- `3` - Play Loop
- `4` - Stop Playback
- `5` - Reset All
- `6` - Home Position
- `7` - Record Position

**Synchronization:**
- `servoMutex` - Protects servo angle arrays
- `relayMutex` - Protects relay state arrays
- `controlQueue` - Command messaging (capacity: 20)

---

## WiFi Connection

### Access Point Mode
- **SSID:** `tech bEE Robot`
- **Password:** `12345678`
- **IP Address:** `192.168.4.1`
- **Port:** `80` (HTTP)

### Connecting
1. Open WiFi settings on phone/tablet/laptop
2. Connect to "ArmoNex Robot"
3. Enter password: `12345678`
4. Open browser
5. Navigate to `http://192.168.4.1/`

---

## Web Interface Features

### 🎯 Robot Arm Visualization
- Real-time SVG representation of 7-axis arm
- Color-coded servo joints (S1-S7)
- Gripper animation based on servo 7 angle
- Status indicators

### ⚙️ Servo Control
- 7 independent slider controls (0-180°)
- Real-time position feedback
- Live angle display for each servo
- Disabled during playback (safety)

### 🎮 Command Panel
- **Home Position** - Reset all servos to home
- **Record Position** - Save current configuration
- **Play Once** - Execute sequence once
- **Play Loop** - Repeat sequence continuously
- **Stop Playback** - Halt execution
- **Reset All** - Clear memory & reset

### ⚡ Relay Control
- 4 independent relay buttons
- Visual ON/OFF indicators
- Instant toggle feedback
- Real-time state synchronization

### ℹ️ System Information
- Display RTOS architecture details
- Show max recording capacity
- Connection status

---

## Usage Examples

### Example 1: Record a Motion Sequence
```
1. Use sliders to position arm at desired location
2. Click "Record Position"
3. Move arm to next location
4. Click "Record Position" again
5. Repeat until sequence complete
6. Click "Play Once" to test
7. Click "Play Loop" for continuous execution
```

### Example 2: Control Relay
```
1. Look at "Relay Control" section
2. Click ON button for "Relay 1"
3. Relay activates (electrical circuit closes)
4. Click OFF button to deactivate
5. Real-time feedback shows current state
```

### Example 3: Home & Reset
```
1. Click "Home Position" to return all servos to 90°
2. Perform manual adjustments if needed
3. Click "Reset All" to clear recording memory
4. Now ready for new sequence recording
```

---

## Performance Specifications

### Real-Time Control
- **Servo Update Frequency:** 100 Hz (10ms loops)
- **Web UI Refresh Rate:** 4 Hz (250ms polling)
- **Queue Processing:** Real-time (Core 1 priority)
- **Motion Smoothness:** 1° per update cycle

### Memory Management
- **Recording Capacity:** 50 positions × 7 servos
- **RAM Usage:** ~12KB for recording buffer
- **Stack Sizes:** 4KB per task (configurable)
- **Queue Size:** 20 message slots

### Connectivity
- **WiFi Range:** ~50-100m (typical AP range)
- **Concurrent Clients:** 5-10 devices
- **Response Time:** <50ms (typical)
- **Update Latency:** <100ms end-to-end

---

## Customization Guide

### Change Home Position
```cpp
// In code.ino, modify line:
int homeAngles[NUM_SERVOS] = {90, 90, 90, 90, 90, 90, 90};
// Example: int homeAngles[NUM_SERVOS] = {90, 45, 120, 80, 90, 90, 0};
```

### Adjust Servo Speed
```cpp
// Line in code.ino:
const float SERVO_SPEED = 1.0;  // degrees per loop cycle
// Increase for faster motion, decrease for slower
```

### Change Recording Capacity
```cpp
// Line in code.ino:
#define MAX_STEPS 50  // Change to desired capacity
```

### Modify Relay Active Level
```cpp
// Line in code.ino:
const bool RELAY_ACTIVE_LOW = true;  // false for Active-HIGH relays
```

### Adjust UI Colors
Search in `design.h`:
```css
/* Change gradient colors */
#00adb5 → New primary color
#00d4ff → New secondary color
#ff6b6b → New accent color
```

---

## Troubleshooting

### Issue: Servos Not Moving
- ✓ Check GPIO connections
- ✓ Verify power supply (5V/10A)
- ✓ Check servo library installation
- ✓ View serial monitor (115200 baud) for errors

### Issue: WiFi Not Connecting
- ✓ Verify SSID "ArmoNex Robot" is broadcasting
- ✓ Check password "12345678"
- ✓ Try connecting to WiFi manually
- ✓ Check IP: 192.168.4.1

### Issue: Web UI Not Loading
- ✓ Ensure connected to correct WiFi network
- ✓ Clear browser cache
- ✓ Try incognito mode
- ✓ Check URL: http://192.168.4.1/ (no HTTPS)

### Issue: Relays Not Activating
- ✓ Check relay module power supply
- ✓ Verify pin connections (16, 17, 18, 19)
- ✓ Test relay module independently
- ✓ Check RELAY_ACTIVE_LOW setting

### Issue: Recording Not Working
- ✓ Click "Stop Playback" first
- ✓ Click "Reset All" to unlock memory
- ✓ Ensure steps < 50
- ✓ Wait for servos to reach target position

---

## Serial Monitor Output

Expected startup sequence:
```
[CORE 0] WiFi Task Started
[CORE 0] Web Server Started - Connect to ArmoNex Robot
[CORE 0] IP: 192.168.4.1
[CORE 1] Control Task Started
=== System Ready ===
Core 0: WiFi Management
Core 1: Servo & Relay Control
```

---

## Safety Considerations

⚠️ **Important:**
1. Keep fingers away from moving arm during operation
2. Ensure adequate space around robot
3. Don't exceed servo torque limits
4. Always disconnect power when not in use
5. Test each servo individually first
6. Start with slow servo speeds
7. Use Emergency Stop (red button) if needed
8. Never hot-connect/disconnect servos

---

## Advanced Topics

### Adding More Servos
```cpp
#define NUM_SERVOS 10  // Increase from 7
const int servoPins[NUM_SERVOS] = {13, 12, 14, 27, 26, 25, 33, GPIO_XX, GPIO_YY, GPIO_ZZ};
// Add servo names and update HTML accordingly
```

### Enabling UART Logging
```cpp
// In controlTask:
Serial.printf("[CORE 1] Servo %d target: %.1f\n", id, targetAngles[id]);
```

### Task Stack Monitoring
```cpp
UBaseType_t stackLeft = uxTaskGetStackHighWaterMark(wifiTaskHandle);
Serial.printf("WiFi Task Stack Left: %d bytes\n", stackLeft * 4);
```

---

## License & Credits
- **ESP32 Board Support:** Espressif Systems
- **Servo Library:** John K. Bennett
- **UI Design:** Modern Web Standards (HTML5, CSS3, JS ES6)
- **RTOS:** FreeRTOS (included with ESP32 Arduino)

---

## Support & Issues
For bugs or improvements, check:
- ESP32 Board package version
- Servo library compatibility
- GPIO availability on your board model
- WiFi bandwidth in your area

Enjoy your ArmoNex Robot! 🚀🤖
