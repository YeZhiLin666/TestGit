#include "../../Entry/includes.h"
#define LBT21164_FILTER_DEPTH   3

char LBT21164_SampleManage::m_chn;
unsigned long int LBT21164_SampleManage::m_value;
char LBT21164_SampleManage::m_type;
char LBT21164_SampleManage::m_ADC_chn;

char LBT21164_SampleManage::chn_history;
char LBT21164_SampleManage::chn_DoIR;
char LBT21164_SampleManage::ADC_Status;
unsigned char LBT21164_SampleManage::IR_CTRL_FLAG;

unsigned short LBT21164_SampleManage::TimeTicks;
const unsigned short LBT21164_SampleManage::DelayTickCount_ADG = 8; //12;  //600us Open ADG--->200us Open ADG seens more good
const unsigned short LBT21164_SampleManage::DelayTickCount_AdcConv = 1;  
const unsigned short LBT21164_SampleManage::DelayTickCount_AdcReadData = 1; 
#if LBT21164_FILTER_DEPTH > 2
unsigned long uMaxCurr[MAXCHANNELNO];
unsigned long uMinCurr[MAXCHANNELNO];
unsigned long uSumCurr[MAXCHANNELNO];
unsigned long uLastCurr[MAXCHANNELNO];  //�ϴμ�¼�ĵ���ֵ
unsigned char uCurrSampCount[MAXCHANNELNO];
unsigned char uErrCount[MAXCHANNELNO]; //�����仯�ϴ�ļ�����

#endif

void LBT21164_SampleManage::Init(void)
{
	m_chn = 0;
	m_value = 0;
	m_type = 0;
	m_ADC_chn = 0;
	chn_history = 0;
	ADC_Status = LBT21164_ADC_IDLE;

	IR_CTRL_FLAG = 0;
}


