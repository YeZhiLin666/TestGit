#include "../Entry/includes.h"

//#define  PULSE_DAC_STAGE_TIME_US    60
#define  PULSE_ADC_STAGE_TIME_US    10
#define  PULSE_ADC_CURR_ERROR_HEAD  0x800000/2
#define  PULSE_ADC_CURR_ERROR_TAIL  0x800000/5
#define  PULSE_NEGLIGIBLE_TIME_MS   5.0f        //用于pulse测试resume判断，可忽略时间  5ms
// bool                            bHigh;
// unsigned long PulseCurrent[3];     //记录3次采样的电流码值
// unsigned long PulseVoltage[3];     //记录3次采样的电压码值
unsigned long Pulse_ADC_Curr,Pulse_ADC_Volt,DeltaCurr;
unsigned char                   Pulse::m_StateMachine;       //下位机台阶流转状态机
St_PulseStageInfo_Running       *Pulse::m_StagePtr;          //台阶运行指针，指向正在运行的台阶定义

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
St_PulseInfo_Running            Pulse::m_Info;               //运行态的PULSE定义，   从上位机下载的原始PULSE定义PULSE_DEF翻译而来
PULSE_RAW_DATA                  Pulse::raw_data[2][2][MAX_PULSE_STAGE_NUM]; //[缓冲区1/2][头/尾][台阶数] [2][2][];
PULSE_STAGE_INFO                Pulse::stage_info[2][MAX_PULSE_STAGE_NUM];  //对应于两个缓冲区的，有关脉冲stage的数据索引

#pragma arm section //结束外存变量定义

PULSE_BUF_INFO                  Pulse::buf_info[2];
unsigned char                   Pulse::active_index;            //pulse两个缓冲区，正在填充数据的 序号
//unsigned char                   Pulse::stage_count;
bool                            Pulse::Pulse_is_Running ;     //Add by DKQ 08.20.2014
float                           Pulse::fMaxVoltage;           //脉冲采样电压最大值，用于狄瑞的V_LIMIT_CHECK任务使用
float                           Pulse::fMinVoltage;           //SafetyCheck_Pulse任务也会用到
float                           Pulse::fMaxVoltage_Curr;      //2014/12/26,per ATL's request
float                           Pulse::fMinVoltage_Curr;      //2014/12/26,per ATL's request
float                           Pulse::fMaxCurrent;
float                           Pulse::fMinCurrent;
float                           Pulse::fMaxPower;
float                           Pulse::fMinPower;
PULSE_RESUME_INFO               Pulse::m_ResumeInfo;
PULSE_RISE_TIME                 Pulse::m_RiseTime;
PulseHistorySample              Pulse::m_HistorySample[3];
PulseHistorySample              *Pulse::m_pSampleLast1;
PulseHistorySample              *Pulse::m_pSampleLast2;
PulseHistorySample              *Pulse::m_pSampleCurrent;
unsigned long                   Pulse::m_SamleErrorCounter;

