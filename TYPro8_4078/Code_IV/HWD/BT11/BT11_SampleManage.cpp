
#include "includes.h"


UInt16 BT11_SampleManage::Priority=PRIORITY4;
UInt16 BT11_SampleManage::TaskID=BT11_SAMPLE_TASK;

//for 接口函数
char BT11_SampleManage::m_chn;
unsigned long int BT11_SampleManage::m_value;
char BT11_SampleManage::m_type;


char BT11_SampleManage::m_ADC_chn;
char BT11_SampleManage::chn_DoIR;
char BT11_SampleManage::chn_history;
char BT11_SampleManage::ADC_Status;
unsigned char BT11_SampleManage::IR_CTRL_FLAG;

unsigned short BT11_SampleManage::TimeTicks;

void BT11_SampleManage::Init(void)
{
	m_chn = 0;
	m_value = 0;
	m_type = 0;

	m_ADC_chn = 0;

	chn_history = 0;
	//状态机
	ADC_Status = IBT_ADC_IDLE;

	IR_CTRL_FLAG = 0;
}

void BT11_SampleManage::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
	OS_ServeProcess::Task_Count[COUNT_BT11_SAMPLE] ++;
#endif    
	if(!Pulse::m_Info.m_bPulseSample)
	{//只有慢脉冲的时候才进行TIMER0采样

		unsigned long IRQ1,IRQ2;                
		IRQFunc::DisableIRQ(&IRQ1,&IRQ2);
		IR_Read_ADC(0,
			&ChannelInfo_Manager::ChannelInfo[0].m_u32VoltageAdc,
			&ChannelInfo_Manager::ChannelInfo[0].m_u32CurrentAdc);		
		IRQFunc::EnableIRQ(IRQ1,IRQ2);

		Sample_Pro::NewCodeSampled[0] = 1;
		SampleProcess_Pulse::NewCodeSampled = 1;
	}
}

char BT11_SampleManage::SampleISR(unsigned long CH_Number,unsigned long type)
{
	ChannelInfo_Manager::Set_TimeRecalculateSign();

	return 0;    //error	
}



char BT11_SampleManage::IR_ADC(char chn)
{
	chn_history = m_ADC_chn;    //记录历史
	chn_DoIR = chn;
	IR_CTRL_FLAG = 1;

	return 1;
}

//恢复
char BT11_SampleManage::renew_IR_ADC(void) 
{
	m_ADC_chn = chn_history;    //还原
	IR_CTRL_FLAG = 0;    //clear
	ADC_Status = IBT_ADC_IDLE;

	return 1;
}


///////////////////

void  BT11_SampleManage::IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI)
{
	unsigned long Current[3];     //记录3次采样的电流码值
	unsigned long Voltage[3];     //记录3次采样的电压码值

	Adc16bit_AD7606::Convert();
	Adc16bit_AD7606::ReadAfterBusy(&Current[0],&Voltage[0]);
	Adc16bit_AD7606::Convert();
	Adc16bit_AD7606::ReadAfterBusy(&Current[1],&Voltage[1]);
	Adc16bit_AD7606::Convert();
	Adc16bit_AD7606::ReadAfterBusy(&Current[2],&Voltage[2]);

	if((Current[0] > Current[1]) == (Current[0] < Current[2]))
		*SampleI = Current[0];
	else if((Current[1] > Current[0]) == (Current[1] < Current[2]))
		*SampleI = Current[1];
	else
		*SampleI = Current[2];

	if((Voltage[0] > Voltage[1]) == (Voltage[0] < Voltage[2]))
		*SampleV = Voltage[0];
	else if((Voltage[1] > Voltage[0]) == (Voltage[1] < Voltage[2]))
		*SampleV = Voltage[1];
	else
		*SampleV = Voltage[2];
}
