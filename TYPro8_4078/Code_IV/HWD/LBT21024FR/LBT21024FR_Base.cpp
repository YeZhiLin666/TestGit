
#include "../../Entry/includes.h"

#define  _LBT21024FR_VERSION_A_         0         //this is a temperory MACRO,and valish somedays later

#define  LBT21024FR_FINE_TUNE_STARTTIME          0.1f
#define  LBT21024FR_FINE_TUNE_MINERRORFACTOR     1E-4 //Modified by hardware Engieer 
#define  LBT21024FR_FINE_TUNE_STEPFACTOR         5E-5
#define  LBT21024FR_FINE_TUNE_MAXADJUSTFACTOR    0.02f  // Modified by hardware Engieer 20140617 ΢��

#define  LBT21024FR_SETTING_TIME                 500       //����������ʱ�䣬��λΪ100us

#if HARDWARE_SDRAM_32M == 1
    #pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x01F40000
#else
    #pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x03E80000
#endif  
St_DisposableTask LBT21024FR_Base::SetCtrlOpenCh[5];
St_DisposableTask LBT21024FR_Base::SetCtrlCloseCh[3];
St_DisposableTask LBT21024FR_Base::SetCCToCC[4];    //�����˶�range�л��������������䵽10��
St_DisposableTask LBT21024FR_Base::SetRstToOff[2];
St_DisposableTask LBT21024FR_Base::SetOffToRst[2];

SSPI_Para  LBT21024FR_Base::LBT21024FR_SSP0DAC;
SSPI_Para  LBT21024FR_Base::LBT21024FR_SSP1ADC;
#pragma arm section 

unsigned char LBT21024FR_Base::SetCtrlOpenCh_Len = 5;
unsigned char LBT21024FR_Base::SetCtrlCloseCh_Len = 3;
unsigned char LBT21024FR_Base::SetCCToCC_Len = 4;
unsigned char LBT21024FR_Base::SetRstToOff_Len = 2;
unsigned char LBT21024FR_Base::SetOffToRst_Len = 2;

