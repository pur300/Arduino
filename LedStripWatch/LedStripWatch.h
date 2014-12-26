#ifndef LEDSTRIPWATCH_H
#define LEDSTRIPWATCH_H

// Defines

#define NUMOFLEDS 60
#define HOURGAP NUMOFLEDS / 12

// Struct for time colors

typedef struct{

    // R
    
    uint8_t r;
    
    // G
    
    uint8_t g;
    
    // B
    
    uint8_t b;

} timeColor;

class LedStripWatch : public Ws2812b{

    private:
        timeColor hourC, minutesC, otherC;
        uint8_t hour, minutes, seconds;
        void setClockColor(void);
    
    public:
        LedStripWatch(uint8_t pin);
        void setClock(uint8_t hour, uint8_t minutes, uint8_t seconds);
        void incrementClock(void);
        void setHourColor(uint8_t r, uint8_t g, uint8_t b);
        void setMinutesColor(uint8_t r, uint8_t g, uint8_t b);
        void setOtherColor(uint8_t r, uint8_t g, uint8_t b);
    
    

};

#endif