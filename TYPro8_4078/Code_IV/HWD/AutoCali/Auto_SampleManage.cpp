
#include "../../COMMON/includes.h"


unsigned long int Auto_Cali_SampleManage::m_value;
char Auto_Cali_SampleManage::m_type;

void Auto_Cali_SampleManage::Init(void)
{
    m_value = 0;
	m_type = 0;
}

//使用AD7190
//同时CONV
//分别读取
int Auto_Cali_SampleManage::ProcessADC(char adc_device_chn)
{
	//only 1 adc_device

    m_value = Adc_AD7190::AD7190_Read(0,0);
	return 1;
}

char Auto_Cali_SampleManage::getCHN(char adc_device_chn)
{
    return 0;    //只认为自动校准只有单通道0
}

unsigned long int  Auto_Cali_SampleManage::getValue(char adc_device_chn)
{
	return m_value;
}

char Auto_Cali_SampleManage::getType(char adc_device_chn)
{
  return 0;
}
