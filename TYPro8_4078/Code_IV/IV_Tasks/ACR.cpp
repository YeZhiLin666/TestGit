///////////////////////////////////////////////////////////////////////////////////
//File Name: ACR.cpp
//Version:		V1.0
//Date:				2012/11/9
//Description:
//						ACR����
//Others:
//History��

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x03E80000
#endif
unsigned char 	ACR::ACR_Status[MAXCHANNELNO];
unsigned char 	ACR::ACR_Calibrate[MAXCHANNELNO];
unsigned char 	ACR::ACR_NegativeFlag;
unsigned long 	ACR::ACR_IHighDacCode;
unsigned long 	ACR::ACR_ILowDacCode;
unsigned long   ACR::ACR_CycleNum[MAXCHANNELNO];
unsigned long 	ACR::ACR_CycleCount[MAXCHANNELNO];
unsigned long 	ACR::ACR_Positive_Dac[MAXCHANNELNO][2];  // ����DAC
unsigned long 	ACR::ACR_Negative_Dac[MAXCHANNELNO][2];  // ����DAC
float           ACR::ACR_I_Offset[MAXCHANNELNO];  // ����ƫ��
float           ACR::ACR_R_V[ACR_R_COUNT];        // ����ֵ
UWord32         ACR::ACR_T1[MAXCHANNELNO];
unsigned short  ACR::ACR_T1_Count[MAXCHANNELNO];
MsTime 	    	ACR::ACR_StartTime[MAXCHANNELNO];
MsTime 	    	ACR::ACR_EndTime[MAXCHANNELNO];
//MsTime  	    ACR::ACR_TimeInterval[MAXCHANNELNO];
ST_PULSELOCK    ACR::st_Pulse_Lock[MAXCHANNELNO];    // ʵ�����
PULSELOCK_CH    ACR::Lock_Ch_Copy;
#pragma arm section


unsigned char   ACR::uc_PLock_Count = 0;
PULSELOCK_CH    ACR::Pulse_Lock_Ch;     // �������
ST_SINGLEGLOCK  ACR::st_Single_Lock;    // ����ִ����
unsigned char   ACR::uc_Pulse_Clear = 0;   // ���廻������

float ACR::SaveACRVoltage[MAXCHANNELNO];//��ʱ���ڴ���ACR��ѹ���⣬������������������ɾ��

unsigned char 	ACR::ACRTimerStatus[MAXCHANNELNO];
unsigned char 	ACR::ACRChannelNo=0;
unsigned short 	ACR::ACRChannelFlg=0;  //ͨ����ACR���
unsigned char 	ACR::ACR_TimerEn=FALSE;

char 	(* ACR::pACR_ADC)(char uc_ChannelNo);
char 	(*ACR::pRenew_ACR_ADC)(void);
void 	(*ACR::pACR_Read_ADC)(char uc_ChannelNo,unsigned long *SampleV,unsigned long *SampleI);

unsigned char 	ACR::DAC_Flag;
UWord32 		ACR::WaitTime;
unsigned char 	ACR::WaitRecord;



// ******************************************************/
// Function Name: Init
// Version:
// Date:
// Description:
//                         ACR��ʼ��
// Others:
//
// History��
//
// ******************************************************/
void ACR::Init(void)
{
    IR::IR_CopyFlg = 0;
    IR::IRChannelFlg_CopyInit[0] = 0;       //ͨ����IR���   // [X] 0:ԭֵ 1������ֵ
    IR::IRChannelFlg_CopyInit[1] = 0XFFFF;  //ͨ����IR���   // [X] 0:ԭֵ 1������ֵ

    //memset((void *)&st_Single_Lock,0,sizeof(st_Single_Lock));
    st_Single_Lock.st_Lock.uc_UseFlag = 0;
    st_Single_Lock.st_Lock.uc_ControleType = CT_IVCH_STOP;
    st_Single_Lock.uc_Ch = 0XFF;

    INIT_QUEUE(Pulse_Lock_Ch);
    INIT_QUEUE(Lock_Ch_Copy);

    uc_PLock_Count = 0;
    ACRChannelNo=0;
    ACRChannelFlg=0;  //ͨ����ACR���
    ACR_TimerEn=FALSE;
    ACR_NegativeFlag = FALSE;
    for(unsigned char uc_i=0; uc_i<ACR_R_COUNT; uc_i++)
        ACR_R_V[uc_i] = 0;

    for(unsigned char uc_i=0; uc_i<MAXCHANNELNO; uc_i++)
    {
        ACR_Calibrate[uc_i]=FALSE;
        //ACR_I_Offset[uc_i] = 0.0001;   //  100uA
        ACR_I_Offset[uc_i] = 0;   //  0
        ACR_EndTime[uc_i] = OS_ServeProcess::OS_Time;

        st_Pulse_Lock[uc_i].uc_UseFlag = 0;
        st_Pulse_Lock[uc_i].uc_ControleType = CT_IVCH_STOP;
    }
    ControlPin::SetMeHigh(board_config::Pin_ACIM);
    ARBUS_DELAY(10);
}


