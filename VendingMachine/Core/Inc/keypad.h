/*
 * keypad.h
 *
 *  Created on: Nov 13, 2025
 *      Author: ADMIN
 */

#ifndef INC_KEYPAD_H_
#define INC_KEYPAD_H_

#include "stm32f1xx_hal.h"
#include "main.h"
void Keypad_LED_Init(void);
char Keypad_Scan(void);

#endif /* INC_KEYPAD_H_ */
