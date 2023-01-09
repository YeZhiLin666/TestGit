
#ifndef _IBT_ADC5DAC_
#define _IBT_ADC5DAC_

#include "../../COMMON_OS/Ext_Devices/ADC16bit_AD7656.h"

#define IBT_DAC_24BITS_ZERO_OUTPUT 0x800000
class IBT_ADC5DAC
{

public:
	static void          Adc_Init(void);
	static void          Dac_Init(void);
	// 	static unsigned long ADC_rd(unsigned long CH_Number,unsigned long type);


	// private:
	// 	//static  unsigned short DacComand;	
	// 	//¼¸¸öDAC²Ù×÷
	// 	static  void Dac_out_Cur(char chn,unsigned long value);
	//   
};
#endif

