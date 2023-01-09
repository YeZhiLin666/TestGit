//改名为METAVARIABE
//MV维护

#include "../Entry/includes.h"
#include "AuxMetavariable.h"

//#include "app/manage/DataManage.h"   //大数据体

#define MVST_DXDT_WAIT 				11
#define MVST_DXDT_CALCULATE 		12
#define MVST_DXDT_LOG 				13

UInt16 MetaVariable_Task::Priority = PRIORITY4;   // PRIORITY8
UInt16 MetaVariable_Task::TaskID = MV_TASK;			//  should be a definition which positioning this task in OS_Task[]


#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif

ST_DXDT_VALUE     MetaVariable_Task::st_History[MAX_DATA_GROUP][MAX_AUXCH_PER_DATA_GROUP];
MsTime MetaVariable_Task::LastSentEnergyTime;
char  MetaVariable_Task::TimeRecordFlag;
int MetaVariable_Task::Cap_Calcu_Ptr;

MsTime MetaVariable_Task::m_capacity_Time[8];    //电量时间
char MetaVariable_Task::capacityStatus[8];
float MetaVariable_Task::ChargeEnergy[8];
float MetaVariable_Task::ChargeCapacity[8];
float MetaVariable_Task::DischargeEnergy[8];
float MetaVariable_Task::DisChargeCapacity[8];
char  MetaVariable_Task::MetaTaskRunCounter;
char  MetaVariable_Task::Inc_Dec_Flag[8];
unsigned char  MetaVariable_Task::C_E_Flag = 0;
#pragma arm section //结束外存变量定义

void MetaVariable_Task::Init(void)
{
    //全部置0
    for(unsigned char i = 0; i < 8 ; i++)    //遍历通道
    {
        capacityStatus[i] = 0;
        Inc_Dec_Flag[i] = 0;      //1:充电     0：放电
    }
    memset(&st_History,0,sizeof(st_History));
    Cap_Calcu_Ptr = 0;
    MetaTaskRunCounter=0;
    LastSentEnergyTime.Us100=0;
    LastSentEnergyTime.Second=0;
    TimeRecordFlag=0;

}

void MetaVariable_Task::DoWork(void)
{
    MsTime send_time;
    memset(&send_time,0,sizeof(MsTime));

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_META] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_META);
#endif

//    calculate_AuxdXdt();

    if(C_E_Flag == 0)
        return;

    if(MetaTaskRunCounter==0)
    {
        return;
    }
    calculate_capacity(0, 99);
    if(TimeRecordFlag==0)
    {
        CommonFunc::GetTimeTicksUpdate(&LastSentEnergyTime);
        TimeRecordFlag=1;
    }
    else
    {
        send_time= CommonFunc::TimeDiff(LastSentEnergyTime);
        if(send_time.Second>=1)
        {
            for(unsigned char i=0; i<8; i++)
            {
                if(capacityStatus[i] == 1)
                {
                    if(Inc_Dec_Flag[i] == 1)
                    {
                        CANBusParse::Aux_RptMetaData(i,0,ChargeEnergy[i]);
                        CANBusParse::Aux_RptMetaData(i,2,ChargeCapacity[i]);
                    }
                    else
                    {
                        CANBusParse::Aux_RptMetaData(i,1,DischargeEnergy[i]); // 发送能量电量
                        CANBusParse::Aux_RptMetaData(i,3,DisChargeCapacity[i]);
                    }
                }
            }
            TimeRecordFlag=0;
        }
    }

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_META] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
		Time_Count::thread_time(COUNT_META);
#endif
}

void MetaVariable_Task::EndTask(void)
{

}


