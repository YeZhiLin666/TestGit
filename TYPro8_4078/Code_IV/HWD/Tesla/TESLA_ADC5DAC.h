
#ifndef _TESLA_ADC5DAC_
#define _TESLA_ADC5DAC_

// #include "../../COMMON_OS/Ext_Devices/ADC16bit_AD7656.h"

class TESLA_ADC5DAC
{

public:
	static void Adc_Init(void);
	static void Dac_Init(void);
	//static unsigned long ADC_rd(unsigned long CH_Number,unsigned long type);

	//¼¸¸öDAC²Ù×÷
	//static  void Dac_out_Cur(char chn,unsigned long value);
	static  void Dac_out_Vlt(char chn,unsigned long value);
	static  void Dac_out_Clamp_Vlt(char chn,unsigned long value);

};
#endif

