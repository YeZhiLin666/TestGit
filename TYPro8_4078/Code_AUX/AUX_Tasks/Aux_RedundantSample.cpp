#include "../Entry/includes.h"






const St_Port RedundantSample_Task::ADC_ADD1    = {1,        22,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port RedundantSample_Task::ADC_ADD2    = {1,        23,         eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port RedundantSample_Task::ADC_ADD3    = {1,        24,        eCPTYPE_MCUGPIO,   GPIO_O,                0};

UInt16 	RedundantSample_Task::Priority = PRIORITY8;
UInt16	RedundantSample_Task::TaskID = REDUNDANTSAMPLE_TASK;


#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif

St_ADCGainOffset 	RedundantSample_Task::st_ADCPara[REDUNDANT_TYPE];
St_RedundantData 	RedundantSample_Task::st_SampleData[REDUNDANT_TYPE];
MsTime						RedundantSample_Task::m_SampleTick;
unsigned char 		RedundantSample_Task::m_MaxChnCount;
MsTime RedundantSample_Task::LastSampleTime;
st_M4ADC_Code_Que RedundantSample_Task::m_M4_adc_history[MAX_SAMPLE_CHN]; //M4 ADC 历史码值

#pragma arm section //结束外存变量定义


void RedundantSample_Task::Init(void)
{
    HardWare_init();
    for (unsigned char i = 0; i<MAX_SAMPLE_CHN; i++)
    {
        memset(&m_M4_adc_history[i],0,sizeof(st_M4ADC_Code_Que));
    }
}

void RedundantSample_Task::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_REDUNDANTSAMPLE] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_REDUNDANTSAMPLE);
#endif

    unsigned char uc_Type = 0;

    SampleOneChn(uc_Type);

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_REDUNDANTSAMPLE] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time(COUNT_REDUNDANTSAMPLE);
#endif
}
void RedundantSample_Task::HardWare_init(void)
{
    m_MaxChnCount = 0;
    st_ADCPara[en_VolType].m_Gain = _Gain;
    st_ADCPara[en_VolType].m_Offset = _Offset;
    st_SampleData[en_VolType].m_bValid = 0;
    ControlPin::SetIObyCP(ADC_ADD1);
    ControlPin::SetIObyCP(ADC_ADD2);
    ControlPin::SetIObyCP(ADC_ADD3);
    ControlPin::SetMeLow(ADC_ADD1);
    ControlPin::SetMeLow(ADC_ADD2);
    ControlPin::SetMeLow(ADC_ADD3);
    ADCFunc::Init();

}

void RedundantSample_Task::SampleOneChn(unsigned char uc_Type)
{

    float fOrigVal = 0.0f;
    static unsigned char chnIdx = 0;
    static unsigned char Status = 0;
    MsTime tempT;


    switch(Status)
    {
    case eOpenADG:
        ScanIVchn(chnIdx);				//ADG
        CommonFunc::GetTimeTicksUpdate(&LastSampleTime);
        //  LPC4078ADC::Trigger(chnIdx);
        Status = eWaitStable;
        break;
    case eWaitStable:
        tempT = CommonFunc::TimeDiff(LastSampleTime);
        if( tempT.Us100>200)
            Status = eSample;
        break;
    case eSample:
        //st_SampleData[uc_Type].m_DataCode[chnIdx] = ADCRead(0);
        //fOrigVal =(ADCRead(0)/1000);//((float)st_SampleData[uc_Type].m_DataCode[chnIdx])/1000;
        //st_SampleData[uc_Type].m_fValue[chnIdx]=ADCRead(0);
        enQueue( chnIdx, (ADCRead(0)/1000));
        fOrigVal = avgQueue(chnIdx, 10);
        st_SampleData[uc_Type].m_fValue[chnIdx]=st_ADCPara[uc_Type].m_Gain*fOrigVal+st_ADCPara[uc_Type].m_Offset;
        st_SampleData[uc_Type].m_bRefreshed[chnIdx] = 1;
        Status = eCloseADG;
        break;
    case eCloseADG:   //close and transfer to next chn
        ADCFunc::StopConvert();
        Status = eOpenADG;
        chnIdx++;
        if(chnIdx>=MAX_SAMPLE_CHN)
            chnIdx = 0;
        break;
    default:
        break;
    }
}

