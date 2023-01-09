#include "../Entry/includes.h"

UInt16                  safety_check::Priority = PRIORITY4;
UInt16                  safety_check::TaskID = SAFETYCHECK_TASK;			// should be a definition which positioning this task in OS_Task[]
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
MsTime                  safety_check::safety_last_time[MAXCHANNELNO];
MsTime                  safety_check::safety_error_time[MAXCHANNELNO];
MsTime                  safety_check::safety_resume_time[MAXCHANNELNO];
St_Safty                safety_check::Check_S[MAXCHANNELNO];


#pragma arm section //结束外存变量定义
unsigned char  safety_check::RedundantSafetyFlg[MAXCHANNELNO];
char                    safety_check::Safety_Check_status[MAXCHANNELNO];
unsigned char           safety_check::safety_chn;
unsigned char           safety_check::I_Sample_Flag[MAXCHANNELNO];
unsigned char           safety_check::V_Sample_Flag[MAXCHANNELNO];
St_BuiltinSafety           safety_check::BuiltinAux_Check[MAXCHANNELNO];
St_CtrlValueCheck       safety_check::CtrlValueCheck[MAXCHANNELNO];
St_IrregularCheck       safety_check::IrregularCheck[MAXCHANNELNO];
unsigned char safety_check::MTCTempSafetyGoto[MAXCHANNELNO];
St_CtrlValueSetting     safety_check::m_CtrlValueSetting[MAXCHANNELNO];//1805222 zyx
St_IrregularSetting     safety_check::m_IrregularSetting;
unsigned char           safety_check::Process_SM_Ptr[MAXCHANNELNO];	// process() status machine ptr.
float                   safety_check::UD_Record[MAXCHANNELNO];                           //170122
int                   safety_check::UD_UnsafeIndex[MAXCHANNELNO];                      //170122
float safety_check::Submaster_Record;//[MAXCHANNELNO];   //定义必须是整个unit才能参与多unit并联。其实应该只需要一个通道的，不需要数组。
unsigned char           safety_check::Safty_HwErrFlag;					//Wzh
unsigned char           safety_check::Hw_Err_Flag[MAXCHANNELNO];		//Wzh
unsigned char           safety_check::Fan_FaultCheckFlag;
unsigned char           safety_check::Temperature_FaultCheckFlag;
unsigned int            SendDelayCounter=0;
MsTime StartFanSafeTime[MAXCHANNELNO];
MsTime StartFanUnsafeTime[MAXCHANNELNO];
MsTime CurrentTime;
MsTime PassTime;
unsigned char FanCheckState[MAXCHANNELNO];
unsigned char FanUnsafeConunter[MAXCHANNELNO];
unsigned char FanElimination[MAXCHANNELNO];

double safety_check::g_dChargeCapacity[MAXCHANNELNO];
double safety_check::g_dDischargeCapacity[MAXCHANNELNO];

void safety_check::Init(void)
{
    //赋0
    for(unsigned char i = 0; i<MAXCHANNELNO; i++)
    {
        Safety_Check_status[i] = 0;    //状态机
        Process_SM_Ptr[i] = SAFE;

        //may be wrong  因为不清楚是board config先运行还是这个先运行
        ChannelInfo_Manager::GetSystemSafty(i,MP_SYS_SAFTY_TYPE_CURRENT,ChannelInfo_Manager::CurrRngPhysicalRange1,& Check_S[i].checkSYS_I); //zyx 181015
        ChannelInfo_Manager::GetSystemSafty(i,MP_SYS_SAFTY_TYPE_VOLTAGE,VOLTAGE_RANGE_HIGH,& Check_S[i].checkSYS_V);
        ChannelInfo_Manager::GetSystemSafty(i,MP_SYS_SAFTY_TYPE_POWER,0,& Check_S[i].checkSYS_P);

        ChannelInfo_Manager::GetSystemSafty(i,MP_SYS_SAFTY_TYPE_INIT_SCHEDULE,0,& Check_S[i].checkSCH_V); //180808 zyx
        ChannelInfo_Manager::GetSystemSafty(i,MP_SYS_SAFTY_TYPE_INIT_SCHEDULE,0,& Check_S[i].checkSCH_I);
        ChannelInfo_Manager::GetSystemSafty(i,MP_SYS_SAFTY_TYPE_INIT_SCHEDULE,0,& Check_S[i].checkSCH_P);
        ChannelInfo_Manager::GetSystemSafty(i,MP_SYS_SAFTY_TYPE_INIT_SCHEDULE,0,& Check_S[i].checkSCH_C);
        ChannelInfo_Manager::GetSystemSafty(i,MP_SYS_SAFTY_TYPE_INIT_SCHEDULE,0,& Check_S[i].checkSCH_t);


        //采样刷新标志
        I_Sample_Flag[i] = 0;
        V_Sample_Flag[i] = 0;
        Hw_Err_Flag[i] = 0;
        BuiltinAux_Check[i].checkSCH_Aux_V.m_bIfCheck =0;
        BuiltinAux_Check[i].checkSCH_Aux_T.m_bIfCheck =0;
        //记录错误
        RedundantSafetyFlg[i]=0;
        if(m_CtrlValueSetting[i].fErrorFactor <= 0)
            m_CtrlValueSetting[i].fErrorFactor = DEF_ACCURACY * DEF_CTRL_VALUE_ERROR_FACTOR;
        m_CtrlValueSetting[i].bChecking  = 0;
        m_CtrlValueSetting[i].NextTime   = CommonFunc::GetSystemTime();//OS_ServeProcess::OS_Time;
        FanCheckState[i] = FanSafe;
        FanUnsafeConunter[i] = 0;
        FanElimination[0] = 0;
        ClearAll_PV_Error(i);
        if(m_CtrlValueSetting[i].fErrorFactor <= 0)
            m_CtrlValueSetting[i].fErrorFactor = DEF_ACCURACY * DEF_CTRL_VALUE_ERROR_FACTOR;
        m_CtrlValueSetting[i].bChecking  = 0;
        m_CtrlValueSetting[i].NextTime   = CommonFunc::GetSystemTime();//OS_ServeProcess::OS_Time;
        g_dChargeCapacity[i] = 0;
        g_dDischargeCapacity[i] = 0;
    }
    safety_chn = 0;


    //以下为调试代码：
    memset(CtrlValueCheck,0,sizeof(CtrlValueCheck));
    memset(IrregularCheck,0,sizeof(IrregularCheck));
    m_IrregularSetting.bChecking  = 0;
    m_IrregularSetting.NextTime   = CommonFunc::GetSystemTime();
    Submaster_Record = 0;
}

//如果我们想知道在这时间段内  超出limit的具体是什么
//则要增加几个error counter来计数
//算法描述：开始记录一个时间点last，然后check
//如果警报则 记录当前时间为error时间，如果error时间与last超过一定时间，则说明这段时间内都会出limit，这时切断通道
//如果没有警报则移动last时间点到当前时间
//comment out chen 20130531    把取数据拿到外面去
void safety_check::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_SAFTY] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time_init(COUNT_SAFTY);
#endif
    unsigned char usedChn = ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;    //m_u16IVChannelCount = 16
    uint16 checkitem;

    for(int i = 0; i < 4; i ++)    //每次4个
    {
        if(safety_chn >= usedChn)    //0-15chn
            safety_chn = 0;

        checkitem = CheckAndSetCandidate(safety_chn);
        if((checkitem != NO_CHECK)
                &&((GetPV_Error(safety_chn, IV_UNSAFE_TYPE)& IRREGULAR_UNSAFE) != IRREGULAR_UNSAFE)
                &&((GetPV_Error(safety_chn, IV_UNSAFE_TYPE)& CTRL_VALUE_CHK_UNSAFE) != CTRL_VALUE_CHK_UNSAFE))								// 如果可以检查   20160414  yy
        {
            check(safety_chn, checkitem);			// 检查相应安全
        }
        if((ChannelInfo_Manager::m_st_BoadType.m_btBUILTIN == 1)   //添加BUILTIN辅助安全检查
                || (board_config::DacInfo.m_bHaveBuildInVolt && ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_MSTAT21044))//190524,hpq
        {
            if(BuiltinAux_Check[safety_chn].m_bIfCheckFlag !=0)
            {
                BuiltinCheck(safety_chn);
            }
        }
        MakeHwErrCheck(safety_chn);		//add Wzh make hardware error check
        MakeCtrlValueCheck(safety_chn); //1805222 zyx
        StatusProcess(safety_chn, checkitem);		// 根据状态处理相关结果
        safety_chn ++;    //从0开始
    }

    MakeIrregularCheck();

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_SAFTY] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time(COUNT_SAFTY);
#endif
}

// void safety_check::EndTask(void)
// {

