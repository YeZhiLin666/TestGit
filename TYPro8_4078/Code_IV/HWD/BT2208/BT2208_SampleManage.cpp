
#include "../../Entry/includes.h"

//把BT2208_Base内的采样ProcessADC转移进来
//同时更新SampleProcess类文件
//comment out chen 20130216
//打算增加一个特殊的采样支持的IR函数
//直接adcreset  把通道置于该通道
//把状态变成noopen
//运行后，再次还原


//for 接口函数
char BT2208_SampleManage::m_chn;
unsigned long int BT2208_SampleManage::m_value;
char BT2208_SampleManage::m_type;

//内部运转
char BT2208_SampleManage::m_ADC_chn;

char BT2208_SampleManage::chn_history;
char BT2208_SampleManage::chn_DoIR;
char BT2208_SampleManage::ADC_Status;
unsigned char BT2208_SampleManage::IR_CTRL_FLAG;

unsigned short BT2208_SampleManage::TimeTicks;
const unsigned short BT2208_SampleManage::DelayTickCount_ADG = 4; //12;  //600us Open ADG--->200us Open ADG seens more good
const unsigned short BT2208_SampleManage::DelayTickCount_AdcConv = 1;  
const unsigned short BT2208_SampleManage::DelayTickCount_AdcReadData = 1; 



void BT2208_SampleManage::Init(void)
{
	m_chn = 0;
	m_value = 0;
	m_type = 0;

	//内部运转
	m_ADC_chn = 0;

	chn_history = 0;
	//状态机
	ADC_Status = BT2208_ADC_IDLE;

	IR_CTRL_FLAG = 0;
}


char BT2208_SampleManage::SampleISR(unsigned long CH_Number,unsigned long type)
{
	if((OS_ServeProcess::OS_Time.Us100 & 0x0F) == 0)     //每隔1.6毫秒重新计算一次时间？ 这个时间间隔是否合理呢？
		ChannelInfo_Manager::Set_TimeRecalculateSign();

	//	unsigned char bOK;
	switch(ADC_Status)
	{
	case BT2208_ADC_IDLE:
		ADC_Status = BT2208_ADC_OpenADG;
		TimeTicks = 0;
		m_ADC_chn = 0;
		break;
	case BT2208_ADC_OpenADG:
		BT2208_Base::ADC_ADG_open(m_ADC_chn);
		TimeTicks = 0;
		ADC_Status = BT2208_ADC_OPENING;

		if(IR_CTRL_FLAG == 1)    //特殊的IR控制
		{
			BT2208_Base::ADG_Reset();
			m_ADC_chn = chn_DoIR;
			BT2208_Base::ADC_ADG_open(m_ADC_chn);
			ADC_Status = BT2208_ADC_NoWork;    //省去了开关模拟门的时序
		}
		break;
	case BT2208_ADC_OPENING:
		TimeTicks++;
		if(TimeTicks >= DelayTickCount_ADG)
		{
			ADC_Status = BT2208_ADC_TriggerI;
			Adc24bit_CS5560::Convert(); 
			TimeTicks = 0;
		}
		break;
	case BT2208_ADC_TriggerI:
		SSPIFunc::ConfirmDevice(1, &BT2208_Base::BT2208_SSP1ADC);
		Adc24bit_CS5560::Pin_CS = BT2208_Base::PinGroup[BT2208_Pin_AdcCS1];
		Adc24bit_CS5560::Trigger(); 
		ADC_Status = BT2208_ADC_ReadI;
		break;
	case BT2208_ADC_ReadI:
		SSPIFunc::ConfirmDevice(1, &BT2208_Base::BT2208_SSP1ADC);
		unsigned long adc_code_temp = Adc24bit_CS5560::ReadData();    //just for debug
		ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc  = adc_code_temp; 
		ADC_Status = BT2208_ADC_TriggerV;
		break;
	case BT2208_ADC_TriggerV:
		SSPIFunc::ConfirmDevice(1, &BT2208_Base::BT2208_SSP1ADC);
		Adc24bit_CS5560::Pin_CS = BT2208_Base::PinGroup[BT2208_Pin_AdcCS2];
		Adc24bit_CS5560::Trigger(); 
		ADC_Status = BT2208_ADC_ReadV;
		break;
	case BT2208_ADC_ReadV:
		SSPIFunc::ConfirmDevice(1, &BT2208_Base::BT2208_SSP1ADC);
		ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc = Adc24bit_CS5560::ReadData();
		ADC_Status = BT2208_ADC_CloseADG;
		Sample_Pro::NewCodeSampled[m_ADC_chn] = 1;
		break;
	case BT2208_ADC_CloseADG:
		if(IR_CTRL_FLAG == 0)    //如果没有IR特殊控制
		{
			m_ADC_chn++;
			if(m_ADC_chn >= BT2208_ADC_I_CHN_MAX)    //
			{
				m_ADC_chn = 0;
			}

			//关闭
			BT2208_Base::ADG_Reset();
			ADC_Status = BT2208_ADC_OpenADG;
		}
		else    //不做m_ADC_chn自增处理
		{
			//关闭
			BT2208_Base::ADG_Reset();
			ADC_Status = BT2208_ADC_OpenADG;
		}
		break;
	case BT2208_ADC_NoWork:
		break;
	default:
		ADC_Status = BT2208_ADC_IDLE;
		break;
	}

	return 0;    //error	
}
//处理ADC     自我调度运行
//ProcessADC本身是一个FSM
//  首先1 ADC_ADG_open  2 10us  3 CS5560_Read while busy 30us  4 ADC_Reset关闭
//实际上 一个周期 读I和V的 一个chn  此板最大的问题就是采样周期较慢  因为有重新打开的问题
//加入电压采样    20130109    comment out chen
int BT2208_SampleManage::ProcessADC(char adc_device_chn)
{
	return -1;    //error
}