const St_Port LBT21024FR_Base::PinGroup[LBT21024FR_Total_Pins] =	//??,??????????????enum?????????
{//Port      Bits       PortType           Function               Address    
                  
 {1,        28,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
 {2,        12,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
 {2,        13,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
 {1,        21,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
 {0,        6,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
 {1,        26,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
 {1,        0,         eCPTYPE_CPLD,      CPLD_O,                NBUS_WRITE_DCS},//Pin_DacCS
 {2,        8,         eCPTYPE_MCUGPIO,   GPIO_O,                0},//Pin_DacRst
 {1,        3,         eCPTYPE_MCUGPIO,   GPIO_O,                0},//Pin_DacLoad
 {1,        2,         eCPTYPE_MCUGPIO,   GPIO_I,                0},//Pin_DacBusy
 {0,        13,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
// {0,        15,        eCPTYPE_MCUGPIO,   SSP0_SCK,              0},
// {0,        17,        eCPTYPE_MCUGPIO,   SSP0_MISO,             0},
// {0,        18,        eCPTYPE_MCUGPIO,   SSP0_MOSI,             0},
 {0,        7,         eCPTYPE_MCUGPIO,   SSP1_SCK,              0},
 {0,        8,         eCPTYPE_MCUGPIO,   SSP1_MISO,             0},
 {0,        9,         eCPTYPE_MCUGPIO,   SSP1_MOSI,             0},
	{3,        18,        eCPTYPE_MCUGPIO,   PWM0,                  0},
 {1,        2,         eCPTYPE_CPLD,      CPLD_O,                NBUS_WRITE_DCS},  //Pin_AdgRst
 {1,        6,         eCPTYPE_MCUGPIO,   GPIO_I,                0},//????,MAXWLL 
 //#if(FAN_SAFETY_MODE==1)
 {3,       16,         eCPTYPE_MCUGPIO,   GPIO_I,                0},
 //#endif
};


//                                           Port      Bits       PortType           Function               Address            
// const St_Port LBT21024FR_Base::Pin_AdcConver = {1,        28,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
// const St_Port LBT21024FR_Base::Pin_AdcBusy1  = {2,        12,        eCPTYPE_MCUGPIO,   GPIO_I,                0};
// const St_Port LBT21024FR_Base::Pin_AdcBusy2  = {2,        13,        eCPTYPE_MCUGPIO,   GPIO_I,                0};
// const St_Port LBT21024FR_Base::Pin_AdcCS1    = {1,        21,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
// const St_Port LBT21024FR_Base::Pin_AdcCS2    = {0,        6,         eCPTYPE_MCUGPIO,   GPIO_O,                0};
// const St_Port LBT21024FR_Base::Pin_AdcRst    = {1,        26,        eCPTYPE_MCUGPIO,   GPIO_O,                0};


// const St_Port LBT21024FR_Base::Pin_DacCS    = {1,        0,         eCPTYPE_CPLD,      CPLD_O,                NBUS_WRITE_DCS};
// const St_Port LBT21024fr_Base::Pin_DacRst    = {2,        8,        eCPTYPE_MCUGPIO,   GPIO_O,                0}; 
// const St_Port LBT21024FR_Base::Pin_DacLoad  = {1,        3,        eCPTYPE_MCUGPIO,   GPIO_O,                1};
// const St_Port LBT21024FR_Base::Pin_DacBusy   = {1,        2,        eCPTYPE_MCUGPIO,   GPIO_O,                1};
// const St_Port LBT21024FR_Base::Pin_WD        = {0,        13,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
// //clamp
// const St_Port LBT21024FR_Base::Pin_SCK0      = {0,        15,        eCPTYPE_MCUGPIO,   SSP0_SCK,              0};
// const St_Port LBT21024FR_Base::Pin_MISO0     = {0,        17,        eCPTYPE_MCUGPIO,   SSP0_MISO,             0};
// const St_Port LBT21024FR_Base::Pin_MOSI0     = {0,        18,        eCPTYPE_MCUGPIO,   SSP0_MOSI,             0};
// const St_Port LBT21024FR_Base::Pin_SCK1      = {0,        7,         eCPTYPE_MCUGPIO,   SSP1_SCK,              0};
// const St_Port LBT21024FR_Base::Pin_MISO1     = {0,        8,         eCPTYPE_MCUGPIO,   SSP1_MISO,             0};
// const St_Port LBT21024FR_Base::Pin_MOSI1     = {0,        9,         eCPTYPE_MCUGPIO,   SSP1_MOSI,             0};

// const St_Port LBT21024FR_Base::Pin_AdgRst    = {1,        2,         eCPTYPE_CPLD,      CPLD_O,                NBUS_WRITE_DCS};

// const St_Port LBT21024FR_Base::Pin_RedundantSafety= {1,   6,        eCPTYPE_MCUGPIO,   GPIO_I,                0};//�����е磬MAXWLL 
//#if(FAN_SAFETY_MODE==1)
// const St_Port LBT21024FR_Base::Pin_FanCheckSignal={3,    16,        eCPTYPE_MCUGPIO,   GPIO_I,                0};  
MsTime     LBT21024FR_Base::FanOverTime;
//#endif

volatile St_LBT21024FR_Relay LBT21024FR_Base::LBT21024FR_relaystatus[MAXCHANNELNO];  

const unsigned char LBT21024FR_Base::m_bHaveVoltageDac = 0;
const unsigned char LBT21024FR_Base::m_bHaveClampVdac = 1;
const unsigned char LBT21024FR_Base::m_bChannelBasedClampVdac = 0;
void LBT21024FR_Base::Hardware_Init(void)
{  
	//unsigned char temp;
	
	LBT21024FR_FeatureEnable_Init();
	IO_Init();   
	Feed_CpldWD(); 
	
	
	CPLD_FUNC::Init();
	memset((void *)(&LBT21024FR_relaystatus), 0, sizeof(LBT21024FR_relaystatus));
    for(unsigned char ChannelIdx = 0;ChannelIdx<MAXCHANNELNO;ChannelIdx++)
	{
	    LBT21024FR_relaystatus[ChannelIdx].Reg = 0;
        LBT21024FR_relaystatus[ChannelIdx].Bits.R_VM = 1;
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21024FR_relaystatus[ChannelIdx].Reg);
	}
	Feed_CpldWD(); 

	board_config::bHaveRedundantCheck = 1;
	board_config::ClampH_Outputflag = 0;
	board_config::ClampL_Outputflag = 0;
	//SSP0 for DAC    AD5754
 	LBT21024FR_SSP0DAC.uc_Module_Name = SSP0_DEVICE3;    //name
	LBT21024FR_SSP0DAC.uc_Speed = 7;//5;
	LBT21024FR_SSP0DAC.uc_CPOL = 0;
	LBT21024FR_SSP0DAC.uc_CPHA = 1;
	LBT21024FR_SSP0DAC.uc_MSTR = 1;    //master
	LBT21024FR_SSP0DAC.uc_BITS = 8;
	LBT21024FR_SSP0DAC.uc_LSBF = 0;
	
	//SSP1 for ADC
	LBT21024FR_SSP1ADC.uc_Module_Name = SSP1_DEVICE1;    //name
	LBT21024FR_SSP1ADC.uc_Speed = 5;
	LBT21024FR_SSP1ADC.uc_CPOL = 0;
	LBT21024FR_SSP1ADC.uc_CPHA = 0;       //rising trig
	LBT21024FR_SSP1ADC.uc_MSTR = 1;    //master
	LBT21024FR_SSP1ADC.uc_BITS = 8;
	LBT21024FR_SSP1ADC.uc_LSBF = 0;
	
	TIMERSFunc::DELAY_SYS( 30000, 0 );	//delay 5ms
	Feed_CpldWD(); 
	
	ADC_Init();
	DAC_Init();

	CPLD_FUNC::WriteDOBus(0,LBT21024FR_relaystatus[0].Reg);  //Ӳ���ı����ʱ�򣬸ĳɸ���Ч 20140331

	ControlPin::ReInitPullUP();
	LBT21024FR_Base::changeADCRange(CURRENT_RANGE_1);
	Feed_CpldWD(); 
	CPLD_FUNC::Write(NBUS_WRITE_VOLTAGERELAY,0x03);//��ѹ���ƿ�   20140331  //01
// 	LBT21024FR_TempSample::AD_Init(1);//add by qjm20140613
	OS_LogicProcess::InitBoardEngine(	&SetCtrlOpenCh[0],	sizeof(SetCtrlOpenCh)	/	sizeof(St_DisposableTask),
										&SetCtrlCloseCh[0], sizeof(SetCtrlCloseCh)	/	sizeof(St_DisposableTask),
										&SetCCToCC[0], 		sizeof(SetCCToCC)		/	sizeof(St_DisposableTask),
										&SetRstToOff[0],	sizeof(SetRstToOff)		/	sizeof(St_DisposableTask)
		);
	AdditionalEngine();
    LBT21024FR_ADC5DAC::Dac_out_Clamp_Vlt(9,DAC_OUT_PUT_MIN);   // ��10V
		LBT21024FR_ADC5DAC::Dac_out_Clamp_Vlt(11,DAC_OUT_PUT_MIN);
	  LBT21024FR_ADC5DAC::Dac_out_Clamp_Vlt(8,DAC_OUT_PUT_MAX);   //�������
	  LBT21024FR_ADC5DAC::Dac_out_Clamp_Vlt(10,DAC_OUT_PUT_MAX);
	 //#if(FAN_SAFETY_MODE==1)
	///////���ȵ���  ��ʼ��ʱ�趨Ϊ100%
	if(board_config::Feature_Enable_init->Fan_Safety_Mode==1)
	{
			MsTime MyMsTime;
			unsigned long long TimeTicks;
			MyMsTime.Second =LBT21024FR_Base::FanOverTime.Second;
			MyMsTime.Us100 = LBT21024FR_Base::FanOverTime.Us100;
			while(1)
			{
				if(PCA9501::SetExpander(34,1))
						{			
								I2CBusNew::AD5245_SET(0,0,I2C2);  //
								PCA9501::SetExpander(34,0);
								safety_check::Fan_FaultCheckFlag=0; 
								break;
						}
				else
				{
					TimeTicks = CommonFunc::GetTimeTicks(&MyMsTime);
								if(TimeTicks >= 1000)     
								{
										safety_check::Fan_FaultCheckFlag=0;
										break;
								}
								else 
										continue;
						}
			}
  }	
	//#endif
    board_config::bFanAdj=1;
	FanAdj_task::FANADJ_INTERVAL=3000;
	FanAdj_task::VF=10.0;
	FanAdj_task::VR=-2.5;
	FanAdj_task::HeatSinkCounter=4;
	FanAdj_task::AdjMod=PWMMOD;	
 	LPC4078PWM::SetPWM(PWM_HD0,PWM_CH3,100,20);	 
}
void LBT21024FR_Base::LBT21024FR_FeatureEnable_Init(void) 
{
	static St_Hardware_Feature_Enable para;
	memset(&para,0,sizeof(para));

	//if(board_config::IVBoardTypeversion==BOARDTYPE_VER_A)
	{
		para.Mcu_PWMFan_Enable=1;
		para.Redundant_Safety_Mode=1;
		para.Power_Off_Enable=1;
		para.Fan_Safety_Mode=1;
		para.RedundantVoltMode=REDUNDANT_M0;
		para.OS_Tick_Interval=800;
	}
	
	board_config::Feature_Enable_init=&para;
} 
void LBT21024FR_Base::IO_Init(void)
{
		for(int i=0;i<LBT21024FR_Total_Pins;i++)
			ControlPin::SetIObyCP(PinGroup[i]);
} 

void LBT21024FR_Base::ADC_Init(void)
{	
	LBT21024FR_ADC5DAC::Adc_Init();

	TIMERSFunc::DELAY_SYS( 10000, 0 );	//delay 5ms
}
void LBT21024FR_Base::DAC_Init(void)
{
	LBT21024FR_ADC5DAC::Dac_Init();
    board_config::DacInfo.m_bCurrRngCount = 4;
    board_config::DacInfo.m_bVoltRngCount = 1;
    board_config::DacInfo.m_bHaveVoltageDac = m_bHaveVoltageDac;
	board_config::DacInfo.m_bHaveClampVdac = m_bHaveClampVdac;
	board_config::DacInfo.m_bChannelBasedClampVdac = m_bChannelBasedClampVdac;
	board_config::DacInfo.m_uSettingTime = LBT21024FR_SETTING_TIME;
	ARBUS_DELAY(10000);    //300us reset time  AD5360
}

void LBT21024FR_Base::Software_Init(void)
{
	Func_Init();
	EngineStuff(); 

	Step_Manager_task::m_CurFineTuneFactor.m_fStartTime = LBT21024FR_FINE_TUNE_STARTTIME;
	Step_Manager_task::m_CurFineTuneFactor.m_fMinError  = LBT21024FR_FINE_TUNE_MINERRORFACTOR;    //  20140617 ΢��
	Step_Manager_task::m_CurFineTuneFactor.m_fMaxAdjust = LBT21024FR_FINE_TUNE_MAXADJUSTFACTOR;
	Step_Manager_task::m_CurFineTuneFactor.m_fStep      = LBT21024FR_FINE_TUNE_STEPFACTOR;

	LBT21024FR_SampleManage::Init();
}

void LBT21024FR_Base::Func_Init(void)
{
	board_config::Func_Dac_out=&LBT21024FR_Base::DAC_out;
	board_config::Func_SetRange=&LBT21024FR_Base::SetRange;
	board_config::Func_SetRelay=&LBT21024FR_Base::SetRelay;
	board_config::Func_Sample = &LBT21024FR_SampleManage::SampleISR;
// 	board_config::Func_TempSample = &LBT21024FR_TempSample::Temp_rd;  // 20140613  �¶Ȳ���
	board_config::Func_RedundantSafety= &LBT21024FR_Base::RedundantSafety; //�����ѹ����
	board_config::Func_ReleaseRedundantSafety=&LBT21024FR_Base::ReleaseRedundantSafety;//����������
    board_config::Func_FanAdj=&LBT21024FR_Base::FanAdjOperature;	//added by NiuYasong
	//#if(FAN_SAFETY_MODE==1)
	if(board_config::Feature_Enable_init->Fan_Safety_Mode==1)
	{
	   board_config::Func_FanSafety= &LBT21024FR_Base::FanCheckSafety; //���ȱ���
  }  //#endif	
	board_config::FeedWatchDog = &Feed_CpldWD;

   // #if(REDUNDANT_VOLT_MODE == REDUNDANT_M0)	
	if(board_config::Feature_Enable_init->RedundantVoltMode == REDUNDANT_M0)
	{	
		M0RedundantFunc::M0_RedundantMap.m_Type = eType_Voltage;
		M0RedundantFunc::M0_RedundantMap.m_fAdcGain = M0_ADC_Gain;
		M0RedundantFunc::M0_RedundantMap.m_fAdcOffset = M0_ADC_Offset;
		// place here
		St_SaftyCheckItem safetyInfo;
		for(unsigned char IV_ChanIdx = 0;IV_ChanIdx<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;IV_ChanIdx++)
		{
				M0RedundantFunc::M0_RedundantMap.m_bValid[IV_ChanIdx] = M0RedundantFunc::m_M0MapChnValid[IV_ChanIdx];	    
				M0RedundantFunc::GetM0SafetyInfo(IV_ChanIdx,M0RedundantFunc::M0_RedundantMap.m_Type,&safetyInfo);
			M0RedundantFunc::M0_RedundantMap.m_fSafetyH[IV_ChanIdx] = safetyInfo.m_fHigh;
			M0RedundantFunc::M0_RedundantMap.m_fSafetyL[IV_ChanIdx] = safetyInfo.m_fLow;
			M0RedundantFunc::M0_RedundantMap.m_ChanNo[IV_ChanIdx] = M0RedundantFunc::m_M0MapTable[IV_ChanIdx];
			M0RedundantFunc::M0RedundantFunc::m_bIsM0ReleaseCheck[IV_ChanIdx] = true;
			M0RedundantFunc::m_bClampValid[IV_ChanIdx] = false;
		}
		M0RedundantFunc::m_RedundantChnMap[M0RedundantFunc::M0_RedundantMap.m_Type]  = M0RedundantFunc::M0_RedundantMap;
  } 
	//#endif	
}

//commnet out chen 20130216

// void LBT21024FR_Base::regCHNInfo(unsigned char CH_Number,unsigned long type)
// {
// 	   
// }
// void LBT21024FR_Base::regEngine(unsigned char CH_Number,unsigned long type)
// {
// 	
// }
void LBT21024FR_Base::AdditionalEngine(void)
{
	OS_LogicProcess::SetCtrlOffToRst.st_Disposabletask_ptr = &LBT21024_Base::SetOffToRst[0];
	OS_LogicProcess::SetCtrlOffToRst.stepLen = LBT21024_Base::SetOffToRst_Len;
}

void LBT21024FR_Base::EngineStuff(void)
{
	memset(&SetCtrlOpenCh,   0,    sizeof(SetCtrlOpenCh));
	memset(&SetCtrlCloseCh,  0,    sizeof(SetCtrlCloseCh));
	memset(&SetCCToCC,       0,    sizeof(SetCCToCC));
	memset(&SetRstToOff,     0,    sizeof(SetRstToOff));

	SetCtrlOpenCh[0].uw32_DelayTime = 200;
	SetCtrlOpenCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

	SetCtrlOpenCh[1].uw32_DelayTime = 100;
	SetCtrlOpenCh[1].RelayMove.ActiveFlag = LBT21024FR_RLY_Protect;

	SetCtrlOpenCh[2].uw32_DelayTime = 200;
	SetCtrlOpenCh[2].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;

	SetCtrlOpenCh[3].uw32_DelayTime = 400;
	SetCtrlOpenCh[3].RelayMove.InactiveFlag = LBT21024FR_RLY_Protect;

	SetCtrlOpenCh[4].uw32_DelayTime = 500;
	SetCtrlOpenCh[4].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

	SetCtrlCloseCh[0].uw32_DelayTime = 100;
	SetCtrlCloseCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
	SetCtrlCloseCh[0].RelayMove.ActiveFlag = LBT21024FR_RLY_Protect;

	SetCtrlCloseCh[1].uw32_DelayTime = 150;
	SetCtrlCloseCh[1].RelayMove.InactiveFlag = LBT21024_RLY_Output;

	SetCtrlCloseCh[2].uw32_DelayTime = 50;
	SetCtrlCloseCh[2].RelayMove.InactiveFlag = LBT21024FR_RLY_Protect;


	SetCCToCC[0].uw32_DelayTime = 100;
	SetCCToCC[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO|ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
	SetCCToCC[0].RelayMove.ActiveFlag = LBT21024FR_RLY_Protect;
	SetCCToCC[0].RangeChangedFlag = 1;

	SetCCToCC[1].uw32_DelayTime = 200;
	SetCCToCC[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
	SetCCToCC[1].RangeChangedFlag = 1;

	SetCCToCC[2].uw32_DelayTime = 400;
	SetCCToCC[2].RelayMove.InactiveFlag = LBT21024FR_RLY_Protect;
	SetCCToCC[2].RangeChangedFlag = 1;

	SetCCToCC[3].uw32_DelayTime = 50;
	SetCCToCC[3].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

	SetRstToOff[0].uw32_DelayTime = 100;
	//SetRstToOff[0].RelayMove.ActiveFlag = 0;
	SetRstToOff[0].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT; //��Ҫ�ſ�clamp
	//SetRstToOff[0].RangeChangedFlag = 0;

	//SetRstToOff[1].uw32_DelayTime = 0;
	//SetRstToOff[1].RelayMove.ActiveFlag = 0;
	SetRstToOff[1].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
	SetOffToRst[0].uw32_DelayTime = 100;
	//SetOffToRst[0].RelayMove.ActiveFlag = 0;
	SetOffToRst[0].DacMove.DacFlag = ACTIVE_DAC_VH | ACTIVE_DAC_VL; 
	//SetOffToRst[0].RangeChangedFlag = 0;

	//SetOffToRst[1].uw32_DelayTime = 0;
	//SetOffToRst[1].RelayMove.ActiveFlag = 0;
	SetOffToRst[1].DacMove.DacFlag = ACTIVE_DAC_VH | ACTIVE_DAC_VL;;
	SetOffToRst[1].RangeChangedFlag = 0;
}

// char LBT21024FR_Base::ADC_rd(unsigned long CH_Number,unsigned long type)
// {
// 	return 0;
// }

/******************************************************************************
** Function name:		DAC_out
**
** Descriptions:	        typeA DAC output
**			     
** parameters:			CH_Number:0->Max Channel Number; Data:32bit value
** Returned value:		0/1
** 
******************************************************************************/
char LBT21024FR_Base::DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type)
{ 
	unsigned char chn;
    switch(type)
	{
        case ACTIVE_DAC_CUR:
            ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_u32DacValue = Data;    //��ֵΪƫ����
            LBT21024FR_ADC5DAC::Dac_out_Cur((ChannelIdx),Data);
            break;
//         case ACTIVE_DAC_VLT:
//             LBT21024FR_ADC5DAC::Dac_out_Vlt((ChannelIdx+2),Data);
//             break;
        case ACTIVE_DAC_VH:
					   if(ChannelIdx==0)
							 chn=9;
						 else
							 chn=11;
						 Data=0xFFFFFF-Data;
						 if(ChannelInfo_Manager::IsMasterChannel(ChannelIdx) || ChannelInfo_Manager::IsSingleChanMode())
             LBT21024FR_ADC5DAC::Dac_out_Clamp_Vlt(chn,Data);   // 20140613  ��ѹ����
             break;
        case ACTIVE_DAC_VL:
				    if(ChannelIdx==0)
							 chn=8;
						 else
							 chn=10;
						 Data=0xFFFFFF-Data;
						 if(ChannelInfo_Manager::IsMasterChannel(ChannelIdx) || ChannelInfo_Manager::IsSingleChanMode())
             LBT21024FR_ADC5DAC::Dac_out_Clamp_Vlt(chn,Data);    //VOUTC - DACL  3?
             break;
        default:
            break;
	}
	return 1;
}


char LBT21024FR_Base::SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange)
{
	switch(CurrRange)
	{
        case CURRENT_RANGE_1:
            LBT21024FR_SampleManage::I_ADG_Address[ChannelIdx]=ChannelIdx;
            LBT21024FR_relaystatus[ChannelIdx].Bits.R_CH = 1;
            CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21024FR_relaystatus[ChannelIdx].Reg);
//             ARBUS_DELAY(10);
					  LBT21024FR_relaystatus[ChannelIdx].Reg &= ~(LBT21024FR_RLY_R_CM | LBT21024FR_RLY_R_CL | LBT21024FR_RLY_R_CLL);
//             LBT21024FR_relaystatus[ChannelIdx].Bits.R_CM = 0;
//             LBT21024FR_relaystatus[ChannelIdx].Bits.R_CL = 0;
//             LBT21024FR_relaystatus[ChannelIdx].Bits.R_CLL = 0;
//             CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21024FR_relaystatus[ChannelIdx].Reg);
            break;
        case CURRENT_RANGE_2:
            LBT21024FR_SampleManage::I_ADG_Address[ChannelIdx]= 32+ChannelIdx;    //1+ChannelIdx*4
            LBT21024FR_relaystatus[ChannelIdx].Bits.R_CM = 1;
            CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21024FR_relaystatus[ChannelIdx].Reg);
//             ARBUS_DELAY(10);
							LBT21024FR_relaystatus[ChannelIdx].Reg &= ~(LBT21024FR_RLY_R_CH | LBT21024FR_RLY_R_CL | LBT21024FR_RLY_R_CLL);
//             LBT21024FR_relaystatus[ChannelIdx].Bits.R_CH = 0;
//             LBT21024FR_relaystatus[ChannelIdx].Bits.R_CL = 0;
//             LBT21024FR_relaystatus[ChannelIdx].Bits.R_CLL = 0;
//             CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21024FR_relaystatus[ChannelIdx].Reg);
            break;
        case CURRENT_RANGE_3:
            LBT21024FR_SampleManage::I_ADG_Address[ChannelIdx]= 64+ChannelIdx;   // 2+ChannelIdx*4
            LBT21024FR_relaystatus[ChannelIdx].Bits.R_CL = 1;
            CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21024FR_relaystatus[ChannelIdx].Reg);
//             ARBUS_DELAY(10);
							 LBT21024FR_relaystatus[ChannelIdx].Reg &= ~(LBT21024FR_RLY_R_CH | LBT21024FR_RLY_R_CM | LBT21024FR_RLY_R_CLL);
//             LBT21024FR_relaystatus[ChannelIdx].Bits.R_CH = 0;
//             LBT21024FR_relaystatus[ChannelIdx].Bits.R_CM = 0;
//             LBT21024FR_relaystatus[ChannelIdx].Bits.R_CLL = 0;
//             CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21024FR_relaystatus[ChannelIdx].Reg);	
            break;
        case CURRENT_RANGE_4:
            LBT21024FR_SampleManage::I_ADG_Address[ChannelIdx]= 96+ChannelIdx;   // 2+ChannelIdx*4
            LBT21024FR_relaystatus[ChannelIdx].Bits.R_CLL = 1;
            CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21024FR_relaystatus[ChannelIdx].Reg);
//             ARBUS_DELAY(10);
							 LBT21024FR_relaystatus[ChannelIdx].Reg &= ~(LBT21024FR_RLY_R_CH | LBT21024FR_RLY_R_CM | LBT21024FR_RLY_R_CL);
//             LBT21024FR_relaystatus[ChannelIdx].Bits.R_CH = 0;
//             LBT21024FR_relaystatus[ChannelIdx].Bits.R_CM = 0;
//             LBT21024FR_relaystatus[ChannelIdx].Bits.R_CL = 0;
//             CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21024FR_relaystatus[ChannelIdx].Reg);	
            break;
        default:
//             LBT21024FR_SampleManage::I_ADG_Address[ChannelIdx]= 64+ChannelIdx;
//             LBT21024FR_relaystatus[ChannelIdx].Bits.R_CH = 0;
//             LBT21024FR_relaystatus[ChannelIdx].Bits.R_CM = 0;
//             LBT21024FR_relaystatus[ChannelIdx].Bits.R_CL = 0;
//             LBT21024FR_relaystatus[ChannelIdx].Bits.R_CLL = 0;
//             CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21024FR_relaystatus[ChannelIdx].Reg);
							 LBT21024FR_relaystatus[ChannelIdx].Reg &= ~(LBT21024FR_RLY_R_CH | LBT21024FR_RLY_R_CM | LBT21024FR_RLY_R_CL | LBT21024FR_RLY_R_CLL);
            break;
	}
//	    LBT21024FR_SampleManage::I_ADG_Address[ChannelIdx]= (CurrRange > CURRENT_RANGE_4? (64+ChannelIdx) : (CurrRange*32+ChannelIdx));
    ARBUS_DELAY(10);
    CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21024FR_relaystatus[ChannelIdx].Reg);

	return 1;
}
void LBT21024FR_Base::SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali)
{
	Un_LBT21024FR_Relay2 MyRly;
 
	MyRly.m_Reg = SetRly;
	if(MyRly.m_Bits.Protect)
	{
		LBT21024FR_relaystatus[ChannelIdx].Bits.P_R = 1;
		CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21024FR_relaystatus[ChannelIdx].Reg);
	}

	MyRly.m_Reg = ClrRly;
	if(MyRly.m_Bits.Protect)
	{
		LBT21024FR_relaystatus[ChannelIdx].Bits.P_R = 0;
		CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21024FR_relaystatus[ChannelIdx].Reg);
	}
	if(MyRly.m_Bits.Output)
	{
		LBT21024FR_relaystatus[ChannelIdx].Bits.R_CH = 0;
			ARBUS_DELAY(10);
		CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21024FR_relaystatus[ChannelIdx].Reg);
		LBT21024FR_relaystatus[ChannelIdx].Bits.R_CM = 0;
			ARBUS_DELAY(10);
		CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21024FR_relaystatus[ChannelIdx].Reg);
		LBT21024FR_relaystatus[ChannelIdx].Bits.R_CL = 0;
		ARBUS_DELAY(10);
		CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21024FR_relaystatus[ChannelIdx].Reg);
		LBT21024FR_relaystatus[ChannelIdx].Bits.R_CLL = 0;
		ARBUS_DELAY(10);
		CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21024FR_relaystatus[ChannelIdx].Reg);
	}
}
void LBT21024FR_Base::Adjust_ADC_Gain(char type,char chnType)
{
	if(chnType == ADC_TYPE_I)
	{
		if(type == 1)    //����
		{
		    cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0xA9);  //modify by qjm20140829
			ARBUS_DELAY(5);
		}
	}
	else if(chnType == ADC_TYPE_V)
	{
		if(type == 1)    //����
		{
			cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x10);
			ARBUS_DELAY(5);
			cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x00);
			ARBUS_DELAY(5);
			cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x10);
		}
	}
}

