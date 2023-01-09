///////////////////////////////////////////////////////////////////////////////////
//File Name: IR.cpp
//Version:		V1.0
//Date:				2012/11/9
//Description:
//						IR操作
//Others:
//History：

///////////////////////////////////////////////////////////////////////////////////
#include "../Entry/includes.h"

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
unsigned char 	IR::IR_Status[MAXCHANNELNO];
unsigned char 	IR::IR_StageIO[MAXCHANNELNO];
// MsTime  	        IR::IR_TimeInterval[MAXCHANNELNO];
unsigned long long  IR::IR_TimeInterval[MAXCHANNELNO];
float  	        IR::IR_Stage[MAXCHANNELNO];
unsigned long 	IR::IR_IHighDacCode[MAXCHANNELNO];
unsigned long 	IR::IR_ILowDacCode[MAXCHANNELNO];
float        	IR::IR_SampleInterval[MAXCHANNELNO];
unsigned char 	IR::IR_PulseRecord[MAXCHANNELNO];
float 	        IR::IR_IHigh[MAXCHANNELNO][MAXPULSE];
float 	        IR::IR_ILow[MAXCHANNELNO][MAXPULSE];
float 	        IR::IR_VHigh[MAXCHANNELNO][MAXPULSE];
float 	        IR::IR_VLow[MAXCHANNELNO][MAXPULSE];
unsigned char 	IR::IR_SampleFlagH[MAXCHANNELNO];
unsigned char 	IR::IR_SampleFlagL[MAXCHANNELNO];
MsTime 	    	IR::IR_StartTime[MAXCHANNELNO];

St_IR_SampCode  IR::SampCode[MAXCHANNELNO];

// IR_T1,, T5 is replaced by the IR_Output_DAC_1st ,,,, IR_Output_Current_0
//UWord32 IR::IR_T1[MAXCHANNELNO];
//UWord32 IR::IR_T2[MAXCHANNELNO];
//UWord32 IR::IR_T3[MAXCHANNELNO];
//UWord32 IR::IR_T4[MAXCHANNELNO];
//UWord32 IR::IR_T5[MAXCHANNELNO];

UWord32 IR::IR_Output_DAC_1st[MAXCHANNELNO];
UWord32 IR::IR_Trigger_ADC_1st[MAXCHANNELNO];
//UWord32 IR::IR_Sample_Data_1st[MAXCHANNELNO];//1903271 zyx
UWord32 IR::IR_Output_DAC_2nd[MAXCHANNELNO];
UWord32 IR::IR_Trigger_ADC_2nd[MAXCHANNELNO];
//UWord32 IR::IR_Sample_Data_2nd[MAXCHANNELNO];//1903271 zyx
UWord32 IR::IR_Output_Current_0[MAXCHANNELNO];

unsigned int IR::Set_LowPulseDacTime;
unsigned int IR::Set_LowPulseAdcTime;
unsigned int IR::Set_HighPulseDacTime;
unsigned int IR::Set_HighPulseAdcTime;
unsigned int IR::Set_MinimumPulseTime;

float IR::PulseH[MAXCHANNELNO]; //IR特殊检查
float IR::PulseL[MAXCHANNELNO];
// float			IR::IR_SpecialStage[MAXCHANNELNO];
// unsigned char	IR::IR_SpecialStatus[MAXCHANNELNO];
// unsigned char	IR::IR_SpecialIO[MAXCHANNELNO];
// MsTime			IR::IR_SpecialTimeInterval[MAXCHANNELNO];
// MsTime			IR::IR_SpecialStartTime[MAXCHANNELNO];	//0819
// float			IR::IR_SpecialSampleInterval[MAXCHANNELNO];
// unsigned char	IR::IR_SpecialSampleFlagH[MAXCHANNELNO];
// unsigned char	IR::IR_SpecialSampleFlagL[MAXCHANNELNO];
// unsigned char	IR::IR_SpecialPulseRecord[MAXCHANNELNO];
// float			IR::IR_SpecialIHigh[MAXCHANNELNO][MAXPULSE];
// float			IR::IR_SpecialILow[MAXCHANNELNO][MAXPULSE];
// float			IR::IR_SpecialVHigh[MAXCHANNELNO][MAXPULSE];
// float			IR::IR_SpecialVLow[MAXCHANNELNO][MAXPULSE];
// unsigned long	IR::IR_SpecialIHighDacCode[MAXCHANNELNO];
// unsigned long	IR::IR_SpecialILowDacCode[MAXCHANNELNO];
unsigned short  IR::IRSpecialGotoNext[MAXCHANNELNO];
unsigned char	IR::IRSpecialRange[MAXCHANNELNO];
#pragma arm section
//float IR::SaveIRVoltage[MAXCHANNELNO];//临时用于处理IR电压问题，若有其他衍生问题请删除

unsigned char 	IR::IR_CtrlType[MAXCHANNELNO];
float 			IR::IR_Value[MAXCHANNELNO];
float IR::MinAmp[MAXCHANNELNO];
float IR::Offset[MAXCHANNELNO];
unsigned char IR::Range[MAXCHANNELNO];
float IR::DAmp[MAXCHANNELNO];
unsigned char 	IR::CheckIRChannel = 0; //控制权
// unsigned char 	IR::CurrentIRChannel=0;//当前IR通道
unsigned char 	IR::IRTimerStatus = 0;
unsigned char 	IR::IRChannelNo = 0;
unsigned short 	IR::IRChannelFlg = 0; //通道做IR标记

//unsigned short 	IR::IRChannelFlg_CopyInit[2];  //通道做IR标记   // [X]  0:原值 1：过滤值//903271 zyx
//unsigned char	IR::IR_CopyFlg=0;  //IR标记//1903271 zyx

//unsigned char IR::IR_TriggerFlag;//1903271 zyx
unsigned char 	IR::IR_TimerEn = FALSE;
//unsigned char   IR::bFastMeasure = 0;

char 	(* IR::pIR_ADC)(char uc_ChannelNo);
char 	(*IR::pRenew_IR_ADC)(void);
void    (*IR::pIR_Trigger_ADC)(void);
void 	(*IR::pIR_Read_ADC)(char uc_ChannelNo, unsigned long *SampleV, unsigned long *SampleI);

