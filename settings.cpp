#include <EEPROM.h>
#include "settings.h"

static unsigned long _settings_crc(settings_t* settings);

void settings_load(settings_t* dst){
    EEPROM.get(SETTINGS_EEPROM_ADDR, *dst);

    // Check validity
    unsigned long saved_crc = dst->crc, actual_crc;
    
    dst->crc = 0L;
    actual_crc = _settings_crc(dst);

    if(actual_crc != saved_crc){
        // Fill with defaults
        dst->rot_enable = false;
        
        dst->rot_limit_l = 400;
        dst->rot_limit_r = 600;

        dst->rot_speed = 32768;
        
        dst->rot_pause_msec = 1000;

        dst->fan_speed = 16384;
    }
}


void settings_store(settings_t* src){
    // Compute CRC32
    src->crc = 0L;
    src->crc = _settings_crc(src);

    EEPROM.put(SETTINGS_EEPROM_ADDR, *src);
}



const static unsigned long crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
};

static unsigned long _settings_crc(settings_t* settings){
    unsigned char* raw_data = (unsigned char*)settings;

    unsigned long crc = ~0L;
    for(unsigned int i = 0;i < sizeof(settings_t);i++){
        crc = crc_table[(crc ^ raw_data[i]) & 0x0f] ^ (crc >> 4);
        crc = crc_table[(crc ^ (raw_data[i] >> 4)) & 0x0f] ^ (crc >> 4);
        crc = ~crc;
    }
    
    return crc;
}
