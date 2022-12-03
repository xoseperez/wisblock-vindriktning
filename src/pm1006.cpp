/**
 * @file pm1006.cpp
 * @author xose.perez@rakwireless.com
 * @brief Vindriktning wisBlock Hack
 * @version 0.1
 * @date 2022-12-03
 * 
 * @copyright Copyright (c) 2022
 */

#include <Arduino.h>
#include "pm1006.h"

unsigned char _pm1006_buffer[20] = {0};
unsigned char _pm1006_position = 0;

double _pm1006_sum = 0;
double _pm1006_max = 0;
unsigned char _pm1006_cnt = 0;

void pm1006_reset() {
    _pm1006_sum = 0;
    _pm1006_max = 0;
    _pm1006_cnt = 0;
}

double pm1006_average() {
    if (_pm1006_cnt > 0) {
        return _pm1006_sum / _pm1006_cnt;
    }
    return 0;
}

double pm1006_max() {
    return _pm1006_max;
}

void pm1006_parse() {
    
    // Show raw strea
    Serial.print("[PM1006] Raw reading: ");
    for (unsigned char i=0; i<20; i++) {
        Serial.printf("%02X", _pm1006_buffer[i]);
    }
    Serial.println();

    // check second header byte
    if ((_pm1006_buffer[1] != 0x11) || (_pm1006_buffer[2] != 0x0B)) {
        Serial.println("[PM1006] Wrong header");
        return;
    }

    // check crc
    uint8_t crc = 0;
    for (unsigned char i=0; i<20; i++) crc += _pm1006_buffer[i];
    if (crc != 0) {
        Serial.println("[PM1006] Wrong CRC");
        return;
    }

    // calculate PM
    double pm25 = 256 * _pm1006_buffer[5] + _pm1006_buffer[6];    
    Serial.printf("[PM1006] Reading: %5.2f\n", pm25);

    // save
    _pm1006_sum += pm25;
    if (pm25 > _pm1006_max) _pm1006_max = pm25;
    _pm1006_cnt++;

}

void pm1006_setup() {
    Serial1.begin(9600);
}

void pm1006_loop() {
    while (Serial1.available()) {
        unsigned char ch = Serial1.read();
        if ((_pm1006_position > 0) || (ch == 0x16)) {
            _pm1006_buffer[_pm1006_position] = ch;
            _pm1006_position++;
            if (_pm1006_position == 20) {
                _pm1006_position = 0;
                pm1006_parse();
                memset(_pm1006_buffer, 0, sizeof(_pm1006_buffer));
            }
        }  
    }
}
