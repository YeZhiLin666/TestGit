
#include "../../Entry/includes.h"

char IBT_SampleManage::m_chn;
unsigned long int IBT_SampleManage::m_value;
char IBT_SampleManage::m_type;
char IBT_SampleManage::m_ADC_chn;

char IBT_SampleManage::chn_history;
char IBT_SampleManage::chn_DoIR;
char IBT_SampleManage::ADC_Status;
unsigned char IBT_SampleManage::IR_CTRL_FLAG;

unsigned short IBT_SampleManage::TimeTicks;
const unsigned short IBT_SampleManage::DelayTickCount_ADG = 4; //12;  //600us Open ADG--->200us Open ADG seens more good
const unsigned short IBT_SampleManage::DelayTickCount_AdcConv = 1;  
const unsigned short IBT_SampleManage::DelayTickCount_AdcReadData = 1; 

void IBT_SampleManage::Init(void)
{
	m_chn = 0;
	m_value = 0;
	m_type = 0;

	//内部运转
	m_ADC_chn = 0;

	chn_history = 0;
	//状态机
	ADC_Status = IBT_ADC_IDLE;

	IR_CTRL_FLAG = 0;
}

char IBT_SampleManage::SampleISR(unsigned long CH_Number,unsigned long type)
{
	//if((OS_ServeProcess::OS_Time.Us100 & 0x0F) == 0)
		ChannelInfo_Manager::Set_TimeRecalculateSign();

	switch(ADC_Status)
	{
	case IBT_ADC_IDLE:
		ADC_Status = IBT_ADC_OpenADG;
		TimeTicks = 0;
		m_ADC_chn = 0;
		break;
	case IBT_ADC_OpenADG:
		if(IR_CTRL_FLAG)   
		{
			m_ADC_chn = chn_DoIR;
			ADC_Status = IBT_ADC_NoWork;   
		}
		else
		{
			TimeTicks = 0;
			ADC_Status = IBT_ADC_OPENING;
		}
		IBT_Base::ADC_ADG_open(m_ADC_chn);
		break;
	case IBT_ADC_OPENING:
		TimeTicks++;
		if(TimeTicks >= DelayTickCount_ADG)
		{
			SSPIFunc::ClearFIFO(1);
			ADC_Status = IBT_ADC_TriggerI;
			Adc24bit_CS5560::Convert(); 
			TimeTicks = 0;
		}
		break;
	case IBT_ADC_TriggerI:
		IBT_Base::ADC_ADG_open((m_ADC_chn+1)%IBT_ADC_I_CHN_MAX);
		Adc24bit_CS5560::Pin_CS = IBT_Base::PinGroup[IBT_Pin_AdcCS1];   
		ControlPin::SetMeLow(Adc24bit_CS5560::Pin_CS); 
		SSPIFunc::ConfirmDevice(1, &IBT_Base::IBT_SSP1ADC);
		Adc24bit_CS5560::Trigger(); 
		ADC_Status = IBT_ADC_ReadI;
		break;
	case IBT_ADC_ReadI:
		{
			SSPIFunc::ConfirmDevice(1, &IBT_Base::IBT_SSP1ADC);
			unsigned long adc_code_temp = Adc24bit_CS5560::ReadData();
			ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc  = adc_code_temp; 
			Adc24bit_CS5560::Pin_CS = IBT_Base::PinGroup[IBT_Pin_AdcCS2];
			ControlPin::SetMeLow(Adc24bit_CS5560::Pin_CS); 
			SSPIFunc::ConfirmDevice(1, &IBT_Base::IBT_SSP1ADC);
			Adc24bit_CS5560::Trigger();    
			ADC_Status = IBT_ADC_ReadV;
			TimeTicks++;
			break;
		}
	case IBT_ADC_ReadV:
#if DEBUG_TASK_STATISTIC == 1
		if(m_ADC_chn == 0)
			OS_ServeProcess::Task_Count[COUNT_SAMPLE_ADC]++;
#endif
		SSPIFunc::ConfirmDevice(1, &IBT_Base::IBT_SSP1ADC);
		ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc = Adc24bit_CS5560::ReadData();
		Sample_Pro::NewCodeSampled[m_ADC_chn] = 1;
		if(IR_CTRL_FLAG == 0)   
		{
			m_ADC_chn++;
			if(m_ADC_chn >= IBT_ADC_I_CHN_MAX)    //
				m_ADC_chn = 0;
			ADC_Status = IBT_ADC_OPENING;
		}
		else
			ADC_Status = IBT_ADC_OpenADG;
		TimeTicks++;
		break;
	case IBT_ADC_NoWork:
		break;
	default:
		ADC_Status = IBT_ADC_IDLE;
		break;
	}

	return 0;    //error   
}


char IBT_SampleManage::IR_ADC(char chn)
{
	chn = g_chn_order[chn];    //verD通道号颠倒  
	chn_DoIR = chn;
	chn_history = m_ADC_chn;    //记录历史
	IR_CTRL_FLAG = 1;

	return 1;
}
char IBT_SampleManage::renew_IR_ADC(void) 
{
	m_ADC_chn = chn_history;    //还原
	IR_CTRL_FLAG = 0;    //clear

	IBT_Base::ADG_Reset();    //关闭
	ADC_Status = IBT_ADC_IDLE;

	return 1;
}
void IBT_SampleManage::IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI)
{
	//	unsigned char old_chn = chn;
	chn = g_chn_order[chn];    //verD通道号颠倒  这个工作需要软件来完成

	Adc24bit_CS5560::Convert();    //conv

	SSPIFunc::ConfirmDevice(1, &IBT_Base::IBT_SSP1ADC);

	Adc24bit_CS5560::Pin_Busy = IBT_Base::PinGroup[IBT_Pin_AdcBusy1];
	Adc24bit_CS5560::Pin_CS = IBT_Base::PinGroup[IBT_Pin_AdcCS1];
	*SampleI = Adc24bit_CS5560::Read();

	Adc24bit_CS5560::Pin_Busy = IBT_Base::PinGroup[IBT_Pin_AdcBusy2];
	Adc24bit_CS5560::Pin_CS = IBT_Base::PinGroup[IBT_Pin_AdcCS2];
	*SampleV  = Adc24bit_CS5560::Read();
}
