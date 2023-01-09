
#include "../../Entry/includes.h"

//��LBT20162_Base�ڵĲ���ProcessADCת�ƽ���
//ͬʱ����SampleProcess���ļ�
//comment out chen 20130216
//��������һ������Ĳ���֧�ֵ�IR����
//ֱ��adcreset  ��ͨ�����ڸ�ͨ��
//��״̬���noopen
//���к��ٴλ�ԭ


//for �ӿں���

char LBT20162_SampleManage::m_chn;
unsigned long int LBT20162_SampleManage::m_value;
char LBT20162_SampleManage::m_type;
char LBT20162_SampleManage::m_ADC_chn;
unsigned char LBT20162_SampleManage::I_ADG_Address[MAXCHANNELNO];

char LBT20162_SampleManage::chn_history;
char LBT20162_SampleManage::chn_DoIR;
char LBT20162_SampleManage::ADC_Status;
unsigned char LBT20162_SampleManage::IR_CTRL_FLAG;
unsigned char LBT20162_SampleManage::IR_SWITCH_FLAG;

unsigned short LBT20162_SampleManage::TimeTicks;
const unsigned short LBT20162_SampleManage::DelayTickCount_ADG = 8; //12;  //600us Open ADG--->200us Open ADG seens more good
const unsigned short LBT20162_SampleManage::DelayTickCount_AdcConv = 1;
const unsigned short LBT20162_SampleManage::DelayTickCount_AdcReadData = 1;
unsigned long LBT20162_SampleManage::RestADC;

void LBT20162_SampleManage::Init(void)
{
    m_chn = 0;
    m_value = 0;
    m_type = 0;

    //�ڲ���ת
    //m_I_chn = 0;    //I��Vͨ����0��ʼ
    //m_V_chn = 0;
    m_ADC_chn = 0;

    chn_history = 0;
    //״̬��
    ADC_Status = LBT20162_ADC_IDLE;
    for(unsigned char i=0; i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; i++)
    {
        I_ADG_Address[i] = i;
    }
    IR_CTRL_FLAG = 0;
    IR_SWITCH_FLAG = 0;
}


