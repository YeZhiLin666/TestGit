#include "../Entry/includes.h"

//#define  PULSE_DAC_STAGE_TIME_US    60
#define  PULSE_ADC_STAGE_TIME_US    10
#define  PULSE_ADC_CURR_ERROR_HEAD  0x800000/2
#define  PULSE_ADC_CURR_ERROR_TAIL  0x800000/5
#define  PULSE_NEGLIGIBLE_TIME_MS   5.0f        //����pulse����resume�жϣ��ɺ���ʱ��  5ms
// bool                            bHigh;
// unsigned long PulseCurrent[3];     //��¼3�β����ĵ�����ֵ
// unsigned long PulseVoltage[3];     //��¼3�β����ĵ�ѹ��ֵ
unsigned long Pulse_ADC_Curr,Pulse_ADC_Volt,DeltaCurr;
unsigned char                   Pulse::m_StateMachine;       //��λ��̨����ת״̬��
St_PulseStageInfo_Running       *Pulse::m_StagePtr;          //̨������ָ�룬ָ���������е�̨�׶���

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x03E80000
#endif
St_PulseInfo_Running            Pulse::m_Info;               //����̬��PULSE���壬   ����λ�����ص�ԭʼPULSE����PULSE_DEF�������
PULSE_RAW_DATA                  Pulse::raw_data[2][2][MAX_PULSE_STAGE_NUM]; //[������1/2][ͷ/β][̨����] [2][2][];
PULSE_STAGE_INFO                Pulse::stage_info[2][MAX_PULSE_STAGE_NUM];  //��Ӧ�������������ģ��й�����stage����������

#pragma arm section //��������������

