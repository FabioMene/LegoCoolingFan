#include "config.h"
#include "utils.h"

// z_*
static unsigned short z_readings[NUM_Z_ROTATION_READINGS];
static   signed char  z_idx = -1;

unsigned short z_update(unsigned short new_value){
    register unsigned char i;
    
    if(z_idx == -1){
        for(i = 0;i < NUM_Z_ROTATION_READINGS;i++){
            z_readings[i] = new_value;
            z_idx = 0;
            // We can place a return here, but this block
            // is executed only once
        }    
    } else {
        z_readings[z_idx++] = new_value;
        if(z_idx == NUM_Z_ROTATION_READINGS)
            z_idx = 0;
    }

    // yeah 16 bit reg vars
    register unsigned short rot = 0;
    for(i = 0;i < NUM_Z_ROTATION_READINGS;i++){
        rot += z_readings[i] / NUM_Z_ROTATION_READINGS;
    }

    return rot;
}


// CycleCounter
CycleCounter::CycleCounter() : top_value(0), curr_value(0) {}

CycleCounter::CycleCounter(unsigned short top) : top_value(top), curr_value(0) {}


void CycleCounter::reset(){
    curr_value = top_value;
}


void CycleCounter::reset(unsigned short new_top){
    top_value = curr_value = new_top;
}


void CycleCounter::stop(){
    curr_value = 0;
}


bool CycleCounter::tick(){
    if(curr_value)
        if(--curr_value == 0)
            return true;

    return false;
}


bool CycleCounter::is_running(){
    return curr_value > 0;
}


unsigned short CycleCounter::val(){
    return curr_value;
}


void CycleCounter::dd_do_dump(Print* out){
    out->print(curr_value);
    out->print('/');
    out->print(top_value);
}


// Basically *value += delta but with bonduary check
void modify_clamp(unsigned short* value, short delta){
    bool is_add = delta > 0;

    if(is_add){
        if(*value > 65535U - delta){
            *value = 65535U;
            return;
        }
        
    } else if(*value < (unsigned short)(-delta)) {
        *value = 0;
        return;
    }

    *value += delta;
}
