#include "xoled.h"

#define DATA_PIN 16
#define NUM_LEDS 53

Config config{};
uint8_t progress = 0;

CRGB PROGRESS_ON_LED = CRGB::Blue;
CRGB PROGRESS_OFF_LED = CRGB::DarkOrange;
CRGB leds[NUM_LEDS];

WiFiClientSecure espClient;

long loops = 0;
unsigned long currentMillisFPS;
unsigned long lastMillisFPS;
void printfps() {
  currentMillisFPS = millis();
  loops++;
  if (currentMillisFPS - lastMillisFPS > 1000) {
    Serial.println(loops);
    lastMillisFPS = currentMillisFPS;
    loops = 0;
  }
}

XOLED::XOLED() {}

bool XOLED::setup_wifi() {
  Serial.print("Connecting to WiFi: ");
  Serial.print(config.wifi_ssid);

  WiFi.begin(config.wifi_ssid, config.wifi_pass);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");

    EVERY_N_SECONDS(10) {
      Serial.println(" failed");
      WiFi.disconnect();
      return false;
    }

    delay(500);
  }

  randomSeed(micros());

  Serial.println(" connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  return true;
}

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

void XOLED::setup_services() {
  while (!setup_time()) delay(500);
  
  mqtt_setup();
  webserver_setup();
}

void XOLED::setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInMilliWatts(850);
  FastLED.setBrightness(64);
  FastLED.clear();
  FastLED.show();
  
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS failed to initialize");
  }

  config_load_from_FS();
  improv_setup();

  if (strlen(config.wifi_ssid) > 0) {
    size_t tries = 0;
    while (!setup_wifi() && tries < 5) {
      tries += 1;
      delay(500);
    }
    
    if (WiFi.isConnected()) setup_services();
  }

  delay(1500); // give hardware time to sort itself out
}

void XOLED::loop() {
  improv_loop();
  mqtt_loop();

  EVERY_N_MILLIS(1000) {
    float percentage = (float)progress/(float)100;
    float onLeds = (float)NUM_LEDS * percentage;

    for(int i = 0; i < NUM_LEDS; i = i + 1) {
      if (i < (int)onLeds) {
        leds[i] = PROGRESS_ON_LED;
      } else {
        leds[i] = PROGRESS_OFF_LED.fadeLightBy(4);
        // leds[i].fadeLightBy(16);
      }
    }
    FastLED.show();
    // FastLED.delay(100);
  }
  // delay(50);
}

void setup() {
  Serial.begin(115200);

	// sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(2000);

  XOLED::instance().setup();
}

void loop() {
  // printfps();
  XOLED::instance().loop();
}