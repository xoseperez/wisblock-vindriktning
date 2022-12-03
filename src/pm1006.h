/**
 * @file pm1006.h
 * @author xose.perez@rakwireless.com
 * @brief Vindriktning wisBlock Hack
 * @version 0.1
 * @date 2022-12-03
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef _PM1006_H
#define _PM1006_H

void pm1006_setup();
void pm1006_loop();
double pm1006_average();
double pm1006_max();
void pm1006_reset();

#endif // _PM1006_H