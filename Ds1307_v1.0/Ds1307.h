#ifndef DS1307_H
#define DS1307_H

#include <Arduino.h>
#include <Wire.h>

// Defines

#define DS1307_I2C_ADDRESS 0x68

#define BASE_REG 0x00

#define HOURS_MASK 0b00111111
#define SECONDS_MASK 0b01111111



class Ds1307{

    public:

		void begin(void);
        void setClock(short year, uint8_t month, uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds);
        void setClock(long epoch);
        void now(short * year, uint8_t * month, uint8_t * day, uint8_t * hours, uint8_t * minutes, uint8_t * seconds);
		void enableOscillator(void);
		void disableOscillator(void);
    
    
    private:
    
		uint8_t numToBCD(uint8_t num);
		uint8_t BCDToNum(uint8_t bcd);
		

};


#endif