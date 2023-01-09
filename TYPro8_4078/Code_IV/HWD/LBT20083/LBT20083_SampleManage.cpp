
#include "../../Entry/includes.h"

char LBT20083_SampleManage::m_chn;
unsigned long int LBT20083_SampleManage::m_value;
char LBT20083_SampleManage::m_type;
char LBT20083_SampleManage::m_ADC_chn;

char LBT20083_SampleManage::chn_history;
char LBT20083_SampleManage::chn_DoIR;
char LBT20083_SampleManage::ADC_Status;
unsigned char LBT20083_SampleManage::IR_CTRL_FLAG;
unsigned char LBT20083_SampleManage::IR_SWITCH_FLAG;	//新增

unsigned short LBT20083_SampleManage::TimeTicks;
const unsigned short LBT20083_SampleManage::DelayTickCount_ADG = 4; //12;  //600us Open ADG--->200us Open ADG seens more good
// const unsigned short LBT20083_SampleManage::DelayTickCount_AdcConv = 1;
// const unsigned short LBT20083_SampleManage::DelayTickCount_AdcReadData = 1;
unsigned char  LBT20083_SampleManage::I_ADG_Address[MAXCHANNELNO];
unsigned long LBT20083_SampleManage::RestADC;

void LBT20083_SampleManage::Init(void)
{
    m_chn = 0;
    m_value = 0;
    m_type = 0;

    //内部运转
    m_ADC_chn = 0;

    chn_history = 0;
    //状态机
    ADC_Status = LBT20083_ADC_IDLE;
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_B)
    {
        for(unsigned char i=0; i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; i++)
        {
            I_ADG_Address[i] = i;
        }
        IR_SWITCH_FLAG = 0;		//新增
    }
    IR_CTRL_FLAG = 0;
}

