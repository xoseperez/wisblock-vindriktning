/**
 * @file main.ino
 * @author xose.perez@rakwireless.com
 * @brief Vindriktning wisBlock Hack
 * @version 0.1
 * @date 2022-12-03
 * 
 * @copyright Copyright (c) 2022
 */

#include <Arduino.h>
#include "CayenneLPP.h"
#include "config.h"
#include "lorawan.h"
#include "bme680.h"
#include "pm1006.h"

TimerEvent_t appTimer;
CayenneLPP lpp(LORAWAN_APP_DATA_BUFF_SIZE);

void timer_update(void) {
	TimerSetValue(&appTimer, LORAWAN_INTERVAL);
	TimerStart(&appTimer);
}

void tx_handler(void) {
	
	timer_update();

	bme680_read();

	lpp.reset();
	lpp.addTemperature(1, bme680_temperature()); // 4 bytes
	lpp.addRelativeHumidity(2, bme680_humidity()); // 3 byte
	lpp.addBarometricPressure(3, bme680_pressure()); // 4 bytes
	lpp.addAnalogInput(4, bme680_gas_resistance()); // 4 bytes
	lpp.addAnalogInput(5, pm1006_average()); // 4 bytes
	lpp.addAnalogInput(6, pm1006_max()); // 4 bytes
	pm1006_reset();

	lorawan_send(lpp.getBuffer(), lpp.getSize());

}

uint32_t timer_setup(void) {
	TimerInit(&appTimer, tx_handler);
	return 0;
}

void lorawan_message(lorawan_message_type type, lmh_app_data_t * data) {

	if (type == LORAWAN_TYPE_JOINED) {

		// Send one message right away
		// it will schedule next message after LORAWAN_INTERVAL
		tx_handler();

	}

	if (type == LORAWAN_TYPE_DOWNLINK) {
		#ifdef DEBUG
			Serial.printf( "[MAIN] Received:%s\n", data->buffer);
		#endif
	}

}

void setup() {

	// Initialize Serial for debug output
	#ifdef DEBUG
		Serial.begin(115200);
		while ((!Serial) && (millis() < 5000)) delay(10);
		Serial.print("\n[MAIN] WisBlock + IKEA Vindriktning\n\n");
	#endif

	// Disable onboard LED
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);

	// Enable sensors
	pinMode(SENSOR_POWER_GPIO, OUTPUT);
	digitalWrite(SENSOR_POWER_GPIO, HIGH);
	
	// Init modules
	bme680_setup();
	pm1006_setup();
	lorawan_setup(lorawan_message);

	//creat a user timer to send data to server period
	unsigned long err_code = timer_setup();
	if (err_code != 0) {
		Serial.printf("[MAIN] Timer error #%d\n", err_code);
	}

	lorawan_join();

}

void loop() {

	lorawan_loop();
	pm1006_loop();
	delay(1);

}