unsigned char 	IR::DAC_Flag;
unsigned char   IR::ADG_Ready; // add by DKQ 01.27.2017
UWord32 		IR::WaitTime;
//unsigned char 	IR::WaitRecord;
// ******************************************************/
// Function Name: InitIR
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                         IR初始化
// Others:
//
// History：
//
// ******************************************************/
void IR::InitIR(unsigned char uc_ChannelNo)
{
    unsigned char ActiveIdx = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx;

//	unsigned char Range;
// 	char BoardType = ChannelInfo_Manager::m_st_BoadType.m_u16BoardType;
    Step_Manager_task::bNeedCheck[uc_ChannelNo] = 0; //Add by DKQ 02_04_2014 for fixing after_IR issue

    Un_CtrlValues * p_Value = &ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_CtrlValues;

    /* if Cap hasn't been set*/
//     float fNorminalCapa[MAXCHANNELNO];
//     for(unsigned char uc_i = 0; uc_i < MAXCHANNELNO; uc_i++)
//     {
    //fNorminalV    = ChannelInfo_Manager::MetaVariable2[uc_i][MetaCode_MV_NorminalVoltage].m_fValue;
//         fNorminalCapa[uc_i] = ChannelInfo_Manager::MetaVariable2[uc_i][MetaCode_MV_NorminalCapacity].m_fValue;

    //if(fNorminalV <= 0)
    //	fNorminalV = ChannelInfo_Manager::MetaVariable2[uc_i][MetaCode_MV_VHmax].m_fValue;
//         if(fNorminalCapa[uc_i] <= 0)
//             fNorminalCapa[uc_i] = ChannelInfo_Manager::MetaVariable2[uc_i][MetaCode_MV_IHmax].m_fValue;
    //fNorminalEnergy = fNorminalCapa * fNorminalV;

    float fNorminalCapa = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_MV_NorminalCapacity].m_fValue;//1903271 zyx
    if(fNorminalCapa <= 0)//Software should protect it greater to 0
        fNorminalCapa = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_MV_IHmax].m_fValue;


    ///////////////////////////

    //CheckIRChannel=0;     //  2013/08/29  狄瑞
    IR_Value[uc_ChannelNo] = 0;
    IR_PulseRecord[uc_ChannelNo] = 0;
    if(Schedule_Manager_task::IRSpecialTestFlg[uc_ChannelNo])
    {
        ChannelInfo_Manager::m_ChannelIRJudge[uc_ChannelNo] = 0;
        IR_Stage[uc_ChannelNo] = MAXSPECIALIRSTAGETIME;
    }
    else
        IR_Stage[uc_ChannelNo] = p_Value->m_fControlValue[1] * 10;
    for(unsigned char i = 0; i < MAXPULSE; i++)
    {
        IR_VHigh[uc_ChannelNo][i] = 0;
        IR_IHigh[uc_ChannelNo][i] = 0;
        IR_VLow[uc_ChannelNo][i] = 0;
        IR_ILow[uc_ChannelNo][i] = 0;
    }

    if((p_Value->m_fControlValue[1] >= 50) || (Schedule_Manager_task::IRSpecialTestFlg[uc_ChannelNo] == 1) || Set_MinimumPulseTime >= 500) // 台阶时间分界线设置大于50毫秒要分时处理。
    {   // 分时处理
        if(IR_Stage[uc_ChannelNo] < Set_MinimumPulseTime)
            IR_Stage[uc_ChannelNo] = Set_MinimumPulseTime;
        IR_CtrlType[uc_ChannelNo] = TIMEDIVISION;
        IR_StageIO[uc_ChannelNo] = 0;
        IR_Status[uc_ChannelNo] = IRSTAGELOW; // 分时处理。
        IR_SampleFlagH[uc_ChannelNo] = false;
        IR_SampleFlagL[uc_ChannelNo] = false;
        CommonFunc::GetTimeTicksUpdate(&IR_StartTime[uc_ChannelNo]);
        //IR_StartTime[uc_ChannelNo] = OS_ServeProcess::OS_Time;
        IR_SampleInterval[uc_ChannelNo] = IR_Stage[uc_ChannelNo] * 3 / 4; //精确到Us100 取Stage 3/4 采样
    }
    else
    {   // 独立处理
//		UWord16 uw16_Type = Step_Manager_task::GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType;

//		if(uw16_Type == CT_IVCH_IR)
//		{
        if(IR_Stage[uc_ChannelNo] < Set_MinimumPulseTime)
            IR_Stage[uc_ChannelNo] = Set_MinimumPulseTime;
//    }

        // IR_T1,, T5 is replaced by the IR_Output_DAC_1st ,,,, IR_Output_Current_0

//		IR_T1[uc_ChannelNo]=IR_INTERVAL100us*100;  //T1=10ms;  //独立采样准备时间
//		IR_T2[uc_ChannelNo]=(unsigned long)IR_Stage[uc_ChannelNo]*IR_INTERVAL100us*Set_LowPulseDacTime/100;			//IR_INTERVAL100us*10;		  //ILow DAC建立时间
//		IR_T3[uc_ChannelNo]=(unsigned long)IR_Stage[uc_ChannelNo]*IR_INTERVAL100us*Set_LowPulseAdcTime/100;			//(IR_Stage[uc_ChannelNo]-1)*IR_INTERVAL100us;//IL采样时间
//		IR_T4[uc_ChannelNo]=(unsigned long)IR_Stage[uc_ChannelNo]*IR_INTERVAL100us*Set_HighPulseDacTime/100;			//IR_INTERVAL100us*10;			//IHigh DAC建立时间
//		IR_T5[uc_ChannelNo]=(unsigned long)IR_Stage[uc_ChannelNo]*IR_INTERVAL100us*Set_HighPulseAdcTime/100;			//(IR_Stage[uc_ChannelNo]-1)*IR_INTERVAL100us;		//IH采样时间

        IR_Output_DAC_1st[uc_ChannelNo] = IR_INTERVAL100us * 100; //T1=10ms;  //独立采样准备时间
        IR_Trigger_ADC_1st[uc_ChannelNo] = (unsigned long)IR_Stage[uc_ChannelNo] * IR_INTERVAL100us * Set_LowPulseDacTime / 100;			// trigger ADC at the 25% of the stage width
        //IR_Sample_Data_1st[uc_ChannelNo]=(unsigned long)IR_Stage[uc_ChannelNo]*IR_INTERVAL100us*0.65;	        // wait 50% of the stage width to archive data

        IR_Output_DAC_2nd[uc_ChannelNo] = (unsigned long)IR_Stage[uc_ChannelNo] * IR_INTERVAL100us * Set_LowPulseAdcTime / 100;  			// output DAC to next pulse at the 100% of the first stage width
        IR_Trigger_ADC_2nd[uc_ChannelNo] = (unsigned long)IR_Stage[uc_ChannelNo] * IR_INTERVAL100us * Set_HighPulseDacTime / 100;			// trigger ADC at the 25% of the second stage width
        IR_Output_Current_0[uc_ChannelNo] = (unsigned long)IR_Stage[uc_ChannelNo] * IR_INTERVAL100us * Set_HighPulseAdcTime / 100;			// output DAC to 0 at the 100% of the first stage width

        // IR_Sample_Data_2nd[uc_ChannelNo]=(unsigned long)IR_Stage[uc_ChannelNo]*IR_INTERVAL100us*0.30;	        // wait 50% of the second stage width to archive data





        //IRChannelFlg|=(1<<uc_ChannelNo);  //建立IR标记     20130830 狄
// 		IRChannelFlg_CopyInit[0] |=(1<<uc_ChannelNo);     // [X]  0:原值 1：过滤值
        IR_CtrlType[uc_ChannelNo] = TIMERINT;

        IR::IRChannelFlg |= (1 << uc_ChannelNo); //Add by DKQ 03_07_2014

// 			ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].ScheduleStatus = GOSTOP;
// 			Schedule_Manager_task::SendFinishFlag[uc_ChannelNo].StopStatus = STOP_ACR_OVERTIME;
    }
    Range[uc_ChannelNo] = ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ActiveIdx].m_StepInfo.m_Data.m_Ctrl.m_ucCurrentRange;
    MinAmp[uc_ChannelNo] = ChannelInfo_Manager::GetCurrRangeForCalculateAccuracy(uc_ChannelNo,Range[uc_ChannelNo]) * 5 / 1000;	//量程的千分之五
    if(Schedule_Manager_task::IRSpecialTestFlg[uc_ChannelNo])
    {
        IR_CtrlType[uc_ChannelNo] = TIMEDIVISION;
        PulseH[uc_ChannelNo] = (float)PERCRATE * fNorminalCapa;//[uc_ChannelNo]; 1903271 zyx//ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_MV_NorminalCapacity);
        PulseL[uc_ChannelNo] = -(float)PERCRATE * fNorminalCapa;//[uc_ChannelNo];  1903271 zyx//ChannelInfo_Manager::Read_SpecifiedMetavariable2(uc_ChannelNo,MetaCode_MV_NorminalCapacity);
    }
    else
    {
        if(p_Value->m_fControlValue[0] >= MinAmp[uc_ChannelNo])
            MinAmp[uc_ChannelNo] = p_Value->m_fControlValue[0];

        PulseH[uc_ChannelNo] = p_Value->m_fControlValue[2] + MinAmp[uc_ChannelNo];
        PulseL[uc_ChannelNo] = p_Value->m_fControlValue[2] - MinAmp[uc_ChannelNo];
        Offset[uc_ChannelNo] = p_Value->m_fControlValue[2];

    }

