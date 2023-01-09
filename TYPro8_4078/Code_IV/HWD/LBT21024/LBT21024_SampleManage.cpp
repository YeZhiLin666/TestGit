
#include "../../Entry/includes.h"

//��LBT21024_Base�ڵĲ���ProcessADCת�ƽ���
//ͬʱ����SampleProcess���ļ�
//comment out chen 20130216
//��������һ������Ĳ���֧�ֵ�IR����
//ֱ��adcreset  ��ͨ�����ڸ�ͨ��
//��״̬���noopen
//���к��ٴλ�ԭ


//for �ӿں���
char LBT21024_SampleManage::m_chn;
unsigned long int LBT21024_SampleManage::m_value;
char LBT21024_SampleManage::m_type;
char LBT21024_SampleManage::m_ADC_chn;
unsigned char LBT21024_SampleManage::I_ADG_Address[MAXCHANNELNO];

char LBT21024_SampleManage::chn_history;
char LBT21024_SampleManage::chn_DoIR;
char LBT21024_SampleManage::ADC_Status;
unsigned char LBT21024_SampleManage::IR_CTRL_FLAG;
unsigned char LBT21024_SampleManage::IR_SWITCH_FLAG;

unsigned short LBT21024_SampleManage::TimeTicks;
const unsigned short LBT21024_SampleManage::DelayTickCount_ADG = 8; //12;  //600us Open ADG--->200us Open ADG seens more good
// const unsigned short LBT21024_SampleManage::DelayTickCount_AdcConv = 1;
// const unsigned short LBT21024_SampleManage::DelayTickCount_AdcReadData = 1;


unsigned char LBT21024_SampleManage::sample_count[2];
unsigned long LBT21024_SampleManage::sample_value[2][3];
unsigned long LBT21024_SampleManage::RestADC;

void LBT21024_SampleManage::Init(void)
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
    ADC_Status = LBT21024_ADC_IDLE;
    I_ADG_Address[0]=0;
    I_ADG_Address[1]=1;
    IR_CTRL_FLAG = 0;
    IR_SWITCH_FLAG = 0;

    for(unsigned char i=0; i<2; i++)
    {
        sample_count[i]=0;
        for(unsigned char j=0; j<3; j++)
            sample_value[i][j]=0;
    }
}


