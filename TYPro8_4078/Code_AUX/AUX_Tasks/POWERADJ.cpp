//#include "includes.h"
#include "../Entry/includes.h"
#include "POWERADJ.h"

/**********************************************************
** Start:
******************************************************************************/
#pragma arm section rwdata = "SRAMSPECIAL",zidata = "SRAMSPECIAL"      // 设置变量存储在外部扩展存储器中     DRAM   0XA0000000  0x00000040
#pragma arm section //结束外存变量定义  
UInt16           POWERADJ::Priority = PRIORITY4;  //PRIORITYBG
UInt16           POWERADJ::TaskID = POWERADJ_TASK;    //DISPOSABLE_TASK;    comment out chen 20130817

float POWERADJ::Vpp_Set;
float POWERADJ::Vmm_Set;
float POWERADJ::Old_Vpp_Set;
float POWERADJ::Old_Vmm_Set;
// float POWERADJ::Vpp_Adc;
// float POWERADJ::Vmm_Adc;

unsigned char POWERADJ::BoardType;
unsigned int POWERADJ::uc_SourceID;
unsigned char POWERADJ::m_DataSaveID[128];
unsigned char POWERADJ::m_UnitCnt;

unsigned char POWERADJ::AdjStatus;   // 调节控制状态
// unsigned char POWERADJ::Vol_TraceFlag=0;
// unsigned char POWERADJ::NeebACKflag=ADJ_NO_NEED_ACK;    // 0:不需要回复   1： 回复Start_ACK      2:Stop_ACK
unsigned char POWERADJ::RefreshFlag=0;
// unsigned char POWERADJ::PowerAdjErrorFlag ;
// unsigned char POWERADJ::RestarACK_ok;
// unsigned short  POWERADJ::TimeTicks;
// unsigned char POWERADJ::CAN_StartFlag;
float POWERADJ::VppSET_VOL_MAX;
float POWERADJ::VppSET_VOL_MIN;
float POWERADJ::VmmSET_VOL_MAX;
float POWERADJ::VmmSET_VOL_MIN;
// bool POWERADJ::CAN_LostFlag;
// unsigned char POWERADJ::HV_OK;
ST_M4_Value  POWERADJ::m_M4_Value;
// MsTime POWERADJ::LastdelayTime;
MsTime POWERADJ::SendDataTime;
bool   POWERADJ::SendFeedBackFlag;
unsigned char  POWERADJ::AdjustCnt=0;
unsigned char  POWERADJ::TimeCnt=0;
float POWERADJ::Vpp_adjust;
float POWERADJ::Vmm_adjust;


ST_PowerAdj_Para POWERADJ::m_PowerAdjData[40];
My_Func_Ptr_void1 POWERADJ::PowerRunningState;
My_Func_Ptr_Ext6 POWERADJ::Func_Voltage_Dac_Out;
My_Func_Ptr_Void  POWERADJ::IndependentSample;//不在中断里进行的采样任务
const St_Port   POWERADJ:: CAN_LED = {1,      19,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port   POWERADJ:: ALM_LED = {1,       21,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
void POWERADJ::Init(void)
{
    m_UnitCnt = 0;
// 	Vol_TraceFlag=0;

    CANFunc::SourceBoardID = CANFunc::SourceBoardID+128;
    memset(POWERADJ::m_DataSaveID,0xFF,128);

    AdjStatus = ADJ_NO_NEED;
    ControlPin::SetIObyCP(CAN_LED);
    ControlPin::SetIObyCP(ALM_LED);
    CommonFunc::GetTimeTicksUpdate(&SendDataTime);
    SendFeedBackFlag=false;
}
void POWERADJ::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_POWERADJ]++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_POWERADJ);
#endif
	
    PowerAdjProcess();
    ReportData();
	 if(IndependentSample!=0)
		 IndependentSample();
	 
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_POWERADJ] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time(COUNT_POWERADJ);
#endif
}
void POWERADJ::PowerAdjProcess(void)
{
    switch(AdjStatus)
    {
    case PADJ_IDLE:
        ControlPin::SetMeLow(ALM_LED);
        ControlPin::SetMeLow(CAN_LED);
        AdjStatus = PADJ_START;
		  Old_Vpp_Set=VppSET_VOL_MAX;
		  Old_Vmm_Set=VmmSET_VOL_MAX;
        break;
    case PADJ_START:
        if(PowerRunningState()==POWER_RUN)
            AdjStatus = PADJ_RUN;
        break;
    case PADJ_RUN:
        if(PowerRunningState()==POWER_ERROR)
        {
            AdjStatus = PADJ_ERROR;
            break;
        }
			if(RefreshFlag==1)
			{   
				RefreshFlag =0;
				AdjStatus = PADJ_CAL_VALUE;
				AdjustCnt=0;
			}				
// 			if(CAN_LostFlag == true)//检查CAN连接情况
// 			{
// 				AdjStatus = PADJ_CAN_LOST;
//             }			
			break;
		case PADJ_CAL_VALUE:
			AdjustCnt=5;
			if(CaculateAdjustValue()==1)
			{
				AdjStatus = PADJ_OUTPUT;
				AdjustCnt=1;
			}
			else
				AdjStatus = PADJ_RUN;
		case PADJ_OUTPUT:
			TimeCnt++;
		    if(TimeCnt>=50)
			{
				 TimeCnt=0;
			     Output();
				AdjustCnt++;
				if(AdjustCnt>=5)
				   AdjStatus = PADJ_RUN;
			}
			if(PowerRunningState()==POWER_ERROR)
			{
				AdjStatus = PADJ_ERROR;
				break;
			}
		    break;
// 		case PADJ_ACK:
// 			SendDataToIV();
// 		  AdjStatus = PADJ_RUN;//回复完了就回到run状态
// 			break;
    case PADJ_ERROR:
        if(Func_Voltage_Dac_Out!=0)
        {
            Func_Voltage_Dac_Out(ADJ_Vpp_CHAN, VppSET_VOL_MIN);
            Func_Voltage_Dac_Out(ADJ_Vmm_CHAN,VmmSET_VOL_MAX);
        }
        ControlPin::SetMeHigh(ALM_LED);
        break;
// 		case PADJ_CAN_LOST:
// 			if(Func_Voltage_Dac_Out!=0)
// 			{
// 				Func_Voltage_Dac_Out(0,VppSET_VOL_MIN);
// 				Func_Voltage_Dac_Out(1,VmmSET_VOL_MAX);
// 			}
// 			ControlPin::SetMeHigh(CAN_LED);
// 			break;
    }
}