char LBT20162_SampleManage::SampleISR(unsigned long CH_Number,unsigned long type)
{
    //if((OS_ServeProcess::OS_Time.Us100 & 0x0F) == 0)     //ÿ��1.6�������¼���һ��ʱ�䣿 ���ʱ�����Ƿ�����أ�
    ChannelInfo_Manager::Set_TimeRecalculateSign();

    unsigned char temp;
    unsigned long data;

    switch(ADC_Status)
    {
    case LBT20162_ADC_IDLE:
        ADC_Status = LBT20162_ADC_OpenADG;
        TimeTicks = 0;
        m_ADC_chn = 0;
        Adc24bit_AD7175::ADC_CS= LBT20162_Base::PinGroup[LBT20162_Pin_AdcCS1];  //����
        ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
        Adc24bit_AD7175::ADC_CS= LBT20162_Base::PinGroup[LBT20162_Pin_AdcCS2];   //��ѹ
        ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
        Adc24bit_AD7175::ADC_START = LBT20162_Base::PinGroup[LBT20162_Pin_AdcSTART];//
        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
        break;
    case LBT20162_ADC_OpenADG:

        TimeTicks = 0;
        ADC_Status = LBT20162_ADC_OPENING;

        if(IR_CTRL_FLAG == 1)    //�����IR����
        {
            IR_SWITCH_FLAG = 1;
//            LBT20162_Base::ADG_Reset();
            m_ADC_chn = chn_DoIR;
            LBT20162_Base::ADC_ADG_open(I_ADG_Address[m_ADC_chn]); //add by hpq,20150730
//            ADC_Status = LBT20162_ADC_NoWork;    //ʡȥ�˿���ģ���ŵ�ʱ��
        }
        else
            LBT20162_Base::ADC_ADG_open(I_ADG_Address[m_ADC_chn]);
        break;
    case LBT20162_ADC_OPENING:
        TimeTicks++;
        if(TimeTicks >= DelayTickCount_ADG)
        {
            if(IR_CTRL_FLAG == 1 && IR_SWITCH_FLAG == 1)    // already switch to IR channel
                ADC_Status = LBT20162_ADC_NoWork;    //Stop other channel's sampling, and waiting for the trigger from IR timer
            else
                ADC_Status = LBT20162_ADC_TriggerI_V;
            TimeTicks = 0;
        }
        break;
    case LBT20162_ADC_TriggerI_V:
        SSPIFunc::ConfirmDevice(1, &LBT20162_Base::LBT20162_SSP1ADC);
        Adc24bit_AD7175::ADC_CS =  LBT20162_Base::PinGroup[LBT20162_Pin_AdcCS1];
        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
        Adc24bit_AD7175::Trigger(1);

        SSPIFunc::ConfirmDevice(1, &LBT20162_Base::LBT20162_SSP1ADC);
        Adc24bit_AD7175::ADC_CS = LBT20162_Base::PinGroup[LBT20162_Pin_AdcCS2];
        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
        Adc24bit_AD7175::Trigger(1);  // Prepare V trigger

        Adc24bit_AD7175::ADC_START = LBT20162_Base::PinGroup[LBT20162_Pin_AdcSTART];//
        ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_START);
        TIMERSFunc::DELAY_SYS(20, 0);
        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
        RestADC = 0;
        ADC_Status = LBT20162_ADC_ReadI;
        Adc24bit_AD7175::ADC_CS = LBT20162_Base::PinGroup[LBT20162_Pin_AdcCS2];
        ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
        break;
    case LBT20162_ADC_ReadI:
        Adc24bit_AD7175::ADC_CS =  LBT20162_Base::PinGroup[LBT20162_Pin_AdcCS1];
        Adc24bit_AD7175::ADC_DRDY = LBT20162_Base::PinGroup[LBT20162_Pin_AdcDRDY1];
        temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
        if(temp==0)
        {
            data= Adc24bit_AD7175::Read(1);
            if(data!=0)
            {
                ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc=data;
            }
//							  ADC_Status = LBT20162_ADC_TriggerV;
            ADC_Status = LBT20162_ADC_ReadV;
            ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
        }
        else
        {
            if(RestADC++>=0xFF)
            {
                Adc24bit_AD7175::Reset2(1);
                LBT20162_Base::ADC_Init();
                LBT20162_SampleManage::Init();
            }
        }
        break;
//         case LBT20162_ADC_TriggerV:
//
//             SSPIFunc::ConfirmDevice(1, &LBT20162_Base::LBT20162_SSP1ADC);
//             Adc24bit_AD7175::ADC_CS = LBT20162_Base::PinGroup[LBT20162_Pin_AdcCS2];
// 						ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
//             Adc24bit_AD7175::Trigger(1);
// 						ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_START);
// 	          TIMERSFunc::DELAY_SYS(20, 0);
// 	          ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
//             ADC_Status = LBT20162_ADC_ReadV;
//             break;
    case LBT20162_ADC_ReadV:
        Adc24bit_AD7175::ADC_CS = LBT20162_Base::PinGroup[LBT20162_Pin_AdcCS2];  // Set Voltage CS low to read V
        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
        Adc24bit_AD7175::ADC_DRDY = LBT20162_Base::PinGroup[LBT20162_Pin_AdcDRDY2];
        temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
        if(temp==0)
        {
            data= Adc24bit_AD7175::Read(1);
            if(data!=0)
            {
                ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc=data;
                Sample_Pro::NewCodeSampled[m_ADC_chn] = 1;
            }
            ADC_Status = LBT20162_ADC_CloseADG;
            Adc24bit_AD7175::ADC_CS = LBT20162_Base::PinGroup[LBT20162_Pin_AdcCS2];
            ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
        }
        else
        {
            if(RestADC++>=0xFF)
            {
                Adc24bit_AD7175::Reset2(1);
                LBT20162_Base::ADC_Init();
                LBT20162_SampleManage::Init();
            }
        }
        break;
    case LBT20162_ADC_CloseADG:
        if(IR_CTRL_FLAG == 0)    //���û��IR�������
        {
            m_ADC_chn++;
            if(m_ADC_chn>=ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
            {
                m_ADC_chn=0;
            }
            ADC_Status = LBT20162_ADC_OpenADG;
        }
        else
        {
            if(IR::ADG_Ready ==1 )  //If alreday at the IR channel, then wait for next trigger from IR timer
                ADC_Status = LBT20162_ADC_NoWork;
            else
            {
                m_ADC_chn = chn_DoIR;   //����ֱ���л�������ͨ��
                ADC_Status = LBT20162_ADC_OpenADG;
            }
        }
        break;
    case LBT20162_ADC_NoWork:
        IR::ADG_Ready =1;
        break;
    default:
        ADC_Status = LBT20162_ADC_IDLE;
        break;
    }

    return 0;    //error
}

