#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>
#include <PubSubClient.h>

// Wi-Fi credentials
const char* ssid = "Novatech";
const char* password = "Nova@Tech";

// MQTT broker
const char* mqtt_server = "172.203.220.96";
const int mqtt_port = 1883;
const char* mqtt_topic = "esp/update/device1";
const char* log_topic = "esp/logs";
const char* firmware_base_url = "http://172.203.220.96/OTA/firmware/";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

const int LED = LED_BUILTIN;
unsigned long lastBlink = 0;
bool ledState = false;

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected");
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  msg.trim();

  Serial.println("📩 OTA Message received: " + msg);
  mqttClient.publish(log_topic, "🚀 Starting OTA update...");

  String url = String(firmware_base_url) + msg;
  mqttClient.publish(log_topic, ("⬇️ Downloading from: " + url).c_str());

  t_httpUpdate_return result = ESPhttpUpdate.update(wifiClient, url);
  switch (result) {
    case HTTP_UPDATE_FAILED:
      mqttClient.publish(log_topic, ("❌ OTA Failed: " + ESPhttpUpdate.getLastErrorString()).c_str());
      break;
    case HTTP_UPDATE_OK:
      mqttClient.publish(log_topic, "✅ OTA Successful! Rebooting...");
      break;
    case HTTP_UPDATE_NO_UPDATES:
      mqttClient.publish(log_topic, "ℹ️ No OTA update found.");
      break;
  }
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    if (mqttClient.connect("ESP8266Client")) {
      mqttClient.subscribe(mqtt_topic);
      mqttClient.publish(log_topic, "📡 ESP connected and ready for OTA");
    } else {
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH); // off initially
  connectToWiFi();
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);
}

void loop() {
  if (!mqttClient.connected()) reconnectMQTT();
  mqttClient.loop();

  if (millis() - lastBlink >= 2000) {
    ledState = !ledState;
    digitalWrite(LED, ledState ? LOW : HIGH); // LOW = ON
    lastBlink = millis();
  }
}
