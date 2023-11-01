
#include "LCD_lib.h"

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <stdlib.h>
#include <avr/io.h>
//#include <stdio.h>

#include <util/delay.h>

void LCD_WriteCmd(uint8_t _command){
    _command &= (0xF0);
    _command |= LED;
    I2C_Write(LCD_ADDR, _command | EN);
    _delay_us(5);
    I2C_Write(LCD_ADDR, _command & ~EN);
    _delay_us(50);
    I2C_Write(LCD_ADDR, _command | EN);

}

void LCD_WriteData(uint8_t _data){
    _data &= 0xF0;
    _data |= RS | LED;
    I2C_Write(LCD_ADDR, _data | EN);
    _delay_us(5);
    I2C_Write(LCD_ADDR, _data & ~EN);
    _delay_us(50);
    I2C_Write(LCD_ADDR, _data | EN);

}

void LCD_WriteCmd4Bit(uint8_t _command){
    LCD_WriteCmd(_command );
    LCD_WriteCmd(_command << 4);
}

void LCD_WriteData4Bit(uint8_t _data){
    LCD_WriteData(_data);
    LCD_WriteData(_data << 4);
}

void LCD_SetPos(uint8_t _str, uint8_t _pos){
    switch (_str) {
        case 0 :
            LCD_WriteCmd4Bit(LCD_ADDRESS_ROW1 + _pos); break;
        case 1 :
            LCD_WriteCmd4Bit(LCD_ADDRESS_ROW2 + _pos); break;
    }
}
    
void LCD_Init(){
    _delay_ms(100);
    LCD_WriteCmd(0x30);
    _delay_ms(10);
    LCD_WriteCmd(0x30);
    _delay_us(4500);
    LCD_WriteCmd(0x30);
    _delay_us(200);
    LCD_WriteCmd(0x20);
    _delay_ms(10);
    
    LCD_WriteCmd4Bit(0x28); 
    _delay_ms(50);
    LCD_WriteCmd4Bit(0x0D);  
    _delay_ms(50);
    LCD_WriteCmd4Bit(0x01); 
    _delay_ms(50);
    
    LCD_WriteCmd4Bit(0x06);
}
    
    
void LCD_Print(unsigned char *_data, uint8_t _str, uint8_t _pos){
    unsigned char *startPointer = _data;
    LCD_SetPos(_str, _pos);
    
    for(;*_data != '\0'; _data++){
        LCD_WriteData4Bit(*_data);
        if(_data - startPointer > 15) return;
    }
}

void LCD_PrintInt(int _data, uint8_t _str, uint8_t _pos){
    char _string[5];
    itoa(_data, _string, 10);
    LCD_Print(_string, _str, _pos);
}

void LCD_PrintIntx10(int _data, uint8_t _str, uint8_t _pos){
    char _string[6];
    _string[5] = '\0';
    if ((_data > -999) && (_data <9999)){  
        itoa(_data, _string, 10);
        if (_data < -99){
            _string[4] = _string[3];
            _string[3] = '.';
        } else if (_data < -9){
             _string[4] = _string[2];
             _string[3] = '.';
             _string[2] = _string[1];
             _string[1] = _string[0];
             _string[0] = ' ';
        } else if (_data < 0) {
            _string[4] = _string[1];
            _string[3] = '.';
            _string[2] = '0';
            _string[1] = _string[0];  
            _string[0] = ' ';
        } else if (_data < 9) {
            _string[4] = _string[0];
            _string[3] = '.';
            _string[2] = '0';
            _string[1] = ' ';  
            _string[0] = ' ';
        } else if (_data < 99) {
             _string[4] = _string[1];
             _string[3] = '.';
             _string[2] = _string[0];
             _string[1] = ' ';
             _string[0] = ' ';
        } else if (_data < 999) {
             _string[4] = _string[2];
             _string[3] = '.';
             _string[2] = _string[1];
             _string[1] = _string[0];
             _string[0] = ' ';
        } else {
            _string[4] = _string[3];
            _string[3] = '.';
        }
        
    }else { 
        _string[2] = 'N';
        _string[1] = 'a';
        _string[0] = 'N';
    }
    LCD_Print(_string, _str, _pos);
}

