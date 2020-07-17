#ifndef LEGOFAN_CONFIG_H
#define LEGOFAN_CONFIG_H 1

/* Samsung remote keys (Upper 16 bits: e0e0)
 *  Up     Down  Left  Right    06f9  8679  a659  46b9
 *  Enter                       16e9
 *  A      B     C     D        36c9  28d7  a857  6897
 *  
 *  Commands:
 *    Up/Down: fan speed faster/slower
 *    A: enable/disable rotation
 *    B: enter left limit edit mode
 *    C: enter right limit edit mode
 *    D: enter stop time edit mode
 *    Enter: Exit (!) from any edit mode
 *    Left/Right:
 *      when rotation is enabled: edit rotation speed slower/faster
 *      when rotation is disabled: rotate left/right 
 *      when in limit edit mode: modify limit left/right
 *      when in stop time edit mode: modify stop time less/more
 */


// Set to 1 to enable the serial state dump
#define LF_DEBUG 0


// Messing with timer 0 changes how long a delay()'s msec is
// The target cycle duration is 5 msec
// Default clock prescaler: 64
// Clock prescaler now: 1
const unsigned int CYCLE_DURATION = 320;


// Z rotation pot is garbage (not anymore tho), so we average the last n readings
// to get a more or less stable value (one reading per cycle)
const unsigned char NUM_Z_ROTATION_READINGS = 3;


// Settings are saved to EEPROM after change, but only after this number of
// cycles have passed to avoid write spamming. The counter is reset on every change
const unsigned short EEPROM_FLUSH_DELAY = 1600;  // 8 seconds


// This is the number of cycles the runtime waits before resetting the last_ir_code
// variable. If two identical codes are received within this time interval the last
// one is considered as a repetition of the last code
const unsigned short IR_CLEAR_LAST_DELAY = 35;  // 175 msec


// IR activity led configuration
const unsigned short IR_ACTIVITY_DURATION  = 40;  // Total duration. 200 msecs
const unsigned short IR_ACTIVITY_PHASE_LEN = 10;  // After how many cycles the led toggles. 50 msec


// Limit edit servo mode speed
const unsigned char LIMIT_EDIT_SPEED = 112;


// Manual rotation speed... duh
const unsigned char MANUAL_ROT_SPEED = 112;


// How many cycles to wait before stopping the rotation motor in static mode
// when rotating manually
const unsigned short MANUAL_ROTATION_STOP_DELAY = 50;


// Single press steps
const unsigned short ROT_SPEED_STEP = 1024;
const unsigned short FAN_SPEED_STEP = 1024;
const unsigned short LIMIT_STEP     = 16;
const unsigned short PAUSE_STEP     = 125;


#endif
