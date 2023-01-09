
#include "../../Entry/includes.h"

#define  _LBT21084VB_VERSION_A_         0         //this is a temperory MACRO,and valish somedays later

#define  LBT21084VB_FINE_TUNE_STARTTIME          0.1f
#define  LBT21084VB_FINE_TUNE_MINERRORFACTOR     1E-4 //2E-5
#define  LBT21084VB_FINE_TUNE_STEPFACTOR         5E-5 //6E-6
#define  LBT21084VB_FINE_TUNE_MAXADJUSTFACTOR    0.03f  //  20140617 微调

#define  LBT21084VB_SETTING_TIME                 1000       //电流上升沿时间，单位为100us

#if HARDWARE_SDRAM_32M == 1
    #pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
    #pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif  
St_DisposableTask LBT21084VB_Base::SetCtrlOpenCh[5];
St_DisposableTask LBT21084VB_Base::SetCtrlCloseCh[3];
St_DisposableTask LBT21084VB_Base::SetCCToCC[4];    //加入了多range切换，所以这里扩充到10步
St_DisposableTask LBT21084VB_Base::SetRstToOff[3];
St_DisposableTask LBT21084VB_Base::SetCtrlOpenAcim[1];
St_DisposableTask LBT21084VB_Base::SetCtrlCloseAcim[1];

SSPI_Para  LBT21084VB_Base::LBT21084VB_SSP0DAC;
SSPI_Para  LBT21084VB_Base::LBT21084VB_SSP0VCDAC;    //VCLAMP
SSPI_Para  LBT21084VB_Base::LBT21084VB_SSP1ADC;
#pragma arm section 

unsigned char LBT21084VB_Base::SetCtrlOpenCh_Len = 5;
unsigned char LBT21084VB_Base::SetCtrlCloseCh_Len = 3;
unsigned char LBT21084VB_Base::SetCCToCC_Len = 4;
unsigned char LBT21084VB_Base::SetRstToOff_Len = 3;

/*
//                                           Port      Bits       PortType           Function               Address            
const St_Port LBT21084VB_Base::Pin_AdcSTART = {1,        28,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port LBT21084VB_Base::Pin_AdcDRDY1  = {2,        12,        eCPTYPE_MCUGPIO,   GPIO_I,                0};
const St_Port LBT21084VB_Base::Pin_AdcDRDY2  = {2,        13,        eCPTYPE_MCUGPIO,   GPIO_I,                0};
const St_Port LBT21084VB_Base::Pin_AdcCS1    = {1,        21,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port LBT21084VB_Base::Pin_AdcCS2    = {0,        6,         eCPTYPE_MCUGPIO,   GPIO_O,                0};
// const St_Port LBT21084VB_Base::Pin_AdcRst    = {1,        26,        eCPTYPE_MCUGPIO,   GPIO_O,                0};

const St_Port LBT21084VB_Base::Pin_DacCS          = {1,        0,         eCPTYPE_CPLD,      CPLD_O,                NBUS_WRITE_DCS};
const St_Port LBT21084VB_Base::Pin_DacRst         = {2,        8,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port LBT21084VB_Base::Pin_DacLoad        = {1,        3,         eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port LBT21084VB_Base::Pin_DacBusy        = {1,        2,         eCPTYPE_MCUGPIO,   GPIO_I,                0};

// const St_Port LBT21084VB_Base::Pin_Clamp_Dac_Bin  = {1,        27,        eCPTYPE_MCUGPIO,   GPIO_O,                1};
const St_Port LBT21084VB_Base::Pin_Clamp_Dac_CS   = {1,        4,         eCPTYPE_CPLD,      CPLD_O,                NBUS_WRITE_DCS};
const St_Port LBT21084VB_Base::Pin_Clamp_Dac_Load = {1,        23,        eCPTYPE_MCUGPIO,   GPIO_O,                1};
const St_Port LBT21084VB_Base::Pin_Clamp_Dac_Clr  = {1,        24,        eCPTYPE_MCUGPIO,   GPIO_O,                1};



 const St_Port LBT21084VB_Base::Pin_WD        = {0,        13,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
//clamp
const St_Port LBT21084VB_Base::Pin_SCK0      = {0,        15,        eCPTYPE_MCUGPIO,   SSP0_SCK,              0};
const St_Port LBT21084VB_Base::Pin_MISO0     = {0,        17,        eCPTYPE_MCUGPIO,   SSP0_MISO,             0};
const St_Port LBT21084VB_Base::Pin_MOSI0     = {0,        18,        eCPTYPE_MCUGPIO,   SSP0_MOSI,             0};
const St_Port LBT21084VB_Base::Pin_SCK1      = {0,        7,         eCPTYPE_MCUGPIO,   SSP1_SCK,              0};
const St_Port LBT21084VB_Base::Pin_MISO1     = {0,        8,         eCPTYPE_MCUGPIO,   SSP1_MISO,             0};
const St_Port LBT21084VB_Base::Pin_MOSI1     = {0,        9,         eCPTYPE_MCUGPIO,   SSP1_MOSI,             0};

const St_Port LBT21084VB_Base::Pin_PowerOn   = {2,        3,         eCPTYPE_MCUGPIO,   GPIO_O,                0};

const St_Port LBT21084VB_Base::Pin_AdgRst    = {1,        2,         eCPTYPE_CPLD,      CPLD_O,                NBUS_WRITE_DCS};

const St_Port LBT21084VB_Base::Pin_RedundantSafety= {1,   6,        eCPTYPE_MCUGPIO,   GPIO_I,                0};//用于中电，MAXWLL 
const St_Port LBT21084VB_Base::Pin_FAN         = {3,   18,          eCPTYPE_MCUGPIO,   PWM0,                 0};

#if(FAN_SAFETY_MODE==1)
const St_Port LBT21084VB_Base::Pin_FanCheckSignal= {3,   16,        eCPTYPE_MCUGPIO,   GPIO_I,                0}; //风扇运行与否的检测引脚
MsTime     LBT21084VB_Base::FanOverTime;
#endif
*/

