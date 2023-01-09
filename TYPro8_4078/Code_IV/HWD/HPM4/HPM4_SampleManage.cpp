
#include "includes.h"



#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
//for 接口函数
char HPM4_SampleManage::m_chn;
unsigned long int HPM4_SampleManage::m_value;
char HPM4_SampleManage::m_type;


char HPM4_SampleManage::m_ADC_chn;
char HPM4_SampleManage::chn_DoIR;
char HPM4_SampleManage::chn_history;
char HPM4_SampleManage::ADC_Status;
unsigned char HPM4_SampleManage::IR_CTRL_FLAG;

unsigned short HPM4_SampleManage::TimeTicks;

unsigned long HPM4_SampleManage::SampleDelayCounter;
unsigned long HPM4_SampleManage::WaitForSampleCounter;
const unsigned short HPM4_SampleManage::DelayTickCount_ADG = 7;   //50*7=350us
#pragma arm section

void HPM4_SampleManage::Init(void)
{
    m_chn = 0;
    m_value = 0;
    m_type = 0;

    m_ADC_chn = 0;

    chn_history = 0;
    //状态机
    ADC_Status = HPM4_ADC_IDLE;
    SampleDelayCounter=0;
    IR_CTRL_FLAG = 0;
}

char HPM4_SampleManage::SampleISR(unsigned long CH_Number,unsigned long type)
{
    //if((OS_ServeProcess::OS_Time.Us100 & 0x0F) == 0)     //每隔1.6毫秒重新计算一次时间？ 这个时间间隔是否合理呢？
//		ChannelInfo_Manager::Set_TimeRecalculateSign();
// 	TimeTicks++;

//	unsigned char bOK;
    unsigned char temp;
    bool Triggerflag=0;
    unsigned long data;

    switch(ADC_Status)
    {

    case HPM4_ADC_IDLE:
        ADC_Status = HPM4_ADC_AssignIV;
        m_ADC_chn = 0;
        TimeTicks=0;
        Adc24bit_AD7175::ADC_CS= HPM4_Base::PinGroup[HPM4_Pin_CS_V];
        ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
        Adc24bit_AD7175::ADC_CS= HPM4_Base::PinGroup[HPM4_Pin_CS_C];
        ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
        Adc24bit_AD7175::ADC_START = HPM4_Base::PinGroup[HPM4_Pin_ADCSTART];
        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
        break;
    case HPM4_ADC_AssignIV:
        Adc24bit_AD7175::ADC_CS= HPM4_Base::PinGroup[HPM4_Pin_CS_C];
        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
        Adc24bit_AD7175::Trigger(1);
        Adc24bit_AD7175::ADC_CS= HPM4_Base::PinGroup[HPM4_Pin_CS_V];
        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
        Adc24bit_AD7175::Trigger(1);
        TimeTicks=0;
        ADC_Status = HPM4_ADC_TriggerIV;
        break;
    case HPM4_ADC_TriggerIV:
        Adc24bit_AD7175::ADC_START = HPM4_Base::PinGroup[HPM4_Pin_ADCSTART];
        ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_START);
        TIMERSFunc::DELAY_SYS(20, 0);
        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
        ADC_Status = HPM4_ADC_WAIT;
        break;
    case  HPM4_ADC_WAIT:
        if(!Triggerflag)
        {
            Adc24bit_AD7175::ADC_CS= HPM4_Base::PinGroup[HPM4_Pin_CS_V];
            ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
            Adc24bit_AD7175::ADC_CS= HPM4_Base::PinGroup[HPM4_Pin_CS_C];
            ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
            Triggerflag =1;
        }
        TimeTicks++;
        if(TimeTicks >= DelayTickCount_ADG)
        {
            ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
            ADC_Status = HPM4_ADC_ReadI;
            TimeTicks = 0;
        }
        break;
    case HPM4_ADC_ReadI:
        Adc24bit_AD7175::ADC_DRDY= HPM4_Base::PinGroup[HPM4_Pin_ADCDRDY];
        temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
        if(temp==0)
        {
            data= Adc24bit_AD7175::Read(1);
            if(data!=0)
            {
                ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc=data;
            }
            ADC_Status = HPM4_ADC_ReadV;
            ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
            Adc24bit_AD7175::ADC_CS= HPM4_Base::PinGroup[HPM4_Pin_CS_V];
            ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
        }
        break;
    case HPM4_ADC_ReadV:
        Adc24bit_AD7175::ADC_DRDY= HPM4_Base::PinGroup[HPM4_Pin_ADCDRDY];
        temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
        if(temp==0)
        {
            data= Adc24bit_AD7175::Read(1);
            if(data!=0)
            {
                ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc =data;
                Sample_Pro::NewCodeSampled[m_ADC_chn] = 1;
                Sample_Pro::NewIRSampled[m_ADC_chn] = 1;
            }
            m_ADC_chn++;
            if(m_ADC_chn>3)
            {
                m_ADC_chn =0;
                ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
                Adc24bit_AD7175::ADC_CS= HPM4_Base::PinGroup[HPM4_Pin_CS_V];
                ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
                ADC_Status = HPM4_ADC_AssignIV;
            }
            else
            {
                Adc24bit_AD7175::ADC_CS= HPM4_Base::PinGroup[HPM4_Pin_CS_C];
                ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
                Triggerflag =0;
                ADC_Status = HPM4_ADC_TriggerIV;
            }
        }
        break;
    case HPM4_ADC_NoWork:
        break;
    default:
        ADC_Status = HPM4_ADC_IDLE;
        break;
    }
    return 0;    //error
}


