#include <Ws2812b.h>

#include <LedStripWatch.h>

#include <Ds1307.h>

#include <Wire.h>

#include <EEPROM.h>

#include <SoftwareSerial.h>

#define CLOCK 1
#define COLORS 2
#define BRIGHTNESS 3
#define NIGHTMODE 4
#define TERMIN 5
#define INFO 6

// Global variables and objects

Ds1307 clock;

LedStripWatch ledWatch(7);

SoftwareSerial bluetooth(3, 4); // RX, TX

uint8_t months, days, hours, minutes, seconds; // Temp variables for time

short years; 

uint8_t type; // Message type

uint8_t nightModeFlag; // Night mode flag

uint8_t from, to; // Night mode time interval

uint8_t brightness; // Current brightness(if night mode is off)

void setup(){
  
  // Set default clock colors in EEPROM
  
  if(EEPROM.read(0) == 255){
  
    EEPROM.write(0, 0);
    
    // Set default color for hours
    
    EEPROM.write(1, 40);
    
    EEPROM.write(2, 0);
    
    EEPROM.write(3, 0);
    
    // Set default color for minutes
    
    EEPROM.write(4, 0);
    
    EEPROM.write(5, 40);
    
    EEPROM.write(6, 0);
    
    // Set default color for others
    
    EEPROM.write(7, 0);
    
    EEPROM.write(8, 0);
    
    EEPROM.write(9, 40);
    
    // Set default brightness
    
    EEPROM.write(10, 255);
    
    // Enabled/disabled night mode
    
    EEPROM.write(11, 255);
    
    // Night mode time period
    
    EEPROM.write(12, 0); // From
    
    EEPROM.write(13, 7); // To
    
  }
  
  // Init clock
  
  clock.begin();
  
  // Init bluetooth serial
  
  bluetooth.begin(9600);
  
  // Read brightness
  
  brightness = EEPROM.read(10);
  
  // Night mode
  
  nightModeFlag = EEPROM.read(11);
  
  // Read and set colors from eeprom

  ledWatch.setHourColor(EEPROM.read(1), EEPROM.read(2), EEPROM.read(3));
  
  ledWatch.setMinutesColor(EEPROM.read(4), EEPROM.read(5), EEPROM.read(6));
  
  ledWatch.setOtherColor(EEPROM.read(7), EEPROM.read(8), EEPROM.read(9));
  
  // Night mode period
  
  from = EEPROM.read(12);
  
  to = EEPROM.read(13);
  
}

void loop(){
  
  // Check for commands
  
  if(bluetooth.available() && bluetooth.read() == '+'){
  
    // beginning of a command
    
    type = readSegment();
   
    if(type == CLOCK){
    
      // Change time command
      
      years = readSegment() + 2000;
      
      months = readSegment();
      
      days = readSegment();
      
      hours = readSegment();
      
      minutes = readSegment();
      
      seconds = readSegment();
      
      // Set clock
      
      clock.setClock(years, months, days, hours, minutes, seconds);
      
      // Send ok response
      
      bluetooth.write("+0:");
      
    }
    else if(type == COLORS){
    
      // Change colors command
      
      uint8_t i;
      
      for(i = 1; i <= 9; i++){
      
        EEPROM.write(i, readSegment());
        
      }
      
      // Read and set colors from eeprom

      ledWatch.setHourColor(EEPROM.read(1), EEPROM.read(2), EEPROM.read(3));
  
      ledWatch.setMinutesColor(EEPROM.read(4), EEPROM.read(5), EEPROM.read(6));
  
      ledWatch.setOtherColor(EEPROM.read(7), EEPROM.read(8), EEPROM.read(9));
      
      // Send ok response
      
      bluetooth.write("+0:");
      
    }
    else if(type == BRIGHTNESS){
    
      // Set brightness
      
      EEPROM.write(10, readSegment());
      
      // Read brightness from eeprom
      
      brightness = EEPROM.read(10);
      
      // Send ok response
      
      bluetooth.write("+0:");
          
    }
    else if(type == NIGHTMODE){
    
      // Turn on/off night mode
      
      nightModeFlag = readSegment();
      
      // Save to eeprom
      
      EEPROM.write(11, nightModeFlag);
      
      // Send ok response
      
      bluetooth.write("+0:");
      
    }
    else if(type == TERMIN){
    
      // Night mode period
      
      from = readSegment();
      
      to = readSegment();
      
      // Save to eeprom
      
      EEPROM.write(12, from);
      
      EEPROM.write(13, to);
      
      // Send ok response
      
      bluetooth.write("+0:");
      
    }
    else if(type == INFO){
      
      uint8_t i;
    
      // Send data to the mobile phone
      
      bluetooth.write("+6:");
      
      // Send time
      
      clock.now(&years, &months, &days, &hours, &minutes, &seconds);
      
      bluetooth.print((years - 2000)); bluetooth.print(":");
      
      bluetooth.print(months); bluetooth.print(":");
      
      bluetooth.print(days); bluetooth.print(":");
      
      bluetooth.print(hours); bluetooth.print(":");
      
      bluetooth.print(minutes); bluetooth.print(":");
      
      bluetooth.print(seconds); bluetooth.print(":");
      
      // Send colors
      
      for(i = 1; i <= 9; i++){
      
        bluetooth.print(EEPROM.read(i)); bluetooth.print(":");
        
      }
      
      // Send brightness
      
      bluetooth.print(EEPROM.read(10)); bluetooth.print(":");
      
      // Send night mode
      
      bluetooth.print(EEPROM.read(11)); bluetooth.print(":");
      
      // Send termin mode
      
      bluetooth.print(EEPROM.read(12)); bluetooth.print(":");
      
    }
    
  }
  
  // Read current clock
 
  clock.now(&years, &months, &days, &hours, &minutes, &seconds);
  
  // Night mode
  
  if(nightModeFlag && hours >= from && hours <= to){
  
    // Set brightness to 30 %
    
    ledWatch.setBrightness(76);
    
  }
  else{
  
    // Set brightness to value selected by user
    
    ledWatch.setBrightness(brightness);
    
  }
  
  // Refresh led strip clock... All previously changed settings are now visible
    
  ledWatch.setClock(hours, minutes, seconds);
  
  delay(10); // Ugly
  
}

uint8_t readSegment(void){

  char buffer[5];
  
  uint8_t counter = 0;
  
  for(;;){
    
    // Wait for data
  
    while(!bluetooth.available());
    
    // Read data
    
    if((buffer[counter] = bluetooth.read()) != ':')
      counter++; // Increase counter
    else
      break;
    
  }
  
  buffer[counter] = '\0';
  
  return atoi(buffer);
  
}


