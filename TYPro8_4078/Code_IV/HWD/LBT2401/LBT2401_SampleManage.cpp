
#include "../../Entry/includes.h"

//��LBT2401_Base�ڵĲ���ProcessADCת�ƽ���
//ͬʱ����SampleProcess���ļ�
//comment out chen 20130216
//��������һ������Ĳ���֧�ֵ�IR����
//ֱ��adcreset  ��ͨ�����ڸ�ͨ��
//��״̬���noopen
//���к��ٴλ�ԭ


//for �ӿں���
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
	//״̬��
	ADC_Status = LBT2401_ADC_IDLE;

	IR_CTRL_FLAG = 0;
}


char LBT2401_SampleManage::SampleISR(unsigned long CH_Number,unsigned long type)
{
	//if((OS_ServeProcess::OS_Time.Us100 & 0x0F) == 0)     //ÿ��1.6�������¼���һ��ʱ�䣿 ���ʱ�����Ƿ�����أ�
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
			SSPIFunc::my_Init(1);              //��ʼ��
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
//������chn�����������ͬʱ��¼�����ر�����ͨ��
char LBT2401_SampleManage::IR_ADC(char chn)
{
	chn_history = m_ADC_chn;    //��¼��ʷ
	chn_DoIR = chn;
	IR_CTRL_FLAG = 1;

	return 1;
}

//�ָ�
char LBT2401_SampleManage::renew_IR_ADC(void) 
{
	m_ADC_chn = chn_history;    //��ԭ
	IR_CTRL_FLAG = 0;    //clear

	LBT2401_Base::ADG_Reset();    //�ر�
	ADC_Status = LBT2401_ADC_IDLE;

	return 1;
}

///////////////////
void LBT2401_SampleManage::IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI)
{
	Adc24bit_CS5560::Convert();    //conv

	ARBUS_DELAY(300);    //�ȴ�

	*SampleI = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc;//LBT2401_ADC5DAC::ADC_rd(chn,LBT2401_ADC_TYPE_I);
	*SampleV = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc;//LBT2401_ADC5DAC::ADC_rd(chn,LBT2401_ADC_TYPE_V);     //��ѹ
}