void        Pulse::SetRiseTime(float fRiseTimeInMs)
{
    if(fRiseTimeInMs < 0.01f)          //less than 10us,  use 10us as rise time
        m_RiseTime.m_fMS = 0.01f;
    else if(fRiseTimeInMs > 100.0f)    //more than 100ms, use 100ms as rise time
        m_RiseTime.m_fMS = 100.0f;
    else
        m_RiseTime.m_fMS = fRiseTimeInMs;
    m_RiseTime.m_fUS = m_RiseTime.m_fMS * 1000.0f;
    m_RiseTime.m_u32TimerSet = m_RiseTime.m_fUS * 60 - 1;
}
void        Pulse::MVinit(void)
{
    memset(&raw_data,0,sizeof(raw_data));
    memset(&stage_info,0,sizeof(stage_info));
    memset(&buf_info,0,sizeof(buf_info));
    memset(&m_Info,0,sizeof(m_Info));

    Pulse_is_Running = false;  // Add by DKQ 08.20.2014
}
void        Pulse::PulseInit(unsigned char ChannelIdx)
{   //这个应该在LoadPulseData ()里完成
    m_pSampleCurrent = &m_HistorySample[0];
    m_pSampleLast1 = &m_HistorySample[1];
    m_pSampleLast2 = &m_HistorySample[2];
    m_pSampleCurrent->bValid = false;
    m_pSampleLast1->bValid = false;
    m_pSampleLast2->bValid = false;
    m_SamleErrorCounter = 0;
    SetRiseTime(board_config::DacInfo.m_fPulseRiseTimeInMS);
    memset(&raw_data,0,sizeof(raw_data));
    memset(&stage_info,0,sizeof(stage_info));
    memset(&buf_info,0,sizeof(buf_info));
    memset(&m_Info,0,sizeof(m_Info));
    active_index = 0;

    St_PulseData *PulsePtr = &ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx].m_PulseInfo.m_Data;  //changed by DKQ 08.20.2014,需要培青确认，只要修改了Channelstep

    m_Info.m_SampType = ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[3];
    //根据St_StepData对Pulse的运行时变量进行初始化动作;
    if(PulsePtr->m_uStageNum > MAX_PULSE_STAGE_NUM)
        PulsePtr->m_uStageNum = MAX_PULSE_STAGE_NUM;

    m_Info.m_StageCount = PulsePtr->m_uStageNum;
    // m_Info.m_StageIdx   = 0;
    // stage_count         = m_Info.m_StageCount;

    unsigned short StageIdx = 0;
    unsigned long  TimeInUS = 0;
    unsigned char  CurrRange = ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCurrentRange;
    float          CycleTime = 0;  //脉冲的周期时间

    for(StageIdx = 0; StageIdx < m_Info.m_StageCount; StageIdx++)
    {
        unsigned long PredictAdcCode = 0;
        St_PulseStageInfo_Running * p_Stage = &m_Info.m_Stages[StageIdx];
        p_Stage->m_DacCode = ChannelInfo_Manager::CalibrateCurrentDAC(ChannelIdx,CurrRange,PulsePtr->m_StageAmplitude[StageIdx]);

        PredictAdcCode = (PulsePtr->m_StageAmplitude[StageIdx] - ChannelInfo_Manager::BoardCFG.m_Channel[0].m_IRange[CurrRange].m_Adc.m_Offset)
                         / ChannelInfo_Manager::BoardCFG.m_Channel[0].m_IRange[CurrRange].m_Adc.m_Gain;
        p_Stage->m_PredictAdcCode_Max_Head = PredictAdcCode + PULSE_ADC_CURR_ERROR_HEAD;
        if(p_Stage->m_PredictAdcCode_Max_Head > 0xFFFFFF)
            p_Stage->m_PredictAdcCode_Max_Head = 0xFFFFFF;
        if(PredictAdcCode > PULSE_ADC_CURR_ERROR_HEAD)
            p_Stage->m_PredictAdcCode_Min_Head = PredictAdcCode - PULSE_ADC_CURR_ERROR_HEAD;
        else
            p_Stage->m_PredictAdcCode_Min_Head = 0;
        p_Stage->m_PredictAdcCode_Max_Tail = PredictAdcCode + PULSE_ADC_CURR_ERROR_TAIL;
        if(p_Stage->m_PredictAdcCode_Max_Tail > 0xFFFFFF)
            p_Stage->m_PredictAdcCode_Max_Tail = 0xFFFFFF;
        if(PredictAdcCode > PULSE_ADC_CURR_ERROR_TAIL)
            p_Stage->m_PredictAdcCode_Min_Tail = PredictAdcCode - PULSE_ADC_CURR_ERROR_TAIL;
        else
            p_Stage->m_PredictAdcCode_Min_Tail = 0;
        p_Stage->m_bFast = (PulsePtr->m_StageWidth[StageIdx] < LOGGEST_FAST_STAGE);        //是否fast stage可能不会从schedule里传下来，得根据台阶宽度在这里确定
        if(p_Stage->m_bFast)
            p_Stage->m_Non_V_Limit_Priority = 8;
        else
            p_Stage->m_Non_V_Limit_Priority = 4;
        p_Stage->m_bVerySlow = (PulsePtr->m_StageWidth[StageIdx] >= 60000);  //如果时间大于60秒，则认为是超慢stage，需要处理timer预分频
        //以上两个时间的转换均基于timer预分频为0时的计算，即tick分辨率为0.017us
        float fTimeVar = PulsePtr->m_StageWidth[StageIdx] - (m_RiseTime.m_fUS + PULSE_ADC_STAGE_TIME_US * 3)*0.001f;
        if(fTimeVar < 0.01f)
            fTimeVar = 0.01f;
        if(p_Stage->m_bVerySlow)
            p_Stage->m_TimerSet = TIMERSFunc::Timer3_GetTick_Pre(fTimeVar);//(stage宽度 - T1 - T3)时间转换为有预分频情况下对应的tick数;
        else
            p_Stage->m_TimerSet = TIMERSFunc::Timer3_GetTick_NoPre(fTimeVar);//(stage宽度 - T1 - T3)时间转换为没有预分频情况下对应的tick数;

// 		p_Stage->m_TimerSet2 = PULSE_ADC_STAGE_TIME_US * 60  -1 + p_Stage->m_TimerSet;
// 		p_Stage->m_TimerSet3 = PULSE_ADC_STAGE_TIME_US * 120 -1 + p_Stage->m_TimerSet;

        stage_info[SAVE_2_HEAD][StageIdx].m_TimeInUS = TimeInUS + 5;  //1605051,hepeiqing,for ATL's request
        TimeInUS += PulsePtr->m_StageWidth[StageIdx] * 1000 ;   //毫秒 转换为 微秒
        stage_info[SAVE_2_TAIL][StageIdx].m_TimeInUS = TimeInUS - 0;  //1605051,hepeiqing,for ATL's request
        PULSE_MV_Task::StgInfo[StageIdx].TimeInHour  = PulsePtr->m_StageWidth[StageIdx] / 1000 / 3600;  //毫秒 转换为 小时
        PULSE_MV_Task::StgInfo[StageIdx].TimeInUs  = PulsePtr->m_StageWidth[StageIdx] * 1000;  //ms to us
        //PULSE_MV_Task::StgInfo[StageIdx].CalculatedCycle = 0;
        CycleTime += PulsePtr->m_StageWidth[StageIdx];
    }
    float StepTime = ChannelInfo_Manager::Read_SpecifiedMetavariable2(0,MetaCode_PV_Steptime);
    if(StepTime < 1)
        StepTime = 0.0f;
    StepTime *= 1000;
    m_Info.m_CycleCounter = StepTime / CycleTime;
    //  unsigned char resumeStgIdx = 0;
    m_ResumeInfo.m_StgIdx = 0;
    m_ResumeInfo.m_bResume = 0;
    m_ResumeInfo.m_fElapsedTime = 0.0f;

    if(StepTime > PULSE_NEGLIGIBLE_TIME_MS)  //如果step time大于等于100ms，证明这是一个pulse resume，需要考虑pulse的起始stage
    {
        m_ResumeInfo.m_fElapsedTime = StepTime - m_Info.m_CycleCounter * CycleTime;
        if(m_ResumeInfo.m_fElapsedTime > PULSE_NEGLIGIBLE_TIME_MS)
        {
            float fStgTime = 0;
            for(StageIdx = 0; StageIdx < m_Info.m_StageCount; StageIdx++)
            {
                fStgTime = PulsePtr->m_StageWidth[StageIdx];

                if(m_ResumeInfo.m_fElapsedTime >= fStgTime)
                    m_ResumeInfo.m_fElapsedTime = m_ResumeInfo.m_fElapsedTime - fStgTime;
                else
                    break;
            }

            m_ResumeInfo.m_bResume = 1;
            if(m_ResumeInfo.m_fElapsedTime <= PULSE_NEGLIGIBLE_TIME_MS)      //if elapsedTime is negligible, restart this stage;
            {
                m_ResumeInfo.m_StgIdx = StageIdx;
                //m_ResumeInfo.m_bResume = 1;
                m_ResumeInfo.m_fElapsedTime = 0;
            }
            else if((fStgTime - m_ResumeInfo.m_fElapsedTime) <= PULSE_NEGLIGIBLE_TIME_MS)
            {
                m_ResumeInfo.m_StgIdx = StageIdx + 1;
                //m_ResumeInfo.m_bResume = 1;
                m_ResumeInfo.m_fElapsedTime = 0;
            }
            else
            {
                //m_ResumeInfo.m_bResume = 1;
                m_ResumeInfo.m_StgIdx = StageIdx;
            }

            if(m_ResumeInfo.m_StgIdx >= m_Info.m_StageCount)
            {
                m_ResumeInfo.m_StgIdx = 0;
                m_ResumeInfo.m_bResume = 0;
                m_ResumeInfo.m_fElapsedTime = 0.0f;
            }
        }
        else
            m_ResumeInfo.m_fElapsedTime = 0.0f;
    }

    //st_GetMetaValue * p_MV2 = &ChannelInfo_Manager::MetaVariable2[0][0];
    ChannelInfo_Manager::MetaVariable2[0][MetaCode_DV_PulseCycle].m_Value.m_uInt32 = 0;    //Resume的时候也初始化的话，前面运行的循环数就不会接续了。dirui 2014/10/29
    ChannelInfo_Manager::MetaVariable2[0][MetaCode_DV_PulseCycle].m_fValue         = 0.0f;
    ChannelInfo_Manager::MetaVariable2[0][MetaCode_PV_PulseCycle].m_Value.m_uInt32 = m_Info.m_CycleCounter; //若Resume到其他脉冲步怎么办？  dirui  2014/10/29
    ChannelInfo_Manager::MetaVariable2[0][MetaCode_PV_PulseCycle].m_fValue         = m_Info.m_CycleCounter;
    for(StageIdx = 0; StageIdx < m_Info.m_StageCount; StageIdx++)
    {
        PULSE_MV_Task::StgInfo[StageIdx].CalculatedCycle = m_Info.m_CycleCounter;
        m_Info.m_Stages[StageIdx].m_DacCode_NextStg = m_Info.m_Stages[(StageIdx+1)%m_Info.m_StageCount].m_DacCode;
    }

    m_StateMachine = PulseSM_Dac;          //初始化pulse中断处理状态机
    m_StagePtr     = &m_Info.m_Stages[0];  //stage指针指向第一个台阶

    buf_info[active_index].m_CycleIdx = m_Info.m_CycleCounter;
    buf_info[active_index].m_CycleStartTime = CommonFunc::GetSystemTime();//OS_Time;
    buf_info[!active_index].m_CycleIdx = m_Info.m_CycleCounter;
    buf_info[!active_index].m_CycleStartTime = CommonFunc::GetSystemTime();
    ChannelInfo_Manager::PulseLimitArriveData[ChannelIdx].bCriticalPoint = 0; //zyx 190321 pluse can't log, bCriticalPoint can't be clear
}
void Pulse::Start(unsigned char ChannelIdx)
{
    PulseInit(ChannelIdx);
    ChannelInfo_Manager::PulseLimitArriveData[ChannelIdx].bPulseStepEnded = 0;

    Pulse::buf_info[0].m_bDataLogged = 1;
    Pulse::buf_info[0].m_bFull = 0;
    Pulse::buf_info[1].m_bDataLogged = 1;
    Pulse::buf_info[1].m_bFull = 0;

    if(m_ResumeInfo.m_bResume == 0)    //直接启动pulse，不是resume
    {
        DAC16bit_AD5754::Dac_SendData(0,m_Info.m_Stages[0].m_DacCode);
        unsigned long PrePulseTick = TIMERSFunc::Timer3_GetTick_NoPre(0.05f);  //进入PULSE中断的 前置等待时间==>50us
        TIMERSFunc::Init_Timer(PrePulseTick,3);
        TIMERSFunc::Enable_Timer(3);
    }
    else
    {   //resume一个pulse
        m_StagePtr = &m_Info.m_Stages[m_ResumeInfo.m_StgIdx];
        if(m_ResumeInfo.m_fElapsedTime <= 0)
        {   //单个stage重新开始
            m_Info.m_StageIdx = m_ResumeInfo.m_StgIdx;
            DAC16bit_AD5754::Dac_SendData(0,m_Info.m_Stages[m_Info.m_StageIdx].m_DacCode);
            unsigned long PrePulseTick = TIMERSFunc::Timer3_GetTick_NoPre(0.05f);  //进入PULSE中断的 前置等待时间==>50us
            TIMERSFunc::Init_Timer(PrePulseTick,3);
            TIMERSFunc::Enable_Timer(3);
        }
        else
        {   //stage被截断开始
            m_Info.m_StageIdx = m_ResumeInfo.m_StgIdx;
            DAC16bit_AD5754::Dac_Out(0,m_Info.m_Stages[m_Info.m_StageIdx].m_DacCode);
            unsigned long PrePulseTick = TIMERSFunc::Timer3_GetTick_NoPre(0.05f);  //进入PULSE中断的 前置等待时间==>50us
            TIMERSFunc::Init_Timer(PrePulseTick,3);
            TIMERSFunc::Enable_Timer(3);
            if(m_StagePtr->m_bVerySlow)
                TIMERSFunc::Timer3_SetTick_Pre(m_StagePtr->m_TimerSet - TIMERSFunc::Timer3_GetTick_Pre(m_ResumeInfo.m_fElapsedTime));     //特慢台阶，使用预分频设定TIMER3
            else
                LPC_TIM3->MR0 = m_StagePtr->m_TimerSet - TIMERSFunc::Timer3_GetTick_NoPre(m_ResumeInfo.m_fElapsedTime);
            m_StateMachine = PulseSM_Sample2_Conv;
        }
    }

    Step_Manager_task::PULSE_bNeedCheck[ChannelIdx]=1;
    buf_info[active_index].m_CycleStartTime = CommonFunc::GetSystemTime();
    m_Info.m_bPulseSample = 0;
    fMaxVoltage = fMinVoltage = ChannelInfo_Manager::MetaVariable2[0][MetaCode_PV_Voltage].m_fValue;
    fMaxCurrent = fMinCurrent = ChannelInfo_Manager::MetaVariable2[0][MetaCode_PV_Current].m_fValue;
    fMaxVoltage_Curr = fMinVoltage_Curr = 0.0f;
    fMaxPower = fMinPower = fMaxVoltage * fMaxCurrent;
}
void        Pulse::End(unsigned char ChannelIdx)
{
    if(Arbin_SC->PCONP.BitReg.bPCTIM3 == 0)
        Arbin_SC->PCONP.BitReg.bPCTIM3 = 1;    //the code below will break to HardwareFault if timer3 in poweroff state
    LPC_GPIO0->SET |= (1<<16);//in case of DAC CS error.
    unsigned long PrePulseTick = TIMERSFunc::Timer3_GetTick_NoPre(0.01f);  //出PULSE中断的 后置等待时间==>10us
    TIMERSFunc::Timer3_SetTick_NoPre(PrePulseTick);
    TIMERSFunc::Disable_Timer(3);
    //DAC16bit_AD5754::Dac_Out(0,0x800000);

    Step_Manager_task::PULSE_bNeedCheck[ChannelIdx]=0;
    Pulse::m_Info.m_bPulseSample = 0;
    //  Pulse::DoingAdc = 0;
    Sample_Pro::NewCodeSampled[0] = 0;
    SampleProcess_Pulse::NewCodeSampled = 0;

    ChannelInfo_Manager::PulseLimitArriveData[ChannelIdx].bPulseStepEnded = 1;
    memset(&Pulse_LogLimit_task::LastReportTime[ChannelIdx],0, sizeof(MsTime)); //180613 zyx
}

