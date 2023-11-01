#include "I2C_lib.h"

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <stdlib.h>
#include <avr/io.h>
#include <stdio.h>

#include <util/delay.h>

void I2C_Start(void) {        //  старт
	I2C_DDR |= (1 << SDA);	        //  SCL----\__...
	I2C_DDR |= (1 << SCL);	        //        
	I2C_PORT |= (1 << SCL);	    //  SDA--\__...
	I2C_PORT |= (1 << SDA);	    // 
	I2C_PORT &= ~ (1 << SDA);	// 
    _delay_us(1);
	I2C_PORT &= ~ (1 << SCL);	// 
}

void I2C_Stop(void) {
    I2C_DDR  |= (1 << SDA);         //  стоп
    I2C_DDR  |= (1 << SCL);         //
	I2C_PORT &= ~ (1 << SCL);	//  SCL..._/---
	I2C_PORT &= ~ (1 << SDA);	// 
	I2C_PORT |= (1 << SCL);	    //  SDA...___/---
    _delay_us(1);
	I2C_PORT |= (1 << SDA);	    // 
	I2C_DDR  &= ~(1 << SDA);        //  SDA на прием
}

void I2C_Byte(uint8_t _byte) {     //передача байта
	uint8_t i;
	for (i = 0; i < 8; i++) {
		if ((_byte << i) & 0x80)
			I2C_PORT |= (1 << SDA);
		else
			I2C_PORT &= ~ (1 << SDA);
		I2C_PORT |= (1 << SCL);
        _delay_us(1);
		I2C_PORT &= ~ (1 << SCL);
        _delay_us(1);
	}
	I2C_PORT |= (1 << SDA);
    I2C_DDR  &= ~(1 << SDA);        //здесь должене прийти бит ACK
	I2C_PORT |= (1 << SCL);
    _delay_us(1);
	I2C_PORT &= ~ (1 << SCL);
    while (! I2C_PIN & (1 << SDA));
    _delay_us(1);                     //по хорошему тут надо анализировать, не прижал ли slave ногу SCL к земле
    I2C_DDR  |= (1 << SDA);

}

void I2C_Write(uint8_t _addr, uint8_t _data){     //запись с последующей записью
    I2C_Start();
    I2C_Byte(_addr);
    I2C_Byte(_data);
    I2C_Stop();
}

/*
uint8_t I2C_Read(void) {      //чтение байта
    uint8_t _data = 0x00;
    I2C_PORT |= (1 << SDA);    //???
    I2C_DDR  &= ~(1 << SDA);        //ставим ногу SDA на прием
    I2C_PORT |= (1 << SDA);
    I2C_DDR  |= (1 << SCL);
	for (uint8_t i = 0; i < 8; i++) {
        I2C_PORT |= (1 << SCL);
        _delay_us(1);
		_data |= (((I2C_PIN >> SDA) & 0x01) << (7 - i) );
		I2C_PORT &= ~ (1 << SCL);
        _delay_us(1);                   //TODO: УБРАТЬ НАХУЙ!!!
	}
    return _data;
}

uint8_t I2C_ReadACK(void) {
    uint8_t _data = I2C_Read();
    I2C_DDR  |= (1 << SDA);             //SDA снова на передачу
	I2C_PORT &= ~ (1 << SDA);          //тут вроде как надо передавать ACK
    I2C_PORT |= (1 << SCL);
    _delay_us(1);
    I2C_PORT &= ~ (1 << SCL);
	I2C_PORT |= (1 << SDA);
    I2C_DDR  &= ~(1 << SDA); 
    I2C_DDR  &= ~(1 << SCL);
    _delay_us(5);                       //без этого не работает
    I2C_DDR  |= (1 << SCL);
    return _data;
}

uint8_t I2C_ReadNAK(void) {
    uint8_t _data = I2C_Read();
    //I2C_DDR  |= (1 << SDA);           //SDA снова на передачу
	//I2C_PORT |= (1 << SDA);             //тут передавать ACK не надо т.к. это последний байт
    I2C_PORT |= (1 << SCL);
    _delay_us(1);
    I2C_PORT &= ~ (1 << SCL);
	//I2C_PORT &= ~ (1 << SDA);
    I2C_DDR  &= ~(1 << SDA); 
    I2C_DDR  &= ~(1 << SCL);
    _delay_us(5);                       //без этого не работает
    I2C_DDR  |= (1 << SCL);
    return _data;
}
*/
