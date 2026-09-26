#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include "freertos/FreeRTOS.h"    
#include "freertos/queue.h"      
#include "freertos/semphr.h"       
#include "freertos/task.h"         
#include "design.h"
// ===================== CORE ALLOCATION =====================
// Core 0: WiFi + Web Server
// Core 1: Servo Control + Relay Control + Playback Logic

// ===================== PINS =====================
#define LED_PIN 2

// ===================== SERVO SETUP =====================
#define NUM_SERVOS 7
Servo servos[NUM_SERVOS];
const int servoPins[NUM_SERVOS] = {13, 12, 14, 27, 26, 25, 33};

float currentAngles[NUM_SERVOS] = {90, 90, 90, 90, 90, 90, 90};
float targetAngles[NUM_SERVOS]  = {90, 90, 90, 90, 90, 90, 90};
int homeAngles[NUM_SERVOS]      = {90, 90, 90, 90, 90, 90, 90};

const char* axisNames[NUM_SERVOS] = {
  "1. Base",
  "2. Shoulder",
  "3. Elbow",
  "4. Wrist Pitch",
  "5. Wrist Roll",
  "6. Gripper Rotate",
  "7. Gripper"
};

const float SERVO_SPEED = 1.0;

// ===================== RELAY SETUP =====================
#define NUM_RELAYS 4
const int relayPins[NUM_RELAYS] = {16, 17, 18, 19};
bool relayState[NUM_RELAYS] = {false, false, false, false};

const char* relayNames[NUM_RELAYS] = {
  "Relay 1",
  "Relay 2",
  "Relay 3",
  "Relay 4"
};

const bool RELAY_ACTIVE_LOW = true;

// ===================== RECORD / PLAYBACK =====================
#define MAX_STEPS 50
int recordedSteps[MAX_STEPS][NUM_SERVOS];
int stepCount = 0;

enum PlayMode { STOPPED, PLAY_ONCE, PLAY_LOOP };
PlayMode currentMode = STOPPED;

bool isMemoryLocked = false;
unsigned long lastStepTime = 0;
int currentPlaybackStep = 0;
const int stepHoldTime = 500;

// ===================== INTER-CORE COMMUNICATION =====================
typedef struct {
  int type;  // 0=setServo, 1=setRelay, 2=playOnce, 3=playLoop, 4=stop, 5=reset, 6=home, 7=record
  int id;
  int value;
} CommandMessage;

QueueHandle_t controlQueue;
SemaphoreHandle_t servoMutex;
SemaphoreHandle_t relayMutex;

String currentStatusMsg = "Ready";

// ===================== WIFI SETUP =====================
const char *ssid = "ArmoNex Robot";
const char *password = "12345678";
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
WebServer server(80);

// ===================== TASK HANDLES =====================
TaskHandle_t wifiTaskHandle;
TaskHandle_t controlTaskHandle;

// ===================== HELPER: SET RELAY =====================
void setRelay(int id, bool state) {
  if (id < 0 || id >= NUM_RELAYS) return;
  xSemaphoreTake(relayMutex, portMAX_DELAY);
  relayState[id] = state;
  if (RELAY_ACTIVE_LOW) {
    digitalWrite(relayPins[id], state ? LOW : HIGH);
  } else {
    digitalWrite(relayPins[id], state ? HIGH : LOW);
  }
  xSemaphoreGive(relayMutex);
}

// ===================== WEB HANDLERS =====================
void handleRoot() {
  server.send(200, "text/html", HTML_CONTENT);
}

void handleSetServo() {
  if (currentMode == STOPPED) {
    if (server.hasArg("id") && server.hasArg("val")) {
      int id = server.arg("id").toInt();
      int val = server.arg("val").toInt();
      if (id >= 0 && id < NUM_SERVOS) {
        CommandMessage msg = {0, id, constrain(val, 0, 180)};
        xQueueSend(controlQueue, &msg, 0);
      }
    }
  }
  server.send(200, "text/plain", "OK");
}

void handleSetRelay() {
  if (server.hasArg("id")) {
    int id = server.arg("id").toInt();
    if (id >= 0 && id < NUM_RELAYS) {
      CommandMessage msg = {1, id, 0};
      xQueueSend(controlQueue, &msg, 0);
      
      xSemaphoreTake(relayMutex, portMAX_DELAY);
      bool state = relayState[id];
      xSemaphoreGive(relayMutex);
      
      String json = "{\"state\":" + String(state ? "true" : "false") + "}";
      server.send(200, "application/json", json);
      return;
    }
  }
  server.send(400, "text/plain", "Bad Request");
}

