
#include "includes.h"




//for 接口函数
char LBT22043_SampleManage::m_chn;
unsigned long int LBT22043_SampleManage::m_value;
char LBT22043_SampleManage::m_type;


char LBT22043_SampleManage::m_ADC_chn;
char LBT22043_SampleManage::chn_DoIR;
char LBT22043_SampleManage::chn_history;
char LBT22043_SampleManage::ADC_Status;
unsigned char LBT22043_SampleManage::IR_CTRL_FLAG;

unsigned short LBT22043_SampleManage::TimeTicks;

void LBT22043_SampleManage::Init(void)
{
	m_chn = 0;
	m_value = 0;
	m_type = 0;

	m_ADC_chn = 0;

	chn_history = 0;
	//状态机
	ADC_Status = LBT22043_ADC_IDLE;

	IR_CTRL_FLAG = 0;
}

char LBT22043_SampleManage::SampleISR(unsigned long CH_Number,unsigned long type)
{
	//if((OS_ServeProcess::OS_Time.Us100 & 0x0F) == 0)     //每隔1.6毫秒重新计算一次时间？ 这个时间间隔是否合理呢？
		ChannelInfo_Manager::Set_TimeRecalculateSign();
	TimeTicks++;

	switch(ADC_Status)
	{
	case LBT22043_ADC_IDLE:
		ADC_Status = LBT22043_ADC_WAIT;
		TimeTicks = 0;
		m_ADC_chn = 0;
		break;
	case LBT22043_ADC_WAIT:
		break;
	case LBT22043_ADC_ReadIV:
		Adc16bit_AD7606::Read(&ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc,
			&ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc);
		Sample_Pro::NewCodeSampled[m_ADC_chn] = 1;
				        
        if(BurstModeSampling::uc_A_BurstMode[m_ADC_chn])
            BurstModeSampling::BurstSavePoint(m_ADC_chn);//  2017.6.8  yy

		ADC_Status = LBT22043_ADC_WAIT;
		break;
	default:
		Adc16bit_AD7606::Reset();
		ADC_Status = LBT22043_ADC_IDLE;
		break;
	}
	return 0;    //error	
}

/*int LBT22043_SampleManage::ProcessADC(char adc_device_chn)
{
	return -1;    //error
}




char LBT22043_SampleManage::getCHN(char adc_device_chn)
{
	return m_chn;
}

//返回的是码字
unsigned long int LBT22043_SampleManage::getValue(char adc_device_chn)
{
	return m_value;
}

char LBT22043_SampleManage::getType(char adc_device_chn)
{
	return m_type;
}*/



char LBT22043_SampleManage::IR_ADC(char chn)
{
	chn_history = m_ADC_chn;    //记录历史
	chn_DoIR = chn;
	IR_CTRL_FLAG = 1;

	return 1;
}

//恢复
char LBT22043_SampleManage::renew_IR_ADC(void) 
{
	m_ADC_chn = chn_history;    //还原
	IR_CTRL_FLAG = 0;    //clear
	ADC_Status = LBT22043_ADC_IDLE;

	return 1;
}


///////////////////

void  LBT22043_SampleManage::IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI)
{
	*SampleI = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc;
	*SampleV = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc;  //电压
}



