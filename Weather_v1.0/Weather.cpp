#include "Weather.h"

void Weather::begin(byte pin){
	
  // Set pin for DHT22
  
  pinDht22 = getPin(pin);
  
  // Set port for DHT22
  
  portDht22 = getPort(pin);

  // Get all needed coefficients for BMP085;
  
  // Get AC1
  
  AC1 = readCalibrationCoef(0xAA, 0xAB);
  
  // Get AC2
  
  AC2 = readCalibrationCoef(0xAC, 0xAD);
  
  // Get AC3
  
  AC3 = readCalibrationCoef(0xAE, 0xAF);
  
  // Get AC4
  
  AC4 = readCalibrationCoef(0xB0, 0xB1);
  
  // Get AC5
  
  AC5 = readCalibrationCoef(0xB2, 0xB3);
  
  // get AC6
  
  AC6 = readCalibrationCoef(0xB4, 0xB5);
  
  // Get BB1
  
  BB1 = readCalibrationCoef(0xB6, 0xB7);
  
  // Get BB2
  
  BB2 = readCalibrationCoef(0xB8, 0xB9);
  
  // Get MB
  
  MB = readCalibrationCoef(0xBA, 0xBB);
  
  // Get MC
  
  MC = readCalibrationCoef(0xBC, 0xBD);
  
  // Get MD
  
  MD = readCalibrationCoef(0xBE, 0xBF);
  
}

// Pressure sensor functions

long Weather::readPressurePascals(void){
  
  long UP, UT, X1, X2, X3, B3, B5, B6, T, P;
  
  unsigned long B4, B7;
  
  // Read uncompensated temperature value
  
  writeRegister(0xF4, 0x2E);
  
  // Delay
  
  delay(DELAY_OSS);
  
  UT = readRegister(0xF6) << 8 | readRegister(0xF7);
  
  // Calculate true temperature
  
  X1 = (UT - (long)AC6) * (long)AC5 / pow(2, 15);
  
  X2 = (long)MC * pow(2, 11) / (X1 + (long)MD);
  
  B5 = X1 + X2;
  
  T = (B5 + 8) / pow(2, 4);
  
  // Read uncompensated pressure value
  
  writeRegister(0xF4, 0x34 | (OSS << 6));
  
  // Delay
  
  delay(DELAY_OSS);
  
  UP = ((long)readRegister(0xF6) << 16 | (long)readRegister(0xF7) << 8 | readRegister(0xF8)) >> (8 - OSS);
  
  // Calculate true pressure
  
  B6 = B5 - 4000;
  
  X1 = ((long)BB2 * B6 * B6 / pow(2, 12)) / pow(2, 11);
  
  X2 = (long)AC2 * B6 / pow(2, 11);
  
  X3 = X1 + X2;
  
  B3 = ((((long)AC1 * 4 + X3) << OSS) + 2) / 4;
  
  X1 = (long)AC3 * B6 / pow(2, 13);
  
  X2 = ((long)BB1 * B6 * B6 / pow(2, 12)) / pow(2, 16);
  
  X3 = ((X1 + X2) + 2) / pow(2, 2);
  
  B4 = (long)AC4 * (unsigned long)(X3 + 32768) / pow(2, 15);
  
  B7 = ((unsigned long)UP - B3) * (50000 >> OSS);
  
  if(B7 < 0x80000000){
  
    P = (B7 * 2) / B4;
    
  }
  else{
  
    P = (B7 / B4) * 2;
    
  }
  
  X1 = (P / pow(2, 8)) * (P / pow(2, 8));
  
  X1 = (X1 * 3038) / pow(2, 16);
  
  X2 = (-7357 * P) / pow(2, 16);
  
  P = P + (X1 + X2 + 3791) / pow(2, 4);
  
  return P;
  
}

double Weather::readPressureBars(void){

  return (double)readPressurePascals() / 100000;
  
}

int Weather::readHeight(void){
  
  long seaLevelPress = 100900; // 1 bar
  
  return 44330 * (1 - pow(((double)readPressurePascals() / seaLevelPress), 1/5.255));
  
}

double Weather::readTemp(void){
  
  long UT;

  // Read uncompensated temperature value
  
  writeRegister(0xF4, 0x2E);
  
  // Delay
  
  delay(DELAY_OSS);
  
  UT = readRegister(0xF6) << 8 | readRegister(0xF7);
  
  // Calculate true temperature
  
  return ((UT - (long)AC6) * (long)AC5 / pow(2, 15) + (long)MC * pow(2, 11) / ((UT - (long)AC6) * (long)AC5 / pow(2, 15) + (long)MD) + 8) / pow(2, 4) * 0.1;

}

// Humidity sensor functions

