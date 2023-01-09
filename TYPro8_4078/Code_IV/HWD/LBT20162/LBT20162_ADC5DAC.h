
#ifndef _LBT20162_ADC5DAC_
#define _LBT20162_ADC5DAC_

#include "../../COMMON_OS/Ext_Devices/ADC16bit_AD7656.h"

#define LBT20162_DAC_24BITS_ZERO_OUTPUT 0x800000
class LBT20162_ADC5DAC
{

public:
    static void          Adc_Init(void);
    static void          Dac_Init(void);

    //����DAC����
    static  void Dac_out_Cur(char chn,unsigned long value);
    static  void Dac_out_Vlt(char chn,unsigned long value);
    static  void Dac_out_Clamp_Vlt(char chn,unsigned long value);
private:
// 	 unsigned short DacComand;

};
#endif

