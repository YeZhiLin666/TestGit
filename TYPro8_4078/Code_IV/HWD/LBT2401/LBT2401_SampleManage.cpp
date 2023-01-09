
#include "../../Entry/includes.h"

//把LBT2401_Base内的采样ProcessADC转移进来
//同时更新SampleProcess类文件
//comment out chen 20130216
//打算增加一个特殊的采样支持的IR函数
//直接adcreset  把通道置于该通道
//把状态变成noopen
//运行后，再次还原


//for 接口函数
char LBT2401_SampleManage::m_chn;
unsigned long int LBT2401_SampleManage::m_value;
char LBT2401_SampleManage::m_type;
char LBT2401_SampleManage::m_ADC_chn;

char LBT2401_SampleManage::chn_history;
char LBT2401_SampleManage::chn_DoIR;
char LBT2401_SampleManage::ADC_Status;
unsigned char LBT2401_SampleManage::IR_CTRL_FLAG;

unsigned short LBT2401_SampleManage::TimeTicks;
const unsigned short LBT2401_SampleManage::DelayTickCount_ADG = 4; //12;  //600us Open ADG--->200us Open ADG seens more good
const unsigned short LBT2401_SampleManage::DelayTickCount_AdcConv = 1;  
const unsigned short LBT2401_SampleManage::DelayTickCount_AdcReadData = 1; 

void LBT2401_SampleManage::Init(void)
{
	m_chn = 0;
	m_value = 0;
	m_type = 0;

	m_ADC_chn = 0;

	chn_history = 0;
	//状态机
	ADC_Status = LBT2401_ADC_IDLE;

	IR_CTRL_FLAG = 0;
}


char LBT2401_SampleManage::SampleISR(unsigned long CH_Number,unsigned long type)
{
	//if((OS_ServeProcess::OS_Time.Us100 & 0x0F) == 0)     //每隔1.6毫秒重新计算一次时间？ 这个时间间隔是否合理呢？
		ChannelInfo_Manager::Set_TimeRecalculateSign();

	TimeTicks++;

	//	unsigned char bOK;
	switch(ADC_Status)
	{
	case LBT2401_ADC_IDLE:
		ADC_Status = LBT2401_ADC_WAIT;
		TimeTicks = 0;
		m_ADC_chn = 0;
		break;
	case LBT2401_ADC_WAIT:
		SSPIFunc::ClearFIFO(1);
		ADC_Status = LBT2401_ADC_TriggerI;
		Adc24bit_CS5560::Convert(); 
		break;
	case LBT2401_ADC_TriggerI:
		SSPIFunc::ConfirmDevice(1, &LBT2401_Base::LBT2401_SSP1ADC);
		Adc24bit_CS5560::Pin_CS = LBT2401_Base::PinGroup[LBT2401_Pin_AdcCS1];
		Adc24bit_CS5560::Trigger(); 
		ADC_Status = LBT2401_ADC_ReadI;
		break;
	case LBT2401_ADC_ReadI:
		if(SSPIFunc::getModuleName(1)  != SSP1_DEVICE1)
		{
			SSPIFunc::setPara(&LBT2401_Base::LBT2401_SSP1ADC,1);
			SSPIFunc::my_Init(1);              //初始化
			ARBUS_DELAY(10);
		}
		ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc = Adc24bit_CS5560::ReadData();    //just for debug
		ADC_Status = LBT2401_ADC_TriggerV;
		break;
	case LBT2401_ADC_TriggerV:
		SSPIFunc::ConfirmDevice(1, &LBT2401_Base::LBT2401_SSP1ADC);
		Adc24bit_CS5560::Pin_CS = LBT2401_Base::PinGroup[LBT2401_Pin_AdcCS2];
		Adc24bit_CS5560::Trigger(); 
		ADC_Status = LBT2401_ADC_ReadV;
		break;
	case LBT2401_ADC_ReadV:
		SSPIFunc::ConfirmDevice(1, &LBT2401_Base::LBT2401_SSP1ADC);
		ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc = Adc24bit_CS5560::ReadData();
		Sample_Pro::NewCodeSampled[0] = 1;
		ADC_Status = LBT2401_ADC_WAIT;
		break;
	default:
		ADC_Status = LBT2401_ADC_IDLE;
		break;
	}

	return 0;    //error	
}


////////////////////////////////////////////////////////////////////////////////////////
//触发该chn的特殊采样，同时记录，并关闭其他通道
char LBT2401_SampleManage::IR_ADC(char chn)
{
	chn_history = m_ADC_chn;    //记录历史
	chn_DoIR = chn;
	IR_CTRL_FLAG = 1;

	return 1;
}

//恢复
char LBT2401_SampleManage::renew_IR_ADC(void) 
{
	m_ADC_chn = chn_history;    //还原
	IR_CTRL_FLAG = 0;    //clear

	LBT2401_Base::ADG_Reset();    //关闭
	ADC_Status = LBT2401_ADC_IDLE;

	return 1;
}

///////////////////
void LBT2401_SampleManage::IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI)
{
	Adc24bit_CS5560::Convert();    //conv

	ARBUS_DELAY(300);    //等待

	*SampleI = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc;//LBT2401_ADC5DAC::ADC_rd(chn,LBT2401_ADC_TYPE_I);
	*SampleV = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc;//LBT2401_ADC5DAC::ADC_rd(chn,LBT2401_ADC_TYPE_V);     //电压
}
