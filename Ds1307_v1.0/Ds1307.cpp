#include "Ds1307.h"

void Ds1307::begin(void){

	// I2C master mode

	Wire.begin();
	
	// Enable oscillator
	
	this->enableOscillator();

}

void Ds1307::setClock(short year, uint8_t month, uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds){

	// Check if values are within ranges
	
	if(year >= 2000 && year <= 2099 && month >= 1 && month <= 12 && day >= 1 && day <= 31 && hours >= 0 && hours <= 23 && minutes >= 0 && minutes <= 59 && seconds >= 0 && seconds <= 59){
	
		// Begin transmission
	
		Wire.beginTransmission(DS1307_I2C_ADDRESS);
		
		// Set address pointer to first register
	
		Wire.write(BASE_REG);
		
		// Set seconds
		
		Wire.write(this->numToBCD(seconds));
		
		// Set minutes
		
		Wire.write(this->numToBCD(minutes));
		
		// Set hours
		
		Wire.write(this->numToBCD(HOURS_MASK & hours));
		
		// Set Day of week... Dummy write
		
		Wire.write(1);
		
		// Set day of month
		
		Wire.write(this->numToBCD(day));
		
		// Set month
		
		Wire.write(this->numToBCD(month));
		
		// Set year
		
		Wire.write(this->numToBCD(year - 2000));
		
		// End transmission
	
		Wire.endTransmission();
	
	}


}

void Ds1307::setClock(long epoch){

    // Not implemented yet

}

void Ds1307::now(short * year, uint8_t * month, uint8_t * day, uint8_t * hours, uint8_t * minutes, uint8_t * seconds){

	// Begin transmission
	
	Wire.beginTransmission(DS1307_I2C_ADDRESS);
	
	// Set address pointer to first register*/
	
	Wire.write(BASE_REG);
	
	// End transmission
	
	Wire.endTransmission();
	
	// Requests 7 bytes
	
	Wire.requestFrom(DS1307_I2C_ADDRESS, 7);
	
	// Wait until bytes are available to be read
	
	while(Wire.available() != 7);
	
	// Get seconds
	
	*seconds = this->BCDToNum(SECONDS_MASK & Wire.read());
	
	// get minutes
	
	*minutes = this->BCDToNum(Wire.read());
	
	// Get hours
	
	*hours = this->BCDToNum(HOURS_MASK & Wire.read());
	
	// Dummy read... Skip Day of week
	
	Wire.read();
	
	// Get day
	
	*day = this->BCDToNum(Wire.read());
	
	// Get month
	
	*month = this->BCDToNum(Wire.read());
	
	// Get year
	
	*year = 2000 + this->BCDToNum(Wire.read());
	

}

uint8_t Ds1307::numToBCD(uint8_t num){
	
	return ((num / 10) % 10) << 4 | num % 10;

}

uint8_t Ds1307::BCDToNum(uint8_t bcd){
	
	return (bcd >> 4) * 10 + (bcd & 0b00001111);

}

void Ds1307::enableOscillator(void){

	uint8_t tempSeconds;
	
	// Begin transmission
	
	Wire.beginTransmission(DS1307_I2C_ADDRESS);
	
	// Set register pointer to first register
	
	Wire.write(BASE_REG);
	
	// End transmission
	
	Wire.endTransmission();
	
	// Request 1 byte
	
	Wire.requestFrom(DS1307_I2C_ADDRESS, 1);
	
	// Wait until byte is available to be read
	
	while(Wire.available() != 1);
	
	// Read seconds
	
	tempSeconds = Wire.read();

	// Begin transmission
	
	Wire.beginTransmission(DS1307_I2C_ADDRESS);
	
	// Set register pointer to first register
	
	Wire.write(BASE_REG);
	
	// Enable oscillator
	
	Wire.write(0b01111111 & tempSeconds);
	
	// End transmission
	
	Wire.endTransmission();

}

void Ds1307::disableOscillator(void){

	uint8_t tempSeconds;
	
	// Begin transmission
	
	Wire.beginTransmission(DS1307_I2C_ADDRESS);
	
	// Set register pointer to first register
	
	Wire.write(BASE_REG);
	
	// End transmission
	
	Wire.endTransmission();
	
	// Request 1 byte
	
	Wire.requestFrom(DS1307_I2C_ADDRESS, 1);
	
	// Wait until byte is available to be read
	
	while(Wire.available() != 1);
	
	// Read seconds
	
	tempSeconds = Wire.read();

	// Begin transmission
	
	Wire.beginTransmission(DS1307_I2C_ADDRESS);
	
	// Set register pointer to first register
	
	Wire.write(BASE_REG);
	
	// Disable oscillator
	
	Wire.write(0b10000000 | tempSeconds);
	
	// End transmission
	
	Wire.endTransmission();

}