/*
  Smart Plant IoT — ESP8266 Firmware
  Hardware: NodeMCU ESP8266 + capacitive soil moisture sensor + relay + water pump
  Sends readings to the Vercel-hosted API and polls for irrigation commands.
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// ---- Wi-Fi ----
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ---- Deployed Vercel API (replace after `vercel deploy`) ----
const char* host        = "your-project.vercel.app";
String dataPath          = "/api/data";
String irrigationPath    = "/api/irrigation";

// ---- Pins ----
#define SOIL_PIN A0      // ESP8266 has a single analog pin (0-1023)
#define RELAY_PIN D1      // Relay IN pin

// ---- Calibration (run the sensor in dry air and in water, then update) ----
const int DRY_VALUE = 780;   // raw ADC reading in dry air
const int WET_VALUE = 320;   // raw ADC reading fully submerged in water

unsigned long bootTime;

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // relay module is active LOW -> HIGH = pump off

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
  bootTime = millis();
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    int raw = analogRead(SOIL_PIN);
    int moisture = map(raw, DRY_VALUE, WET_VALUE, 0, 100);
    moisture = constrain(moisture, 0, 100);
    unsigned long uptime = (millis() - bootTime) / 1000;

    bool pumpState = digitalRead(RELAY_PIN) == LOW;

    sendReading(moisture, raw, pumpState, uptime);
    bool command = fetchPumpCommand();
    applyPump(command);
  } else {
    Serial.println("WiFi disconnected!");
  }
  delay(10000); // matches the dashboard's 10s refresh
}

void sendReading(int moisture, int raw, bool pump, unsigned long uptime) {
  WiFiClientSecure client;
  client.setInsecure(); // Vercel uses valid certs; setInsecure keeps this simple for ESP8266
  HTTPClient http;

  String url = "https://" + String(host) + dataPath;
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<128> doc;
  doc["moisture"] = moisture;
  doc["raw"] = raw;
  doc["pump"] = pump;
  doc["uptime"] = uptime;

  String body;
  serializeJson(doc, body);
  int code = http.POST(body);

  Serial.print("POST /api/data -> ");
  Serial.println(code);
  http.end();
}

bool fetchPumpCommand() {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  String url = "https://" + String(host) + irrigationPath;
  http.begin(client, url);
  int code = http.GET();
  bool state = false;

  if (code == 200) {
    StaticJsonDocument<64> doc;
    deserializeJson(doc, http.getString());
    state = doc["state"];
  }
  http.end();
  return state;
}

void applyPump(bool on) {
  digitalWrite(RELAY_PIN, on ? LOW : HIGH); // active LOW relay
}
