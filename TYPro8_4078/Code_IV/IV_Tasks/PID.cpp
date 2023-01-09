////////////////////////////////////////////////////////////////////////////////////////////
//File Name: PID.cpp
//Version:	 V1.0
//Date:	  04.23.2013
//Description:   PID Calculation
//
//Others:
//History：
//////////////////////////////////////////////////////////////////////////////////////////

#include "../Entry/includes.h"


// for debugging  // 用 DEBUG_TRACE_RECORD 代替

// float  Ctrl_Pid_Coef::f_Last_Current;
// float  Ctrl_Pid_Coef::f_Last_Voltage;
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"
#endif
float  Pid_Ctrl::fUpLimit[MAXCHANNELNO];
float  Pid_Ctrl::fDnLimit[MAXCHANNELNO];
// unsigned long Pid_Ctrl::m_Counter[MAXCHANNELNO];
//unsigned char Ctrl_Pid_Coef::m_SampleDelayCounter = 0;
//unsigned long long Pid_Ctrl::pidCtrlCount[MAXCHANNELNO] = {0};

float Pid_Ctrl::IBased_PID_Output_Calculation(unsigned char uc_Ch_No, float fKp,  float fKi, float fKd, Ctrl_Pid_Coef * pPidCoef)
{
    float DeltaOutput = 0;
    float Err_1B = pPidCoef->m_SetValue_1B - pPidCoef->m_SampleValue_1B;
    float  Output=0.0;
    float  Output_back=0.0;
	  
		
//=========CCCV_TB will use the old style PID controller=====================================
//============add by zc 2018.02.01===========================================================
    unsigned char ActiveIdx; //add by zc 02022018
    ActiveIdx=ChannelInfo_Manager::ChannelStep[uc_Ch_No].m_ucActiveIdx;//add by zc 02022018
    St_StepCtrl * p_Ctrl = &ChannelInfo_Manager::ChannelStep[uc_Ch_No].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl;//add by zc 02022018
    if	(	ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode() //single mode
            || (ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_Ch_No)&&(ChannelInfo_Manager::IsCurrentRangeParallelHigh(uc_Ch_No)))  //master, but not PH current range
            || (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_Ch_No)&&(ChannelInfo_Manager::IsCurrentRangeParallelHigh(uc_Ch_No))))
    {
        if(pPidCoef->m_LastOutput_1B>1.0)// Design for parallel problems  dirui 20180404
        {
            pPidCoef->m_LastOutput_1B = 1.0;
        }
        if(pPidCoef->m_LastOutput_1B<-1.0)
        {
            pPidCoef->m_LastOutput_1B = -1.0;
        }
    }


    if(p_Ctrl->m_u16ControlType != CT_IVCH_CCCV_TB)     // CCCV_TB does not need filter add by zc 02022018
    {
//         ChannelInfo_Manager::ChannelInfo[uc_Ch_No].m_bNeedFilter=true;
        ChannelInfo_Manager::RecoverFilter(uc_Ch_No);
        ChannelInfo_Manager::m_NeedRCFilterFlg[uc_Ch_No]=true;
        ChannelInfo_Manager::m_RC_CurrentFilterDeepth[uc_Ch_No]=200;
        ChannelInfo_Manager::m_RC_VoltageFilterDeepth[uc_Ch_No]=1;
    }

    
        
    DeltaOutput =   fKi * Err_1B;
    float DeltaOutputCurrent =  ChannelInfo_Manager::UnifyCurrent(uc_Ch_No, DeltaOutput);
    Output = pPidCoef->m_LastOutput_1B + DeltaOutputCurrent;
    

    if(Output > fUpLimit[uc_Ch_No]) 	// 将电流设置值作为PID钳位, 09.09.2015
        Output = fUpLimit[uc_Ch_No];
    if(Output < fDnLimit[uc_Ch_No])
        Output = fDnLimit[uc_Ch_No];

    if	(ChannelInfo_Manager::Serial2ParaMod_IsSingleChanMode() //single mode
            || (ChannelInfo_Manager::Serial2ParaMod_IsIntraUnitMasterChannel(uc_Ch_No)&&(ChannelInfo_Manager::IsCurrentRangeParallelHigh(uc_Ch_No)))  //master, but not PH current range
            || (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitMasterChannel(uc_Ch_No)&&(ChannelInfo_Manager::IsCurrentRangeParallelHigh(uc_Ch_No))))
    {

        if(Output >= 1.0)
            Output = 1.0;
        else if(Output <= -1.0)
            Output = -1.0;
    }
    pPidCoef->m_LastOutput_1B = Output;

    pPidCoef->m_fLastErr_1B = Err_1B;

    return Output;
}

void Ctrl_Pid_Coef::Init(void)
{

//     for(unsigned char uc_i=0; uc_i<MAXCHANNELNO; uc_i++)
//         Pid_Ctrl::m_Counter[uc_i]=0;
    m_PidTypeFlag = 0;

// 	f_Last_Current = 0.0f;
// 	f_Last_Voltage = 0.0f;

    m_LastOutput_1B=0;
// 	m_CurrentIRange=ChannelInfo_Manager::CurrRngPhysicalRange1;
    m_SetValue_1B=0;

    m_fLastErr_1B = 0;

//	m_bLoadIsResistor = false;
}