//     if(Step_Manager_task::Unipolar.BoardFlag)   // 单向功率板处理 20161123
//     {
//         if(p_Value->m_fControlValue[2] >= 0.0) // 判断偏移作为正方向
//         {
//             if(PulseH[uc_ChannelNo] < 0.0)
//                 PulseH[uc_ChannelNo] = 0.0;

//             if(PulseL[uc_ChannelNo] < 0.0)
//                 PulseL[uc_ChannelNo] = 0.0;
//         }
//         else
//         {   // 判断偏移作为负方向
//             if(PulseH[uc_ChannelNo] > 0.0)
//                 PulseH[uc_ChannelNo] = 0.0;
//
//             if(PulseL[uc_ChannelNo] > 0.0)
//                 PulseL[uc_ChannelNo] = 0.0;
//         }
//     }

    float fValue2 = ChannelInfo_Manager::CurrentCheck(uc_ChannelNo, PulseH[uc_ChannelNo]);	//170719 zyx
// 	fValue2 =ChannelInfo_Manager::ClampPower(uc_ChannelNo,fValue2);                 //160518
    fValue2 = ChannelInfo_Manager::ClampIRIH(uc_ChannelNo, Range[uc_ChannelNo], fValue2);
    IR_IHighDacCode[uc_ChannelNo] = ChannelInfo_Manager::CalibrateCurrentDAC(uc_ChannelNo, Range[uc_ChannelNo], fValue2);

    fValue2 = ChannelInfo_Manager::CurrentCheck(uc_ChannelNo, PulseL[uc_ChannelNo]);	//170719 zyx
//	fValue2 =ChannelInfo_Manager::ClampPower(uc_ChannelNo,fValue2);                 //160518
    fValue2 = ChannelInfo_Manager::ClampIRIL(uc_ChannelNo, Range[uc_ChannelNo], fValue2);
    IR_ILowDacCode[uc_ChannelNo] = ChannelInfo_Manager::CalibrateCurrentDAC(uc_ChannelNo, Range[uc_ChannelNo], fValue2);
    WaitTime = IR_INTERVAL100us;

    Sample_Pro::NewIRSampled[uc_ChannelNo] = 0;
//	WaitRecord=0;
}

