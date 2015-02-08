#include "Ws2812b.h" 

// Constructor

Ws2812b::Ws2812b(uint8_t pin, short numOfLeds){

  // Set number of pins

  this->numOfLeds = numOfLeds;

  // Set port

  this->port = Ws2812b::getPort(pin);

  // Set proper pin

  this->pin = Ws2812b::get07Pin(pin);

  // Set output pin

  if(this->port == SEL_PORTC){
    
    DDRC |= (1 << this->pin);
    
    PORTC &= ~(1 << this->pin);

  }
  else if(this->port == SEL_PORTB){
    
    DDRB |= (1 << this->pin);
    
    PORTB &= ~(1 << this->pin);
    
  }
  else{
    
    DDRD |= (1 << this->pin);
    
    PORTD &= ~(1 << this->pin);

  }

  // Create dynamic array for led color values

  this->ledStrip = (uint8_t *) malloc(numOfLeds * 3 * sizeof(uint8_t));
  
  // Set brightness to 1
  
  this->brightness = 1;

}


void Ws2812b::setColor(uint8_t r, uint8_t g, uint8_t b){

  short i;

  // Set color

  for(i = 3; i <= this->numOfLeds * 3; i+=3){

    // Set blue

    this->ledStrip[i - 1] = b * this->brightness;

    // Set red

    this->ledStrip[i - 2] = r * this->brightness;

    // Set green

    this->ledStrip[i - 3] = g * this->brightness;


  }

}

void Ws2812b::setColorAt(short pos, uint8_t r, uint8_t g, uint8_t b){

  // Check if position is in range

  if(pos >= 0 && pos < this->numOfLeds){

    // Set blue

    this->ledStrip[(pos + 1) * 3 - 1] = b * this->brightness;

    // Set red

    this->ledStrip[(pos + 1) * 3 - 2] = r * this->brightness;

    // Set green

    this->ledStrip[(pos + 1) * 3 - 3] = g * this->brightness;

  }

}