bool RedundantSample_Task::SetGainOffset(St_Uart_SetGainOffset* AdcInfo)
{
    unsigned char Type = AdcInfo->m_CaliType;
    if(Type>=eType_MAX)
        return false;
    st_ADCPara[Type].m_Gain = AdcInfo->m_fGain;
    st_ADCPara[Type].m_Offset = AdcInfo->m_fOffset;
    st_ADCPara[Type].m_Type = Type;
    st_SampleData[Type].m_bValid = 1;
    return true;
}
bool RedundantSample_Task::SetChannelMap(St_Uart_SetChannelMap *ChannelMapInfo)
{
    return true;
}
void RedundantSample_Task::ScanIVchn(unsigned char uc_ChannelNo)
{
    switch(uc_ChannelNo)
    {
    case 0:
        ControlPin::SetMeLow(ADC_ADD1);
        ControlPin::SetMeLow(ADC_ADD2);
        ControlPin::SetMeLow(ADC_ADD3);
        break;
    case 1:
        ControlPin::SetMeHigh(ADC_ADD1);
        ControlPin::SetMeLow(ADC_ADD2);
        ControlPin::SetMeLow(ADC_ADD3);
        break;
    case 2:
        ControlPin::SetMeLow(ADC_ADD1);
        ControlPin::SetMeHigh(ADC_ADD2);
        ControlPin::SetMeLow(ADC_ADD3);
        break;
    case 3:
        ControlPin::SetMeHigh(ADC_ADD1);
        ControlPin::SetMeHigh(ADC_ADD2);
        ControlPin::SetMeLow(ADC_ADD3);
        break;
    case 4:
        ControlPin::SetMeLow(ADC_ADD1);
        ControlPin::SetMeLow(ADC_ADD2);
        ControlPin::SetMeHigh(ADC_ADD3);
        break;
    case 5:
        ControlPin::SetMeHigh(ADC_ADD1);
        ControlPin::SetMeLow(ADC_ADD2);
        ControlPin::SetMeHigh(ADC_ADD3);
        break;
    case 6:
        ControlPin::SetMeLow(ADC_ADD1);
        ControlPin::SetMeHigh(ADC_ADD2);
        ControlPin::SetMeHigh(ADC_ADD3);
        break;
    case 7:
        ControlPin::SetMeHigh(ADC_ADD1);
        ControlPin::SetMeHigh(ADC_ADD2);
        ControlPin::SetMeHigh(ADC_ADD3);
        break;
    default:
        ControlPin::SetMeLow(ADC_ADD1);
        ControlPin::SetMeLow(ADC_ADD2);
        ControlPin::SetMeLow(ADC_ADD3);
        break;
    }

}


float RedundantSample_Task::ADCRead(unsigned char uc_ADCchannel)
{
    float ul_ADCCode;
    ul_ADCCode=ADCFunc::Read(uc_ADCchannel);
    return ul_ADCCode;
}

void RedundantSample_Task::enQueue( unsigned char chn, float code)
{
    m_M4_adc_history[chn].ul_data[m_M4_adc_history[chn].uc_start] = code;

    m_M4_adc_history[chn].uc_start ++;
    m_M4_adc_history[chn].uc_length ++;
    if(m_M4_adc_history[chn].uc_start >= MAX_ADC_CODE_QUE)
    {
        m_M4_adc_history[chn].uc_start = 0;
    }
    if(m_M4_adc_history[chn].uc_length >=MAX_ADC_CODE_QUE)
    {
        m_M4_adc_history[chn].uc_length = MAX_ADC_CODE_QUE;
    }
}

float RedundantSample_Task::avgQueue(unsigned char chn, unsigned char filter_depth)

{
    float sum = 0;
    unsigned char counter = 0;
    unsigned char pointer = 0;
    if (filter_depth <=1) //不需滤波，直接返回当前码值
        return m_M4_adc_history[chn].ul_data[m_M4_adc_history[chn].uc_start];
    else
    {
        if (m_M4_adc_history[chn].uc_length == 0 )
            return 0;
        if (filter_depth >m_M4_adc_history[chn].uc_length)
            counter = m_M4_adc_history[chn].uc_length;
        else
            counter = filter_depth;

        for (char i = 0; i<counter; i++)
        {   pointer = (m_M4_adc_history[chn].uc_start - i) ;
            if (pointer >MAX_ADC_CODE_QUE)	// pointer是unsigned char型，当i>uc_start时，pointer会变成非常大的数，因此此处判断改为pointer >MAX_ADC_CODE_QUE, Yifei
                pointer +=MAX_ADC_CODE_QUE;

            sum += m_M4_adc_history[chn].ul_data[pointer]; //累加最近的几个数据
        }

        return (float)sum/counter;
    }
}