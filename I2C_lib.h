/*
 * Cамая лучшая библиотека для I2C
 */


#include <avr/io.h>

#define SCL		    PB7     // ----> [SCL]	
#define SDA		    PB5     // ----> [SDA]	
                            //       ATTiny2313
#define I2C_PORT    PORTB
#define I2C_DDR     DDRB
#define I2C_PIN     PINB

#define I2CSW_HIGH(PORT) PORTB |= (1 << PORT)
#define I2CSW_LOW(PORT) PORTB &= ~(1 << PORT)


void I2C_Start(void);         
void I2C_Stop(void);          
void I2C_Byte(uint8_t _byte);
void I2C_Write(uint8_t _addr, uint8_t _data);
/*uint8_t I2C_Read(void);
uint8_t I2C_ReadACK(void);
uint8_t I2C_ReadNAK(void);*/
