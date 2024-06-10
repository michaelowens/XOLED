#include "xoled.h"

ImprovWiFi improv_serial(&Serial);

void onImprovWiFiErrorCb(ImprovTypes::Error err)
{
  Serial.println("WiFi Improv error");
  // server.stop();
}

void onImprovWiFiConnectedCb(const char *ssid, const char *password)
{
  Serial.println("WiFi Improv connected");
  // strlcpy(config.wifi_ssid, ssid, 33);
  // strlcpy(config.wifi_pass, password, 65);
  config_save();
  XOLED::instance().setup_services();
}

bool connectWifi(const char *ssid, const char *password)
{
  Serial.println("WiFi Improv got credentials");

  strncpy(config.wifi_ssid, ssid, sizeof(config.wifi_ssid) - 1);
  strncpy(config.wifi_pass, password, sizeof(config.wifi_pass) - 1);

  size_t tries = 0;
  while (!XOLED::instance().setup_wifi() && tries < 3) {
    tries += 1;
    delay(500);
  }
   
  return WiFi.isConnected();
}

void improv_setup() {
  improv_serial.setDeviceInfo(ImprovTypes::ChipFamily::CF_ESP32, "BambuLED", "0.0.1", "BambuLED", "http://{LOCAL_IPV4}");
  improv_serial.onImprovError(onImprovWiFiErrorCb);
  improv_serial.onImprovConnected(onImprovWiFiConnectedCb);
  improv_serial.setCustomConnectWiFi(connectWifi);  // Optional
}

void improv_loop() {
  improv_serial.handleSerial();
}