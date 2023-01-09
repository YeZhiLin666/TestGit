#ifndef _DAC16bit_AD5764_H_
#define _DAC16bit_AD5764_H_
//#include "../Ext_Devices/Device_MoreCase.h"

#define REG_Function_AD5764                         0x0	 //All DAC  ¹¦ÄÜ¼Ä´æÆ÷ 000
//#define Function_register_AD5764         0x1<<(19 -16)
#define DATA_register_AD5764				      0x1<<(20 - 16)  //Êý¾Ý¼Ä´æÆ÷  010
#define REG_Coarse_Gain_AD5764           (0x1<<(19 -16) + 0x1<<(20 - 16)) //´Öµ÷¼Ä´æÆ÷ 011
#define REG_Fine_Gain_AD5764              0x1<<(21 - 16)   //´Öµ÷¼Ä´æÆ÷ 100
#define REG_Offset_Gain_AD5764           (0x1<<(19 -16) + 0x1<<(21 - 16)) //´Öµ÷¼Ä´æÆ÷ 101

#define DAC_A   0x00
#define DAC_B   0x01
#define DAC_C   0x02
#define DAC_D   0x03
#define DAC_ALL 0x04
#define RANGE_10V       0x00
#define RANGE_10_2564V  0x01
#define RANGE_10_5263V  0x02

class DAC16bit_AD5764
{
private:
	static unsigned char CodeType;
public:
	static unsigned char SPI_Idx;
	static St_Port   Pin_DacBin;
	static St_Port   Pin_DacCS;
	static St_Port   Pin_DacLoad;
	static St_Port   Pin_DacClr;

public:
	static void Init(unsigned char ConvCodeType);
	static UChar8 Dac_Out(unsigned char chn,unsigned long int value);
	static void   Dac_SendData(unsigned char chn,unsigned long value);
// 	static void   Dac_SetLoad(void);
};

#endif