void handleGetPositions() {
  xSemaphoreTake(servoMutex, portMAX_DELAY);
  
  String json = "{\"angles\":[";
  for (int i = 0; i < NUM_SERVOS; i++) {
    json += String(currentAngles[i], 1);
    if (i < NUM_SERVOS - 1) json += ",";
  }
  json += "],\"status\":\"" + currentStatusMsg + "\",";
  json += "\"isPlaying\":" + String(currentMode != STOPPED ? "true" : "false") + ",";
  json += "\"relays\":[";
  
  xSemaphoreTake(relayMutex, portMAX_DELAY);
  for (int i = 0; i < NUM_RELAYS; i++) {
    json += relayState[i] ? "true" : "false";
    if (i < NUM_RELAYS - 1) json += ",";
  }
  xSemaphoreGive(relayMutex);
  
  json += "]}";
  xSemaphoreGive(servoMutex);
  
  server.send(200, "application/json", json);
}

void handleHome() {
  CommandMessage msg = {6, 0, 0};
  xQueueSend(controlQueue, &msg, 0);
  server.send(200, "text/plain", "Homing...");
}

void handleRecord() {
  CommandMessage msg = {7, 0, 0};
  xQueueSend(controlQueue, &msg, 0);
  server.send(200, "text/plain", "Recording...");
}

void handlePlayOnce() {
  CommandMessage msg = {2, 0, 0};
  xQueueSend(controlQueue, &msg, 0);
  server.send(200, "text/plain", "Playing Once...");
}

void handlePlayLoop() {
  CommandMessage msg = {3, 0, 0};
  xQueueSend(controlQueue, &msg, 0);
  server.send(200, "text/plain", "Playing Loop...");
}

void handleStop() {
  CommandMessage msg = {4, 0, 0};
  xQueueSend(controlQueue, &msg, 0);
  server.send(200, "text/plain", "Stopping...");
}

void handleReset() {
  CommandMessage msg = {5, 0, 0};
  xQueueSend(controlQueue, &msg, 0);
  server.send(200, "text/plain", "Resetting...");
}

// ===================== CORE 0: WIFI TASK =====================
void wifiTask(void *parameter) {
  Serial.println("[CORE 0] WiFi Task Started");
  
  // WiFi Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);
  
  dnsServer.start(DNS_PORT, "*", apIP);
  
  // Web routes
  server.on("/", handleRoot);
  server.on("/setServo", handleSetServo);
  server.on("/setRelay", handleSetRelay);
  server.on("/getPositions", handleGetPositions);
  server.on("/home", handleHome);
  server.on("/record", handleRecord);
  server.on("/playOnce", handlePlayOnce);
  server.on("/playLoop", handlePlayLoop);
  server.on("/stop", handleStop);
  server.on("/reset", handleReset);
  
  server.onNotFound([]() {
    server.sendHeader("Location", "http://192.168.4.1/", true);
    server.send(302, "text/plain", "");
  });
  
  server.begin();
  Serial.println("[CORE 0] Web Server Started - Connect to ArmoNex Robot");
  Serial.println("[CORE 0] IP: 192.168.4.1");
  
  // WiFi loop
  while (1) {
    dnsServer.processNextRequest();
    server.handleClient();
    vTaskDelay(5 / portTICK_PERIOD_MS);
  }
}

