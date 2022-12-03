/**
 * @file lorawan.cpp
 * @author xose.perez@rakwireless.com
 * @brief Vindriktning wisBlock Hack
 * @version 0.1
 * @date 2022-12-03
 * 
 * @copyright Copyright (c) 2022
 */

#include <Arduino.h>
#include "config.h"
#include "lorawan.h"

void (*_lorawan_message)(lorawan_message_type type, lmh_app_data_t * data);

void lorawan_join() {

	#ifdef LORAWAN_USE_OTAA

		unsigned char deveui[] = LORAWAN_DEVEUI;
		unsigned char appeui[] = LORAWAN_APPEUI;
		unsigned char appkey[] = LORAWAN_APPKEY;

		// Setup the EUIs and Keys
		lmh_setDevEui(deveui);
		lmh_setAppEui(appeui);
		lmh_setAppKey(appkey);
	
	#endif

	#ifdef LORAWAN_USE_ABP

		unsigned long devaddr = LORAWAN_DEVADDR;
		unsigned char appskey[] = LORAWAN_APPSKEY;
		unsigned char nwkskey[] = LORAWAN_NWKSKEY;

		lmh_setNwkSKey(nwkskey);
		lmh_setAppSKey(appskey);
		lmh_setDevAddr(devaddr);

	#endif

	// Start Join procedure
	lmh_join();

}

/**
 * @brief Function for handling LoRaWan received data from Gateway
 * @param[in] data  Pointer to rx data
 */
void lorawan_rx_handler(lmh_app_data_t * data) {

	#ifdef DEBUG
	Serial.printf(
		"[LORA] LoRa Packet received on port %d, size:%d, rssi:%d, snr:%d, data:%s\n",
		data->port, data->buffsize, data->rssi, data->snr, data->buffer
	);
	#endif

	if (_lorawan_message) {
		_lorawan_message(LORAWAN_TYPE_DOWNLINK, data);
	}

}

/**
 * @brief LoRa function for handling HasJoined event.
 */
void lorawan_has_joined_handler(void) {

	#ifdef DEBUG
	Serial.println("[LORA] OTAA Mode, Network Joined!");
	#endif

	lmh_error_status ret = lmh_class_request((DeviceClass_t) LORAWAN_CLASS);
	if (ret == LMH_SUCCESS) {
		//delay(1000);
		if (_lorawan_message) {
			_lorawan_message(LORAWAN_TYPE_JOINED, nullptr);
		}
	}

}

void lorawan_confirm_class_handler(DeviceClass_t Class) {

	Serial.printf("[LORA] Switch to class %c done\n", "ABC"[Class]);

	// Inform the server that switch has occurred ASAP
	lmh_app_data_t m_lora_app_data = { 
		nullptr, 		// buffer
		0, 				// buffer size
		LORAWAN_PORT, 	// port
		0, 				// RSSI
		0				// SNR
	};
	lmh_send(&m_lora_app_data, LORAWAN_CONFIRM);

}

bool lorawan_send(unsigned char * data, unsigned char len, unsigned char port, lmh_confirm type) {

	if (lmh_join_status_get() != LMH_SET) {
		//Not joined, try again later
		return false;
	}

	#ifdef DEBUG
		Serial.print("[LORA] Sending frame: ");
		for (unsigned char i=0; i<len; i++) {
			Serial.printf("%02X", data[i]);
		}
		Serial.println();
	#endif

	// Build message structure
	lmh_app_data_t m_lora_app_data = { 
		data, 			// buffer
		len, 			// buffer size
		LORAWAN_PORT, 	// port
		0, 				// RSSI
		0				// SNR
	};
	lmh_error_status error = lmh_send(&m_lora_app_data, LORAWAN_CONFIRM);
	return (error == LMH_SUCCESS);

}

/**
 * @brief Structure containing LoRaWan callback functions, needed for lmh_init()
 */
static lmh_callback_t lora_callbacks = {
    BoardGetBatteryLevel, BoardGetUniqueId, BoardGetRandomSeed,
	lorawan_rx_handler, lorawan_has_joined_handler, lorawan_confirm_class_handler
};

bool lorawan_setup(void (*lorawan_message)(lorawan_message_type type, lmh_app_data_t * data)) {
    
	// Set callback
	_lorawan_message = lorawan_message;

    // Initialize LoRa chip.
	lora_rak4630_init();

	// Init structure
	lmh_param_t lora_param_init = {
    	LORAWAN_ADR, LORAWAN_DATARATE, LORAWAN_PUBLIC_NETWORK, JOINREQ_NBTRIALS, LORAWAN_TX_POWER, LORAWAN_DUTYCYCLE_OFF
	};

	// Initialize LoRaWan
	#ifdef LORAWAN_USE_OTAA
		unsigned long err_code = lmh_init(&lora_callbacks, lora_param_init, true, CLASS_A, LORAMAC_REGION_EU868);
	#endif
	#ifdef LORAWAN_USE_ABP
		unsigned long err_code = lmh_init(&lora_callbacks, lora_param_init, false, CLASS_A, LORAMAC_REGION_EU868);
	#endif
	if (err_code != 0) {
		Serial.printf("[LORA] LoRa init failed with error: %d\n", err_code);
	} else {
		unsigned char deveui[] = LORAWAN_DEVEUI;
		Serial.printf("[LORA] Device EUI: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\n", 
			deveui[0], deveui[1], deveui[2], deveui[3],
			deveui[4], deveui[5], deveui[6], deveui[7]
		);
	}
	return (err_code == 0);

}

void lorawan_loop() {

	// Handle Radio events
	Radio.IrqProcess();

}
