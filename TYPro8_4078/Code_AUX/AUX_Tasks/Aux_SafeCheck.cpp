//AuxSafeCheck
//按照IV来遍历
//即 当前aux mcu被map到几个iv mcu上  来查询
//每次只操作一个即可
//updata coment out chen 20131211

#include "../Entry/includes.h"
#include "../../COMMON_os/Ext_Devices/LM75B.h"

//UInt16 safety_check::Priority = PRIORITY4;
//UInt16 safety_check::TaskID = SAFETYCHECK_TASK;			// should be a definition which positioning this task in OS_Task[]


UInt16 Aux_Safety_Check::Priority = PRIORITY4;
UInt16 Aux_Safety_Check::TaskID = SAFETYCHECK_TASK;


#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
unsigned char Aux_Safety_Check::sampleFlag[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU];   //采样标志


ST_AUX_SAFETY_CHECK Aux_Safety_Check::st_aux_safety[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU];    //[MAX_AUX_TYPE];

unsigned char Aux_Safety_Check::safety_id[MAX_IVMCU_PER_SYS];
unsigned char Aux_Safety_Check::safety_chn[MAX_IVMCU_PER_SYS];

// 设置变量定义在外存

MsTime Aux_Safety_Check::safety_last_time[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU][MAX_AUXCH_PER_AUXMCU];
MsTime Aux_Safety_Check::safety_error_time[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU][MAX_AUXCH_PER_AUXMCU];
MsTime Aux_Safety_Check::safety_resume_time[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU][MAX_AUXCH_PER_AUXMCU];
ST_AUX_SAFETY_RECORD Aux_Safety_Check::st_check_result[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU];
unsigned char Process_SM_Ptr[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU][MAX_AUX_TYPE_PER_MCU][MAX_AUXCH_PER_AUXMCU];
//RedundantVoltage
St_IV_M4ADCChnMap Aux_Safety_Check::st_IV2M4ADCChnMap;
St_SafetyCeck Aux_Safety_Check::st_SafetyCheck[M4_MAX_CHANNELNO];
unsigned char  Aux_Safety_Check::EmergencyStopCheckEnable=0;     //0为不具备此功能， 1为具备急停DI输入检测功能

ST_OnlineRSM_Safety_RECORD  Aux_Safety_Check::st_OnlineRSM_UnsafeData[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU];
unsigned char Aux_Safety_Check::MonitorSafetyLedState[MAX_IVMCU_PER_SYS][MAX_IVCH_PER_IVMCU];//三种状态 0 Nouse,1 moitoring,2protect.
unsigned char Aux_Safety_Check::Led_State[8];
MsTime       Aux_Safety_Check::LED_ProtectTime;
#pragma arm section //结束外存变量定义


void Aux_Safety_Check::Init(void)
{

    for (unsigned char m = 0; m<MAX_IVMCU_PER_SYS; m++)
    {
        safety_id[m] = 0;    //当成iv_chn
        safety_chn[m] = 0;

        for (unsigned char i = 0; i<MAX_IVCH_PER_IVMCU; i++)
        {
            for (unsigned char j = 0; j<MAX_AUX_TYPE_PER_MCU; j++)
            {
                for (unsigned char k = 0; k<MAX_AUXCH_PER_AUXMCU; k++)
                {
                    Process_SM_Ptr[m][i][j][k] = SAFE;
                }
            }
        }
    }
}


//具体实现
//1 m_AuxSafetyBank.m_safety_alive进行check
//2 在这些通道内进行check，并根据sampleFlag进行check
//3 从m_AuxDataBank取出pv与m_AuxSafetyBank比较
//4 在一定时间内连续unsafe，则根据m_AuxSafetyBank 记录的IVid向IV报警
//---------------------------------------------------------------
//上述有一定问题  20131211 chen
//m_AuxSafetyBank是按照IV号，辅助通道类型 放的  也就说他存放的是IV的信息    Aux_AddSafety函数    CAN_CMD_DOWNLOAD_SAFETY
//而Aux的数据仍然是按照辅助板号和通道号来存放的，所以这里需要一个转换
//所以Aux的安全检查不同于iv，每次按照iv通道来检查
//扩展过后的辅助，需要根据MAP的IVMCU来检查。 因为各个IVMCU使用的辅助通道会不一样。