// }
void  safety_check::BuiltinCheck(unsigned char chn)
{
    St_ScheduleMessage SchMessage;
    if(BuiltinAux_Check[chn].checkSCH_Aux_V.m_bIfCheck &0x01)  //电压
    {
        float voltage;
        for(unsigned char uc_chn =0; uc_chn<BuiltIn_DataManager::m_BuiltInAuxInfo.VolCntInOneIV; uc_chn++)
        {
            voltage = ChannelInfo_Manager::MetaVariable2[chn][MetaCode_PV_BuiltIn_Data1 + uc_chn].m_fValue;
            if(voltage < BuiltinAux_Check[chn].checkSCH_Aux_V.m_fLow  ||voltage > BuiltinAux_Check[chn].checkSCH_Aux_V.m_fHigh)
            {
                BuiltinAuxReportUnsafe(chn,AUX_VOLTAGE,voltage,uc_chn);
                break;
            }
        }
    }
    if(safety_check::BuiltinAux_Check[chn].checkSCH_Aux_T.m_bIfCheck &0x02)   //温度
    {
        float temperature;

        for(unsigned char uc_chn =0; uc_chn<BuiltIn_DataManager::m_BuiltInAuxInfo.TempCntInOneIV; uc_chn++)
        {
            temperature = ChannelInfo_Manager::MetaVariable2[chn][MetaCode_PV_BuiltIn_Data1 + uc_chn].m_fValue;
            if(temperature < BuiltinAux_Check[chn].checkSCH_Aux_T.m_fLow ||temperature > BuiltinAux_Check[chn].checkSCH_Aux_T.m_fHigh)
            {
                BuiltinAuxReportUnsafe(chn,AUX_TEMPERATURE,temperature,uc_chn);
                break;
            }
        }
    }
}
void safety_check::BuiltinAuxReportUnsafe(unsigned char chn,unsigned char AuxType,float value,unsigned char aux_chn)
{
    St_ScheduleMessage SchMessage;
//     SchMessage.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeSpecial[0]=value;
	SchMessage.st_status_sch_running.m_Value.st_UnsafeMetaValue.un_UnsafeSpecialInfo.st_UnsafeInfo.f_UnsafeValue = value;
	SchMessage.st_status_sch_running.m_Value.st_UnsafeMetaValue.un_UnsafeSpecialInfo.st_UnsafeInfo.f_UnsafeChnID = aux_chn + 1;
    SchMessage.MsgNumber = AUX_Unsafe_Msg;
    SchMessage.st_status_sch_running.m_bUnsafeType = AUX_UNSAFE_TYPE;
    SchMessage.st_status_sch_running.m_uw32Safe_Code = AUX_UNSAFE;  //  ????????????    20160331 yy
    SchMessage.st_status_sch_running.m_btSubCode = AuxType;
    ChannelInfo_Manager::ChannelInfo[chn].m_AUXSafetyStatus = AUX_UNSAFE;
    ChannelInfo_Manager::ChannelInfo[chn].m_UnsafeType = AUX_UNSAFE_TYPE;
    safety_check::SetUnsafe(chn,AUX_UNSAFE,0, AUX_UNSAFE_TYPE);
    ChannelInfo_Manager::ChannelInfo[chn].m_SafetyStatus = ChannelInfo_Manager::ChannelInfo[chn].m_AUXSafetyStatus;
    Schedule_Manager_task::UnsafeContent[chn]=SchMessage;
    Schedule_Manager_task::SendMsgToSchedule(chn,SchMessage);//???????????
}
uint32 safety_check::CheckAndSetCandidate(unsigned char chn)
{   // return values
    // 0: not ready to check
    // 1: current and power check
    // 2: voltage check

    if (ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode() ||ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(chn) ||ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(chn))
    {
        if( ChannelInfo_Manager::ChannelInfo[chn].ScheduleStatus == NOTREADY )  //Add by DKQ on 09_09_2013 for not checking safety in NOTREADY
        {
            return 0; // when the channal is NOTREADY, we don't do any check
        }
    }

    //
    // no check safety during transition
    //

    if(OS_LogicProcess::Logic_For_Safe[chn] == LOGIC_PROCESS_TRAN)
    {
        return 0;		// When in Transition stage, we don't check safety values.
    }

    //
    //only record time for the first time safety checking
    //

    if(Safety_Check_status[chn] == 0)    //第一次进入记录时间
    {
        CommonFunc::GetTimeTicksUpdate(&safety_last_time[chn]);    //使用此接口函数来代替上述操作  记录当前时间到safety_last_time中去
        Safety_Check_status[chn] = 1;
        return 0;		// First time
    }

    //
    // if no sample coming yet (after transition), no safety checking
    //

    //keep the same since present only for single chan inter unit parallel

    if(I_Sample_Flag[chn] == 0)
    {
        if(V_Sample_Flag[chn] == 0)
        {
            return NO_CHECK;
        }
        else
        {

            V_Sample_Flag[chn] = 0;    //clear

            return V_CHECK;
        }
    }
    else
    {

        I_Sample_Flag[chn] = 0;    //clear

        if(V_Sample_Flag[chn] == 0)
        {
            return PI_CHECK;
        }
        else
        {
            V_Sample_Flag[chn] = 0;

            return V_CHECK | PI_CHECK;
        }
    }

}

void safety_check::SetUnsafe(unsigned char chn, uint32 unsafeCode, float value,unsigned char UnsafeType)
{   // each time only one unsafe type is set, and meantime, data recorded.
    //ErrorFlag标志位    I_Record等用来记录
    //ErrorFlag 须与上位机匹配
    SetPV_Error(chn, unsafeCode,UnsafeType );
    float * f_Value = &Schedule_Manager_task::UnsafeContent[chn].st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeCurrent;
    switch(UnsafeType)
    {
    case IV_UNSAFE_TYPE:
        switch(unsafeCode)		// record data here
        {
        case CURRENT_SCH_UNSAFE:
        case CURRENT_SYS_UNSAFE:
            * f_Value = value;
            return;
        case VOLTAGE_SCH_UNSAFE:
        case VOLTAGE_SYS_UNSAFE:
        case IV_CABLE_CONNECTION_UNSAFE:
        case VCLAMP_SAFETY:
            f_Value += 1;
            * f_Value = value;
            return;
        case POWER_SCH_UNSAFE:
        case POWER_SYS_UNSAFE:
            f_Value += 2;
            * f_Value = value;
            return;
        case CAPACITY_SCH_UNSAFE:
            f_Value += 3;
            * f_Value = value;
            return;
        case STEPTIME_SCH_UNSAFE:
            f_Value += 4;
            * f_Value = value;
            return;
        case AMP_IR_UNSAFE:
            f_Value += 5; //3
            * f_Value = value;
            return;
        case PARA_SUB_CUR_UNSAFE:
        case PARA_SUB_VOL_UNSAFE:
            Submaster_Record = value;

            return;
        case SCH_UserDefined_UNSAFE:    //170122
            UD_Record[chn] = value;
            return;
        case CTRL_VALUE_CHK_UNSAFE:    //  20200619
            Schedule_Manager_task::UnsafeContent[chn].st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeCurrent = 0.0;
            Schedule_Manager_task::UnsafeContent[chn].st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeVoltage = 0.0;
            Schedule_Manager_task::UnsafeContent[chn].st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafePower = 0.0;
            switch(CtrlValueCheck[chn].CheckType)
            {
            case CtrlValueCheck_Current:
                * f_Value = ChannelInfo_Manager::MetaVariable2[chn][MetaCode_PV_Current].m_fValue;
                break;
            case CtrlValueCheck_Voltage:
                f_Value += 1;
                * f_Value = ChannelInfo_Manager::MetaVariable2[chn][MetaCode_PV_Voltage].m_fValue;
                break;
            case CtrlValueCheck_Power:
                f_Value += 2;
                * f_Value = ChannelInfo_Manager::MetaVariable2[chn][MetaCode_PV_Current].m_fValue*ChannelInfo_Manager::MetaVariable2[chn][MetaCode_PV_Voltage].m_fValue;
                break;
            default:
                return ;
            }
            if(* f_Value == 0.0)
                * f_Value = 0.000002;
            return ;

        case REDUNDANT_SAFETY:
            f_Value += 1;
            * f_Value = M0RedundantFunc::m_fM0Value;
            return ;

        default:
            return;
        }

// 			break;
    case AUX_UNSAFE_TYPE:
        switch(unsafeCode)		// record data here
        {
        case AUX_UNSAFE:
            * f_Value = value;
            return;
        default:
            return;
        }
// 		  break;
    case DCDC_UNSAFE_TYPE:
    case CANBMS_UNSAFE_TYPE:
    case SMB_UNSAFE_TYPE:
    case INVERTER_UNSAFE_TYPE:
    default:
        break;

    }

}

