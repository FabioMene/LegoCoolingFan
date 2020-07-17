#ifndef LEGOFAN_TIMERS_H
#define LEGOFAN_TIMERS_H 1

// Inits timer 0 and output pins
void rot_init();

// Sets the rotation motor speed. positive/negative translates to right/left
// Value range is [-255, 255]
void rot_set_speed(short val);


// Inits timer 1 and the output pin
void fan_init();

// Sets the fan speed. The range is [0, 65535]
void fan_set_speed(unsigned short spd);

#endif
