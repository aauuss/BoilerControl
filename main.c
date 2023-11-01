#ifndef F_CPU
#define F_CPU 8000000UL
#endif


#include <avr/io.h>
//#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>

#include <avr/interrupt.h>
#include "EEPROM_lib.h"

//#include "I2C_lib.h"
#include "LCD_lib.h"
#include "onewire.h"

#define	DS18B20_PIN	PD6

#define ENC_A PD4
#define ENC_B PD3
#define ENC_SW PD5
#define ZC      PD2
#define TRIAC   PB0


//uint16_t    msec = 0, sec = 0, lastTimeChange = 0;
uint8_t mode = 0x80, rot = 0, buttonPressed = 0, interruptDetected = 0;
//***********mode : 7   -   6   -   5   -   4   -   3   -   2   -   1   -   0*********
//                  draw -  draw    meas-           _________________________
//                  screen  data    ure                    menu mode
//int8_t tmax, tmin, tup, tdwn;
int t[3], dt;

uint8_t GetTmax(){
    return EEPROM_read(65);
}
uint8_t GetTmin(){
    return EEPROM_read(66);
}
uint8_t GetdTup(){
    return EEPROM_read(67);
}
uint8_t GetdTdwn(){
    return EEPROM_read(68);
}
void SetTmax(int8_t change){
    EEPROM_write(65, GetTmax() + change);
}
void SetTmin(int8_t change){
    EEPROM_write(66, GetTmin() + change);
}

ISR(INT1_vect) {
    if (PIND & (1 << ENC_A)) {
        rot = 2;
    } else {
        rot = 1;
    }
    interruptDetected = 1;
}
/*
 * ISR(TIMER1_COMPA_vect){
 *    if (++msec > 999) {
 *        msec = 0;
 *        sec++;
 *    }  
 * }*/

ISR(WDT_OVERFLOW_vect){
    mode |= 0x60;
}  



//===================================НАСТРОЙКИ==================================================
void Setup(void) { 
    GIMSK = (1 << INT1); //вкл. внешнее прерывание INT0
    MCUCR = (1 << ISC11); //прерывание по переднему фронту
    //настройка таймера
    //    TCCR1A = 0x00;
    //    TCCR1B = (1 << WGM12) | (1 << CS10);
    //    TIMSK = (1 << OCIE1A);
    //    OCR1AH = 0x1F;       //считаем до 8000
    //    OCR1AL = 0x40;
    //настроим watchdog как очень медленный таймер
    WDTCSR |= (1 << WDIE) | (1 << WDP2) | (1 << WDP0);    //прерывание каждые 0,5 сек
    //настройка ног
    DDRD &= ~((1 << ENC_A) | (1 << ENC_B) | (1 << ENC_SW)); //ноги от энкодера на вход
    DDRB |= (1 << TRIAC);
    sei();
    
}

//==================================ПРОЦЕДУРЫ==================================================

uint16_t read_temperature(uint8_t pin){ //200b
    uint16_t t;
    
    onewire_reset(pin); // 1-Wire reset
    onewire_write(pin, ONEWIRE_SKIP_ROM); // to all devices on the bus
    onewire_write(pin, 0x44); // send DS18B20 command, "CONVERT T"
    
    onewire_reset(pin); // 1-Wire reset
    onewire_write(pin, ONEWIRE_SKIP_ROM); // to all devices on the bus
    onewire_write(pin, 0xBE); // send DS18B20 command, "READ SCRATCHPAD"
    
    t = onewire_read(pin); // read temperature low byte
    t |= (uint16_t)onewire_read(pin) << 8; // and high byte
    t = ((t >> 4) * 10 + ((t << 12) / 6553 )); // decode temp
    
    return (uint16_t)t;
}

void PrintScreen(){ //220b
    unsigned char _char;
    if (mode & 0x80){
        if (mode & 0x0F){
            for (int i = 0; i < 16; i++){
                _char = EEPROM_read(i+32);
                LCD_Print(&_char, 0, i);
                _char = EEPROM_read(i+48);
                LCD_Print(&_char, 1, i); 
            }
        } else {            
            for (int i = 0; i < 16; i++){
                _char = EEPROM_read(i);
                LCD_Print(&_char, 0, i);
                _char = EEPROM_read(i+16);
                LCD_Print(&_char, 1, i);     
            }
        }
        mode &= 0x7F;
    }else if (mode & 0x40){
        if (mode & 0x0F){
            LCD_PrintInt(GetTmax(), 0, 4);
            LCD_PrintInt(GetTmin(), 0, 13);
            LCD_PrintInt(GetdTup(), 1, 4);
            LCD_PrintInt(GetdTdwn(), 1, 13);
        } else {
            LCD_PrintIntx10(t[0], 0, 3);  
            LCD_PrintIntx10(dt, 1, 3);             
        }
        mode &= 0xBF;
    }
}




void CheckButton(){
    
    if (!(PIND & (1 << ENC_SW))) {
        buttonPressed = 1;
    }        
    
    if ((PIND & (1 << ENC_SW)) && buttonPressed) {
        if (interruptDetected){
            interruptDetected = 0;
            buttonPressed = 0;   
        } else {
            buttonPressed = 0;             
            if((mode & 0x0F) >= 4) { 
                mode = 0x80;
            } else { 
                mode++;
                mode |= 0x80;
            }
        }
    }
    
}

void Control(){
    if (t[0] > GetTmax()) PORTB |= (1 << TRIAC);
    if (t[0] < GetTmin()) PORTB &= ~(1 << TRIAC);
    
    
} 
/*
void PrepareEEPROM(){
        unsigned char chars[64] =  {'t', ' ', '=', ' ', ' ', ' ', ' ', ' ', ' ', 0xDF, 'C', ' ', ' ', ' ', ' ', ' ',  
                                'd', 't', '=', ' ', ' ', ' ', ' ', ' ', ' ', 0xDF,'C', '/', 's', 'e', 'c', ' ',
                                'M', 'a', 'x', ' ', ' ', ' ', 0xDF,' ', 'M', 'i', 'n', ' ', ' ', ' ', ' ', 0xDF,
                                'U', 'p', ' ', ' ', ' ', ' ', 0xDF,' ', 'D', 'w', 'n', ' ', ' ', ' ', ' ', 0xDF
                                };
    for (int i = 0; i < 64; i++){
        EEPROM_write(i, chars[i]);}
 }*/
//===================================ГЛАВНАЯ ПРОГРАММА==========================================
void main(void) {
//        PrepareEEPROM();
    
    Setup();
    LCD_Init();

    while (1) {
        CheckButton();
        switch(mode & 0x0F){
            case 1:
                if (rot == 1) {
                    SetTmax(1);
                    rot = 0;                     
                }
                if (rot == 2){ 
                    SetTmax(-1);
                    rot = 0;                     
                }

                break;
            case 2:
                if (rot == 1){ 
                    SetTmin(1);
                    rot = 0;                     
                }
                if (rot == 2){ 
                    SetTmin(-1);
                    rot = 0;                     
                }

                break;
            case 3:
                
                break;
            case 4:
                
                break;  
                
        }
        
        if (mode & 0x20) {            
            t[2] = t[1];
            t[1] = t[0];
            t[0] = read_temperature(DS18B20_PIN);       
            dt =(t[0] - t[2]);
            mode &= 0xDF;
        }
        
        PrintScreen();
        Control();
    }
}