void LBT21024FR_Base::ADC_ADG_open(unsigned char ChannelIdx) 
{
	CPLD_FUNC::WriteADGBus(ChannelIdx,0x01);
}

void LBT21024FR_Base::ADG_Reset(void)
{
	ControlPin::SetMeLow(LBT21024FR_Base::PinGroup[LBT21024FR_Pin_AdgRst]);
	ARBUS_DELAY(5);
	ControlPin::SetMeHigh(LBT21024FR_Base::PinGroup[LBT21024FR_Pin_AdgRst]);

}

void LBT21024FR_Base::changeADCRange(unsigned char range)
{
	ARBUS_DELAY(10);
	//?D??��?3��
	LBT21024FR_Base::Adjust_ADC_Gain(1,ADC_TYPE_I);
}

void LBT21024FR_Base::Feed_CpldWD(void)
{
  ControlPin::SetMeHigh(PinGroup[LBT21024FR_Pin_WD]);
	TIMERSFunc::DELAY_SYS(200, 0);
	ControlPin::SetMeLow(PinGroup[LBT21024FR_Pin_WD]);
}

char  LBT21024FR_Base::RedundantSafety(unsigned long CH_Number,unsigned long type)
{
	
//#if(REDUNDANT_VOLT_MODE == REDUNDANT_M0)
if(board_config::Feature_Enable_init->RedundantVoltMode == REDUNDANT_M0)	
	return (safety_check::RedundantSafetyFlg[CH_Number]);
else
	return (ControlPin::ReadMe(PinGroup[LBT21024FR_Pin_RedundantSafety]) == 0? 0: 1);
//#endif

}