/* requestType, 0 : not mandatory, only do capacity calculation when new sampling is available.
// requestType, 1 : mandatory, do capacity calcuation no matter new sampling is available or not.
// this usually happens at the end of of a step. The correct order of operation is:
// 1. cmd or conditions require a step to stop.
// 2. do mandatory cap calcu to updating the final capacity value.
// 3. record all data that needed by this event.
// 4. start stopping this step(transition process)*/
void MetaVariable_Task::calculate_capacity(char requestType, char chn)
{
    int i;
    float fCapacity = 0.0f;
    float fEnergy = 0.0f;
    float time = 0.0f;
    MsTime temp_time;
    memset(&temp_time,0,sizeof(MsTime));
    unsigned char uc_Bid = 0;
    unsigned char uc_AuxType;  // qjm 09.28.2015
    unsigned char uc_ChIdx;
    unsigned char uc_data_group_idx,uc_chn;

    // if not mandatory cap, Cycling channel # being processed.
    if(requestType == 0)
    {
        chn = 99;
        // ??????,???????????,???????????,???????????
        // since we need also update the status and timerecord of some channels,
        // we don't start from Cap_Calcu_Ptr and we have to go through all channels.
        //	for(i = 0; i < MAXCHANNELNO; i ++)
        for(i = 0; i < 8 ; i ++)
        {
            if(capacityStatus[i] == 1)
            {
                if(chn == 99)
                {
                    chn = i;
                }
                else if(chn <= Cap_Calcu_Ptr && i > Cap_Calcu_Ptr)
                {
                    chn = i;
                }
            } // endif
        }// end for

        // if we want to do time interval control in future, we can do it here by recognizing the change pattern of Cap_Calcu_Ptr <<<LATER>>>-philip 8/6/2013-
        // e.g. if(chn!= 99){ if(chn <= Cap_Calcu_Ptr) { //means another round here,
        // a 5ms time interval need to be waited by using State machine. and Cap_Calcu_Ptr is set to 0. etc.} }
        if(chn != 99)		// if found, reset Cap_Calcu_Ptr
        {
            Cap_Calcu_Ptr = chn;
        }
    }
    else	// if mandatory cap process request. check if valid.
    {
        if(capacityStatus[chn] == 0)
        {
            chn = 99;
        }
    }

    if(chn != 99)	// if next valid channel is found.
    {
        // 		ChannelInfo_Manager::Isamp_NeedRecalculateSign->m_Bits.CalCapacity = 0;
        // 		ChannelInfo_Manager::Vsamp_NeedRecalculateSign->m_Bits.CalCapacity = 0;

        temp_time = CommonFunc::TimeDiff(m_capacity_Time[chn]);    //?????last???
        CommonFunc::GetTimeTicksUpdate(&m_capacity_Time[chn]);    //??????
        time = temp_time.Us100 * 2.777777e-8f; 		// = * 1.0e-5 / 3600

        uc_AuxType = AUX_EXTERNALCHARGE;
        uc_data_group_idx = Aux_DataManager::m_AuxChanLongSnake[(Aux_DataManager::m_AuxChnLookupTable[uc_AuxType].m_StartInSnake+ chn)].m_data_group_idx;
        uc_chn = Aux_DataManager::m_AuxChanLongSnake[(Aux_DataManager::m_AuxChnLookupTable[uc_AuxType].m_StartInSnake+ chn)].m_chn;
        uc_Bid = Aux_DataManager::m_DGIdx2BIdx[uc_data_group_idx];

        fCapacity = Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn].aux_data * time;

        // 		uc_AuxType = AUX_VOLTAGE;
        // 		uc_data_group_idx = Aux_DataManager::m_AuxChanLongSnake[(Aux_DataManager::m_AuxChnLookupTable[uc_AuxType].m_StartInSnake+ chn)].m_data_group_idx;
        // 		uc_chn = Aux_DataManager::m_AuxChanLongSnake[(Aux_DataManager::m_AuxChnLookupTable[uc_AuxType].m_StartInSnake+ chn)].m_chn;
        // 		uc_Bid = Aux_DataManager::m_DGIdx2BIdx[uc_data_group_idx];
        fEnergy = fCapacity * Aux_DataManager::m_AuxDataBank[uc_data_group_idx][uc_chn+1].aux_data;
        // for energy We may also need to get the first voltage too, however, for battery, there's no large vibration in V,
        // and we can leave it now and deal it <<<LATER>>>-philip 8/6/2013- if necessory

        if(fCapacity > 0)    //??
        {
            Inc_Dec_Flag[chn] = 1;
            IncCCapacity(chn,fCapacity);     //?????????
            IncCEnergy(chn,fEnergy);
        }
        else    //??
        {
            Inc_Dec_Flag[chn] = 0;
            IncDCapacity(chn,-fCapacity);      //?????????
            IncDEnergy(chn,-fEnergy);
        }
    }
}

// return -1: not valid channel for cap process, return 1: valid.
// int16 MetaVariable_Task::CheckAndSetCapCandidate(char requestType, char chn)
// {
// 	// if the channel's off or rest, or
// 	// if the channel's not running, e.g. it's in transition stage
// 	// we don't do cap caculation.
// 	if(ChannelInfo_Manager::ChannelInfo[chn].m_CtrlState == CCS_REST	\
// 		|| ChannelInfo_Manager::ChannelInfo[chn].m_CtrlState == CCS_OFF	\
// 		|| ChannelInfo_Manager::ChannelInfo[chn].StepTransationToStatus != STEPRUNNING)
// 	{
// 		capacityStatus[chn] = 0;    //reset
// 		return -1;
// 	}

