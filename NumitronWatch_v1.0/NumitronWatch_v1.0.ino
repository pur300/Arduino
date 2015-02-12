#include <SPI.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetUdp.h>
#include <Ds1307.h>
#include <SPI.h>
#include <Wire.h>
#include <EthernetUdp.h>
#include <NumitronTime.h>
#include <Time.h>

// Vars and objects

NumitronTime numitronTubesTime(17, 15, 16); // Time on numitron tubes. Input, latch, clock

Ds1307 time;

EthernetUDP udp;

byte mac[] = {0xDC, 0x30, 0xBF, 0xBC, 0x84, 0x94};

short years;
  
uint8_t months, days, hours, minutes, seconds;

boolean volatile interruptFlag = false;

long nextNtpPoint;


void setup(){
  
  // Init time module
  
  time.begin();
  
  // Enable square pin on a Ds1307
  
  time.enableSquarePin();
  
  // Attach interrupt int0
  
  attachInterrupt(0, interruptRoutine, RISING);
  
  // Set time
  
  setTime();
  
}


void loop(){

  // Check if an interrupt flag is set
  
  if(interruptFlag){
  
    // Get time from a time module
    
    time.now(&years, &months, &days, &hours, &minutes, &seconds);
    
    // Check if time needs to be changed
    
    if(dateTimeToSec(years, months, days, hours, minutes, seconds) >= nextNtpPoint){
    
      // Set new time;
      
      setTime();
      
      // Get time from a time module
    
      time.now(&years, &months, &days, &hours, &minutes, &seconds);
      
    }
    
    // Update time on tubes
    
    numitronTubesTime.setClock(hours, minutes, seconds);
    
    // Clear interrupt flag
    
    interruptFlag = false;
    
  }
  

}


/*
 * © Francesco Potortì 2013 - GPLv3 - Revision: 1.13
 *
 * Send an NTP packet and wait for the response, return the Unix time
 *
 * To lower the memory footprint, no buffers are allocated for sending
 * and receiving the NTP packets.  Four bytes of memory are allocated
 * for transmision, the rest is random garbage collected from the data
 * memory segment, and the received packet is read one byte at a time.
 * The Unix time is returned, that is, seconds from 1970-01-01T00:00.
 */
 
unsigned long ntpUnixTime (UDP &udp){
  
  static int udpInited = udp.begin(123); // open socket on arbitrary port

  const char timeServer[] = "ntp2.arnes.si";  // NTP server

  // Only the first four bytes of an outgoing NTP packet need to be set
  // appropriately, the rest can be whatever.
  const long ntpFirstFourBytes = 0xEC0600E3; // NTP request header

  // Fail if WiFiUdp.begin() could not init a socket
  if (! udpInited)
    return 0;

  // Clear received data from possible stray received packets
  udp.flush();

  // Send an NTP request
  if (! (udp.beginPacket(timeServer, 123) // 123 is the NTP port
	 && udp.write((byte *)&ntpFirstFourBytes, 48) == 48
	 && udp.endPacket()))
    return 0;				// sending request failed

  // Wait for response; check every pollIntv ms up to maxPoll times
  const int pollIntv = 150;		// poll every this many ms
  const byte maxPoll = 15;		// poll up to this many times
  int pktLen;				// received packet length
  for (byte i=0; i<maxPoll; i++) {
    if ((pktLen = udp.parsePacket()) == 48)
      break;
    delay(pollIntv);
  }
  if (pktLen != 48)
    return 0;				// no correct packet received

  // Read and discard the first useless bytes
  // Set useless to 32 for speed; set to 40 for accuracy.
  const byte useless = 40;
  for (byte i = 0; i < useless; ++i)
    udp.read();

  // Read the integer part of sending time
  unsigned long time = udp.read();	// NTP time
  for (byte i = 1; i < 4; i++)
    time = time << 8 | udp.read();

  // Round to the nearest second if we want accuracy
  // The fractionary part is the next byte divided by 256: if it is
  // greater than 500ms we round to the next second; we also account
  // for an assumed network delay of 50ms, and (0.5-0.05)*256=115;
  // additionally, we account for how much we delayed reading the packet
  // since its arrival, which we assume on average to be pollIntv/2.
  time += (udp.read() > 115 - pollIntv/8);

  // Discard the rest of the packet
  udp.flush();

  return time - 2208988800ul;		// convert NTP time to Unix time
}

