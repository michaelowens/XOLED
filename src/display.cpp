#include "xoled.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int8_t display_state = 2; // 2 = on, 1 = dimmed, 0 = off
int prevButtonState = HIGH;
int buttonState = HIGH;

void display_setup() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    return;
  }

  pinMode(BUTTON_PIN, INPUT);

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(4);
  display.setCursor(2, 0);
  display.println(F("XOLED"));
  display.display();
}

void display_update() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(2, 0);
  display.println(F("XOLED"));

  display.setCursor(90, 0);
  display.println(F("v1.0.0"));
  
  int16_t value_x = 60;
  display.setCursor(2, 15);
  display.println(F("WiFi:"));
  display.setCursor(value_x, 15);
  display.println(WiFi.isConnected() ? F("OK") : F("..."));

  display.setCursor(2, 25);
  display.println(F("Printer:"));
  display.setCursor(value_x, 25);
  display.println(mqtt_is_connected() ? F("OK") : F("..."));
}

void display_loop() {
  prevButtonState = buttonState;
  buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW && buttonState != prevButtonState) {
    display_state = display_state - 1;
    if (display_state < 0) display_state = 2;
    
    display.ssd1306_command(display_state > 0 ? SSD1306_DISPLAYON : SSD1306_DISPLAYOFF);
    display.dim(display_state == 1);
  }

  EVERY_N_MILLIS(500) {
    display_update();
    display.display();
  }
}