// ******************************************************/
// Function Name: InitACR
// Version:
// Date:
// Description:
//                         ACR��ʼ��
// Others:
//
// History��
//
// ******************************************************/
void ACR::InitACR(unsigned char uc_ChannelNo)
{
    unsigned char ActiveIdx=ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;

    unsigned char Range;
    //    char BoardType = ChannelInfo_Manager::BoardCFG.m_Global.m_u16BoardType;
    Step_Manager_task::bNeedCheck[uc_ChannelNo]=0;   //  ������κ�Limit.

    ACR_EndTime[uc_ChannelNo] = OS_ServeProcess::OS_Time;
    ACR_Status[uc_ChannelNo] = ACR_START;

    ACR_T1[uc_ChannelNo]=ACR_INTERVAL100us*5;  //T1=500us;  //��������׼��ʱ��

    //ACRChannelFlg|=(1<<uc_ChannelNo);


    //     if((ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0] == 0)
    //         ||(ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[1] == 0))
    //{
    //#if ( _BOARD_TYPE_ == BOARDTYPE_LBT2108)
    ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCurrentRange =  ChannelInfo_Manager::CurrRngPhysicalRange1;
    ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0]  = 1;  //0.5;
    //#else
    //ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCurrentRange =  CURRENT_RANGE_1;
    //ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0]  = 0.2;
    //#endif
    ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[1]  = ACR_MAXSCH_TIME;   // 6�����
    //}

    ACR_CycleNum[uc_ChannelNo] = (unsigned long)(ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[1]*1000);  // �����룬����������
    Range = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCurrentRange;
    // ����
    float fValue2 = ChannelInfo_Manager::CurrentCheck(uc_ChannelNo,abs(ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0] + ACR_I_Offset[uc_ChannelNo]));	//170719 zyx
//	fValue2 = ChannelInfo_Manager::ClampPower(uc_ChannelNo, fValue2);                 //160518
    ACR_Positive_Dac[uc_ChannelNo][ACR_H_DAC]=ChannelInfo_Manager::CalibrateCurrentDAC(uc_ChannelNo,Range,fValue2);

    fValue2 = ChannelInfo_Manager::CurrentCheck(uc_ChannelNo,abs(0 + ACR_I_Offset[uc_ChannelNo]));	//170719 zyx
//	fValue2 = ChannelInfo_Manager::ClampPower(uc_ChannelNo,fValue2);
    ACR_Positive_Dac[uc_ChannelNo][ACR_L_DAC]=ChannelInfo_Manager::CalibrateCurrentDAC(uc_ChannelNo,Range,fValue2);
    // ����
    fValue2 = ChannelInfo_Manager::CurrentCheck(uc_ChannelNo,-abs(0 + ACR_I_Offset[uc_ChannelNo]));	//170719 zyx
//	fValue2 = ChannelInfo_Manager::ClampPower(uc_ChannelNo,fValue2);
    ACR_Negative_Dac[uc_ChannelNo][ACR_H_DAC]=ChannelInfo_Manager::CalibrateCurrentDAC(uc_ChannelNo,Range,fValue2);

    fValue2 = ChannelInfo_Manager::CurrentCheck(uc_ChannelNo,-abs(ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0] + ACR_I_Offset[uc_ChannelNo]));	//170719 zyx
//    fValue2 = ChannelInfo_Manager::ClampPower(uc_ChannelNo,fValue2);
    ACR_Negative_Dac[uc_ChannelNo][ACR_L_DAC]=ChannelInfo_Manager::CalibrateCurrentDAC(uc_ChannelNo,Range,fValue2);

    //ACR_IHighDacCode[uc_ChannelNo] = ACR_Positive_Dac[uc_ChannelNo][];  // ����
    //ACR_ILowDacCode[uc_ChannelNo] =  ACR_Negative_Dac[uc_ChannelNo];  // ����
    //ACR_ILowDacCode[uc_ChannelNo]=ChannelInfo_Manager::CalibrateCurrentDAC(uc_ChannelNo,Range,0 + ACR_I_Offset[uc_ChannelNo]);
    ACRTimerStatus[uc_ChannelNo] = ACR_TIMER_HIGH;
    ACR_CycleCount[uc_ChannelNo] = 0;
    ACR::ACR_Calibrate[uc_ChannelNo] = FALSE;

    ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue = 0;    // ������ʾ����0A

    unsigned char  uc_Flag = ACR::TimerChannel_Put(uc_ChannelNo,CT_IVCH_ACR);
    if(!uc_Flag)
    {
        ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = GOSTOP;
        Schedule_Manager_task::SendFinishFlag[uc_ChannelNo].StopStatus = STOP_ACR_OVERTIME;
    }
}


