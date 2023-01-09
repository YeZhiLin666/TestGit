
#pragma once

#define FRAM_VALID_SIGN          0x12345678
//FM25L256
#define EEPROM_WREN				0X06		//write enable
#define EEPROM_WRDI				0X04		//write disable
#define EEPROM_RDSR				0x05		//read  the register
#define EEPROM_WRSR				0x01		//write the register 
#define EEPROM_READ				0x03		//write  command 
#define EEPROM_WRITE			0x02		//read   command

class FM25L256Func
{
private:
    static const St_Port    Pin_FramCS;
// 	static unsigned char    bIsBusy;
    static SSPI_Para        FRAM_SSPI0;

private:
    static unsigned char EEPROM_EnableWrite(void);
    static unsigned char EEPROM_DisableWrite(void);

public:
    static void          MyInit(void);
// 	static unsigned char checkEEPROMBusy(void);
// 	static unsigned char clear(unsigned char chn);
    static unsigned char writePage(UWord16 wordAddr,unsigned char *array,unsigned char length);
    static unsigned char readPage(UWord16 wordAddr,unsigned char *array,unsigned char length);
};