const St_Port LBT21084VB_Base::PinGroup[LBT21084VB_Total_Pins] =	//注意，此处的管脚顺序必须和头文件中enum的管脚顺序高度一致
{//Port     Bits			PortType					  Function		  Address 
	{1,				28,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_AdcSTART
	{2,				12,				eCPTYPE_MCUGPIO,		GPIO_I,				0},	//Pin_AdcDRDY1
	{2,				13,				eCPTYPE_MCUGPIO,		GPIO_I,				0},	//Pin_AdcDRDY2
	{1,				21,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_AdcCS1
	{0,				6,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_AdcCS2

	//{1,				26,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_AdcRst
	{1,				0,				eCPTYPE_CPLD,		    CPLD_O,				NBUS_WRITE_DCS},	//Pin_DacCS
	{2,				8,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_DacRst
	{1,				3,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_DacLoad
	{1,				2,				eCPTYPE_MCUGPIO,		GPIO_I,				0},	//Pin_DacBusy

	{1,				27,				eCPTYPE_MCUGPIO,		GPIO_O,				1},	//Pin_Clamp_Dac_Bin
	{1,				4,				eCPTYPE_CPLD,		    CPLD_O,				NBUS_WRITE_DCS},	//Pin_Clamp_Dac_CS 
	{1,				23,				eCPTYPE_MCUGPIO,		GPIO_O,				1},	//Pin_Clamp_Dac_Load
	{1,				24,				eCPTYPE_MCUGPIO,		GPIO_O,				1},	//Pin_Clamp_Dac_Clr
	{0,				13,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_WD

//	{0,				15,				eCPTYPE_MCUGPIO,		SSP0_SCK,			0},	//Pin_SCK0
//	{0,				17,				eCPTYPE_MCUGPIO,		SSP0_MISO,		    0},	//Pin_MISO0
//	{0,				18,				eCPTYPE_MCUGPIO,		SSP0_MOSI,		    0},	//Pin_MOSI0
	{0,				7,				eCPTYPE_MCUGPIO,		SSP1_SCK,			0},	//Pin_SCK1
	{0,				8,				eCPTYPE_MCUGPIO,		SSP1_MISO,		    0},	//Pin_MISO1
	{0,				9,				eCPTYPE_MCUGPIO,		SSP1_MOSI,		    0},	//Pin_MOSI1

	{2,				3,				eCPTYPE_MCUGPIO,		GPIO_O,				1},	//Pin_PowerOn
	{1,				2,				eCPTYPE_CPLD,		    CPLD_O,				NBUS_WRITE_DCS},	//Pin_AdgRst
    {1,				6,				eCPTYPE_MCUGPIO,		GPIO_I,				0},	//Pin_RedundantSafety
	{3,				18,				eCPTYPE_MCUGPIO,		PWM0,				0},	//Pin_FAN
    //#if(FAN_SAFETY_MODE==1)
    {3,				16,				eCPTYPE_MCUGPIO,		GPIO_I,				0},	//Pin_FanCheckSignal
	//#endif
};

//#if(FAN_SAFETY_MODE==1)
MsTime     LBT21084VB_Base::FanOverTime;
//#endif

volatile St_LBT21084VB_Relay LBT21084VB_Base::LBT21084VB_relaystatus[MAXCHANNELNO];    //记录自身的relay状态

const unsigned char LBT21084VB_Base::m_bHaveVoltageDac = 0;
const unsigned char LBT21084VB_Base::m_bHaveClampVdac = 1;
const unsigned char LBT21084VB_Base::m_bChannelBasedClampVdac = 0;

volatile St_LBT21084VB_DIDO     LBT21084VB_Base::CPLD_DO;    //记录DO的channel状态
volatile St_LBT21084VB_DIDO     LBT21084VB_Base::CPLD_DI;    //记录DI的channel状态

void LBT21084VB_Base::Hardware_Init(void)
{  
	//unsigned char temp;
	LBT21084VB_FeatureEnable_Init();
	IO_Init();    //内部IO初始化
	Feed_CpldWD();  // 喂狗信号
	board_config::bHaveRedundantCheck = 1;    //1;  modify by qjm20140922
	board_config::ClampH_Outputflag = 0;
	board_config::ClampL_Outputflag = 0;
	
	UnitDO_Reset(0,0);
	
	CPLD_FUNC::Init();
	memset((void *)(&LBT21084VB_relaystatus), 0, sizeof(LBT21084VB_relaystatus));
		
	for(unsigned char ChannelIdx = 0;ChannelIdx<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;ChannelIdx++)
	{
        //LBT21084VB_relaystatus[ChannelIdx].Bits.R_VM = 1;
        //LBT21084VB_relaystatus[ChannelIdx].Bits.R_CH = 0;
        //LBT21084VB_relaystatus[ChannelIdx].Bits.R_CM = 0;
        //LBT21084VB_relaystatus[ChannelIdx].Bits.R_CL = 0;
        //LBT21084VB_relaystatus[ChannelIdx].Bits.P_R = 0;
		LBT21084VB_relaystatus[ChannelIdx].Reg = 0;
		LBT21084VB_relaystatus[ChannelIdx].Bits.R_VM = 1;
        CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21084VB_relaystatus[ChannelIdx].Reg);
	}
	Feed_CpldWD();  // 喂狗信号

	board_config::bHaveRedundantCheck = 1;
	board_config::ClampH_Outputflag = 0;
	board_config::ClampL_Outputflag = 0;	
	//SSP0 for DAC    AD5360
	LBT21084VB_SSP0DAC.uc_Module_Name = SSP0_DEVICE1;    //name
	LBT21084VB_SSP0DAC.uc_Speed = 7;//7;
	LBT21084VB_SSP0DAC.uc_CPOL = 0;
	LBT21084VB_SSP0DAC.uc_CPHA = 1;
	LBT21084VB_SSP0DAC.uc_MSTR = 1;    //master
	LBT21084VB_SSP0DAC.uc_BITS = 8;
	LBT21084VB_SSP0DAC.uc_LSBF = 0;
  	//SSP0 for DAC    AD5754
 	LBT21084VB_SSP0VCDAC.uc_Module_Name = SSP0_DEVICE3;    //name
	LBT21084VB_SSP0VCDAC.uc_Speed = 7;//5;
	LBT21084VB_SSP0VCDAC.uc_CPOL = 0;
	LBT21084VB_SSP0VCDAC.uc_CPHA = 1;
	LBT21084VB_SSP0VCDAC.uc_MSTR = 1;    //master
	LBT21084VB_SSP0VCDAC.uc_BITS = 8;
	LBT21084VB_SSP0VCDAC.uc_LSBF = 0;
	
	//SSP1 for ADC    AD7175
	LBT21084VB_SSP1ADC.uc_Module_Name = SSP1_DEVICE1;    //name
	LBT21084VB_SSP1ADC.uc_Speed = 7;
	LBT21084VB_SSP1ADC.uc_CPOL = 1;
	LBT21084VB_SSP1ADC.uc_CPHA = 1;       
	LBT21084VB_SSP1ADC.uc_MSTR = 1;    //master
	LBT21084VB_SSP1ADC.uc_BITS = 8;
	LBT21084VB_SSP1ADC.uc_LSBF = 0;
	
	TIMERSFunc::DELAY_SYS( 30000, 0 );	//delay 5ms
	Feed_CpldWD();  // 喂狗信号
	
	ADC_Init();
	DAC_Init();

	CPLD_FUNC::WriteDOBus(0,LBT21084VB_relaystatus[0].Reg);  //硬件改变控制时序，改成高有效 20140331

	ControlPin::ReInitPullUP();
	LBT21084VB_Base::changeADCRange(CURRENT_RANGE_1);
	Feed_CpldWD();  // ?11·D?o?
	CPLD_FUNC::Write(NBUS_WRITE_VOLTAGERELAY,0xFF);//电压控制开   modify by qjm 20140922
	//Engine();
	
	for(int i=0;i<8;i++)	// reset all ACIM relay
	{
		ARBUS_DELAY(10);
		CPLD_FUNC::WriteSpecialACIM(i,0);
	}
	
	OS_LogicProcess::InitBoardEngine(	&SetCtrlOpenCh[0],	sizeof(SetCtrlOpenCh)		/		sizeof(St_DisposableTask),
		&SetCtrlCloseCh[0],        sizeof(SetCtrlCloseCh)	/		sizeof(St_DisposableTask),
		&SetCCToCC[0], 			sizeof(SetCCToCC)				/		sizeof(St_DisposableTask),
		&SetRstToOff[0],		sizeof(SetRstToOff)			/		sizeof(St_DisposableTask)
		);
	AdditionalEngine();
	////////拉高钳压     // 20140613  电压保护
	LBT21084VB_Base::DAC_out(0,DAC_OUT_PUT_MAX,ACTIVE_DAC_VH);
	LBT21084VB_Base::DAC_out(1,DAC_OUT_PUT_MIN,ACTIVE_DAC_VL);
	
	//#if(FAN_SAFETY_MODE==1)
	if(board_config::Feature_Enable_init->Fan_Safety_Mode==1)
	{
	    // 风扇初始化 
	    ///////风扇调速  初始化时设定为100%
	    MsTime MyMsTime;
	    unsigned long long TimeTicks;
	    MyMsTime.Second =LBT21084VB_Base::FanOverTime.Second;
	    MyMsTime.Us100 = LBT21084VB_Base::FanOverTime.Us100;
	    while(1)
	    {
            if(PCA9501::SetExpander(34,1))
            {			
                I2CBusNew::AD5245_SET(0,0,I2C2); 
                PCA9501::SetExpander(34,0);
                //#if(FAN_SAFETY_MODE==1)
				if(board_config::Feature_Enable_init->Fan_Safety_Mode==1)
//             temp =ControlPin::ReadMe(PinGroup[LBT21084VB_Pin_FanCheckSignal]);
//             if(temp==0)
//                 safety_check::Fan_FaultCheckFlag=1;
//             else
                    safety_check::Fan_FaultCheckFlag=0; 
                //#endif
                break;
            }
            else
            {
                TimeTicks = CommonFunc::GetTimeTicks(&MyMsTime);
                if(TimeTicks >= 1000)       
                {
                    //#if(FAN_SAFETY_MODE==1)
					if(board_config::Feature_Enable_init->Fan_Safety_Mode==1)
//                 temp =ControlPin::ReadMe(PinGroup[LBT21084VB_Pin_FanCheckSignal]);
//                 if(temp==0)
//                     safety_check::Fan_FaultCheckFlag=1;
//                 else
                        safety_check::Fan_FaultCheckFlag=0;
                //#endif
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
	FanAdj_task::VF=12; //gai
	FanAdj_task::VR=-2.5; //gai
	FanAdj_task::HeatSinkCounter=4;
	FanAdj_task::AdjMod=PWMMOD;
	FanAdj_task::PerAdjPower=1;  //gai
	board_config::bFanAdj=1;
	LPC4078PWM::SetPWM(PWM_HD0,PWM_CH3,100,20);
}

void LBT21084VB_Base::LBT21084VB_FeatureEnable_Init(void) 
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

void LBT21084VB_Base::IO_Init(void)
{
    for(int i=0;i<LBT21084VB_Total_Pins;i++)
	{
		ControlPin::SetIObyCP(PinGroup[i]);
	}
} 

void LBT21084VB_Base::ADC_Init(void)
{	
	LBT21084VB_ADC5DAC::Adc_Init();

	TIMERSFunc::DELAY_SYS( 10000, 0 );	//delay 5ms
}
void LBT21084VB_Base::DAC_Init(void)
{
    LBT21084VB_ADC5DAC::Dac_Init();
    board_config::DacInfo.m_bCurrRngCount = 4;
    board_config::DacInfo.m_bVoltRngCount = 1;
	board_config::DacInfo.m_bHaveVoltageDac = m_bHaveVoltageDac;
	board_config::DacInfo.m_bHaveClampVdac = m_bHaveClampVdac;
	board_config::DacInfo.m_bChannelBasedClampVdac = m_bChannelBasedClampVdac;
	board_config::DacInfo.m_uSettingTime = LBT21084VB_SETTING_TIME;
	TIMERSFunc::DELAY_SYS( 10000, 0 );	//delay 5ms	
}

void LBT21084VB_Base::Software_Init(void)
{
	Func_Init();//函数指针初始化，四个基本操作
	EngineStuff();   //引擎结构指针初始化

// 	for(unsigned char i = 0; i < MAXCHANNELNO; i++)    //从0开始
// 	{
// 		regCHNInfo(i,board_config::st_channel_info[i].Channel_type);//注册chn info
// 		regEngine(i,board_config::st_channel_info[i].Channel_type);//注册引擎			
// 	}

	Step_Manager_task::m_CurFineTuneFactor.m_fStartTime = LBT21084VB_FINE_TUNE_STARTTIME;
	Step_Manager_task::m_CurFineTuneFactor.m_fMinError  = LBT21084VB_FINE_TUNE_MINERRORFACTOR;    //  20140617 微调
	Step_Manager_task::m_CurFineTuneFactor.m_fMaxAdjust = LBT21084VB_FINE_TUNE_MAXADJUSTFACTOR;
	Step_Manager_task::m_CurFineTuneFactor.m_fStep      = LBT21084VB_FINE_TUNE_STEPFACTOR;

	LBT21084VB_SampleManage::Init();
}

void LBT21084VB_Base::Func_Init(void)
{
// 	board_config::Func_Adc_rd=&LBT21084VB_Base::ADC_rd;
	board_config::Func_Dac_out=&LBT21084VB_Base::DAC_out;
	board_config::Func_SetRange=&LBT21084VB_Base::SetRange;
	board_config::Func_SetRelay=&LBT21084VB_Base::SetRelay;
	board_config::Func_Sample = &LBT21084VB_SampleManage::SampleISR;

    board_config::Func_UnitDO=&LBT21084VB_Base::OUTPUT_UnitDO;
	board_config::Func_UnitDI=&LBT21084VB_Base::Read_UnitDI;
	board_config::Func_UnitDI_ReadAll = &LBT21084VB_Base::UnitDI_ReadAll;
	board_config::Func_UnitDO_Reset = &LBT21084VB_Base::UnitDO_Reset;
	
	board_config::Func_RedundantSafety= &LBT21084VB_Base::RedundantSafety; //冗余电压保护
	board_config::Func_ReleaseRedundantSafety=&LBT21084VB_Base::ReleaseRedundantSafety;//冗余解除操作
	board_config::Func_FanAdj=&LBT21084VB_Base::FanAdjOperate;
	//#if(FAN_SAFETY_MODE==1)
	if(board_config::Feature_Enable_init->Fan_Safety_Mode==1)
	{
	  board_config::Func_FanSafety= &LBT21084VB_Base::FanCheckSafety; //风扇保护		
	}    
	//#endif	
	board_config::FeedWatchDog = &Feed_CpldWD;

    //#if(REDUNDANT_VOLT_MODE == REDUNDANT_M0)	
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
			M0RedundantFunc::m_bIsM0ReleaseCheck[IV_ChanIdx] = true;
			M0RedundantFunc::m_bClampValid[IV_ChanIdx] = false;
		}
		M0RedundantFunc::m_RedundantChnMap[M0RedundantFunc::M0_RedundantMap.m_Type]  = M0RedundantFunc::M0_RedundantMap;
  } 
	//#endif	
}
/*
//commnet out chen 20130216
void LBT21084VB_Base::Engine(void)
{
	OS_LogicProcess::SetCtrlRstToCur.st_Disposabletask_ptr = &LBT21084VB_Base::SetCtrlOpenCh[0];
	OS_LogicProcess::SetCtrlRstToCur.stepLen = LBT21084VB_Base::SetCtrlOpenCh_Len;

	OS_LogicProcess::SetCtrlRstToVlt.st_Disposabletask_ptr = &LBT21084VB_Base::SetCtrlOpenCh[0];
	OS_LogicProcess::SetCtrlRstToVlt.stepLen = LBT21084VB_Base::SetCtrlOpenCh_Len;

	//没有动作
	OS_LogicProcess::SetCtrlRstToOff.st_Disposabletask_ptr = &LBT21084VB_Base::SetRstToOff[0];
	OS_LogicProcess::SetCtrlRstToOff.stepLen = LBT21084VB_Base::SetRstToOff_Len;
	OS_LogicProcess::SetCtrlOffToRst.st_Disposabletask_ptr = &LBT21084VB_Base::SetRstToOff[0];
	OS_LogicProcess::SetCtrlOffToRst.stepLen = LBT21084VB_Base::SetRstToOff_Len;

	OS_LogicProcess::SetCtrlOffToCur.st_Disposabletask_ptr = &LBT21084VB_Base::SetCtrlOpenCh[0];    //指向数组0
	OS_LogicProcess::SetCtrlOffToCur.stepLen = LBT21084VB_Base::SetCtrlOpenCh_Len;

	OS_LogicProcess::SetCtrlOffToVlt.st_Disposabletask_ptr = &LBT21084VB_Base::SetCtrlOpenCh[0];
	OS_LogicProcess::SetCtrlOffToVlt.stepLen = LBT21084VB_Base::SetCtrlOpenCh_Len;

	OS_LogicProcess::SetCtrlCurToCur.st_Disposabletask_ptr = &LBT21084VB_Base::SetCCToCC[0];
	OS_LogicProcess::SetCtrlCurToCur.stepLen = LBT21084VB_Base::SetCCToCC_Len;


	OS_LogicProcess::SetCtrlCurToVlt.st_Disposabletask_ptr = &LBT21084VB_Base::SetCCToCC[0];
	OS_LogicProcess::SetCtrlCurToVlt.stepLen = LBT21084VB_Base::SetCCToCC_Len;

	OS_LogicProcess::SetCtrlCurToOff.st_Disposabletask_ptr = &LBT21084VB_Base::SetCtrlCloseCh[0];
	OS_LogicProcess::SetCtrlCurToOff.stepLen = LBT21084VB_Base::SetCtrlCloseCh_Len ;

	OS_LogicProcess::SetCtrlCurToRst.st_Disposabletask_ptr = &LBT21084VB_Base::SetCtrlCloseCh[0];
	OS_LogicProcess::SetCtrlCurToRst.stepLen = LBT21084VB_Base::SetCtrlCloseCh_Len ;

	
	OS_LogicProcess::SetCtrlVltToVlt.st_Disposabletask_ptr = &LBT21084VB_Base::SetCCToCC[0];
	OS_LogicProcess::SetCtrlVltToVlt.stepLen = LBT21084VB_Base::SetCCToCC_Len;

	OS_LogicProcess::SetCtrlVltToCur.st_Disposabletask_ptr = &LBT21084VB_Base::SetCCToCC[0];
	OS_LogicProcess::SetCtrlVltToCur.stepLen = LBT21084VB_Base::SetCCToCC_Len;

	OS_LogicProcess::SetCtrlVltToOff.st_Disposabletask_ptr = &LBT21084VB_Base::SetCtrlCloseCh[0];
	OS_LogicProcess::SetCtrlVltToOff.stepLen = LBT21084VB_Base::SetCtrlCloseCh_Len ;

	OS_LogicProcess::SetCtrlVltToRst.st_Disposabletask_ptr = &LBT21084VB_Base::SetCtrlCloseCh[0];
	OS_LogicProcess::SetCtrlVltToRst.stepLen = LBT21084VB_Base::SetCtrlCloseCh_Len ;

	
}*/

// void LBT21084VB_Base::regCHNInfo(unsigned char CH_Number,unsigned long type)
// {
// 	   
// }
// void LBT21084VB_Base::regEngine(unsigned char CH_Number,unsigned long type)
// {
// 	
// }

void LBT21084VB_Base::AdditionalEngine(void)
{
	// ACIM Engine
	OS_LogicProcess::SetCtrlAcimOn.st_Disposabletask_ptr = &LBT21084VB_Base::SetCtrlOpenAcim[0];
	OS_LogicProcess::SetCtrlAcimOn.stepLen = sizeof(SetCtrlOpenAcim) / sizeof(St_DisposableTask);
	
	OS_LogicProcess::SetCtrlAcimOff.st_Disposabletask_ptr = &LBT21084VB_Base::SetCtrlCloseAcim[0];
	OS_LogicProcess::SetCtrlAcimOff.stepLen = sizeof(SetCtrlCloseAcim) / sizeof(St_DisposableTask);
}
void LBT21084VB_Base::EngineStuff(void)
{
	memset(&SetCtrlOpenCh,   0,    sizeof(SetCtrlOpenCh));
	memset(&SetCtrlCloseCh,  0,    sizeof(SetCtrlCloseCh));
	memset(&SetCCToCC,       0,    sizeof(SetCCToCC));
	memset(&SetRstToOff,     0,    sizeof(SetRstToOff));
	memset(&SetCtrlOpenAcim,	0,    sizeof(SetCtrlOpenAcim));
	memset(&SetCtrlCloseAcim,	0,    sizeof(SetCtrlCloseAcim));

	SetCtrlOpenCh[0].uw32_DelayTime = 10;
	SetCtrlOpenCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

	SetCtrlOpenCh[1].uw32_DelayTime = 50;
	SetCtrlOpenCh[1].RelayMove.ActiveFlag = LBT21084VB_RLY_Protect;

	SetCtrlOpenCh[2].uw32_DelayTime = 100;
	SetCtrlOpenCh[2].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;

	SetCtrlOpenCh[3].uw32_DelayTime = 50;
	SetCtrlOpenCh[3].RelayMove.InactiveFlag = LBT21084VB_RLY_Protect;

	SetCtrlOpenCh[4].uw32_DelayTime = 10;
	SetCtrlOpenCh[4].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

	SetCtrlCloseCh[0].uw32_DelayTime = 100;
	SetCtrlCloseCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
//	SetCtrlCloseCh[0].RelayMove.ActiveFlag = LBT21084VB_RLY_Protect;

	SetCtrlCloseCh[1].uw32_DelayTime = 100;
	SetCtrlCloseCh[1].RelayMove.InactiveFlag = LBT21084VB_RLY_Output;

//	SetCtrlCloseCh[2].uw32_DelayTime = 50;
//	SetCtrlCloseCh[2].RelayMove.InactiveFlag = LBT21084VB_RLY_Protect;
	
	SetCtrlCloseCh[2].uw32_DelayTime = 50;
	SetCtrlCloseCh[2].RelayMove.InactiveFlag = LBT21084VB_RLY_ACIM;


	SetCCToCC[0].uw32_DelayTime = 50;
	SetCCToCC[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
	SetCCToCC[0].RelayMove.ActiveFlag = LBT21084VB_RLY_Protect;
	SetCCToCC[0].RangeChangedFlag = 1;

	SetCCToCC[1].uw32_DelayTime = 100;
	SetCCToCC[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
	SetCCToCC[1].RangeChangedFlag = 1;

	SetCCToCC[2].uw32_DelayTime = 50;
	SetCCToCC[2].RelayMove.InactiveFlag = LBT21084VB_RLY_Protect;
	SetCCToCC[2].RangeChangedFlag = 1;

	SetCCToCC[3].uw32_DelayTime = 10;  //50;
	SetCCToCC[3].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

	SetRstToOff[0].uw32_DelayTime = 100;
	SetRstToOff[0].RelayMove.InactiveFlag = LBT21084VB_RLY_Output;  //为防止并联时从通道状态被意外设置成CCS_OFF ,保险起见这个地方也处理继电器关闭  dirui 2015/09/16
	SetRstToOff[0].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT; //需要放开clamp;
	SetRstToOff[0].RangeChangedFlag = 0;

	SetRstToOff[1].uw32_DelayTime = 0;
	SetRstToOff[1].RelayMove.ActiveFlag = 0;
	SetRstToOff[1].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
	SetRstToOff[1].RangeChangedFlag = 0;
	
	SetRstToOff[2].uw32_DelayTime = 50;
	SetRstToOff[2].RelayMove.InactiveFlag = LBT21084VB_RLY_ACIM;
	
	SetCtrlOpenAcim[0].uw32_DelayTime = 100;
	SetCtrlOpenAcim[0].RelayMove.ActiveFlag = LBT21084VB_RLY_ACIM;

	SetCtrlCloseAcim[0].uw32_DelayTime = 100;
	SetCtrlCloseAcim[0].RelayMove.InactiveFlag = LBT21084VB_RLY_ACIM;
}

//char LBT21084VB_Base::ADC_rd(unsigned long CH_Number,unsigned long type)
//{
//	return 0;
//}

/******************************************************************************
** Function name:		DAC_out
**
** Descriptions:	        typeA DAC output
**			     
** parameters:			CH_Number:0->Max Channel Number; Data:32bit value
** Returned value:		0/1
** 
******************************************************************************/
char LBT21084VB_Base::DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type)
{ 
char chn = (char)(ChannelIdx);
	unsigned long value = Data;    //强制保护
	
    switch(type)
	{
	    case ACTIVE_DAC_CUR:
			ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_u32DacValue = Data;    //此值为偏移码
	        //if(SSPIFunc::getModuleName(0)  != LBT21084VB_SSP0_DAC_NAME)
            //{
            //    SSPIFunc::setPara(&LBT21084VB_Base::LBT21084VB_SSP0DAC,0);
            //    SSPIFunc::my_Init(0);              //初始化
            //    ARBUS_DELAY(10);
            //}
			SSPIFunc::ConfirmDevice(0, &LBT21084VB_Base::LBT21084VB_SSP0DAC);
			//if(ChannelIdx<4)
		    //    DAC16bit_AD5360::Dac_out(ChannelIdx,Data);
			//else
			//    DAC16bit_AD5360::Dac_out((ChannelIdx+4),Data);
				
            if(ChannelIdx >= 4)
			    ChannelIdx += 4;
		    DAC16bit_AD5360::Dac_out(ChannelIdx,Data);
			break;

		//comment out chen
		case ACTIVE_DAC_VLT:
			LBT21084VB_ADC5DAC::Dac_out_Vlt(chn,value);
			break;
			///////////////////钳位
		case ACTIVE_DAC_VH:
			LBT21084VB_ADC5DAC::Dac_out_Clamp_Vlt(0,value);    //VOUTA - DACH
			break;
		case ACTIVE_DAC_VL:
			LBT21084VB_ADC5DAC::Dac_out_Clamp_Vlt(1,value);    //VOUTC - DACL
            break;
		default:
			break;
	}
	return 1;
}


char LBT21084VB_Base::SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange)
{
	switch(CurrRange)
	{
        case CURRENT_RANGE_1:
            //LBT21084VB_SampleManage::I_ADG_Address[ChannelIdx]=32+ChannelIdx;   //ChannelIdx+8;
            LBT21084VB_relaystatus[ChannelIdx].Bits.R_CH = 1;
            CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21084VB_relaystatus[ChannelIdx].Reg);
            //ARBUS_DELAY(10);
            //LBT21084VB_relaystatus[ChannelIdx].Bits.R_CM = 0;
            //LBT21084VB_relaystatus[ChannelIdx].Bits.R_CL = 0;
            //LBT21084VB_relaystatus[ChannelIdx].Bits.R_CLL = 0;
            //CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21084VB_relaystatus[ChannelIdx].Reg);
            LBT21084VB_relaystatus[ChannelIdx].Reg &= ~(LBT21084VB_RLY_R_CM | LBT21084VB_RLY_R_CL | LBT21084VB_RLY_R_CLL);
			break;
        case CURRENT_RANGE_2:
            //LBT21084VB_SampleManage::I_ADG_Address[ChannelIdx]= ChannelIdx;
            LBT21084VB_relaystatus[ChannelIdx].Bits.R_CM = 1;
            CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21084VB_relaystatus[ChannelIdx].Reg);
            //ARBUS_DELAY(10);
            //LBT21084VB_relaystatus[ChannelIdx].Bits.R_CH = 0;
            //LBT21084VB_relaystatus[ChannelIdx].Bits.R_CL = 0;
            //LBT21084VB_relaystatus[ChannelIdx].Bits.R_CLL = 0;
            //CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21084VB_relaystatus[ChannelIdx].Reg);
            LBT21084VB_relaystatus[ChannelIdx].Reg &= ~(LBT21084VB_RLY_R_CH | LBT21084VB_RLY_R_CL | LBT21084VB_RLY_R_CLL);
			break;
        case CURRENT_RANGE_3:
            //LBT21084VB_SampleManage::I_ADG_Address[ChannelIdx]= ChannelIdx;
            LBT21084VB_relaystatus[ChannelIdx].Bits.R_CL = 1;
            CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21084VB_relaystatus[ChannelIdx].Reg);
            //ARBUS_DELAY(10);
            //LBT21084VB_relaystatus[ChannelIdx].Bits.R_CH = 0;
            //LBT21084VB_relaystatus[ChannelIdx].Bits.R_CM = 0;
            //LBT21084VB_relaystatus[ChannelIdx].Bits.R_CLL = 0;
            //CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21084VB_relaystatus[ChannelIdx].Reg);
			LBT21084VB_relaystatus[ChannelIdx].Reg &= ~(LBT21084VB_RLY_R_CH | LBT21084VB_RLY_R_CM | LBT21084VB_RLY_R_CLL);	
            break;
        case CURRENT_RANGE_4:
            //LBT21084VB_SampleManage::I_ADG_Address[ChannelIdx]= ChannelIdx;
            LBT21084VB_relaystatus[ChannelIdx].Bits.R_CLL = 1;
            CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21084VB_relaystatus[ChannelIdx].Reg);
            //ARBUS_DELAY(10);
            //LBT21084VB_relaystatus[ChannelIdx].Bits.R_CH = 0;
            //LBT21084VB_relaystatus[ChannelIdx].Bits.R_CM = 0;
            //LBT21084VB_relaystatus[ChannelIdx].Bits.R_CL = 0;
            //CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21084VB_relaystatus[ChannelIdx].Reg);	
            LBT21084VB_relaystatus[ChannelIdx].Reg &= ~(LBT21084VB_RLY_R_CH | LBT21084VB_RLY_R_CM | LBT21084VB_RLY_R_CL);	
			break;
        default:
            //LBT21084VB_SampleManage::I_ADG_Address[ChannelIdx]= ChannelIdx;
            //LBT21084VB_relaystatus[ChannelIdx].Bits.R_CH = 0;
            //LBT21084VB_relaystatus[ChannelIdx].Bits.R_CM = 0;
            //LBT21084VB_relaystatus[ChannelIdx].Bits.R_CL = 0;
            //LBT21084VB_relaystatus[ChannelIdx].Bits.R_CLL = 0;
            //CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21084VB_relaystatus[ChannelIdx].Reg);
            LBT21084VB_relaystatus[ChannelIdx].Reg &= ~(LBT21084VB_RLY_R_CH | LBT21084VB_RLY_R_CM | LBT21084VB_RLY_R_CL | LBT21084VB_RLY_R_CLL);
			break;
	}
	LBT21084VB_SampleManage::I_ADG_Address[ChannelIdx]= (CurrRange == CURRENT_RANGE_1? 32+ChannelIdx : ChannelIdx);;
	ARBUS_DELAY(10);
	CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21084VB_relaystatus[ChannelIdx].Reg);
	
	//if((LBT21084VB_relaystatus[ChannelIdx].Bits.R_CH == 0) && 
	//	 (LBT21084VB_relaystatus[ChannelIdx].Bits.R_CM == 0) &&
	//   (LBT21084VB_relaystatus[ChannelIdx].Bits.R_CL == 0) &&
	//   (LBT21084VB_relaystatus[ChannelIdx].Bits.R_CLL == 0))
	if( (LBT21084VB_relaystatus[ChannelIdx].Reg & (LBT21084VB_RLY_R_CH | LBT21084VB_RLY_R_CM | LBT21084VB_RLY_R_CL | LBT21084VB_RLY_R_CLL)) == 0x0000)
	{
		LBT21084VB_relaystatus[ChannelIdx].Bits.R_VM = 1;
		CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21084VB_relaystatus[ChannelIdx].Reg);
	}
	return 1;
}
void LBT21084VB_Base::SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali)
{
	Un_LBT21084VB_Relay2 MyRly;
    //bAutoCali = 0;
	//if(bAutoCali)
	//	LBT21084VB_relaystatus[ChannelIdx].Bits.R_VM = 0;
	//else
	//	LBT21084VB_relaystatus[ChannelIdx].Bits.R_VM = 1;
    LBT21084VB_relaystatus[ChannelIdx].Bits.R_VM = ~bAutoCali;
	CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21084VB_relaystatus[ChannelIdx].Reg);
	
	MyRly.m_Reg = SetRly;
	if(MyRly.m_Bits.Protect)
	{
		LBT21084VB_relaystatus[ChannelIdx].Bits.P_R = 1;
		CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21084VB_relaystatus[ChannelIdx].Reg);
	}
	if(MyRly.m_Bits.Acim)
	{
		LBT21084VB_relaystatus[ChannelIdx].Bits.R_ACIM = 1;
		if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_HC)	// This board is 2108MC or 2108HC, with special ACIM control
		{
			Step_Manager_task::ACIM_CH_ON = ChannelIdx;
			CPLD_FUNC::WriteSpecialACIM(ChannelIdx,1);
		}
		else	// This board is 2108LC, with common ACIM control(one bit for each ACIM relay)
			CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21084VB_relaystatus[ChannelIdx].Reg);
	}

	MyRly.m_Reg = ClrRly;
	if(MyRly.m_Bits.Protect)
	{
		LBT21084VB_relaystatus[ChannelIdx].Bits.P_R = 0;
		CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21084VB_relaystatus[ChannelIdx].Reg);
	}
	if(MyRly.m_Bits.Acim)
	{
		LBT21084VB_relaystatus[ChannelIdx].Bits.R_ACIM = 0;
		if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_HC)	// This board is 2108MC or 2108HC, with special ACIM control
		{
			if(Step_Manager_task::ACIM_CH_ON == ChannelIdx)	// 如果待关闭的通道不是已打开的通道，直接跳过
			{
				Step_Manager_task::ACIM_CH_ON = 8;	// 无ACIM通道连接
				CPLD_FUNC::WriteSpecialACIM(ChannelIdx,0);
			}
		}
		else	// This board is 2108LC, with common ACIM control(one bit for each ACIM relay)
			CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21084VB_relaystatus[ChannelIdx].Reg);
	}
	if(MyRly.m_Bits.Output)
	{
        unsigned short relay[4] = {LBT21084VB_RLY_R_CH, LBT21084VB_RLY_R_CM, LBT21084VB_RLY_R_CL, LBT21084VB_RLY_R_CLL};
		for(int i=0;i<4;i++)
		{
			LBT21084VB_relaystatus[ChannelIdx].Reg &= ~relay[i];	// CH=0 then CM=0 then CL=0 then CLL=0
			ARBUS_DELAY(10);
			CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21084VB_relaystatus[ChannelIdx].Reg);
		}
		
		/*LBT21084VB_relaystatus[ChannelIdx].Bits.R_CH = 0;
		ARBUS_DELAY(10);
		CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21084VB_relaystatus[ChannelIdx].Reg);
		LBT21084VB_relaystatus[ChannelIdx].Bits.R_CM = 0;
		ARBUS_DELAY(10);
		CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21084VB_relaystatus[ChannelIdx].Reg);
		LBT21084VB_relaystatus[ChannelIdx].Bits.R_CL = 0;
		ARBUS_DELAY(10);
		CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21084VB_relaystatus[ChannelIdx].Reg);
		LBT21084VB_relaystatus[ChannelIdx].Bits.R_CLL = 0;
		ARBUS_DELAY(10);
		CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21084VB_relaystatus[ChannelIdx].Reg);*/
		
	}
	
	//if((LBT21084VB_relaystatus[ChannelIdx].Bits.R_CH == 0) && 
	//	 (LBT21084VB_relaystatus[ChannelIdx].Bits.R_CM == 0) &&
	//   (LBT21084VB_relaystatus[ChannelIdx].Bits.R_CL == 0) &&
	//   (LBT21084VB_relaystatus[ChannelIdx].Bits.R_CLL == 0))
	if( (LBT21084VB_relaystatus[ChannelIdx].Reg & (LBT21084VB_RLY_R_CH | LBT21084VB_RLY_R_CM | LBT21084VB_RLY_R_CL | LBT21084VB_RLY_R_CLL)) == 0x0000)
	{
		LBT21084VB_relaystatus[ChannelIdx].Bits.R_VM = 1;
		CPLD_FUNC::WriteDOBus(ChannelIdx,LBT21084VB_relaystatus[ChannelIdx].Reg);
	}
}
void LBT21084VB_Base::Adjust_ADC_Gain(char type,char chnType)
{
	 float VF =  ChannelInfo_Manager::BoardCFG.m_Channel[0].m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High;	
	
		if(chnType == ADC_TYPE_I)
		{
			if(type == 1)    //单倍
			{
				if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_HC)
				{
					if(VF<=5)
					{
						cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x2A9);  
							ARBUS_DELAY(5);
					}
					else if((VF>5)&&(VF<=10))
					{
						cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x29A);  
							ARBUS_DELAY(5);
					}
					else if((VF>10)&&(VF<=15))
					{
						cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x21A);  
						ARBUS_DELAY(5);
					}
					else 
					{
						cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x29A);  
						ARBUS_DELAY(5);
					}
				}
				else
				{
					cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x2A9);  //modify by qjm20140925
				  ARBUS_DELAY(5);
        }
		}
	}
	else if(chnType == ADC_TYPE_V)
	{
		if(type == 1)    //单倍
		{
			cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x10);
			ARBUS_DELAY(5);
			cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x00);
			ARBUS_DELAY(5);
			cpu_iowr_16(NBUS_WRITE_PGA_GAIN,0x10);
		}
	}
}