unsigned char POWERADJ::CaculateAdjustValue(void)
{
    float Vpp_cal = 0;
    float Vmm_cal = 0;
    bool UpdateFlag = false;
    for(unsigned char unit_index=0; unit_index<m_UnitCnt; unit_index++)
    {
// 		if(m_PowerAdjData[unit_index].m_Updataflag == true)
// 		{
        if(Vpp_cal<m_PowerAdjData[unit_index].m_VppValue)
            Vpp_cal = m_PowerAdjData[unit_index].m_VppValue;
        if(Vmm_cal<m_PowerAdjData[unit_index].m_VmmValue)
            Vmm_cal = m_PowerAdjData[unit_index].m_VmmValue;
// 			m_PowerAdjData[unit_index].m_Updataflag = false;
// 			m_PowerAdjData[unit_index].m_PacketLostCnt = 0;
// 			UpdateFlag = true;
// 		}
    }
// 	if(UpdateFlag == true)
// 	{
    Vpp_Set = Vpp_cal;
    Vmm_Set = Vmm_cal;
	if(Vpp_Set != Old_Vpp_Set)//刷新了值就等会再回复
	{
		Vpp_adjust = (Vpp_Set-Old_Vpp_Set)/AdjustCnt;
			Old_Vpp_Set +=Vpp_adjust;
		if(Func_Voltage_Dac_Out!=0)
		{			
			Func_Voltage_Dac_Out(ADJ_Vpp_CHAN, Old_Vpp_Set);
			CommonFunc::GetTimeTicksUpdate(&SendDataTime);
		}									
	}
	if(Vmm_Set != Old_Vmm_Set)//刷新了值就等会再回复
	{
		Vmm_adjust = (Vmm_Set-Old_Vmm_Set)/AdjustCnt;
		Old_Vmm_Set += Vmm_adjust;
		if(Func_Voltage_Dac_Out!=0)
		{
				Func_Voltage_Dac_Out(ADJ_Vmm_CHAN, Old_Vmm_Set);
				CommonFunc::GetTimeTicksUpdate(&SendDataTime);
		}
// 		Old_Vmm_Set = Vmm_Set;					
	}	
//     }
	if(m_UnitCnt == 0)//一个都没有输出最大电压
	{
		Vpp_Set = VppSET_VOL_MAX;
		Vmm_Set = VmmSET_VOL_MIN;		
	}	
  
}
void POWERADJ::Output(void)
{
    if(Vpp_Set != Old_Vpp_Set)//刷新了值就等会再回复
	{		
		Old_Vpp_Set +=Vpp_adjust;
		if(Func_Voltage_Dac_Out!=0)
		{			
			Func_Voltage_Dac_Out(ADJ_Vpp_CHAN, Old_Vpp_Set);
			CommonFunc::GetTimeTicksUpdate(&SendDataTime);
		}									
	}
	if(Vmm_Set != Old_Vmm_Set)//刷新了值就等会再回复
	{		
		Old_Vmm_Set += Vmm_adjust;
		if(Func_Voltage_Dac_Out!=0)
		{
				Func_Voltage_Dac_Out(ADJ_Vmm_CHAN, Old_Vmm_Set);
				CommonFunc::GetTimeTicksUpdate(&SendDataTime);
		}				
	}	

}
void POWERADJ::SendDataToIV(void)
{
    POWERADJ::m_M4_Value.m_Vpp_Adc = Aux_DataManager::m_AuxDataBank[0][0].aux_data;
    if(Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_BoardType==Aux_PowerADJ_200A_412206)
        POWERADJ::m_M4_Value.m_Vmm_Adc = 4.0f;
    else
        POWERADJ::m_M4_Value.m_Vmm_Adc = Aux_DataManager::m_AuxDataBank[0][1].aux_data;

    CANBusParse::Send_AUXToIV_COMMON2Data(CAM_CMD_M0_REPORT,POWERADJ::m_M4_Value);//
}
void POWERADJ::ReportData(void)
{
    if(!SendFeedBackFlag)
    {
        if(CommonFunc::GetTimeTicks(&SendDataTime)>50000)   // 50000*100=5秒钟
        {
            SendDataToIV();
            CommonFunc::GetTimeTicksUpdate(&SendDataTime);
        }
        else
            return;
    }
    else
    {
        if(CommonFunc::GetTimeTicks(&SendDataTime)>2000)  // 200ms
        {
            SendDataToIV();
			SendFeedBackFlag=false;
            CommonFunc::GetTimeTicksUpdate(&SendDataTime);
        }
        else
            return;
    }
}


/** End:
******************************************************************************/


