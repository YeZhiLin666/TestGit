/*****************************************************************************
// 程序文件      :PowerAdj.cpp     用于电压跟随处理
// 文件描述      :为MCU board服务
// 编写日期      :2018.02.11
*****************************************************************************/


#include "../Entry/includes.h"

UInt16 PowerAdj::TaskID = POWERADJ_TASK;
UInt16 PowerAdj::Priority = PRIORITY4;
bool  PowerAdj::Calib_PowerParaFlag[MAXCHANNELNO];
// bool  PowerAdj::PowerAdjWorkFlag;
// bool   PowerAdj::b_M0ADCValueUnsafeFlag;
St_PowerData   PowerAdj::m_PowerValue;
St_PowerData   PowerAdj::m_M0Feedback_Value[4];
unsigned char  PowerAdj::m_PowerAdjStatus;
// UInt16        PowerAdj::Time20msCnt;
// uint32        PowerAdj::Time50usCnt;
// unsigned char  PowerAdj::uc_SendComCnt;
// bool           PowerAdj::TimeStartFlag;
// bool           PowerAdj::Time20msStartFlag;
// unsigned char  PowerAdj::Receive_ACK_Status;
// unsigned char  PowerAdj::PackLostCnt[4];
// bool            PowerAdj::Receive_M0_StartedACKFlag;
// bool           PowerAdj::Receive_M0_IDACKFlag[4];
St_LoadTypePowerAdj PowerAdj::SchLoadPara[MAXCHANNELNO]; //Schedule 电阻负载参数
St_PowerLevelData   PowerAdj::m_PowerLevelValue;
unsigned char PowerAdj::uc_M0ADData_UpdataFlag;      //收到数据置1 ， 使用后清零
St_FeedBackCheckData   PowerAdj::m_FeedBackData;
unsigned char PowerAdj::uc_DefaultVoltageFlag;
unsigned char PowerAdj::uc_DoCheckFlag;
unsigned long  PowerAdj::WaitCnt;

