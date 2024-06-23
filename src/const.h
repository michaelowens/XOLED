#ifndef XOLED_CONST_H
#define XOLED_CONST_H

// Built in LED pin
#define LED_BUILTIN 2

// Toggle screen button GPIO pin
#define BUTTON_PIN 0

// Size of buffer for API JSON object (increase for more segments)
#ifdef ESP8266
  #define JSON_BUFFER_SIZE 10240
#else
  #define JSON_BUFFER_SIZE 24576
#endif

#ifdef ARDUINO_ARCH_ESP32C3
  #define DATA_PIN 3
#else // ESP32
  #define DATA_PIN 14
#endif

#define SCREEN_WIDTH   128  // OLED display width, in pixels
#define SCREEN_HEIGHT  32   // OLED display height, in pixels
#define OLED_RESET     -1   // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#endif // XOLED_CONST_H