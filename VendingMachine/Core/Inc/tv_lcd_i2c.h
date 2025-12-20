#ifndef __LCDI2C_H__
#define __LCDI2C_H__

#include "i2c.h"
#include "stdio.h"

void LCD_cursor(char on);
void lcd_gotoxy(unsigned char x, unsigned char y);
void lcd_init(uint8_t addr);
void lcd_clear(void);
void lcd_write_char(int s);
void lcd_write_string(char *s);
void ledON(void);
void ledOFF(void);
#endif
//----------------------------------------------------------------------------end file--------------------------------------------------------------------------//
