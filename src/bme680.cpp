/**
 * @file bme680.cpp
 * @author xose.perez@rakwireless.com
 * @brief Vindriktning wisBlock Hack
 * @version 0.1
 * @date 2022-12-03
 * 
 * @copyright Copyright (c) 2022
 */

#include "bme680.h"

Adafruit_BME680 bme;

void bme680_setup() {
  
  Wire.begin();

  if (!bme.begin(0x76)) {
    Serial.println("[BME680] Could not find a valid BME680 sensor, check wiring!");
    return;
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

}

void bme680_read() {
    if (! bme.performReading()) {
        Serial.println("[BME680] Failed to read BME680 :(");
    }
}

float bme680_temperature() {
    return bme.temperature;
}

float bme680_pressure() {
    return bme.pressure / 100.0;
}

float bme680_humidity() {
    return bme.humidity;
}

float bme680_gas_resistance() {
    return bme.gas_resistance / 1000.0;
}