void Aux_Safety_Check::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_SAFTY] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_SAFTY);
#endif

// 	unsigned char checkItem = CheckAndSetCandidate(safety_id);
    unsigned char checkItem;
    unsigned char uc_IVunitBufIndex;
    unsigned char CheckValue;
    for (uc_IVunitBufIndex =0; uc_IVunitBufIndex < MAX_IVMCU_PER_SYS; uc_IVunitBufIndex ++)
    {
        checkItem = CheckAndSetCandidate(safety_id[uc_IVunitBufIndex]);
        if(Aux_DataManager::m_IVMCU_ID_Info[uc_IVunitBufIndex].m_Valid)	//对没有MAP到的IVMCU跳过
        {
            check(uc_IVunitBufIndex,safety_id[uc_IVunitBufIndex],checkItem);			// 检查相应安全    checkItem只是延续了IV，这里并不起作用

            statusProcess(uc_IVunitBufIndex,safety_id[uc_IVunitBufIndex]);		// 根据状态处理相关结果

            safety_id[uc_IVunitBufIndex]++;    //next

            if(safety_id[uc_IVunitBufIndex] > MAX_IVCH_PER_IVMCU)
            {
                safety_id[uc_IVunitBufIndex] = 0;
            }
        }
    }
    if(EmergencyStopCheckEnable==1)
    {
        CheckValue = Aux_ExtCharge_601618_Base::EmergencyStop_Check();
        Aux_ExtCharge_601618_Base::EmergencyStop_Set(CheckValue);
    }
    else if(EmergencyStopCheckEnable==2)
    {
        CheckValue = Aux_Monitor_601618_Base::EmergencyStop_Check();
        Aux_Monitor_601618_Base::EmergencyStop_Set(CheckValue);
    }
    if(Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_BoardType==Aux_OnlineRSM_602026)
    {
        for (unsigned char uc_LedIdex =0; uc_LedIdex < 8; uc_LedIdex ++)
        {
            Aux_OnlineRSM_602026_Base::Set_LEDAlarm(Led_State[uc_LedIdex], uc_LedIdex);
        }
    }

    if(Aux_MTC_Base::Aux_MTC_Board == true)  //add by zc for MTC
    {
        for (uc_IVunitBufIndex =0; uc_IVunitBufIndex < MAX_IVMCU_PER_SYS; uc_IVunitBufIndex ++)
        {
            for (int m = 0; m < MAX_IVCH_PER_IVMCU; m++)
            {
                for( int n = 0; n < Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][m][0].m_Counts_ByScheduleMap; n++)
                {
                    if( Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][m][0].m_MapInfo[n].m_Index_InAuxUnit == Aux_MTC_Base::index)
                    {
                        if(Aux_MTC_Base::Aux_MTC_TemperatureSafetyCheckFlag[Aux_MTC_Base::index] == 1) {
                            Aux_MTC_Base::TemperatureCheck(Aux_DataManager::m_IVMCU_ID_Info[uc_IVunitBufIndex].m_ID, m, AUX_TEMPERATURE, Aux_MTC_Base::index);
                        }
                        Aux_MTC_Base::FanLockCheck(Aux_DataManager::m_IVMCU_ID_Info[uc_IVunitBufIndex].m_ID, m, AUX_TEMPERATURE, Aux_MTC_Base::index);
                    }
                }
            }
        }
    }

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_SAFTY] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
		Time_Count::thread_time(COUNT_SAFTY);
#endif
}

void Aux_Safety_Check::EndTask(void)
{

}


//private
//Aux_DataManager::m_AuxSafetyBank加入了一个range，成了三维数组
unsigned char Aux_Safety_Check::CheckAndSetCandidate(unsigned char uc_IVch)
{
    //if (Aux_DataManager::m_AuxSafetyBank[Bid][chn][0].m_bUnsafe == 0)    //IVMCU在启动实验时将该IV通道的safety设定用CAN下传到AuxMCU
    //{
    //	return AUX_NO_CHECK;
    //}
    //else
    //{
    //	if(Aux_DataManager::m_AuxSafetyBank[Bid][chn][0].m_aux_type == AUX_TEMPERATURE)
    //	{
    //		return AUX_T_CHECK;
    //	}
    //	else    //二次电压
    //	{
    //		return AUX_V_CHECK;
    //	}
    //}


    //去掉这个，每次必check
    return 1;
}