void LBT21084VB_Base::ADC_ADG_open(unsigned char ChannelIdx) 
{
	CPLD_FUNC::WriteADGBus(ChannelIdx,0x01);
}

void LBT21084VB_Base::ADG_Reset(void)
{

}

void LBT21084VB_Base::changeADCRange(unsigned char range)
{
	ARBUS_DELAY(10);
	/*switch(range)
	{
        case CURRENT_RANGE_1:
            LBT21084VB_Base::Adjust_ADC_Gain(1,ADC_TYPE_I);
            break;
        case CURRENT_RANGE_2:
            LBT21084VB_Base::Adjust_ADC_Gain(1,ADC_TYPE_I);
            break;
        case CURRENT_RANGE_3:
            LBT21084VB_Base::Adjust_ADC_Gain(1,ADC_TYPE_I);
            break;
        default:
            LBT21084VB_Base::Adjust_ADC_Gain(1,ADC_TYPE_I);
            break;			
	}*/
    LBT21084VB_Base::Adjust_ADC_Gain(1,ADC_TYPE_I);
}

void LBT21084VB_Base::Feed_CpldWD(void)
{
	ControlPin::SetMeHigh(PinGroup[LBT21084VB_Pin_WD]);
	TIMERSFunc::DELAY_SYS(200, 0);
	ControlPin::SetMeLow(PinGroup[LBT21084VB_Pin_WD]);
}

