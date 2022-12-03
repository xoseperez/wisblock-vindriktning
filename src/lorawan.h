/**
 * @file lorawan.h
 * @author xose.perez@rakwireless.com
 * @brief Vindriktning wisBlock Hack
 * @version 0.1
 * @date 2022-12-03
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef _LORAWAN_H
#define _LORAWAN_H

#include <LoRaWan-RAK4630.h>

#define LORAWAN_APP_DATA_BUFF_SIZE  51

typedef enum {
    LORAWAN_TYPE_JOINED,
    LORAWAN_TYPE_DOWNLINK
} lorawan_message_type;

bool lorawan_send(unsigned char * data, unsigned char len, unsigned char port = LORAWAN_PORT, lmh_confirm type = LORAWAN_CONFIRM);
void lorawan_join();
bool lorawan_setup(void (*)(lorawan_message_type type, lmh_app_data_t * data));
void lorawan_loop();

#endif // _LORAWAN_H