//Aux_AddSafety (ST_AUX_SAFETY st_safety)  只能根据这个进行统一的操作
//每次检查一个IVMCU中的一个IVCH
unsigned char Aux_Safety_Check::check(unsigned char uc_IVunitBufIndex, unsigned char uc_IVch,uint16 checkItem)
{

    for (unsigned char uc_AuxType =0; uc_AuxType < MAX_AUX_TYPE; uc_AuxType++)  //对该IV通道下所有类型的辅助进行循环
    {
        unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(uc_AuxType);
        if(TempAuxTypeIdex == 0xFF)//辅助限制
            continue;

        if (uc_AuxType==AUX_SAFETY)  //如果类型是Safety， 不执行以下代码
        {
            OnlineRSMSafetyCheck(uc_IVunitBufIndex,uc_IVch);
            OnlineRSMStateScan(uc_IVunitBufIndex, uc_IVch);
            continue;
        }
        if(Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_HIGH_END]&&
                Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_LOW_END])   //如果该类型的辅助通道设了安全上限下限
        {
            for (unsigned char i =0; i< Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Counts_ByScheduleMap; i++) //检查该类型的所通道
                //找出该IV该类型辅助的所有通道进行轮询	,
            {

                ST_AUX_DATA  st_auxdata;
                unsigned char uc_AuxCh = Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_MapInfo[i].m_Index_InAuxUnit;
                Aux_DataManager::Aux_GetData(Aux_DataManager::m_IVMCU_ID_Info[uc_IVunitBufIndex].m_ID, uc_IVch, uc_AuxType, uc_AuxCh, &st_auxdata);
                float f_PresentValue = st_auxdata.aux_data;

                //[0] 相当于low   [1]相当于high
                if( (f_PresentValue >= Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safe_value[AUX_SAFETY_HIGH_END]  )
                        || (f_PresentValue <= Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safe_value[AUX_SAFETY_LOW_END])	)
                {
                    if(uc_AuxCh<=63)
                    {
                        st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[0] |= ((U64)(1)<<uc_AuxCh);
                    }
                    else
                    {
                        st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[1] |= ((U64)(1)<<(uc_AuxCh-64));
                    }

                    st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].f_record[uc_AuxCh] = f_PresentValue;
                    st_aux_safety[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].ErrorFlag = uc_AuxType;    //对于IV来说，如果有一个aux通道超出unsafe，就是unsafe
                    st_aux_safety[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].f_record = f_PresentValue;    //向IV汇报的值

                }
                else  //否则，清除标准
                {
                    if(i<=63)
                    {
                        st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[0] &= ~((U64)(1)<<uc_AuxCh);
                    }
                    else
                    {
                        st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[1] &= ~((U64)(1)<<(uc_AuxCh-64));
                    }
                }
            }
        }


    }

    return 1;
}

void Aux_Safety_Check::CleanSafetyCheckResults( unsigned char uc_IVunitBufIndex,unsigned char uc_IVch)
{
    for (unsigned char uc_AuxType =0; uc_AuxType < MAX_AUX_TYPE; uc_AuxType++)
    {
        unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(uc_AuxType);
        if(TempAuxTypeIdex == 0xFF)//辅助限制
            continue;

        st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[0] = 0;
        st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[1] = 0;     //这里也应该清零

        for(unsigned char j = 0; j< Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Counts_ByScheduleMap; j++)
        {
            unsigned char uc_AuxCh =Aux_DataManager:: m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_MapInfo[j].m_Index_InAuxUnit;
            Process_SM_Ptr[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh] = SAFE;
        }

    }
}


