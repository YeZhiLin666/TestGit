///////////////////////////////////////////////////////////////////////////////////
//File Name: PULSE_Safety.cpp
//Version:        V1.0
//Date:                2014/8/21
//Description:
//           脉冲安全检查任务，控制值异常只检查电流，除了Mv的电流电压之外还应和台阶
//			 数据m_Pulse_Stage_Data比较，任何一个Unsafe了就响应
//			 合理性检查暂时保留
//Others:
//History：

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"


UInt16                  PULSE_safety_check::Priority = PRIORITY4;
UInt16                  PULSE_safety_check::TaskID = PULSE_SAFETY_TASK;			// should be a definition which positioning this task in OS_Task[]
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
MsTime                  PULSE_safety_check::safety_last_time[MAXCHANNELNO];
// MsTime                  PULSE_safety_check::safety_error_time[MAXCHANNELNO];
MsTime                  PULSE_safety_check::safety_resume_time[MAXCHANNELNO];

unsigned char		PULSE_safety_check::Pulse_UnsafeType[MAXCHANNELNO];


ST_PULSE_SAFETYCHECK 	PULSE_safety_check::Pulse_SafetyCheck[MAXCHANNELNO];


//////////////////creat buf for test
//PULSE_RAW_DATA test_buf[2][2][30];

#pragma arm section //结束外存变量定义
// unsigned char  PULSE_safety_check::RedundantSafetyFlg[MAXCHANNELNO];
// char                    PULSE_safety_check::PULSE_Safety_Check_status[MAXCHANNELNO];
unsigned long           PULSE_safety_check::ErrorFlag[MAXCHANNELNO];    //safety标志   bit 0 脉冲数据  bit 1 普通检查
// unsigned char           PULSE_safety_check::safety_chn;
// unsigned char           PULSE_safety_check::I_Sample_Flag[MAXCHANNELNO];
// unsigned char           PULSE_safety_check::V_Sample_Flag[MAXCHANNELNO];
// St_CtrlValueCheck       PULSE_safety_check::CtrlValueCheck[MAXCHANNELNO];
// St_IrregularCheck       PULSE_safety_check::IrregularCheck[MAXCHANNELNO];
// St_CtrlValueSetting     PULSE_safety_check::m_CtrlValueSetting;
// St_IrregularSetting     PULSE_safety_check::m_IrregularSetting;
unsigned char           PULSE_safety_check::Process_SM_Ptr[MAXCHANNELNO];	// process() status machine ptr.
float                   PULSE_safety_check::I_Record[MAXCHANNELNO];		// used for recording error values. each channel should has it's own buffer because
float                   PULSE_safety_check::V_Record[MAXCHANNELNO];		// - sometimes there's no new value for this channel, but there's a timeout  triggered
float                   PULSE_safety_check::P_Record[MAXCHANNELNO];		// - unsafe and if only one variable, it could be updated by other channels value already.
float                   PULSE_safety_check::C_Record[MAXCHANNELNO];		// - unsafe and if only one variable, it could be updated by other channels value already.
float                   PULSE_safety_check::t_Record[MAXCHANNELNO];		// - unsafe and if only one variable, it could be updated by other channels value already.
// unsigned char           PULSE_safety_check:: b_ClearError[MAXCHANNELNO];
float                   PULSE_safety_check::UD_Record[MAXCHANNELNO];                           //170122
int                   PULSE_safety_check::UD_UnsafeIndex[MAXCHANNELNO];                      //170122

PULSE_RAW_DATA		*CheckPulseSafetyBuf1_ptr;
PULSE_RAW_DATA		*CheckPulseSafetyBuf2_ptr;
PULSE_RAW_DATA		*BeCheckedPulseSafetyBuf_ptr;

unsigned char Unsafetype;
unsigned char PulseSafetyErrorflg;

///////////////////////////////////