////////////////////////////////////////////////////////////////////////////////////////
//������chn�����������ͬʱ��¼�����ر�����ͨ��
char LBT20162_SampleManage::IR_ADC(char chn)
{
    chn_history = m_ADC_chn;    //��¼��ʷ
    chn_DoIR = chn;
    IR_CTRL_FLAG = 1;
    IR::ADG_Ready =0;
    return 1;
}

void LBT20162_SampleManage::IR_Trigger_ADC(void)
{
    ADC_Status = LBT20162_ADC_TriggerI_V;

}
//�ָ�
char LBT20162_SampleManage::renew_IR_ADC(void)
{
    m_ADC_chn = chn_history;    //��ԭ
    IR_CTRL_FLAG = 0;    //clear
    ADC_Status = LBT20162_ADC_IDLE;
    IR_SWITCH_FLAG = 0;

    return 1;
}

///////////////////
void LBT20162_SampleManage::IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI)
{
// 	  SSPIFunc::ConfirmDevice(1, &LBT20162_Base::LBT20162_SSP1ADC);
//     Adc24bit_AD7175::ADC_CS =  LBT20162_Base::PinGroup[LBT20162_Pin_AdcCS1];
// 		ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
//     Adc24bit_AD7175::Trigger(1);

// 		ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_START);
// 	  TIMERSFunc::DELAY_SYS(20, 0);
// 	  ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
// 		TIMERSFunc::DELAY_SYS(300, 0);
// 	  Adc24bit_AD7175::ADC_DRDY = LBT20162_Base::PinGroup[LBT20162_Pin_AdcDRDY1];
// 		unsigned char  temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
// 		if(temp==0)
// 		{
// 	       ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc= Adc24bit_AD7175::Read(1);
//          Adc24bit_AD7175::ADC_CS =  LBT20162_Base::PinGroup[LBT20162_Pin_AdcCS1];
// 				ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
// 	   }
// 		 TIMERSFunc::DELAY_SYS(300, 0);
// 	   SSPIFunc::ConfirmDevice(1, &LBT20162_Base::LBT20162_SSP1ADC);
//      Adc24bit_AD7175::ADC_CS =  LBT20162_Base::PinGroup[LBT20162_Pin_AdcCS2];
// 		 ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
//      Adc24bit_AD7175::Trigger(1);

// 		 ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_START);
// 	   TIMERSFunc::DELAY_SYS(20, 0);
// 	   ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
// 		 TIMERSFunc::DELAY_SYS(300, 0);
// 		 Adc24bit_AD7175::ADC_DRDY = LBT20162_Base::PinGroup[LBT20162_Pin_AdcDRDY2];
// 		 temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
// 		 if(temp==0)
// 		 {
// 	        ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc= Adc24bit_AD7175::Read(1);
// 	        Adc24bit_AD7175::ADC_CS =  LBT20162_Base::PinGroup[LBT20162_Pin_AdcCS2];
// 					ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
// 			}

    *SampleI = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc;
    *SampleV = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc;     //��ѹ
}