char HPM4_SampleManage::IR_ADC(char chn)
{
    chn_history = m_ADC_chn;    //记录历史
    chn_DoIR = chn;
    ADC_Status = HPM4_ADC_NoWork;
    IR_CTRL_FLAG = 1;

    return 1;
}

//恢复
char HPM4_SampleManage::renew_IR_ADC(void)
{
    //m_ADC_chn = chn_history;    //还原
    IR_CTRL_FLAG = 0;    //clear

    //HPM4_Base::ADG_Reset();    //关闭
    ADC_Status = HPM4_ADC_IDLE;
    return 1;
}

void  HPM4_SampleManage::IR_Read_ADC(char chn,unsigned long *SampleV,unsigned long *SampleI)
{
    unsigned char temp;
    bool Triggerflag=0;
    unsigned long data;
    unsigned long TimeOut_Count = 0;
    if(chn>3)
        return;

    m_ADC_chn = 0;

    Adc24bit_AD7175::ADC_CS= HPM4_Base::PinGroup[HPM4_Pin_CS_V];
    ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
    Adc24bit_AD7175::ADC_CS= HPM4_Base::PinGroup[HPM4_Pin_CS_C];
    ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
    Adc24bit_AD7175::ADC_START = HPM4_Base::PinGroup[HPM4_Pin_ADCSTART];
    ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);

    Adc24bit_AD7175::ADC_CS= HPM4_Base::PinGroup[HPM4_Pin_CS_C];
    ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
    Adc24bit_AD7175::Trigger(1);
    Adc24bit_AD7175::ADC_CS= HPM4_Base::PinGroup[HPM4_Pin_CS_V];
    ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
    Adc24bit_AD7175::Trigger(1);

    for(unsigned char i=0; i<=3; i++)
    {

        ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_START);
        TIMERSFunc::DELAY_SYS(20, 0);
        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_START);
        ADC_Status = HPM4_ADC_WAIT;

        if(!Triggerflag)
        {
            Adc24bit_AD7175::ADC_CS= HPM4_Base::PinGroup[HPM4_Pin_CS_V];
            ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
            Adc24bit_AD7175::ADC_CS= HPM4_Base::PinGroup[HPM4_Pin_CS_C];
            ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
            Triggerflag =1;
        }

        TIMERSFunc::DELAY_SYS(20, 0);//niu
        ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);

        Adc24bit_AD7175::ADC_DRDY= HPM4_Base::PinGroup[HPM4_Pin_ADCDRDY];
        temp =1;
        TimeOut_Count = 0;
        while(temp)
        {
            temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
            if(temp==0)
            {
                data= Adc24bit_AD7175::Read(1);
                if(data!=0)
                {
                    ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32CurrentAdc=data;
                }

                ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
                Adc24bit_AD7175::ADC_CS= HPM4_Base::PinGroup[HPM4_Pin_CS_V];
                ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
            }
            if(++TimeOut_Count > 24000)  // 1ms
            {
                temp = 0;
                HPM4_ADC5DAC::Adc_Init();
                Adc24bit_AD7175::ADC_CS = HPM4_Base::PinGroup[HPM4_Pin_CS_C];
            }
        }

        Adc24bit_AD7175::ADC_DRDY= HPM4_Base::PinGroup[HPM4_Pin_ADCDRDY];

        temp = 1;
        TimeOut_Count = 0;
        while(temp)
        {
            temp=ControlPin::ReadMe(Adc24bit_AD7175::ADC_DRDY);
            if(temp==0)
            {
                data= Adc24bit_AD7175::Read(1);
                if(data!=0)
                {
                    ChannelInfo_Manager::ChannelInfo[m_ADC_chn].m_u32VoltageAdc =data;
                }

                m_ADC_chn++;
                if(m_ADC_chn>3)
                {
                    m_ADC_chn =0;
                    ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);
                    Adc24bit_AD7175::ADC_CS= HPM4_Base::PinGroup[HPM4_Pin_CS_V];
                    ControlPin::SetMeHigh(Adc24bit_AD7175::ADC_CS);

                }
                else
                {
                    Adc24bit_AD7175::ADC_CS= HPM4_Base::PinGroup[HPM4_Pin_CS_C];
                    ControlPin::SetMeLow(Adc24bit_AD7175::ADC_CS);
                    Triggerflag =0;

                }
            }
            if(++TimeOut_Count > 24000)  // 1ms
            {
                temp = 0;
                HPM4_ADC5DAC::Adc_Init();
                Adc24bit_AD7175::ADC_CS = HPM4_Base::PinGroup[HPM4_Pin_CS_V];
            }
        }

    }

    *SampleI = ChannelInfo_Manager::ChannelInfo[chn].m_u32CurrentAdc;
    *SampleV = ChannelInfo_Manager::ChannelInfo[chn].m_u32VoltageAdc;     //电压
}

// MsTime  HPM4_SampleManage::TimeT(MsTime a,MsTime b)    // a<b
// {
// 	MsTime tempDiff;
// 	signed short temp;

// 	tempDiff.Second = b.Second - a.Second;
// 	temp = b.Us100 - a.Us100;
// 	if(temp < 0)
// 	{
// 		temp = temp + 10000;
// 		tempDiff.Second --;
// 	}
// 	tempDiff.Us100 = temp;
// 	return tempDiff;
// }