unsigned char Aux_Safety_Check::statusProcess(unsigned char uc_IVunitBufIndex, unsigned char uc_IVch)
{
    unsigned char unsafe = 0;    //这个没清会导致后面的报错  20131218
    MsTime Time;    //用于时间比较

    for(unsigned char uc_AuxType = 0; uc_AuxType < MAX_AUX_TYPE; uc_AuxType ++)
    {
        unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(uc_AuxType);
        if(TempAuxTypeIdex == 0xFF)//辅助限制
            continue;

        if( (!Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_HIGH_END])  ||  \
                (!Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_LOW_END]) )    //如果该类型的辅助通道没设安全上限下限
            continue;
        for(unsigned char i =0; i< Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Counts_ByScheduleMap; i++)    // DKQ 01032014    //1个IV通道最大有128个Aux
        {
            unsigned char uc_AuxCh = Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_MapInfo[i].m_Index_InAuxUnit;  //找到在辅助里的通道号

            if(uc_AuxCh <64)
            {
                if(	st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[0]&((U64)(1)<<uc_AuxCh))
                {
                    unsafe = 1;
                }
                else
                {
                    unsafe = 0;
                }
            }
            else if(uc_AuxCh>=64 )    //else有利于加快运行速度
            {
                if(	st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[1]&((U64)(1)<<(uc_AuxCh-64)))
                {
                    unsafe = 1;
                }
                else
                {
                    unsafe = 0;
                }
            }

            switch(Process_SM_Ptr[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh])
            {
            case SAFE:
                if (unsafe)
                {
                    //记录时间
                    CommonFunc::GetTimeTicksUpdate(&safety_last_time[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh]);
                    //st_check_result[uc_IVch][uc_AuxType].m_state[uc_AuxCh] = SAFE2UNSAFE;
                    Process_SM_Ptr[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh] = SAFE2UNSAFE;
                }
                else
                {
                    //do nothing
                }
                break;
            case SAFE2UNSAFE:
                if (unsafe)
                {
                    //比较时间
                    CommonFunc::GetTimeTicksUpdate(&safety_error_time[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh]);
                    Time = TimeT(safety_last_time[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh],safety_error_time[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh]);    //比较这2个时间  safety_error_time>safety_last_time
                    if(Time.Us100 >= SAFETY_CHECK_PERIOD)
                    {
                        sendError(uc_IVunitBufIndex,uc_IVch,uc_AuxType,uc_AuxCh);		// trigger a sending //这个需要根据新的MAP修改
                        //Schedule_Manager_task::LockModeSwitch[chn]=1; //现在unsafe处理只给Schedule发送一次，那么若状态切换了，并且没有回复正常的话，就需要有标记表示现在的Unasfe状态。 0901 dirui
                        Process_SM_Ptr[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh] = UNSAFE;

                        break;
                    }
                }
                else
                {
                    Process_SM_Ptr[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh] = SAFE;
                }
                break;
            case UNSAFE:
                if(unsafe)
                {
                    //do nothing
                }
                else
                {
                    //记录恢复时间
                    CommonFunc::GetTimeTicksUpdate(&safety_resume_time[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh]);
                    Process_SM_Ptr[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh] = UNSAFE2SAFE;
                }
                break;
            case UNSAFE2SAFE:
                if (unsafe)
                {
                    Process_SM_Ptr[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh] = UNSAFE;
                }
                else
                {
                    //比较恢复时间，准备恢复正常
                    CommonFunc::GetTimeTicksUpdate(&safety_last_time[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh]);
                    Time = TimeT(safety_last_time[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh],safety_resume_time[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh]);    //resume time 比较
                    //if(Time.Us100 >= SAFETY_CHECK_RESUME)
                    if(Time.Us100+Time.Second*10000 >= SAFETY_CHECK_RESUME)
                    {
                        resumeError(uc_IVunitBufIndex,uc_IVch,uc_AuxType,uc_AuxCh);		//需要重写这个函数
                        Process_SM_Ptr[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh] = SAFE;
                        break;
                    }
                }
                break;

            default:
                Process_SM_Ptr[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex][uc_AuxCh] = SAFE;
                break;
            }

        }
    }
    return 1;
}

