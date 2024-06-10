#ifndef XOLED_CONST_H
#define XOLED_CONST_H

// Size of buffer for API JSON object (increase for more segments)
#ifdef ESP8266
  #define JSON_BUFFER_SIZE 10240
#else
  #define JSON_BUFFER_SIZE 24576
#endif

#endif // XOLED_CONST_H