#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

// ============ CONFIG ============
// Wi-Fi credentials
const char* ssid = "Novatech";
const char* password = "Nova@Tech";

// MQTT broker details
const char* mqtt_server = "172.203.220.96";
const int mqtt_port = 1883;

// Device-specific ID (used to create OTA topic)
#define DEVICE_ID "device1"

// OTA firmware base URL (firmware must match filename sent)
const char* firmware_base_url = "http://172.203.220.96/OTA/firmware/";

// OTA log topic (shared by all devices)
const char* log_topic = "esp/logs";

// ============ OBJECTS ============
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Will hold the full topic: esp/update/device1
String ota_topic = "esp/update/" + String(DEVICE_ID);

// ============ FUNCTIONS ============
void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected");
  Serial.print("📶 IP Address: ");
  Serial.println(WiFi.localIP());
}

void publishLog(String message) {
  Serial.println(message);
  mqttClient.publish(log_topic, message.c_str());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  msg.trim();

  Serial.print("📩 Message received on [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(msg);

  if (msg.endsWith(".bin")) {
    publishLog("🚀 Starting OTA update...");

    String full_url = String(firmware_base_url) + msg;
    publishLog("⬇️ Downloading from: " + full_url);

    t_httpUpdate_return result = ESPhttpUpdate.update(wifiClient, full_url);

    switch (result) {
      case HTTP_UPDATE_FAILED:
        publishLog("❌ OTA Update Failed: " + ESPhttpUpdate.getLastErrorString());
        break;
      case HTTP_UPDATE_NO_UPDATES:
        publishLog("ℹ️ No update available.");
        break;
      case HTTP_UPDATE_OK:
        publishLog("✅ OTA Update successful! Rebooting...");
        break;
    }
  } else {
    publishLog("⚠️ Invalid OTA command. Expecting a .bin filename.");
  }
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("🔄 Attempting MQTT connection...");
    if (mqttClient.connect(DEVICE_ID)) {
      Serial.println("✅ MQTT connected");

      mqttClient.subscribe(ota_topic.c_str());
      publishLog("📡 " + String(DEVICE_ID) + " connected and listening on " + ota_topic);
    } else {
      Serial.print("❌ MQTT failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" - retrying in 5 seconds");
      delay(5000);
    }
  }
}

// ============ SETUP & LOOP ============
void setup() {
  Serial.begin(115200);
  connectToWiFi();
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();
}