boolean checkConnection(void){

  // vars
  
  EthernetClient client;
  
  if(client.connect("www.google.com", 80) == 1){
    
    // Close connection
    
    client.stop();
    
    return true;
    
  }
    
  return false;
  
}

boolean dhcpRequest(){
  
  if(Ethernet.begin(mac) == 1){
  
    return true;
    
  }
  
  return false;
  
}

void setTime(void){
  
  short years;
  
  uint8_t months, days, hours, minutes, seconds;
  
  boolean isConnected;
  
  unsigned long summerTimePoint, nextSummerTime, winterTimePoint, currentTimePoint;
  
  unsigned long unixTime = 0;
  
  // Check if connected to the network. If not try to make dhcp request
  
  if(checkConnection() || dhcpRequest()){
    
    // Set isConnected to true
      
    isConnected = true;

  }
  
  // Both failed
  
  else{
    
    // Set isConnected to false
      
    isConnected = false;

      
  }
  
  // If connected then make a ntp request
  
  if(isConnected && (unixTime = ntpUnixTime(udp))){
    
    // Set time and date from ntp request
    
    years = year(unixTime);
    
    months = month(unixTime);
    
    days = day(unixTime);
    
    hours = hour(unixTime);
    
    minutes = minute(unixTime);
    
    seconds = second(unixTime);
    
    // Get time points
    
    currentTimePoint = dateTimeToSec(years, months, days, hours, minutes, seconds);
      
    summerTimePoint = dateTimeToSec(years, 3, getSummerTimeDay(years), 3, 0, 0);
      
    winterTimePoint = dateTimeToSec(years, 10, getSummerTimeDay(years), 3, 0, 0);
    
    nextSummerTime = dateTimeToSec(years + 1, 3, getSummerTimeDay(years + 1), 3, 0, 0);
    
    // Check winter time
    
    if(currentTimePoint >= winterTimePoint){
    
      nextNtpPoint = nextSummerTime;
      
      time.setClock(years, months, days, (hours + 1) % 24, minutes, seconds);
      
    }
    
    // Check for summer time
    
    else if(currentTimePoint < summerTimePoint){
    
      nextNtpPoint = winterTimePoint;
      
      time.setClock(years, months, days, (hours + 1) % 24, minutes, seconds);
      
    }
    
    // Winter time
    
    else{
    
      nextNtpPoint = summerTimePoint;
      
      time.setClock(years, months, days, (hours + 1) % 24, minutes, seconds);
      
    }
 
  }
  else{
      
    // Read current date and time from a Ds1307
      
    time.now(&years, &months, &days, &hours, &minutes, &seconds);
    
    // Set nextNtpPoint
    
    nextNtpPoint = dateTimeToSec(years, months, days, hours, minutes, seconds) + 60;

  }
  
}

uint8_t getWinterTimeDay(long yr){

  return 31 - (1 + 5 * yr / 4) % 7;
  
}

uint8_t getSummerTimeDay(long yr){
  
  return 31 - (4 + 5 * yr / 4) % 7;

}

long dateTimeToSec(long years, uint8_t months, uint8_t days, uint8_t hours, uint8_t minutes, uint8_t seconds){

  return years * 365 * 31 * 24 * 3600 + months * 31 * 24 * 3600 + days * 24 * 3600 + hours * 3600 + minutes * 60 + seconds;
   
}

void interruptRoutine(void){

  // Set interrupt flag
  
  interruptFlag = true;
  
}

