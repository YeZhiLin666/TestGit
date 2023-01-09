#ifndef _DAC16bit_AD5754_H_
#define _DAC16bit_AD5754_H_
//#include "../Ext_Devices/Device_MoreCase.h"

#define DAC_REG_AD5754                         0x0	 //All DAC
#define Output_range_select_register_AD5754         0x1<<(19 -16)
#define Power_control_register_AD5754				 0x1<<(20 - 16)
#define Control_register_AD5754           ((0x1<<(19 -16)) + (0x1<<(20 - 16)))

class DAC16bit_AD5754
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