#define     PULSE_SET_TIMER3_US(x)  LPC_TIM3->MR0 = x * 60 -1;      LPC_TIM3->PR  = 0;
#define     PULSE_CONVERT           LPC_GPIO1->CLR = (0x03 << 12);  LPC_GPIO1->SET = (0x03 << 12);

void        Pulse::PulseISR3(void)   //这个是调试代码，为了测试状态机分拆的效果
{
    unsigned short StgIdx = m_Info.m_StageIdx;
    st_GetMetaValue * p_MV2 = &ChannelInfo_Manager::MetaVariable2[0][0];

    switch(m_StateMachine)
    {
    case PulseSM_Dac:
        LPC_TIM3->MR0 = m_RiseTime.m_u32TimerSet;
        if(m_Info.m_StageIdx == 0)
            buf_info[active_index].m_CycleStartTime = CommonFunc::GetSystemTime();
        LPC_GPIO0->SET = (1<<16);      //Set DAC CS signal to high
        OS_ServeProcess::Set_TaskPriority2(OS_ServeProcess::OTID_4_Pulse_Check_non_V_Limit,m_StagePtr->m_Non_V_Limit_Priority);
        m_StateMachine = PulseSM_Sample1_Conv;
        return;
    case PulseSM_Sample1_Conv:
        PULSE_CONVERT;
        PULSE_SET_TIMER3_US(PULSE_ADC_STAGE_TIME_US);
        m_StateMachine = PulseSM_Sample1_Read;
        return;
    case PulseSM_Sample1_Read:
        LPC_TIM3->PR  = 0;
        ReadAdcAfterBusy(&Pulse_ADC_Curr,&Pulse_ADC_Volt);
        if((m_pSampleLast1->bValid)&&(m_pSampleLast2->bValid))
        {
            if((Pulse_ADC_Curr > m_StagePtr->m_PredictAdcCode_Max_Head)||(Pulse_ADC_Curr < m_StagePtr->m_PredictAdcCode_Min_Head))
            {
                Pulse_ADC_Curr = 2 * m_pSampleLast1->m_Stags[m_Info.m_StageIdx].m_uI_ADC_Code_Head - m_pSampleLast2->m_Stags[m_Info.m_StageIdx].m_uI_ADC_Code_Head;
                Pulse_ADC_Volt = 2 * m_pSampleLast1->m_Stags[m_Info.m_StageIdx].m_uV_ADC_Code_Head - m_pSampleLast2->m_Stags[m_Info.m_StageIdx].m_uV_ADC_Code_Head;
                m_SamleErrorCounter++;
            }
// 				PulseCurrent[1] = Pulse_ADC_Curr;
// 				PulseVoltage[1] = Pulse_ADC_Volt;
        }
        SaveRawData(StgIdx,Pulse_ADC_Curr,Pulse_ADC_Volt,SAVE_2_HEAD);
        if(m_StagePtr->m_bVerySlow)
            TIMERSFunc::Timer3_SetTick_Pre(m_StagePtr->m_TimerSet);     //特慢台阶，使用预分频设定TIMER3
        else
            LPC_TIM3->MR0 = m_StagePtr->m_TimerSet;
        if(!m_StagePtr->m_bFast)
        {
            m_Info.m_bPulseSample = 0;     //启动Timer0的Normal采样过程
            Sample_Pro::NewCodeSampled[0] = 0;
            SampleProcess_Pulse::NewCodeSampled = 0;
            ChannelInfo_Manager::Init_HistoryData(0);
            Sample_Pro::NewCodeSampled[0] = 0;
        }
        m_StateMachine = PulseSM_Sample2_Conv;
        return;

    case PulseSM_Sample2_Conv:
        m_Info.m_bPulseSample = 1;
        Dac_SendData(m_StagePtr->m_DacCode_NextStg);
        PULSE_CONVERT;
        PULSE_SET_TIMER3_US(PULSE_ADC_STAGE_TIME_US);
        m_StateMachine = PulseSM_Sample2_Read;

        return;
    case PulseSM_Sample2_Read:
        LPC_TIM3->MR0 = PULSE_ADC_STAGE_TIME_US * 60 -1;
        LPC_TIM3->PR  = 0;
        ReadAdcAfterBusy(&Pulse_ADC_Curr,&Pulse_ADC_Volt);
        if((m_pSampleLast1->bValid)&&(m_pSampleLast2->bValid))
        {
            if((Pulse_ADC_Curr > m_StagePtr->m_PredictAdcCode_Max_Tail)||(Pulse_ADC_Curr < m_StagePtr->m_PredictAdcCode_Min_Tail))
            {
                Pulse_ADC_Curr = 2 * m_pSampleLast1->m_Stags[m_Info.m_StageIdx].m_uI_ADC_Code_Tail - m_pSampleLast2->m_Stags[m_Info.m_StageIdx].m_uI_ADC_Code_Tail;
                Pulse_ADC_Volt = 2 * m_pSampleLast1->m_Stags[m_Info.m_StageIdx].m_uV_ADC_Code_Tail - m_pSampleLast2->m_Stags[m_Info.m_StageIdx].m_uV_ADC_Code_Tail;
                m_SamleErrorCounter++;
            }
// 			}
        }
        SaveRawData(StgIdx,Pulse_ADC_Curr,Pulse_ADC_Volt,SAVE_2_TAIL);
        m_Info.m_StageIdx++;
        if(m_Info.m_StageIdx >= m_Info.m_StageCount)
        {
            m_Info.m_StageIdx = 0;
            m_Info.m_CycleCounter++;
            p_MV2[MetaCode_PV_PulseCycle].m_Value.m_uInt32 ++;
            p_MV2[MetaCode_DV_PulseCycle].m_Value.m_uInt32 ++;
            p_MV2[MetaCode_PV_PulseCycle].m_fValue = (float)p_MV2[MetaCode_PV_PulseCycle].m_Value.m_uInt32;
            p_MV2[MetaCode_DV_PulseCycle].m_fValue = (float)p_MV2[MetaCode_DV_PulseCycle].m_Value.m_uInt32;
            buf_info[active_index].m_bDataLogged = 0;
            buf_info[active_index].m_bFull = 1;           //处理旧的buffer info
            m_StagePtr = &m_Info.m_Stages[0];
            active_index = !active_index;      //一个PULSE周期结束，换一个PULSE原始数据缓冲区进行数据存储

            buf_info[active_index].m_CycleIdx = m_Info.m_CycleCounter;
            buf_info[active_index].m_bFull = 0;
            buf_info[active_index].m_bDataLogged = 0;
            PulseHistorySample *myBuf = m_pSampleLast2;
            m_pSampleLast2 = m_pSampleLast1;
            m_pSampleLast1 = m_pSampleCurrent;
            m_pSampleCurrent = myBuf;
            if(m_SamleErrorCounter > 10)
            {
                m_pSampleCurrent = &m_HistorySample[0];
                m_pSampleLast1 = &m_HistorySample[1];
                m_pSampleLast2 = &m_HistorySample[2];
                m_pSampleCurrent->bValid = false;
                m_pSampleLast1->bValid = false;
                m_pSampleLast2->bValid = false;
                m_SamleErrorCounter = 0;
            }
        }
        else
            m_StagePtr++;
        m_StateMachine = PulseSM_Dac;
        return;
    case PulseSM_End:
    default:
        TIMERSFunc::Disable_Timer(3);     //disable timer3;
        return;
    }
}
inline void Pulse::SaveRawData(unsigned char StgIdx,unsigned long Current,unsigned long Voltage,unsigned char head_or_tail)
{
    PULSE_RAW_DATA *pData = &raw_data[active_index][head_or_tail][StgIdx];
    pData->m_bDataConverted = 0;
    pData->m_bSampleUpdated = 1;
    pData->m_uI_ADC_Code    = Current;
    pData->m_uV_ADC_Code    = Voltage;
    pData->m_uStageNum      = StgIdx;
    pData->m_bSumCapacity   = 0;
    pData->m_bSafetyChecked = 0;
    if(head_or_tail)
    {
        m_pSampleCurrent->m_Stags[StgIdx].m_uI_ADC_Code_Head = Current;
        m_pSampleCurrent->m_Stags[StgIdx].m_uV_ADC_Code_Head = Voltage;
    }
    else
    {
        m_pSampleCurrent->m_Stags[StgIdx].m_uI_ADC_Code_Tail = Current;
        m_pSampleCurrent->m_Stags[StgIdx].m_uV_ADC_Code_Tail = Voltage;
    }
    m_pSampleCurrent->bValid = true;
    pData->m_dataPoint = ChannelInfo_Manager::MetaVariable2[0][MetaCode_PV_DataPoints].m_Value.m_uInt32;
    if(board_config::DacInfo.m_bHaveBuildInVolt)
    {
        if((StgIdx == 0) && head_or_tail)
        {
            ChannelInfo_Manager::ChannelInfo[0].m_u32BuildInVolt_Pulse[0]  = ChannelInfo_Manager::ChannelInfo[0].m_u32BuildInVolt[0];
            ChannelInfo_Manager::ChannelInfo[0].m_bBuildInVoltNewCode_Pulse[0] = 1;
        }
    }
//   ChannelInfo_Manager::MetaVariable2[0][MetaCode_PV_DataPoints].m_Value.m_uInt32++;
}
inline void Pulse::ReadAdcAfterBusy (unsigned long *Data1,unsigned long *Data2)
{
    unsigned long uw32_timeout = 0;

    while (LPC_GPIO1->PIN & (0x01 << 19))
    {
        uw32_timeout++;
        if(uw32_timeout > 0xFFFF)
            return;
    }
    //    unsigned long Value;
    *Data1 = * (volatile unsigned short  *)(CPLD_BASE_ADDR+2);   //Hardware designer is so clever that we do not need to Setting CS or Readdata Pin.by HPQ
    *Data2 = * (volatile unsigned short  *)(CPLD_BASE_ADDR+2);
    ChannelInfo_Manager::ChannelInfo[0].m_u32BuildInVolt[0]  = * (volatile unsigned short  *)(CPLD_BASE_ADDR+2);
    ChannelInfo_Manager::ChannelInfo[0].m_u32BuildInVolt[1]  = * (volatile unsigned short  *)(CPLD_BASE_ADDR+2);
    *Data1 <<= 8;
    *Data2 <<= 8;
    if(board_config::DacInfo.m_bHaveBuildInVolt)
        ChannelInfo_Manager::ChannelInfo[0].m_u32BuildInVolt[0] <<= 8;

    volatile signed long sValue;

    sValue = (signed long)*Data1;
    sValue = sValue << 8;
    sValue = sValue >> 8;
    *Data1  = (unsigned long)(sValue + 0x800000);

    sValue = (signed long)*Data2;
    sValue = sValue << 8;
    sValue = sValue >> 8;
    *Data2  = (unsigned long)(sValue + 0x800000);

    if(board_config::DacInfo.m_bHaveBuildInVolt)
    {
        sValue = (signed long)ChannelInfo_Manager::ChannelInfo[0].m_u32BuildInVolt[0];
        sValue = sValue << 8;
        sValue = sValue >> 8;
        ChannelInfo_Manager::ChannelInfo[0].m_u32BuildInVolt[0]  = (unsigned long)(sValue + 0x800000);
        ChannelInfo_Manager::ChannelInfo[0].m_bBuildInVoltNewCode[0] = 1;
    }
}
inline void Pulse::Dac_SendData(unsigned long value)
{
    unsigned long DacCode = 0;

    DacCode = value >> 8;

    LPC_GPIO0->CLR |= (1<<16);
    LPC_SSP0->DR = DAC_REG_AD5754;
    LPC_SSP0->DR = (DacCode>>8);
    LPC_SSP0->DR = (0x00FF & DacCode);
}



