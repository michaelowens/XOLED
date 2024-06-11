#ifndef XOLED_H
#define XOLED_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <FastLED.h>
#include <time.h>
#include <ArduinoJson.h>

#ifdef ESP32
  #include "esp_wifi.h"
  #include <WiFiClientSecure.h>
  #include <FS.h>
  #include <SPIFFS.h>
  #include <AsyncTCP.h>
#elif defined(ESP8266)
  #include <ESPAsyncTCP.h>
#endif

#include <ESPAsyncWebServer.h>
#include "ImprovWiFiLibrary.h"

#include "const.h"
#include "config.h"
#include "improv.h"
#include "mqtt.h"
#include "webserver.h"

extern Config config;
extern uint8_t progress;

extern WiFiClientSecure esp_client;

extern CRGB PROGRESS_ON_LED;
extern CRGB PROGRESS_OFF_LED;

class XOLED {
public:
  static XOLED& instance() {
    static XOLED instance;
    return instance;
  }
  XOLED(XOLED const&)           = delete;
  void operator=(XOLED const&)  = delete;

  bool setup_wifi();
  bool setup_time();
  void setup_services();
  void setup_leds();

  void setup();
  void loop();

private:
  XOLED();
};

#endif // XOLED_H