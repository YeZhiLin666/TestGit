
#include "../../Entry/includes.h"

//��BT2208_Base�ڵĲ���ProcessADCת�ƽ���
//ͬʱ����SampleProcess���ļ�
//comment out chen 20130216
//��������һ������Ĳ���֧�ֵ�IR����
//ֱ��adcreset  ��ͨ�����ڸ�ͨ��
//��״̬���noopen
//���к��ٴλ�ԭ


//for �ӿں���
char BT2208_SampleManage::m_chn;
unsigned long int BT2208_SampleManage::m_value;
char BT2208_SampleManage::m_type;

//�ڲ���ת
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

	//�ڲ���ת
	m_ADC_chn = 0;

	chn_history = 0;
	//״̬��
	ADC_Status = BT2208_ADC_IDLE;

	IR_CTRL_FLAG = 0;
}


char BT2208_SampleManage::SampleISR(unsigned long CH_Number,unsigned long type)
{
	if((OS_ServeProcess::OS_Time.Us100 & 0x0F) == 0)     //ÿ��1.6�������¼���һ��ʱ�䣿 ���ʱ�����Ƿ�����أ�
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

		if(IR_CTRL_FLAG == 1)    //�����IR����
		{
			BT2208_Base::ADG_Reset();
			m_ADC_chn = chn_DoIR;
			BT2208_Base::ADC_ADG_open(m_ADC_chn);
			ADC_Status = BT2208_ADC_NoWork;    //ʡȥ�˿���ģ���ŵ�ʱ��
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
		if(IR_CTRL_FLAG == 0)    //���û��IR�������
		{
			m_ADC_chn++;
			if(m_ADC_chn >= BT2208_ADC_I_CHN_MAX)    //
			{
				m_ADC_chn = 0;
			}

			//�ر�
			BT2208_Base::ADG_Reset();
			ADC_Status = BT2208_ADC_OpenADG;
		}
		else    //����m_ADC_chn��������
		{
			//�ر�
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
//����ADC     ���ҵ�������
//ProcessADC������һ��FSM
//  ����1 ADC_ADG_open  2 10us  3 CS5560_Read while busy 30us  4 ADC_Reset�ر�
//ʵ���� һ������ ��I��V�� һ��chn  �˰�����������ǲ������ڽ���  ��Ϊ�����´򿪵�����
//�����ѹ����    20130109    comment out chen
int BT2208_SampleManage::ProcessADC(char adc_device_chn)
{
	return -1;    //error
}



//���Կ����ֹ���˵  ��2013 02 16���ż�  ����:	 �����¼��
//�д��޸� comment out chen 20130216
char BT2208_SampleManage::getCHN(char adc_device_chn)
{
	return m_chn;
}

//���ص�������
unsigned long int BT2208_SampleManage::getValue(char adc_device_chn)
{
	return m_value;
}

char BT2208_SampleManage::getType(char adc_device_chn)
{
	return m_type;
}


////////////////////////////////////////////////////////////////////////////////////////
//������chn�����������ͬʱ��¼�����ر�����ͨ��
char BT2208_SampleManage::IR_ADC(char chn)
{
	chn_history = m_ADC_chn;    //��¼��ʷ
	chn_DoIR = chn;
	IR_CTRL_FLAG = 1;

	return 1;
}

//�ָ�
char BT2208_SampleManage::renew_IR_ADC(void) 
{
	m_ADC_chn = chn_history;    //��ԭ
	IR_CTRL_FLAG = 0;    //clear

	BT2208_Base::ADG_Reset();    //�ر�
	ADC_Status = BT2208_ADC_IDLE;

	return 1;
}


///////////////////
void BT2208_SampleManage::IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI)
{
	Adc24bit_CS5560::Convert();    //conv

	ARBUS_DELAY(300);    //�ȴ�

	SSPIFunc::ConfirmDevice(1, &BT2208_Base::BT2208_SSP1ADC);

	Adc24bit_CS5560::Pin_Busy = BT2208_Base::PinGroup[BT2208_Pin_AdcBusy1];
	Adc24bit_CS5560::Pin_CS = BT2208_Base::PinGroup[BT2208_Pin_AdcCS1];
	*SampleI = Adc24bit_CS5560::Read();

	Adc24bit_CS5560::Pin_Busy = BT2208_Base::PinGroup[BT2208_Pin_AdcBusy2];
	Adc24bit_CS5560::Pin_CS = BT2208_Base::PinGroup[BT2208_Pin_AdcCS2];
	*SampleV = Adc24bit_CS5560::Read();
}