char LBT21024_SampleManage::SampleISR(unsigned long CH_Number,unsigned long type)
{
    //if((OS_ServeProcess::OS_Time.Us100 & 0x0F) == 0)     //ÿ��1.6�������¼���һ��ʱ�䣿 ���ʱ�����Ƿ�����أ�
//		ChannelInfo_Manager::Set_TimeRecalculateSign();

    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion<=BOARDTYPE_VER_C )
    {
        switch(ADC_Status)
        {
        case LBT21024_ADC_IDLE:
            ADC_Status = LBT21024_ADC_OpenADG;
            TimeTicks = 0;
            m_ADC_chn = 0;
            break;
        case LBT21024_ADC_OpenADG:
            LBT21024_Base::ADC_ADG_open(I_ADG_Address[m_ADC_chn]);
            TimeTicks = 0;
            ADC_Status = LBT21024_ADC_OPENING;

            if(IR_CTRL_FLAG == 1)    //�����IR����
            {
                LBT21024_Base::ADG_Reset();
                m_ADC_chn = chn_DoIR;
                LBT21024_Base::ADC_ADG_open(m_ADC_chn);
                ADC_Status = LBT21024_ADC_NoWork;    //ʡȥ�˿���ģ���ŵ�ʱ��
            }
            break;
        case LBT21024_ADC_OPENING:
            TimeTicks++;
            if(TimeTicks >= DelayTickCount_ADG)
            {
                ADC_Status = LBT21024_ADC_TriggerI;
                Adc24bit_CS5560::Convert();
                TimeTicks = 0;
            }
            break;
        case LBT21024_ADC_TriggerI:
            SSPIFunc::ConfirmDevice(1, &LBT21024_Base::LBT21024_SSP1ADC);
            Adc24bit_CS5560::Pin_CS = LBT21024_Base::PinGroup_ADC1[LBT21024_Pin_AdcCS1];
            Adc24bit_CS5560::Trigger();
            ADC_Status = LBT21024_ADC_ReadI;
            break;
        case LBT21024_ADC_ReadI:
        {
            SSPIFunc::ConfirmDevice(1, &LBT21024_Base::LBT21024_SSP1ADC);
            unsigned long adc_code_temp = Adc24bit_CS5560::ReadData();    //just for debug
            ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc  = adc_code_temp;
            ADC_Status = LBT21024_ADC_TriggerV;
            break;
        }
        case LBT21024_ADC_TriggerV:
            SSPIFunc::ConfirmDevice(1, &LBT21024_Base::LBT21024_SSP1ADC);
            Adc24bit_CS5560::Pin_CS = LBT21024_Base::PinGroup_ADC1[LBT21024_Pin_AdcCS2];
            Adc24bit_CS5560::Trigger();
            ADC_Status = LBT21024_ADC_ReadV;
            break;
        case LBT21024_ADC_ReadV:
            SSPIFunc::ConfirmDevice(1, &LBT21024_Base::LBT21024_SSP1ADC);
            ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc = Adc24bit_CS5560::ReadData();
            ADC_Status = LBT21024_ADC_CloseADG;
            Sample_Pro::NewCodeSampled[m_ADC_chn] = 1;
            break;
        case LBT21024_ADC_CloseADG:
            if(IR_CTRL_FLAG == 0)    //���û��IR�������
            {
                m_ADC_chn++;
                if(m_ADC_chn>=ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
                {
                    m_ADC_chn=0;
                }
                //�ر�
                ADC_Status = LBT21024_ADC_OpenADG;
            }
            else
                ADC_Status = LBT21024_ADC_OpenADG;
            break;
        case LBT21024_ADC_NoWork:
            break;
        default:
            ADC_Status = LBT21024_ADC_IDLE;
            break;
        }

    }
    else
    {
        unsigned char temp;
        unsigned long data;
        switch(ADC_Status)
        {
        case LBT21024_ADC_IDLE:
            ADC_Status = LBT21024_ADC_OpenADG;
            TimeTicks = 0;
            m_ADC_chn = 0;
            Adc24bit_AD7175::ADC_CS= LBT21024_Base::PinGroup_ADC2[LBT21024_Pin_Adc2CS1];  //����
            ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
            Adc24bit_AD7175::ADC_CS= LBT21024_Base::PinGroup_ADC2[LBT21024_Pin_Adc2CS2];   //��ѹ
            ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
            Adc24bit_AD7175::ADC_START = LBT21024_Base::PinGroup_ADC2[LBT21024_Pin_Adc2START];//
            ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
            break;
        case LBT21024_ADC_OpenADG:

            TimeTicks = 0;
            ADC_Status = LBT21024_ADC_OPENING;

            if(IR_CTRL_FLAG == 1)    //�����IR����
            {
                IR_SWITCH_FLAG = 1;  //need to switch to IR channel
//                LBT21024FRVB_Base::ADG_Reset();
                m_ADC_chn = chn_DoIR;
//                LBT21024FRVB_Base::ADC_ADG_open(m_ADC_chn);
                LBT21024_Base::ADC_ADG_open(I_ADG_Address[m_ADC_chn]);
//                ADC_Status = LBT21024FRVB_ADC_NoWork;    //ʡȥ�˿���ģ���ŵ�ʱ��
            }
            else
                LBT21024_Base::ADC_ADG_open(I_ADG_Address[m_ADC_chn]);
            break;
        case LBT21024_ADC_OPENING:
            TimeTicks++;
            if(TimeTicks >= DelayTickCount_ADG)
            {
                if(IR_CTRL_FLAG == 1 && IR_SWITCH_FLAG == 1)    // already switch to IR channel
                    ADC_Status = LBT21024_ADC_NoWork;    //Stop other channel's sampling, and waiting for the trigger from IR timer
                else
                    ADC_Status = LBT21024_ADC_TriggerI_V;
//                 Adc24bit_CS5560::Convert();
                TimeTicks = 0;
            }
            break;
        case LBT21024_ADC_TriggerI_V:        //Trigger Current and Voltage at the same time
            SSPIFunc::ConfirmDevice(1, &LBT21024_Base::LBT21024_SSP1ADC);
            Adc24bit_AD7175::ADC_CS =  LBT21024_Base::PinGroup_ADC2[LBT21024_Pin_Adc2CS1];
            ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
            Adc24bit_AD7175::Trigger(1);

            SSPIFunc::ConfirmDevice(1, &LBT21024_Base::LBT21024_SSP1ADC);
            Adc24bit_AD7175::ADC_CS = LBT21024_Base::PinGroup_ADC2[LBT21024_Pin_Adc2CS2];
            ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
            Adc24bit_AD7175::Trigger(1);  // Prepare V trigger

            Adc24bit_AD7175::ADC_START = LBT21024_Base::PinGroup_ADC2[LBT21024_Pin_Adc2START];
            ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_START);
            TIMERSFunc::DELAY_SYS(20, 0);
            ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
            ADC_Status = LBT21024_ADC_ReadI;
            Adc24bit_AD7175::ADC_CS = LBT21024_Base::PinGroup_ADC2[LBT21024_Pin_Adc2CS2];  // Set Voltage CS high to read I
            ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
            RestADC = 0;
            break;
        case LBT21024_ADC_ReadI:
            Adc24bit_AD7175::ADC_CS =  LBT21024_Base::PinGroup_ADC2[LBT21024_Pin_Adc2CS1];  //Set CS to Current ADC

            Adc24bit_AD7175::ADC_DRDY = LBT21024_Base::PinGroup_ADC2[LBT21024_Pin_Adc2DRDY1];
            temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
            if(temp==0)
            {
                data= Adc24bit_AD7175::Read(1);
                if(data!=0)
                {
                    ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc=data;
                }
                ADC_Status = LBT21024_ADC_ReadV;
                ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
            }
            else
            {
                if(RestADC++>=0xFF)
                {
                    LBT21024_Base::ADC_Init();
                    LBT21024_SampleManage::Init();
                }
            }
            break;

        case LBT21024_ADC_ReadV:
            Adc24bit_AD7175::ADC_CS = LBT21024_Base::PinGroup_ADC2[LBT21024_Pin_Adc2CS2];
            ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
            Adc24bit_AD7175::ADC_DRDY = LBT21024_Base::PinGroup_ADC2[LBT21024_Pin_Adc2DRDY2];
            temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
            if(temp==0)
            {
                data= Adc24bit_AD7175::Read(1);
                if(data!=0)
                {
                    ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc=data;
                    Sample_Pro::NewCodeSampled[m_ADC_chn] = 1;
                }
                ADC_Status = LBT21024_ADC_CloseADG;
                Adc24bit_AD7175::ADC_CS = LBT21024_Base::PinGroup_ADC2[LBT21024_Pin_Adc2CS2];
                ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
            }
            else
            {
                if(RestADC++>=0xFF)
                {
                    LBT21024_Base::ADC_Init();
                    LBT21024_SampleManage::Init();
                }
            }
            break;
        case LBT21024_ADC_CloseADG:
            if(IR_CTRL_FLAG == 0)    //���û��IR�������
            {
                m_ADC_chn++;
                if(m_ADC_chn>=ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
                {
                    m_ADC_chn=0;
                }
                //�ر�
                ADC_Status = LBT21024_ADC_OpenADG;
            }
            else if(IR::ADG_Ready ==1 ) //If alreday at the IR channel, then wait for next trigger from IR timer
                ADC_Status = ADC_Status = LBT21024_ADC_NoWork;
            else   //other wise, switch to the IR channel
            {
                m_ADC_chn = chn_DoIR;   //����ֱ���л�������ͨ��
                ADC_Status = LBT21024_ADC_OpenADG;
            }
            break;
        case LBT21024_ADC_NoWork:
            IR::ADG_Ready =1;
            break;
        default:
            ADC_Status = LBT21024_ADC_IDLE;
            break;
        }
    }
    return 0;    //error
}
//����ADC     ���ҵ�������
/*int LBT21024_SampleManage::ProcessADC(char adc_device_chn)
{
	return -1;    //error
}

char LBT21024_SampleManage::getCHN(char adc_device_chn)
{
	return m_chn;
}

//���ص�������
unsigned long int LBT21024_SampleManage::getValue(char adc_device_chn)
{
	return m_value;
}

char LBT21024_SampleManage::getType(char adc_device_chn)
{
	return m_type;
}*/

