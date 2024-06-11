#include "xoled.h"

AsyncWebServer server(80);

void not_found(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

bool store_string_param(AsyncWebServerRequest *request, char *dest, const char *key) {
  if (request->hasParam(key, true)) {
    const char* value = request->getParam(key, true)->value().c_str();
    if (strlen(value) > 0) {
      strlcpy(dest, value, 33);
      return true;
    }
  }

  return false;
}

void webserver_setup() {
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server.on("/colors.json", HTTP_GET, [](AsyncWebServerRequest *request){
    JsonDocument doc;

    char buffer[8];
    snprintf(buffer, 8, "#%02x%02x%02x", config.color_bg[0], config.color_bg[1], config.color_bg[2]);
    doc["color_bg"] = buffer;
    snprintf(buffer, 8, "#%02x%02x%02x", config.color_fg[0], config.color_fg[1], config.color_fg[2]);
    doc["color_fg"] = buffer;
    // snprintf(doc["color_fg"], 7, "#%02x%02x%02x", config.color_fg[0], config.color_fg[1], config.color_fg[2]);
    // doc["color_fg"] = std::hex(config.color_fg[0] << 16 | config.color_fg[1] << 8 | config.color_fg[2]);
    doc["brightness"] = config.brightness;

    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });

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

  server.on("/save_colors", HTTP_POST, [](AsyncWebServerRequest *request){
    bool has_changes = false;

    if (request->hasParam("color_bg", true)) {
      const char* value = request->getParam("color_bg", true)->value().c_str();
      uint8_t r, g, b;
      sscanf(value, "#%02x%02x%02x", &r, &g, &b);
      config.color_bg[0] = r;
      config.color_bg[1] = g;
      config.color_bg[2] = b;
      has_changes = true;
    }

    if (request->hasParam("color_fg", true)) {
      const char* value = request->getParam("color_fg", true)->value().c_str();
      uint8_t r, g, b;
      sscanf(value, "#%02x%02x%02x", &r, &g, &b);
      config.color_fg[0] = r;
      config.color_fg[1] = g;
      config.color_fg[2] = b;
      has_changes = true;
    }
    
    if (request->hasParam("brightness", true)) {
      long value = request->getParam("brightness", true)->value().toInt();
      config.brightness = value;
      has_changes = true;
    }

    // if (store_string_param(request, config.wifi_ssid, "wifi_ssid")) {
    //   has_changes = true;
    // }
    
    // if (store_string_param(request, config.wifi_pass, "wifi_pass")) {
    //   has_changes = true;
    // }

    if (has_changes) {
      config_save();
    }
    
    request->redirect("/");
  });

  server.on("/save_wifi", HTTP_POST, [](AsyncWebServerRequest *request){
    bool has_changes = false;

    if (store_string_param(request, config.wifi_ssid, "wifi_ssid")) {
      has_changes = true;
    }
    
    if (store_string_param(request, config.wifi_pass, "wifi_pass")) {
      has_changes = true;
    }

    if (has_changes) {
      config_save();
    }
    
    request->redirect("/settings.html");
  });

  server.on("/save_printer", HTTP_POST, [](AsyncWebServerRequest *request){
    bool has_changes = false;

    if (store_string_param(request, config.mqtt_server, "ip")) {
      has_changes = true;
    }

    if (store_string_param(request, config.mqtt_device_serial, "serial")) {
      has_changes = true;
    }

    if (store_string_param(request, config.mqtt_pass, "access_code")) {
      has_changes = true;
    }

    if (has_changes) {
      config_save();
    }

    request->redirect("/settings.html");
  });

  server.onNotFound(not_found);
  server.begin();
}