void safety_check::check(unsigned char chn, uint32 checkItem)
{   // please load everything into checkSYS_I[](V, P too) in this task's init(); a sys setting is not changed from minute to minute.
    // Also when a new setting is downloaded from PC, update not only the FRAM, but also the checkSYS_I[](V, P too)
    // Dirui needs to update the content of checkSCH_I(V/P) when a schedule is started or resumed. They are not changed when running the schedule unless a live update
    // which we assume in MPU, that's starting a different schedule with current metavariables. no need to clear checkSCH_I(V/P) when a schedule finishes.

    if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitSubmasterChannel(chn))
    {
        check_Submaster(chn, checkItem);
        return;
    }

    if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitSubUnitSlaveChannel(chn))
    {
        check_SubSlave(chn, checkItem);
        return;
    }
    float current, voltage, power;
    unsigned char type;

    type = safety_check_sch_is_ready(chn);
    voltage = ChannelInfo_Manager::Get_MetaVariable(chn, MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, MetaCode_PV_Voltage);	// Get Voltage, anyway, it will be used.

    if((checkItem & PI_CHECK) != 0)    //P and I
    {
        current = ChannelInfo_Manager::Get_MetaVariable(chn, MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, MetaCode_PV_Current);	// Get Current

        if (!ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode())
        {
            if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(chn))
            {   //calculate inter parallel group current sum
                current = ChannelInfo_Manager::ChannelInfo[chn].m_GroupData.m_fGroupCurrent;
            }
            //else if (ChannelInfo_Manager::IsIntraUnitMasterChannel(chn))
            else if (ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(chn) && ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[chn].m_UnitGroupInfo.m_WholeGroupSize >0x01)
            {   //calculate intra parallel group current sum
                current = ChannelInfo_Manager::ChannelInfo[chn].m_GroupData.m_fGroupCurrent;
            }
        }

        // in this design, we didn't select a stricter value from sys and schedule safety setting, instead, we check them both.
        if(type == SAFETY_CHECK_SCH)	// Sch safety is checked only schedule is running, sys safety is checked all the time.
        {
            power = current * voltage;    //P=I*V
            SafetyValueCheck(CURRENT_SCH_UNSAFE, chn, current);
            SafetyValueCheck(POWER_SCH_UNSAFE, chn, power);
            /*充放电最大值与最大容量作比较*/
            SafetyValueCheck(CAPACITY_SCH_UNSAFE, chn, fabs(g_dChargeCapacity[chn] - g_dDischargeCapacity[chn]));//充放电量累积报警问题 20190809
            float fStepTime = ChannelInfo_Manager::Get_MetaVariable(chn, MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, MetaCode_PV_Steptime);
            SafetyValueCheck(STEPTIME_SCH_UNSAFE, chn, fStepTime);
        }
        power = ChannelInfo_Manager::m_RecordMasterChannelPVCurrent[chn] * voltage;    //P=I*V
        SafetyValueCheck(POWER_SYS_UNSAFE, chn, power);
        // Additional I check when sch is not running(relay off). if no schedule running, we don't care to do the check
        // coz aboundant MPU resource left, and if something really goes wrong, it can be detected. it's a good thing.
        // also when 3rd party cmd is working: sch is not running, but relay is possible on.
        SafetyValueCheck(CURRENT_SYS_UNSAFE, chn, ChannelInfo_Manager::m_RecordMasterChannelPVCurrent[chn]); //181227 zyx
        // Power safety for System is a very complicated issue, we don't take care of it right now.
    }
    if((checkItem & V_CHECK) != 0)
    {
        if(type == SAFETY_CHECK_SCH)
        {
            SafetyValueCheck(VOLTAGE_SCH_UNSAFE, chn, voltage);
            //170122 start
            St_Safety_UD  *    pUDsafety    =    &ChannelInfo_Manager::ChannelSchedule[chn].m_Settings.m_Safety_UD[0];
            for(int UD_Idx = 0; UD_Idx < MAX_USERDEFINED_SAFETY; UD_Idx++)
            {
                if(pUDsafety->m_bValid)
                {
                    float fValue = ChannelInfo_Manager::Get_MetaVariable(chn,pUDsafety->m_DataType,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT,pUDsafety->m_MetaCode);
                    float fHigh = ChannelInfo_Manager::Get_MetaVariable(chn,pUDsafety->m_ucDataTypeHigh,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT,pUDsafety->m_fHigh);
					float fLow = ChannelInfo_Manager::Get_MetaVariable(chn,pUDsafety->m_ucDataTypeLow,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT,pUDsafety->m_fLow);
					if(fValue > fHigh || fValue < fLow)
                    {
                        SetUnsafe(chn,SCH_UserDefined_UNSAFE,fValue,IV_UNSAFE_TYPE);
                        UD_UnsafeIndex[chn] = UD_Idx;
                        break;
                    }
                }
                else
                    ClrUnsafe(chn, SCH_UserDefined_UNSAFE,IV_UNSAFE_TYPE);
                pUDsafety++;
            } // 170122 end
        }
        SafetyValueCheck(VOLTAGE_SYS_UNSAFE, chn, voltage);//180906 zyx
        if(board_config::Feature_Enable_init.Redundant_Safety_Mode==1)
        {
            if((Check_S[chn].checkSYS_V.m_bIfCheck == 1) && (board_config::RedundantSafety(chn,0)==1))  //add redundant voltage safety dirui 2014/07/29
                SetUnsafe(chn, REDUNDANT_SAFETY, voltage,IV_UNSAFE_TYPE);
            else
                ClrUnsafe(chn, REDUNDANT_SAFETY,IV_UNSAFE_TYPE);
        }
    }
    //fan  故障报警
    if(board_config::Feature_Enable_init.Fan_Safety_Mode==1)    //Test 20180129
    {
        Fan_Safety_Check(chn);
    }
    if(board_config::Feature_Enable_init.Temperature_Safety_Mode==1)    //Test 20180129
    {
        if(Temperature_FaultCheckFlag==1)
        {
            SetUnsafe(chn, IV_TEMPERATURE_UNSAFE, voltage,IV_UNSAFE_TYPE);
            if(board_config::TemperatureSafetyCheck(chn,0)==0)
                Temperature_FaultCheckFlag=0;
        }
        else
        {
            ClrUnsafe(chn, IV_TEMPERATURE_UNSAFE,IV_UNSAFE_TYPE);
            if(board_config::TemperatureSafetyCheck(chn,0)==1)
                Temperature_FaultCheckFlag=1;
        }
    }
}
void safety_check::Fan_Safety_Check(unsigned char chn)
{
    switch(FanCheckState[chn])
    {
    case FanSafe:
        PassTime = CommonFunc::TimeDiff(StartFanSafeTime[chn]);;
        if(PassTime.Second >= 3 )//3秒之内没有电平异常
        {
            ClrUnsafe(chn, FAN_FAULT_UNSAFE,IV_UNSAFE_TYPE);
            CommonFunc::GetTimeTicksUpdate(&StartFanSafeTime[chn]);
            FanUnsafeConunter[chn] = 0;
            FanElimination[chn] = 0;
        }
        else
        {
            if(board_config::FanSafetyCheck(chn,0)==1)
            {
                FanElimination[chn]++;
                if(FanElimination[chn] >= 5)//去抖处理，连续监测到5次错误电平。
                {
                    FanCheckState[chn] = FanUnsafe;
                    FanUnsafeConunter[chn]++;
                    CommonFunc::GetTimeTicksUpdate(&StartFanUnsafeTime[chn]);
                }
                if(FanUnsafeConunter[chn] >= 4)//2//连续监测到两个周期错误。
                {
                    SetUnsafe(chn, FAN_FAULT_UNSAFE, 0,IV_UNSAFE_TYPE);
                    FanUnsafeConunter[chn] = 0;
                }
            }
            else
            {
                FanElimination[chn] = 0;
            }
        }
        break;
    case FanUnsafe:
        PassTime = CommonFunc::TimeDiff(StartFanUnsafeTime[chn]);
        if(PassTime.Second>=3 )//3秒只能全是电平异常
        {
            SetUnsafe(chn, FAN_FAULT_UNSAFE, 0,IV_UNSAFE_TYPE);
            FanUnsafeConunter[chn] = 0;
            FanElimination[chn] = 0;
            CommonFunc::GetTimeTicksUpdate(&StartFanUnsafeTime[chn]);
        }
        else
        {
            if(board_config::FanSafetyCheck(chn,0)==0)
            {
                FanCheckState[chn] = FanSafe;
                FanElimination[chn] = 0;
                CommonFunc::GetTimeTicksUpdate(&StartFanSafeTime[chn]);
            }
        }
        break;
    default:
        break;
    }
}

void safety_check::CheckCableConnection(unsigned char chn)
{
    if((ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_PWM)&&(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_D))
    {
        float voltage = ChannelInfo_Manager::Get_MetaVariable(chn, MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, MetaCode_PV_Voltage);
        float Checkvoltage = ChannelInfo_Manager::ChannelDirection(chn,voltage,PWM_SampleManage::m_u32ChannelVoltageAdc);

        if(Checkvoltage > ChannelInfo_Manager::BoardCFG.m_Channel[chn].m_VRange[ChannelInfo_Manager::ChannelInfo[chn].m_ucVoltageRange].m_Bound.m_High*0.05)
        {
            SetUnsafe(chn, IV_CABLE_CONNECTION_UNSAFE, Checkvoltage,IV_UNSAFE_TYPE);
        }
        else
        {
            ClrUnsafe(chn, IV_CABLE_CONNECTION_UNSAFE,IV_UNSAFE_TYPE);
        }
    }

}

void safety_check::SafetyValueCheck(DWORD type, unsigned char chn, float value)
{
    unsigned char m_bIfCheck;
    St_SaftyCheckItem  * Check = &Check_S[chn].checkSYS_V;
    unsigned char check_offset = CHECK_SYS_V;
    float m_fHigh,m_fLow;
    switch(type)
    {
    case CURRENT_SCH_UNSAFE:
        check_offset = CHECK_SCH_I;
        break;
    case POWER_SCH_UNSAFE:
        check_offset = CHECK_SCH_P;
        break;
    case POWER_SYS_UNSAFE:
        check_offset = CHECK_SYS_P;
        break;
    case CURRENT_SYS_UNSAFE:
    case PARA_SUB_CUR_UNSAFE:
        check_offset = CHECK_SYS_I;
        break;
    case VOLTAGE_SCH_UNSAFE:
        check_offset = CHECK_SCH_V;
        break;
    case CAPACITY_SCH_UNSAFE:
        check_offset = CHECK_SCH_C;
        break;
    case STEPTIME_SCH_UNSAFE:
        check_offset = CHECK_SCH_t;
        break;
    case VOLTAGE_SYS_UNSAFE:
    case PARA_SUB_VOL_UNSAFE:
        break;
    default:
// 			//break;
        return;
    }
//
    Check += check_offset;
    m_bIfCheck = Check->m_bIfCheck;
    m_fHigh = Check->m_fHigh;
    m_fLow = Check->m_fLow;

    if(m_bIfCheck==1)
    {
        if(type == CAPACITY_SCH_UNSAFE)// 20190806
        {
            if(value> m_fHigh)
            {
                if(fabs(g_dChargeCapacity[chn]) < fabs(g_dDischargeCapacity[chn]))
                    value = -value;
                SetUnsafe(chn, type, value,IV_UNSAFE_TYPE);
            }
            else
                ClrUnsafe(chn, type,IV_UNSAFE_TYPE);
        }
        else
        {
            if ((value> m_fHigh)||(value<m_fLow)) //?????
            {
                SetUnsafe(chn, type, value,IV_UNSAFE_TYPE);
            }
            else
                ClrUnsafe(chn, type,IV_UNSAFE_TYPE);
        }
    }
    else
    {
        ClrUnsafe(chn, type, IV_UNSAFE_TYPE);
    }
}

