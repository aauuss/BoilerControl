#include "EEPROM_lib.h"

#include <avr/io.h>


void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
while(EECR & (1<<EEPE));
    EECR = (0 << EEPM1) | (0 << EEPM0);
    EEAR = uiAddress; 
    EEDR = ucData; 
    EECR |= (1<<EEMPE); 
    EECR |= (1<<EEPE);
}

unsigned char EEPROM_read(unsigned int uiAddress)
{
    while(EECR & (1<<EEPE));
    EEAR = uiAddress;
    EECR |= (1<<EERE);
    return EEDR;
}