// 	unsigned char ActiveIdx=ChannelInfo_Manager::ChannelStep[chn].m_ucActiveIdx;
// 	switch(ChannelInfo_Manager::ChannelStep[chn].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType)
// 	{
// 	case CT_IVCH_CPULSE:	// more can be added here if required.
// 	case CT_IVCH_IR:
// 		return -1;

// 	default:
// 		break;
// 	}

// 	if(capacityStatus[chn] == 0)    //????????
// 	{
// 		capacityStatus[chn] = 1;
// 		OS_ServeProcess::recordCurTime(&m_capacity_Time[chn]);    //????
// 		return -1;
// 	}
// 	// No new data for regular request. Mandatory request is ignored here
// 	if((ChannelInfo_Manager::Isamp_NeedRecalculateSign->m_Bits.CalCapacity != 1)
// 		&&(ChannelInfo_Manager::Vsamp_NeedRecalculateSign->m_Bits.CalCapacity != 1)
// 		&& (requestType != 1) )
// 	{
// 		return -1;
// 	}

// 	return 1;
// }

void MetaVariable_Task::IncCCapacity(unsigned short uc_ChannelNo, double dSetValue)
{
    ChargeCapacity[uc_ChannelNo] += dSetValue;
}
void MetaVariable_Task::IncDCapacity(unsigned short uc_ChannelNo, double dSetValue)
{
    DisChargeCapacity[uc_ChannelNo] += dSetValue;
}
void MetaVariable_Task::IncCEnergy(unsigned short uc_ChannelNo, double dSetValue)
{
    ChargeEnergy[uc_ChannelNo] += dSetValue;
}
void MetaVariable_Task::IncDEnergy(unsigned short uc_ChannelNo, double dSetValue)
{
    DischargeEnergy[uc_ChannelNo] += dSetValue;
}


/* Additional // Important // Need Other People's Cooperation //

// 1. We need capacityStatus[chn] to be reset to 0 each time there's a Step ends.
// in case we can not reset it to 0 by updating due to task's running speed
// and then a false time will be used for next cap calculating
//
// and if possible, please also set capacityStatus[chn] to 1 when a step(other than setvariables, IR, Pulse)
// finishes transistion stage and start the running stage, also record the start time by
// OS_ServeProcess::recordCurTime(&m_capacity_Time[chn]);  then we don't lose any time need to be integration

// 2. We need each time a step ends, before ending,
// MetaVariable_Task::calculate_capacity(char requestType, char chn) needs to be
// called before ending the step with the parameter requestType value = 1. This
// will update capacity and engrgy, then a up to xx ms error can be avoided.*/


void MetaVariable_Task::Calculate_AuxDXDT(unsigned char uc_Group,unsigned char uc_ch)    // all channels will be processed here in perticular order.
{
    St_DXDT_Parameter  st_dxdt_Parameter;
    AuxDXDT_GetParameter(uc_Group,uc_ch,&st_dxdt_Parameter);
    float f_Deta = Aux_DataManager::m_AuxDataBank[uc_Group][uc_ch].aux_data - st_History[uc_Group][uc_ch].Value;
    float f_Time = CommonFunc::TimeCompare(st_History[uc_Group][uc_ch].Time,Aux_DataManager::m_AuxDataBank[uc_Group][uc_ch].value_update_time);
    if(((f_Time>st_dxdt_Parameter.Min_Time)&&(abs(f_Deta)>st_dxdt_Parameter.Precision))||(f_Time>st_dxdt_Parameter.Max_Time))
    {
        AuxDXDT_Result(uc_Group,uc_ch,f_Deta,f_Time);
    }
}