//可以考虑林工所说  见2013 02 16的信件  主题:	 今天的录像
//有待修改 comment out chen 20130216
char BT2208_SampleManage::getCHN(char adc_device_chn)
{
	return m_chn;
}

//返回的是码字
unsigned long int BT2208_SampleManage::getValue(char adc_device_chn)
{
	return m_value;
}

char BT2208_SampleManage::getType(char adc_device_chn)
{
	return m_type;
}


////////////////////////////////////////////////////////////////////////////////////////
//触发该chn的特殊采样，同时记录，并关闭其他通道
char BT2208_SampleManage::IR_ADC(char chn)
{
	chn_history = m_ADC_chn;    //记录历史
	chn_DoIR = chn;
	IR_CTRL_FLAG = 1;

	return 1;
}

//恢复
char BT2208_SampleManage::renew_IR_ADC(void) 
{
	m_ADC_chn = chn_history;    //还原
	IR_CTRL_FLAG = 0;    //clear

	BT2208_Base::ADG_Reset();    //关闭
	ADC_Status = BT2208_ADC_IDLE;

	return 1;
}


///////////////////
void BT2208_SampleManage::IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI)
{
	Adc24bit_CS5560::Convert();    //conv

	ARBUS_DELAY(300);    //等待

	SSPIFunc::ConfirmDevice(1, &BT2208_Base::BT2208_SSP1ADC);

	Adc24bit_CS5560::Pin_Busy = BT2208_Base::PinGroup[BT2208_Pin_AdcBusy1];
	Adc24bit_CS5560::Pin_CS = BT2208_Base::PinGroup[BT2208_Pin_AdcCS1];
	*SampleI = Adc24bit_CS5560::Read();

	Adc24bit_CS5560::Pin_Busy = BT2208_Base::PinGroup[BT2208_Pin_AdcBusy2];
	Adc24bit_CS5560::Pin_CS = BT2208_Base::PinGroup[BT2208_Pin_AdcCS2];
	*SampleV = Adc24bit_CS5560::Read();
}