char LBT20083_SampleManage::SampleISR(unsigned long CH_Number,unsigned long type)
{
    //if((OS_ServeProcess::OS_Time.Us100 & 0x0F) == 0)
//		ChannelInfo_Manager::Set_TimeRecalculateSign();
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {
#if ADC_CS5560_ENABLE == 1
        switch(ADC_Status)
        {
        case LBT20083_ADC_IDLE:
            ADC_Status = LBT20083_ADC_OpenADG;
            TimeTicks = 0;
            m_ADC_chn = 0;
            break;
        case LBT20083_ADC_OpenADG:
            if(IR_CTRL_FLAG)
            {
                m_ADC_chn = chn_DoIR;
                ADC_Status = LBT20083_ADC_NoWork;
            }
            else
            {
                TimeTicks = 0;
                ADC_Status = LBT20083_ADC_OPENING;
            }
            LBT20083_Base::ADC_ADG_open(m_ADC_chn);
            break;
        case LBT20083_ADC_OPENING:
            TimeTicks++;
            if(TimeTicks >= DelayTickCount_ADG)
            {
                SSPIFunc::ClearFIFO(1);
                ADC_Status = LBT20083_ADC_TriggerI;
                Adc24bit_CS5560::Convert();
                TimeTicks = 0;
            }
            break;
        case LBT20083_ADC_TriggerI:
            Adc24bit_CS5560::Pin_CS = LBT20083_Base::PinGroup[LBT20083_Pin_AdcCS1];
            ControlPin::SetMeLow(Adc24bit_CS5560::Pin_CS);
            SSPIFunc::ConfirmDevice(1, &LBT20083_Base::LBT20083_SSP1ADC);
            Adc24bit_CS5560::Trigger();
            ADC_Status = LBT20083_ADC_ReadI;
            break;
        case LBT20083_ADC_ReadI:
            SSPIFunc::ConfirmDevice(1, &LBT20083_Base::LBT20083_SSP1ADC);
            unsigned long adc_code_temp = Adc24bit_CS5560::ReadData();
            ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc  = adc_code_temp;
            Adc24bit_CS5560::Pin_CS = LBT20083_Base::PinGroup[LBT20083_Pin_AdcCS2];
            ControlPin::SetMeLow(Adc24bit_CS5560::Pin_CS);
            SSPIFunc::ConfirmDevice(1, &LBT20083_Base::LBT20083_SSP1ADC);
            Adc24bit_CS5560::Trigger();
            ADC_Status = LBT20083_ADC_ReadV;
            TimeTicks++;
            break;
        case LBT20083_ADC_ReadV:
// #if DEBUG_TASK_STATISTIC == 1
//             if(m_ADC_chn == 0)
//                 OS_ServeProcess::Task_Count[COUNT_SAMPLE_ADC]++;
// #endif
            SSPIFunc::ConfirmDevice(1, &LBT20083_Base::LBT20083_SSP1ADC);
            ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc = Adc24bit_CS5560::ReadData();
            Sample_Pro::NewCodeSampled[m_ADC_chn] = 1;
            Sample_Pro::NewIRSampled[m_ADC_chn] = 1;
            if(IR_CTRL_FLAG == 0)
            {
                m_ADC_chn++;
                if(m_ADC_chn >=	ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
                    m_ADC_chn = 0;
                ADC_Status =LBT20083_ADC_OpenADG; // LBT20083_ADC_OPENING;
            }
            else
                ADC_Status = LBT20083_ADC_OpenADG;
            TimeTicks++;
            break;
        case LBT20083_ADC_NoWork:
            break;
        default:
            ADC_Status = LBT20083_ADC_IDLE;
            break;
        }
#endif
    }
    else
    {
        unsigned char temp;
        unsigned long data;

        switch(ADC_Status)
        {
        case LBT20083_ADC_IDLE:
            ADC_Status = LBT20083_ADC_OpenADG;
            TimeTicks = 0;
            m_ADC_chn = 0;
            Adc24bit_AD7175::ADC_CS= LBT20083_Base::PinGroup[LBT20083_Pin_AdcCS1];  //电流
            ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
            Adc24bit_AD7175::ADC_CS= LBT20083_Base::PinGroup[LBT20083_Pin_AdcCS2];   //电压
            ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
            Adc24bit_AD7175::ADC_START = LBT20083_Base::PinGroup[LBT20083_Pin_AdcConver];//
            ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
            break;
        case LBT20083_ADC_OpenADG:
            TimeTicks = 0;
            ADC_Status = LBT20083_ADC_OPENING;

            if(IR_CTRL_FLAG == 1)    //特殊的IR控制
            {
                IR_SWITCH_FLAG = 1;		//新增
                m_ADC_chn = chn_DoIR;
                LBT20083_Base::ADC_ADG_open(I_ADG_Address[m_ADC_chn]); //add by hpq,20150730
            }
            else
                LBT20083_Base::ADC_ADG_open(I_ADG_Address[m_ADC_chn]);

            break;
        case LBT20083_ADC_OPENING:
            TimeTicks++;
            if(TimeTicks >= DelayTickCount_ADG)
            {
                if(IR_CTRL_FLAG == 1 && IR_SWITCH_FLAG == 1)    // already switch to IR channel
                    ADC_Status = LBT20083_ADC_NoWork;    //Stop other channel's sampling, and waiting for the trigger from IR timer
                else
                    ADC_Status = LBT20083_ADC_TriggerI_V;
                TimeTicks = 0;
            }
            break;
        case LBT20083_ADC_TriggerI_V:

            SSPIFunc::ConfirmDevice(1, &LBT20083_Base::LBT20083_SSP1ADC);
            Adc24bit_AD7175::ADC_CS =  LBT20083_Base::PinGroup[LBT20083_Pin_AdcCS1];
            ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
            Adc24bit_AD7175::Trigger(1);

            SSPIFunc::ConfirmDevice(1, &LBT20083_Base::LBT20083_SSP1ADC);
            Adc24bit_AD7175::ADC_CS = LBT20083_Base::PinGroup[LBT20083_Pin_AdcCS2];
            ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
            Adc24bit_AD7175::Trigger(1);  // Prepare V trigger

            Adc24bit_AD7175::ADC_START = LBT20083_Base::PinGroup[LBT20083_Pin_AdcConver];
            ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_START);
            TIMERSFunc::DELAY_SYS(20,0);
            ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
            RestADC = 0;
            ADC_Status = LBT20083_ADC_ReadI;
            Adc24bit_AD7175::ADC_CS = LBT20083_Base::PinGroup[LBT20083_Pin_AdcCS2];
            ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);

            break;
        case LBT20083_ADC_ReadI:
            Adc24bit_AD7175::ADC_CS =  LBT20083_Base::PinGroup[LBT20083_Pin_AdcCS1];  //Set CS to Current ADC
            Adc24bit_AD7175::ADC_DRDY = LBT20083_Base::PinGroup[LBT20083_Pin_AdcBusy1];
            temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
            if(temp==0)
            {
                data= Adc24bit_AD7175::Read(1);
                if(data!=0)
                {
                    ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc=data;
                }
                ADC_Status = LBT20083_ADC_ReadV;
                ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
            }
            else
            {
                if(RestADC++>=0xFF)
                {
                    LBT20083_Base::ADC_Init();
                    LBT20083_SampleManage::Init();
                }
            }
            break;
        case LBT20083_ADC_ReadV:
            Adc24bit_AD7175::ADC_CS = LBT20083_Base::PinGroup[LBT20083_Pin_AdcCS2];  // Set Voltage CS low to read V
            ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
            Adc24bit_AD7175::ADC_DRDY = LBT20083_Base::PinGroup[LBT20083_Pin_AdcBusy2];
            temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
            if(temp==0)
            {
                data= Adc24bit_AD7175::Read(1);
                if(data!=0)
                {
                    ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc=data;
                    Sample_Pro::NewCodeSampled[m_ADC_chn] = 1;
                    Sample_Pro::NewIRSampled[m_ADC_chn] = 1;
                }
                ADC_Status = LBT20083_ADC_CloseADG;

                Adc24bit_AD7175::ADC_CS = LBT20083_Base::PinGroup[LBT20083_Pin_AdcCS2];

                ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
            }
            else
            {
                if(RestADC++>=0xFF)
                {
                    LBT20083_Base::ADC_Init();
                    LBT20083_SampleManage::Init();
                }
            }
            break;
        case LBT20083_ADC_CloseADG:
            if(IR_CTRL_FLAG == 0)    //如果没有IR特殊控制
            {
                m_ADC_chn++;
                if(m_ADC_chn>=ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
                {
                    m_ADC_chn=0;
                }
                ADC_Status = LBT20083_ADC_OpenADG;
            }
            else
            {
                if(IR::ADG_Ready ==1 )  //If alreday at the IR channel, then wait for next trigger from IR timer
                    ADC_Status = LBT20083_ADC_NoWork;
                else
                {
                    m_ADC_chn = chn_DoIR;
                    ADC_Status = LBT20083_ADC_OpenADG;
                }
            }
            break;
        case LBT20083_ADC_NoWork:
            IR::ADG_Ready =1;
            break;
        default:
            ADC_Status = LBT20083_ADC_IDLE;
            break;
        }

    }

    return 0;    //error
}

