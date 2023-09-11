#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <Arduino_JSON.h>
// Replace with your network credentials
const char* ssid = "Phat";
const char* password = "0937875855";
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");
// Json Variable to Hold Sensor Readings
JSONVar readings;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

// Initialize SPIFFS
void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}
void notifyClients(String sensorReadings) {
  ws.textAll(sensorReadings);
}

// Get Sensor Readings and return JSON object
String getSensorReadings(){
  int nhietdo=random(1,100);
  int doam=random(1,100);
  int apsuat=random(1,100);
  int text=random(1,100);
  int value_one=random(1,1000);
  int value_two=random(1,1000);
  readings["temperature"] = String(nhietdo);
  readings["humidity"] =  String(doam);
  readings["pressure"] = String(apsuat);
  readings["text"]=String(text);
  readings["value_one"]=String(value_one);
  readings["value_two"]=String(value_two);
  String jsonString = JSON.stringify(readings);
  return jsonString;
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      String message = (char*)data;
      if (message == "one") {
        Serial.println("one");
      } else if (message == "two") {
        Serial.println("two");
      } else if (message == "three") {
        Serial.println("three");
      } else if (message == "four") {
        Serial.println("four");
      }
      String sensorReadings = getSensorReadings();
      Serial.print(sensorReadings);
      notifyClients(sensorReadings);
    
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}
void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  initWiFi();
  initSPIFFS();
  initWebSocket();
  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.serveStatic("/", SPIFFS, "/");

  // Start server
  server.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  if ((millis() - lastTime) > timerDelay) {
    String sensorReadings = getSensorReadings();
    //Serial.print(sensorReadings);
    notifyClients(sensorReadings);

  lastTime = millis();

  }

  ws.cleanupClients();
}
