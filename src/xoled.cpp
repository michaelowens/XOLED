#include "xoled.h"

Config config{};
uint8_t progress = 0;
bool services_setup_has_run = false;

CRGB PROGRESS_ON_LED = CRGB::Blue;
CRGB PROGRESS_OFF_LED = CRGB::DarkOrange;
CRGB *leds;

WiFiClientSecure esp_client;

Printer g_Printer;
XOLED g_XOLED;

long fps_loops = 0;
unsigned long fps_current_millis;
unsigned long fps_last_millis;
void printfps() {
  fps_current_millis = millis();
  fps_loops++;
  if (fps_current_millis - fps_last_millis > 1000) {
    Serial.println(fps_loops);
    fps_last_millis = fps_current_millis;
    fps_loops = 0;
  }
}

XOLED::XOLED() {}

bool XOLED::setup_time() {
  Serial.print("set_time: Setting time using SNTP");
  configTime(8 * 3600, 0, "de.pool.ntp.org");

  time_t now = time(nullptr);
  while (now < 1000) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
    
    EVERY_N_SECONDS(10) {
      Serial.println(" failed");
      return false;
    }
  }

  Serial.println(" success");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("set_time: Current time: ");
  Serial.print(asctime(&timeinfo));

  return true;
}

void XOLED::setup_leds() {
  if (FastLED.count() != 0) {
    FastLED.clear();
    FastLED.show();
    delete[] leds;
  }

  leds = new CRGB[config.led_count];

  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, config.led_count);
  FastLED.setMaxPowerInMilliWatts(config.max_milliwatts); // (850);
  FastLED.setBrightness(config.brightness);
  FastLED.clear();
  FastLED.show();
}

void XOLED::setup() {
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS failed to initialize");
  }

  Serial.println("XOLED v2.0.0");

  config_load_from_FS();
  display_setup();

  PROGRESS_ON_LED = CRGB(config.color_fg[0], config.color_fg[1], config.color_fg[2]);
  PROGRESS_OFF_LED = CRGB(config.color_bg[0], config.color_bg[1], config.color_bg[2]);
  setup_leds();

  improv_setup();
}

bool wifi_connecting = false;
void wifi_loop() {
  if (WiFi.isConnected()) {
    if (wifi_connecting) {
      wifi_connecting = false;
      services_setup_has_run = false;
      randomSeed(micros());
      Serial.println("WiFi connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      
      configTime(8 * 3600, 0, "de.pool.ntp.org");
    }
    return;
  }

  if (!wifi_connecting) {
    if (strlen(config.wifi_ssid) < 1) return;

    wifi_connecting = true;
    
    Serial.print("Connecting to WiFi: ");
    Serial.println(config.wifi_ssid);
    WiFi.begin(config.wifi_ssid, config.wifi_pass);
  } else {
    // if (WiFi.status() == WL_CONNECTED) return;
    EVERY_N_SECONDS(10) {
      Serial.println("WiFi connection failed");
      WiFi.disconnect();
      wifi_connecting = false;
      // return false;
    }
  }
}

void services_loop() {
  if (services_setup_has_run) return;
  if (!WiFi.isConnected()) return;

  EVERY_N_SECONDS(10) {
    Serial.println("Failed to sync time");
    configTime(8 * 3600, 0, "de.pool.ntp.org");
    return;
  }
  
  time_t now = time(nullptr);
  if (now > 1000) {
    return;
  }

  mqtt_setup();
  webserver_setup();

  services_setup_has_run = true;
}

void XOLED::loop() {
  wifi_loop();
  services_loop();
  improv_loop();
  mqtt_loop();
  display_loop();

  // mpark::visit(PrinterStateHandler, state);
  switch (g_Printer.state) {
    case PrinterState::Loading: {
      unsigned long t = millis();
      int x = 10.5+sin(t/750.0)*10;
      for (int i = 0; i < config.led_count; i++) {
        if (i < x) {
          leds[i] = CRGB::Orange;
        } else {
          leds[i] = CRGB(30, 20, 0);
        }
      }
      break;
    }

    case PrinterState::Error: {
      unsigned long t = millis();
      int x = 10.5+sin(t/750.0)*10;
      for (int i = 0; i < config.led_count; i++) {
        if (i < x) {
          leds[i] = CRGB::Red;
        } else {
          leds[i] = CRGB(30, 0, 0);
        }
      }
      break;
    }

    case PrinterState::Printing: {
      float percentage = (float)g_Printer.data.printing.percentage/(float)100;
      float onLeds = (float)config.led_count * percentage;

      for(int i = 0; i < config.led_count; i = i + 1) {
        if (i < (int)onLeds) {
          leds[i] = PROGRESS_ON_LED;
        } else {
          leds[i] = PROGRESS_OFF_LED;
        }
      }
      break;
    }

    case PrinterState::Finished: {
      unsigned long t = millis();

      // Let the leds be on for 2 seconds before playing the animation
      if (t - g_Printer.data.finished.since < 2000) {
        for(int i = 0; i < config.led_count; i = i + 1) {
          leds[i] = PROGRESS_ON_LED;
        }
        break;
      }

      // After 30 seconds, switch to solid colors
      if (t - g_Printer.data.finished.since > 30000) {
        for(int i = 0; i < config.led_count; i = i + 1) {
          leds[i] = PROGRESS_ON_LED;
        }
        break;
      }

      for(int i=0; i<config.led_count; i++){
        int period = 500+(i*421)%500;
        leds[i]=CRGB(0, 255*(t%period<period/2), 0);
      }
      break;
    }
  }

  EVERY_N_MILLIS(67) {
    FastLED.show();
  }
}

void setup() {
  Serial.begin(115200);

	// sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);

  g_XOLED.setup();
}

void loop() {
  // printfps();
  g_XOLED.loop();
}