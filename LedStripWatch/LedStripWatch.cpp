#include "LedStripWatch.h"

LedStripWatch::LedStripWatch(uint8_t pin):Ws2812b(pin, NUMOFLEDS){

    // Set initial color for hour
    
    this->hourC.r = 50;
    
    this->hourC.g = 0;
    
    this->hourC.b = 0;
    
    // Set initial color for minutes
    
    this->minutesC.r = 0;
    
    this->minutesC.g = 50;
    
    this->minutesC.b = 0;
    
    // Set initial color for other
    
    this->otherC.r = 0;
    
    this->otherC.g = 0;
    
    this->otherC.b = 50;
    
    // Set hour, minutes and seconds to zero
    
    this->hour = this->minutes = this->seconds = 0;

}

bool LedStripWatch::setClock(uint8_t hour, uint8_t minutes, uint8_t seconds){

    // check if all is within proper range
    
    if(hour >= 0 && hour <= 23 && minutes >= 0 && minutes <= 59 && seconds >= 0 && seconds <= 59){
    
        this->hour = hour;
            
        this->minutes = minutes;
    
        this->seconds = seconds;
        
        // Set colors
        
        this->setClockColor();
        
        // Refresh strip and return true
        
        this->refresh();
        
        return true;

    }
    
    return false;
}

void LedStripWatch::incrementClock(void){
    
    // Increment clock
    
    if((this->seconds = ((++this->seconds) % 60)) == 0 && (this->minutes = ((++this->minutes) % 60)) == 0 && (this->hour = ((++this->hour) % 12))  == 0);
    
    // Set colors and refresh
    
    this->setClockColor();
    
    this->refresh();

}

void LedStripWatch::setHourColor(uint8_t r, uint8_t g, uint8_t b){

    this->hourC.r = r;
    
    this->hourC.g = g;
    
    this->hourC.b = b;
    
    // Set colors and refresh
    
    this->setClockColor();
    
    this->refresh();

}

void LedStripWatch::setMinutesColor(uint8_t r, uint8_t g, uint8_t b){

    this->minutesC.r = r;
    
    this->minutesC.g = g;
    
    this->minutesC.b = b;
    
    // Set colors and refresh
    
    this->setClockColor();
    
    this->refresh();

}

void LedStripWatch::setOtherColor(uint8_t r, uint8_t g, uint8_t b){

    this->otherC.r = r;
    
    this->otherC.g = g;
    
    this->otherC.b = b;
    
    // Set colors and refresh
    
    this->setClockColor();
    
    this->refresh();

}

void LedStripWatch::setClockColor(void){

    short i;

    // Set color to the hour, minutes and other
        
    for(i = 0; i < NUMOFLEDS; i++){
        
        if(i == this->hour * HOURGAP){
          
            // Set color to the hour. 
        
            this->setColorAt(i, this->hourC.r, this->hourC.g, this->hourC.b);
            
        }
        else if(i <= this->minutes && i != this->hour * HOURGAP){
            
            // Set color to minutes
                
            this->setColorAt(i, this->minutesC.r, this->minutesC.g, this->minutesC.b);
            
        }
        else{
            
            // Set color to other
                
            this->setColorAt(i, this->otherC.r, this->otherC.g, this->otherC.b);
            
        }
        
    }

}