//报错和恢复
//通过can parse 向恰当的IVMCU的ＩＶ报告出现ＵＮＳＡＦＥ状态
//有待重写
unsigned char Aux_Safety_Check::sendError(unsigned char uc_IVunitBufIndex, unsigned char uc_IVch,unsigned char uc_AuxType,unsigned char uc_AuxCh)
{
    unsigned char IVMCU_ID ;
    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(uc_AuxType);
    if(TempAuxTypeIdex == 0xFF)//辅助限制
        return 1;
    if(uc_AuxType==AUX_SAFETY)
    {
        IVMCU_ID = Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Source_CANID;
        if((Aux_DataManager::m_AuxDataBank[1][8+uc_AuxCh].updated_aux_data.m_Char & 0x02))
        {
            CANBusParse::Aux_RptUnsafe(IVMCU_ID, uc_IVch,uc_AuxCh,AUX_SAFETY,OnlineRSMVol_UNSAFE,st_OnlineRSM_UnsafeData[uc_IVunitBufIndex][uc_IVch].f_SafetyVoltage[uc_AuxCh]);
        }
        if((Aux_DataManager::m_AuxDataBank[1][8+uc_AuxCh].updated_aux_data.m_Char & 0x04))
        {
            CANBusParse::Aux_RptUnsafe(IVMCU_ID, uc_IVch,uc_AuxCh,AUX_SAFETY,OnlineRSMCur_UNSAFE,st_OnlineRSM_UnsafeData[uc_IVunitBufIndex][uc_IVch].f_SafetyCurrent[uc_AuxCh]);
        }
        if((Aux_DataManager::m_AuxDataBank[1][8+uc_AuxCh].updated_aux_data.m_Char & 0x08))
        {
            CANBusParse::Aux_RptUnsafe(IVMCU_ID, uc_IVch,uc_AuxCh,AUX_SAFETY,OnlineRSMTemp_UNSAFE,st_OnlineRSM_UnsafeData[uc_IVunitBufIndex][uc_IVch].f_SafetyTemperature[uc_AuxCh]);
        }
    }
    else
    {
        IVMCU_ID = Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Source_CANID;
        CANBusParse::Aux_RptUnsafe(IVMCU_ID, uc_IVch,uc_AuxCh,uc_AuxType,AUX_UNSAFE_TOIV,st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].f_record[uc_AuxCh]);
    }
    return 1;
}

//向IV报告回复到ＳＡＦＥ状态
unsigned char Aux_Safety_Check::resumeError(unsigned char uc_IVunitBufIndex,unsigned char uc_IVch,unsigned char uc_AuxType,unsigned char uc_AuxCh)
{
    unsigned char IVMCU_ID;
    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(uc_AuxType);
    if(TempAuxTypeIdex == 0xFF)//辅助限制
        return 1;
    IVMCU_ID = Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Source_CANID;
    CANBusParse::Aux_RptUnsafe(IVMCU_ID,uc_IVch,uc_AuxCh,uc_AuxType,AUX_SAFE,0);  //回复到安全状态

    return 1;
}

//极其简单的比较时间差函数...
//一般都是ms级别的，所以没必要比较s级别
//comment out chen 20130812  need modify    <<<LATER>>>
MsTime Aux_Safety_Check::TimeT(MsTime a,MsTime b)    // a<b
{
    MsTime tempDiff;
    signed short temp;

    tempDiff.Second = b.Second - a.Second;
    temp = b.Us100 - a.Us100;
    if(temp < 0)
    {
        temp = temp + 10000;
        tempDiff.Second --;
    }
    tempDiff.Us100 = temp;
    return tempDiff;
}

