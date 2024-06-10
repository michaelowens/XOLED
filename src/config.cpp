#include "xoled.h"

#define CJSON(a,b) a = b | a

JsonDocument doc;

void get_string_from_json(char* dest, const char* src, size_t len) {
  if (src != nullptr) strlcpy(dest, src, len);
}

bool read_object_from_file(const char* file, JsonDocument* dest) {
  File f = SPIFFS.open(file, "r");
  if (!f || f.available() < 1) return false;

  Serial.println(f.readString());
  f.seek(0);
  Serial.println(f.readString());
  f.seek(0);

  DeserializationError error = deserializeJson(*dest, f);
  if (error) {
    Serial.println(error.code());
  }

  f.close();
  return true;
}

bool config_load(bool fromFS) {
  get_string_from_json(config.wifi_ssid, doc["wifi_ssid"], 33);
  get_string_from_json(config.wifi_pass, doc["wifi_pass"], 65);
  
  get_string_from_json(config.mqtt_server, doc["mqtt_server"], 256);
  // CJSON(config.mqtt_port, doc["mqtt_port"]);
  get_string_from_json(config.mqtt_user, doc["mqtt_user"], 128);
  get_string_from_json(config.mqtt_pass, doc["mqtt_pass"], 128);
  get_string_from_json(config.mqtt_device_serial, doc["mqtt_device_serial"], 128);

  config.color_bg[0] = doc["color_bg"][0];
  config.color_bg[1] = doc["color_bg"][1];
  config.color_bg[2] = doc["color_bg"][2];
  
  config.color_fg[0] = doc["color_fg"][0];
  config.color_fg[1] = doc["color_fg"][1];
  config.color_fg[2] = doc["color_fg"][2];

  return false;
}

void config_save() {
  doc.clear();

  doc["wifi_ssid"] = config.wifi_ssid;
  doc["wifi_pass"] = config.wifi_pass;

  doc["mqtt_server"] = config.mqtt_server;
  // doc["mqtt_port"] = config.mqtt_port;
  doc["mqtt_user"] = config.mqtt_user;
  doc["mqtt_pass"] = config.mqtt_pass;
  doc["mqtt_device_serial"] = config.mqtt_device_serial;
  
  JsonArray color_bg = doc["color_bg"].to<JsonArray>();
  color_bg.add(config.color_bg[0]);
  color_bg.add(config.color_bg[1]);
  color_bg.add(config.color_bg[2]);
  
  JsonArray color_fg = doc["color_fg"].to<JsonArray>();
  color_fg.add(config.color_fg[0]);
  color_fg.add(config.color_fg[1]);
  color_fg.add(config.color_fg[2]);
  
  doc["brightness"] = config.brightness;

  File f = SPIFFS.open("/config.json", FILE_WRITE, true);
  if (f) serializeJson(doc, f);
  f.close();
}

bool config_load_from_FS() {
  bool success = read_object_from_file("/config.json", &doc);
  if (!success) {
    // file doesn't exist
    config_save();
    return false;
  }

  config_load(true);
  return true;
}