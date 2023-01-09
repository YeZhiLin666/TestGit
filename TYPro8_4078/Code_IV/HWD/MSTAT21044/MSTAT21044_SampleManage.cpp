
#include "../../Entry/includes.h"

//��MSTAT21044_Base�ڵĲ���ProcessADCת�ƽ���
//ͬʱ����SampleProcess���ļ�
//comment out chen 20130216
//��������һ������Ĳ���֧�ֵ�IR����
//ֱ��adcreset  ��ͨ�����ڸ�ͨ��
//��״̬���noopen
//���к��ٴλ�ԭ


//for �ӿں���

char MSTAT21044_SampleManage::m_chn;
unsigned long int MSTAT21044_SampleManage::m_value;
char MSTAT21044_SampleManage::m_type;
char MSTAT21044_SampleManage::m_ADC_chn;
unsigned char MSTAT21044_SampleManage::I_ADG_Address[MAXCHANNELNO];

char MSTAT21044_SampleManage::chn_history;
char MSTAT21044_SampleManage::chn_DoIR;
char MSTAT21044_SampleManage::ADC_Status;
unsigned char MSTAT21044_SampleManage::IR_CTRL_FLAG;

unsigned short MSTAT21044_SampleManage::TimeTicks;
const unsigned short MSTAT21044_SampleManage::DelayTickCount_ADG = 8; //12;  //600us Open ADG--->200us Open ADG seens more good
const unsigned short MSTAT21044_SampleManage::DelayTickCount_AdcConv = 1;
const unsigned short MSTAT21044_SampleManage::DelayTickCount_AdcReadData = 1;


void MSTAT21044_SampleManage::Init(void)
{
    m_chn = 0;
    m_value = 0;
    m_type = 0;

    //�ڲ���ת
    m_ADC_chn = 0;

    chn_history = 0;
    //״̬��
    ADC_Status = MSTAT21044_ADC_IDLE;
    for(unsigned char i=0; i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; i++)
    {
        I_ADG_Address[i] = i;
    }
    IR_CTRL_FLAG = 0;
}


char MSTAT21044_SampleManage::SampleISR(unsigned long CH_Number,unsigned long type)
{
    //if((OS_ServeProcess::OS_Time.Us100 & 0x0F) == 0)     //ÿ��1.6�������¼���һ��ʱ�䣿 ���ʱ�����Ƿ�����أ�
//		ChannelInfo_Manager::Set_TimeRecalculateSign();

    //	unsigned char bOK;
    switch(ADC_Status)
    {
    case MSTAT21044_ADC_IDLE:
        ADC_Status = MSTAT21044_ADC_OpenADG;
        TimeTicks = 0;
        m_ADC_chn = 0;
        break;
    case MSTAT21044_ADC_OpenADG:
        MSTAT21044_Base::ADC_ADG_open(m_ADC_chn);
        TimeTicks = 0;
        ADC_Status = MSTAT21044_ADC_OPENING;

        if(IR_CTRL_FLAG == 1)    //�����IR����
        {
            MSTAT21044_Base::ADG_Reset();
            m_ADC_chn = chn_DoIR;
            MSTAT21044_Base::ADC_ADG_open(m_ADC_chn);
            ADC_Status = MSTAT21044_ADC_NoWork;    //ʡȥ�˿���ģ���ŵ�ʱ��
        }
        break;
    case MSTAT21044_ADC_OPENING:
        TimeTicks++;
        if(TimeTicks >= DelayTickCount_ADG)
        {
            ADC_Status = MSTAT21044_ADC_TriggerI;
            Adc24bit_CS5560::Convert();
            TimeTicks = 0;
        }
        break;
    case MSTAT21044_ADC_TriggerI:
        SSPIFunc::ConfirmDevice(1, &MSTAT21044_Base::MSTAT21044_SSP1ADC);
        Adc24bit_CS5560::Pin_CS = MSTAT21044_Base::PinGroup[MSTAT21044_Pin_AdcCS1];
        Adc24bit_CS5560::Trigger();
        ADC_Status = MSTAT21044_ADC_ReadI;
        break;
    case MSTAT21044_ADC_ReadI:
        SSPIFunc::ConfirmDevice(1, &MSTAT21044_Base::MSTAT21044_SSP1ADC);
        if(m_ADC_chn < 4)
            ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc  = Adc24bit_CS5560::ReadData();
        else
            Adc24bit_CS5560::ReadData();   //It must be read out,even it is useless.
        ADC_Status = MSTAT21044_ADC_TriggerV;
        break;
    case MSTAT21044_ADC_TriggerV:
        SSPIFunc::ConfirmDevice(1, &MSTAT21044_Base::MSTAT21044_SSP1ADC);
        Adc24bit_CS5560::Pin_CS = MSTAT21044_Base::PinGroup[MSTAT21044_Pin_AdcCS2];
        Adc24bit_CS5560::Trigger();
        ADC_Status = MSTAT21044_ADC_ReadV;
        break;
    case MSTAT21044_ADC_ReadV:
        SSPIFunc::ConfirmDevice(1, &MSTAT21044_Base::MSTAT21044_SSP1ADC);
        if(m_ADC_chn >= 4)
        {
            ChannelInfo_Manager::ChannelInfo[m_ADC_chn - 4].m_u32BuildInVolt[0] = Adc24bit_CS5560::ReadData();
            ChannelInfo_Manager::ChannelInfo[m_ADC_chn - 4].m_bBuildInVoltNewCode[0] = 1;
        }
        else
        {
            ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc = Adc24bit_CS5560::ReadData();
            Sample_Pro::NewCodeSampled[m_ADC_chn] = 1;
        }
        ADC_Status = MSTAT21044_ADC_CloseADG;
        break;
    case MSTAT21044_ADC_CloseADG:
        if(IR_CTRL_FLAG == 0)    //���û��IR�������
        {
            m_ADC_chn++;
            //if(m_ADC_chn>=ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
            if(m_ADC_chn >= 8)  //21044, 4IV channels + 4 build-in volt channels
                m_ADC_chn=0;
            //�ر�
            ADC_Status = MSTAT21044_ADC_OpenADG;
        }
        else
            ADC_Status = MSTAT21044_ADC_OpenADG;
        break;
    case MSTAT21044_ADC_NoWork:
        //do nothing comment out chen 20130702
        break;
    default:
        ADC_Status = MSTAT21044_ADC_IDLE;
        break;
    }
    return 0;    //error
}
//����ADC     ���ҵ�������
// int MSTAT21044_SampleManage::ProcessADC(char adc_device_chn)
// {
//