//RedundantVoltage:
bool Aux_Safety_Check::SetSafetyInfo(const St_Uart_SetSafety *safetyInfo)
{
    unsigned char IVChnIdx = safetyInfo->m_IVChnNo;
    if(IVChnIdx>=M4_MAX_CHANNELNO)
        return false;
    st_IV2M4ADCChnMap.m_IVChnNo[safetyInfo->m_M4ADCChnIdx] = IVChnIdx;  //set map
    st_IV2M4ADCChnMap.m_bValid[safetyInfo->m_M4ADCChnIdx] = 1;
    if( safetyInfo->m_SafetyH>=999 &&  safetyInfo->m_SafetyL<=-999)
        AuxRedundantSafetyCheck::st_RedundantSafetyCheck[IVChnIdx].m_bReleaseCheck = true;
    AuxRedundantSafetyCheck::st_RedundantSafetyCheck[IVChnIdx].m_SafetyH = safetyInfo->m_SafetyH;
    AuxRedundantSafetyCheck::st_RedundantSafetyCheck[IVChnIdx].m_SafetyL = safetyInfo->m_SafetyL;
    AuxRedundantSafetyCheck::st_RedundantSafetyCheck[IVChnIdx].m_SafetyValid = true;
    AuxRedundantSafetyCheck::st_RedundantSafetyCheck[IVChnIdx].m_safeStatus = en_Safe;  //DKQ 03.11.2015
    return true;
    //st_SafetyCheck
}
void Aux_Safety_Check::OnlineRSMSafetyCheck(unsigned char uc_IVunitBufIndex, unsigned char uc_IVch)
{
    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(AUX_SAFETY);
    if(TempAuxTypeIdex == 0xFF)//辅助限制
        return;

    if(!Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_HIGH_END]&&
            !Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_LOW_END])
        return;