// int LBT20083_SampleManage::ProcessADC(char adc_device_chn)
// {
// 	return -1;    //error
// }
// char LBT20083_SampleManage::getCHN(char adc_device_chn)
// {
// 	return m_chn;
// }
// unsigned long int LBT20083_SampleManage::getValue(char adc_device_chn)
// {
// 	return m_value;
// }
// char LBT20083_SampleManage::getType(char adc_device_chn)
// {
// 	return m_type;
// }
char LBT20083_SampleManage::IR_ADC(char chn)
{
// 	chn = g_chn_order[chn];    //verD通道号颠倒
    chn_DoIR = chn;
    chn_history = m_ADC_chn;    //记录历史
    IR_CTRL_FLAG = 1;
    IR::ADG_Ready =0;
    return 1;
}
void LBT20083_SampleManage::IR_Trigger_ADC(void)
{
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_B)
    {
        ADC_Status = LBT20083_ADC_TriggerI_V;
    }

}
char LBT20083_SampleManage::renew_IR_ADC(void)
{
    m_ADC_chn = chn_history;    //还原
    IR_CTRL_FLAG = 0;    //clear
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {
        LBT20083_Base::ADG_Reset();    //关闭
    }
    ADC_Status = LBT20083_ADC_IDLE;
    IR_SWITCH_FLAG = 0;
    return 1;
}
void LBT20083_SampleManage::IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI)
{
    //	unsigned char old_chn = chn;
// 	chn = g_chn_order[chn];    //verD通道号颠倒  这个工作需要软件来完成
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {
#if ADC_CS5560_ENABLE == 1
        Adc24bit_CS5560::Convert();    //conv
        ARBUS_DELAY(300);    //等待

        SSPIFunc::ConfirmDevice(1, &LBT20083_Base::LBT20083_SSP1ADC);

        Adc24bit_CS5560::Pin_Busy = LBT20083_Base::PinGroup[LBT20083_Pin_AdcBusy1];
        Adc24bit_CS5560::Pin_CS = LBT20083_Base::PinGroup[LBT20083_Pin_AdcCS1];
        *SampleI = Adc24bit_CS5560::Read();

        Adc24bit_CS5560::Pin_Busy = LBT20083_Base::PinGroup[LBT20083_Pin_AdcBusy2];
        Adc24bit_CS5560::Pin_CS = LBT20083_Base::PinGroup[LBT20083_Pin_AdcCS2];
        *SampleV = Adc24bit_CS5560::Read();
#endif
    }
    else
    {
        *SampleI = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc;
        *SampleV = ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc;
    }
}
