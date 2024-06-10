#include "xoled.h"

AsyncWebServer server(80);

void not_found(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void webserver_setup() {
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server.on("/settings.json", HTTP_GET, [](AsyncWebServerRequest *request){
    JsonDocument doc;
    doc["wifi_ssid"] = config.wifi_ssid;
    doc["mqtt_server"] = config.mqtt_server;
    doc["mqtt_device_serial"] = config.mqtt_device_serial;
    doc["mqtt_pass"] = config.mqtt_pass;

    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });

  server.on("/save_wifi", HTTP_POST, [](AsyncWebServerRequest *request){
    bool has_changes = false;

    if (request->hasParam("wifi_ssid", true)) {
      const char* value = request->getParam("wifi_ssid", true)->value().c_str();
      if (strlen(value) > 0) {
        strlcpy(config.wifi_ssid, value, 33);
        has_changes = true;
      }
    }
    
    if (request->hasParam("wifi_pass", true)) {
      const char* value = request->getParam("wifi_pass", true)->value().c_str();
      if (strlen(value) > 0) {
        strlcpy(config.wifi_pass, value, 65);
        has_changes = true;
      }
    }

    if (has_changes) {
      config_save();
    }
    
    request->redirect("/settings.html");
  });

  server.on("/save_printer", HTTP_POST, [](AsyncWebServerRequest *request){
    bool has_changes = false;

    if (request->hasParam("ip", true)) {
      const char* value = request->getParam("ip", true)->value().c_str();
      if (strlen(value) > 0) {
        strlcpy(config.mqtt_server, value, 128);
        has_changes = true;
      }
    }

    if (request->hasParam("serial", true)) {
      const char* value = request->getParam("serial", true)->value().c_str();
      if (strlen(value) > 0) {
        strlcpy(config.mqtt_device_serial, value, 32);
        has_changes = true;
      }
    }

    if (request->hasParam("access_code", true)) {
      const char* value = request->getParam("access_code", true)->value().c_str();
      if (strlen(value) > 0) {
        strlcpy(config.mqtt_pass, value, 128);
        has_changes = true;
      }
    }

    if (has_changes) {
      config_save();
    }

    request->redirect("/settings.html");
  });

  server.onNotFound(not_found);
  server.begin();
}