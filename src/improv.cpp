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
  config_save();
}

bool connectWifi(const char *ssid, const char *password)
{
  Serial.println("WiFi Improv got credentials");

  strncpy(config.wifi_ssid, ssid, sizeof(config.wifi_ssid) - 1);
  strncpy(config.wifi_pass, password, sizeof(config.wifi_pass) - 1);

  // Shouldn't be necessary anymore as this is now handled in wifi_loop
  // size_t tries = 0;
  // while (!g_XOLED.setup_wifi() && tries < 3)
  // {
  //   tries += 1;
  //   delay(500);
  // }

  return WiFi.isConnected();
}

void improv_setup()
{
  improv_serial.setDeviceInfo(ImprovTypes::ChipFamily::CF_ESP32, "XOLED", "0.1.0", "XOLED", "http://{LOCAL_IPV4}");
  improv_serial.onImprovError(onImprovWiFiErrorCb);
  improv_serial.onImprovConnected(onImprovWiFiConnectedCb);
  improv_serial.setCustomConnectWiFi(connectWifi); // Optional
}

void improv_loop()
{
  improv_serial.handleSerial();
}