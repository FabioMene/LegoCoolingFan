#include <Arduino.h>

#include "timers.h"


void rot_init(){
    // Set pins as output
    DDRD |= _BV(5) | _BV(6);  // dp5/oc0b/left rot, dp6/oc0a/right rot

    // Init timer0: fast PWM mode (top: 0xff), enable oc0a and oc0b
    // No prescaler to get past the audible range
    TCCR0A = _BV(COM0A1) | _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);
    TCCR0B = _BV(CS00);

    // Both outputs at 0
    OCR0A = 0;  // r
    OCR0B = 0;  // l
}


// There is no bound check/clamp on val
void rot_set_speed(short val){
    if(val > 0){
        // rotate right
        OCR0A = val;
        OCR0B = 0;
    
    } else if(val < 0){
        // rotate left
        OCR0A = 0;
        OCR0B = -val;
    
    } else {
        // corner case
        OCR0A = 0;
        OCR0B = 0;
    }
}



void fan_init(){
    DDRB |= _BV(2);  // dp10/oc1b

    // Init timer1: fast PWM mode (top: OCR1A), enable inverted OC1B
    // no prescaler
    TCCR1A = _BV(COM1B0) | _BV(COM1B1) | _BV(WGM10);
    TCCR1B = _BV(WGM13) | _BV(CS10);
    
    OCR1A = 320;  // 16000000 (clock freq) / 320 (319 + 1) = 50000 -> 25kHz
    OCR1B = 0;
}

void fan_set_speed(unsigned short spd){
    OCR1B = spd / 205;  // 65535 / 320 = 204.7968 ~= 205
}