// ******************************************************/
// Function Name: InitACR
// Version:
// Date:
// Description:
//                         У��ʱACR��ʼ��
// Others:
//
// History��
//
// ******************************************************/
void ACR::Calibrate_InitACR(unsigned char uc_ChannelNo)
{

    ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx = 0;
    //#if ( _BOARD_TYPE_ == BOARDTYPE_LBT2108)
    ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[0].m_StepInfo.m_Data.m_Ctrl.m_ucCurrentRange = ChannelInfo_Manager::CurrRngPhysicalRange1;
    ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[0].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0]  = 1;  //0.5;
    //#else
    //ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[0].m_StepInfo.m_Data.m_Ctrl.m_ucCurrentRange =  CURRENT_RANGE_1;
    //ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[0].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0]  = 0.2;
    //#endif

    ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[0].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[1]  = 3600*2;
    TIMERSFunc::Timer1Handler = &ACR::ProcessACRTimer1;

    unsigned char ActiveIdx=ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
    unsigned char Range;
    //    char BoardType = ChannelInfo_Manager::BoardCFG.m_Global.m_u16BoardType;
    Step_Manager_task::bNeedCheck[uc_ChannelNo]=0;   //  ������κ�Limit.

    ACR_EndTime[uc_ChannelNo] = OS_ServeProcess::OS_Time;
    //ACR_Status[uc_ChannelNo] = ACR_START;

    ACR_T1[uc_ChannelNo]=ACR_INTERVAL100us*5;  //T1=500us;  //��������׼��ʱ��

    //ACRChannelFlg|=(1<<uc_ChannelNo);



    ACR_CycleNum[uc_ChannelNo] = (unsigned long)(ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[1]*1000);  // �����룬����������

    Range=ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCurrentRange;
    // ����
    float fValue2 = ChannelInfo_Manager::CurrentCheck(uc_ChannelNo,abs(ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0] + ACR_I_Offset[uc_ChannelNo]));	//170719 zyx
//    fValue2 = ChannelInfo_Manager::ClampPower(uc_ChannelNo,fValue2);
    ACR_Positive_Dac[uc_ChannelNo][ACR_H_DAC]=ChannelInfo_Manager::CalibrateCurrentDAC(uc_ChannelNo,Range,fValue2);
    fValue2 = ChannelInfo_Manager::CurrentCheck(uc_ChannelNo,abs(0 + ACR_I_Offset[uc_ChannelNo]));
//    fValue2 = ChannelInfo_Manager::ClampPower(uc_ChannelNo,fValue2);
    ACR_Positive_Dac[uc_ChannelNo][ACR_L_DAC]=ChannelInfo_Manager::CalibrateCurrentDAC(uc_ChannelNo,Range,fValue2);
    // ����
    fValue2 = ChannelInfo_Manager::CurrentCheck(uc_ChannelNo,-abs(0 + ACR_I_Offset[uc_ChannelNo]));
//    fValue2 = ChannelInfo_Manager::ClampPower(uc_ChannelNo,fValue2);
    ACR_Negative_Dac[uc_ChannelNo][ACR_H_DAC]=ChannelInfo_Manager::CalibrateCurrentDAC(uc_ChannelNo,Range,fValue2);
    fValue2 = ChannelInfo_Manager::CurrentCheck(uc_ChannelNo,-abs(ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0] + ACR_I_Offset[uc_ChannelNo]));
//    fValue2 = ChannelInfo_Manager::ClampPower(uc_ChannelNo,fValue2);
    ACR_Negative_Dac[uc_ChannelNo][ACR_L_DAC]=ChannelInfo_Manager::CalibrateCurrentDAC(uc_ChannelNo,Range,fValue2);

    //ACR_IHighDacCode[uc_ChannelNo] = ACR_Positive_Dac[uc_ChannelNo];  // ����
    //ACR_ILowDacCode[uc_ChannelNo] =  ACR_Negative_Dac[uc_ChannelNo];  // ����
    //ACR_ILowDacCode[uc_ChannelNo]=ChannelInfo_Manager::CalibrateCurrentDAC(uc_ChannelNo,Range,0 + ACR_I_Offset[uc_ChannelNo]);

    ACRTimerStatus[uc_ChannelNo] = ACR_TIMER_HIGH;
    ACR_CycleCount[uc_ChannelNo] = 0;
    ACR::ACR_Calibrate[uc_ChannelNo] = TRUE;

    //ACR::TimerChannel_Put(uc_ChannelNo,CT_IVCH_ACR);
}


