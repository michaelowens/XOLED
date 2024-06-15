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

struct LoadingState {
  uint8_t angle; // sine wave phase for fx
};
struct ErrorState {};
struct PrintingState { uint8_t percentage; };
struct FinishedState {
  unsigned long since;
  uint8_t angle; // sine wave phase for fx
};

union StateData {
  LoadingState loading;
  PrintingState printing;
  FinishedState finished;
  ErrorState error;
};

enum class DeviceState {
  Loading, Printing, Finished, Error
};

class Device {
public:
  DeviceState state;
  StateData data;

  Device() : state(DeviceState::Loading) {}

  void set_state(DeviceState new_state, StateData new_data) {
    state = new_state;
    data = new_data;
  }
};

extern Device device;

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