unsigned char LBT21084VB_Base::getBID(void)
{
	//BS0-BS7    P3.24-P3.31
	unsigned char BoardID = 0;

	St_Port MyPort;
	MyPort.PortType = eCPTYPE_MCUGPIO;
	MyPort.Function = GPIO_I;
	MyPort.Port = 3;
	MyPort.Bits = 24;

	for(unsigned char i = 0;i<8;i++)
	{
		ControlPin::SetIObyCP(MyPort);
		BoardID |= ControlPin::ReadMe(MyPort)<<i;
		
		MyPort.Bits++;
	}

	return BoardID;
}

char LBT21084VB_Base::OUTPUT_UnitDO(unsigned long CH_Number,unsigned long DO_Idx,unsigned long Value)
{
//    unsigned char m_OutPutValue;
    unsigned short int temp = 0;

    //if(Value>0)
    //    m_OutPutValue=1;
    //else
    //    m_OutPutValue=0;

    //if(m_OutPutValue)
	if(Value>0)
    {
        temp = (1<<CH_Number);
        CPLD_DO.Reg |= temp;
        //CPLD_FUNC::Write(NBUS_CPLD2_DO,CPLD_DO.Reg);
        //ARBUS_DELAY(10);
        ChannelInfo_Manager::BuildIn_DO[0][CH_Number] = 1;
    }
    else
    {
        temp = ~(1<<CH_Number);
        CPLD_DO.Reg &= temp;
        //CPLD_FUNC::Write(NBUS_CPLD2_DO,CPLD_DO.Reg);
        //ARBUS_DELAY(10);
        ChannelInfo_Manager::BuildIn_DO[0][CH_Number] = 0;
    }
	CPLD_FUNC::Write(NBUS_CPLD2_DO,CPLD_DO.Reg);
    ARBUS_DELAY(10);

    return 1;
}
char LBT21084VB_Base::Read_UnitDI(unsigned long CH_Number,unsigned long DI_Idx,unsigned long Value)
{
	char ReturnValue=0;
	unsigned short int temp = 0;
    if(DI_Idx >= MAX_BUILDIN_DI_COUNT)
      return 0;

    temp =  CPLD_FUNC::Read(NBUS_CPLD2_DI);
	temp &= 0X00FF;
	temp = (temp>>CH_Number) & 0X0001;
	ReturnValue= temp;
	
	return ReturnValue;
}
void LBT21084VB_Base::UnitDI_ReadAll(void)
{
	unsigned short int temp = CPLD_FUNC::Read(NBUS_CPLD2_DI);
    CPLD_DI.Reg = temp;
    temp  &= 0X00FF;
	//161125 zyx
    for(unsigned char uc_i=0;uc_i<8;uc_i++)  //8为最大通道数
	{
        if((temp>>uc_i)& 0X0001)
        {
            if(Sample_Pro::Digital_SampleCounter[uc_i][0]>3)	
            {
                ChannelInfo_Manager::MetaVariable2[uc_i][MetaCode_PV_UNIT_DI1].m_fValue=1;  //modified by xp //zyx 161205
                Sample_Pro::Digital_SampleCounter[uc_i][0]=3;
            }
            Sample_Pro::Digital_SampleCounter[uc_i][0]++;
        }
        else
        {
            Sample_Pro::Digital_SampleCounter[uc_i][0]=0;
            ChannelInfo_Manager::MetaVariable2[uc_i][MetaCode_PV_UNIT_DI1].m_fValue=0;
        }
	}
	//161125 zyx
}
char LBT21084VB_Base::UnitDO_Reset(unsigned long CH_Number,unsigned long Value)
{
    for(unsigned char uc_i=0;uc_i<MAX_BUILDIN_DI_COUNT;uc_i++)
	    OUTPUT_UnitDO(0,uc_i,Value);
	return 1;
}