void PULSE_safety_check::Init(void)
{
    //赋0
    for(unsigned char i = 0; i<MAXCHANNELNO; i++)
    {
// 		PULSE_Safety_Check_status[i] = 0;    //状态机
        ErrorFlag[i] = 0;                   //错误标志
        Process_SM_Ptr[i] = SAFE;

        //may be wrong  因为不清楚是board config先运行还是这个先运行
        ChannelInfo_Manager::GetSystemSafty(i,MP_SYS_SAFTY_TYPE_CURRENT,ChannelInfo_Manager::CurrRngPhysicalRange1,& safety_check::Check_S[i].checkSYS_I);
        ChannelInfo_Manager::GetSystemSafty(i,MP_SYS_SAFTY_TYPE_VOLTAGE,VOLTAGE_RANGE_HIGH,& safety_check::Check_S[i].checkSYS_V);
        ChannelInfo_Manager::GetSystemSafty(i,MP_SYS_SAFTY_TYPE_POWER,0,& safety_check::Check_S[i].checkSYS_P);

        //采样刷新标志
// 		I_Sample_Flag[i] = 0;
// 		V_Sample_Flag[i] = 0;


        //记录错误
        I_Record[i] = 0;
        V_Record[i] = 0;
        P_Record[i] = 0;
        C_Record[i] = 0;
        t_Record[i] = 0;
// 		RedundantSafetyFlg[i]=0;
// 		b_ClearError[i]=0;
// 		Pulse_SafetyCheck[i].UnsafeCount=0;
// 		Pulse_SafetyCheck[i].CheckCount=0;
// 		Pulse_SafetyCheck[i].RecordCount=0;
// 		Pulse_SafetyCheck[i].CheckStageID=1;
// 		Pulse_SafetyCheck[i].UnsafeStageCount=0;
// 		Pulse_SafetyCheck[i].BufOffset=0;
        memset(Pulse_SafetyCheck,0,sizeof(ST_PULSE_SAFETYCHECK));
        Pulse_SafetyCheck[i].CheckStageID=1;
    }

// 	safety_chn = 0;

// 	if(m_CtrlValueSetting.fErrorFactor <= 0)
// 		m_CtrlValueSetting.fErrorFactor = DEF_ACCURACY * DEF_CTRL_VALUE_ERROR_FACTOR;


// 	memset(CtrlValueCheck,0,sizeof(CtrlValueCheck));
// 	memset(IrregularCheck,0,sizeof(IrregularCheck));
// 	m_CtrlValueSetting.bChecking  = 0;
// 	m_CtrlValueSetting.NextTime   = OS_ServeProcess::OS_Time;
// 	m_IrregularSetting.bChecking  = 0;
// 	m_IrregularSetting.NextTime   = OS_ServeProcess::OS_Time;





    CheckPulseSafetyBuf1_ptr=&Pulse::raw_data[0][0][0];
    CheckPulseSafetyBuf2_ptr=&Pulse::raw_data[1][0][0];

    //   	CheckPulseSafetyBuf1_ptr=&test_buf[0][0][0];  //test buf check
    //  	CheckPulseSafetyBuf2_ptr=&test_buf[1][0][0];

    // 	for(unsigned char i=0;i<2;i++)
    // 	{
    // 		for(unsigned char j=0;j<2;j++)
    // 		{
    // 			for(unsigned char k=0;k<2;k++)
    // 			{
    // 				test_buf[i][j][k].m_bDataConverted=1;
    // 				test_buf[i][j][k].m_bSafetyChecked=0;
    // 				test_buf[i][j][k].m_bSampleUpdated=1;
    // 				test_buf[i][j][k].m_fCurrent=1;
    // 				test_buf[i][j][k].m_fVoltage=2;
    // 				test_buf[i][j][k].m_uStageNum=k;
    // 			}
    // 			for(unsigned char k=2;k<30;k++)
    // 			{
    // 				test_buf[i][j][k].m_bDataConverted=0;
    // 				test_buf[i][j][k].m_bSafetyChecked=0;
    // 				test_buf[i][j][k].m_bSampleUpdated=0;
    // 				test_buf[i][j][k].m_fCurrent=1;
    // 				test_buf[i][j][k].m_fVoltage=2;
    // 				test_buf[i][j][k].m_uStageNum=k;
    // 			}
    // 		}
    // 	}

    BeCheckedPulseSafetyBuf_ptr=CheckPulseSafetyBuf1_ptr;


}

//如果我们想知道在这时间段内  超出limit的具体是什么
//则要增加几个error counter来计数
//算法描述：开始记录一个时间点last，然后check
//如果警报则 记录当前时间为error时间，如果error时间与last超过一定时间，则说明这段时间内都会出limit，这时切断通道
//如果没有警报则移动last时间点到当前时间
//comment out chen 20130531    把取数据拿到外面去
void PULSE_safety_check::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_SAFTY_PULSE] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_SAFTY_PULSE);
#endif
    uint16 checkitem;


    checkitem =V_CHECK | PI_CHECK;
    check(0,checkitem);
// 	b_ClearError[0]=0;


// 	ClearSchUnsafeIfNotRunning(0);
    StatusProcess(0, checkitem);		// 根据状态处理相关结果

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_SAFTY_PULSE] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time(COUNT_SAFTY_PULSE);
#endif
}

// void PULSE_safety_check::EndTask(void)
// {

// }