//#if (SUPPORT_PARALLEL == 1)
void safety_check::check_Submaster(unsigned char chn, uint32 checkItem)
{   // please load everything into checkSYS_I[](V, P too) in this task's init(); a sys setting is not changed from minute to minute.
    // Also when a new setting is downloaded from PC, update not only the FRAM, but also the checkSYS_I[](V, P too)
    // Dirui needs to update the content of checkSCH_I(V/P) when a schedule is started or resumed. They are not changed when running the schedule unless a live update
    // which we assume in MPU, that's starting a different schedule with current metavariables. no need to clear checkSCH_I(V/P) when a schedule finishes.
    float current, voltage;//, power;
    voltage = ChannelInfo_Manager::Get_MetaVariable(chn, MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, MetaCode_PV_Voltage);	// Get Voltage, anyway, it will be used.

    if((checkItem & PI_CHECK) != 0)    //P and I
    {
        current = ChannelInfo_Manager::m_RecordMasterChannelPVCurrent[chn];	// Get Current
        // Power safety for System is a very complicated issue, we don't take care of it right now.
        SafetyValueCheck(PARA_SUB_CUR_UNSAFE, chn, current);
    }
    if((checkItem & V_CHECK) != 0)
    {
        SafetyValueCheck(PARA_SUB_VOL_UNSAFE, chn, voltage);
    }
    if(board_config::Feature_Enable_init.Fan_Safety_Mode==1)
    {
        Fan_Safety_Check(chn);
    }
}
//#endif
void safety_check::check_SubSlave(unsigned char chn, uint32 checkItem)
{
    float current, voltage;//, power;
    ClearAll_PV_Error(chn);  //dirui add 2015/07/15  防止Unit内并unsafe联标记没有被清除
    voltage = ChannelInfo_Manager::Get_MetaVariable(chn, MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, MetaCode_PV_Voltage);	// Get Voltage, anyway, it will be used.

    if((checkItem & PI_CHECK) != 0)    //P and I
    {
        current = ChannelInfo_Manager::Get_MetaVariable(chn, MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, MetaCode_PV_Current);	// Get Current
        SafetyValueCheck(PARA_SUB_CUR_UNSAFE, chn, current);
        // Power safety for System is a very complicated issue, we don't take care of it right now.

    }
    if((checkItem & V_CHECK) != 0)
    {
        SafetyValueCheck(PARA_SUB_VOL_UNSAFE, chn, voltage);
    }
}
void safety_check::ClearUnsafeIfNotRunning(unsigned char chn)//需要手动清除的错误码
{
	for(unsigned char i = chn;i < (chn + ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[chn].m_GroupID].m_UnitGroupInfo.m_WholeGroupSize);i++)
	{
		Schedule_Manager_task::LockModeSwitch[i] = 0;
		ClearAll_PV_Error(i);
	}

}
void safety_check::ClearSchUnsafeIfNotRunning(unsigned char chn)
{
    unsigned char type = safety_check_sch_is_ready(chn);

    if(type == SAFETY_CHECK_SYS)	// if no schedule is running, clear the schedule unsafe status.
    {
        for(unsigned char i=0; i<3; i++)
            ClrUnsafe(chn, CURRENT_SCH_UNSAFE<<(2*i),IV_UNSAFE_TYPE);  // CURRENT_SCH_UNSAFE,VOLTAGE_SCH_UNSAFE,POWER_SCH_UNSAFE
        ClrUnsafe(chn, SCH_UserDefined_UNSAFE,IV_UNSAFE_TYPE);  //170122
        ClrUnsafe(chn, STEPTIME_SCH_UNSAFE,IV_UNSAFE_TYPE);  //170122
        ClrUnsafe(chn, CAPACITY_SCH_UNSAFE,IV_UNSAFE_TYPE);  //170122
    }
}

void safety_check::StatusProcess(unsigned char chn, uint32 checkItem)		// 根据状态处理相关结果
{
    MsTime Time;
    unsigned char unsafe = 0;

    if(GetPV_ErrorType(chn) != 0XFF)    //if have error unsafe = 1
        unsafe = 1;

    switch(Process_SM_Ptr[chn])
    {
    case SAFE:
        if(unsafe)
        {
            CommonFunc::GetTimeTicksUpdate(&safety_last_time[chn]);
            Process_SM_Ptr[chn] = SAFE2UNSAFE;
        }
        Schedule_Manager_task::LockModeSwitch[chn]=0;
        break;

    case SAFE2UNSAFE:
        if(unsafe)
        {
            Time = CommonFunc::TimeDiff(safety_last_time[chn]);
            unsigned short timeout;
            if ( ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_PWM)   // ADD DKQ 10.03.2016
                timeout = 2* SAFETY_CHECK_PERIOD;
            else
                timeout = SAFETY_CHECK_PERIOD;
            if(Time.Us100 >= timeout)
            {
                sendError(chn);		// trigger a sending
                Schedule_Manager_task::LockModeSwitch[chn]=1; //现在unsafe处理只给Schedule发送一次，那么若状态切换了，并且没有回复正常的话，就需要有标记表示现在的Unasfe状态。 0901 dirui
                Schedule_Manager_task::LockModeSwitch[ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[chn].m_GroupID].m_UnitGroupInfo.m_HeadChn]=1;//Lock para master channel dirui 2015/0714
                thirdParty::UnsafeLockSwitch[chn]=1;    //add by qjm20140825
                Process_SM_Ptr[chn] = UNSAFE;
                break;
            }
        }
        else
        {
            Process_SM_Ptr[chn] = SAFE;
        }
        break;

    case UNSAFE:
        if(!unsafe)
        {
            CommonFunc::GetTimeTicksUpdate(&safety_resume_time[chn]);
            Process_SM_Ptr[chn] = UNSAFE2SAFE;
        }
        else
        {
            //在unsafe状态下  还是unsafe的话，不做处理
            // check every 5 seconds to Make sure channel is closed.<<<LATER>>> -philip 8/8/2013-

//#if(REDUNDANT_VOLT_MODE == REDUNDANT_M0)
            if(board_config::Feature_Enable_init.RedundantVoltMode == REDUNDANT_M0)
                RedundantSafetyFlg[chn]=0;  //Add DKQ 03.11.2015
//#endif
        }
        if(ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(chn))
            Schedule_Manager_task::LockModeSwitch[chn]=1; //现在unsafe处理只给Schedule发送一次，那么若状态切换了，并且没有回复正常的话，就需要有标记表示现在的Unasfe状态。 0901 dirui
        else if(ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitSlaveChannel(chn))
		{
            Schedule_Manager_task::LockModeSwitch[ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[chn].m_GroupID].m_UnitGroupInfo.m_HeadChn]=1;//Lock para master channel dirui 2015/0714
			Schedule_Manager_task::UnsafeContent[ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[chn].m_GroupID].m_UnitGroupInfo.m_HeadChn].st_status_sch_running.m_Value.st_UnsafeMetaValue.un_UnsafeSpecialInfo.st_UnsafeInfo.f_UnsafeChnID = chn + 1;
		}
        else if((ChannelInfo_Manager::Serial2ParaMod_IsInterUnitSubmasterChannel(chn))||(ChannelInfo_Manager::Serial2ParaMod_IsInterUnitSubUnitSlaveChannel(chn)))
        {
            if(SendDelayCounter>100)
            {
                sendError(chn);
                SendDelayCounter=0;
            }
            else
                SendDelayCounter++;
        }
        break;

    case UNSAFE2SAFE:
        if(!unsafe && ChannelInfo_Manager::ChannelInfo[chn].ScheduleStatus != GOSTOP)
        {
            Time = CommonFunc::TimeDiff(safety_last_time[chn]);
            if(Time.Us100+Time.Second*10000 >= SAFETY_CHECK_RESUME)
            {
                resumeError(ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[chn].m_GroupID].m_UnitGroupInfo.m_HeadChn);	   //改为向各自的master通道发出，即便是单通道也如此； dirui 20150713
                thirdParty::UnsafeLockSwitch[chn]=0;  //add by qjm20140825
                Process_SM_Ptr[chn] = SAFE;
                break;
            }
        }
        else
        {
            Process_SM_Ptr[chn] = UNSAFE;
        }
        break;

    default:
        Process_SM_Ptr[chn] = SAFE;
    }
}

bool safety_check::IsUnsafe(unsigned char chn,uint32 unsafeCode,unsigned char UnsafeType)
{
    if((GetPV_Error( chn, UnsafeType) &unsafeCode) !=0)
        return true;
    return false;
}