/*****************************************************
// Function Name: ProcessIR
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                      IR分时处理
// Others:
//
// History：
//
// ******************************************************/
void IR::ProcessIR(unsigned char uc_ChannelNo)
{
    //		St_ScheduleMessage ScheduleMessage;

    if(IR_CtrlType[uc_ChannelNo] == TIMEDIVISION) //分时处理
    {
        switch(IR_Status[uc_ChannelNo])
        {
        case IRSTAGEHIGH:
            IR_StageIO[uc_ChannelNo] = 1;
            IR_TimeInterval[uc_ChannelNo] = CommonFunc::GetTimeTicks(&IR_StartTime[uc_ChannelNo]);//OS_ServeProcess::TimeDiff(IR_StartTime[uc_ChannelNo]);  //算出时间差
            if(IR_TimeInterval[uc_ChannelNo] < IR_Stage[uc_ChannelNo])
            {
                IR::DAC_Flag = 1;
                board_config::DAC_out(uc_ChannelNo, IR_IHighDacCode[uc_ChannelNo], ACTIVE_DAC_CUR);
                IR::DAC_Flag = 0;
                if((IR_TimeInterval[uc_ChannelNo] > IR_SampleInterval[uc_ChannelNo]) && (IR_SampleFlagH[uc_ChannelNo] == false))
                {
                    Sample_Pro::NewIRSampled[uc_ChannelNo] = 0;
                    IR_Status[uc_ChannelNo] = SAMPLEHIGH;
                }
            }
            else
            {
                if(IR_PulseRecord[uc_ChannelNo] >= (MAXPULSE - 1))
                {
                    IR_Status[uc_ChannelNo] = IRCALCULATE;
                    IR::DAC_Flag = 1;
//                     UWord32 Dac_Value = ChannelInfo_Manager::CalibrateCurrentDAC(uc_ChannelNo,Step_Manager_task::m_Range[uc_ChannelNo],0);    //计算码字  ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucCurrentRange
//                     board_config::DAC_out(uc_ChannelNo,Dac_Value,ACTIVE_DAC_CUR);   //运行完毕脉冲  输出0
                    board_config::DAC_out(uc_ChannelNo, DAC_OUT_PUT_ZERO, ACTIVE_DAC_CUR); //运行完毕脉冲  输出0
                    IR::DAC_Flag = 0;
                }
                else
                    IR_Status[uc_ChannelNo] = CHECKIFIO;

            }
            break;
        case IRSTAGELOW:
            IR_StageIO[uc_ChannelNo] = 0;
            IR_TimeInterval[uc_ChannelNo] = CommonFunc::GetTimeTicks(&IR_StartTime[uc_ChannelNo]);//OS_ServeProcess::TimeDiff(IR_StartTime[uc_ChannelNo]);  //算出时间差
            if(IR_TimeInterval[uc_ChannelNo] < IR_Stage[uc_ChannelNo])
            {
                IR::DAC_Flag = 1;
                board_config::DAC_out(uc_ChannelNo, IR_ILowDacCode[uc_ChannelNo], ACTIVE_DAC_CUR);
                IR::DAC_Flag = 0;
                if((IR_TimeInterval[uc_ChannelNo] > IR_SampleInterval[uc_ChannelNo]) && (IR_SampleFlagL[uc_ChannelNo] == false))
                {
                    Sample_Pro::NewIRSampled[uc_ChannelNo] = 0;
                    IR_Status[uc_ChannelNo] = SAMPLELOW;
                }
            }
            else
            {
                IR_Status[uc_ChannelNo] = CHECKIFIO;

            }
            break;
        case SAMPLEHIGH:
            if(!Sample_Pro::NewIRSampled[uc_ChannelNo])
                break;

            IR_VHigh[uc_ChannelNo][IR_PulseRecord[uc_ChannelNo]] = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Voltage].m_fValue;
            IR_IHigh[uc_ChannelNo][IR_PulseRecord[uc_ChannelNo]] = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue;
            IR_SampleFlagH[uc_ChannelNo] = true;
            IR_Status[uc_ChannelNo] = IRSTAGEHIGH;
            break;
        case SAMPLELOW:
            if(!Sample_Pro::NewIRSampled[uc_ChannelNo])
                break;

            IR_VLow[uc_ChannelNo][IR_PulseRecord[uc_ChannelNo]] = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Voltage].m_fValue;
            IR_ILow[uc_ChannelNo][IR_PulseRecord[uc_ChannelNo]] = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue;
            IR_SampleFlagL[uc_ChannelNo] = true;
            IR_Status[uc_ChannelNo] = IRSTAGELOW;
            break;
        case CHECKIFIO:
//            IR_StartTime[uc_ChannelNo] = OS_ServeProcess::OS_Time;
            CommonFunc::GetTimeTicksUpdate(&IR_StartTime[uc_ChannelNo]);


            if(!IR_StageIO[uc_ChannelNo])
            {
                IR_Status[uc_ChannelNo] = IRSTAGEHIGH;

            }
            else
            {
                IR_Status[uc_ChannelNo] = IRSTAGELOW;
                IR_PulseRecord[uc_ChannelNo]++;
                if(!Schedule_Manager_task::IRSpecialTestFlg[uc_ChannelNo])
                    if(IR::IR_AmpCheck(uc_ChannelNo))
                    {
                        IR_PulseRecord[uc_ChannelNo] = 0;
                        IR::IR_AmpAdjust(uc_ChannelNo);
                    }
                IR_SampleFlagH[uc_ChannelNo] = false;
                IR_SampleFlagL[uc_ChannelNo] = false;
            }

            break;
        case IRCALCULATE:
            if(Schedule_Manager_task::IRSpecialTestFlg[uc_ChannelNo])
            {
                ChannelInfo_Manager::m_ChannelIRJudge[uc_ChannelNo] = IR_SpecialCalculate(uc_ChannelNo);
                Schedule_Manager_task::IRSpecialTestFlg[uc_ChannelNo] = 0;
            }
            else
                IR_Calculate(uc_ChannelNo);
            //IRChannelFlg&=~(1<<IRChannelNo); //去除标记位

            //St_StepBuf * MyStep = &(ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx]);
            //MyStep->m_StepInfo.m_Data.m_Limits[0].m_bChecked = 1;
            //MyStep->m_StepInfo.m_Data.m_Limits[0].m_bReached = 1;
// 			ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_limitInfo[0].m_bChecked = 1;
// 			ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_limitInfo[0].m_bReached = 1;
            for(int limitidx = 0; limitidx < IV_MAX_LIMIT_NUMBER; limitidx++)
            {
                ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_limitInfo[limitidx].m_bChecked = 1;
                ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_limitInfo[limitidx].m_bReached = 1;
            }
            Step_Manager_task::bStepLimitChecked[uc_ChannelNo] = 1;
            break;
        }
    }
    if(IR_CtrlType[uc_ChannelNo] == TIMERINT) 			//定时器过程  add by DKQ
    {
        if(CheckIRChannel == MAXCHANNELNO) //检查归0
            CheckIRChannel = 0;

        if(IR_TimerEn == false)
        {
            for(unsigned char uc_i = CheckIRChannel; uc_i < MAXCHANNELNO; uc_i++) //查找下一个IR测试通道
            {

                if((IRChannelFlg >> uc_i) & 0x0001)
                {

                    IRChannelNo = uc_i;
                    CheckIRChannel = uc_i + 1;

                    if(uc_ChannelNo != IRChannelNo) //本通道不是指定通道
                    {
                        if((IRChannelFlg >> IRChannelNo) & 0x0001) //若此时目标通道没有被异常干掉
                            CheckIRChannel--;  //回滚继续找，否则从下一个通道开始找
                        return;
                    }

                    break;
                }
                CheckIRChannel = uc_i + 1;
            }

            if((IRChannelFlg >> IRChannelNo) == 0x0001) //最后一个检查通道扫描完毕
                CheckIRChannel = 0; //重新来
        }
        else
            return;

        if(IR_PulseRecord[IRChannelNo] > (MAXPULSE - 1)) //完成
        {
            IR_TimerEn = false;
            IR_Calculate(IRChannelNo);
            IRChannelFlg &= ~(1 << IRChannelNo); //去除标记位
            /*St_StepBuf * MyStep = &(ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_Info[ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_ucActiveIdx]);
            MyStep->m_StepInfo.m_Data.m_Limits[0].m_bChecked = 1;
            MyStep->m_StepInfo.m_Data.m_Limits[0].m_bReached = 1;*/
            for(int limitidx = 0; limitidx < IV_MAX_LIMIT_NUMBER; limitidx++)
            {
                ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_limitInfo[limitidx].m_bChecked = 1;
                ChannelInfo_Manager::ChannelStep[uc_ChannelNo].m_limitInfo[limitidx].m_bReached = 1;
            }
            Step_Manager_task::bStepLimitChecked[uc_ChannelNo] = 1;
            return;
        }


        if(IR_TimerEn == false)
        {
            //IBT_SampleManage::IR_ADC((char)IRChannelNo);// 触发ADC独立采样
            //board_config::DAC_out(IRChannelNo,DAC_OUT_PUT_ZERO,ACTIVE_DAC_CUR);	 //dirui 0806


// 			if(IR_TriggerFlag)
// 			{
            IR::ADG_Ready = 0;
            IRTimerStatus = OUTPUT_DAC_1st;
            IR::pIR_ADC((char)IRChannelNo);// 触发ADC独立采样
            TIMERSFunc::Timer1Handler = &IR::ProcessIRTimer1;
            TIMERSFunc::Init_Timer(IR_Output_DAC_1st[IRChannelNo], 1); //初始化定时器
// 				CurrentIRChannel=IRChannelNo;

// 			}
            TIMERSFunc::Enable_Timer(1);
            IR_TimerEn = true;
            // else, do not change anything, not to enable timer

        }

    }
}