void Ws2812b::refresh(void){

  // Variables are used to store current port value. Bits at location indicated by pin variable are set and cleared in order to generate proper signal that set color to each led.

  volatile uint8_t highPort;

  volatile uint8_t lowPort;

  // Save location of the first element in array. Variable is used by assembly code to iterate through array of rgb values for each led in a strip

  volatile uint8_t * ptr = this->ledStrip;

  // Disable interrupts

  cli();

  // PORTD

  if(this->port == SEL_PORTD){
    
    highPort = PORTD | (1 << this->pin);

    lowPort = PORTD & ~(1 << this->pin);

    asm volatile("mov r16, %0" :: "r" (this->numOfLeds * 3) : "r16");
    asm volatile("ldi r17, %0" :: "I" (8)  : "r17");
    asm volatile("fetchByteD:");
    asm volatile("ld r18, %a0+" :: "e" (ptr)  : "r18"); // 2 cycles
    asm volatile("startD:");
    asm volatile("out %0, %1" :: "I" (_SFR_IO_ADDR(PORTD)), "r" (highPort)); // ? cycles
    asm volatile("rol r18" ::: "r18"); // 1 cycle
    asm volatile("brcc continueZeroD"); // 1/2 cycles
    asm volatile("dec r17" ::: "r17"); // 1 cycle
    asm volatile("tst r17" ::: "r17"); // 1 cycle
    asm volatile("brne continueOneD"); // 1/2 cycles
    asm volatile("dec r16" ::: "r16"); // 1 cycle
    asm volatile("tst r16" ::: "r16"); // 1 cycle
    asm volatile("breq endOneD"); // 1/2 cycles
    asm volatile("ldi r17, %0" :: "I" (8) : "r17"); // 1 cycle
    asm volatile("rjmp .+0"); // 2 cycles
    asm volatile("out %0, %1" :: "I" (_SFR_IO_ADDR(PORTD)), "r" (lowPort)); // ? cycles
    asm volatile("rjmp fetchByteD"); // 2 cycles

    asm volatile("continueZeroD:");
    asm volatile("out %0, %1" :: "I" (_SFR_IO_ADDR(PORTD)), "r" (lowPort)); // ? cycles
    asm volatile("dec r17" ::: "r17"); // 1 cycle
    asm volatile("tst r17" ::: "r17"); // 1 cycle
    asm volatile("brne skipZeroFetchD"); // 1/2 cycles
    asm volatile("dec r16" ::: "r16"); // 1 cycle
    asm volatile("tst r16" ::: "r16"); // 1 cycle
    asm volatile("breq endZeroD"); // 1/2 cycles
    asm volatile("ldi r17, %0" :: "I" (8): "r17"); // 1 cycle
    asm volatile("rjmp fetchByteD"); // 2 cycles

    asm volatile("skipZeroFetchD:");
    asm volatile("rjmp .+0"); // 2 cycles
    asm volatile("rjmp .+0"); // 2 cycles
    asm volatile("nop"); // 1 cycle
    asm volatile("rjmp startD"); // 2 cycles

    asm volatile("continueOneD:");
    asm volatile("rjmp .+0"); // 2 cycles
    asm volatile("rjmp .+0"); // 2 cycles
    asm volatile("nop"); // 1 cycle
    asm volatile("out %0, %1" :: "I" (_SFR_IO_ADDR(PORTD)), "r" (lowPort)); // ? cycles
    asm volatile("nop"); // 1 cycle
    asm volatile("rjmp startD"); // 2 cycles

    asm volatile("endOneD:");
    asm volatile("rjmp .+0"); // 2 cycles
    asm volatile("nop"); // 1 cycle
    asm volatile("out %0, %1" :: "I" (_SFR_IO_ADDR(PORTD)), "r" (lowPort)); // ? cycles
    asm volatile("endZeroD:");

  }

  // PORTC

  else if(this->port == SEL_PORTC){

    highPort = PORTC | (1 << this->pin);

    lowPort = PORTC & ~(1 << this->pin);

    asm volatile("mov r16, %0" :: "r" (this->numOfLeds * 3) : "r16");
    asm volatile("ldi r17, %0" :: "I" (8)  : "r17");
    asm volatile("fetchByteC:");
    asm volatile("ld r18, %a0+" :: "e" (ptr)  : "r18"); // 2 cycles
    asm volatile("startC:");
    asm volatile("out %0, %1" :: "I" (_SFR_IO_ADDR(PORTC)), "r" (highPort)); // ? cycles
    asm volatile("rol r18" ::: "r18"); // 1 cycle
    asm volatile("brcc continueZeroC"); // 1/2 cycles
    asm volatile("dec r17" ::: "r17"); // 1 cycle
    asm volatile("tst r17" ::: "r17"); // 1 cycle
    asm volatile("brne continueOneC"); // 1/2 cycles
    asm volatile("dec r16" ::: "r16"); // 1 cycle
    asm volatile("tst r16" ::: "r16"); // 1 cycle
    asm volatile("breq endOneC"); // 1/2 cycles
    asm volatile("ldi r17, %0" :: "I" (8) : "r17"); // 1 cycle
    asm volatile("rjmp .+0"); // 2 cycles
    asm volatile("out %0, %1" :: "I" (_SFR_IO_ADDR(PORTC)), "r" (lowPort)); // ? cycles
    asm volatile("rjmp fetchByteC"); // 2 cycles

    asm volatile("continueZeroC:");
    asm volatile("out %0, %1" :: "I" (_SFR_IO_ADDR(PORTC)), "r" (lowPort)); // ? cycles
    asm volatile("dec r17" ::: "r17"); // 1 cycle
    asm volatile("tst r17" ::: "r17"); // 1 cycle
    asm volatile("brne skipZeroFetchC"); // 1/2 cycles
    asm volatile("dec r16" ::: "r16"); // 1 cycle
    asm volatile("tst r16" ::: "r16"); // 1 cycle
    asm volatile("breq endZeroC"); // 1/2 cycles
    asm volatile("ldi r17, %0" :: "I" (8): "r17"); // 1 cycle
    asm volatile("rjmp fetchByteC"); // 2 cycles

    asm volatile("skipZeroFetchC:");
    asm volatile("rjmp .+0"); // 2 cycles
    asm volatile("rjmp .+0"); // 2 cycles
    asm volatile("nop"); // 1 cycle
    asm volatile("rjmp startC"); // 2 cycles

    asm volatile("continueOneC:");
    asm volatile("rjmp .+0"); // 2 cycles
    asm volatile("rjmp .+0"); // 2 cycles
    asm volatile("nop"); // 1 cycle
    asm volatile("out %0, %1" :: "I" (_SFR_IO_ADDR(PORTC)), "r" (lowPort)); // ? cycles
    asm volatile("nop"); // 1 cycle
    asm volatile("rjmp startC"); // 2 cycles

    asm volatile("endOneC:");
    asm volatile("rjmp .+0"); // 2 cycles
    asm volatile("nop"); // 1 cycle
    asm volatile("out %0, %1" :: "I" (_SFR_IO_ADDR(PORTC)), "r" (lowPort)); // ? cycles
    asm volatile("endZeroC:");

  }

  // PORTB

  else{

    highPort = PORTB | (1 << this->pin);

    lowPort = PORTB & ~(1 << this->pin);

    asm volatile("mov r16, %0" :: "r" (this->numOfLeds * 3) : "r16");
    asm volatile("ldi r17, %0" :: "I" (8)  : "r17");
    asm volatile("fetchByteB:");
    asm volatile("ld r18, %a0+" :: "e" (ptr)  : "r18"); // 2 cycles
    asm volatile("startB:");
    asm volatile("out %0, %1" :: "I" (_SFR_IO_ADDR(PORTB)), "r" (highPort)); // ? cycles
    asm volatile("rol r18" ::: "r18"); // 1 cycle
    asm volatile("brcc continueZeroB"); // 1/2 cycles
    asm volatile("dec r17" ::: "r17"); // 1 cycle
    asm volatile("tst r17" ::: "r17"); // 1 cycle
    asm volatile("brne continueOneB"); // 1/2 cycles
    asm volatile("dec r16" ::: "r16"); // 1 cycle
    asm volatile("tst r16" ::: "r16"); // 1 cycle
    asm volatile("breq endOneB"); // 1/2 cycles
    asm volatile("ldi r17, %0" :: "I" (8) : "r17"); // 1 cycle
    asm volatile("rjmp .+0"); // 2 cycles
    asm volatile("out %0, %1" :: "I" (_SFR_IO_ADDR(PORTB)), "r" (lowPort)); // ? cycles
    asm volatile("rjmp fetchByteB"); // 2 cycles

    asm volatile("continueZeroB:");
    asm volatile("out %0, %1" :: "I" (_SFR_IO_ADDR(PORTB)), "r" (lowPort)); // ? cycles
    asm volatile("dec r17" ::: "r17"); // 1 cycle
    asm volatile("tst r17" ::: "r17"); // 1 cycle
    asm volatile("brne skipZeroFetchB"); // 1/2 cycles
    asm volatile("dec r16" ::: "r16"); // 1 cycle
    asm volatile("tst r16" ::: "r16"); // 1 cycle
    asm volatile("breq endZeroB"); // 1/2 cycles
    asm volatile("ldi r17, %0" :: "I" (8): "r17"); // 1 cycle
    asm volatile("rjmp fetchByteB"); // 2 cycles

    asm volatile("skipZeroFetchB:");
    asm volatile("rjmp .+0"); // 2 cycles
    asm volatile("rjmp .+0"); // 2 cycles
    asm volatile("nop"); // 1 cycle
    asm volatile("rjmp startB"); // 2 cycles

    asm volatile("continueOneB:");
    asm volatile("rjmp .+0"); // 2 cycles
    asm volatile("rjmp .+0"); // 2 cycles
    asm volatile("nop"); // 1 cycle
    asm volatile("out %0, %1" :: "I" (_SFR_IO_ADDR(PORTB)), "r" (lowPort)); // ? cycles
    asm volatile("nop"); // 1 cycle
    asm volatile("rjmp startB"); // 2 cycles

    asm volatile("endOneB:");
    asm volatile("rjmp .+0"); // 2 cycles
    asm volatile("nop"); // 1 cycle
    asm volatile("out %0, %1" :: "I" (_SFR_IO_ADDR(PORTB)), "r" (lowPort)); // ? cycles
    asm volatile("endZeroB:");

  }

  // Enable interrupts

  sei();


}



uint8_t Ws2812b::get07Pin(uint8_t pin){
  
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

uint8_t Ws2812b::getPort(uint8_t pin){
  
  // Get proper port number from pin number 

  if(pin >= 8 && pin <= 13)
    return SEL_PORTB;

  else if(pin >= 14 && pin <= 19)
    return SEL_PORTC;

  else
    return SEL_PORTD; // Return PORTD as a default setting

}

void Ws2812b::clearStrip(void){

  // Set rgb values to 0
  
  this->setColor(0,0,0);
  
  // Refresh strip
  
  this->refresh();

}

void Ws2812b::setBrightness(uint8_t brightness){
  
    // Brightness between 0 and 1
    
    this->brightness = brightness/255.0;
  
}