void safety_check::sendError(unsigned char chn)
{   //触发警戒条件
    //按照ErrorFlag 来填充 // m_btSafe_Code need to be re-explained in Schedule management task
    St_ScheduleMessage sch_msg;
    char if_third;
    memset((void *)&sch_msg,0,sizeof(St_ScheduleMessage));
    if_third = thirdParty::getStatus(chn);

    unsigned char uc_sch_running = 0;
    if((if_third == THIRD_PARTY_STATUS_INACTIVE)||(if_third == THIRD_PARTY_STATUS_IDLE))
        uc_sch_running = 1;    // 20161116 y
    unsigned char UnsafeType = GetPV_ErrorType(chn);
    if(!uc_sch_running)	//Third part process
    {
        if(if_third == THIRD_PARTY_STATUS_OUTPUT)	// only stop when doing output, // I keep the original code without major modifying, only delete the error code clearing- philip
        {
            st_Third_Party_Msg TP_msg;
            TP_msg.chn = chn;
            TP_msg.TaskID = SAFETYCHECK_TASK;
            TP_msg.st_third_party_cmd.cmd = THIRD_PARTY_STOP;    //从任何状态退出
            thirdParty::sendMsgToMe(&TP_msg);    //停止通道
            // Packing Msg
            sch_msg.MsgNumber = Sch_OnUnsafe_Msg;    //lock sch
            sch_msg.m_Message.m_Assign.m_u16ChannelIdx = chn;    //通道
            sch_msg.st_status_sch_running.m_btCMD_Source = ST_SAFECHECK;
            sch_msg.st_status_sch_running.m_bUnsafeType = UnsafeType;
            sch_msg.st_status_sch_running.m_uw32Safe_Code = GetPV_Error(chn, UnsafeType);
			sch_msg.st_status_sch_running.m_btLost_Unit = CANFunc::SourceBoardID;
			
            //important    comment out chen 20130814
            ChannelInfo_Manager::ChannelInfo[chn].m_UnsafeType = UnsafeType;
            ChannelInfo_Manager::ChannelInfo[chn].m_SafetyStatus = GetPV_Error(chn, UnsafeType);
			
            if(UnsafeType==INVERTER_UNSAFE_TYPE)
            {
                sch_msg.st_status_sch_running.m_btLost_Unit = Schedule_Manager_task::UnsafeContent[chn].st_status_sch_running.m_btLost_Unit;
            }

            //根据不同触发类型来向sch汇报m_btSafe_ClassType  具体请参考    enum MP_SCH_SAFTY_TYPE in SF_Hwf.h
            //现在有与上位机不匹配的问题
            float * f_Value = &Schedule_Manager_task::UnsafeContent[chn].st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeCurrent;
            if(GetPV_Error(chn,  IV_UNSAFE_TYPE) & CTRL_VALUE_CHK_UNSAFE)    // 20200619
            {
                sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue = Schedule_Manager_task::UnsafeContent[chn].st_status_sch_running.m_Value.st_UnsafeMetaValue;
            }
            if((GetPV_Error(chn,  IV_UNSAFE_TYPE) & CURRENT_SYS_UNSAFE) || (GetPV_Error(chn,  IV_UNSAFE_TYPE)& CURRENT_SCH_UNSAFE))
            {
                sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeCurrent = *f_Value;
            }
            if((GetPV_Error(chn,  IV_UNSAFE_TYPE) & VCLAMP_SAFETY) || (GetPV_Error(chn,  IV_UNSAFE_TYPE) & VOLTAGE_SYS_UNSAFE) || (GetPV_Error(chn,  IV_UNSAFE_TYPE) & VOLTAGE_SCH_UNSAFE))
            {
                f_Value +=1;
                sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeVoltage = *f_Value;
            }
            if((GetPV_Error(chn,  IV_UNSAFE_TYPE) & POWER_SYS_UNSAFE) || (GetPV_Error(chn,  IV_UNSAFE_TYPE) & POWER_SCH_UNSAFE))
            {
                f_Value +=2;
                sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafePower = *f_Value;
            }
            if(GetPV_Error(chn,  IV_UNSAFE_TYPE) & CAPACITY_SCH_UNSAFE)
            {
                f_Value += 3;
                sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeCapacity = *f_Value;
            }
            if(GetPV_Error(chn,  IV_UNSAFE_TYPE)  & STEPTIME_SCH_UNSAFE)
            {
                f_Value += 4;
                sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeStepTime = *f_Value;
            }
            // Send to Schedule manager. Here, there might not be schedule running, but msg need to be sent each time safe->unsafe happens.
            Schedule_Manager_task::UnsafeContent[chn]=sch_msg;
            Schedule_Manager_task::SendMsgToSchedule(chn,sch_msg);
        }
    }
    else		// send unsafe to Sch
    {
        // Packing Msg
        sch_msg.MsgNumber = Sch_OnUnsafe_Msg;    //lock sch
        sch_msg.m_Message.m_Assign.m_u16ChannelIdx = chn;    //通道
        sch_msg.st_status_sch_running.m_btCMD_Source = ST_SAFECHECK;
        sch_msg.st_status_sch_running.m_bUnsafeType =  UnsafeType;
        sch_msg.st_status_sch_running.m_uw32Safe_Code = GetPV_Error(chn, UnsafeType);
		sch_msg.st_status_sch_running.m_btLost_Unit = CANFunc::SourceBoardID;
		
        //important    comment out chen 20130814
        ChannelInfo_Manager::ChannelInfo[chn].m_UnsafeType = UnsafeType;
        ChannelInfo_Manager::ChannelInfo[chn].m_SafetyStatus = GetPV_Error(chn, UnsafeType);
        if(UnsafeType==INVERTER_UNSAFE_TYPE)
        {
            sch_msg.st_status_sch_running.m_btLost_Unit = Schedule_Manager_task::UnsafeContent[chn].st_status_sch_running.m_btLost_Unit;
        }
        //根据不同触发类型来向sch汇报m_btSafe_ClassType  具体请参考    enum MP_SCH_SAFTY_TYPE in SF_Hwf.h
        //现在有与上位机不匹配的问题

        if(GetPV_Error(chn,  IV_UNSAFE_TYPE) & CTRL_VALUE_CHK_UNSAFE)
            sch_msg.st_status_sch_running.m_btSubCode = CtrlValueCheck[chn].ErrorCode;
        if(GetPV_Error(chn,  IV_UNSAFE_TYPE) & IRREGULAR_UNSAFE)
            sch_msg.st_status_sch_running.m_btSubCode = IrregularCheck[chn].ErrorCode;
        //add Wzh
        if(GetPV_Error(chn,  DCDC_UNSAFE_TYPE) & HARDWARE_UNSAFE)
            sch_msg.st_status_sch_running.m_btSubCode = HARDWARE_UNSAFE - 1;
        //add Wzh
        float * f_Value = &Schedule_Manager_task::UnsafeContent[chn].st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeCurrent;
        if(GetPV_Error(chn,  IV_UNSAFE_TYPE) & CTRL_VALUE_CHK_UNSAFE)    // 20200619
        {
            sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue = Schedule_Manager_task::UnsafeContent[chn].st_status_sch_running.m_Value.st_UnsafeMetaValue;
        }
        if((GetPV_Error(chn,  IV_UNSAFE_TYPE) & CURRENT_SYS_UNSAFE) || (GetPV_Error(chn,  IV_UNSAFE_TYPE) & CURRENT_SCH_UNSAFE))
        {
            sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeCurrent = *f_Value;
        }
        if((GetPV_Error(chn,  IV_UNSAFE_TYPE) & VCLAMP_SAFETY) || (GetPV_Error(chn,  IV_UNSAFE_TYPE) & VOLTAGE_SYS_UNSAFE) || (GetPV_Error(chn,  IV_UNSAFE_TYPE) & VOLTAGE_SCH_UNSAFE)
                ||(GetPV_Error(chn,  IV_UNSAFE_TYPE) & IV_CABLE_CONNECTION_UNSAFE) || (GetPV_Error(chn,  IV_UNSAFE_TYPE) & REDUNDANT_SAFETY))
        {
            f_Value +=1;
            sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeVoltage = *f_Value;
        }
        if((GetPV_Error(chn,  IV_UNSAFE_TYPE) & POWER_SYS_UNSAFE) || (GetPV_Error(chn,  IV_UNSAFE_TYPE) & POWER_SCH_UNSAFE))
        {
            f_Value +=2;
            sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafePower = *f_Value;
        }
        if(GetPV_Error(chn,  IV_UNSAFE_TYPE) & CAPACITY_SCH_UNSAFE)
        {
            f_Value += 3;
            sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeCapacity = *f_Value;
        }
        if(GetPV_Error(chn,  IV_UNSAFE_TYPE) & STEPTIME_SCH_UNSAFE)
        {
            f_Value += 4;
            sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeStepTime = *f_Value;
        }
        if(GetPV_Error(chn,  IV_UNSAFE_TYPE) & AMP_IR_UNSAFE)
        {
            f_Value +=5;  //3
            sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafePower = *f_Value;
        }
        if(GetPV_Error(chn,  IV_UNSAFE_TYPE) & SCH_UserDefined_UNSAFE)  //170122
        {
            sch_msg.st_status_sch_running.m_Value.st_UnsafeUD_Value.f_Value = UD_Record[chn];
            int UD_Idx = UD_UnsafeIndex[chn];
            if( (UD_Idx < 0) || (UD_Idx >= MAX_USERDEFINED_SAFETY) )
                UD_Idx = 0;
            St_Safety_UD  *pUDsafety = &ChannelInfo_Manager::ChannelSchedule[chn].m_Settings.m_Safety_UD[UD_Idx];
            sch_msg.st_status_sch_running.m_btSafetyUD_DataType = pUDsafety->m_DataType;
            sch_msg.st_status_sch_running.m_btSafetyUD_MetaCode = pUDsafety->m_MetaCode;
        }
        if(  (GetPV_Error(chn,  DCDC_UNSAFE_TYPE) & DCDC_DRIVER_ERROR)!=0 )
        {
            if(!((GetPV_Error(chn,  IV_UNSAFE_TYPE) & VOLTAGE_SYS_UNSAFE) || (GetPV_Error(chn,  IV_UNSAFE_TYPE) & VOLTAGE_SCH_UNSAFE)))
                sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeVoltage = ChannelInfo_Manager::Get_MetaVariable(chn, MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, MetaCode_PV_Voltage);
            if(!((GetPV_Error(chn,  IV_UNSAFE_TYPE) & CURRENT_SYS_UNSAFE) || (GetPV_Error(chn,  IV_UNSAFE_TYPE) & CURRENT_SCH_UNSAFE)))
                sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeCurrent = ChannelInfo_Manager::Get_MetaVariable(chn, MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, MetaCode_PV_Current);
            if(!((GetPV_Error(chn,  IV_UNSAFE_TYPE) & POWER_SYS_UNSAFE) || (GetPV_Error(chn,  IV_UNSAFE_TYPE) & POWER_SCH_UNSAFE)))
                sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafePower   = ChannelInfo_Manager::Get_MetaVariable(chn, MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, MetaCode_PV_Current)
                        *ChannelInfo_Manager::Get_MetaVariable(chn, MP_DATA_TYPE_METAVALUE,DEFAULT_OPERATOR,DEFAULT_COEFFICIENT, MetaCode_PV_Voltage);
            if(GetPV_Error(chn,  DCDC_UNSAFE_TYPE) & DCDC_FUSE_ERROR)
                sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeInverter_Fuse = Schedule_Manager_task::UnsafeContent[chn].st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeInverter_Fuse;
        }
        if (GetPV_Error(chn,  IV_UNSAFE_TYPE) & PARA_SUB_CUR_UNSAFE)
        {
            //TODO: 放入收到的subunit报来的chan idx和value
            sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeSubmasterValue = Submaster_Record;
        }
        if (GetPV_Error(chn,  IV_UNSAFE_TYPE) & PARA_SUB_VOL_UNSAFE)
        {
            //TODO: 放入收到的subunit报来的chan idx和value
            sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.f_UnsafeSubmasterValue = Submaster_Record;
        }

        // Send to Schedule manager. Here, there might not be schedule running, but msg need to be sent each time safe->unsafe happens.
        if(board_config::Feature_Enable_init.Redundant_Safety_Mode==1)
        {
            if((GetPV_Error(chn,  IV_UNSAFE_TYPE )&REDUNDANT_SAFETY)!=0)
                RedundantSafetyFlg[chn]=1;
        }
        if (ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode() ||ChannelInfo_Manager::Serial2ParaMod_IsMasterChannel(chn))
        {
            // Send to Schedule manager. Here, there might not be schedule running, but msg need to be sent each time safe->unsafe happens.
			sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.un_UnsafeSpecialInfo.st_UnsafeInfo.f_UnsafeChnID = chn + 1;
            Schedule_Manager_task::UnsafeContent[chn]=sch_msg;
            Schedule_Manager_task::SendMsgToSchedule(chn,sch_msg);
        }
        else if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitSubmasterChannel(chn))
        {
            //send CAN msg to master chan
            CANBusParse::Parallel_SubmasterSend_ReportUnsafe(GetPV_Error(chn, UnsafeType), Submaster_Record,chn);
        }
        else if (ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitSlaveChannel(chn)||ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMsUnitSlaveChannel(chn))    // 20161021 yangy
        {
            unsigned char ReprotChn=ChannelInfo_Manager::gUnit_stUnitInfo.m_GroupInfo[ChannelInfo_Manager::ChannelInfo[chn].m_GroupID].m_UnitGroupInfo.m_HeadChn;
			sch_msg.st_status_sch_running.m_Value.st_UnsafeMetaValue.un_UnsafeSpecialInfo.st_UnsafeInfo.f_UnsafeChnID = chn + 1;
            Schedule_Manager_task::UnsafeContent[ReprotChn]=sch_msg;
            sch_msg.m_Message.m_Assign.m_u16ChannelIdx = ReprotChn;
//             ChannelInfo_Manager::ChannelInfo[chn].m_UnsafeType = UnsafeType;
            ChannelInfo_Manager::ChannelInfo[ReprotChn].m_UnsafeType = UnsafeType;
            ChannelInfo_Manager::ChannelInfo[ReprotChn].m_SafetyStatus = GetPV_Error(chn, UnsafeType);
            Schedule_Manager_task::SendMsgToSchedule(ReprotChn,sch_msg);

            Schedule_Manager_task::UnsafeContent[chn]=sch_msg;

        }
        else if(ChannelInfo_Manager::Serial2ParaMod_IsInterUnitSubUnitSlaveChannel(chn))
        {
            CANBusParse::Parallel_SubmasterSend_ReportUnsafe(GetPV_Error(chn, UnsafeType), Submaster_Record,chn);
        }
    }
}
char safety_check::resumeError(unsigned char chn)
{
    //发送恢复
    St_ScheduleMessage sch_msg;
    sch_msg.m_Message.m_Assign.m_u16ChannelIdx = chn;    //通道
    //ChannelInfo_Manager::ChannelInfo[chn].m_SafetyStatus = 0;  //Resume的时候这个条件没有必要清除掉 Unsafe的时候直接可以覆盖 0902 dirui
    sch_msg.MsgNumber = Sch_OnSafe_Msg;    //unlock
    Schedule_Manager_task::LockModeSwitch[chn]=0; //解除安全标记  0902
    memset(&Schedule_Manager_task::UnsafeContent[chn],0,sizeof(St_ScheduleMessage));
    Schedule_Manager_task::SendMsgToSchedule(chn,sch_msg);

    return 1;
}

