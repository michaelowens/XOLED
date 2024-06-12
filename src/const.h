#ifndef XOLED_CONST_H
#define XOLED_CONST_H

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

#endif // XOLED_CONST_H