void PULSE_safety_check::SetUnsafe(unsigned char chn, uint32 unsafeType, float value)
{   // each time only one unsafe type is set, and meantime, data recorded.
    //ErrorFlag标志位    I_Record等用来记录
    //ErrorFlag 须与上位机匹配
    ErrorFlag[chn] |= unsafeType;
    switch(unsafeType)		// record data here
    {
    case CURRENT_SCH_UNSAFE:
    case CURRENT_SYS_UNSAFE:
        I_Record[chn] = value;
        break;
    case VOLTAGE_SCH_UNSAFE:
    case VOLTAGE_SYS_UNSAFE:
        V_Record[chn] = value;
        break;
    case POWER_SCH_UNSAFE:
    case POWER_SYS_UNSAFE:
        P_Record[chn] = value;
        break;
    case SCH_UserDefined_UNSAFE:    //170122
        UD_Record[chn] = value;
        break;
    case CAPACITY_SCH_UNSAFE:
        C_Record[chn] = value;
        break;
    case STEPTIME_SCH_UNSAFE:
        t_Record[chn] = value;
        break;
    }
}
// void inline PULSE_safety_check::ClrUnsafe(unsigned char chn, uint16 unsafeType)
// {
// 	if(	b_ClearError[chn])   // Can't clear the Error flag ,when Pulse status.      dirui 20140902
// 		ErrorFlag[chn] &= ~unsafeType;
// }
void PULSE_safety_check::check(unsigned char chn, uint16 checkItem)
{
    float current, voltage, power;
    unsigned char type = PULSE_safety_check_sch_is_ready(chn);
    unsigned char m_ActiveBuf=0;   //记录当前buf数，用于检查当前循环数
    Unsafetype = 0;
    PulseSafetyErrorflg=0;
    ////////////////////////////普通检查 每次都要做
    if(type==SAFETY_CHECK_SCH)
    {
        PulseSafetyErrorflg=0;
        current = ChannelInfo_Manager::Get_MetaVariable(chn, MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, MetaCode_PV_Current);
        voltage = ChannelInfo_Manager::Get_MetaVariable(chn, MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, MetaCode_PV_Voltage);
        power   = current*voltage;
        if((checkItem & PI_CHECK) != 0)    //P and I
        {

// 			if(safety_check::checkSCH_I[chn].m_bIfCheck == 1)
// 			{
// 				if((current > safety_check::checkSCH_I[chn].m_fHigh )|| (current < safety_check::checkSCH_I[chn].m_fLow))
// 				{
// 					PulseSafetyErrorflg=1;
// 					SetUnsafe(chn, CURRENT_SCH_UNSAFE, current);
// 				}
// 			}
            BeNormalSafetyPIValueCheck(chn,current,power);


        }
// 		else
// 		{
// 			if((safety_check::checkSYS_I[chn].m_bIfCheck == 1) && (current> safety_check::checkSYS_I[chn].m_fHigh || current < safety_check::checkSYS_I[chn].m_fLow))
// 			{
// 				PulseSafetyErrorflg=1;
// 				SetUnsafe(chn, CURRENT_SYS_UNSAFE, current);
// 				Unsafetype=CURRENT_SYS_UNSAFE;
// 			}
        NormalSafetyValueCheck(chn,CURRENT_SYS_UNSAFE,current);
// 		}

        if((checkItem & V_CHECK) != 0)
        {
// 			if((checkItem & V_CHECK) != 0)
// 			{
// 				if((voltage > safety_check::checkSCH_V[chn].m_fHigh )|| (voltage < safety_check::checkSCH_V[chn].m_fLow))
// 				{
// 					PulseSafetyErrorflg=1;
// 					SetUnsafe(chn, VOLTAGE_SCH_UNSAFE, voltage);
// 				}
// 			}
            NormalSafetyValueCheck(chn,VOLTAGE_SCH_UNSAFE,voltage);
// 			if((safety_check::checkSYS_V[chn].m_bIfCheck == 1) && (voltage > safety_check::checkSYS_V[chn].m_fHigh || voltage < safety_check::checkSYS_V[chn].m_fLow))
// 			{
// 				PulseSafetyErrorflg=1;
// 				Unsafetype=VOLTAGE_SYS_UNSAFE;
// 				SetUnsafe(chn, VOLTAGE_SYS_UNSAFE, voltage);
// 			}
        }
// 		else
// 		{
// 			if((safety_check::checkSYS_V[chn].m_bIfCheck == 1) && (voltage > safety_check::checkSYS_V[chn].m_fHigh || voltage < safety_check::checkSYS_V[chn].m_fLow))
// 			{
// 				PulseSafetyErrorflg=1;
// 				Unsafetype=VOLTAGE_SYS_UNSAFE;
// 				SetUnsafe(chn, VOLTAGE_SYS_UNSAFE, voltage);
// 			}
        NormalSafetyValueCheck(chn,VOLTAGE_SYS_UNSAFE,voltage);
// 		}
        if(PulseSafetyErrorflg)
        {
            Pulse_SafetyCheck[chn].UnsafeCount++; //异常累加
            Pulse_UnsafeType[chn]=UNSAFE_COUNTER;
        }

        //170122 start
        St_Safety_UD  *    pUDsafety    =    &ChannelInfo_Manager::ChannelSchedule[chn].m_Settings.m_Safety_UD[0];
        for(int UD_Idx = 0; UD_Idx < MAX_USERDEFINED_SAFETY; UD_Idx++)
        {
            if(pUDsafety->m_bValid)
            {
                float fValue = ChannelInfo_Manager::Get_MetaVariable(chn,pUDsafety->m_DataType,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT,pUDsafety->m_MetaCode);
                if(fValue > pUDsafety->m_fHigh || fValue < pUDsafety->m_fLow)
                {
                    SetUnsafe(chn,SCH_UserDefined_UNSAFE,fValue);
                    UD_UnsafeIndex[chn] = UD_Idx;
                    break;
                }
            }
//       else
//         ClrUnsafe(chn, SCH_UserDefined_UNSAFE);
            pUDsafety++;
        } // 170122 end
    }

    ///////////////////////////////////////////以下为脉冲数据Buf检查
    ST_PULSE_SAFETYCHECK *pSafety = &Pulse_SafetyCheck[chn];
    for(unsigned char i=0; i<30; i++) //每次只检查30个数据，节约资源
    {
        PulseSafetyErrorflg=0;//清除错误标记
        if(pSafety->CheckStageID==Pulse::m_Info.m_StageCount) //若已经查完一个Cycle了要换Buf
        {
            if(m_ActiveBuf)
                pSafety->BufOffset=0;  //buf0首偏移量
            else
                pSafety->BufOffset=60;  //buf1首偏移量

            pSafety->CheckStageID=1;  //重新记录Stage数
        }

        if((pSafety->CheckCount%120)==0)
            pSafety->BufOffset=0;  //两个buf都查完了

        if(pSafety->BufOffset>59)
        {
            if((pSafety->CheckCount>0)&&((pSafety->CheckCount&1)==1))//第一次查完了，下次进入还得从首点数组开始，否则就只查找尾点了
                BeCheckedPulseSafetyBuf_ptr=CheckPulseSafetyBuf2_ptr+((pSafety->BufOffset-60)/2)+30;
            else
                BeCheckedPulseSafetyBuf_ptr=CheckPulseSafetyBuf2_ptr+((pSafety->BufOffset-60)-1)/2;
            m_ActiveBuf=1;
        }
        else
        {
            if((pSafety->CheckCount>0)&&((pSafety->CheckCount&1)==1))//第一次查完了，下次进入还得从首点数组开始，否则就只查找尾点了
                BeCheckedPulseSafetyBuf_ptr=CheckPulseSafetyBuf1_ptr+(pSafety->BufOffset/2)+30;
            else
            {
                if(pSafety->BufOffset>0)
                    BeCheckedPulseSafetyBuf_ptr=CheckPulseSafetyBuf1_ptr+(pSafety->BufOffset-1)/2;
                else
                    BeCheckedPulseSafetyBuf_ptr=CheckPulseSafetyBuf1_ptr;
            }
            m_ActiveBuf=0;
        }

        if(BeCheckedPulseSafetyBuf_ptr->m_bSafetyChecked)
        {
            pSafety->BufOffset++;
            continue;
        }

        if(!BeCheckedPulseSafetyBuf_ptr->m_bDataConverted)  //是否是有效值
        {
            if((BeCheckedPulseSafetyBuf_ptr==(&Pulse::raw_data[m_ActiveBuf][0][0]+Pulse::m_Info.m_StageCount)) \
                    ||  (BeCheckedPulseSafetyBuf_ptr==(&Pulse::raw_data[m_ActiveBuf][0][1]+Pulse::m_Info.m_StageCount))  \
                    ||  (BeCheckedPulseSafetyBuf_ptr==(&Pulse::raw_data[m_ActiveBuf][1][0]+Pulse::m_Info.m_StageCount))  \
                    ||  (BeCheckedPulseSafetyBuf_ptr==(&Pulse::raw_data[m_ActiveBuf][1][1]+Pulse::m_Info.m_StageCount))  )    //一个循环的最后一个点
            {
                m_ActiveBuf=!m_ActiveBuf;
                if(m_ActiveBuf)
                    pSafety->BufOffset=60;
                else
                    pSafety->BufOffset=0;
                return;
            }
            if(pSafety->CheckStageID<Pulse::m_Info.m_StageCount)  //如果数据没有刷新完毕不检查了
                continue;
        }
        else  //数据有效的话
        {
            BeCheckedPulseSafetyBuf_ptr->m_bSafetyChecked=1;
            pSafety->CheckCount++;  //计数累加

            if(type==SAFETY_CHECK_SCH)
            {
                if((checkItem & PI_CHECK) != 0)    //P and I
                {
                    power=BeCheckedPulseSafetyBuf_ptr->m_fCurrent*BeCheckedPulseSafetyBuf_ptr->m_fVoltage;
// 					if(safety_check::checkSCH_I[chn].m_bIfCheck == 1)
// 					{
// 						if ((BeCheckedPulseSafetyBuf_ptr->m_fCurrent> safety_check::checkSCH_I[chn].m_fHigh)
// 							||(BeCheckedPulseSafetyBuf_ptr->m_fCurrent< safety_check::checkSCH_I[chn].m_fLow))        //检查上下限
// 						{
// 							PulseSafetyErrorflg=1;
// 							Unsafetype=CURRENT_SCH_UNSAFE;
// 							SetUnsafe(chn, CURRENT_SCH_UNSAFE, BeCheckedPulseSafetyBuf_ptr->m_fCurrent);
// 						}
// 					}
                    BeNormalSafetyPIValueCheck(chn,current,power);
                }
                if((checkItem & V_CHECK) != 0)    //V
                {
// 					if(safety_check::checkSCH_V[chn].m_bIfCheck==1)
// 					{
// 						if ((BeCheckedPulseSafetyBuf_ptr->m_fVoltage> safety_check::checkSCH_V[chn].m_fHigh)||(BeCheckedPulseSafetyBuf_ptr->m_fVoltage<safety_check::checkSCH_V[chn].m_fLow)) //检查上下限
// 						{
// 							PulseSafetyErrorflg=1;
// 							Unsafetype=VOLTAGE_SCH_UNSAFE;
// 							SetUnsafe(chn, VOLTAGE_SCH_UNSAFE,BeCheckedPulseSafetyBuf_ptr->m_fVoltage);
// 						}
// 					}
                    NormalSafetyValueCheck(chn,VOLTAGE_SCH_UNSAFE,BeCheckedPulseSafetyBuf_ptr->m_fVoltage);
                }
            }
            else
            {
// 				if((safety_check::checkSYS_I[chn].m_bIfCheck == 1) && (BeCheckedPulseSafetyBuf_ptr->m_fCurrent> safety_check::checkSYS_I[chn].m_fHigh || BeCheckedPulseSafetyBuf_ptr->m_fCurrent < safety_check::checkSYS_I[chn].m_fLow))
// 				{
// 					PulseSafetyErrorflg=1;
// 					SetUnsafe(chn, CURRENT_SYS_UNSAFE, BeCheckedPulseSafetyBuf_ptr->m_fCurrent);
// 					Unsafetype=CURRENT_SYS_UNSAFE;
// 				}
                NormalSafetyValueCheck(chn,CURRENT_SYS_UNSAFE,BeCheckedPulseSafetyBuf_ptr->m_fCurrent);
// 				if((safety_check::checkSYS_V[chn].m_bIfCheck == 1) && (BeCheckedPulseSafetyBuf_ptr->m_fVoltage > safety_check::checkSYS_V[chn].m_fHigh || BeCheckedPulseSafetyBuf_ptr->m_fVoltage < safety_check::checkSYS_V[chn].m_fLow))
// 				{
// 					PulseSafetyErrorflg=1;
// 					Unsafetype=VOLTAGE_SYS_UNSAFE;
// 					SetUnsafe(chn, VOLTAGE_SYS_UNSAFE, BeCheckedPulseSafetyBuf_ptr->m_fVoltage);
// 				}
                NormalSafetyValueCheck(chn,VOLTAGE_SYS_UNSAFE,BeCheckedPulseSafetyBuf_ptr->m_fVoltage);
            }

            if(PulseSafetyErrorflg)
            {
                Pulse_SafetyCheck[chn].UnsafeCount++; //异常累加
                Pulse_UnsafeType[chn]=UNSAFE_COUNTER;
                //继续检查其他两种异常
                if(pSafety->UnsafeCycleID!= Pulse::buf_info[m_ActiveBuf].m_CycleIdx)//若没有被记录
                {
                    if(pSafety->UnsafeCycleID==(Pulse::buf_info[m_ActiveBuf].m_CycleIdx-1))
                        pSafety->UnsafeCycleCount++;//循环数累加
                    else
                        pSafety->UnsafeCycleCount=0;  //不是连续循环就不累加了
                    pSafety->UnsafeCycleID= Pulse::buf_info[m_ActiveBuf].m_CycleIdx;
                    pSafety->UnsafeStageCount=0;  //重新记录Stage异常
                    if(Pulse_SafetyCheck[chn]. UnsafeCycleCount>=2)  //第一次已经给统计到了，所以记录两次就可以了
                    {
                        //…若三次超限报告Unsafe并退出检查
                        switch(Unsafetype)
                        {
                        case CURRENT_SYS_UNSAFE:
                        case CURRENT_SCH_UNSAFE:
                            SetUnsafe(chn, Unsafetype, BeCheckedPulseSafetyBuf_ptr->m_fCurrent);
                            break;
                        case VOLTAGE_SYS_UNSAFE:
                        case VOLTAGE_SCH_UNSAFE:
                            SetUnsafe(chn, Unsafetype, BeCheckedPulseSafetyBuf_ptr->m_fVoltage);
                            break;
                        case POWER_SCH_UNSAFE:
                            SetUnsafe(chn, Unsafetype, power);
                            break;
                        default:
                            break;
                        }

                        Pulse_UnsafeType[chn]=UNSAFE_CYCLE;
                        return;   //处理完毕直接退出
                    }
                }
                else if(pSafety->UnsafeCycleID==Pulse::buf_info[m_ActiveBuf].m_CycleIdx)//一个循环之内要检查Stage是否达到异常标准
                {
                    if((pSafety->CheckCount&1))  //一个Cycle之内有3个stage采样都超标
                    {   //如果是首点的话
                        pSafety->CheckCount++;  //计数累加
                        i++;//循环累加
                        BeCheckedPulseSafetyBuf_ptr+=30;//找尾点
                        pSafety->BufOffset++;
                        if(!BeCheckedPulseSafetyBuf_ptr->m_bDataConverted)  //不是有效值
                            continue;  // 不处理
                        BeCheckedPulseSafetyBuf_ptr->m_bSafetyChecked=1;
                        //以下对尾点查一遍
                        if(type==SAFETY_CHECK_SCH)
                        {
                            if((checkItem & PI_CHECK) != 0)    //P and I
                            {
                                power=BeCheckedPulseSafetyBuf_ptr->m_fCurrent*BeCheckedPulseSafetyBuf_ptr->m_fVoltage;
// 								if(safety_check::checkSCH_I[chn].m_bIfCheck == 1)
// 								{
// 									if ((BeCheckedPulseSafetyBuf_ptr->m_fCurrent> safety_check::checkSCH_I[chn].m_fHigh)||(BeCheckedPulseSafetyBuf_ptr->m_fCurrent<         \
// 										safety_check::checkSCH_I[chn].m_fLow))        //检查上下限
// 									{
// 										PulseSafetyErrorflg=1;
// 										Unsafetype=CURRENT_SCH_UNSAFE;
// 										SetUnsafe(chn, CURRENT_SCH_UNSAFE, BeCheckedPulseSafetyBuf_ptr->m_fCurrent);
// 									}
// 								}
                                NormalSafetyValueCheck(chn,CURRENT_SCH_UNSAFE,BeCheckedPulseSafetyBuf_ptr->m_fCurrent);
// 								if(safety_check::checkSCH_P[chn].m_bIfCheck == 1)
// 								{
// 									if ((power> safety_check::checkSCH_P[chn].m_fHigh)||(power<safety_check::checkSCH_P[chn].m_fLow)) //检查上下限
// 									{
// 										PulseSafetyErrorflg=1;
// 										Unsafetype=POWER_SCH_UNSAFE;
// 										SetUnsafe(chn, POWER_SCH_UNSAFE,power);
// 									}
// 								}
                                NormalSafetyValueCheck(chn,POWER_SCH_UNSAFE,power);
                            }

                            if((checkItem & V_CHECK) != 0)    //V
                            {
// 								if(safety_check::checkSCH_V[chn].m_bIfCheck==1)
// 								{
// 									if ((BeCheckedPulseSafetyBuf_ptr->m_fVoltage> safety_check::checkSCH_V[chn].m_fHigh)||(BeCheckedPulseSafetyBuf_ptr->m_fVoltage<safety_check::checkSCH_V[chn].m_fLow)) //检查上下限
// 									{
// 										PulseSafetyErrorflg=1;
// 										Unsafetype=VOLTAGE_SCH_UNSAFE;
// 										SetUnsafe(chn, POWER_SCH_UNSAFE,power);
// 									}
// 								}
                                NormalSafetyValueCheck(chn,VOLTAGE_SCH_UNSAFE,BeCheckedPulseSafetyBuf_ptr->m_fVoltage);
                            }
                        }
                        else
                        {
// 							if((safety_check::checkSYS_I[chn].m_bIfCheck == 1) && (BeCheckedPulseSafetyBuf_ptr->m_fCurrent> safety_check::checkSYS_I[chn].m_fHigh || BeCheckedPulseSafetyBuf_ptr->m_fCurrent < safety_check::checkSYS_I[chn].m_fLow))
// 							{
// 								PulseSafetyErrorflg=1;
// 								SetUnsafe(chn, CURRENT_SYS_UNSAFE, BeCheckedPulseSafetyBuf_ptr->m_fCurrent);
// 								Unsafetype=CURRENT_SYS_UNSAFE;
// 							}
                            NormalSafetyValueCheck(chn,CURRENT_SYS_UNSAFE,BeCheckedPulseSafetyBuf_ptr->m_fCurrent);
// 							if((safety_check::checkSYS_V[chn].m_bIfCheck == 1) && (BeCheckedPulseSafetyBuf_ptr->m_fVoltage > safety_check::checkSYS_V[chn].m_fHigh || BeCheckedPulseSafetyBuf_ptr->m_fVoltage < safety_check::checkSYS_V[chn].m_fLow))
// 							{
// 								PulseSafetyErrorflg=1;
// 								Unsafetype=VOLTAGE_SYS_UNSAFE;
// 								SetUnsafe(chn, VOLTAGE_SYS_UNSAFE, voltage);
// 							}
                            NormalSafetyValueCheck(chn,VOLTAGE_SYS_UNSAFE,BeCheckedPulseSafetyBuf_ptr->m_fVoltage);
                        }
                    }

                    if(PulseSafetyErrorflg)//尾点超限
                    {
                        pSafety->UnsafeCount++; //异常累加
                        pSafety->UnsafeStageCount++;//stage超限记录一次
                    }
                    if(pSafety->UnsafeStageCount>=3)
                    {   //…若3次Stage超限制则报告Unsafe
                        switch(Unsafetype)
                        {
                        case CURRENT_SYS_UNSAFE:
                        case CURRENT_SCH_UNSAFE:
                            SetUnsafe(chn, Unsafetype, BeCheckedPulseSafetyBuf_ptr->m_fCurrent);
                            break;
                        case VOLTAGE_SYS_UNSAFE:
                        case VOLTAGE_SCH_UNSAFE:
                            SetUnsafe(chn, Unsafetype, BeCheckedPulseSafetyBuf_ptr->m_fVoltage);
                            break;
                        case POWER_SCH_UNSAFE:
                            SetUnsafe(chn, Unsafetype, power);
                            break;
                        default:
                            break;
                        }
                        Pulse_UnsafeType[chn]=UNSAFE_STAGE;
                        return;  //完成stage异常，直接退出
                    }
                }
            }

            if(!(pSafety->CheckCount&1))  //统计检查的stage数量
                pSafety->CheckStageID++;   // 若是检查了一个Stage了

            pSafety->BufOffset++;
        }
    }
}
void PULSE_safety_check::NormalSafetyValueCheck(unsigned char chn,DWORD type,float value)
{
// 	unsigned char m_bIfCheck;
    float m_fHigh,m_fLow;
    St_SaftyCheckItem  * Check = &safety_check::Check_S[chn].checkSYS_V;
    unsigned char check_offset = CHECK_SYS_V;
    switch(type)
    {
    case CURRENT_SYS_UNSAFE:
// 			m_bIfCheck = safety_check::checkSYS_I[chn].m_bIfCheck;
// 		  m_fHigh = safety_check::checkSYS_I[chn].m_fHigh;
// 		  m_fLow = safety_check::checkSYS_I[chn].m_fLow;
        check_offset = CHECK_SYS_I;
        break;
    case CURRENT_SCH_UNSAFE:
// 			m_bIfCheck = safety_check::checkSCH_I[chn].m_bIfCheck;
// 		  m_fHigh = safety_check::checkSCH_I[chn].m_fHigh;
// 		  m_fLow = safety_check::checkSCH_I[chn].m_fLow;
        check_offset = CHECK_SCH_I;
        break;
    case VOLTAGE_SYS_UNSAFE:
// 			m_bIfCheck = safety_check::checkSYS_V[chn].m_bIfCheck;
// 		  m_fHigh = safety_check::checkSYS_V[chn].m_fHigh;
// 		  m_fLow = safety_check::checkSYS_V[chn].m_fLow;
        break;
    case VOLTAGE_SCH_UNSAFE:
// 			m_bIfCheck = safety_check::checkSCH_V[chn].m_bIfCheck;
// 		  m_fHigh = safety_check::checkSCH_V[chn].m_fHigh;
// 		  m_fLow = safety_check::checkSCH_V[chn].m_fLow;
        check_offset = CHECK_SCH_V;
        break;
    case POWER_SCH_UNSAFE:
// 			m_bIfCheck = safety_check::checkSCH_P[chn].m_bIfCheck;
//       m_fHigh = safety_check::checkSCH_P[chn].m_fHigh;
// 		  m_fLow = safety_check::checkSCH_P[chn].m_fLow;
        check_offset = CHECK_SCH_P;
        break;
    case CAPACITY_SCH_UNSAFE:
        check_offset = CHECK_SCH_C;
        break;
    case STEPTIME_SCH_UNSAFE:
        check_offset = CHECK_SCH_t;
        break;
    default:
        //break;
        return;
    }

    Check += check_offset;
    // m_bIfCheck = Check->m_bIfCheck;//180726 zyx
    if(type == CAPACITY_SCH_UNSAFE || type == STEPTIME_SCH_UNSAFE)
    {
        m_fHigh = Check->m_fHigh;
        if(value > m_fHigh) //?????
        {
            PulseSafetyErrorflg = 1;
            Unsafetype = type;
			if(type == CAPACITY_SCH_UNSAFE)
			{
				if(fabs(safety_check::g_dChargeCapacity[chn]) < fabs(safety_check::g_dDischargeCapacity[chn]))
				    value = -value;
			}
            SetUnsafe(chn, type, value);
        }
    }
    else
    {
        m_fHigh = Check->m_fHigh;
        m_fLow = Check->m_fLow;

//     if(m_bIfCheck==1)//180726 zyx
//     {
        if ((value> m_fHigh)||(value<m_fLow)) //检查上下限
        {
            PulseSafetyErrorflg=1;
            Unsafetype=type;
            SetUnsafe(chn, type, value);
        }
//     }
    }
}
// void PULSE_safety_check::ClearSchUnsafeIfNotRunning(unsigned char chn)
// {
// 	unsigned char type = PULSE_safety_check_sch_is_ready(chn);