double Weather::readHumidity(void){
  
  byte i;
  
  unsigned long startTime, stopTime;
  
  static unsigned long lastCall;
  
  int humidity, temperature = 0;
  
  byte crc = 0;
  
  // Check if two seconds have already passed since the last function call
  
  if(micros() - lastCall >= 2000000){
  
    setOutput();
 
    // Set pin to low for 5ms
  
    writeZero();
  
    _delay_ms(10);
 
    // Set pin to high for 40us
  
    writeOne();
  
    _delay_us(40);
  
    // Set pin to input
  
    setInput();
  
    // Wait for the response from the slave
  
    while(readPin() != 1);while(readPin() != 0);
  
    //  Read humidity
 
    for(i = 0; i < DATA_BIT_COUNT; i++){
    
      // Wait for the beginning of a high signal
      
      while(readPin() != 1){};
      
      // Capture time
      
      startTime = micros();
      
      // Wait for the beginning of a low signal
      
      while(readPin() != 0){};
      
      stopTime = micros();
      
      if(i >= 0 && i <= 15){
        
        (stopTime - startTime > 40) ? humidity |= (1 << 15 - i) : humidity &= ~(1 << 15 - i);
      
      }
      else if(i >= 16 && i <= 31){
      
        (stopTime - startTime > 40) ? temperature |= (1 << 31 - i) : temperature &= ~(1 << 31 - i);
        
      }
      else{
    
        (stopTime - startTime > 40) ? crc |= (1 << 39 - i) : crc &= ~(1 << 39 - i);
        
      }

    }
    
    // Set latest time to the lastCall
    
    lastCall = micros();
	
    // Check checksum
	
    if(crc == humidity + temperature >> 8){
		
        // Check if decimal part is not zero
		
            if(humidity & 0xFF != 0){
			
                return (humidity >> 8) + (humidity & 0xFF) / pow(10, (int)log10(humidity & 0xFF) + 1);
			
            }
		
	    return humidity >> 8;
		
     }
    
  }
  
  return -1;
  
}

// Private functions


int Weather::readCalibrationCoef(byte msb, byte lsb){
  
  int value;

  Wire.beginTransmission(BMP085_I2C_ADDRESS);
  
  Wire.write(msb);
  
  Wire.write(lsb);
  
  Wire.endTransmission();
  
  Wire.requestFrom(BMP085_I2C_ADDRESS, 2);
  
  while(!Wire.available());
  
  value = Wire.read();
  
  value = value << 8;
  
  value |= Wire.read();
  
  return value;
  
}

void Weather::writeRegister(byte reg, byte value){

  Wire.beginTransmission(BMP085_I2C_ADDRESS);
  
  Wire.write(reg);
  
  Wire.write(value);
  
  Wire.endTransmission();
  
}

byte Weather::readRegister(byte reg){

  Wire.beginTransmission(BMP085_I2C_ADDRESS);
  
  Wire.write(reg);
  
  Wire.endTransmission();
  
  Wire.requestFrom(BMP085_I2C_ADDRESS, 1);
  
  while(!Wire.available());
  
  return Wire.read();
  
}

byte Weather::getPort(byte pin){

  // Get proper port number from pin number 

  if(pin >= 8 && pin <= 13)
    return SEL_PORTB;

  else if(pin >= 14 && pin <= 19)
    return SEL_PORTC;

  else
    return SEL_PORTD; // Return PORTD as a default setting
  
}

byte Weather::getPin(byte pin){

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

void Weather::setOutput(void){

  if(portDht22 == SEL_PORTC){
    
    DDRC |= (1 << pinDht22);

  }
  else if(portDht22 == SEL_PORTB){
    
    DDRB |= (1 << pinDht22);
    
  }
  else{
    
    DDRD |= (1 << pinDht22);

  }
  
}

void Weather::setInput(void){

  if(portDht22 == SEL_PORTC){
    
    DDRC &= ~(1 << pinDht22);

  }
  else if(portDht22 == SEL_PORTB){
    
    DDRB &= ~(1 << pinDht22);
    
  }
  else{
    
    DDRD &= ~(1 << pinDht22);

  }
  
}

void Weather::writeOne(void){

  if(portDht22 == SEL_PORTC){
    
    PORTC |= (1 << pinDht22);

  }
  else if(portDht22 == SEL_PORTB){
    
    PORTB |= (1 << pinDht22);
    
  }
  else{
    
    PORTD |= (1 << pinDht22);

  }
  
}

void Weather::writeZero(void){

  if(portDht22 == SEL_PORTC){
    
    PORTC &= ~(1 << pinDht22);

  }
  else if(portDht22 == SEL_PORTB){
    
    PORTB &= ~(1 << pinDht22);
    
  }
  else{
    
    PORTD &= ~(1 << pinDht22);

  }
  
}

byte Weather::readPin(void){

  if(portDht22 == SEL_PORTC){
    
    return (PINC >> pinDht22) & 1;

  }
  else if(portDht22 == SEL_PORTB){
    
    return (PINB >> pinDht22) & 1;
    
  }
  else{
    
    return (PIND >> pinDht22) & 1;

  }
  
}
