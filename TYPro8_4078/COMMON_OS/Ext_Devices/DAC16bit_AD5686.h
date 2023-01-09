#ifndef _DAC16bit_AD5686_H_
#define _DAC16bit_AD5686_H_
//#include "../Ext_Devices/Device_MoreCase.h"

// 24bit[ Command 4bit  Address 4bit  Data 16bit]
// SPI发送MSB高位先行，LSB低位最后。
// Command Bit Definitions
#define Write_input_register_AD5686                 (0x1 << 4)
#define Update_DAC_register_AD5686                  (0x2 << 4)
#define Write_and_Updata_DAC_n_AD5686               (0x3 << 4)
#define Power_up_and_down_AD5686                    (0x4 << 4)
#define Hardware_LDAC_mask_register_AD5686          (0x5 << 4)
#define Software_rest_AD5686                        (0x6 << 4)
#define Internal_Ref_setup_register_AD5686          (0x7 << 4)
#define Enable_DCEN_register_AD5686                 (0x8 << 4)
#define Enable_readback_register_AD5686             (0x9 << 4)




class DAC16bit_AD5686
{
private:
    static unsigned char CodeType;
public:
    static unsigned char SPI_Idx;
    static St_Port   Pin_DacCS;
    static St_Port   Pin_DacLoad;
    static St_Port   Pin_DacRst;

public:
    static void Init(unsigned char ConvCodeType);
    static UChar8 Dac_Out(unsigned char chn,unsigned long int value);
    static void   Dac_SendData(unsigned char chn,unsigned long value);
    static void   Dac_SetLoad(void);
};

#endif