char safety_check::safety_check_sch_is_ready(unsigned char chn)
{
    if (!ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode())
    {
        if (!ChannelInfo_Manager::Serial2ParaMod_IsMasterChannel(chn))
        {
            return SAFETY_CHECK_SYS;
        }
    }
    //	#endif
    //检查当前的是否在运行sch和第三方还是idle
    //如果是sch  则返回1
    //这里或许有问题
    //comment out chen 20130219

    if(ChannelInfo_Manager::ChannelInfo[chn].chn_Running_Flag != CHN_LOCK_SCH)
    {
        return SAFETY_CHECK_SYS;
    }
    else    //说明在sch状态
    {
        return SAFETY_CHECK_SCH;
    }
}

void safety_check::InitAbnormalCheck(unsigned char ChannelIdx)
{
    IrregularCheck[ChannelIdx].Counter        = 0;
    IrregularCheck[ChannelIdx].MaxCount       = ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_Settings.m_AbnormalCheck.VTL_CheckCount;
    IrregularCheck[ChannelIdx].fErrorVoltage  = ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_TestObject.m_fMaxAllowVoltage * ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_Settings.m_AbnormalCheck.VTL_Threshold * 0.01f;//IRREGULAR_CHECK_ERRORVOLT;
    IrregularCheck[ChannelIdx].bNeedCheck     = ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_Settings.m_AbnormalCheck.VTL_IsNeedCheck;
    CtrlValueCheck[ChannelIdx].Counter        = 0;
    CtrlValueCheck[ChannelIdx].bWarning       = 0;

    ClrUnsafe(ChannelIdx,CTRL_VALUE_CHK_UNSAFE,IV_UNSAFE_TYPE);
    ClrUnsafe(ChannelIdx,IRREGULAR_UNSAFE,IV_UNSAFE_TYPE);
}
void          safety_check::MakeIrregularCheck(void)
{   //异常检查---合理性
    if(m_IrregularSetting.bChecking == 0)
    {
        if(GetSystemTimeInSecond() >= m_IrregularSetting.NextTime.Second)
        {   //时间周期到了，而且现在没有检查，则开启检查
            m_IrregularSetting.NextTime.Second = GetSystemTimeInSecond() + IRREGULAR_CHECK_PERIOD; //记录检查周期启动时间
            m_IrregularSetting.bChecking = 1;                       //标志进入检查周期
            m_IrregularSetting.ChannelIdx = 0;                      //从第一通道开始
        }
    }
    if(m_IrregularSetting.bChecking)
    {   //需要检查了，每次只查一个通道，以节约时间消耗
        if(IrregularCheck[m_IrregularSetting.ChannelIdx].bNeedCheck)
        {
            bool bError = CheckIrrationality(m_IrregularSetting.ChannelIdx);
            if (!ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode())
            {
                if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitSubmasterChannel(m_IrregularSetting.ChannelIdx) )
                {
                    if(bError)
                    {
                        SetUnsafe(m_IrregularSetting.ChannelIdx,IRREGULAR_UNSAFE,0, IV_UNSAFE_TYPE);  //stop submaster channel first.
                        CANBusParse::Parallel_SubmasterSend_ReportIrregulation(m_IrregularSetting.ChannelIdx, IrregularCheck[m_IrregularSetting.ChannelIdx].ErrorCode);
                    }
                    return;
                }
            }
            if(bError)
            {
                SetUnsafe(m_IrregularSetting.ChannelIdx,IRREGULAR_UNSAFE,0, IV_UNSAFE_TYPE);
            }
            else
            {
                ClrUnsafe(m_IrregularSetting.ChannelIdx,IRREGULAR_UNSAFE,IV_UNSAFE_TYPE);
            }
        }

        m_IrregularSetting.ChannelIdx++;
        if(m_IrregularSetting.ChannelIdx >= ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount)
            m_IrregularSetting.bChecking = 0;
    }
}
unsigned char safety_check::CheckIrrationality(unsigned char ChannelIdx)
{
    if(!ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_Settings.m_AbnormalCheck.VTL_IsNeedCheck) //180827 zyx
        return 0;
    if(ChannelInfo_Manager::ChannelInfo[ChannelIdx].ScheduleStatus != RUNNING)
        return 0;
    float fVoltage   = ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_Voltage].m_fValue;

    if(fVoltage < IrregularCheck[ChannelIdx].fErrorVoltage)
    {
        IrregularCheck[ChannelIdx].Counter++;
    }
    else
    {
        if(IrregularCheck[ChannelIdx].Counter > 0)
            IrregularCheck[ChannelIdx].Counter--;
    }
    if(IrregularCheck[ChannelIdx].Counter > IrregularCheck[ChannelIdx].MaxCount) //IRREGULAR_VOLT_CHECK_COUNT
    {
        IrregularCheck[ChannelIdx].ErrorCode = IRREGULAR_ERROR_Voltage;
        return 1;
    }

    return 0;
}
void safety_check::MakeCtrlValueCheck(unsigned char ChannelIdx) //1805222 zyx
{   //异常检查---控制值
    if(!ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_Settings.m_AbnormalCheck.CRM_IsNeedCheck)
        return;
    if(m_CtrlValueSetting[ChannelIdx].bChecking == 0)
    {
        if(GetSystemTimeInSecond() >= m_CtrlValueSetting[ChannelIdx].NextTime.Second)
        {   //时间周期到了，而且现在没有检查，则开启检查
            m_CtrlValueSetting[ChannelIdx].NextTime.Second = GetSystemTimeInSecond() + CTRL_VALUE_CHECK_PERIOD;//ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_Settings.m_AbnormalCheck.CRM_CheckTime;//CTRL_VALUE_CHECK_PERIOD; //记录检查周期启动时间 //180808 zyx
            m_CtrlValueSetting[ChannelIdx].bChecking = 1;                       //标志进入检查周期
        }
    }
    if(m_CtrlValueSetting[ChannelIdx].bChecking)
    {   //需要检查了，每次只查一个通道，以节约时间消耗
        bool bError = CheckCtrlValue(ChannelIdx);
        if (!ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode())
        {
            if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitSubmasterChannel(ChannelIdx) )
            {
                if(bError)
                {
                    SetUnsafe(ChannelIdx, CTRL_VALUE_CHK_UNSAFE, 0, IV_UNSAFE_TYPE);  //stop submaster channel first.
                }
                return;
            }
        }
        if(bError)
        {
            SetUnsafe(ChannelIdx, CTRL_VALUE_CHK_UNSAFE, 0, IV_UNSAFE_TYPE);
        }
        else
        {
            //ClrUnsafe(m_CtrlValueSetting[ChannelIdx].ChannelIdx,CTRL_VALUE_CHK_UNSAFE,IV_UNSAFE_TYPE);
            ClrUnsafe(ChannelIdx,CTRL_VALUE_CHK_UNSAFE,IV_UNSAFE_TYPE);
        }
        m_CtrlValueSetting[ChannelIdx].bChecking = 0;                       //标志进入检查周期 //1805222 zyx
    }
}
unsigned char safety_check::CheckCtrlValue(unsigned char ChannelIdx)
{
    float fValue = 0.0f;
    float fCurrent = ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_Current].m_fValue;
    float fVoltage = ChannelInfo_Manager::MetaVariable2[ChannelIdx][MetaCode_PV_Voltage].m_fValue;
    switch(CtrlValueCheck[ChannelIdx].CheckType)
    {
    case CtrlValueCheck_Current:
        fValue = fCurrent;
        break;
    case CtrlValueCheck_Voltage:
        fValue = fVoltage;
        break;
    case CtrlValueCheck_Power:
        fValue = fCurrent * fVoltage;
        break;
    case CtrlValueCheck_Null:
    default:
        CtrlValueCheck[ChannelIdx].Counter = 0;
        return 0;
    }

    bool bClampOverflow = false;
    bool bClampIsBoss = false;
    if(CtrlValueCheck[ChannelIdx].bCtrlCurrClampV)
    {
        if(::abs(fVoltage - ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_fVclampHigh) < CtrlValueCheck[ChannelIdx].fClampVerror)
            bClampIsBoss = true;
        else if(::abs(fVoltage - ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_fVclampLow) < CtrlValueCheck[ChannelIdx].fClampVerror)
            bClampIsBoss = true;

        if((fCurrent >= 0)&&(fVoltage > ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_fVclampHigh + CtrlValueCheck[ChannelIdx].fClampVerror))
            bClampOverflow = true;
        else if((fCurrent < 0)&&(fVoltage < ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_fVclampLow - CtrlValueCheck[ChannelIdx].fClampVerror))
            bClampOverflow = true;
    }

    if(!bClampIsBoss && ((::abs(fValue - CtrlValueCheck[ChannelIdx].fCtrlValue) > CtrlValueCheck[ChannelIdx].fError)||bClampOverflow))
    {
        if(CtrlValueCheck[ChannelIdx].Counter < 0)
            CtrlValueCheck[ChannelIdx].Counter = 0;
        CtrlValueCheck[ChannelIdx].Counter++;     //这个Counter理论上有可能会爆满，但是实际上是不会的，因为这个需要持续测试几千年!
    }
    else
    {
        if(CtrlValueCheck[ChannelIdx].Counter <= 0)
            CtrlValueCheck[ChannelIdx].Counter = 0;
        else
            CtrlValueCheck[ChannelIdx].Counter--;
    }
    if(CtrlValueCheck[ChannelIdx].Counter > ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_Settings.m_AbnormalCheck.CRM_CheckCount)//CTRL_VALUE_CHECK_COUNT) //180808 zyx
        return 1;
    else
        return 0;
}
void safety_check::InitCtrlValue(unsigned char ChannelIdx,unsigned char CheckType,unsigned char VoltRange,unsigned char CurrRange)
{
    CtrlValueCheck[ChannelIdx].CheckType = CheckType;
    CtrlValueCheck[ChannelIdx].Counter   = 0;
    CtrlValueCheck[ChannelIdx].fError    = 0.0f;
    CtrlValueCheck[ChannelIdx].bCtrlCurrClampV = false;
    CtrlValueCheck[ChannelIdx].fClampVerror = 0.0f;

    float fVHigh,fVLow,fIHigh,fILow;
    St_ChannelCFG * p_Chn_CFG = &ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx];

    fIHigh = p_Chn_CFG->m_IRange[CurrRange].m_Bound.m_High;
    fILow  = p_Chn_CFG->m_IRange[CurrRange].m_Bound.m_Low;
    fVHigh = p_Chn_CFG->m_VRange[VoltRange].m_Bound.m_High;
    fVLow  = p_Chn_CFG->m_VRange[VoltRange].m_Bound.m_Low;
    fIHigh = ::abs(fIHigh);
    fILow  = ::abs(fILow);
    fVHigh = ::abs(fVHigh);
    fVLow  = ::abs(fVLow);
    if(fIHigh < fILow)
        fIHigh = fILow;
    if(fVHigh < fVLow)
        fVHigh = fVLow;

    CtrlValueCheck[ChannelIdx].fClampVerror = ::abs(fVHigh * 0.02f);
    m_CtrlValueSetting[ChannelIdx].fErrorFactor = ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_Settings.m_AbnormalCheck.CRM_Deviation * 0.01;
    switch (CheckType)
    {
    case CtrlValueCheck_Current:
        CtrlValueCheck[ChannelIdx].fError = fIHigh * m_CtrlValueSetting[ChannelIdx].fErrorFactor * ChannelInfo_Manager::GetTotalChanNumOfParaGroup(ChannelIdx);//2110091 zyx
        CtrlValueCheck[ChannelIdx].ErrorCode = CTRL_VALUE_ERROR_Current;
        CtrlValueCheck[ChannelIdx].bCtrlCurrClampV = board_config::DacInfo.m_bHaveClampVdac;
        break;
    case CtrlValueCheck_Voltage:
        CtrlValueCheck[ChannelIdx].fError = fVHigh * m_CtrlValueSetting[ChannelIdx].fErrorFactor;
        CtrlValueCheck[ChannelIdx].ErrorCode = CTRL_VALUE_ERROR_Voltage;
        CtrlValueCheck[ChannelIdx].bCtrlCurrClampV = (ChannelInfo_Manager::ChannelInfo[ChannelIdx].bDigitalV && board_config::DacInfo.m_bHaveClampVdac);
        break;
    case CtrlValueCheck_Power:
        CtrlValueCheck[ChannelIdx].fError = fIHigh * fVHigh * m_CtrlValueSetting[ChannelIdx].fErrorFactor * ChannelInfo_Manager::GetTotalChanNumOfParaGroup(ChannelIdx);//2110091 zyx
        CtrlValueCheck[ChannelIdx].ErrorCode = CTRL_VALUE_ERROR_Power;
        CtrlValueCheck[ChannelIdx].bCtrlCurrClampV = board_config::DacInfo.m_bHaveClampVdac;
        break;
    case CtrlValueCheck_Null:
    default:
        break;
    }
}
//add Wzh
void safety_check::MakeHwErrCheck(unsigned char chn)
{
    if(board_config::Func_CheckError != 0)
    {
        char ErrorCode = board_config::Func_CheckError(chn,0);
        if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_PWM)
        {
            if(board_config::Feature_Enable_init.PWM_Version_Control==0)
            {
                if(ErrorCode == 1)							//normal err
                {
                    Hw_Err_Flag[chn] = 1;
                }
                else if(ErrorCode == 2)					//RST err
                {
                    Hw_Err_Flag[chn] = 1;
                }
                else
                {
                    Hw_Err_Flag[chn] = 0;					//no err
                }

                if(Hw_Err_Flag[chn] == 1)
                {
#if _LOCK_ERROR_ == 1
                    SetUnsafe(chn, HARDWARE_UNSAFE, 0.0f,DCDC_UNSAFE_TYPE);
#else
                    SetUnsafe(chn, HARDWARE_UNSAFE, 0.0f,DCDC_UNSAFE_TYPE);
#endif
                }
            }
            else
            {
                switch (ErrorCode)
                {
                case 1:
                    SetUnsafe(chn, DCDC_DRIVER_ERROR, 0,DCDC_UNSAFE_TYPE);
                    break;
                case 2:
                    SetUnsafe(chn, HARDWARE_UNSAFE, 0,DCDC_UNSAFE_TYPE);
                    break;
                case 4:
                    SetUnsafe(chn, DCLINK_OVERVOLTAGE_ERROR, 0,DCDC_UNSAFE_TYPE);
                    break;
                case 8:
                    SetUnsafe(chn, DCDC_FUSE_ERROR, 0,DCDC_UNSAFE_TYPE);
                    break;
                case 0:
                default:
                    ClrUnsafe(chn, DCDC_DRIVER_ERROR,DCDC_UNSAFE_TYPE);
                    ClrUnsafe(chn, HARDWARE_UNSAFE,DCDC_UNSAFE_TYPE);
                    ClrUnsafe(chn, DCLINK_OVERVOLTAGE_ERROR,DCDC_UNSAFE_TYPE);
                    ClrUnsafe(chn, DCDC_FUSE_ERROR,DCDC_UNSAFE_TYPE);
                    break;
                }
            }

        }

    }
}