/*****************************************************
// Function Name: ProcessIRTimer1
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                 IR定时器中断处理
// Others:
//
// History：
//
// ******************************************************/
void IR::ProcessIRTimer1(void)
{
// 	if(IR_TriggerFlag)
// 	{
    switch(IRTimerStatus)
    {
    case OUTPUT_DAC_1st:
        // IR::pIR_ADC((char)IRChannelNo);// 触发ADC独立采样
        if(IR_CheckFail())
        {
            LPC_TIM1->MR0 = WaitTime;

            break;
        }
        IR::DAC_Flag = 1;
        board_config::DAC_out(IRChannelNo, IR_ILowDacCode[IRChannelNo], ACTIVE_DAC_CUR);
        IR::DAC_Flag = 0;
        IRTimerStatus = TRIGGER_ADC_1st,

        LPC_TIM1->MR0 = IR_Trigger_ADC_1st[IRChannelNo]; // wait for this time period then trigger ADC 1st time
        break;

    case TRIGGER_ADC_1st:
        if(pIR_Trigger_ADC != 0)
        {
            IR::pIR_Trigger_ADC();
        }
        IRTimerStatus = OUTPUT_DAC_2nd;
        LPC_TIM1->MR0 = IR_Output_DAC_2nd[IRChannelNo];
        break;

    case OUTPUT_DAC_2nd:
        IR::pIR_Read_ADC(IRChannelNo,
                         &SampCode[IRChannelNo].LoStageV[IR_PulseRecord[IRChannelNo]],
                         &SampCode[IRChannelNo].LoStageI[IR_PulseRecord[IRChannelNo]]);

        if(IR_CheckFail())
        {
            LPC_TIM1->MR0 = WaitTime;
            break;
        }

        IR::DAC_Flag = 1;
        board_config::DAC_out(IRChannelNo, IR_IHighDacCode[IRChannelNo], ACTIVE_DAC_CUR);
        IR::DAC_Flag = 0;
        //		WaitRecord=0;
        IRTimerStatus = TRIGGER_ADC_2nd;
        //  IR::pIR_ADC((char)IRChannelNo);// 触发ADC独立采样
        LPC_TIM1->MR0 = IR_Trigger_ADC_2nd[IRChannelNo];
        break;

    case TRIGGER_ADC_2nd:
        if(pIR_Trigger_ADC != 0)
        {
            IR::pIR_Trigger_ADC();
        }
        //IRTimerStatus = SAMPLE_DATA_2nd;
        IRTimerStatus = OUTPUT_CURRENT_0;
        LPC_TIM1->MR0 = IR_Output_Current_0[IRChannelNo];
        break;


        //	case SAMPLE_DATA_1st:
        //		IR::pIR_Read_ADC(IRChannelNo,
        //			&SampCode[IRChannelNo].LoStageV[IR_PulseRecord[IRChannelNo]],
        //			&SampCode[IRChannelNo].LoStageI[IR_PulseRecord[IRChannelNo]]);
        //	IR::pRenew_IR_ADC();  //退出独立采样
        /*
          IRTimerStatus=OUTPUT_DAC_2nd;
          LPC_TIM1->MR0 =IR_Output_DAC_2nd[IRChannelNo];
          break;
        */
    case OUTPUT_CURRENT_0:
        IR::pIR_Read_ADC(IRChannelNo,
                         &SampCode[IRChannelNo].HiStageV[IR_PulseRecord[IRChannelNo]],
                         &SampCode[IRChannelNo].HiStageI[IR_PulseRecord[IRChannelNo]]);
        IR::pRenew_IR_ADC();  //退出独立采样

        //		LPC_TIM1->MR0 = IR_Sample_Data_2nd[IRChannelNo];
        //		IRTimerStatus = SAMPLE_DATA_2nd;
        //
        if(DAC_Flag == 1)
        {
            LPC_TIM1->MR0 = WaitTime;
            break;
        }
        IR::DAC_Flag = 1;
        UWord32 Dac_Value = ChannelInfo_Manager::CalibrateCurrentDAC(IRChannelNo, Step_Manager_task::m_Range[IRChannelNo], 0);  //计算码字  ChannelInfo_Manager::ChannelInfo[IRChannelNo].m_ucCurrentRange
        board_config::DAC_out(IRChannelNo, Dac_Value, ACTIVE_DAC_CUR); // 运行完毕脉冲  输出0
        IR::DAC_Flag = 0;
        TIMERSFunc::Disable_Timer(1);
        IR_TimerEn = false;
        IR_PulseRecord[IRChannelNo]++;

        if(IR::IR_AmpCheck(IRChannelNo))
        {
            IR_PulseRecord[IRChannelNo] = 0;
            IR::IR_AmpAdjust(IRChannelNo);
        }
        break;
        //	case  SAMPLE_DATA_2nd:
        //
        //		IR::pIR_Read_ADC(IRChannelNo,
        //			&SampCode[IRChannelNo].HiStageV[IR_PulseRecord[IRChannelNo]],
        //			&SampCode[IRChannelNo].HiStageI[IR_PulseRecord[IRChannelNo]]);
        //		IR::pRenew_IR_ADC();  //退出独立采样
        //	    TIMERSFunc::Disable_Timer(1);
        //		IR_TimerEn=false;
        //		IR_PulseRecord[IRChannelNo]++;
        //
        //		break;
    default:
        break;
    }
// 	}

// Since IR_TriggerFlag was initilized as 1, the following code should never been excuted  DKQ 12_18_2017
    /*
    	else
    	{
    	switch(IRTimerStatus)
    	{
    	case INTERVALT1:

    		if(DAC_Flag==1)
    		{
    			LPC_TIM1->MR0 =WaitTime;

    			break;
    		}
    		IR::DAC_Flag=1;
    		board_config::DAC_out(IRChannelNo,IR_ILowDacCode[IRChannelNo],ACTIVE_DAC_CUR);
    		IR::DAC_Flag=0;
    		IRTimerStatus=INTERVALT2;
    		IR::pIR_ADC((char)IRChannelNo);// 触发ADC独立采样
    		LPC_TIM1->MR0 =IR_T2[IRChannelNo];
    		break;
    	case INTERVALT2:
    		IR::pIR_Read_ADC(IRChannelNo,
    			&SampCode[IRChannelNo].LoStageV[IR_PulseRecord[IRChannelNo]],
    			&SampCode[IRChannelNo].LoStageI[IR_PulseRecord[IRChannelNo]]);
    		IR::pRenew_IR_ADC();  //退出独立采样
    		IRTimerStatus=INTERVALT3;
    		LPC_TIM1->MR0 =IR_T3[IRChannelNo];
    		break;
    	case INTERVALT3:
    		if(DAC_Flag==1)
    		{
    			LPC_TIM1->MR0 =WaitTime;
    			break;
    		}
    		IR::DAC_Flag=1;
    		board_config::DAC_out(IRChannelNo, IR_IHighDacCode[IRChannelNo],ACTIVE_DAC_CUR);
    		IR::DAC_Flag=0;
    //		WaitRecord=0;
    		IRTimerStatus=INTERVALT4;
    		IR::pIR_ADC((char)IRChannelNo);// 触发ADC独立采样
    		LPC_TIM1->MR0 =IR_T4[IRChannelNo];
    		break;
    	case INTERVALT4:
    		LPC_TIM1->MR0 = IR_T5[IRChannelNo];
    		IR::pIR_Read_ADC(IRChannelNo,
    			&SampCode[IRChannelNo].HiStageV[IR_PulseRecord[IRChannelNo]],
    			&SampCode[IRChannelNo].HiStageI[IR_PulseRecord[IRChannelNo]]);
    		IR::pRenew_IR_ADC();  //退出独立采样
    		IRTimerStatus=INTERVALT5;
    		break;
    	case INTERVALT5:
    		if(DAC_Flag==1)
    		{
    			LPC_TIM1->MR0 =WaitTime;
    			break;
    		}
    		IR::DAC_Flag=1;
    //         UWord32 Dac_Value = ChannelInfo_Manager::CalibrateCurrentDAC(IRChannelNo,Step_Manager_task::m_Range[IRChannelNo],0);    //计算码字  ChannelInfo_Manager::ChannelInfo[IRChannelNo].m_ucCurrentRange
    //         board_config::DAC_out(IRChannelNo,Dac_Value,ACTIVE_DAC_CUR);  // 运行完毕脉冲  输出0
    		 board_config::DAC_out(IRChannelNo,DAC_OUT_PUT_ZERO,ACTIVE_DAC_CUR);   //运行完毕脉冲  输出0
    		IR::DAC_Flag=0;
    		TIMERSFunc::Disable_Timer(1);
    		IR_TimerEn=false;
    		IR_PulseRecord[IRChannelNo]++;

    		if(IR::IR_AmpCheck(IRChannelNo))
    		{
    			IR_PulseRecord[IRChannelNo]=0;
    			IR::IR_AmpAdjust(IRChannelNo);
    		}
    		break;
    	default:
    		break;
    	  }
      }
     */
}

