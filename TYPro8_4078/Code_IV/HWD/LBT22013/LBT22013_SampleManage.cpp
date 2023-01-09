
#include "../../Entry/includes.h"

//��LBT22013_Base�ڵĲ���ProcessADCת�ƽ���
//ͬʱ����SampleProcess���ļ�
//comment out chen 20130216
//��������һ������Ĳ���֧�ֵ�IR����
//ֱ��adcreset  ��ͨ�����ڸ�ͨ��
//��״̬���noopen
//���к��ٴλ�ԭ


//for �ӿں���
char LBT22013_SampleManage::m_chn;
unsigned long int LBT22013_SampleManage::m_value;
char LBT22013_SampleManage::m_type;
char LBT22013_SampleManage::m_ADC_chn;

char LBT22013_SampleManage::chn_history;
char LBT22013_SampleManage::chn_DoIR;
char LBT22013_SampleManage::ADC_Status;
unsigned char LBT22013_SampleManage::IR_CTRL_FLAG;

unsigned short LBT22013_SampleManage::TimeTicks;

void LBT22013_SampleManage::Init(void)
{
    m_chn = 0;
    m_value = 0;
    m_type = 0;

    //�ڲ���ת
    m_ADC_chn = 0;

    chn_history = 0;
    //״̬��
    ADC_Status = LBT22013_ADC_IDLE;

    IR_CTRL_FLAG = 0;
}


char LBT22013_SampleManage::SampleISR(unsigned long CH_Number,unsigned long type)
{
    //if((OS_ServeProcess::OS_Time.Us100 & 0x0F) == 0)     //ÿ��1.6�������¼���һ��ʱ�䣿 ���ʱ�����Ƿ�����أ�
//		ChannelInfo_Manager::Set_TimeRecalculateSign();

    TimeTicks++;

    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {
#if ADC_CS5560_ENABLE == 1
        switch(ADC_Status)
        {
        case LBT22013_ADC_IDLE:
            ADC_Status = LBT22013_ADC_WAIT;
            TimeTicks = 0;
            m_ADC_chn = 0;
            break;
        case LBT22013_ADC_WAIT:
            SSPIFunc::ClearFIFO(1);
            ADC_Status = LBT22013_ADC_TriggerI;
            Adc24bit_CS5560::Convert();
            break;
        case LBT22013_ADC_TriggerI:
            SSPIFunc::ConfirmDevice(1, &LBT22013_Base::LBT22013_SSP1ADC);
            Adc24bit_CS5560::Pin_CS = LBT22013_Base::PinGroupVA[LBT22013_VA_Pin_AdcCS1];
            Adc24bit_CS5560::Trigger();
            ADC_Status = LBT22013_ADC_ReadI;
            break;
        case LBT22013_ADC_ReadI:
            SSPIFunc::ConfirmDevice(1, &LBT22013_Base::LBT22013_SSP1ADC);
            ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc = Adc24bit_CS5560::ReadData();    //just for debug
            ADC_Status = LBT22013_ADC_TriggerV;
            break;
        case LBT22013_ADC_TriggerV:
            SSPIFunc::ConfirmDevice(1, &LBT22013_Base::LBT22013_SSP1ADC);
            Adc24bit_CS5560::Pin_CS = LBT22013_Base::PinGroupVA[LBT22013_VA_Pin_AdcCS2];
            Adc24bit_CS5560::Trigger();
            ADC_Status = LBT22013_ADC_ReadV;
            break;
        case LBT22013_ADC_ReadV:
            SSPIFunc::ConfirmDevice(1, &LBT22013_Base::LBT22013_SSP1ADC);
            ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc = Adc24bit_CS5560::ReadData();
            Sample_Pro::NewCodeSampled[0] = 1;
            Sample_Pro::NewIRSampled[m_ADC_chn] = 1;

            if(BurstModeSampling::uc_A_BurstMode[m_ADC_chn])
                BurstModeSampling::BurstSavePoint(m_ADC_chn);//  2017.6.8  yy

            ADC_Status = LBT22013_ADC_WAIT;
            break;
        default:
            ADC_Status = LBT22013_ADC_IDLE;
            break;
        }
#endif
    }
    else
    {
        switch(ADC_Status)
        {
        case LBT22013_ADC_IDLE:
            ADC_Status = LBT22013_ADC_WAIT;
            TimeTicks = 0;
            m_ADC_chn = 0;
            break;
        case LBT22013_ADC_WAIT:
            ADC_Status = LBT22013_ADC_ReadIV;
            Adc16bit_AD7606::Convert();
            break;
        case LBT22013_ADC_ReadIV:
            Adc16bit_AD7606::Read(&ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc,
                                  &ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc);
            Sample_Pro::NewCodeSampled[m_ADC_chn] = 1;
            Sample_Pro::NewIRSampled[m_ADC_chn] = 1;
            ADC_Status = LBT22013_ADC_WAIT;
            break;
        default:
            Adc16bit_AD7606::Reset();
            ADC_Status = LBT22013_ADC_IDLE;
            break;
        }
    }
    return 0;    //error
}


////////////////////////////////////////////////////////////////////////////////////////
//������chn�����������ͬʱ��¼�����ر�����ͨ��
char LBT22013_SampleManage::IR_ADC(char chn)
{
    chn_history = m_ADC_chn;    //��¼��ʷ
    chn_DoIR = chn;
    IR_CTRL_FLAG = 1;

    return 1;
}

//�ָ�
char LBT22013_SampleManage::renew_IR_ADC(void)
{
    m_ADC_chn = chn_history;    //��ԭ
    IR_CTRL_FLAG = 0;    //clear
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
        LBT22013_Base::ADG_Reset();    //�ر�
    ADC_Status = LBT22013_ADC_IDLE;

    return 1;
}

///////////////////
void LBT22013_SampleManage::IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI)
{
#if ADC_CS5560_ENABLE == 1
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {
        Adc24bit_CS5560::Convert();    //conv
        ARBUS_DELAY(300);    //�ȴ�
    }
#endif
    *SampleI = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc;
    *SampleV = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc;    //��ѹ
}




