#include "tv_lcd_i2c.h"
#include <string.h>

uint8_t _backlightval;
uint8_t LCDI2C_ADDR;
void PCD8574_write(unsigned char byte)
{
  I2C_Start(); // bat dau ghi
  I2C_Write(LCDI2C_ADDR<<1);
   I2C_CheckAck();
  I2C_Write(byte);     
   I2C_CheckAck();
  I2C_STOP();    // ket thuc qua trinh truyen du lieu
}
//------------------------------Giao tiep LCD 1602-------------------------//
unsigned char data_MASK=0xFF; //byte mat na
#define LCD_RS  (1<<0)
#define LCD_RW  (1<<1)
#define LCD_EN  (1<<2)
#define LCD_LED (1<<3)
void ledOFF(void)
{
   data_MASK&=~LCD_LED;
   PCD8574_write(data_MASK);
}
void ledON(void)
{
   data_MASK|=LCD_LED;
   PCD8574_write(data_MASK);
}
void LCD_Enable(void)
{
    data_MASK |= LCD_EN;     //SET Enable
    for(int i=0;i<50;i++)__NOP();
    PCD8574_write(data_MASK);
    data_MASK &= ~LCD_EN;   //RESET Enable
    for(int i=0;i<100;i++)__NOP();
    PCD8574_write(data_MASK);
}
void LCD_Send4Bit(unsigned char Data) //chi su dung 4 bit thap cua Data
{
    data_MASK &= 0x0F; //clear 4 bit cao
    data_MASK |=  (Data & 0x01)<<4;        // lay ra bit 0 dua vao vi tri bit 4
    data_MASK |=  (Data & 0x02)<<4;        // lay ra bit 1 dua vao vi tri bit 5
    data_MASK |=  (Data & 0x04)<<4;        // lay ra bit 2 dua vao vi tri bit 6
    data_MASK |=  (Data & 0x08)<<4;        // lay ra bit 3 dua vao vi tri bit 7
}
void LCD_Send1Byte(unsigned char byte)
{
   LCD_Send4Bit(byte >>4);/* Gui 4 bit cao */
   LCD_Enable();
   LCD_Send4Bit(byte); /* Gui 4 bit thap*/
   LCD_Enable();
}
// Ham di chuyen con tro: row=0-1; col=0-15 (2 hang + 16 cot)
void lcd_gotoxy(unsigned char x, unsigned char y)
{
   int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  LCD_Send1Byte(0x80 | (x + row_offsets[y]));
}
void LCD_cursor(char on)
{
   if(on)LCD_Send1Byte(0x0F);
   else LCD_Send1Byte(0x0C);
}
// Ham hien thi ra man hinh chuoi ki tu
void lcd_write_string(char *s)
{
   while(*s)
   {
      data_MASK |= LCD_RS;  //dua chan RS len vcc
    PCD8574_write(data_MASK);
      LCD_Send1Byte(*s);
      data_MASK &= ~LCD_RS;  //dua chan RS xuong mass
     PCD8574_write(data_MASK);
      s++;
   }
}
void lcd_write_char(int s)
{
      data_MASK |= LCD_RS;  //dua chan RS len vcc
     PCD8574_write(data_MASK);
      LCD_Send1Byte(s);
    data_MASK &= ~LCD_RS;  //dua chan RS xuong mass
    PCD8574_write(data_MASK);
}
// Ham xoa man hinh
void lcd_clear(void)
{
   lcd_gotoxy(0,0);
   lcd_write_string("                    ");
   lcd_gotoxy(0,1);
   lcd_write_string("                    ");
   lcd_gotoxy(0,2);
   lcd_write_string("                    ");
   lcd_gotoxy(0,3);
   lcd_write_string("                    ");
   lcd_gotoxy(0,0);
//LCD_Send1Byte(0x01);
//LCD_chonvitri(0,0);
}
// Ham khoi tao LCD
void lcd_init(uint8_t addr)
{
   LCDI2C_ADDR = addr;
   
   //data_MASK &= ~LCD_LED;
   data_MASK &= ~LCD_RW;  //dua chan RW xuong mass
   data_MASK &= ~LCD_RS;  //dua chan RS xuong mass
//PCD8574_write(data_MASK);
   LCD_Send4Bit(0x03);
   LCD_Enable();
   HAL_Delay(5);
   LCD_Enable();
   HAL_Delay(5);
   LCD_Enable();
   LCD_Send4Bit(0x02);   // dua con tro ve dau man hinh
   LCD_Enable();

   LCD_Send1Byte(0x28);
   LCD_Send1Byte(0x0C); // Bat hien thi, bat con tro    0x0C neu muon tat con tro
   LCD_Send1Byte(0x06);
   lcd_gotoxy(0,0);
  //LCD_xoamanhinh();
}

void lcd_center_text(int row, char *str) {
    int len = strlen(str);
    int padding = 0;
    if (len < 16) {
        padding = (16 - len) / 2;
    }
    lcd_gotoxy(padding, row);
    lcd_write_string(str);
}
