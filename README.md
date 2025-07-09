# ESP8266 OTA Update Dashboard with MQTT & Web Interface  
This project provides an elegant solution to remotely upload firmware to ESP8266 devices via OTA (Over-The-Air) updates using MQTT and a web-based dashboard.
## Features  
-Upload `.bin` firmware files via browser  
-Trigger OTA update via MQTT  
-View real-time logs from ESP8266 on the dashboard  
-Supports multiple devices using custom topics  
-Works over local network or public server (with port forwarding or reverse proxy)  
## Technologies Used  
-ESP8266 (Arduino Framework)  
-PHP (upload and trigger scripts)  
-MQTT (Mosquitto broker)  
-HTML + JavaScript (MQTT.js over WebSocket)  
-OTA Update via `ESPhttpUpdate`










