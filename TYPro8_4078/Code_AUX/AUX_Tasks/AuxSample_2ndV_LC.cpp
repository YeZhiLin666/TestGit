#include "../Entry/includes.h"

//public:
unsigned short               Aux_Sample_2ndV_LC::Priority = PRIORITY4;
unsigned short               Aux_Sample_2ndV_LC::TaskID = AUX_SAMPLE_2ndV_LC_TASK;
unsigned char								 Aux_Sample_2ndV_LC::ChanIdx;
unsigned char                Aux_Sample_2ndV_LC::CS_Idx;
MsTime											 Aux_Sample_2ndV_LC::TimeStamp;
MsTime											 Aux_Sample_2ndV_LC::WaitTime;
unsigned char								 Aux_Sample_2ndV_LC::TaskStatus;
unsigned short               Aux_Sample_2ndV_LC::TimeCount;

My_Func_Ptr_Void             Aux_Sample_2ndV_LC::Board_Adc_Trigger;
My_Func_Ptr_Ext1             Aux_Sample_2ndV_LC::Board_Adc_Read;
St_Port                      Aux_Sample_2ndV_LC::Pin_MUXWR;
St_Port                      Aux_Sample_2ndV_LC::Pin_VAD0;
St_Port                      Aux_Sample_2ndV_LC::Pin_VAD1;
St_Port                      Aux_Sample_2ndV_LC::Pin_VAD2;
St_Port                      Aux_Sample_2ndV_LC::Pin_VAD3;
unsigned short               Aux_Sample_2ndV_LC::Sample_Wait_Count = 199;
unsigned short               Aux_Sample_2ndV_LC::Sample_Wait_Count2 = 11;
// void  (*Aux_ReadDI::m_ReadDI)(void);
// void  (*Aux_ReadDI::m_InitDI)(void);
//private:
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif

#pragma arm section //结束外存变量定义 


////////
//    Init(void)
//    : DI模块初始化处理函数
// Input:  N/A
// Return: N/A
///////
void Aux_Sample_2ndV_LC::Init(void)
{
    // (*m_InitDI)();
    ChanIdx = 0;
    CS_Idx = 0;
    TimeStamp.Second = 0;
    TimeStamp.Us100 = 0;
    WaitTime.Second = 0;
    WaitTime.Us100 = 0;
    TimeCount = 0;

    TaskStatus = AUX_2ndV_LC_Sample_IDLE;


    CommonFunc::GetTimeTicksUpdate(&WaitTime);
}