void safety_check::SetRedundantSafetyFlgCheck(unsigned char chn,float f_Value)
{
    RedundantSafetyFlg[chn]=1;
    SetUnsafe(chn, REDUNDANT_SAFETY, f_Value, IV_UNSAFE_TYPE);
}

void safety_check::ReleaseRedundantSafetyFlgCheck(unsigned char chn)
{
    safety_check::ClrUnsafe(chn, REDUNDANT_SAFETY,IV_UNSAFE_TYPE);
}

unsigned char safety_check::VClampRangeCheck(unsigned char ChannelIdx,float Value)
{
    unsigned char uc_CheckVClamp = 0;
    float f_H,f_L,f_D;

    if (ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode())  // 没并联    // 增加提前检查所设置的钳压是否正常 yy  20160216
        uc_CheckVClamp = 1;
    else if (ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(ChannelIdx))  // MCU内并联主
        uc_CheckVClamp = 1;
    else if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(ChannelIdx))  // 多MCU并联主MCU
        uc_CheckVClamp = 1;
    else if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitSubmasterChannel(ChannelIdx)) // 多MCU并联从MCU的主
        uc_CheckVClamp = 1;

    unsigned char Range = ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_ucVoltageRange;
    if(Range > VOLTAGE_RANGE_HIGH)
        Range = VOLTAGE_RANGE_HIGH;

    f_D = abs(ChannelInfo_Manager::BoardCFG.m_Channel[ChannelIdx].m_VRange[Range].m_Bound.m_High)*0.01f;
    f_H = ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_fVclampHigh + f_D;   // 放大量程的1%;
    f_L = ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_fVclampLow - f_D;

    if(uc_CheckVClamp)
    {
        if((Value > f_H )
                ||(Value < f_L))   // 每次运行前检查所设置的钳压是否正常
        {
            return 0; // 超范围失败
        }
    }
    return 1;  // 未超范围成功
}

