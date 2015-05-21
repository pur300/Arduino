#ifndef WEATHER_H
#define WEATHER_H

#include <Arduino.h>
#include <Wire.h>
#include "util/delay.h"

// Pressure module defines

#define BMP085_I2C_ADDRESS 0x77
#define OSS 3 // Very percise mode
#define DELAY_OSS (5 << OSS)

// Humidity module defines

#define SEL_PORTB 0
#define SEL_PORTC 1
#define SEL_PORTD 2
#define TIMEOUT_US 100
#define DATA_BIT_COUNT 40

class Weather{

  private:
  
    // Vars for pressure measurement
  
    short AC1, AC2, AC3, BB1, BB2, MB, MC, MD;
    unsigned short AC4, AC5, AC6;
    byte pinDht22, portDht22;
    
    // Private functions for pressure measurements

    int readCalibrationCoef(byte, byte);
    void writeRegister(byte, byte);
    byte readRegister(byte);
    
    // Private functions for humidity measurements
    
    byte getPort(byte);
    byte getPin(byte);
    void setOutput(void);
    void setInput(void);
    void writeOne(void);
    void writeZero(void);
    byte readPin(void);
    
  
  public:
  
	// Init sensors

	void begin(byte);
  
    // Pressure module functions
    
    long readPressurePascals(void);
    double readPressureBars(void);
    double readTemp(void);
    int readHeight(void);
    
    // Humidity module functions
    
    double readHumidity(void);
  
};

#endif