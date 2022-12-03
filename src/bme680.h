/**
 * @file bme680.h
 * @author xose.perez@rakwireless.com
 * @brief Vindriktning wisBlock Hack
 * @version 0.1
 * @date 2022-12-03
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef _BME680_H
#define _BME680_H

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h> 

void bme680_setup();
void bme680_read();
float bme680_temperature();
float bme680_pressure();
float bme680_humidity();
float bme680_gas_resistance();

#endif // _BME680_H