char LBT21024FR_Base::ReleaseRedundantSafety(unsigned long uc_ChannelNo,unsigned long type)
{
	//Hardware can't set the voltage relay.
//#if(REDUNDANT_VOLT_MODE == REDUNDANT_M0)	
	if(board_config::Feature_Enable_init->RedundantVoltMode == REDUNDANT_M0)
	  return (excheck::M0ReleaseRedundantSafety(uc_ChannelNo,type));
  else
	  return 1;
//#endif
}
//#if(FAN_SAFETY_MODE==1)
char LBT21024FR_Base::FanCheckSafety(unsigned long CH_Number,unsigned long type)
{
	return (excheck::ExFanCheckSafety(PinGroup[LBT21024FR_Pin_FanCheckSignal]));
}
//#endif
char LBT21024FR_Base::FanAdjOperature(unsigned long DutyRatio,unsigned long FanAdjMod)//added by NiuYasong
{
	
    if(FanAdjMod!=PWMMOD)
    {
        board_config::bFanAdj=0;
        return 0;
    }
   
	for(unsigned char uc_i=0;uc_i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;uc_i++)
	{
		 float FanCtrlValue[8]={50,50,50,60,70,80,90,100};
		 float V_High = ChannelInfo_Manager::BoardCFG.m_Channel[uc_i].m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High;
	   float I_Low = ChannelInfo_Manager::BoardCFG.m_Channel[uc_i].m_IRange[ChannelInfo_Manager::ChannelInfo[uc_i].m_ucCurrentRange].m_Bound.m_Low;	  
	   FanAdj_task::ChannelPerAdjPower[uc_i] = (FanAdj_task::VR-V_High)*I_Low;	
		 
			switch(ChannelInfo_Manager::ChannelInfo[uc_i].m_ucCurrentRange)
			{
				//case CURRENT_RANGE_2:
				case CURRENT_RANGE_3:
            for(unsigned char uc_j=0;uc_j<8;uc_j++)
                FanCtrlValue[uc_j] = 50;
						
						break;
				case CURRENT_RANGE_4:
				       for(unsigned char uc_j=0;uc_j<8;uc_j++)
                            FanCtrlValue[uc_j] = 50;

						break;
				default:
						break;
			}
			FanAdj_task::Channel_Duty_Ratio[uc_i]=FanCtrlValue[FanAdj_task::Channel_m_Level[uc_i]];//��ø�ͨ����Ӧ�����ٶ�
  }

  FanAdj_task::MAX_Ratio = 50;//�������С
	for(unsigned char uc_i=0;uc_i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;uc_i++)//Ѱ�Ҹ�ͨ���������ٶ�
	{			
		if(FanAdj_task::Channel_Duty_Ratio[uc_i]>FanAdj_task::MAX_Ratio)
		{
			FanAdj_task::MAX_Ratio=FanAdj_task::Channel_Duty_Ratio[uc_i];
    }
	}
		

	LPC4078PWM::PWM_MatchUpdate(PWM_HD0,PWM_CH3, DutyRatio, PWM_MATCH_UPDATE_NOW);
	return 1;
} 
 
/*********************************************************************************
**                            End Of File
*********************************************************************************/