void Aux_Sample_2ndV_LC::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_SAMPLE_2NDV_LC] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_SAMPLE_2NDV_LC);
#endif
    //	(*m_ReadDI)();
    //unsigned char Last_chn;
    //if(AUX_Unit_Base::Aux_Init_Finished_flag==0)
    // return;


    //Adc24bit_ADS1259::Convert();
    static unsigned long long ticks = 0;
    static MsTime TimeStamp1;
    switch(TaskStatus)
    {
    case  AUX_2ndV_LC_Sample_IDLE:
        TaskStatus = AUX_2ndV_LC_Sample_SWITCH;
        break;
    case  AUX_2ndV_LC_Sample_SWITCH:
        SetChip(ChanIdx);

        CommonFunc::GetTimeTicksUpdate(&TimeStamp);
        TaskStatus = AUX_2ndV_LC_Sample_WAIT1;
        break;
    case  AUX_2ndV_LC_Sample_WAIT1:

        if (CommonFunc::GetTimeTicks(&TimeStamp) > 15)
            TaskStatus = AUX_2ndV_LC_Sample_TRIGGER;
        else
            TaskStatus = AUX_2ndV_LC_Sample_WAIT1;
        break;
    case AUX_2ndV_LC_Sample_TRIGGER:
        ticks = CommonFunc::GetTimeTicks(&WaitTime);
        CommonFunc::GetTimeTicksUpdate(&WaitTime);
        Aux_2ndV_LC_601310_Base::Aux_V_Trigger();  // Trigger for all 2ndV_LC boards
        TimeStamp1 = CommonFunc::GetSystemTime();
//         TimeStamp1.Second = OS_ServeProcess::OS_Time.Second;
//         TimeStamp1.Us100	= OS_ServeProcess::OS_Time.Us100;
        CommonFunc::GetTimeTicksUpdate(&TimeStamp);
        //TaskStatus = AUX_2ndV_LC_Sample_WAIT2;
        TaskStatus = AUX_2ndV_LC_Sample_READ;
    case  AUX_2ndV_LC_Sample_WAIT2:
        if (CommonFunc::GetTimeTicks(&TimeStamp) > 9)     //55
            TaskStatus = AUX_2ndV_LC_Sample_READ;
        else
            TaskStatus = AUX_2ndV_LC_Sample_WAIT2;
        break;
    case AUX_2ndV_LC_Sample_READ:

        unsigned char checked_CS =0;
        for (unsigned char i = 0; i<8; i++)  // At most 8 LC boards
        {
            if ((Aux_DataManager::Aux_ADC_CS_Info[CS_Idx].m_Valid)&&
                    Aux_DataManager::Aux_ADC_CS_Info[CS_Idx].b_ADCCH_Valid[ChanIdx])  //only sample for a valid board and valid channels

            {
                Aux_DataManager::SettingCS_Low(Aux_DataManager::Aux_ADC_CS_Info[CS_Idx].m_CS_Address.m_Value);
                Aux_2ndV_LC_601310_Base::Aux_V_Read(CS_Idx, ChanIdx);
                Aux_DataManager::SettingCS_High();
                checked_CS++;

            }
            CS_Idx++;

            if ( CS_Idx>=8)
            {
                CS_Idx = 0;
                TaskStatus = AUX_2ndV_LC_Sample_SWITCH;   //switch to next channel after all 8 boards
                if( (++ChanIdx) > 15)
                {
                    ChanIdx = 0;
                }


                SetChip(ChanIdx);

                //OS_ServeProcess::recordCurTime(&TimeStamp);
                TaskStatus = AUX_2ndV_LC_Sample_WAIT1;

                break;

                //							   if (checked_CS >=2)  // each time only sample at most 2 boards
                //										{	checked_CS=0;
                //											break;
                //										}
            }
        }

        break;
    default:
        break;

        /*case  AUX_2ndV_LC_Sample_IDLE:
        TaskStatus = AUX_2ndV_LC_Sample_SWITCH;
        break;
        case  AUX_2ndV_LC_Sample_SWITCH:
        SetChip(0);
        OS_ServeProcess::recordCurTime(&TimeStamp);
        TaskStatus = AUX_2ndV_LC_Sample_WAIT1;
        break;
        case  AUX_2ndV_LC_Sample_WAIT1:
        WaitTime = OS_ServeProcess::TimeDiff(TimeStamp);
        if(WaitTime.Second>0  || WaitTime.Us100>25)
        TaskStatus = AUX_2ndV_LC_Sample_TRIGGER;
        break;
        case AUX_2ndV_LC_Sample_TRIGGER:
        Aux_DataManager::Aux_ADC_CS_Info[0].m_BoardADC_Trigger(7,0);
        OS_ServeProcess::recordCurTime(&TimeStamp);
        TaskStatus = AUX_2ndV_LC_Sample_WAIT2;
        case  AUX_2ndV_LC_Sample_WAIT2:
        WaitTime = OS_ServeProcess::TimeDiff(TimeStamp);
        if(WaitTime.Second>0  || WaitTime.Us100>5)
        TaskStatus = AUX_2ndV_LC_Sample_READ;
        break;
        case AUX_2ndV_LC_Sample_READ:
        for(int boardID=0; boardID<Aux_DataManager::m_Total_2ndV_LC_Boards; boardID++)
        {
        Aux_DataManager::SettingCS_Low(7);
        Aux_DataManager::Aux_ADC_CS_Info[boardID].m_BoardADC_Rd(0,0);
        }
        TaskStatus = AUX_2ndV_LC_Sample_SWITCH;
        break;
        default:
        break;*/
    }

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_SAMPLE_2NDV_LC] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
		Time_Count::thread_time(COUNT_SAMPLE_2NDV_LC);
#endif
}


void Aux_Sample_2ndV_LC::SetChip(unsigned char ChIdx)
{
    static MsTime TimeStamp2;
    static unsigned long long diff=0;
    ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_MUXWR);
    ARBUS_DELAY(50);
    ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_MUXWR);
    switch(ChIdx)
    {
    case 0:
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD3);
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD2);
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD1);
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD0);

        diff = CommonFunc::GetTimeTicks(&TimeStamp2);
        TimeStamp2 = CommonFunc::GetSystemTime();
//         TimeStamp2.Second = OS_ServeProcess::OS_Time.Second;
//         TimeStamp2.Us100	= OS_ServeProcess::OS_Time.Us100;
        break;
    case 1:
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD3);
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD2);
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD1);
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD0);
        break;
    case 2:
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD3);
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD2);
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD1);
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD0);
        break;
    case 3:
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD3);
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD2);
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD1);
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD0);
        break;
    case 4:
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD3);
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD2);
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD1);
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD0);
        break;
    case 5:
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD3);
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD2);
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD1);
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD0);
        break;
    case 6:
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD3);
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD2);
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD1);
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD0);
        break;
    case 7:
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD3);
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD2);
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD1);
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD0);
        break;
    case 8:
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD3);
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD2);
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD1);
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD0);
        break;
    case 9:
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD3);
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD2);
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD1);
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD0);
        break;
    case 10:
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD3);
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD2);
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD1);
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD0);
        break;
    case 11:
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD3);
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD2);
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD1);
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD0);
        break;
    case 12:
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD3);
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD2);
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD1);
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD0);
        break;
    case 13:
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD3);
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD2);
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD1);
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD0);
        break;
    case 14:
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD3);
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD2);
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD1);
        ControlPin::SetMeLow(Aux_Sample_2ndV_LC::Pin_VAD0);
        break;
    case 15:
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD3);
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD2);
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD1);
        ControlPin::SetMeHigh(Aux_Sample_2ndV_LC::Pin_VAD0);
        break;
    default:
        break;
    }
}