// 	return -1;    //error
// }

// char MSTAT21044_SampleManage::getCHN(char adc_device_chn)
// {
// 	return m_chn;
// }

// //���ص�������
// unsigned long int MSTAT21044_SampleManage::getValue(char adc_device_chn)
// {
// 	return m_value;
// }

// char MSTAT21044_SampleManage::getType(char adc_device_chn)
// {
// 	return m_type;
// }

////////////////////////////////////////////////////////////////////////////////////////
//������chn�����������ͬʱ��¼�����ر�����ͨ��
char MSTAT21044_SampleManage::IR_ADC(char chn)
{
    chn_history = m_ADC_chn;    //��¼��ʷ
    chn_DoIR = chn;
    IR_CTRL_FLAG = 1;

    return 1;
}

//�ָ�
char MSTAT21044_SampleManage::renew_IR_ADC(void)
{
    m_ADC_chn = chn_history;    //��ԭ
    IR_CTRL_FLAG = 0;    //clear
    ADC_Status = MSTAT21044_ADC_IDLE;

    return 1;
}

///////////////////
void MSTAT21044_SampleManage::IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI)
{
    Adc24bit_CS5560::Convert();    //conv

    ARBUS_DELAY(300);    //�ȴ�

    //add by hpq,20150730
    Adc24bit_CS5560::Pin_CS = MSTAT21044_Base::PinGroup[MSTAT21044_Pin_AdcCS1];
    ChannelInfo_Manager::ChannelInfo[chn].m_u32CurrentAdc  = Adc24bit_CS5560::Read();

    Adc24bit_CS5560::Pin_CS = MSTAT21044_Base::PinGroup[MSTAT21044_Pin_AdcCS2];
    ChannelInfo_Manager::ChannelInfo[chn].m_u32VoltageAdc  = Adc24bit_CS5560::Read();

    *SampleI = ChannelInfo_Manager::ChannelInfo[chn].m_u32CurrentAdc;//MSTAT21044_ADC5DAC::ADC_rd(chn,MSTAT21044_ADC_TYPE_I);
    *SampleV = ChannelInfo_Manager::ChannelInfo[chn].m_u32VoltageAdc;//MSTAT21044_ADC5DAC::ADC_rd(chn,MSTAT21044_ADC_TYPE_V);     //��ѹ
}