//检查该通道是否想控制温控箱，是否其他通道已控制温控箱
unsigned char safety_check::TemperatureChamberCheck(unsigned char ChannelIdx)
{
    char sc_SchBufIdx = ChannelInfo_Manager::ChannelSchedule[ChannelIdx].m_scBufIdx;
    if((sc_SchBufIdx < 0) || (sc_SchBufIdx >= IV_MAX_SCHEDULE_BUFFER_NUMBER))
        return 0;

    unsigned short StepCount  = ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_ItemCount.m_u16StepCount;//该Schedule总步数

    for( int i = 0; i < StepCount; i++)
    {
        short TestSettingIdx = ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_StepData[i].m_Ctrl.m_ucTestSettingIdx;
        if(TestSettingIdx >= 0)
        {
            const St_TestSetting *pMyTestSetting = &ChannelInfo_Manager::ScheduleBuf[sc_SchBufIdx].m_TestSetting[TestSettingIdx];
            for(unsigned char elementIdx = 0; elementIdx<pMyTestSetting->m_ucElementNum; elementIdx++)
                if(((pMyTestSetting->m_TestSettingElement[elementIdx].m_ucType == AUX_TEMPERATURE)
                        ||(pMyTestSetting->m_TestSettingElement[elementIdx].m_ucType == AUX_HUMIDITY)) //AUX_FLOW_RATE改为AUX_HUMIDITY
                        && pMyTestSetting->m_TestSettingElement[elementIdx].m_ucTurnOn)
                {
                    //=============changed by zc 04.25.2018==============================
                    unsigned char AuxVirtIndx = pMyTestSetting->m_TestSettingElement[elementIdx].m_uwAuxVirtIndx;
                    unsigned char AuxType = pMyTestSetting->m_TestSettingElement[elementIdx].m_ucType;
                    St_AUXchannelInfo MyInfo = Channel_MapAuxInfo::GetAuxChannelInfo(ChannelIdx, AuxType, AuxVirtIndx); //获取AUX物理通道号和所属ID（ip）

                    //温控箱已被占用，但该Channel企图控制温控箱，该Schudule有误，应禁止其开启
                    if((((Schedule_Manager_task::TemperatureChamberSwitch[MyInfo.m_UnitId].usingFlag>>MyInfo.m_PhysicalChannelIdx)&0x01) == 0x01) && Schedule_Manager_task::TemperatureChamberSwitch[MyInfo.m_UnitId].uc_channel[MyInfo.m_PhysicalChannelIdx] != ChannelIdx)
                        return 0;
                    //如果温控箱未被占用
                    else if((((Schedule_Manager_task::TemperatureChamberSwitch[MyInfo.m_UnitId].usingFlag>>MyInfo.m_PhysicalChannelIdx)&0x01) == 0x00) && Schedule_Manager_task::TemperatureChamberSwitch[MyInfo.m_UnitId].uc_channel[MyInfo.m_PhysicalChannelIdx] == 0xFF)
                    {
                        Schedule_Manager_task::UseTemperatureChamber( ChannelIdx, MyInfo.m_UnitId, MyInfo.m_PhysicalChannelIdx );
                        //return 1;
                    }
                    //=============end 04.25.2018==============================

                }

        }

    }
    return 1;
}

void safety_check::SetPV_Error(unsigned char chn, uint32 unsafeCode, unsigned char UnsafeType )
{
    unsigned char PV_Error_Type = 0;
    switch (UnsafeType)
    {
    case IV_UNSAFE_TYPE:
        PV_Error_Type = MetaCode_PV_Error_IV ;
        break;
    case AUX_UNSAFE_TYPE:
        PV_Error_Type = MetaCode_PV_Error_AUX ;
        break;
    case DCDC_UNSAFE_TYPE:
        PV_Error_Type = MetaCode_PV_Error_DCDC ;
        break;
    case CANBMS_UNSAFE_TYPE:
        PV_Error_Type = MetaCode_PV_Error_CANBMS ;
        break;
    case SMB_UNSAFE_TYPE:
        PV_Error_Type = MetaCode_PV_Error_SMB ;
        break;
    case INVERTER_UNSAFE_TYPE:
        PV_Error_Type = MetaCode_PV_Error_INVERTER ;
        break;
    default:
        return;
    }
    uint32 *PV_Error_Code = (uint32*)&ChannelInfo_Manager::MetaVariable2[chn][PV_Error_Type].m_fValue;
    *PV_Error_Code |= unsafeCode;
}
void safety_check::ClrUnsafe(unsigned char chn, uint32 unsafeCode, unsigned char UnsafeType )
{
    unsigned char PV_Error_Type = 0;
    switch (UnsafeType)
    {
    case IV_UNSAFE_TYPE:
        PV_Error_Type = MetaCode_PV_Error_IV ;
        break;
    case AUX_UNSAFE_TYPE:
        PV_Error_Type = MetaCode_PV_Error_AUX ;
        break;
    case DCDC_UNSAFE_TYPE:
        PV_Error_Type = MetaCode_PV_Error_DCDC ;
        break;
    case CANBMS_UNSAFE_TYPE:
        PV_Error_Type = MetaCode_PV_Error_CANBMS ;
        break;
    case SMB_UNSAFE_TYPE:
        PV_Error_Type = MetaCode_PV_Error_SMB ;
        break;
    case INVERTER_UNSAFE_TYPE:
        PV_Error_Type = MetaCode_PV_Error_INVERTER ;
        break;
    default:
        return;
    }
    uint32 *PV_Error_Code = (uint32*)&ChannelInfo_Manager::MetaVariable2[chn][PV_Error_Type].m_fValue;
    *PV_Error_Code &= ~unsafeCode;
}
void safety_check::ClearAll_PV_Error(unsigned char chn)
{
    ChannelInfo_Manager::MetaVariable2[chn][MetaCode_PV_Error_IV].m_fValue = 0;
    ChannelInfo_Manager::MetaVariable2[chn][MetaCode_PV_Error_AUX].m_fValue = 0;
    ChannelInfo_Manager::MetaVariable2[chn][MetaCode_PV_Error_DCDC].m_fValue = 0;
    ChannelInfo_Manager::MetaVariable2[chn][MetaCode_PV_Error_CANBMS].m_fValue = 0;
    ChannelInfo_Manager::MetaVariable2[chn][MetaCode_PV_Error_SMB].m_fValue = 0;
    ChannelInfo_Manager::MetaVariable2[chn][MetaCode_PV_Error_INVERTER].m_fValue = 0;
}
uint32 safety_check::GetPV_Error(unsigned char chn,  unsigned char UnsafeType )
{
    uint32 ErrorCode = 0;
    unsigned char PV_Error_Type = 0;
    switch (UnsafeType)
    {
    case IV_UNSAFE_TYPE:
        PV_Error_Type = MetaCode_PV_Error_IV ;
        break;
    case AUX_UNSAFE_TYPE:
        PV_Error_Type = MetaCode_PV_Error_AUX ;
        break;
    case DCDC_UNSAFE_TYPE:
        PV_Error_Type = MetaCode_PV_Error_DCDC ;
        break;
    case CANBMS_UNSAFE_TYPE:
        PV_Error_Type = MetaCode_PV_Error_CANBMS ;
        break;
    case SMB_UNSAFE_TYPE:
        PV_Error_Type = MetaCode_PV_Error_SMB ;
        break;
    case INVERTER_UNSAFE_TYPE:
        PV_Error_Type = MetaCode_PV_Error_INVERTER ;
        break;
    default:
        return 0;
    }
    memcpy(&ErrorCode, &ChannelInfo_Manager::MetaVariable2[chn][PV_Error_Type].m_fValue, 4);
    return ErrorCode;
}
uint8 safety_check::GetPV_ErrorType(unsigned char chn)//只查找动态的错误码
{
    if((GetPV_Error(chn, IV_UNSAFE_TYPE)&( CURRENT_SYS_UNSAFE|CURRENT_SCH_UNSAFE\
                                           |POWER_SYS_UNSAFE|POWER_SCH_UNSAFE\
                                           |VOLTAGE_SYS_UNSAFE|VOLTAGE_SCH_UNSAFE|VCLAMP_SAFETY\
                                           |STEPTIME_SCH_UNSAFE|CAPACITY_SCH_UNSAFE\
                                           |FAN_FAULT_UNSAFE|SCH_UserDefined_UNSAFE\
                                           |CTRL_VALUE_CHK_UNSAFE|IRREGULAR_UNSAFE|REDUNDANT_SAFETY\
                                           |AMP_IR_UNSAFE|POWER_COMMUNICATION_FAIL|POWER_FEEDBACK_ERROR\
                                           |PARA_SUB_CUR_UNSAFE|PARA_SUB_VOL_UNSAFE|IV_TEMPERATURE_UNSAFE\
                                           |IV_CABLE_CONNECTION_UNSAFE))!=0)
        return IV_UNSAFE_TYPE;

    if((GetPV_Error(chn, INVERTER_UNSAFE_TYPE))!=0)
        return INVERTER_UNSAFE_TYPE;

    if((GetPV_Error(chn, DCDC_UNSAFE_TYPE))!=0)
        return DCDC_UNSAFE_TYPE;



    return 0xFF;
}
void safety_check::SetBuiltInSafetyPara(unsigned char uc_ChannelNo)
{
    safety_check::BuiltinAux_Check[uc_ChannelNo].m_bIfCheckFlag = 0;
    if(ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_SchAux_Glb_Voltage.m_bEnabled)
    {
        int AuxIndex = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_SchAux_Glb_Voltage.m_SafetyStartIndex;
        safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_V.m_fHigh = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AuxSafty[AuxIndex].m_fMax;
        safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_V.m_fLow = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AuxSafty[AuxIndex].m_fMin;
        safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_V.m_bIfCheck |= (1 << 0); //AUX_VOLTAGE;
        safety_check::BuiltinAux_Check[uc_ChannelNo].m_bIfCheckFlag |= (1 << 0);
    }
    if(ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_SchAux_Glb_Temperature.m_bEnabled) // 设置BUILT-IN 温度安全
    {
        int AuxIndex = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_SchAux_Glb_Temperature.m_SafetyStartIndex;
        safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_T.m_fHigh = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AuxSafty[AuxIndex].m_fMax;
        safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_T.m_fLow = ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AuxSafty[AuxIndex].m_fMin;
        safety_check::BuiltinAux_Check[uc_ChannelNo].checkSCH_Aux_T.m_bIfCheck |= (1 << 1); //AUX TEMPERATURE;
        safety_check::BuiltinAux_Check[uc_ChannelNo].m_bIfCheckFlag |= (1 << 1);
    }

}
