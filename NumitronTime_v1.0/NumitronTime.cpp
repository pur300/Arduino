#include "NumitronTime.h"

// Constructor

NumitronTime::NumitronTime(uint8_t serialIn, uint8_t latch, uint8_t clock){

	// Set serialIn pin and port

	this->serialInPin = this->get07Pin(serialIn);
	
	this->serialInPort = this->getPort(serialIn);
	
	// Set latch pin and port
	
	this->latchPin = this->get07Pin(latch);
	
	this->latchPort = this->getPort(latch);
	
	// Set clock pin and port
	
	this->clockPin = this->get07Pin(clock);
	
	this->clockPort = this->getPort(clock);
	
	// Set output for serialIn
	
	NumitronTime::setOutputPin(this->serialInPin, this->serialInPort);
	
	// Set output for serialIn
	
	NumitronTime::setOutputPin(this->latchPin, this->latchPort);
	
	// Set output for serialIn
	
	NumitronTime::setOutputPin(this->clockPin, this->clockPort);
	

}

void NumitronTime::setClock(uint8_t hour, uint8_t minutes, uint8_t seconds){

	// Set seconds
	
	this->setNumitronDigit(NumitronTime::numitronDigit(seconds % 10));
	
	this->setNumitronDigit(NumitronTime::numitronDigit((seconds / 10) % 10));
	
	// Set minutes
	
	this->setNumitronDigit(NumitronTime::numitronDigit(minutes % 10));
	
	this->setNumitronDigit(NumitronTime::numitronDigit((minutes / 10) % 10));
	
	// Set hours
	
	this->setNumitronDigit(NumitronTime::numitronDigit(hour % 10));
	
	this->setNumitronDigit(NumitronTime::numitronDigit((hour / 10) % 10));

}

uint8_t NumitronTime::get07Pin(uint8_t pin){
  
  // Get proper pin number

  if(pin >= 0 && pin <= 7)
    return pin;

  else if(pin >= 8 && pin <= 13)
    return pin - 8;

  else if(pin >= 14 && pin <= 19)
    return pin - 14;

  else
    return 2; // Return pin 2 as a default setting

}

uint8_t NumitronTime::getPort(uint8_t pin){
  
  // Get proper port number from pin number 

  if(pin >= 8 && pin <= 13)
    return SEL_PORTB;

  else if(pin >= 14 && pin <= 19)
    return SEL_PORTC;

  else
    return SEL_PORTD; // Return PORTD as a default setting

}

void NumitronTime::setOutputPin(uint8_t pin, uint8_t port){

	if(port == SEL_PORTC){
    
		DDRC |= (1 << pin);

	}
	else if(port == SEL_PORTB){
    
		DDRB |= (1 << pin);
    
	}
	else{
		
		DDRD |= (1 << pin);

	}

}

byte NumitronTime::numitronDigit(byte digit){

	switch(digit){
	
		case 0: 
		
			return A | B | C | D | E | F;
			
		case 1:
	
			return B | C;
			
		case 2:
		
			return A | B | G | E | D;
			
		case 3:
		
			return A | B | G | C | D;
			
		case 4:
		
			return F | B | G | C;
			
		case 5:
		
			return A | F | G | C | D;
			
		case 6:
		
			return F | G | E | C | D | A;
			
		case 7:
		
			return A | B | C;
			
		case 8:
		
			return A | B | C | D | E | F | G;
			
		case 9:
		
			return A | B | C | D | F | G;
			
		default:
		
			return WRONG_DIGIT; // Return zeros if number is greater than 9... Error
		
	}

}

void NumitronTime::setNumitronDigit(byte digit){

	byte i;
	
	for(i = 0; i < 8; i++){
	
		if((digit >> i) & 0x01){
		
			// Write bit into SerialInPin. Check for port
			
			if(this->serialInPort == SEL_PORTC){
				PORTC |= (1 << this->serialInPin);
				delayMicroseconds(1);
			}
			else if(this->serialInPort == SEL_PORTB){
				PORTC |= (1 << this->serialInPin);
				delayMicroseconds(1);
			}
			else{
				PORTD |= (1 << this->serialInPin);
				delayMicroseconds(1);
			}
		
		}
		else{
		
			if(this->serialInPort == SEL_PORTC){
				PORTC &= ~ (1 << this->serialInPin);
				delayMicroseconds(1);
			}
			else if(this->serialInPort == SEL_PORTB){
				PORTC &= ~ (1 << this->serialInPin);
				delayMicroseconds(1);
			}
			else{
				PORTD &= ~ (1 << this->serialInPin);
				delayMicroseconds(1);
			}
		
		}
		
		// Clock in data. Check for port
		
		if(this->clockPort == SEL_PORTC){
		
			PORTC &= ~ (1 << this->clockPin);
			
			delayMicroseconds(1);
			
			PORTC |= (1 << this->clockPin);
			
			delayMicroseconds(1);
			
			PORTC &= ~ (1 << this->clockPin);
		
		}
		else if(this->clockPort == SEL_PORTB){
		
			PORTB &= ~ (1 << this->clockPin);
			
			delayMicroseconds(1);
			
			PORTB |= (1 << this->clockPin);
			
			delayMicroseconds(1);
			
			PORTB &= ~ (1 << this->clockPin);
		
		}
		else{
		
			PORTD &= ~ (1 << this->clockPin);
			
			delayMicroseconds(1);
			
			PORTD |= (1 << this->clockPin);
			
			delayMicroseconds(1);
			
			PORTD &= ~ (1 << this->clockPin);
		
		}
		
	
	}
	
	// latch data
	
	if(this->latchPort == SEL_PORTC){
		
		PORTC &= ~ (1 << this->latchPin);
		
		delayMicroseconds(1);
			
		PORTC |= (1 << this->latchPin);
		
		delayMicroseconds(1);
		
		PORTC &= ~ (1 << this->latchPin);
		
	}
	else if(this->latchPort == SEL_PORTB){
		
		PORTB &= ~ (1 << this->latchPin);
		
		delayMicroseconds(1);
			
		PORTB |= (1 << this->latchPin);
		
		delayMicroseconds(1);
			
		PORTB &= ~ (1 << this->latchPin);
		
	}
	else{
		
		PORTD &= ~ (1 << this->latchPin);
		
		delayMicroseconds(1);
			
		PORTD |= (1 << this->latchPin);
		
		delayMicroseconds(1);
			
		PORTD &= ~ (1 << this->latchPin);
		
	}

}

void NumitronTime::delay(void){



}