/*****************************************************
// Function Name: ProcessACR
// Version:
// Date:
// Description:
//                      ACR��ʱ����
// Others:
//
// History��
//
// ******************************************************/
void ACR::ProcessACR(unsigned char uc_ChannelNo)
{
    switch(ACR_Status[uc_ChannelNo])
    {
    case ACR_START:
        ACR_StartTime[uc_ChannelNo] = OS_ServeProcess::OS_Time;
        //ACR_Status[uc_ChannelNo]= ACR_HARDWARE_OPERATION;
        ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue = 0;   // ������ʾ����0A
        break;

    case ACR_HARDWARE_OPERATION:
    {
        MsTime ACR_Time =OS_ServeProcess::TimeDiff(ACR_StartTime[uc_ChannelNo]);    //���ʱ���
        if((ACR_Time.Second*10000+ACR_Time.Us100)>=ACR_MAXWAIT_TIME)
        {
            ACR_Status[uc_ChannelNo]= ACR_RUNNING;
            ChannelInfo_Manager::Reset_ACR2HistoryData();
        }
        break;
    }
    case ACR_RUNNING:
        //             if(ACR::ACR_Calibrate[uc_ChannelNo])
        //             {
        //                 ACR_Calculate(uc_ChannelNo);
        //             }
        //
        ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue = 0;    // ������ʾ����0A
        if(ACR_CycleCount[uc_ChannelNo]>=ACR_CycleNum[uc_ChannelNo])
        {
            TIMERSFunc::Disable_Timer(1);
            ACR_Status[uc_ChannelNo]= ACR_CALCULATE;
            ACR::DAC_Flag=1;
            board_config::DAC_out(uc_ChannelNo,DAC_OUT_PUT_ZERO,ACTIVE_DAC_CUR); //���������������
            ACR::DAC_Flag=0;

        }
        break;

    case ACR_CALCULATE:
        ACR_Status[uc_ChannelNo]= ACR_END;
        ACR_Calculate(uc_ChannelNo);
        break;

    case ACR_END:
    {
        ACR_TimerEn=false;
        ACRChannelFlg&=~(1<<uc_ChannelNo); //ȥ�����λ
        Lock_Clear(uc_ChannelNo,CT_IVCH_ACR);
        ACR::WriteChannel(uc_ChannelNo,ACR_OFF);   //   �ر�����Ӳ��
        board_config::ACR_V(ACR_OFF);
        ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_limitInfo[0].m_bChecked = 1;
        ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_limitInfo[0].m_bReached = 1;
        Step_Manager_task::bStepLimitChecked[uc_ChannelNo] = 1;
        unsigned char ActiveIdx=ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;
        //ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Limits[0].m_fRight_Value = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[1];
        //ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Limits[0].m_ucRightValue_DataType = MetaCode_PV_Steptime;
        if(ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucStepLimitNo<1)
        {
            ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucStepLimitNo = 1;
            UWord16 uw16_GotoStep = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Limits[0].m_u16GotoStepID;
            if(uw16_GotoStep == MCU_GOTOSTOPSTEP)
                ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Limits[0].m_u16GotoStepID = MCU_GOTOSTOPSTEP;
            else
                ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Limits[0].m_u16GotoStepID = MCU_GOTONEXTSTEP;
        }
        TIMERSFunc::Disable_Timer(1);
        ACR_Status[uc_ChannelNo] = ACR_START;
        //TIMERSFunc::Timer1Handler = &IR::ProcessIRTimer1;
        if(!ACR::ACR_Calibrate[uc_ChannelNo])
        {
            ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_ACR].m_fValue = ACR_R_V[ACR_RESULT];
        }
        break;
    }
    default:
        break;
    }

    if(ACR_TimerEn==false)      //��ʱ������
    {
        if((ACR::st_Single_Lock.st_Lock.uc_UseFlag)&&(ACR::st_Single_Lock.st_Lock.uc_ControleType != CT_IVCH_STOP))
            return;
        signed short w16_Ch = TimerChannel_Out(CT_IVCH_ACR);
        if((ACR::st_Single_Lock.st_Lock.uc_UseFlag)&&(ACR::st_Single_Lock.st_Lock.uc_ControleType != CT_IVCH_ACR))
            return;

        if(w16_Ch >= 0)
        {
            ACRChannelFlg=(1<<w16_Ch);
            ACRChannelNo = w16_Ch;
            ACR_StartTime[uc_ChannelNo] = OS_ServeProcess::OS_Time;
            ACR_Status[ACRChannelNo]= ACR_HARDWARE_OPERATION;
            ACR::WriteChannel(ACRChannelNo,ACR_ON);   //   ������Ӳ��
            //ChannelInfo_Manager::ChannelInfo[ACRChannelNo].m_ucVoltageRange = VOLTAGE_RANGE_LOW;   //�л���ǰrange;
            ACR_TimerEn=true;
            ACRTimerStatus[ACRChannelNo] = ACR_TIMER_HIGH;
            ACR_NegativeFlag = FALSE;
            for(unsigned char uc_i=0; uc_i<ACR_R_COUNT; uc_i++)
                ACR_R_V[uc_i] = 0;
            ACR_IHighDacCode = ACR_Positive_Dac[ACRChannelNo][ACR_H_DAC];
            ACR_ILowDacCode = ACR_Positive_Dac[ACRChannelNo][ACR_L_DAC];
            TIMERSFunc::Timer1Handler = &ACR::ProcessACRTimer1;
            TIMERSFunc::Init_Timer(ACR_T1[ACRChannelNo],1);  //��ʼ����ʱ��
            TIMERSFunc::Enable_Timer(1);
            ChannelInfo_Manager::Reset_ACR2HistoryData();
        }
    }

    if(!ACR::ACR_Calibrate[uc_ChannelNo])  // ��ʱ���޷��õ���Դ�Զ��˳�
    {
        MsTime ACR_Time = OS_ServeProcess::TimeDiff(ACR_EndTime[uc_ChannelNo]);    //���ʱ���
        if((ACR_Time.Second*10000+ACR_Time.Us100)>=ACR_MAXSCH_ENDTIME)
        {
            //             St_ScheduleMessage sch_msg;
            // 			sch_msg.MsgNumber = Sch_Stop_Msg;
            // 			sch_msg.m_Message.m_Assign.m_u16ChannelIdx = uc_ChannelNo;
            // 			Schedule_Manager_task::SendFinishFlag[uc_ChannelNo].StopStatus = STOP_ACR_OVERTIME;
            // 			ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_SafetyStatus = 0;
            // 			Schedule_Manager_task::SendMsgToSchedule(uc_ChannelNo,sch_msg);

            ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = GOSTOP;
            Schedule_Manager_task::SendFinishFlag[uc_ChannelNo].StopStatus = STOP_ACR_OVERTIME;
        }
    }
}