void MetaVariable_Task::AuxDXDT_GetParameter(unsigned char uc_Group,unsigned char uc_ch,St_DXDT_Parameter * st_dxdt_Parameter)    // all channels will be processed here in perticular order.
{
    switch(Aux_DataManager::m_AuxDataBank[uc_Group][uc_ch].m_Chn_Info.m_AuxType)
    {
        case AUX_TEMPERATURE:
            st_dxdt_Parameter->Max_Time = MAX_DXDT_T_TIME;
            st_dxdt_Parameter->Min_Time = MIN_DXDT_T_TIME;
            st_dxdt_Parameter->Precision = DXDT_T_PRECISION;
            break;
        
        case AUX_VOLTAGE:
            st_dxdt_Parameter->Max_Time = MAX_DXDT_V_TIME;
            st_dxdt_Parameter->Min_Time = MIN_DXDT_V_TIME;
            st_dxdt_Parameter->Precision = DXDT_V_PRECISION * 2 * \
                                               abs(Aux_DataManager::m_AuxDataBank[uc_Group][uc_ch].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH);
            break;
        
        case AUX_CURRENT:
            st_dxdt_Parameter->Max_Time =   MIN_DXDT_AuxCurr_TIME;         
            st_dxdt_Parameter->Min_Time =   MAX_DXDT_AuxCurr_TIME;  
            st_dxdt_Parameter->Precision =  DXDT_AuxCurr_PRECISION * 2 * \
                                                abs(Aux_DataManager::m_AuxDataBank[uc_Group][uc_ch].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH);      
            break;

        case AUX_PRESSURE:
            st_dxdt_Parameter->Max_Time =   MIN_DXDT_P_TIME;         
            st_dxdt_Parameter->Min_Time =   MAX_DXDT_P_TIME;  
            st_dxdt_Parameter->Precision =  DXDT_P_PRECISION * 2 * \
                                                abs(Aux_DataManager::m_AuxDataBank[uc_Group][uc_ch].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH);      
            break;
        case AUX_EXTERNALCHARGE:
            st_dxdt_Parameter->Max_Time =   MIN_DXDT_EXT_TIME;         
            st_dxdt_Parameter->Min_Time =   MAX_DXDT_EXT_TIME;  
            st_dxdt_Parameter->Precision =  DXDT_EXT_PRECISION * 2 * \
                                               abs(Aux_DataManager::m_AuxDataBank[uc_Group][uc_ch].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH);      
            break;
        case AUX_ACIM:
            st_dxdt_Parameter->Max_Time =   MIN_DXDT_ACIM_TIME;         
            st_dxdt_Parameter->Min_Time =   MAX_DXDT_ACIM_TIME;  
            st_dxdt_Parameter->Precision =  DXDT_ACIM_PRECISION * 2 * \
                                                abs(Aux_DataManager::m_AuxDataBank[uc_Group][uc_ch].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH);      
            break;

        default:            
            break;

    }
}

//void MetaVariable_Task::AuxDXDT_GetParameter(unsigned char uc_Group,unsigned char uc_ch,St_DXDT_Parameter * st_dxdt_Parameter)    // all channels will be processed here in perticular order.
//{
//    if(Aux_DataManager::m_AuxDataBank[uc_Group][uc_ch].m_Chn_Info.m_AuxType == AUX_TEMPERATURE)
//    {
//        st_dxdt_Parameter->Max_Time = MAX_DXDT_T_TIME;
//        st_dxdt_Parameter->Min_Time = MIN_DXDT_T_TIME;
//        st_dxdt_Parameter->Precision = DXDT_T_PRECISION;
//    }
//    else if(Aux_DataManager::m_AuxDataBank[uc_Group][uc_ch].m_Chn_Info.m_AuxType == AUX_VOLTAGE)
//    {
//        st_dxdt_Parameter->Max_Time = MAX_DXDT_V_TIME;
//        st_dxdt_Parameter->Min_Time = MIN_DXDT_V_TIME;
//        st_dxdt_Parameter->Precision = DXDT_V_PRECISION * 2 * \
//                                       abs(Aux_DataManager::m_AuxDataBank[uc_Group][uc_ch].m_Chn_Info.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH);
//    }        
//}

void MetaVariable_Task::AuxDXDT_Result(unsigned char uc_Group,unsigned char uc_ch,float f_Deta,float f_Time)
{
    unsigned char FloatType = FP_ZERO;
    float TempVal = 0;
    TempVal = f_Deta/f_Time;
    FloatType = fpclassify(TempVal);

    if((FloatType == FP_ZERO) || (FloatType == FP_NORMAL))
    {
        Aux_DataManager::m_AuxDataBank[uc_Group][uc_ch].aux_dx_dt = TempVal;    //把dvdt的值存放到大数据体内  Y/1=X/t
        Aux_DataManager::m_AuxDataBank[uc_Group][uc_ch].aux_d2x_dt2 = \
                ((TempVal - st_History[uc_Group][uc_ch].aux_dx_dt)/f_Time);
        st_History[uc_Group][uc_ch].aux_dx_dt = Aux_DataManager::m_AuxDataBank[uc_Group][uc_ch].aux_dx_dt;
    }
    st_History[uc_Group][uc_ch].Value = Aux_DataManager::m_AuxDataBank[uc_Group][uc_ch].aux_data;
    st_History[uc_Group][uc_ch].Time = Aux_DataManager::m_AuxDataBank[uc_Group][uc_ch].value_update_time;
}
