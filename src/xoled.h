#pragma once

#include <Arduino.h>
#include <PubSubClient.h>
#include <FastLED.h>
#include <time.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#ifdef ESP8266
  #include <ESPAsyncTCP.h>
// #ifdef ESP32
#else
  #include "esp_wifi.h"
  #include <WiFiClientSecure.h>
  #include <FS.h>
  #include <SPIFFS.h>
  #include <AsyncTCP.h>
#endif

#include <ESPAsyncWebServer.h>
#include "ImprovWiFiLibrary.h"

#include "const.h"
#include "config.h"
#include "improv.h"
#include "mqtt.h"
#include "webserver.h"
#include "display.h"

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

enum class PrinterState {
  Loading, Printing, Finished, Error
};

class Printer {
public:
  PrinterState state;
  StateData data;

  Printer() : state(PrinterState::Loading) {}

  void set_state(PrinterState new_state, StateData new_data) {
    state = new_state;
    data = new_data;
  }
};

class XOLED {
public:
  XOLED();

  bool setup_time();
  void setup_leds();

  void setup();
  void loop();
};

extern Printer g_Printer;
extern XOLED g_XOLED;