unsigned char IR::IR_CheckFail(void)
{
    unsigned char uc_fail = 0;
    if(DAC_Flag == 1)
        uc_fail = 1;

    if((ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_LBT21024)
            || (ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_LBT21084)
            || (ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_LBT21044)
            || (ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_LBT21014)
            || (ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_LBT21162)
            || (ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_MSTAT21044)
            || (ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_LBT20084)
            || (ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_LBT20162)
            || (ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_LBT20083))
    {
        if(ADG_Ready == 0)
            uc_fail = 1;
    }
    return uc_fail;
}
unsigned char IR::IR_AmpCheck(unsigned char uc_ChannelNo)
{
    unsigned char CurrRange =  ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucCurrentRange;
    unsigned char VoltRange =  ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucVoltageRange;
    unsigned char	beNeedD = 0;
    float delI = ChannelInfo_Manager::GetCurrRangeForCalculateAccuracy(uc_ChannelNo,Range[uc_ChannelNo]);

    if(IR_CtrlType[uc_ChannelNo] == TIMERINT)
    {
        IR_IHigh[uc_ChannelNo][0] = ChannelInfo_Manager::CalibrateCurrentADC(uc_ChannelNo,
                                    CurrRange,
                                    SampCode[uc_ChannelNo].HiStageI[0]);
        IR_ILow[uc_ChannelNo][0] = ChannelInfo_Manager::CalibrateCurrentADC(uc_ChannelNo,
                                   CurrRange,
                                   SampCode[uc_ChannelNo].LoStageI[0]);

        IR_VHigh[uc_ChannelNo][0] = ChannelInfo_Manager::CalibrateVoltageADC(uc_ChannelNo,
                                    VoltRange,
                                    SampCode[uc_ChannelNo].HiStageV[0]);
        IR_VLow[uc_ChannelNo][0] = ChannelInfo_Manager::CalibrateVoltageADC(uc_ChannelNo,
                                   VoltRange,
                                   SampCode[uc_ChannelNo].LoStageV[0]);
    }
    if((IR_IHigh[uc_ChannelNo][0] >= delI * (-0.002f) && IR_IHigh[uc_ChannelNo][0] <= delI * (0.002f)) && (IR_ILow[uc_ChannelNo][0] >= delI * (-0.002f) && IR_ILow[uc_ChannelNo][0] <= delI * (0.002f)))
    {
        ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_InternalResistance].m_fValue = 0xFF; //1/IR_IHigh[uc_ChannelNo][0];
        safety_check::SetUnsafe(uc_ChannelNo, AMP_IR_UNSAFE, 0, IV_UNSAFE_TYPE); //Unsafe
//		safety_check::SetUnsafe(uc_ChannelNo, CURRENT_SYS_UNSAFE, 0);//Unsafe
        safety_check::sendError(uc_ChannelNo);		// trigger a sending
//		 safety_check::ErrorFlag[uc_ChannelNo] ^= CURRENT_SYS_UNSAFE;
// 		safety_check::ClrUnsafe(uc_ChannelNo, AMP_IR_UNSAFE, IV_UNSAFE_TYPE);
    }
    if((IR_VHigh[uc_ChannelNo][0] - IR_VLow[uc_ChannelNo][0]) <
            ((ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_VRange[VoltRange].m_Bound.m_High - ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_VRange[VoltRange].m_Bound.m_Low) * 0.0002f))
    {
        DAmp[uc_ChannelNo] = 2.0f;
        if(((IR_VHigh[uc_ChannelNo][0] - IR_VLow[uc_ChannelNo][0]) <
                (ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_VRange[VoltRange].m_Bound.m_High - ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_VRange[VoltRange].m_Bound.m_Low) * 0.0001f))
            DAmp[uc_ChannelNo] = (0.001f * (ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_VRange[VoltRange].m_Bound.m_High
                                            - ChannelInfo_Manager::BoardCFG.m_Channel[uc_ChannelNo].m_VRange[VoltRange].m_Bound.m_Low)) / (IR_VHigh[uc_ChannelNo][0] - IR_VLow[uc_ChannelNo][0]);
//		DAmp[uc_ChannelNo]=10.0f;
        beNeedD = 1;

    }
    return beNeedD;
}
void IR::IR_AmpAdjust(unsigned char uc_ChannelNo)
{
    if(ChannelInfo_Manager::GetMaxCurrRangeValue(uc_ChannelNo,Range[uc_ChannelNo]) <= 1.0f)
        MinAmp[uc_ChannelNo] += 0.05f * ChannelInfo_Manager::GetCurrRangeForCalculateAccuracy(uc_ChannelNo,Range[uc_ChannelNo]);
    else
        MinAmp[uc_ChannelNo] *= DAmp[uc_ChannelNo];
    if(!ChannelInfo_Manager::Clamp2AMP(uc_ChannelNo, Range[uc_ChannelNo], MinAmp[uc_ChannelNo]))
    {
        PulseH[uc_ChannelNo] = Offset[uc_ChannelNo] + MinAmp[uc_ChannelNo];
        PulseL[uc_ChannelNo] = Offset[uc_ChannelNo] - MinAmp[uc_ChannelNo];

        float fValue2 = ChannelInfo_Manager::ClampIRIH(uc_ChannelNo, Range[uc_ChannelNo], PulseH[uc_ChannelNo]);
        fValue2 = ChannelInfo_Manager::CurrentCheck(uc_ChannelNo, fValue2);	//170719 zyx
//		fValue2 =ChannelInfo_Manager::ClampPower(uc_ChannelNo,fValue2);
        IR_IHighDacCode[uc_ChannelNo] = ChannelInfo_Manager::CalibrateCurrentDAC(uc_ChannelNo, Range[uc_ChannelNo], fValue2);
        fValue2 = ChannelInfo_Manager::ClampIRIL(uc_ChannelNo, Range[uc_ChannelNo], PulseL[uc_ChannelNo]);
        fValue2 = ChannelInfo_Manager::CurrentCheck(uc_ChannelNo, fValue2);	//170719 zyx
//		fValue2 =ChannelInfo_Manager::ClampPower(uc_ChannelNo,fValue2);
        IR_ILowDacCode[uc_ChannelNo] = ChannelInfo_Manager::CalibrateCurrentDAC(uc_ChannelNo, Range[uc_ChannelNo], fValue2);
    }
}
/*****************************************************
// Function Name: IR_Calculate
// Version:        V1.0
// Date:                2012/11/12
// Description:
//                 IR结果计算
// Others:
//
// History：
//
// ******************************************************/
void IR::IR_Calculate(unsigned char uc_ChannelNo)
{
    float Value[MAXPULSE];
    float tmp = 0.0f;

    if(IR_CtrlType[uc_ChannelNo] != TIMEDIVISION)
    {
        unsigned char CurrRange =  ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucCurrentRange;
        unsigned char VoltRange =  ChannelInfo_Manager::ChannelInfo[uc_ChannelNo].m_ucVoltageRange;

        for(unsigned char StageIdx = 0; StageIdx < MAXPULSE; StageIdx++)
        {
            IR_IHigh[uc_ChannelNo][StageIdx] = ChannelInfo_Manager::CalibrateCurrentADC(uc_ChannelNo,
                                               CurrRange,
                                               SampCode[uc_ChannelNo].HiStageI[StageIdx]);
            IR_ILow[uc_ChannelNo][StageIdx] = ChannelInfo_Manager::CalibrateCurrentADC(uc_ChannelNo,
                                              CurrRange,
                                              SampCode[uc_ChannelNo].LoStageI[StageIdx]);

            IR_VHigh[uc_ChannelNo][StageIdx] = ChannelInfo_Manager::CalibrateVoltageADC(uc_ChannelNo,
                                               VoltRange,
                                               SampCode[uc_ChannelNo].HiStageV[StageIdx]);
            IR_VLow[uc_ChannelNo][StageIdx] = ChannelInfo_Manager::CalibrateVoltageADC(uc_ChannelNo,
                                              VoltRange,
                                              SampCode[uc_ChannelNo].LoStageV[StageIdx]);
        }

//         ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue = (IR_IHigh[uc_ChannelNo][MAXPULSE - 1] + IR_ILow[uc_ChannelNo][MAXPULSE - 1]) / 2; //average current;
//         ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Voltage].m_fValue = IR_VLow[uc_ChannelNo][MAXPULSE - 1]; //last voltage
    }
	
	ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Current].m_fValue = (IR_IHigh[uc_ChannelNo][MAXPULSE - 1] + IR_ILow[uc_ChannelNo][MAXPULSE - 1]) / 2; //average current;
	ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_Voltage].m_fValue = IR_VLow[uc_ChannelNo][MAXPULSE - 1]; //last voltage
	
    for(unsigned char uc_i = 0; uc_i < MAXPULSE; uc_i++)
    {
        if((IR_IHigh[uc_ChannelNo][uc_i] - IR_ILow[uc_ChannelNo][uc_i]) == 0)
            //continue;
            Value[uc_i] = 0.0f;
        else
            Value[uc_i] = (IR_VHigh[uc_ChannelNo][uc_i] - IR_VLow[uc_ChannelNo][uc_i]) / (IR_IHigh[uc_ChannelNo][uc_i] - IR_ILow[uc_ChannelNo][uc_i]);
    }
    for(unsigned char uc_i = 1; uc_i < MAXPULSE; uc_i++) //Max
    {
        if(Value[uc_i] < Value[uc_i - 1])
        {
            tmp = Value[uc_i - 1];
            Value[uc_i - 1] = Value[uc_i];
            Value[uc_i] = tmp;
        }
    }

    for(unsigned char uc_i = 1; uc_i < MAXPULSE - 1; uc_i++) //Min
    {
        if(Value[uc_i - 1] < Value[uc_i])
        {
            tmp = Value[uc_i];
            Value[uc_i] = Value[uc_i - 1];
            Value[uc_i - 1] = tmp;
        }
    }
    for(unsigned char uc_i = 0; uc_i < (MAXPULSE - 2); uc_i++)
        IR_Value[uc_ChannelNo] += Value[uc_i];

    //ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_InternalResistance].m_fValue = IR_Value[uc_ChannelNo]/(MAXPULSE-2);
    //ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_IR_by_IRStep = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_InternalResistance].m_fValue;
    //ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_IR_from_IRStep =  ((ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_IR_by_IRStep > 0 )? true:false);
    UWord16 uw16_Type = Step_Manager_task::GetStepRunInfo[uc_ChannelNo].GetStepInfo.m_Data.m_Ctrl.m_u16ControlType;

    if(uw16_Type == CT_IVCH_IR)
    {
        ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_InternalResistance].m_fValue = IR_Value[uc_ChannelNo] / (MAXPULSE - 2);
        ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_by_IRStep = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_InternalResistance].m_fValue;
        ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_IRStep =  ((ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_by_IRStep > 0 ) ? true : false);
    }
    else if(uw16_Type == CT_IVCH_ACIR)
    {
        ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_ACR].m_fValue = IR_Value[uc_ChannelNo] / (MAXPULSE - 2);
        ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_by_IRStep = ChannelInfo_Manager::MetaVariable2[uc_ChannelNo][MetaCode_PV_ACR].m_fValue;
        ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].IR_From.m_bIR_from_IRStep =  ((ChannelInfo_Manager::m_IR_Info[uc_ChannelNo].m_fIR_by_IRStep > 0 ) ? true : false);
    }
}
/*****************************************************
// Function Name: IR_SepcialCalculate
// Version:        V1.0
// Date:                2014/06/09
// Description:
//                 特殊IR结果计算
// Others:
//
// History：
//
// ******************************************************/
unsigned char IR::IR_SpecialCalculate(unsigned char uc_ChannelNo)
{
    float Sum_I = 0.0f;
    float Sum_V = 0.0f;
    float Sum_Vtmp = 0.0f;
    float Sum_Itmp = 0.0f;
    float Value[MAXPULSE];
    unsigned char bWarnning = 0;

    float fI_max = -99999.0f, fI_min = 99999.0f, fV_max = -99999.0f, fV_min = 99999.0f;
    float fR_max = -99999.0f, fR_min = 99999.0f, fR_sum = 0.0f;

    for(unsigned char i = 0; i < MAXPULSE; i++)
    {
        if(IR_IHigh[uc_ChannelNo][i] > fI_max)
            fI_max = IR_IHigh[uc_ChannelNo][i];
        if(IR_IHigh[uc_ChannelNo][i] < fI_min)
            fI_min = IR_IHigh[uc_ChannelNo][i];

        if(IR_VHigh[uc_ChannelNo][i] > fV_max)
            fV_max = IR_VHigh[uc_ChannelNo][i];
        if(IR_IHigh[uc_ChannelNo][i] < fV_min)
            fV_min = IR_VHigh[uc_ChannelNo][i];

        Sum_Itmp += IR_IHigh[uc_ChannelNo][i];
        Sum_Vtmp += IR_VHigh[uc_ChannelNo][i];

        if((IR_IHigh[uc_ChannelNo][i] - IR_ILow[uc_ChannelNo][i]) == 0)
            Value[i] = 0.0f;
        else
            Value[i] = (IR_VHigh[uc_ChannelNo][i] - IR_VLow[uc_ChannelNo][i])
                       / (IR_IHigh[uc_ChannelNo][i] - IR_ILow[uc_ChannelNo][i]);

        fR_sum += Value[i];
        if(Value[i] > fR_max)
            fR_max = Value[i];
        if(Value[i] < fR_min)
            fR_min = Value[i];
    }
    Sum_Itmp -= (fI_max + fI_min);
    Sum_Vtmp -= (fV_max + fV_min);
    fR_sum   -= (fR_max + fR_min);

    float AV_I = Sum_Itmp / 8;
    float AV_V = Sum_Vtmp / 8;

    if((AV_I < 0) || (AV_V < 0)) //正电流时采到负平均值
        bWarnning = 1;
    if(AV_I < PulseH[uc_ChannelNo] * 0.5f) //正电流不到设定值的一半，有出现电压不变，电流为极小值的情况
        bWarnning = 1;

    float fValue = fR_sum / (MAXPULSE - 2);
    fValue = abs(fValue);

    bWarnning |= ((ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AbnormalCheck.IIR_MaxIR * 0.001) < fValue); //180808 zyx
    bWarnning |= ((ChannelInfo_Manager::ChannelSchedule[uc_ChannelNo].m_Settings.m_AbnormalCheck.IIR_MinIR * 0.001) > fValue); //180808 zyx

    for(unsigned char uc_i = 0; uc_i < MAXPULSE; uc_i++)
    {
        if((IR_IHigh[uc_ChannelNo][uc_i] != fI_max) && (IR_IHigh[uc_ChannelNo][uc_i] != fI_min))
            Sum_I += (IR_IHigh[uc_ChannelNo][uc_i] - AV_I) * (IR_IHigh[uc_ChannelNo][uc_i] - AV_I); //pow((IR_IHigh[uc_ChannelNo][uc_i]-AV_I),2);
        if((IR_VHigh[uc_ChannelNo][uc_i] != fV_max) && (IR_VHigh[uc_ChannelNo][uc_i] != fV_min))
            Sum_V += (IR_VHigh[uc_ChannelNo][uc_i] - AV_V) * (IR_VHigh[uc_ChannelNo][uc_i] - AV_V); // pow((IR_VHigh[uc_ChannelNo][uc_i]-AV_V),2);
    }

    Sum_I = Sum_I / 7;
    Sum_V = Sum_V / 7;

    if((Sum_I > 1) || (Sum_V > 1)) //若能出现如此大的波动的话，证明确实通道挂了
        bWarnning |= 1;

//     Un_ERROR_MESSAGE * st_SPB;
//     st_SPB = (Un_ERROR_MESSAGE *) &EthernetParse::m_Point_Temp;
//     st_SPB->m_IV_ERROR.m_btChannel_ID = uc_ChannelNo;
//     st_SPB->m_IV_ERROR.m_btCode_Type = IV_ERROR_TYPE_WARNNING;
//     st_SPB->m_IV_ERROR.m_btType_ErrorNum = IV_ERROR_WARNNING_IR ;
//     st_SPB->m_IV_ERROR.m_fIR_Value = fValue; //hpq/20150814

//     if(bWarnning)
//         st_SPB->m_IV_ERROR.m_btSubCode = eIRCHECK_ERROR_Unsafe;
//     else
//         st_SPB->m_IV_ERROR.m_btSubCode = eIRCHECK_ERROR_Safe;
//     EthernetParse::MPReportData(uc_ChannelNo, SEND_ERROR_MESSAGE);

    return bWarnning;
}

/*****************BoardConfig Class******************************************
** End:           BoardConfig Class
******************************************************************************/


