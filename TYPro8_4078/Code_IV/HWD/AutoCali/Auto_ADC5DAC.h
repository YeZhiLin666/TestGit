
#ifndef _AUTO_CALI_ADDA_H_
#define _AUTO_CALI_ADDA_H_

class AutoADC5DAC
{
public:
	static void Adc_Init(unsigned char Device_Number);
	static void Dac_Init(unsigned char Device_Number);

	static void Dac_out(unsigned long value);
};

#endif