/*****************************************************
// Function Name: ProcessACRTimer1
// Version:        V1.0
// Date:
// Description:
//                 ACR��ʱ���жϴ���
// Others:
//
// History��
//
// ******************************************************/
void ACR::ProcessACRTimer1(void)
{
    ACR_T1_Count[ACRChannelNo] ++;

    switch(ACRTimerStatus[ACRChannelNo])
    {
    case ACR_TIMER_HIGH:
        DAC_Flag=1;
        board_config::DAC_out(ACRChannelNo,ACR_IHighDacCode,ACTIVE_DAC_CUR);
        DAC_Flag=0;
        //LPC_TIM1->MR0 =ACR_T1[ACRChannelNo];
        break;
    case ACR_TIMER_LOW:
        DAC_Flag=1;
        board_config::DAC_out(ACRChannelNo,ACR_ILowDacCode,ACTIVE_DAC_CUR);
        DAC_Flag=0;
        //LPC_TIM1->MR0 =ACR_T1[ACRChannelNo];
        break;
    case ACR_TIMER_NEGATIVE:
        ACR_NegativeFlag = TRUE;
        ACRTimerStatus[ACRChannelNo] = ACR_TIMER_HIGH;
        ACR_IHighDacCode = ACR_Negative_Dac[ACRChannelNo][ACR_H_DAC];
        ACR_ILowDacCode = ACR_Negative_Dac[ACRChannelNo][ACR_L_DAC];
        ChannelInfo_Manager::Reset_ACR2HistoryData();
        break;
    case ACR_TIMER_END:
    default:
        break;
    }

    if(ACR_T1_Count[ACRChannelNo] >= MAX_ACR_T1)
    {
        ACR_T1_Count[ACRChannelNo] = 0;
        if(ACRTimerStatus[ACRChannelNo] == ACR_TIMER_HIGH)
            ACRTimerStatus[ACRChannelNo] = ACR_TIMER_LOW;
        else if(ACRTimerStatus[ACRChannelNo] == ACR_TIMER_LOW)
        {
            ACRTimerStatus[ACRChannelNo] = ACR_TIMER_HIGH;
            ACR_CycleCount[ACRChannelNo] ++;
        }

        if(!ACR_NegativeFlag)
        {
            if(ACR_CycleCount[ACRChannelNo]>=(ACR_CycleNum[ACRChannelNo]/2))
            {
                ACR_R_V[ACR_POSITIVE] = ChannelInfo_Manager::MetaVariable2[ACRChannelNo][MetaCode_PV_ACR].m_fValue;
                ACRTimerStatus[ACRChannelNo] = ACR_TIMER_NEGATIVE;
            }
        }

        if((ACR_CycleCount[ACRChannelNo]>=ACR_CycleNum[ACRChannelNo])
                ||(ChannelInfo_Manager::ChannelInfo[ACRChannelNo].ScheduleStatus == GOSTOP))
        {
            ACR_R_V[ACR_NEGATIVE] = ChannelInfo_Manager::MetaVariable2[ACRChannelNo][MetaCode_PV_ACR].m_fValue;
            ACRTimerStatus[ACRChannelNo] = ACR_TIMER_END;
            //ACR_CycleCount[ACRChannelNo] = 0;
            ACR::DAC_Flag=1;
            board_config::DAC_out(ACRChannelNo,DAC_OUT_PUT_ZERO,ACTIVE_DAC_CUR); //���������������
            ACR::DAC_Flag=0;
            TIMERSFunc::Disable_Timer(1);
        }

        if(!ACR::ACR_Calibrate[ACRChannelNo])
        {
            unsigned char ActiveIdx=ChannelInfo_Manager::ChannelStep[ACRChannelNo].m_ucActiveIdx;  // ����������޷��˳�ACR�����⡣  20150908 yy
            UWord16 uw16_CtrlType = ChannelInfo_Manager::ChannelStep[ACRChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_u16ControlType;
            if(uw16_CtrlType != CT_IVCH_ACR)
            {
                ACR_TimerEn=false;
                ACRChannelFlg&=~(1<<ACRChannelNo); //ȥ�����λ
                Lock_Clear(ACRChannelNo,CT_IVCH_ACR);
                ACR::WriteChannel(ACRChannelNo,ACR_OFF);   //   �ر�����Ӳ��
                board_config::ACR_V(ACR_OFF);
                TIMERSFunc::Disable_Timer(1);
            }
        }
    }
}
/*****************************************************
// Function Name: ACR_Calculate
// Version:        V1.0
// Date:
// Description:
//                 ACR�������
// Others:
//
// History��
//
// ******************************************************/
void ACR::ACR_Calculate(unsigned char uc_ChannelNo)
{
    /*    unsigned long int uw32_V = board_config::ADC_Read(uc_ChannelNo,ADC_TYPE_V);

    //float  f_V = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Voltage].m_fValue;
    float  f_V = (float) uw32_V;
    float  f_G = ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_ACR_R.m_Gain;
    float  f_Offset =  ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_ACR_R.m_Offset;
    //    unsigned char ActiveIdx=ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;

    // 	ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_InternalResistance].m_fValue =
    //        ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Voltage].m_fValue/ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue;

    //    	ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_InternalResistance].m_fValue =
    //        ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Voltage].m_fValue/abs(ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues.m_fControlValue[0]);

    f_V =  f_V* f_G + f_Offset;
    ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_InternalResistance].m_fValue =  f_V;  */

    if(!ACR::ACR_Calibrate[uc_ChannelNo])
    {
        //IVSDL::uc_ACR_SendCount[uc_ChannelNo] = 1;             // ACR���ݲ������ϴ�������   // �������ACR����¼��   20150908 yy

        ACR_R_V[ACR_RESULT] = ACR_R_V[ACR_POSITIVE];
        //ACR_R_V[ACR_RESULT] = (ACR_R_V[ACR_POSITIVE]+ACR_R_V[ACR_NEGATIVE])/2;  // ȡƽ��ֵ
    }
}

/*****************************************************
// Function Name: TimerChannel_Put
// Version:        V1.0
// Date:
// Description:
//                 ACR���廥������
// Others:
//
// History��
//
// ******************************************************/
unsigned char ACR::TimerChannel_Put(unsigned char uc_ChannelNo,unsigned char uc_CtrlType)
{
    ST_PULSECH  st_ch;
    st_ch.uc_Ch = uc_ChannelNo;
    st_ch.uc_ControleType = uc_CtrlType;

    if(IS_QUEUE_FULL(Pulse_Lock_Ch,MAX_ACR_CH_BUFFER))
        return 0;

    //if(!st_Pulse_Lock[uc_ChannelNo].uc_UseFlag)
    ENQUEUE(Pulse_Lock_Ch,MAX_ACR_CH_BUFFER,st_ch);

    st_Pulse_Lock[uc_ChannelNo].uc_UseFlag = 1;
    st_Pulse_Lock[uc_ChannelNo].uc_ControleType = uc_CtrlType;
    uc_PLock_Count ++;

    return 1;
}


/*****************************************************
// Function Name: TimerChannel_Out
// Version:        V1.0
// Date:
// Description:
//                 ACR���廥������
// Others:
//
// History��
//
// ******************************************************/
signed int ACR::TimerChannel_Out(unsigned char uc_CtrlType)
{
    signed int w16_i=-1;
    if(st_Single_Lock.st_Lock.uc_UseFlag == 1)
    {
        if(st_Single_Lock.st_Lock.uc_ControleType != CT_IVCH_IR)
            return (-1);
    }

    if(uc_CtrlType == CT_IVCH_IR)   //  ����IR�Ƚ��ر��ռʱ��ͨ�����ٷ�ʱ����
    {
        if(IR::IR_CopyFlg == 0)
        {   // �״�IR����
            w16_i = TimerChannel_Check(uc_CtrlType);
            if(w16_i == -1)
                return (-1);
            IR::IRChannelFlg = IR::IRChannelFlg_CopyInit[0];
            IR::IRChannelFlg_CopyInit[1] = ~IR::IRChannelFlg_CopyInit[0];   // [X]  0:ԭֵ 1������ֵ
            IR::IRChannelFlg_CopyInit[0] = 0;
            IR::IR_CopyFlg = 1;
        }
        else
        {   // IR�����д���ʹ�ÿ��ŵ�����
            unsigned int uw16_Flag = 0;
            uw16_Flag = IR::IRChannelFlg_CopyInit[0] & IR::IRChannelFlg_CopyInit[1];  // ȡ����ִ��λ  �� 0X000F&0XFFFF = 0X000F
            if(uw16_Flag != 0)
            {
                IR::IRChannelFlg_CopyInit[1] = uw16_Flag ^ IR::IRChannelFlg_CopyInit[1];  // ����λ���¿���  �� 0X000F^0XFFFF= 0XFFF0
                IR::IRChannelFlg_CopyInit[0] = uw16_Flag ^ IR::IRChannelFlg_CopyInit[0];  // ׼��ִ��λ��������  �� 0X000F^0X000F = 0X0000
                IR::IRChannelFlg |= uw16_Flag;    // ��ִֵ��λ����
            }
        }


        if(IR::IRChannelFlg != 0)
        {
            for(unsigned char uc_n=0; uc_n<MAXCHANNELNO; uc_n++)
            {
                if((IR::IRChannelFlg>>uc_n)&0X0001)
                {
                    w16_i = uc_n;
                    break;
                }
            }
        }
        else
        {
            IR::IR_CopyFlg = 0;      //  20151008 yang
        }

        if(w16_i != -1)
            TIMERSFunc::Timer1Handler = &IR::ProcessIRTimer1;
    }
    else
    {
        if((st_Single_Lock.st_Lock.uc_UseFlag)&&(st_Single_Lock.st_Lock.uc_ControleType == CT_IVCH_IR))
            return (-1);

        w16_i = TimerChannel_Check(uc_CtrlType);
        if(w16_i != -1)
        {
            //st_Pulse_Lock[w16_i].uc_UseFlag = 0;
            //st_Pulse_Lock[w16_i].uc_ControleType = CT_IVCH_STOP;
            if(uc_CtrlType == CT_IVCH_ACR)
                TIMERSFunc::Timer1Handler = &ACR::ProcessACRTimer1;
            IR::IR_CopyFlg = 0;
        }
    }


    if(w16_i >= 0)
    {
        //uc_PLock_Count --;
        st_Single_Lock.st_Lock.uc_UseFlag = 1;
        st_Single_Lock.st_Lock.uc_ControleType = uc_CtrlType;
        st_Single_Lock.uc_Ch = w16_i;

        if(uc_CtrlType != CT_IVCH_IR)
            IR::IR_CopyFlg = 0;
        return w16_i;
    }
    return (-1);
}

/*****************************************************
// Function Name: TimerChannel_Check
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                 ACR���廥������
// Others:
//
// History��
//
// ******************************************************/
signed int ACR::TimerChannel_Check(unsigned char uc_CtrlType)
{
    unsigned char uc_OK;
    ST_PULSECH  st_ch;

    while((QUEUE_DEPTH(Pulse_Lock_Ch)>0))
    {
        if(IS_QUEUE_EMPTY(Pulse_Lock_Ch))
            break;

        DEQUEUE(Pulse_Lock_Ch,MAX_ACR_CH_BUFFER,st_ch,uc_OK);
        if(st_Pulse_Lock[st_ch.uc_Ch].uc_UseFlag)
        {   // ��Ч����

            if(st_ch.uc_ControleType == uc_CtrlType)
            {
                if(st_Pulse_Lock[st_ch.uc_Ch].uc_ControleType == uc_CtrlType)
                    return (st_ch.uc_Ch);   // ��Ч����
            }

            Pulse_Lock_Ch.m_u32Start = Pulse_Lock_Ch.m_u32Start>=1?Pulse_Lock_Ch.m_u32Start-1:MAX_ACR_CH_BUFFER;  // ��ԭ��ԭ״
            Pulse_Lock_Ch.m_u32Length++;
            return (-1);

        }
    }
    return (-1);
}

/*****************************************************
// Function Name: Lock_Clear
// Version:        V1.0
// Date:
// Description:
//                 ACR���廥�����
// Others:
//
// History��
//
// ******************************************************/
signed int ACR::Lock_Clear(unsigned char uc_ChannelNo,unsigned char uc_CtrlType)
{

    if(uc_CtrlType == CT_IVCH_IR)   //  ����IR�Ƚ��ر��ռʱ��ͨ�����ٷ�ʱ����
    {
        if(((IR::IRChannelFlg>>uc_ChannelNo)&0x0001) == 0)
        {
            st_Pulse_Lock[uc_ChannelNo].uc_UseFlag = 0;
            st_Pulse_Lock[uc_ChannelNo].uc_ControleType = CT_IVCH_STOP;
            Lock_QueueClear(uc_ChannelNo);  //  ����������޷��Զ���������
        }

        if(IR::IRChannelFlg != 0)
            return (-1);

        if(st_Single_Lock.st_Lock.uc_ControleType == CT_IVCH_IR)
        {
            st_Single_Lock.st_Lock.uc_UseFlag = 0;
            st_Single_Lock.st_Lock.uc_ControleType = CT_IVCH_STOP;
            st_Single_Lock.uc_Ch = 0XFF;
            //return (uc_ChannelNo);
        }
        return (uc_ChannelNo);
    }
    else
    {
        st_Pulse_Lock[uc_ChannelNo].uc_UseFlag = 0;
        st_Pulse_Lock[uc_ChannelNo].uc_ControleType = CT_IVCH_STOP;
        Lock_QueueClear(uc_ChannelNo);  //  ����������޷��Զ���������

        if((uc_CtrlType == CT_IVCH_ACR)&&(ACRChannelFlg == (1<<uc_ChannelNo)))
        {
            ACR_TimerEn=false;
            ACRChannelFlg&=~(1<<uc_ChannelNo); //ȥ�����λ
            ACR::WriteChannel(uc_ChannelNo,ACR_OFF);   //   �ر�����Ӳ��
        }

        if((st_Single_Lock.st_Lock.uc_UseFlag)&&(st_Single_Lock.uc_Ch == uc_ChannelNo))
        {
            if(st_Single_Lock.st_Lock.uc_ControleType == uc_CtrlType)
            {
                st_Single_Lock.st_Lock.uc_UseFlag = 0;
                st_Single_Lock.st_Lock.uc_ControleType = CT_IVCH_STOP;
                st_Single_Lock.uc_Ch = 0XFF;
                return (uc_ChannelNo);
            }
        }
    }

    unsigned char uc_n=0;
    for(uc_n=0; uc_n<MAXCHANNELNO; uc_n++)
    {
        if(st_Pulse_Lock[uc_n].uc_UseFlag != 0)
        {
            break;
        }
    }
    if(uc_n >= MAXCHANNELNO)
    {
        Pulse_Lock_Ch.m_u32Start = 0;  // �������ͨ��������
        Pulse_Lock_Ch.m_u32Length =0;

        st_Single_Lock.st_Lock.uc_UseFlag = 0;
        st_Single_Lock.st_Lock.uc_ControleType = CT_IVCH_STOP;
        st_Single_Lock.uc_Ch = 0XFF;

        INIT_QUEUE(Pulse_Lock_Ch);
        IR::IRChannelFlg_CopyInit[0] = 0;       //ͨ����IR���    // [X]  0:ԭֵ 1������ֵ
        IR::IRChannelFlg_CopyInit[1] = 0XFFFF;  //ͨ����IR���    // [X]  0:ԭֵ 1������ֵ
        IR::IR_CopyFlg = 0;
    }
    return (uc_ChannelNo);
}

/*****************************************************
// Function Name: Lock_QueueClear
// Version:        V1.0
// Date:
// Description:
//                 ACR���廥�������ظ�ͨ�������
// Others:
//
// History��
//
// ******************************************************/
void ACR::Lock_QueueClear(unsigned char uc_ChannelNo)
{
    if(!IS_QUEUE_EMPTY(Pulse_Lock_Ch))  //  ����������޷��Զ���������
    {
        ST_PULSECH  st_ch;
        unsigned long uw32_Count = 0;
        unsigned long uw32_Length = Pulse_Lock_Ch.m_u32Length;
        unsigned long uw32_Start = Pulse_Lock_Ch.m_u32Start;
        unsigned short uw16_Ch = Pulse_Lock_Ch.Object[uw32_Start].uc_Ch;
        memset((void *)&Lock_Ch_Copy,0,sizeof(Lock_Ch_Copy));
        INIT_QUEUE(Lock_Ch_Copy);

        for(unsigned long uw32_j=0; uw32_j<uw32_Length; uw32_j++)
        {
            if((Pulse_Lock_Ch.Object[uw32_Start].uc_Ch != uc_ChannelNo)
                    &&(st_Pulse_Lock[Pulse_Lock_Ch.Object[uw32_Start].uc_Ch].uc_UseFlag))
            {
                st_ch = Pulse_Lock_Ch.Object[uw32_Start];
                ENQUEUE(Lock_Ch_Copy,MAX_ACR_CH_BUFFER,st_ch);               // ȡ�׸���Ϊ�Լ���
                uw32_Count ++;

                for(unsigned long uw32_k=0; uw32_k<Pulse_Lock_Ch.m_u32Length; uw32_k++)
                {
                    if((uc_ChannelNo == Pulse_Lock_Ch.Object[Pulse_Lock_Ch.m_u32Start+uw32_k].uc_Ch)  // ���Ѿ�ɾ�������ó��������
                            ||(!st_Pulse_Lock[Pulse_Lock_Ch.Object[Pulse_Lock_Ch.m_u32Start+uw32_k].uc_Ch].uc_UseFlag))
                    {
                        Pulse_Lock_Ch.Object[Pulse_Lock_Ch.m_u32Start+uw32_k].uc_Ch = 0XFF;
                    }
                }

                uw32_Start =(uw32_Start + 1)%MAX_ACR_CH_BUFFER;
                uw32_Length --;
                for(unsigned short uw32_k=1; uw32_k<uw32_Length; uw32_k++)
                {
                    if(uw16_Ch == Pulse_Lock_Ch.Object[uw32_Start+uw32_k].uc_Ch)  // ����ͬ�����ó��������
                    {
                        Pulse_Lock_Ch.Object[uw32_Start+uw32_k].uc_Ch = 0XFF;
                    }
                }
                break;
            }
            uw32_Start =(uw32_Start + 1)%MAX_ACR_CH_BUFFER;
            uw32_Length --;
        }

        for(unsigned long uw32_j=0; uw32_j<uw32_Length; uw32_j++)
        {
            if((uw16_Ch != Pulse_Lock_Ch.Object[uw32_Start+uw32_j].uc_Ch)
                    &&(Pulse_Lock_Ch.Object[uw32_Start+uw32_j].uc_Ch != 0XFF)
                    &&(st_Pulse_Lock[Pulse_Lock_Ch.Object[uw32_Start+uw32_j].uc_Ch].uc_UseFlag))  // ȡ����ͬ��
            {
                st_ch = Pulse_Lock_Ch.Object[uw32_Start+uw32_j];
                ENQUEUE(Lock_Ch_Copy,MAX_ACR_CH_BUFFER,st_ch);
                uw32_Start =  (uw32_Start + uw32_j)%MAX_ACR_CH_BUFFER;
                uw32_Length = uw32_Length - uw32_j;
                uw32_Count ++;

                for(unsigned long uw32_k=1; uw32_k<uw32_Length; uw32_k++)
                {
                    if(uw16_Ch == Pulse_Lock_Ch.Object[uw32_Start+uw32_k].uc_Ch)  // ����ͬ�����ó��������
                    {
                        Pulse_Lock_Ch.Object[uw32_Start+uw32_k].uc_Ch = 0XFF;
                    }
                }
                uw32_j = 0;
            }
        }

        INIT_QUEUE(Pulse_Lock_Ch);
        Pulse_Lock_Ch = Lock_Ch_Copy;   // �����Ѵ�������ظ�ͨ���Ķ���
    }
}

/*****************************************************
// Function Name: WriteChannel
// Version:        V1.0
// Date:
// Description:
//                 ����ACR���ʰ�CPLDӲ��
// Others:
//
// History��
//
// ******************************************************/
void ACR::WriteChannel(unsigned char ChannelIdx,unsigned char SetFlag)
{
    //    unsigned short uw16_data = 0;

    if(ChannelIdx >= MAXCHANNELNO)
        return;

    board_config::ACR_En(ChannelIdx,SetFlag);   //  ��ѡһͨ������

}

/*****************************************************
// Function Name: WriteChannel
// Version:        V1.0
// Date:
// Description:
//                ����ACRMCU�ܿ��ص�ѹ����CPLDӲ��
// Others:
//
// History��
//
// ******************************************************/
void ACR::Write_V(unsigned char ChannelIdx,unsigned char SetFlag)
{
    if(SetFlag == ACR_OFF)     //  ��ѡһ������ѹ����
    {
        board_config::ACR_V(ACR_OFF);
        return;
    }

    if(ACR_TimerEn)
    {
        if(ACRChannelNo == ChannelIdx)
        {
            board_config::ACR_V(SetFlag);
        }
    }
}

