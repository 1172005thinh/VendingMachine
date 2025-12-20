/*
 * sensor.h
 * Module cảm biến siêu âm HC-SR04
 */

#ifndef INC_SENSOR_H_
#define INC_SENSOR_H_

#include "main.h"

#define TRIG_PIN  GPIO_PIN_9
#define TRIG_PORT GPIOA

extern uint16_t Distance;

extern uint32_t IC_Val1;
extern uint32_t IC_Val2;
extern uint32_t Difference;
extern uint8_t Is_First_Captured;

void Sensor_Init(void);
void delay_us(uint16_t time);
void Sensor_Trigger(void);

#endif /* INC_SENSOR_H_ */