// ===================== CORE 1: CONTROL TASK =====================
void controlTask(void *parameter) {
  Serial.println("[CORE 1] Control Task Started");
  
  // Initialize Servos
  for (int i = 0; i < NUM_SERVOS; i++) {
    servos[i].attach(servoPins[i]);
    servos[i].write(homeAngles[i]);
  }
  
  // Initialize Relays
  for (int i = 0; i < NUM_RELAYS; i++) {
    pinMode(relayPins[i], OUTPUT);
    setRelay(i, false);
  }
  
  CommandMessage msg;
  
  while (1) {
    // Process commands from queue
    if (xQueueReceive(controlQueue, &msg, 0) == pdTRUE) {
      xSemaphoreTake(servoMutex, portMAX_DELAY);
      
      switch (msg.type) {
        case 0: // Set Servo
          targetAngles[msg.id] = msg.value;
          break;
          
        case 1: // Toggle Relay
          setRelay(msg.id, !relayState[msg.id]);
          break;
          
        case 2: // Play Once
          if (stepCount == 0) {
            currentStatusMsg = "No Positions Recorded!";
          } else {
            currentPlaybackStep = 0;
            currentMode = PLAY_ONCE;
            isMemoryLocked = true;
            currentStatusMsg = "Playing Once (Step 1/" + String(stepCount) + ")";
          }
          break;
          
        case 3: // Play Loop
          if (stepCount == 0) {
            currentStatusMsg = "No Positions Recorded!";
          } else {
            currentPlaybackStep = 0;
            currentMode = PLAY_LOOP;
            isMemoryLocked = true;
            currentStatusMsg = "Looping Playback (Step 1/" + String(stepCount) + ")";
          }
          break;
          
        case 4: // Stop
          currentMode = STOPPED;
          currentStatusMsg = "Playback Stopped. (Reset required to record new)";
          break;
          
        case 5: // Reset
          currentMode = STOPPED;
          stepCount = 0;
          isMemoryLocked = false;
          currentStatusMsg = "All Records Cleared! Ready for new recording.";
          break;
          
        case 6: // Home
          currentMode = STOPPED;
          for (int i = 0; i < NUM_SERVOS; i++) {
            targetAngles[i] = homeAngles[i];
          }
          currentStatusMsg = "Moved to Home Position";
          break;
          
        case 7: // Record
          if (currentMode != STOPPED) {
            currentStatusMsg = "Stop Playback First!";
          } else if (isMemoryLocked) {
            currentStatusMsg = "Memory Locked! Reset positions first.";
          } else if (stepCount < MAX_STEPS) {
            for (int i = 0; i < NUM_SERVOS; i++) {
              recordedSteps[stepCount][i] = targetAngles[i];
            }
            stepCount++;
            currentStatusMsg = "Recorded Position " + String(stepCount);
          } else {
            currentStatusMsg = "Memory Full! Maximum 50 steps.";
          }
          break;
      }
      
      xSemaphoreGive(servoMutex);
    }
    
    // Smooth servo movement
    xSemaphoreTake(servoMutex, portMAX_DELAY);
    for (int i = 0; i < NUM_SERVOS; i++) {
      if (abs(currentAngles[i] - targetAngles[i]) > 0.2) {
        if (currentAngles[i] < targetAngles[i]) {
          currentAngles[i] += SERVO_SPEED;
        } else {
          currentAngles[i] -= SERVO_SPEED;
        }
        servos[i].write((int)currentAngles[i]);
      } else {
        currentAngles[i] = targetAngles[i];
      }
    }
    
    // Playback logic
    if (currentMode != STOPPED && stepCount > 0) {
      bool reachedTarget = true;
      for (int i = 0; i < NUM_SERVOS; i++) {
        if (abs(currentAngles[i] - targetAngles[i]) > 0.5) {
          reachedTarget = false;
          break;
        }
      }
      
      if (reachedTarget) {
        if (millis() - lastStepTime >= stepHoldTime) {
          lastStepTime = millis();
          currentPlaybackStep++;
          
          if (currentPlaybackStep >= stepCount) {
            if (currentMode == PLAY_ONCE) {
              currentMode = STOPPED;
              currentStatusMsg = "Playback Complete! (Reset to Record new)";
            } else if (currentMode == PLAY_LOOP) {
              currentPlaybackStep = 0;
            }
          }
          
          if (currentMode != STOPPED) {
            for (int i = 0; i < NUM_SERVOS; i++) {
              targetAngles[i] = recordedSteps[currentPlaybackStep][i];
            }
            currentStatusMsg = "Playing Step " + String(currentPlaybackStep + 1) + " of " + String(stepCount);
          }
        }
      } else {
        lastStepTime = millis();
      }
    }
    
    xSemaphoreGive(servoMutex);
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

// ===================== SETUP =====================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  
  // Create mutexes and queue
  servoMutex = xSemaphoreCreateMutex();
  relayMutex = xSemaphoreCreateMutex();
  controlQueue = xQueueCreate(20, sizeof(CommandMessage));
  
  Serial.println("\n\n=== ArmoNex Robot RTOS Starting ===");
  Serial.println("Initializing Dual-Core System...");
  
  // Create WiFi task on Core 0
  xTaskCreatePinnedToCore(
    wifiTask,           // Function
    "WiFi_Task",        // Name
    4096,               // Stack size
    NULL,               // Parameters
    1,                  // Priority
    &wifiTaskHandle,    // Task handle
    0                   // Core 0
  );
  
  // Create Control task on Core 1
  xTaskCreatePinnedToCore(
    controlTask,        // Function
    "Control_Task",     // Name
    4096,               // Stack size
    NULL,               // Parameters
    1,                  // Priority
    &controlTaskHandle, // Task handle
    1                   // Core 1
  );
  
  Serial.println("=== System Ready ===");
  Serial.println("Core 0: WiFi Management");
  Serial.println("Core 1: Servo & Relay Control");
}

// ===================== LOOP =====================
void loop() {
  // Main loop runs on Core 0 after boot
  // Keep it minimal - let tasks handle the work
  digitalWrite(LED_PIN, currentMode != STOPPED ? HIGH : LOW);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}