// 	if(type == SAFETY_CHECK_SYS)	// if no schedule is running, clear the schedule unsafe status.
// 	{
// 		ClrUnsafe(chn, CURRENT_SCH_UNSAFE);
// 		ClrUnsafe(chn, VOLTAGE_SCH_UNSAFE);
// 		ClrUnsafe(chn, POWER_SCH_UNSAFE);
// 		ClrUnsafe(chn, SCH_UserDefined_UNSAFE);    //170122
// 	}
// }
void PULSE_safety_check::StatusProcess(unsigned char chn, uint16 checkItem)		// 根据状态处理相关结果
{
    MsTime Time;
    unsigned char unsafe = 0;

    if(ErrorFlag[chn] != 0)    //if have error unsafe = 1
        unsafe = 1;

    switch(Process_SM_Ptr[chn])
    {
    case SAFE:
        if(unsafe)
        {
            switch (Pulse_UnsafeType[chn])
            {
            case UNSAFE_COUNTER:    //需要10ms等待判断
                CommonFunc::GetTimeTicksUpdate(&safety_last_time[chn]);
                Process_SM_Ptr[chn] = SAFE2UNSAFE;
                Pulse_SafetyCheck[chn].RecordCount++;  //计数累加
                ErrorFlag[chn]=0;
                break;
            case UNSAFE_CYCLE:    //连续3个循环出异常则直接报告
            case UNSAFE_STAGE:    //1个cycle中3个Stage出异常
                Process_SM_Ptr[chn] =UNSAFE;
                break;
            default:
                break;
            }
        }
        break;
    case SAFE2UNSAFE:
        //这里判断是否到达10ms并且有80%数据异常了
        //在此期间还有可能出现其他两种异常
        if((Pulse_UnsafeType[chn]==UNSAFE_CYCLE)||(Pulse_UnsafeType[chn]==UNSAFE_STAGE))
        {
            Schedule_Manager_task::LockModeSwitch[chn]=1;
            Process_SM_Ptr[chn] =UNSAFE;
            break;
        }
        Pulse_SafetyCheck[chn].RecordCount++; //计数累加

// 		OS_ServeProcess::recordCurTime(&safety_error_time[chn]);
// 		Time = TimeT(safety_last_time[chn],safety_error_time[chn]);    //比较这2个时间  safety_error_time>safety_last_time
        //Time = OS_ServeProcess::TimeDiff(safety_last_time[chn]);
        Time = CommonFunc::TimeDiff(safety_last_time[chn]);
        //if(Time.Us100 >= PULSESAFETY_CHECK_PERIOD)//&&(Pulse_SafetyCheck[chn].UnsafeCount*100/Pulse_SafetyCheck[chn].CheckCount>80))
        if(Time.Us100 >= PULSESAFETY_CHECK_PERIOD)//&&(Pulse_SafetyCheck[chn].UnsafeCount*100/Pulse_SafetyCheck[chn].CheckCount>80))
        {

            if(Pulse_SafetyCheck[chn].UnsafeCount*100/(Pulse_SafetyCheck[chn].RecordCount+Pulse_SafetyCheck[chn].CheckCount)>80)
            {
                Schedule_Manager_task::LockModeSwitch[chn]=1; //现在unsafe处理只给Schedule发送一次，那么若状态切换了，并且没有回复正常的话，就需要有标记表示现在的Unasfe状态。 0901 dirui
                Process_SM_Ptr[chn] = UNSAFE;
            }
            else
            {
                Process_SM_Ptr[chn] = SAFE;
                ErrorFlag[chn]=0;
            }
        }
        break;
    case UNSAFE:
        sendError(chn);		// trigger a sending
        Process_SM_Ptr[chn] = UNSAFE2SAFE;
        break;
    case UNSAFE2SAFE:
        Schedule_Manager_task::LockModeSwitch[chn]=0;
        //	Process_SM_Ptr[chn] = SAFE;
        break;

    default:
        Process_SM_Ptr[chn] = SAFE;
    }
}
void PULSE_safety_check::sendError(unsigned char chn)  //第三方不必处理  dirui 20140819
{   //触发警戒条件
    //按照ErrorFlag 来填充 // m_btSafe_Code need to be re-explained in Schedule management task
    St_ScheduleMessage sch_msg;
    memset((void *)&sch_msg,0,sizeof(St_ScheduleMessage));
    //		unsigned short temp = 0;
    //		temp = ErrorFlag[chn];
    unsigned long err_flg = ErrorFlag[chn];
    // Packing Msg
    sch_msg.MsgNumber = Sch_OnUnsafe_Msg;    //lock sch
    sch_msg.m_Message.m_Assign.m_u16ChannelIdx = chn;    //通道
    sch_msg.st_status_sch_running.m_btCMD_Source = ST_SAFECHECK;
    sch_msg.st_status_sch_running.m_uw32Safe_Code = err_flg;
//     sch_msg.st_status_sch_running.m_btLost_Unit = 0;
    //important    comment out chen 20130814
    ChannelInfo_Manager::ChannelInfo[chn].m_SafetyStatus = err_flg;
    ChannelInfo_Manager::ChannelInfo[chn].m_UnsafeType = IV_UNSAFE_TYPE;
    //根据不同触发类型来向sch汇报m_btSafe_ClassType  具体请参考    enum MP_SCH_SAFTY_TYPE in SF_Hwf.h
    //现在有与上位机不匹配的问题

// 	if(err_flg & CTRL_VALUE_CHK_UNSAFE)
// 		sch_msg.st_status_sch_running.m_btSubCode = CtrlValueCheck[chn].ErrorCode;
// 	if(err_flg & IRREGULAR_UNSAFE)
// 		sch_msg.st_status_sch_running.m_btSubCode = IrregularCheck[chn].ErrorCode;

    if((err_flg & CURRENT_SYS_UNSAFE) || (err_flg & CURRENT_SCH_UNSAFE))
    {
        sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeCurrent = I_Record[chn];
    }
    else if((err_flg & VOLTAGE_SYS_UNSAFE) || (err_flg & VOLTAGE_SCH_UNSAFE))
    {
        sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeVoltage = V_Record[chn];
    }
    else if((err_flg & POWER_SYS_UNSAFE) || (err_flg & POWER_SCH_UNSAFE))
    {
        sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafePower = P_Record[chn];
    }
    else if(ErrorFlag[chn] & SCH_UserDefined_UNSAFE)  //170122
    {
        sch_msg.st_status_sch_running.m_Value.st_UnsafeUD_Value.f_Value = UD_Record[chn];
        int UD_Idx = UD_UnsafeIndex[chn];
        if( (UD_Idx < 0) || (UD_Idx >= MAX_USERDEFINED_SAFETY) )
            UD_Idx = 0;
        St_Safety_UD  *pUDsafety = &ChannelInfo_Manager::ChannelSchedule[chn].m_Settings.m_Safety_UD[UD_Idx];
        sch_msg.st_status_sch_running.m_btSafetyUD_DataType = pUDsafety->m_DataType;
        sch_msg.st_status_sch_running.m_btSafetyUD_MetaCode = pUDsafety->m_MetaCode;
    }
    else if(err_flg & CAPACITY_SCH_UNSAFE)
    {
        sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeCapacity = C_Record[chn];
    }
    else if(err_flg & STEPTIME_SCH_UNSAFE)
    {
        sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeStepTime = t_Record[chn];
    }
    else
    {
        ErrorFlag[chn]=0;   //用于跟踪异常
    }

    // Send to Schedule manager. Here, there might not be schedule running, but msg need to be sent each time safe->unsafe happens.
    Schedule_Manager_task::UnsafeContent[chn]=sch_msg;
    Schedule_Manager_task::SendMsgToSchedule(chn,sch_msg);

//#if REDUNDANT_SAFETY_MODE == 1
//     if(board_config::Feature_Enable_init.Redundant_Safety_Mode==1)
// 	{
// 	    if((PULSE_safety_check::ErrorFlag[chn]&REDUNDANT_SAFETY)!=0)
// 		    RedundantSafetyFlg[chn]=1;
// 	}
//#endif
    //	}
// 	return 1;
}
//char PULSE_safety_check::resumeError(unsigned char chn)
//{
//	//发送恢复
//	St_ScheduleMessage sch_msg;
//	sch_msg.m_Message.m_Assign.m_u16ChannelIdx = chn;    //通道
//	//ChannelInfo_Manager::ChannelInfo[chn].m_SafetyStatus = 0;  //Resume的时候这个条件没有必要清除掉 Unsafe的时候直接可以覆盖 0902 dirui
//	sch_msg.MsgNumber = Sch_OnSafe_Msg;    //unlock
//	Schedule_Manager_task::LockModeSwitch[chn]=0; //解除安全标记  0902
//	memset(&Schedule_Manager_task::UnsafeContent[chn],0,sizeof(St_ScheduleMessage));
//	Schedule_Manager_task::SendMsgToSchedule(chn,sch_msg);

