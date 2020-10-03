#ifndef _EEPROM_H
#define _EEPROM_H

void DisableEEPROM(void);
void EEPROM_read_n(unsigned int EE_address,unsigned char *DataAddress,unsigned char lenth);
void EEPROM_SectorErase(unsigned int EE_address);
void EEPROM_write_n(unsigned int EE_address,unsigned char *DataAddress,unsigned char lenth);
void EEPROM_write_byte(unsigned int EE_address,unsigned char *DataAddress, unsigned char lenth, unsigned char num,unsigned char Data);

#endif