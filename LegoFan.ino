#include <IRremote.h>

#include "config.h"
#include "timers.h"
#include "settings.h"
#include "utils.h"

#include "dumpdbg.h"

// Rotation states
typedef enum {
    ROT_ST_IDLE = 0,         // Not rotating automatically
    ROT_ST_ROTATING,         // Rotating
    ROT_ST_ROTATION_PAUSED,  // Waiting for rotation to resume
    ROT_ST_SETTING_A_LIMIT   // Servo mode: tries to match ADC's position to *curr_limit_ptr
} rotstate_e;

// IR mode
typedef enum {
    IR_M_ROOT = 0,  // "root" mode
    IR_M_LIMIT,     // limit edit mode
    IR_M_PAUSETIME  // pause time edit mode
} irmode_e;

#if LF_DEBUG
    // Enum values to enum labels
    
    static const char* rotstate_e_labels[] = {
        "IDLE",
        "ROTATING",
        "PAUSED",
        "LIMIT"
    };
    
    static const char* irmode_e_labels[] = {
        "ROOT",
        rotstate_e_labels[3],  // strings are expensive ("LIMIT" btw)
        "PTIME"
    };
#endif

// IR receiver
IRrecv ir(12);  // dp12


void setup() {
    // HW setup, disable interrupts
    cli();

    // Pin setup
    DDRB |= _BV(5);  // dp13/ir activity led

    // Timer 0: rotation motor
    rot_init();
    
    // Timer 1: fan PWM control
    fan_init();
    
    // Timer 2: used by this lib
    ir.enableIRIn();

    // Reenable interrupts
    sei();

    
    // Load settings (or defaults)
    settings_t settings;
    settings_load(&settings);

    // State variables
    rotstate_e rot_state = ROT_ST_IDLE;
    irmode_e   ir_mode   = IR_M_ROOT;

    // Current rotation position
    unsigned short rot;

    // Rotation direction
    bool rot_to_right = true;

    // Used in ROT_ST_SETTING_A_LIMIT rotstate
    unsigned short* curr_limit_ptr = &settings.rot_limit_l;


    // Cycle counters
    CycleCounter pause_cc;  // The top value is not fixed here

    CycleCounter manual_rot_stop_cc(MANUAL_ROTATION_STOP_DELAY);  // ROT_ST_IDLE stops rotations if this is 0

    CycleCounter eeprom_flush_cc(EEPROM_FLUSH_DELAY);  // Settings are saved to eeprom on end

    CycleCounter ir_clear_last_cc(IR_CLEAR_LAST_DELAY);  // last_ir_code is cleared on end

    CycleCounter ir_activity_cc(IR_ACTIVITY_DURATION);  // Blinks the led on dp13 when counting

    // IR state
    decode_results irdec;
    unsigned short last_ir_code = 0;

    // Initial direction is to right, but a limit check
    // is done before setting the direction/speed down below
    // in switch(rot_state)
    if(settings.rot_enable){
        rot_state = ROT_ST_ROTATING;
    }

  #if LF_DEBUG
    // Again this is very memory hard
    
    dd_elem_t dumpdef[] = {
        D_MKCLR(),
        D_MKENUM_L("rot_st", &rot_state, rotstate_e_labels),
        D_MKENUM_L("irmode", &ir_mode,   irmode_e_labels),
        D_MKBOOL_L("rot_r",  &rot_to_right),
        D_MKINT_L(D_U16, "rot", &rot),
        D_MKLF(),
        
        D_MKBOOL_L("roten", &settings.rot_enable),
        D_MKINT_L(D_U16, "lim_l",   &settings.rot_limit_l),
        D_MKINT_L(D_U16, "lim_r",   &settings.rot_limit_r),
        D_MKINT_L(D_U16, "rot_spd", &settings.rot_speed),
        D_MKINT_L(D_U16, "rot_wt",  &settings.rot_pause_msec),
        D_MKINT_L(D_U16, "fan_spd", &settings.fan_speed),
        D_MKLF(),

        D_MKINT_LB(D_U16, "lastIR", &last_ir_code, 16),
        D_MKLF(),

        D_MKSTR("ccnts"),
        D_MKDMP_L("pause",    &pause_cc),
        D_MKDMP_L("mrotstop", &manual_rot_stop_cc),
        D_MKDMP_L("eeflush",  &eeprom_flush_cc),
        D_MKDMP_L("irclr",    &ir_clear_last_cc),
        D_MKDMP_L("iract",    &ir_activity_cc),
        
        D_MKEND()
    };
    
    // 
    Serial.begin(2000000);
    Serial.print("\x1b[2J");  // Clear all
    
    // This cycle counter is used to dump the application's state
    // every N cycles (N * 5 msecs)
    CycleCounter dump_cc(3); // In this case 66 times/sec
    dump_cc.reset();
  
  #endif

    while(true){
        // Read and average Z rotation
        rot = z_update(1023 - analogRead(A1));

        // Rotation state check
        switch(rot_state){
            case ROT_ST_IDLE:
                if(!manual_rot_stop_cc.val()){
                    rot_set_speed(0);
                }
                break;

            case ROT_ST_ROTATING: {
                bool is_over_limit = rot_to_right ? (rot > settings.rot_limit_r) : (rot < settings.rot_limit_l);
                
                if(is_over_limit){
                    rot_set_speed(0);

                    // Flip direction
                    rot_to_right = !rot_to_right;

                    pause_cc.reset(settings.rot_pause_msec / 5);
                    if(pause_cc.val()){
                        // Don't enter this state is the pause is null
                        rot_state = ROT_ST_ROTATION_PAUSED;
                    }
                
                } else {
                    // Reduce range from [0, 65535] to [0, 255]
                    register short rot_speed = settings.rot_speed >> 8;
                    
                    rot_set_speed(rot_to_right ? rot_speed : -rot_speed);
                }
                
                break;
            }

            case ROT_ST_ROTATION_PAUSED:
                // we're paused after all
                break;

            case ROT_ST_SETTING_A_LIMIT:
                if(rot < *curr_limit_ptr - LIMIT_STEP / 2){
                    rot_set_speed(LIMIT_EDIT_SPEED);
                
                } else if(rot > *curr_limit_ptr +  LIMIT_STEP / 2) {
                    rot_set_speed(-LIMIT_EDIT_SPEED);
                    
                } else {
                    rot_set_speed(0);
                }
                break;
        }

        //
        fan_set_speed(settings.fan_speed);

        // IR commands
        if(ir.decode(&irdec)){
            // Get the code
            unsigned short code = irdec.value & 0xffffU;
            bool is_repeated = false;
            
            if((irdec.value >> 16) != 0xe0e0U){
                // Ignore this code
                code = 0;
            
            } else if(code == last_ir_code){
                is_repeated = true;
            
            } else {
                last_ir_code = code;
            }
            
            ir.resume();

            ir_clear_last_cc.reset();

            // This is stopped in the switch's default case when the code is not recognized
            ir_activity_cc.reset();

            // If true at the end of this block starts (or resets) the eeprom write timer
            bool changed = false;

            // If true at the end of the block sets the rot_state
            // to ROT_ST_IDLE or ROT_ST_ROTATING, depending on settings.rot_enabled
            bool set_rot_state = false;

            switch(code){
                case 0x06f9U:  // Up
                case 0x8679U:  // Down
                    modify_clamp(&settings.fan_speed, code == 0x06f9U ? FAN_SPEED_STEP : -FAN_SPEED_STEP);
                    
                    changed = true;
                    break;

                case 0x36c9U:  // A
                    if(is_repeated) break;
                    settings.rot_enable = !settings.rot_enable;
                    
                    changed = true;
                    set_rot_state = true;
                    break;

                case 0x28d7U:  // B
                case 0xa857U:  // C
                    curr_limit_ptr = code == 0x28d7U ? &settings.rot_limit_l : &settings.rot_limit_r;
                    rot_state = ROT_ST_SETTING_A_LIMIT;

                    ir_mode = IR_M_LIMIT;
                    break;

                case 0x6897U:  // D
                    ir_mode = IR_M_PAUSETIME;
                    break;

                case 0x16e9U:  // Enter
                    ir_mode = IR_M_ROOT;

                    // Sets the rotation state to a safe value
                    set_rot_state = true;
                    break;

                case 0xa659U:    // Left
                case 0x46b9U: {  // Right
                    bool is_right = code == 0x46b9U;

                    if(ir_mode == IR_M_ROOT && settings.rot_enable){
                        modify_clamp(&settings.rot_speed, is_right ? ROT_SPEED_STEP : -ROT_SPEED_STEP);

                        changed = true;
                        
                    } else if(ir_mode == IR_M_ROOT){
                        rot_set_speed(is_right ? MANUAL_ROT_SPEED : -MANUAL_ROT_SPEED);
                        
                        manual_rot_stop_cc.reset();
                    
                    } else if(ir_mode == IR_M_LIMIT){
                        modify_clamp(curr_limit_ptr, is_right ? LIMIT_STEP : -LIMIT_STEP);

                        changed = true;
                        
                    } else if(ir_mode == IR_M_PAUSETIME){
                        modify_clamp(&settings.rot_pause_msec, is_right ? PAUSE_STEP : -PAUSE_STEP);

                        changed = true;
                    }

                    break;
                }
                
                default:
                    ir_activity_cc.stop();
                    break;
            }

            if(changed)
                eeprom_flush_cc.reset();

            if(set_rot_state){
                if(settings.rot_enable)
                    rot_state = ROT_ST_ROTATING;

                else
                    rot_state = ROT_ST_IDLE;
            }
        }


        // Restore rotating state on pause end, but only if rotation is enabled
        // and we're still in the paused state, otherwise we could restart the rotation
        // while in limit edit mode
        if(pause_cc.tick()){
            if(settings.rot_enable && rot_state == ROT_ST_ROTATION_PAUSED)
                rot_state = ROT_ST_ROTATING;
        }


        // Rotation is stopped in the next rot_state check if we're still in ROT_ST_IDLE
        // so we don't care about tick()'s return value
        manual_rot_stop_cc.tick();
        
        
        if(eeprom_flush_cc.tick()){
            settings_store(&settings);
        }


        if(ir_clear_last_cc.tick()){
            last_ir_code = 0;
        }


        // Simple pin 13 blink code
        ir_activity_cc.tick();
        if(ir_activity_cc.is_running()){
            if(ir_activity_cc.val() % IR_ACTIVITY_PHASE_LEN == 0){
                PORTB ^= _BV(5);
            }

        } else {
            PORTB &= ~_BV(5);
        }
        
      #if LF_DEBUG
        if(dump_cc.tick()){
            // Reset the timer right away
            dump_cc.reset();
            
            // Dump state to the serial port
            dd_dump(dumpdef, &Serial);
        }
      #endif
        
        delay(CYCLE_DURATION);
    }
}
