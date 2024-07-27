#pragma once

struct Config {
  char wifi_ssid[33];
  char wifi_pass[65];

  char mqtt_server[128];
  uint16_t mqtt_port;
  char mqtt_user[128];
  char mqtt_pass[128];
  char mqtt_device_serial[32];

  uint16_t led_count;
  uint32_t max_milliwatts;

  uint8_t color_bg[3];
  uint8_t color_fg[3];
  uint8_t brightness;
};

bool config_load(JsonObject doc, bool fromFS);
void config_save();
bool config_load_from_FS();