void PowerAdj::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_POWERADJ] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time_init(COUNT_POWERADJ);
#endif
	
    DoVoltageProcess();
    CheckFeedBack();
	
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_POWERADJ] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT + 1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time(COUNT_POWERADJ);
#endif	
}
void PowerAdj::DoVoltageProcess(void)
{
	unsigned char uc_waitFlag=0;
    switch(m_PowerAdjStatus)
    {
    case POWERADJ_IDLE:      							//空闲
        if(ChannelInfo_Manager::BoardCFG.m_Global.m_PowerRegulation)
        {
			WaitCnt = 0;
			m_PowerAdjStatus = POWERADJ_WAIT;
		}
        break;
	case POWERADJ_WAIT:
		WaitCnt++;
	     if(board_config::Feature_Enable_init.OS_Tick_Interval ==200)
		 {
			  if(WaitCnt>=15000)
			{
				 WaitCnt = 0;
				 uc_waitFlag=1;
			 }

		 }	 
		 else if(board_config::Feature_Enable_init.OS_Tick_Interval ==400)
		{
			 if(WaitCnt>=7500)
			{
				 WaitCnt = 0;
				 uc_waitFlag=1;
			 }
		 }
		 else
		 {
			 if(WaitCnt>=3750)
			{
				 WaitCnt = 0;
				 uc_waitFlag=1;
			 }
		 }
		 if(uc_waitFlag==1)
		 {
			 CalPowerSetValue();
             m_PowerAdjStatus = POWERADJ_REQUIRE;
         }
	     break;
    case POWERADJ_REQUIRE:
//         CalPowerSetValue();
        CANBusParse::Send_M0_COMMON2Data(CAN_CMD_M0_REQUIRE, m_PowerValue);
        m_PowerAdjStatus = POWERADJ_WAIT_REQUIREACK;
        break;
    case POWERADJ_WAIT_REQUIREACK:
        if(CANBusParse::m_Para_PowerAdjAckMgr.CmdStatus==eParaAckOK)
            m_PowerAdjStatus = POWERADJ_RUNNING;
        break;
    case POWERADJ_RUNNING:
        CalPowerUpdataValue();
        break;
    case POWERADJ_COMMUNATION_ERROR:
        CANBusParse::m_Para_PowerAdjAckMgr.m_ReSendCnt=0;
//         if(ChannelInfo_Manager::BoardCFG.chn_Running_Flag2 == CHN_LOCK_SCH)
//         {
// 				St_ScheduleMessage SchMessage;
// 				memset((void *)&SchMessage,0,sizeof(St_ScheduleMessage));
            for (unsigned char chn=0; chn<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; chn++ )
            {
                safety_check::SetUnsafe(chn, POWER_COMMUNICATION_FAIL,0, IV_UNSAFE_TYPE);//Unsafe
                safety_check::sendError(chn);		// trigger a sending
                // 		        safety_check::ErrorFlag[chn] ^= NOTAVAIL_UNSAFE;
            }
			 m_PowerAdjStatus = POWERADJ_FIXUNSAFE;
//         }
//         m_PowerAdjStatus = POWERADJ_FIXUNSAFE;
        break;
    case POWERADJ_Feedback_ERROR:
        CANBusParse::m_Para_PowerAdjAckMgr.m_ReSendCnt=0;
//         if(ChannelInfo_Manager::BoardCFG.chn_Running_Flag2 == CHN_LOCK_SCH)
//         {
// 				St_ScheduleMessage SchMessage1;
// 				memset((void *)&SchMessage1,0,sizeof(St_ScheduleMessage));
            for (unsigned char chn=0; chn<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; chn++ )
            {
                safety_check::SetUnsafe(chn, POWER_FEEDBACK_ERROR,0, IV_UNSAFE_TYPE);//Unsafe
                safety_check::sendError(chn);		// trigger a sending
                // 		        safety_check::ErrorFlag[chn] ^= NOTAVAIL_UNSAFE;
            }
			m_PowerAdjStatus = POWERADJ_FIXUNSAFE;
//         }
//         m_PowerAdjStatus = POWERADJ_FIXUNSAFE;
        break;
    case POWERADJ_FIXUNSAFE:
			if((!m_FeedBackData.m_unsafeFlag)&& (uc_DoCheckFlag==1))
// 		    if(uc_DoCheckFlag==1)
			{
				uc_DoCheckFlag=0;
				m_PowerAdjStatus =POWERADJ_REQUIRE;
			}
			break;
    case POWERADJ_GOTOThirdParty:
        PowerSetDefaultValue();
        m_PowerAdjStatus =POWERADJ_WaitForDefault;
        break;
    case POWERADJ_WaitForDefault:
        if(CANBusParse::m_Para_PowerAdjAckMgr.CmdStatus==eParaAckOK)
        {
				bool flag=true;
				for(unsigned char i=0;i<ChannelInfo_Manager::BoardCFG.m_Global.m_PowerCrlBoardUnits;i++)
				{
					if(abs(m_M0Feedback_Value[i].m_Vpp-m_PowerValue.m_Vpp)>0.1f*m_PowerValue.m_Vpp)    //0.03f
				  {
					  flag=false;
						break;
				  }					
// 				  if(m_PowerValue.m_Vmm - m_M0Feedback_Value[i].m_Vmm<0.03*m_PowerValue.m_Vmm )
// 				  {
// 					    flag=false;
// 						  break;
// 				  }
            }
            if(flag)
            {
                m_PowerAdjStatus =POWERADJ_OnThirdParty;
            }
        }
        break;
    case POWERADJ_OnThirdParty:

        break;
    default:

        break;
    }
}
void        PowerAdj::MyInit(void)
{
    for(unsigned char i=0; i<16; i++)
    {
        SchLoadPara[i].IsResistorFlag =false;
    }
    m_PowerAdjStatus=POWERADJ_IDLE;
    m_PowerValue.m_Vpp = ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVpp;
    m_PowerValue.m_Vmm = abs(ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVmm);
    uc_DefaultVoltageFlag=false;
	m_FeedBackData.m_unsafeFlag =false;
// 	for(unsigned char i=0;i<4;i++)
	   uc_M0ADData_UpdataFlag=0;
	uc_DoCheckFlag=0;
//     Time20msCnt =0;
// 	Time50usCnt =0;
// 	b_M0ADCValueUnsafeFlag = false;
    //下面需要增加 电源功率调节命令
// 	PowerAdjWorkFlag =false;
// 	TimeStartFlag =false;
// 		PackLostCnt[j]=0;

// 	for(unsigned char i=0;i<4;i++)
// 	{
// 	Receive_M0_IDACKFlag[i]=false;
// 	}
}