//	return 1;
//}

// MsTime PULSE_safety_check::TimeT(MsTime a,MsTime b)    // a<b
// {///////////////////////////////////////////////////////////////////// Above By Philip ///////////////////////////////////////////////////
// 	//极其简单的比较时间差函数...
// 	//一般都是ms级别的，所以没必要比较s级别
// 	//comment out chen 20130812  need modify    <<<LATER>>>
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
// unsigned char PULSE_safety_check::getSafetyFlag(unsigned char chn)
// {
// 	return ErrorFlag[chn];
// }
char PULSE_safety_check::PULSE_safety_check_sch_is_ready(unsigned char chn)
{
    //检查当前的是否在运行sch和第三方还是idle
    //如果是sch  则返回1
    //这里或许有问题
    //comment out chen 20130219
    //return ChannelInfo_Manager::ChannelInfo[chn].ScheduleStatus;
    //if((ChannelInfo_Manager::ChannelInfo[chn].ScheduleStatus == IDLE)
    //	||(ChannelInfo_Manager::ChannelInfo[chn].ScheduleStatus == NOTREADY))
    if(ChannelInfo_Manager::ChannelInfo[chn].chn_Running_Flag != CHN_LOCK_SCH)
    {
        return SAFETY_CHECK_SYS;
    }
    else    //说明在sch状态
    {
        return SAFETY_CHECK_SCH;
    }
}
void PULSE_safety_check::BeNormalSafetyPIValueCheck(unsigned char chn,float current,float power)
{
    NormalSafetyValueCheck(chn,CURRENT_SCH_UNSAFE,current);


    NormalSafetyValueCheck(chn,POWER_SCH_UNSAFE,power);

//     float fChargeCapacity = ChannelInfo_Manager::Get_MetaVariable(chn, MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, MetaCode_PV_ChargeCapacity);
//     float fDischargeCapacity = ChannelInfo_Manager::Get_MetaVariable(chn, MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, MetaCode_PV_DischargeCapacity);
//     if(fChargeCapacity > fDischargeCapacity)
//         NormalSafetyValueCheck( chn, CAPACITY_SCH_UNSAFE, fChargeCapacity);
//     else
//         NormalSafetyValueCheck(chn, CAPACITY_SCH_UNSAFE, fDischargeCapacity);
	NormalSafetyValueCheck(chn, CAPACITY_SCH_UNSAFE, fabs(safety_check::g_dChargeCapacity[chn] - safety_check::g_dDischargeCapacity[chn]));
    float fStepTime = ChannelInfo_Manager::Get_MetaVariable(chn, MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, MetaCode_PV_Steptime);
    NormalSafetyValueCheck(chn, STEPTIME_SCH_UNSAFE, fStepTime);
}

