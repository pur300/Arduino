#ifndef NUMITRONTIME_H
#define NUMITRONTIME_H

#include <Arduino.h>

// Defines

#define SEL_PORTB 0
#define SEL_PORTC 1
#define SEL_PORTD 2

#define A 0b10000000
#define B 0b01000000
#define C 0b00100000
#define D 0b00010000
#define E 0b00001000
#define F 0b00000100
#define G 0b00000010
#define WRONG_DIGIT 0b00000000;

class NumitronTime{

	private:
		uint8_t serialInPort;
		uint8_t serialInPin;
		uint8_t latchPort;
		uint8_t latchPin;
		uint8_t clockPort;
		uint8_t clockPin;
		
		static uint8_t get07Pin(uint8_t pin);
		static uint8_t getPort(uint8_t pin);
		static void setOutputPin(uint8_t pin, uint8_t port);
		static byte numitronDigit(byte digit);
		static void delay(void);
		void setNumitronDigit(byte digit);
		
	
	public:
		NumitronTime(uint8_t serialIn, uint8_t latch, uint8_t clock);
		void setClock(uint8_t hour, uint8_t minutes, uint8_t seconds);
		
	

};

#endif