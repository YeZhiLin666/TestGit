
#ifndef _AUTO_CALI_SAMPLE_H_
#define _AUTO_CALI_SAMPLE_H_

#define AUTO_CALI_ADC_NUM 2    //0 I    1 V

class Auto_Cali_SampleManage
{
    public:

		static void Init(void);
		static int ProcessADC(char adc_device_chn);

// 		static char getCHN(char adc_device_chn);
// 		static unsigned long int  getValue(char adc_device_chn);
// 		static char getType(char adc_device_chn);
    //protected:
    private:

		//只有单通道，2个ADC：I or V
		static unsigned long int m_value;
		static char m_type;

};

#endif
