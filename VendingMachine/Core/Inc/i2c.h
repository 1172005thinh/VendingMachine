#ifndef __SWI2C_H__
#define __SWI2C_H__

#include "main.h"

#define __I2C_DELAY 100
 
 void I2C_init(GPIO_TypeDef *SDA_Port,uint16_t SDA_pin,GPIO_TypeDef *SCL_Port,uint16_t SCL_pin);
 void SDA_in(void);
 void I2C_Delay(unsigned int time);
 void SDA_out(void);
 void I2C_Start(void);
 void I2C_STOP(void);
 unsigned char I2C_CheckAck(void);
 void I2C_Write(unsigned char Data);
 unsigned char I2C_Read(void);
#endif