//以下代码处理辅助安全类的检测
    if(Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_Voltage_HIGH]&&
            Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_Voltage_LOW])   //如果该类型的辅助通道设了安全上限下限
    {
        for (unsigned char i =0; i< Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Counts_ByScheduleMap; i++) //检查该类型的所通道
            //找出该IV该类型辅助的所有通道进行轮询	,
        {
            ST_AUX_DATA  st_auxdata;
            unsigned char uc_AuxCh = Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_MapInfo[i].m_Index_InAuxUnit;
            unsigned char uc_ch= uc_AuxCh*2+1;
            st_auxdata =Aux_DataManager::m_AuxDataBank[0][uc_ch];
            float f_PresentValue = st_auxdata.aux_data;
            //  f_PresentValue = 11;
            //[2] 相当于low   [3]相当于high
            if( (f_PresentValue >= Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safe_value[AUX_SAFETY_Voltage_HIGH]  )
                    || (f_PresentValue <= Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safe_value[AUX_SAFETY_Voltage_LOW])	)
            {
                Aux_DataManager::m_AuxDataBank[1][8+uc_AuxCh].updated_aux_data.m_Char |=(1<<1);

                st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[0] |= ((U64)(1)<<uc_AuxCh);
//
                st_OnlineRSM_UnsafeData[uc_IVunitBufIndex][uc_IVch].f_SafetyVoltage[uc_AuxCh]=f_PresentValue;

            }
            else
            {
                Aux_DataManager::m_AuxDataBank[1][8+uc_AuxCh].updated_aux_data.m_Char &=~(1<<1);
            }

        }
    }
    if(Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_Current_HIGH]&&
            Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_Current_LOW])   //如果该类型的辅助通道设了安全上限下限
    {
        for (unsigned char i =0; i< Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Counts_ByScheduleMap; i++) //检查该类型的所通道
            //找出该IV该类型辅助的所有通道进行轮询	,
        {
            ST_AUX_DATA  st_auxdata;
            unsigned char uc_AuxCh = Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_MapInfo[i].m_Index_InAuxUnit;
            unsigned char uc_ch= uc_AuxCh*2;
            st_auxdata =Aux_DataManager::m_AuxDataBank[0][uc_ch];
            float f_PresentValue = st_auxdata.aux_data;

            //[2] 相当于low   [3]相当于high
            if( (f_PresentValue >= Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safe_value[AUX_SAFETY_Current_HIGH]  )
                    || (f_PresentValue <= Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safe_value[AUX_SAFETY_Current_LOW])	)
            {
                Aux_DataManager::m_AuxDataBank[1][8+uc_AuxCh].updated_aux_data.m_Char |=(1<<2);;
                st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[0] |= ((U64)(1)<<uc_AuxCh);
                st_OnlineRSM_UnsafeData[uc_IVunitBufIndex][uc_IVch].f_SafetyCurrent[uc_AuxCh]=f_PresentValue;

            }
            else
            {
                Aux_DataManager::m_AuxDataBank[1][8+uc_AuxCh].updated_aux_data.m_Char &=~(1<<2);
            }
        }
    }
    if(Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_Temperature_HIGH]&&
            Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_Temperature_LOW])   //如果该类型的辅助通道设了安全上限下限
    {
        for (unsigned char i =0; i< Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Counts_ByScheduleMap; i++) //检查该类型的所通道
            //找出该IV该类型辅助的所有通道进行轮询	,
        {
            ST_AUX_DATA  st_auxdata;
            unsigned char uc_AuxCh = Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_MapInfo[i].m_Index_InAuxUnit;
            unsigned char uc_ch= uc_AuxCh;
            st_auxdata =Aux_DataManager::m_AuxDataBank[1][uc_ch]; //0改成1,温度在第二group
            float f_PresentValue = st_auxdata.aux_data;

            //[2] 相当于low   [3]相当于high
            if( (f_PresentValue >= Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safe_value[AUX_SAFETY_Temperature_HIGH]  )
                    || (f_PresentValue <= Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safe_value[AUX_SAFETY_Temperature_LOW])	)
            {
                Aux_DataManager::m_AuxDataBank[1][8+uc_AuxCh].updated_aux_data.m_Char |=(1<<3);;
                st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[0] |= ((U64)(1)<<uc_AuxCh);
                st_OnlineRSM_UnsafeData[uc_IVunitBufIndex][uc_IVch].f_SafetyTemperature[uc_AuxCh]=f_PresentValue;

            }
            else
            {
                Aux_DataManager::m_AuxDataBank[1][8+uc_AuxCh].updated_aux_data.m_Char &=~(1<<3);
            }
        }
    }


    for (unsigned char i =0; i< Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_Counts_ByScheduleMap; i++)
    {
        unsigned char uc_AuxCh = Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_MapInfo[i].m_Index_InAuxUnit;
        if(Aux_DataManager::m_AuxDataBank[1][8+uc_AuxCh].updated_aux_data.m_Char ==0)
            st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[0] &= ~((U64)(1)<<uc_AuxCh);
    }

}
//判断在线监控状态;未使用，监控，保护三种状态
void Aux_Safety_Check::OnlineRSMStateScan(unsigned char uc_IVunitBufIndex, unsigned char uc_IVch)
{
    unsigned char TempAuxTypeIdex = Aux_DataManager::Aux_LocateAuxTypeArrayIndex(AUX_SAFETY);
    if(TempAuxTypeIdex == 0xFF)//辅助限制
        return;

    unsigned char uc_AuxCh = Aux_DataManager::m_MapFromPC[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_MapInfo[0].m_Index_InAuxUnit;
    switch(MonitorSafetyLedState[uc_IVunitBufIndex][uc_IVch])
    {
    case NoUse:
        if(Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_HIGH_END]&&
                Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_LOW_END])
        {
            Led_State[uc_AuxCh]= Monitoring;
            MonitorSafetyLedState[uc_IVunitBufIndex][uc_IVch] = Monitoring;
        }
        break;
    case  Monitoring:
        if((st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[0]!=0)&&
                (!Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_HIGH_END]&&
                 !Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_LOW_END]))
        {
            Led_State[uc_AuxCh] = Protect;
            MonitorSafetyLedState[uc_IVunitBufIndex][uc_IVch] = Protect;
            LED_ProtectTime = CommonFunc::GetSystemTime();
        }
        else if(!Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_HIGH_END]&&
                !Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_LOW_END])
        {
            Led_State[uc_AuxCh] = NoUse;
            MonitorSafetyLedState[uc_IVunitBufIndex][uc_IVch] = NoUse;
        }
        break;
    case Protect:
        if((st_check_result[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].NotSafeFlag[0]==0)&&
                (Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_HIGH_END]&&
                 Aux_DataManager::m_AuxSafetyBank[uc_IVunitBufIndex][uc_IVch][TempAuxTypeIdex].m_safety_alive[AUX_SAFETY_LOW_END]))
        {
            Led_State[uc_AuxCh] = Monitoring;
            MonitorSafetyLedState[uc_IVunitBufIndex][uc_IVch] = Monitoring;
        }
        if(CommonFunc::TimeDiff(LED_ProtectTime).Second>60)
        {
            Led_State[uc_AuxCh] = NoUse;
            MonitorSafetyLedState[uc_IVunitBufIndex][uc_IVch] = NoUse;
        }
        break;

    }

}