void PowerAdj::CalPowerSetValue(void)
{
    float Vpp,Vmm,Vmax;
    float Vmin=ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVpp;
		Vmax= -Vmin;
    float Vrange =ChannelInfo_Manager::BoardCFG.m_Channel[0].m_VRange[2].m_Bound.m_High;
		float DeltaV = Vrange/8;
    unsigned char m;
		signed n;
    for(unsigned char uc_channel=0; uc_channel<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; uc_channel++)
    {
        if(SchLoadPara[uc_channel].IsResistorFlag)
        {
            float Irange = ChannelInfo_Manager::GetMaxCurrRangeValue(uc_channel,0);
            float resistor =SchLoadPara[uc_channel].m_ResistorValue;
			Vpp =Irange*resistor;
			if(Vpp>ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVpp)
			   Vpp =ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVpp;
		}
		else
		   Vpp = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_channel,MetaCode_PV_Voltage);
		if(Vmax<Vpp)
			Vmax =Vpp;
		if(Vmin>Vpp)
            Vmin=Vpp;
    }
    if(Vmax>0)
        m = ceil(Vmax/DeltaV);
    else
        m=1;
    Vmax = m*DeltaV;
    m_PowerLevelValue.m_Vm = Vmax;
    Vpp = Vmax + ChannelInfo_Manager::BoardCFG.m_Global.m_VoltageAdj;
    if(Vpp>ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVpp)
        Vpp =ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVpp;
    if(Vpp < 0.1*ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVpp)  //0.1f
        Vpp = 0.1*ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVpp;//0.1f;
    m_PowerValue.m_Vpp = Vpp;

     n=floor(Vmin/DeltaV);
    Vmin = n*DeltaV;
    m_PowerLevelValue.m_Vn =Vmin;
    Vmm= Vmin - ChannelInfo_Manager::BoardCFG.m_Global.m_VoltageAdj;
    if(Vmm > 0.1*ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVmm) //-0.3f
        Vmm = 0.1*ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVmm;//-0.3f;
    if(Vmm<ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVmm)
        Vmm =ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVmm;
    m_PowerValue.m_Vmm = abs(Vmm);
}
void PowerAdj::CalPowerUpdataValue(void)
{
    unsigned char updata=0;
    float Vpp,Vmm;
    float Vmax =0;
    float Vmin=ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVpp;
    float Vrange =ChannelInfo_Manager::BoardCFG.m_Channel[0].m_VRange[2].m_Bound.m_High;
	float DeltaV = Vrange/8;

    for(unsigned char uc_channel=0; uc_channel<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; uc_channel++)
    {
		if(SchLoadPara[uc_channel].IsResistorFlag)
// 		if(ChannelInfo_Manager::ChannelSchedule[uc_channel].m_Settings.m_PowerAdjPara.IsCheckPowerAdj)
		{
			float Irange = ChannelInfo_Manager::GetMaxCurrRangeValue(uc_channel,0);
			float resistor =SchLoadPara[uc_channel]. m_ResistorValue;//ChannelInfo_Manager::ChannelSchedule[uc_channel].m_Settings.m_PowerAdjPara.m_ResistorValue;;//SchLoadPara[uc_channel].m_ResistorValue;
			Vpp =Irange*resistor;
			if(Vpp>ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVpp)
			   Vpp =ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVpp;
// 			CalPowerSetValue();
        }
        else
            Vpp = ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_channel,MetaCode_PV_Voltage);
        if(Vmax<Vpp)
            Vmax =Vpp;
        if(Vmin>Vpp)
            Vmin=Vpp;
    }
    float deltaVm1 =Vmax-m_PowerLevelValue.m_Vm;
    if(deltaVm1>0)
    {
        if(deltaVm1> (Vrange/32)) //升高到m+1挡位
        {
            updata =1;
            m_PowerLevelValue.m_Vm += DeltaV;
            Vmax=m_PowerLevelValue.m_Vm;
        }
    }
    else
    {
        float deltaVm2 = Vmax-m_PowerLevelValue.m_Vm+DeltaV;
        if(deltaVm2<-Vrange/32)
        {
            updata =1;
            m_PowerLevelValue.m_Vm-= DeltaV;
            Vmax=m_PowerLevelValue.m_Vm;
        }
    }
    float deltaVn1 =Vmin-m_PowerLevelValue.m_Vn;
    if(deltaVn1>0)
    {
        if(deltaVn1> (Vrange/32)) //升高到n+1挡位
        {
            updata =1;
            m_PowerLevelValue.m_Vn += DeltaV;
            Vmin=m_PowerLevelValue.m_Vn;
        }
    }
    else
    {
        float deltaVn2 = Vmin-m_PowerLevelValue.m_Vn+DeltaV;
        if(deltaVn2<-Vrange/32)
        {
            updata =1;
            m_PowerLevelValue.m_Vn -= DeltaV;
            Vmin=m_PowerLevelValue.m_Vn;
        }
    }
    if(updata==1)
    {
        Vpp = m_PowerLevelValue.m_Vm + ChannelInfo_Manager::BoardCFG.m_Global.m_VoltageAdj;
        if(Vpp>ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVpp)
            Vpp =ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVpp;
        if(Vpp < 0.1*ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVpp)  //0.1f
            Vpp = 0.1*ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVpp;//0.1f;
        m_PowerValue.m_Vpp = Vpp;

        Vmm= m_PowerLevelValue.m_Vn - ChannelInfo_Manager::BoardCFG.m_Global.m_VoltageAdj;
        if(Vmm > 0.1*ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVmm) //-0.3f
            Vmm = 0.1*ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVmm;//-0.3f;
        if(Vmm<ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVmm)
            Vmm =ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVmm;
        m_PowerValue.m_Vmm = abs(Vmm);
				m_PowerAdjStatus = POWERADJ_REQUIRE;
//         CANBusParse::Send_M0_COMMON2Data(CAN_CMD_M0_REQUIRE, m_PowerValue);
    }

}
void PowerAdj::PowerSetDefaultValue(void)
{
    m_PowerValue.m_Vpp=ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVpp;
    m_PowerValue.m_Vmm = abs(ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVmm);
    CANBusParse::Send_M0_COMMON2Data(CAN_CMD_M0_REQUIRE, m_PowerValue);
}
void PowerAdj::CheckFeedBack(void)
{
	float delta_Vpp,delta_Vmm;
	unsigned char uc_unsafeFlag=0;
// 	if((m_PowerAdjStatus!=POWERADJ_RUNNING)||(m_PowerAdjStatus!=POWERADJ_FIXUNSAFE))
	if(m_PowerAdjStatus<POWERADJ_RUNNING)
	{
		if(uc_M0ADData_UpdataFlag==1)
		{
			uc_DoCheckFlag=1;
			uc_M0ADData_UpdataFlag=0;
		}
		return;
	}
	
	if(!m_FeedBackData.m_unsafeFlag)  //反馈值在安全范围内
    {
        if(uc_M0ADData_UpdataFlag==1)
		{
			uc_DoCheckFlag=1;
			for(unsigned char i=0;i<ChannelInfo_Manager::BoardCFG.m_Global.m_PowerCrlBoardUnits;i++)
			{
				delta_Vpp = abs(m_M0Feedback_Value[i].m_Vpp-m_PowerValue.m_Vpp) ;
				if(delta_Vpp>0.1f*ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVpp)    //0.05f
				{
					uc_unsafeFlag=1;
					break;
				}	
				delta_Vmm = m_PowerValue.m_Vmm - m_M0Feedback_Value[i].m_Vmm;	 
				if(delta_Vmm>abs(0.1 *ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVmm))   //0.05f
 				{
					uc_unsafeFlag=1;
					break;
				}
			}						
// 					delta_Vmm = m_PowerValue.m_Vmm - m_M0Feedback_Value.m_Vmm;	 
// 					if(delta_Vmm>abs(0.03 *ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVmm))
// 					{
// 						 uc_unsafeFlag=1;
// 					}
			if(uc_unsafeFlag==1)
			{
				m_FeedBackData.m_unsafeFlag =true;
				CommonFunc::GetTimeTicksUpdate(&m_FeedBackData.m_UnsafeTime);
			}
			uc_M0ADData_UpdataFlag=0;
            CommonFunc::GetTimeTicksUpdate(&m_FeedBackData.m_UpdataTime);
        }
        else
        {
			if(m_PowerAdjStatus!=POWERADJ_FIXUNSAFE)
			{	
				if(CommonFunc::GetTimeTicks(&m_FeedBackData.m_UpdataTime)>AD_OVERTIME)    //超过20秒没有收到刷新数据，认为Can  LOST
					m_PowerAdjStatus =POWERADJ_COMMUNATION_ERROR;
			}
			return;
		}
	 }
	 else
	 {
			if(CommonFunc::GetTimeTicks(&m_FeedBackData.m_UnsafeTime)>Unsafe_Overtime)   //超过16秒一直处于反馈不安全状态，则报警停机
			{
				  if(m_PowerAdjStatus!=POWERADJ_FIXUNSAFE)
					{
						m_PowerAdjStatus =POWERADJ_Feedback_ERROR;
					}
// 					else
// 						CommonFunc::GetTimeTicksUpdate(&m_FeedBackData.m_UnsafeTime);
			}
// 			else
// 			{
					if(uc_M0ADData_UpdataFlag==1)
					{
							uc_DoCheckFlag=1;
						 for(unsigned char i=0;i<ChannelInfo_Manager::BoardCFG.m_Global.m_PowerCrlBoardUnits;i++)
					   {
									delta_Vpp = abs(m_PowerValue.m_Vpp - m_M0Feedback_Value[i].m_Vpp);
									if(delta_Vpp>0.1*ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVpp)   //0.05f
									{
										uc_unsafeFlag=1;
										break;
									}		
									delta_Vmm = m_PowerValue.m_Vmm - m_M0Feedback_Value[i].m_Vmm;	 
									if(delta_Vmm>abs(0.1 *ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVmm))  //0.05f
									{
										uc_unsafeFlag=1;
									}
							}									
// 							delta_Vmm = m_PowerValue.m_Vmm - m_M0Feedback_Value.m_Vmm;	 
// 							if(delta_Vmm>abs(0.03 *ChannelInfo_Manager::BoardCFG.m_Channel[0].m_fVmm))
// 							{
// 								 uc_unsafeFlag=1;
// 							}
							if(uc_unsafeFlag==0)
							{
								m_FeedBackData.m_unsafeFlag =false;
			 					CommonFunc::GetTimeTicksUpdate(&m_FeedBackData.m_UnsafeTime);
							}
							uc_M0ADData_UpdataFlag=0;
							CommonFunc::GetTimeTicksUpdate(&m_FeedBackData.m_UpdataTime);
					}
					else
					{
							if(CommonFunc::GetTimeTicks(&m_FeedBackData.m_UpdataTime)>AD_OVERTIME)    //超过20秒没有收到刷新数据，认为Can  LOST
								m_PowerAdjStatus =POWERADJ_COMMUNATION_ERROR;
					}
// 			}
   }    
}
