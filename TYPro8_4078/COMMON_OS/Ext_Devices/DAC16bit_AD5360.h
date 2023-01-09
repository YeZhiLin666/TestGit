#ifndef _ADC5360_H_
#define _ADC5360_H_

#define DAC_REG_AD5360                0xC8
#define DAC_OFFSET_REG_AD5360    0x80
#define DAC_GAIN_REG_AD5360       0x40
#define DAC_SP_FUN_REG_AD5360    0x00    //特殊控制功能寄存器
#define DAC_SP_FUN_POWER_UP_AD5360    0x00


enum
{
    NotAvailable = 0,
    DacAvailable = 1,
    OutputData = 2,
    DACFinish = 3,
};
enum
{
    SSP0IDLE =0,
    CLAMPDACBUSY,
    FRAMBUSY,
};
enum
{
    DacDevice =0,
    ClampDevice =1,
};
//DAC
//使用SPI总线进行驱动
class DAC16bit_AD5360
{
public:
    static       St_Port     Pin_DacCS;
    static       St_Port     Pin_DacRst;
    static       St_Port     Pin_DacLoad;
    static       St_Port     Pin_DacBusy;

    static       SSPI_Para   SPI_Para;
    static unsigned char     SPI_Idx;

    static void Init(unsigned char ConvCodeType);
// 	static unsigned char  Set_DacGain(unsigned char chn,unsigned long value);
    static unsigned char  Dac_out(unsigned char chn,unsigned long value);
// 	static void Dac_Reset(void);
// 	static void SendDacData(unsigned char chn,unsigned long value);
// 	static void FinishSendData();

private:
    static unsigned char CodeType;

};


#endif