char  LBT21084VB_Base::RedundantSafety(unsigned long CH_Number,unsigned long type)
{
//#if(REDUNDANT_VOLT_MODE == REDUNDANT_M0)
    if(board_config::Feature_Enable_init->RedundantVoltMode == REDUNDANT_M0)	
	    return (safety_check::RedundantSafetyFlg[CH_Number]);
//#else
    else
	//char temp;
	//temp=ControlPin::ReadMe(Pin_RedundantSafety);
	//if(temp)
	//	return 0;
	//else
	//	return 1;
        return (ControlPin::ReadMe(LBT21084VB_Base::PinGroup[LBT21084VB_Pin_RedundantSafety]) == 0? 0 :1);
//#endif	

}

char LBT21084VB_Base::ReleaseRedundantSafety(unsigned long uc_ChannelNo,unsigned long type)
{
	//Hardware can't set the voltage relay.
//#if(REDUNDANT_VOLT_MODE == REDUNDANT_M0)
    if(board_config::Feature_Enable_init->RedundantVoltMode == REDUNDANT_M0)	
	//M0RedundantFunc::ReleaseSafetyCheck(uc_ChannelNo,(unsigned char)type);
    //safety_check::ReleaseRedundantSafetyFlgCheck(uc_ChannelNo);
	//safety_check::RedundantSafetyFlg[uc_ChannelNo]=0;
	//M0RedundantFunc::m_bClampValid[uc_ChannelNo] = false;
	//return 0;
	    return (excheck::M0ReleaseRedundantSafety(uc_ChannelNo,type));
//#else
    else
	    return 1;
//#endif
}
 