////////////////////////////////////////////////////////////////////////////////////////
//������chn�����������ͬʱ��¼�����ر�����ͨ��
char LBT21024_SampleManage::IR_ADC(char chn)
{
    chn_history = m_ADC_chn;    //��¼��ʷ
    chn_DoIR = chn;
    IR_CTRL_FLAG = 1;
    IR::ADG_Ready =0;
    return 1;
}
// void LBT21024_SampleManage::IR_Trigger_ADC(void)
// {
// 	ADC_Status = LBT21024_ADC_TriggerI_V;

// }

//�ָ�
char LBT21024_SampleManage::renew_IR_ADC(void)
{
    m_ADC_chn = chn_history;    //��ԭ
    IR_CTRL_FLAG = 0;    //clear

    ADC_Status = LBT21024_ADC_IDLE;
    IR_SWITCH_FLAG = 0;
    return 1;
}

///////////////////
void LBT21024_SampleManage::IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI)
{
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion<=BOARDTYPE_VER_C )
    {
        Adc24bit_CS5560::Convert();    //conv
        ARBUS_DELAY(300);    //�ȴ�

        *SampleI = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc;//LBT21024_ADC5DAC::ADC_rd(chn,LBT21024_ADC_TYPE_I);
        *SampleV = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc;//LBT21024_ADC5DAC::ADC_rd(chn,LBT21024_ADC_TYPE_V);     //��ѹ
    }
    else
    {
        *SampleI = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc;//LBT21024_ADC5DAC::ADC_rd(chn,LBT21024_ADC_TYPE_I);
        *SampleV = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc;//LBT21024_ADC5DAC::ADC_rd(chn,LBT21024_ADC_TYPE_V);     //��ѹ
    }
}