char LBT21164_SampleManage::SampleISR(unsigned long CH_Number,unsigned long type)
{
	if((OS_ServeProcess::OS_Time.Us100 & 0x0F) == 0)     //ÿ��1.6�������¼���һ��ʱ�䣿 ���ʱ�����Ƿ�����أ�
		ChannelInfo_Manager::Set_TimeRecalculateSign();

	unsigned char bOK;
	switch(ADC_Status)
	{
	case LBT21164_ADC_IDLE:
		ADC_Status = LBT21164_ADC_OpenADG;
#if LBT21164_FILTER_DEPTH > 2
		for(m_ADC_chn = 0;m_ADC_chn < MAXCHANNELNO;m_ADC_chn++)
		{
			uCurrSampCount[m_ADC_chn] = 0;
			uErrCount[m_ADC_chn]=0;
			uMaxCurr[m_ADC_chn] = 0;
			uMinCurr[m_ADC_chn] = 0xFFFFFF;
			uSumCurr[m_ADC_chn] = 0;
		}
#endif
		TimeTicks = 0;
		m_ADC_chn = 0;
		break;
	case LBT21164_ADC_OpenADG:
		if(IR_CTRL_FLAG)    //�����IR����
		{
			m_ADC_chn = chn_DoIR;
			LBT21164_Base::ADC_ADG_open(m_ADC_chn);
			ADC_Status = LBT21164_ADC_NoWork;    //ʡȥ�˿���ģ���ŵ�ʱ��
		}
		else
		{
			LBT21164_Base::ADC_ADG_open(m_ADC_chn);
			TimeTicks = 0;
			ADC_Status = LBT21164_ADC_OPENING;
		}
		break;
	case LBT21164_ADC_OPENING:
		TimeTicks++;
		if(TimeTicks >= DelayTickCount_ADG)
		{
			ADC_Status = LBT21164_ADC_TriggerI;
			Adc24bit_CS5560::Convert(); 
			TimeTicks = 0;
		}
		break;
	case LBT21164_ADC_TriggerI:
		bOK = SSPIFunc::getModuleName(1);
		if(bOK  != SSP1_DEVICE1)
		{
			SSPIFunc::setPara(&LBT21164_Base::LBT21164_SSP1ADC,1);
			SSPIFunc::my_Init(1);              //��ʼ��
			ARBUS_DELAY(10);
		}
		Adc24bit_CS5560::Pin_CS = LBT21164_Base::Pin_AdcCS1;
		Adc24bit_CS5560::Trigger(); 
		ADC_Status = LBT21164_ADC_ReadI;
		break;
	case LBT21164_ADC_ReadI:
		if(SSPIFunc::getModuleName(1)  != SSP1_DEVICE1)
		{
			SSPIFunc::setPara(&LBT21164_Base::LBT21164_SSP1ADC,1);
			SSPIFunc::my_Init(1);              //��ʼ��
			ARBUS_DELAY(10);
		}
#if LBT21164_FILTER_DEPTH <= 2  //sunshine
		ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc  = Adc24bit_CS5560::ReadData(); 
#else
		unsigned long Current ;
		unsigned long TempL;
		Current = Adc24bit_CS5560::ReadData();

		if(Current > uMaxCurr[m_ADC_chn])
			uMaxCurr[m_ADC_chn] = Current;
		if(Current < uMinCurr[m_ADC_chn])
			uMinCurr[m_ADC_chn] = Current;
		uSumCurr[m_ADC_chn] += Current;
		uCurrSampCount[m_ADC_chn]++;
		if(uCurrSampCount[m_ADC_chn] >= LBT21164_FILTER_DEPTH)
		{
			//�жϵ����Ƿ�仯̫�����̫���򲻴���
			Current=(uSumCurr[m_ADC_chn] - uMaxCurr[m_ADC_chn] - uMinCurr[m_ADC_chn]) / (LBT21164_FILTER_DEPTH - 2); //ȥ�����ֵ��ȥ�����ֵ���ó����
			TempL=ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc;
			unsigned long DeltaCurr = 0;
			if(TempL > Current)
				DeltaCurr = TempL - Current;
			else
				DeltaCurr = Current - TempL;
			if(DeltaCurr > 4194)  //0.05%
				//if ((Current >TempL*1.0005)||(Current <TempL*0.9995))//��������仯����0.05%���������ж�2�η��ı���
			{ uErrCount[m_ADC_chn]++;
			if (uErrCount[m_ADC_chn]>=2)
				goto refresh_Channelinfo;				
			else //�������2�Σ���ˢ��
				goto no_refresh;
			}
			else
				goto refresh_Channelinfo;

refresh_Channelinfo:
			ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc = Current;
			uErrCount[m_ADC_chn]=0;

no_refresh:				
			uCurrSampCount[m_ADC_chn] = 0;
			uMaxCurr[m_ADC_chn] = 0;
			uMinCurr[m_ADC_chn] = 0xFFFFFF;
			uSumCurr[m_ADC_chn] = 0;
		}
#endif
		ADC_Status = LBT21164_ADC_TriggerV;
		break;
	case LBT21164_ADC_TriggerV:
		if(SSPIFunc::getModuleName(1)  != SSP1_DEVICE1)
		{
			SSPIFunc::setPara(&LBT21164_Base::LBT21164_SSP1ADC,1);
			SSPIFunc::my_Init(1);              //��ʼ��
			ARBUS_DELAY(10);
		}
		Adc24bit_CS5560::Pin_CS = LBT21164_Base::Pin_AdcCS2;
		Adc24bit_CS5560::Trigger(); 
		ADC_Status = LBT21164_ADC_ReadV;
		break;
	case LBT21164_ADC_ReadV:
		if(SSPIFunc::getModuleName(1)  != SSP1_DEVICE1)
		{
			SSPIFunc::setPara(&LBT21164_Base::LBT21164_SSP1ADC,1);
			SSPIFunc::my_Init(1);              //��ʼ��
			ARBUS_DELAY(10);
		}
		ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc = Adc24bit_CS5560::ReadData();
		Sample_Pro::NewCodeSampled[m_ADC_chn] = 1;
		m_ADC_chn++;	
		if(m_ADC_chn>=ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
			m_ADC_chn=0;	
		ADC_Status = LBT21164_ADC_OpenADG;
		break;
	case LBT21164_ADC_NoWork://do nothing comment out chen 20130702		
		break;
	default:
		ADC_Status = LBT21164_ADC_IDLE;
		break;
	}

	return 0;    //error	
}
//����ADC     ���ҵ�������
int LBT21164_SampleManage::ProcessADC(char adc_device_chn)
{
	return -1;    //error
}
char LBT21164_SampleManage::getCHN(char adc_device_chn)
{
	return m_chn;
}
unsigned long int LBT21164_SampleManage::getValue(char adc_device_chn)
{
	return m_value;
}
char LBT21164_SampleManage::getType(char adc_device_chn)
{
	return m_type;
}
char LBT21164_SampleManage::IR_ADC(char chn)
{
	chn_history = m_ADC_chn;    //��¼��ʷ
	chn_DoIR = chn;
	IR_CTRL_FLAG = 1;

	return 1;
}
char LBT21164_SampleManage::renew_IR_ADC(void) 
{
	m_ADC_chn = chn_history;    //��ԭ
	IR_CTRL_FLAG = 0;    //clear
	ADC_Status = LBT21164_ADC_IDLE;

	return 1;
}

void LBT21164_SampleManage::IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI)
{
	Adc24bit_CS5560::Convert();    //conv

	ARBUS_DELAY(300);    //�ȴ�

	*SampleI = LBT21164_ADC5DAC::ADC_rd(chn,ADC_TYPE_I);
	*SampleV = LBT21164_ADC5DAC::ADC_rd(chn,ADC_TYPE_V);     //��ѹ
}