//#if(FAN_SAFETY_MODE==1)
char LBT21084VB_Base::FanCheckSafety(unsigned long CH_Number,unsigned long type)
{
    //char temp;
    //temp=ControlPin::ReadMe(PinGroup[LBT21084VB_Pin_FanCheckSignal]);
    //if(temp)
    //    return 0;
    //else
    //    return 1;
	return (excheck::ExFanCheckSafety(PinGroup[LBT21084VB_Pin_FanCheckSignal]));	
}
//#endif 

char LBT21084VB_Base::FanAdjOperate(unsigned long DutyRatio,unsigned long FanAdjMod)
{

    if(FanAdjMod!=PWMMOD)
    {
        board_config::bFanAdj=0;
        return 0;
    }
    //float FanCtrlValue[8];
	for(unsigned char uc_i=0;uc_i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;uc_i++)
	{
		  float FanCtrlValue[8]={50,50,50,60,70,80,90,100};
			float V_High = ChannelInfo_Manager::BoardCFG.m_Channel[uc_i].m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High;
	    float I_Low = ChannelInfo_Manager::BoardCFG.m_Channel[uc_i].m_IRange[ChannelInfo_Manager::ChannelInfo[uc_i].m_ucCurrentRange].m_Bound.m_Low;
	  
	    FanAdj_task::ChannelPerAdjPower[uc_i] = (FanAdj_task::VR-V_High)*I_Low;		
			switch(ChannelInfo_Manager::ChannelInfo[uc_i].m_ucCurrentRange)
			{
				case CURRENT_RANGE_2:
				case CURRENT_RANGE_3:
            for(unsigned char uc_j=0;uc_j<8;uc_j++)
            FanCtrlValue[uc_j] = 50;
						if(ChannelInfo_Manager::BoardCFG.m_Channel[uc_i].m_IRange[ChannelInfo_Manager::ChannelInfo[uc_i].m_ucCurrentRange].m_Bound.m_High>0.5f)
						{
							FanCtrlValue[6]=50;
							FanCtrlValue[7]=50;
						}
						break;
				case CURRENT_RANGE_4:
				       for(unsigned char uc_j=0;uc_j<8;uc_j++)
                            FanCtrlValue[uc_j] = 50;
						break;
				default:
						break;
			}
			FanAdj_task::Channel_Duty_Ratio[uc_i]=FanCtrlValue[FanAdj_task::Channel_m_Level[uc_i]];//获得各通道对应风扇速度
  }

  FanAdj_task::MAX_Ratio = 50;//先设成最小
	for(unsigned char uc_i=0;uc_i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;uc_i++)//寻找各通道最大风扇速度
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
