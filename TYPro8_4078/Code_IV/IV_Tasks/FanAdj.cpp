///////////////////////////////////////////////////////////////////////////////////
//File Name: FanAdj.cpp
//Version:		V1.0
//Date:				2015/05/13
//Description:
//						Fan管理操作
//Others:
//History：

///////////////////////////////////////////////////////////////////////////////////
#include "includes.h"

UInt16 FanAdj_task::TaskID=FANADJ_TASK;                                                                    //任务ID
UInt16 FanAdj_task::Priority=PRIORITY8;




unsigned char FanAdj_task::AdjMod;
unsigned char FanAdj_task::Output_Level;


unsigned char FanAdj_task::MAX_Ratio=0;//added by NiuYasong 2016.4.20
MsTime FanAdj_task::LastSampleTime;

FanLeveInfo FanAdj_task::AllFanLevelInfo[8];

unsigned char FanAdj_task::FanCtrlValue[CURRENT_RANGE_COUNT][8];
void  FanAdj_task::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_FANADJ] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time_init(COUNT_FANADJ);
#endif
	
    if(!board_config::bFanAdj)
        return;

    unsigned char m_Level;
    float  m_ChannelConsumePower;
    float ChannelPowerRatio;
    float  m_CurrentVol,m_CurrentCur;
    float  ChannelPerAdjPower;
    unsigned char Channel_m_Level;
    unsigned char Channel_Duty_Ratio;
    float  VF;
    float  VR;
    MsTime tempT;
    tempT = CommonFunc::TimeDiff(LastSampleTime);


    if(tempT.Us100 >= 3000)
    {
        VF=ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVpp;
        VR=ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVmm;
        MAX_Ratio = 30;//先设成最小

        for(unsigned char uc_i=0; uc_i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; uc_i++)
        {
            float V_High = ChannelInfo_Manager::BoardCFG.m_Channel[uc_i].m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High;
            float I_High = ChannelInfo_Manager::GetMaxCurrRangeValue(uc_i,ChannelInfo_Manager::ChannelInfo[uc_i].m_ucCurrentRange);
			
						ChannelPerAdjPower = abs(VR-V_High)*I_High;	



            if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_PWM)
                ChannelPerAdjPower = I_High;

            m_CurrentCur=ChannelInfo_Manager::Get_MetaVariable(uc_i,MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT,MetaCode_PV_Current);
            m_CurrentVol=ChannelInfo_Manager::Get_MetaVariable(uc_i,MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT,MetaCode_PV_Voltage);
// 			 if(m_CurrentCur>0)
// 				 m_ChannelConsumePower=abs(((VF-m_CurrentVol)*m_CurrentCur));
// 			 else
// 				 m_ChannelConsumePower=abs(((m_CurrentVol-VR)*m_CurrentCur));

            if(m_CurrentCur>0)
                m_ChannelConsumePower=VF-m_CurrentVol;
            else
                m_ChannelConsumePower=m_CurrentVol-VR;
            m_ChannelConsumePower=abs(m_ChannelConsumePower*m_CurrentCur);


            if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_PWM)
                ChannelPowerRatio = abs(m_CurrentCur)/ChannelPerAdjPower;	//Here ChannelPerAdjPower[uc_i] is max current of this range.
            else
                ChannelPowerRatio = m_ChannelConsumePower/ChannelPerAdjPower;//added by NiuYasong 2016.4.20

            Channel_m_Level= (int)((ChannelPowerRatio-0.1f)*10);
            if(ChannelPowerRatio<0.1f)
                Channel_m_Level= 0;
            if(Channel_m_Level>7)
                Channel_m_Level= 7;

            Channel_Duty_Ratio = FanCtrlValue[ChannelInfo_Manager::ChannelInfo[uc_i].m_ucCurrentRange][Channel_m_Level];//获得各通道对应风扇速度
            if(Channel_Duty_Ratio>MAX_Ratio)
            {
                MAX_Ratio = Channel_Duty_Ratio;
            }
        }


        if(MAX_Ratio<30)
            MAX_Ratio=30;
        m_Level = (int)(MAX_Ratio/10)-3;
        if(m_Level>7)
            m_Level = 7;

        AllFanLevelInfo[m_Level].Fan_Level_Valid = 1;
        AllFanLevelInfo[m_Level].RecordProduceTime = CommonFunc::GetSystemTime();


        MsTime FanTempT;
        Output_Level = 0;
        for(unsigned char level = 7; level>0; level--)
        {
            if(AllFanLevelInfo[level].Fan_Level_Valid == 0)
            {
                continue;
            }
            else
            {
                FanTempT = CommonFunc::TimeDiff(AllFanLevelInfo[level].RecordProduceTime);
                if(FanTempT.Second<=5)
                {
                    Output_Level = level;
                    break;
                }
                else
                {
                    AllFanLevelInfo[level].Fan_Level_Valid = 0;
                }
            }
        }

        if(AdjMod == PWMMOD)
        {
            unsigned char duty_ratio = Output_Level*10 + 30;	// calculate duty ratio directly, only for PWM based fan control
            if(duty_ratio>100)
                duty_ratio = 100;
            (* board_config::Func_FanAdj)(duty_ratio,AdjMod);
        }
        else
            (* board_config::Func_FanAdj)(Output_Level,AdjMod);

        CommonFunc::GetTimeTicksUpdate(&LastSampleTime);
    }
		
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_FANADJ] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT + 1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time(COUNT_FANADJ);
#endif
}
void  FanAdj_task::MyInit(void)
{
    // 	board_config::bFanAdj=0;

    // 	FanAdj_task::VF=0;
    // 	FanAdj_task::VR=0;
    // 	FanAdj_task::HeatSinkCounter=1;
    CommonFunc::GetTimeTicksUpdate(&LastSampleTime);
    for(unsigned char level = 0; level<8; level++)
    {
        AllFanLevelInfo[level].Fan_Level_Valid = 0;
        AllFanLevelInfo[level].RecordProduceTime = CommonFunc::GetSystemTime();
    }
    AllFanLevelInfo[7].Fan_Level_Valid = 1;// Highest level
    AllFanLevelInfo[7].RecordProduceTime = CommonFunc::GetSystemTime();
    // 	FanAdj_task::PerAdjPower=1;
    // 	FanAdj_task::AdjMod=FANADJ_NA;
    Output_Level = 0;


}
// void  FanAdj_task::EndTask(void)
// {

// }
