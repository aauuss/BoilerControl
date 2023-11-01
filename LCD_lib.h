/*
 * Cамая лучшая библиотека для 1602 c i2c
 */


#include <avr/io.h>
#include "I2C_lib.h"

#define EN  0x04  // Enable bit
#define RW  0x02  // Read/Write bit
#define RS  0x01  // Register select bit
#define LED 0x08  // Led light

#define LCD_ADDRESS_ROW1    0x80
#define LCD_ADDRESS_ROW2    0xC0
#define LCD_4BIT_2LINE_5x8  0x28
#define LCD_OFF_CURSOR_OFF  0x08
#define LCD_ON_CURSOR_OFF   0x0C
#define LCD_ON_CURSOR_BLINK 0x0D
#define LCD_MOVE_CURSOR_RIGHT_ONE_CHAR 0x14
#define LCD_CLEAR           0x01
#define LCD_ENTRY_MODE      0x06

#define LCD_ADDR 0x7E           //0x3f << 1

void LCD_WriteCmd(uint8_t _command);
void LCD_WriteData(uint8_t _data);
void LCD_WriteCmd4Bit(uint8_t _command);
void LCD_WriteData4Bit(uint8_t _data);
void LCD_SetPos(uint8_t _str, uint8_t _pos);
void LCD_Init();
void LCD_Print(unsigned char *_data, uint8_t _str, uint8_t _pos);
void LCD_PrintInt(int _data, uint8_t _str, uint8_t _pos);
void LCD_PrintIntx10(int _data, uint8_t _str, uint8_t _pos);

//char * itoa_div10(int _in, char * _buffer);
