#pragma once

#define ADC101C027_ADDR_CONV				(0x00)
#define	ADC101C027_ADDR_ALERT				(0x01)
#define	ADC101C027_ADDR_CONFG				(0x02)
#define	ADC101C027_ADDR_LOWL				(0x03)
#define	ADC101C027_ADDR_HIGHL				(0x04)
#define	ADC101C027_ADDR_HYST				(0x05)
#define	ADC101C027_ADDR_LOWCONV			(0x06)
#define	ADC101C027_ADDR_HIGHCONV		(0x07)

typedef struct
{
	unsigned short Result;
	unsigned char AlertFlag;
	unsigned char AlertStatus;
}ADC101C027_TypeDef;

class  ADC101C027
{
public:
   static void ADC101C027_Init(unsigned char I2Cx); 
   static ADC101C027_TypeDef ADC101C027_Read(unsigned char I2Cx,unsigned char chan);
   static unsigned char ADC101C027_WriteInterReg1(unsigned char I2Cx, unsigned char chan, unsigned char Value, unsigned char Addr);
   static unsigned char ADC101C027_WriteInterReg2(unsigned char I2Cx, unsigned char chan, unsigned short Value, unsigned char Addr);
   static unsigned short ADC101C027_ReadInterReg(unsigned char I2Cx, unsigned char chan, unsigned char Addr, unsigned char Len);

private:
};

