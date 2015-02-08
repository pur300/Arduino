#ifndef WS2812B_H
#define WS2812B_H

#include <Arduino.h>

// Defines

#define SEL_PORTB 0
#define SEL_PORTC 1
#define SEL_PORTD 2

class Ws2812b{

private:
  uint8_t port;
  byte numOfLeds;
  uint8_t pin;
  float brightness;
  volatile uint8_t * ledStrip;
  static uint8_t get07Pin(uint8_t pin);
  static uint8_t getPort(uint8_t pin);

public:
  Ws2812b(uint8_t pin, short numOfLeds);
  void setColor(uint8_t r, uint8_t g, uint8_t b); // Set one color for whole led strip
  void setColorAt(short pos, uint8_t r, uint8_t g, uint8_t b); // Set color for particular led in the strip
  void refresh(void);
  void clearStrip(void);
  void setBrightness(uint8_t brightness);

};

#endif