PULSE_BUF_INFO                  Pulse::buf_info[2];
unsigned char                   Pulse::active_index;            //pulse����������������������ݵ� ���
//unsigned char                   Pulse::stage_count;
bool                            Pulse::Pulse_is_Running ;     //Add by DKQ 08.20.2014
float                           Pulse::fMaxVoltage;           //���������ѹ���ֵ�����ڵ����V_LIMIT_CHECK����ʹ��
float                           Pulse::fMinVoltage;           //SafetyCheck_Pulse����Ҳ���õ�
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
{   //���Ӧ����LoadPulseData ()�����
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

    St_PulseData *PulsePtr = &ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx].m_PulseInfo.m_Data;  //changed by DKQ 08.20.2014,��Ҫ����ȷ�ϣ�ֻҪ�޸���Channelstep

    m_Info.m_SampType = ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[3];
    //����St_StepData��Pulse������ʱ�������г�ʼ������;
    if(PulsePtr->m_uStageNum > MAX_PULSE_STAGE_NUM)
        PulsePtr->m_uStageNum = MAX_PULSE_STAGE_NUM;

    m_Info.m_StageCount = PulsePtr->m_uStageNum;
    // m_Info.m_StageIdx   = 0;
    // stage_count         = m_Info.m_StageCount;

    unsigned short StageIdx = 0;
    unsigned long  TimeInUS = 0;
    unsigned char  CurrRange = ChannelInfo_Manager::ChannelStep[ChannelIdx].m_Info[ChannelInfo_Manager::ChannelStep[ChannelIdx].m_ucActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCurrentRange;
    float          CycleTime = 0;  //���������ʱ��

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
        p_Stage->m_bFast = (PulsePtr->m_StageWidth[StageIdx] < LOGGEST_FAST_STAGE);        //�Ƿ�fast stage���ܲ����schedule�ﴫ�������ø���̨�׿��������ȷ��
        if(p_Stage->m_bFast)
            p_Stage->m_Non_V_Limit_Priority = 8;
        else
            p_Stage->m_Non_V_Limit_Priority = 4;
        p_Stage->m_bVerySlow = (PulsePtr->m_StageWidth[StageIdx] >= 60000);  //���ʱ�����60�룬����Ϊ�ǳ���stage����Ҫ����timerԤ��Ƶ
        //��������ʱ���ת��������timerԤ��ƵΪ0ʱ�ļ��㣬��tick�ֱ���Ϊ0.017us
        float fTimeVar = PulsePtr->m_StageWidth[StageIdx] - (m_RiseTime.m_fUS + PULSE_ADC_STAGE_TIME_US * 3)*0.001f;
        if(fTimeVar < 0.01f)
            fTimeVar = 0.01f;
        if(p_Stage->m_bVerySlow)
            p_Stage->m_TimerSet = TIMERSFunc::Timer3_GetTick_Pre(fTimeVar);//(stage��� - T1 - T3)ʱ��ת��Ϊ��Ԥ��Ƶ����¶�Ӧ��tick��;
        else
            p_Stage->m_TimerSet = TIMERSFunc::Timer3_GetTick_NoPre(fTimeVar);//(stage��� - T1 - T3)ʱ��ת��Ϊû��Ԥ��Ƶ����¶�Ӧ��tick��;

// 		p_Stage->m_TimerSet2 = PULSE_ADC_STAGE_TIME_US * 60  -1 + p_Stage->m_TimerSet;
// 		p_Stage->m_TimerSet3 = PULSE_ADC_STAGE_TIME_US * 120 -1 + p_Stage->m_TimerSet;

        stage_info[SAVE_2_HEAD][StageIdx].m_TimeInUS = TimeInUS + 5;  //1605051,hepeiqing,for ATL's request
        TimeInUS += PulsePtr->m_StageWidth[StageIdx] * 1000 ;   //���� ת��Ϊ ΢��
        stage_info[SAVE_2_TAIL][StageIdx].m_TimeInUS = TimeInUS - 0;  //1605051,hepeiqing,for ATL's request
        PULSE_MV_Task::StgInfo[StageIdx].TimeInHour  = PulsePtr->m_StageWidth[StageIdx] / 1000 / 3600;  //���� ת��Ϊ Сʱ
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

    if(StepTime > PULSE_NEGLIGIBLE_TIME_MS)  //���step time���ڵ���100ms��֤������һ��pulse resume����Ҫ����pulse����ʼstage
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
    ChannelInfo_Manager::MetaVariable2[0][MetaCode_DV_PulseCycle].m_Value.m_uInt32 = 0;    //Resume��ʱ��Ҳ��ʼ���Ļ���ǰ�����е�ѭ�����Ͳ�������ˡ�dirui 2014/10/29
    ChannelInfo_Manager::MetaVariable2[0][MetaCode_DV_PulseCycle].m_fValue         = 0.0f;
    ChannelInfo_Manager::MetaVariable2[0][MetaCode_PV_PulseCycle].m_Value.m_uInt32 = m_Info.m_CycleCounter; //��Resume���������岽��ô�죿  dirui  2014/10/29
    ChannelInfo_Manager::MetaVariable2[0][MetaCode_PV_PulseCycle].m_fValue         = m_Info.m_CycleCounter;
    for(StageIdx = 0; StageIdx < m_Info.m_StageCount; StageIdx++)
    {
        PULSE_MV_Task::StgInfo[StageIdx].CalculatedCycle = m_Info.m_CycleCounter;
        m_Info.m_Stages[StageIdx].m_DacCode_NextStg = m_Info.m_Stages[(StageIdx+1)%m_Info.m_StageCount].m_DacCode;
    }

    m_StateMachine = PulseSM_Dac;          //��ʼ��pulse�жϴ���״̬��
    m_StagePtr     = &m_Info.m_Stages[0];  //stageָ��ָ���һ��̨��

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

    if(m_ResumeInfo.m_bResume == 0)    //ֱ������pulse������resume
    {
        DAC16bit_AD5754::Dac_SendData(0,m_Info.m_Stages[0].m_DacCode);
        unsigned long PrePulseTick = TIMERSFunc::Timer3_GetTick_NoPre(0.05f);  //����PULSE�жϵ� ǰ�õȴ�ʱ��==>50us
        TIMERSFunc::Init_Timer(PrePulseTick,3);
        TIMERSFunc::Enable_Timer(3);
    }
    else
    {   //resumeһ��pulse
        m_StagePtr = &m_Info.m_Stages[m_ResumeInfo.m_StgIdx];
        if(m_ResumeInfo.m_fElapsedTime <= 0)
        {   //����stage���¿�ʼ
            m_Info.m_StageIdx = m_ResumeInfo.m_StgIdx;
            DAC16bit_AD5754::Dac_SendData(0,m_Info.m_Stages[m_Info.m_StageIdx].m_DacCode);
            unsigned long PrePulseTick = TIMERSFunc::Timer3_GetTick_NoPre(0.05f);  //����PULSE�жϵ� ǰ�õȴ�ʱ��==>50us
            TIMERSFunc::Init_Timer(PrePulseTick,3);
            TIMERSFunc::Enable_Timer(3);
        }
        else
        {   //stage���ضϿ�ʼ
            m_Info.m_StageIdx = m_ResumeInfo.m_StgIdx;
            DAC16bit_AD5754::Dac_Out(0,m_Info.m_Stages[m_Info.m_StageIdx].m_DacCode);
            unsigned long PrePulseTick = TIMERSFunc::Timer3_GetTick_NoPre(0.05f);  //����PULSE�жϵ� ǰ�õȴ�ʱ��==>50us
            TIMERSFunc::Init_Timer(PrePulseTick,3);
            TIMERSFunc::Enable_Timer(3);
            if(m_StagePtr->m_bVerySlow)
                TIMERSFunc::Timer3_SetTick_Pre(m_StagePtr->m_TimerSet - TIMERSFunc::Timer3_GetTick_Pre(m_ResumeInfo.m_fElapsedTime));     //����̨�ף�ʹ��Ԥ��Ƶ�趨TIMER3
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
    unsigned long PrePulseTick = TIMERSFunc::Timer3_GetTick_NoPre(0.01f);  //��PULSE�жϵ� ���õȴ�ʱ��==>10us
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

void        Pulse::PulseISR3(void)   //����ǵ��Դ��룬Ϊ�˲���״̬���ֲ��Ч��
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
            TIMERSFunc::Timer3_SetTick_Pre(m_StagePtr->m_TimerSet);     //����̨�ף�ʹ��Ԥ��Ƶ�趨TIMER3
        else
            LPC_TIM3->MR0 = m_StagePtr->m_TimerSet;
        if(!m_StagePtr->m_bFast)
        {
            m_Info.m_bPulseSample = 0;     //����Timer0��Normal��������
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
            buf_info[active_index].m_bFull = 1;           //����ɵ�buffer info
            m_StagePtr = &m_Info.m_Stages[0];
            active_index = !active_index;      //һ��PULSE���ڽ�������һ��PULSEԭʼ���ݻ������������ݴ洢

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



