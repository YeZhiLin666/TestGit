
#include "../../Entry/includes.h"

//把IBT_Base内的采样ProcessADC转移进来
//同时更新SampleProcess类文件
//comment out chen 20130216
//打算增加一个特殊的采样支持的IR函数
//直接adcreset  把通道置于该通道
//把状态变成noopen
//运行后，再次还原
//Tesla 不清楚是否可以更快点


//for 接口函数
char TESLA_SampleManage::m_chn;
unsigned long int TESLA_SampleManage::m_value;
char TESLA_SampleManage::m_type;

char TESLA_SampleManage::m_ADC_chn;

//for IR
char TESLA_SampleManage::chn_history;
char TESLA_SampleManage::chn_DoIR;
char TESLA_SampleManage::ADC_Status;
unsigned char TESLA_SampleManage::IR_CTRL_FLAG;


unsigned short TESLA_SampleManage::TimeTicks;

//信息内置宏
const unsigned short TESLA_SampleManage::DelayTickCount_ADG = TESLA_ADC_ADG_TICKS; 
const unsigned short TESLA_SampleManage::DelayTickCount_AdcConv = TESLA_ADC_CONV_TICKS;  
const unsigned short TESLA_SampleManage::DelayTickCount_AdcReadData = TESLA_ADC_READ_TICKS;  


void TESLA_SampleManage::Init(void)
{
	m_chn = 0;
	m_value = 0;
	m_type = 0;

	m_ADC_chn = 0;

	chn_history = 0;

	IR_CTRL_FLAG = 0;

	//状态机
	ADC_Status = TESLA_ADC_IDLE;    //ADC_Status 
}

char TESLA_SampleManage::SampleISR(unsigned long CH_Number,unsigned long type)
{
	//if((OS_ServeProcess::OS_Time.Us100 & 0x07) == 0)
//		ChannelInfo_Manager::Set_TimeRecalculateSign();

	switch(ADC_Status)
	{
	case TESLA_ADC_IDLE:
		ADC_Status = TESLA_ADC_OpenADG;
		TESLA_Base::ADG_Reset();
		TimeTicks = 0;
		m_ADC_chn = 0;
		break;
	case TESLA_ADC_OpenADG:
		if(IR_CTRL_FLAG)    //特殊的IR控制
		{
			m_ADC_chn = chn_DoIR;
			ADC_Status = TESLA_ADC_NoWork;    //省去了开关模拟门的时序
		}
		else
		{
			TimeTicks = 0;
			ADC_Status = TESLA_ADC_OPENING;
		}
		TESLA_Base::ADC_ADG_open(m_ADC_chn);
		break;
	case TESLA_ADC_OPENING:
		TimeTicks++;
		if(TimeTicks >= DelayTickCount_ADG)  //修改状态机后，减少了2个50us
		{
			SSPIFunc::ClearFIFO(1);
			ADC_Status = TESLA_ADC_TriggerI;
			Adc24bit_CS5560::Convert(); 
			TimeTicks = 0;
		}
		break;
	case TESLA_ADC_TriggerI:
		TESLA_Base::ADG_Reset();

		Adc24bit_CS5560::Pin_CS = TESLA_Base::PinGroup[TESLA_Pin_AdcCS1];   
		ControlPin::SetMeLow(Adc24bit_CS5560::Pin_CS); 
		SSPIFunc::ConfirmDevice(1, &TESLA_Base::Tesla_SSP1IADC);
		Adc24bit_CS5560::Trigger(); 
		ADC_Status = TESLA_ADC_ReadI;
		break;
	case TESLA_ADC_ReadI:
		TESLA_Base::ADC_ADG_open((m_ADC_chn+1)%TESLA_ADC_I_CHN_MAX);
		SSPIFunc::ConfirmDevice(1, &TESLA_Base::Tesla_SSP1IADC);
		ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc  = Adc24bit_CS5560::ReadData(); 
		Adc24bit_CS5560::Pin_CS = TESLA_Base::PinGroup[TESLA_Pin_AdcCS2];
		ControlPin::SetMeLow(Adc24bit_CS5560::Pin_CS); 
		SSPIFunc::ConfirmDevice(1, &TESLA_Base::Tesla_SSP1IADC);
		Adc24bit_CS5560::Trigger();    
		ADC_Status = TESLA_ADC_ReadV;
		break;
	case TESLA_ADC_ReadV:
#if DEBUG_TASK_STATISTIC == 1
		if(m_ADC_chn == 0)
			OS_ServeProcess::Task_Count[COUNT_SAMPLE_ADC]++;
#endif
		SSPIFunc::ConfirmDevice(1, &TESLA_Base::Tesla_SSP1IADC);
		ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc = Adc24bit_CS5560::ReadData();
		Sample_Pro::NewCodeSampled[m_ADC_chn] = 1;
		if(IR_CTRL_FLAG == 0)    //如果没有IR特殊控制
		{
			m_ADC_chn++;
			if(m_ADC_chn >= TESLA_ADC_I_CHN_MAX)    //
				m_ADC_chn = 0;
			ADC_Status = TESLA_ADC_OPENING;
			TimeTicks++;
		}
		else
		{
			TESLA_Base::ADG_Reset();
			ADC_Status = TESLA_ADC_OpenADG;
		}
		break;
	case TESLA_ADC_NoWork:
		break;
	default:
		ADC_Status = TESLA_ADC_IDLE;
		break;
	}

	return 0;    //error	
}


////////////////////////////////////////////////////////////////////////////////////////
//触发该chn的特殊采样，同时记录，并关闭其他通道
char TESLA_SampleManage::IR_ADC(char chn)
{
	chn_history = m_ADC_chn;    //记录历史
	chn_DoIR = chn;

	IR_CTRL_FLAG = 1;

	return 1;
}

//恢复
char TESLA_SampleManage::renew_IR_ADC(void) 
{
	m_ADC_chn = chn_history;    //还原
	IR_CTRL_FLAG = 0;    //clear

	TESLA_Base::ADG_Reset();    //关闭
	ADC_Status = TESLA_ADC_IDLE;

	return 1;
}


///////////////////
void TESLA_SampleManage::IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI)
{
	Adc24bit_CS5560::Convert(); 

	ARBUS_DELAY(300);    //等待

	SSPIFunc::ConfirmDevice(1, &TESLA_Base::Tesla_SSP1IADC);

	Adc24bit_CS5560::Pin_Busy = TESLA_Base::PinGroup[TESLA_Pin_AdcBusy1];
	Adc24bit_CS5560::Pin_CS = TESLA_Base::PinGroup[TESLA_Pin_AdcCS1];
	*SampleI = Adc24bit_CS5560::Read();

	Adc24bit_CS5560::Pin_Busy = TESLA_Base::PinGroup[TESLA_Pin_AdcBusy2];
	Adc24bit_CS5560::Pin_CS = TESLA_Base::PinGroup[TESLA_Pin_AdcCS2];
	*SampleV = Adc24bit_CS5560::Read();
}

