#ifndef LEGOFAN_SETTINGS_H
#define LEGOFAN_SETTINGS_H 1

typedef struct {
    bool           rot_enable;
    unsigned short rot_limit_l, rot_limit_r;
    unsigned short rot_speed;
    unsigned short rot_pause_msec;
    unsigned short fan_speed;

    unsigned long  crc;
} settings_t;

// Settings eeprom address
#define SETTINGS_EEPROM_ADDR 0x00

// Loads settings from EEPROM, or defaults if the CRC check fails
void settings_load(settings_t* dst);

// Store settings to EEPROM with the correct CRC
void settings_store(settings_t* src);

#endif
