
#include "../../Entry/includes.h"

#define  _IBT31162_VERSION_A_         0         //this is a temperory MACRO,and valish somedays later

#define  IBT31162_FINE_TUNE_STARTTIME          0.1f
#define  IBT31162_FINE_TUNE_MINERRORFACTOR     2E-5
#define  IBT31162_FINE_TUNE_STEPFACTOR         6E-6
#define  IBT31162_FINE_TUNE_MAXADJUSTFACTOR    0.03f  //  20140617 微调

#define  IBT31162_SETTING_TIME                 1000       //电流上升沿时间，单位为100us

#if HARDWARE_SDRAM_32M == 1
    #pragma arm section rwdata = "SRAM",zidata = "SRAM"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
    #pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif  
St_DisposableTask IBT31162_Base::SetCtrlOpenCh[5];
St_DisposableTask IBT31162_Base::SetCtrlCloseCh[3];
St_DisposableTask IBT31162_Base::SetCCToCC[5];    //加入了多range切换，所以这里扩充到10步
St_DisposableTask IBT31162_Base::SetRstToOff[2];
St_DisposableTask IBT31162_Base::SetCtrlOpenAcim[1];
St_DisposableTask IBT31162_Base::SetCtrlCloseAcim[1];

SSPI_Para  IBT31162_Base::IBT31162_SSP0DAC;
SSPI_Para  IBT31162_Base::IBT31162_SSP0VCDAC;    //VCLAMP
SSPI_Para  IBT31162_Base::IBT31162_SSP1ADC;

unsigned short int     IBT31162_Base::AD8450_FAULT;    //  AD8450 故障信号 0:报警
#pragma arm section 

const St_Port IBT31162_Base::PinGroup[IBT31162_Total_Pins] =	//注意，此处的管脚顺序必须和头文件中enum的管脚顺序高度一致
{//Port     Bits			PortType					  Function		  Address 
	{2,				12,				eCPTYPE_MCUGPIO,		GPIO_I,				0},	//Pin_ADCDRDYC              BUSY1   //  ADC  AD7175-2
	{2,				13,				eCPTYPE_MCUGPIO,		GPIO_I,				0},	//Pin_ADCDRDYV              BUSY2
	{1,				28,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_ADCSTART              CONV
	{1,				21,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_CS_C                  DCS1
	{0,				6,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_CS_V                  DCS2

	{1,				0,				eCPTYPE_CPLD,			CPLD_O,				NBUS_WRITE_DCS}, //Pin_DacCS3    DCS3   // DAC  AD5676R
	{1,				3,				eCPTYPE_CPLD,			CPLD_O,				NBUS_WRITE_DCS}, //Pin_DacCS5    DCS5  
	{1,				25,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_DacRst                 DAC-RST
	{1,				3,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_DacLoad                DAC-LOAD1

	{1,				1,				eCPTYPE_CPLD,			CPLD_O,				NBUS_WRITE_DCS},	//Pin_Clamp_Dac_CS   DCS4   DAC AD5686R
	{1,				19,				eCPTYPE_MCUGPIO,		GPIO_O,				1},	//Pin_Clamp_Dac_Load                 DAC-LOAD3

	{0,				13,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_WD      WI
	//{2,				26,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_WO      WO

//	{0,				15,				eCPTYPE_MCUGPIO,		SSP0_SCK,			0},	//Pin_SCK0
//	{0,				17,				eCPTYPE_MCUGPIO,		SSP0_MISO,		    0},	//Pin_MISO0
//	{0,				18,				eCPTYPE_MCUGPIO,		SSP0_MOSI,		    0},	//Pin_MOSI0
	{0,				7,				eCPTYPE_MCUGPIO,		SSP1_SCK,			0},	//Pin_SCK1
	{0,				8,				eCPTYPE_MCUGPIO,		SSP1_MISO,		    0},	//Pin_MISO1
	{0,				9,				eCPTYPE_MCUGPIO,		SSP1_MOSI,		    0},	//Pin_MOSI1

	{2,				3,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_PowerOn          P2.3   Relay Buffer

	{1,				2,				eCPTYPE_CPLD,			CPLD_O,				NBUS_WRITE_DCS},	//Pin_AdgRst
    
	{3,				18,				eCPTYPE_MCUGPIO,		PWM0,				0},	//Pin_FAN          Fan_C
	{3,				19,				eCPTYPE_MCUGPIO,		GPIO_O,				0},	//Pin_FANON        Fan_ON
 
    {0,				11,				eCPTYPE_MCUGPIO,		GPIO_O,				0}, //Pin_FO_INVT      FO_INVT   
    {0,				10,				eCPTYPE_MCUGPIO,		GPIO_I,				0}, //Pin_FI_INVT      FI_INVT 
    {1,				11,				eCPTYPE_MCUGPIO,		GPIO_I,				0}, //Pin_P_OFF        P_OFF
	{3,				17,				eCPTYPE_MCUGPIO,		GPIO_I,				0}, //Pin_T_ALARM      T_ALARM
//#if(FAN_SAFETY_MODE==1)
	{3,				16,				eCPTYPE_MCUGPIO,		GPIO_I,				0}, //Pin_FanCheckSignal, 风扇运行与否的检测引脚   Fan_A
//#endif
};

//#if(FAN_SAFETY_MODE==1)
MsTime     IBT31162_Base::FanOverTime;
//#endif
volatile St_IBT31162_Relay IBT31162_Base::IBT31162_relaystatus[MAXCHANNELNO];    //记录自身的relay状态
volatile St_IBT31162_PGA   IBT31162_Base::IBT31162_PGA_IVA; 
volatile unsigned char     IBT31162_Base::IBT31162_PGA_DA; 


const unsigned char IBT31162_Base::m_bHaveVoltageDac = 0;
const unsigned char IBT31162_Base::m_bHaveClampVdac = 1;
const unsigned char IBT31162_Base::m_bChannelBasedClampVdac = 0;



void IBT31162_Base::Avoid_Hot(void)
{
 // 避免AD8450芯片发热    
     FM25L256Func::MyInit();
     CConfig::ReadContent_ForFRAM((unsigned char *)&ChannelInfo_Manager::m_st_BoadType, FRAM_IV_BOARDTYPE, sizeof(IV_BOARDTYPE_CFG));
      
     if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_IBT31162)    //  20160622
	{
        ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount = 16;
        IBT31162_Base::IO_Init();    //内部IO初始化
        ControlPin::SetMeHigh(IBT31162_Base::PinGroup[IBT31162_Pin_WD]);
        TIMERSFunc::DELAY_SYS(200, 1);
        ControlPin::SetMeLow(IBT31162_Base::PinGroup[IBT31162_Pin_WD]);

        CPLD_FUNC::Init();
        for(unsigned char ChannelIdx = 0;ChannelIdx<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;ChannelIdx++)
        {
            IBT31162_Base::IBT31162_relaystatus[ChannelIdx].Bits.R_MODE = 1;   //  初始化为充电模式
            CPLD_FUNC::WriteDOBus(ChannelIdx,IBT31162_Base::IBT31162_relaystatus[ChannelIdx].Reg);
            TIMERSFunc::DELAY_SYS( 100, 1 );	//delay 100ms
        }
	}   
}

void IBT31162_Base::Hardware_Init(void)
{  
	//unsigned char temp;
    IBT31162_FeatureEnable_Init();
	Step_Manager_task::Unipolar.BoardFlag = 1;
    AD8450_FAULT = 0XFFFF;
    for(unsigned char ChannelIdx = 0;ChannelIdx<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;ChannelIdx++)
	{
        Step_Manager_task::Unipolar.Charge[ChannelIdx] = 1;   //  初始化为充电模式
        //Step_Manager_task::Unipolar.ModeChange[ChannelIdx] = 1;
        Step_Manager_task::Unipolar.EngineEnable[ChannelIdx] = UNIPOLAR_INTERNCHANGE_START;
        Step_Manager_task::Unipolar.EngineEnable[ChannelIdx] = 0;
        Step_Manager_task::Unipolar.EngineInterval[ChannelIdx] = OS_ServeProcess::OS_Time;
	}

	IO_Init();    //内部IO初始化
	//Feed_CpldWD();  // 喂狗信号
	board_config::bHaveRedundantCheck = 1;    //1;  modify by qjm20140922
	board_config::ClampH_Outputflag = 0;
	board_config::ClampL_Outputflag = 0;
	
	//UnitDO_Reset(0,0);
	
	CPLD_FUNC::Init();
	memset((void *)(&IBT31162_relaystatus), 0, sizeof(IBT31162_relaystatus));
		
	IBT31162_PGA_DA = 0;
  cpu_iowr_16(NBUS_WRITE_PGA_GAIN,IBT31162_PGA_DA);
  IBT31162_PGA_IVA.Bits.AdcGain_I = IBT31162_PGA_G2  ;   // IA0在最高位，大头序     高量程PGA281的增益为2
  IBT31162_PGA_IVA.Bits.AdcGain_V = IBT31162_PGA_G1  ;   // VA0在最高位，大头序     电压PGA281的增益为1

	for(unsigned char ChannelIdx = 0;ChannelIdx<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;ChannelIdx++)
	{
        IBT31162_relaystatus[ChannelIdx].Bits.R_MODE = 1;   //  初始化为充电模式
//		IBT31162_relaystatus[ChannelIdx].Bits.R_OUT = 1;	//LIANGLIANG
        CPLD_FUNC::WriteDOBus(ChannelIdx,IBT31162_relaystatus[ChannelIdx].Reg);
        TIMERSFunc::DELAY_SYS( 100, 1 );	//delay 100ms
        IBT31162_ADC5DAC::Dac_out_Cur(ChannelIdx,OS_LogicProcess::Set_DAC_Output_MIN);   // I_SET output 0
		    CPLD_FUNC::WriteDOBus(16+ChannelIdx,IBT31162_PGA_IVA.Reg); 
	}

    
	//Feed_CpldWD();  // 喂狗信号

	board_config::bHaveRedundantCheck = 1;
	board_config::ClampH_Outputflag = 0;
	board_config::ClampL_Outputflag = 0;	
	//SSP0 for DAC    AD5676
	IBT31162_SSP0DAC.uc_Module_Name = SSP0_DEVICE1;    //name
	IBT31162_SSP0DAC.uc_Speed = 7;//7;
	IBT31162_SSP0DAC.uc_CPOL = 0;
	IBT31162_SSP0DAC.uc_CPHA = 1;
	IBT31162_SSP0DAC.uc_MSTR = 1;    //master
	IBT31162_SSP0DAC.uc_BITS = 8;
	IBT31162_SSP0DAC.uc_LSBF = 0;
  	//SSP0 for DAC    AD5686
 	IBT31162_SSP0VCDAC.uc_Module_Name = SSP0_DEVICE3;    //name
	IBT31162_SSP0VCDAC.uc_Speed = 7;//5;
	IBT31162_SSP0VCDAC.uc_CPOL = 0;
	IBT31162_SSP0VCDAC.uc_CPHA = 1;
	IBT31162_SSP0VCDAC.uc_MSTR = 1;    //master
	IBT31162_SSP0VCDAC.uc_BITS = 8;
	IBT31162_SSP0VCDAC.uc_LSBF = 0;
	
	//SSP1 for ADC  ADS7175
	IBT31162_SSP1ADC.uc_Module_Name = SSP1_DEVICE1;    //name
	IBT31162_SSP1ADC.uc_Speed = 7; // 1: SPI 1 MHz 
	IBT31162_SSP1ADC.uc_CPOL = 1;
	IBT31162_SSP1ADC.uc_CPHA = 1;       //rising trig
	IBT31162_SSP1ADC.uc_MSTR = 1;    //master
	IBT31162_SSP1ADC.uc_BITS = 8;
	IBT31162_SSP1ADC.uc_LSBF = 0;
	
	TIMERSFunc::DELAY_SYS( 30000, 0 );	//delay 5ms
	//Feed_CpldWD();  // 喂狗信号
	
	DAC_Init();

	ControlPin::ReInitPullUP();
	IBT31162_Base::changeADCRange(CURRENT_RANGE_1);
	//Feed_CpldWD();  // ?11·D?o?
	CPLD_FUNC::Write(NBUS_WRITE_VOLTAGERELAY,0xFF);//电压控制开   modify by qjm 20140922

	OS_LogicProcess::InitBoardEngine(	&SetCtrlOpenCh[0],	sizeof(SetCtrlOpenCh)		/		sizeof(St_DisposableTask),
		&SetCtrlCloseCh[0], sizeof(SetCtrlCloseCh)	/		sizeof(St_DisposableTask),
		&SetCCToCC[0], 			sizeof(SetCCToCC)				/		sizeof(St_DisposableTask),
		&SetRstToOff[0],		sizeof(SetRstToOff)			/		sizeof(St_DisposableTask)
		);

	AdditionalEngine();	//Init ACIM function

	////////拉高钳压     // 20140613  电压保护
	IBT31162_Base::DAC_out(0,DAC_OUT_PUT_MAX,ACTIVE_DAC_VH);
//IBT31162_Base::DAC_out(1,DAC_OUT_PUT_8450MIN,ACTIVE_DAC_VL);
	IBT31162_Base::DAC_out(0,0,ACTIVE_DAC_VL);
	
//#if(FAN_SAFETY_MODE==1)
    if(board_config::Feature_Enable_init->Fan_Safety_Mode==1)
	{
	// 风扇初始化 
	///////风扇调速  初始化时设定为100%
	MsTime MyMsTime;
	unsigned long long TimeTicks;
	MyMsTime.Second =IBT31162_Base::FanOverTime.Second;
	MyMsTime.Us100 = IBT31162_Base::FanOverTime.Us100;
	safety_check::Fan_FaultCheckFlag = 0;
// 	while(1)
// 	{
//        if(PCA9501::SetExpander(34,1))
//        {			
//            I2CBusNew::AD5245_SET(0,0,I2C2); 
//            PCA9501::SetExpander(34,0);
// // 			temp =ControlPin::ReadMe(PinGroup[IBT31162_Pin_FanCheckSignal]);
// // 			if(temp==0)
// // 				safety_check::Fan_FaultCheckFlag=1;
// // 			else
//                safety_check::Fan_FaultCheckFlag=0; 
//            break;
//        }
//        else
//        {
// 			TimeTicks = CommonFunc::GetTimeTicks(&MyMsTime);
//            if(TimeTicks >= 1000)       
//            {
// // 				temp =ControlPin::ReadMe(PinGroup[IBT31162_Pin_FanCheckSignal]);
// // 				if(temp==0)
// // 					safety_check::Fan_FaultCheckFlag=1;
// // 				else
//                    safety_check::Fan_FaultCheckFlag=0;
//                break;
//            }
//            else 
//                continue;
//        }
// 		}	
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
    
    ADC_Init();
}

void IBT31162_Base::IBT31162_FeatureEnable_Init(void) 
{
	static St_Hardware_Feature_Enable para;
	memset(&para,0,sizeof(para));
    
	OS_LogicProcess::Set_DAC_Output_MIN = DAC_8450MIN_PWM; // 设置一个微值ISET提升过死区
	if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_20)
	{
	    para.Mcu_PWMFan_Enable=1;
		para.Power_Off_Enable=1;
		para.Fan_Safety_Mode=1;
        OS_LogicProcess::Set_DAC_Output_MIN = DAC_8450MIN_LINE; // 设置一个微值ISET提升过死区
	}
    else if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {
		para.Power_Off_Enable=1;
        ADCFunc::Init();
    }
	para.OS_Tick_Interval=800;
	board_config::Feature_Enable_init=&para;
} 

void IBT31162_Base::IO_Init(void)
{
	
	for(int i=0;i<IBT31162_Total_Pins;i++)
	{
		ControlPin::SetIObyCP(PinGroup[i]);
	}
} 

void IBT31162_Base::ADC_Init(void)
{	
	IBT31162_ADC5DAC::Adc_Init();

}
void IBT31162_Base::DAC_Init(void)
{
    IBT31162_ADC5DAC::Dac_Init();
    board_config::DacInfo.m_bCurrRngCount = 4;
    board_config::DacInfo.m_bVoltRngCount = 1;
	board_config::DacInfo.m_bHaveVoltageDac = m_bHaveVoltageDac;
	board_config::DacInfo.m_bHaveClampVdac = m_bHaveClampVdac;
	board_config::DacInfo.m_bChannelBasedClampVdac = m_bChannelBasedClampVdac;
	board_config::DacInfo.m_uSettingTime = IBT31162_SETTING_TIME;
	TIMERSFunc::DELAY_SYS( 10000, 0 );	//delay 5ms	
}

void IBT31162_Base::Software_Init(void)
{
	Func_Init();//函数指针初始化，四个基本操作
	EngineStuff();   //引擎结构指针初始化

// 	for(unsigned char i = 0; i < MAXCHANNELNO; i++)    //从0开始
// 	{
// 		regCHNInfo(i,board_config::st_channel_info[i].Channel_type);//注册chn info
// 		regEngine(i,board_config::st_channel_info[i].Channel_type);//注册引擎			
// 	}
	board_config::Accuracy = 0.001;
	
	Step_Manager_task::m_CurFineTuneFactor.m_fStartTime = IBT31162_FINE_TUNE_STARTTIME;
	Step_Manager_task::m_CurFineTuneFactor.m_fMinError  = IBT31162_FINE_TUNE_MINERRORFACTOR;    //  20140617 微调
	Step_Manager_task::m_CurFineTuneFactor.m_fMaxAdjust = IBT31162_FINE_TUNE_MAXADJUSTFACTOR;
	Step_Manager_task::m_CurFineTuneFactor.m_fStep      = IBT31162_FINE_TUNE_STEPFACTOR;

	IBT31162_SampleManage::Init();
}

void IBT31162_Base::Func_Init(void)
{
// 	board_config::Func_Adc_rd=&IBT31162_Base::ADC_rd;
	board_config::Func_Dac_out=&IBT31162_Base::DAC_out;
	board_config::Func_SetRange=&IBT31162_Base::SetRange;
	board_config::Func_SetRelay=&IBT31162_Base::SetRelay;
	board_config::Func_Sample = &IBT31162_SampleManage::SampleISR;

	board_config::Func_FanAdj=&IBT31162_Base::FanAdjOperate;
    //#if(FAN_SAFETY_MODE==1)
    if(board_config::Feature_Enable_init->Fan_Safety_Mode==1)
	board_config::Func_FanSafety= &IBT31162_Base::FanCheckSafety; //风扇保护
    //#endif	
	board_config::FeedWatchDog = &Feed_CpldWD;	
}
void IBT31162_Base::AdditionalEngine()
{
	// ACIM Engine
	OS_LogicProcess::SetCtrlAcimOn.st_Disposabletask_ptr = &IBT31162_Base::SetCtrlOpenAcim[0];
	OS_LogicProcess::SetCtrlAcimOn.stepLen = sizeof(SetCtrlOpenAcim) / sizeof(St_DisposableTask);
	
	OS_LogicProcess::SetCtrlAcimOff.st_Disposabletask_ptr = &IBT31162_Base::SetCtrlCloseAcim[0];
	OS_LogicProcess::SetCtrlAcimOff.stepLen = sizeof(SetCtrlCloseAcim) / sizeof(St_DisposableTask);
}
void IBT31162_Base::EngineStuff(void)
{
	memset(&SetCtrlOpenCh,   0,    sizeof(SetCtrlOpenCh));
	memset(&SetCtrlCloseCh,  0,    sizeof(SetCtrlCloseCh));
	memset(&SetCCToCC,       0,    sizeof(SetCCToCC));
	memset(&SetRstToOff,     0,    sizeof(SetRstToOff));
	memset(&SetCtrlOpenAcim,	0,    sizeof(SetCtrlOpenAcim));
	memset(&SetCtrlCloseAcim,	0,    sizeof(SetCtrlCloseAcim));

    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {
        //  开通道
        SetCtrlOpenCh[0].uw32_DelayTime = 200;
        SetCtrlOpenCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_NEAR_ZERO;
	    SetCtrlOpenCh[0].RelayMove.ActiveFlag = IBT31162_RLY_R_MODE;
	    SetCtrlOpenCh[0].RelayMove.ActiveFlag |= IBT31162_RLY_R_OUT;

        SetCtrlOpenCh[1].uw32_DelayTime = 150;
        SetCtrlOpenCh[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
        //SetCtrlOpenCh[1].RelayMove.InactiveFlag = IBT31162_RLY_R_CH_EN;
     
        SetCtrlOpenCh[2].uw32_DelayTime = 100;
        SetCtrlOpenCh[2].DacMove.DacFlag = ACTIVE_DAC_VH | ACTIVE_DAC_VL;
        //SetCtrlOpenCh[2].RelayMove.ActiveFlag = IBT31162_RLY_R_MODE;
        
        SetCtrlOpenCh[3].uw32_DelayTime = 300;//50;
        //SetCtrlOpenCh[3].RelayMove.ActiveFlag = IBT31162_RLY_R_OUT;
			  SetCtrlOpenCh[3].RelayMove.ActiveFlag |= IBT31162_RLY_R_CH_EN;
        
        SetCtrlOpenCh[4].uw32_DelayTime = 100;
        SetCtrlOpenCh[4].DacMove.DacFlag = ACTIVE_DAC_CUR;

        
        // 电流到电流切换
        SetCCToCC[0].uw32_DelayTime = 100;
        SetCCToCC[0].DacMove.DacFlag = ACTIVE_DAC_CUR_NEAR_ZERO;
        SetCCToCC[0].RangeChangedFlag = 1;
        
        SetCCToCC[1].uw32_DelayTime = 150;
        SetCCToCC[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
        SetCCToCC[1].RangeChangedFlag = 1;
        
        SetCCToCC[2].uw32_DelayTime = 5;  // 50=5ms
        SetCCToCC[2].RelayMove.ActiveFlag = IBT31162_RLY_R_MODE;
        SetCCToCC[2].RelayMove.InactiveFlag = IBT31162_RLY_R_CH_EN;
        SetCCToCC[2].RangeChangedFlag = 1;   // RangeChangedFlag 0时必定执行，1时可不执行。

        SetCCToCC[3].uw32_DelayTime = 200;
        SetCCToCC[3].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;
        SetCCToCC[3].RelayMove.ActiveFlag = IBT31162_RLY_R_CH_EN;
        
        SetCCToCC[4].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;
    }
    else
    {
         //  开通道
        SetCtrlOpenCh[0].uw32_DelayTime = 100;
		SetCtrlOpenCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_NEAR_ZERO;
        SetCtrlOpenCh[0].RelayMove.ActiveFlag = IBT31162_RLY_R_MODE;
        

        SetCtrlOpenCh[1].uw32_DelayTime = 100;
        SetCtrlOpenCh[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
        SetCtrlOpenCh[1].RelayMove.ActiveFlag = IBT31162_RLY_R_OUT;
     
        SetCtrlOpenCh[2].uw32_DelayTime = 100;
        SetCtrlOpenCh[2].DacMove.DacFlag = ACTIVE_DAC_VH | ACTIVE_DAC_VL;
        SetCtrlOpenCh[2].RelayMove.ActiveFlag = IBT31162_RLY_R_CH_EN;
        
        SetCtrlOpenCh[3].uw32_DelayTime = 2000;
        SetCtrlOpenCh[3].DacMove.DacFlag = ACTIVE_DAC_CUR_NEAR_ZERO;

        SetCtrlOpenCh[4].uw32_DelayTime = 50;
        SetCtrlOpenCh[4].DacMove.DacFlag = ACTIVE_DAC_CUR;
        
        // 电流到电流切换
        SetCCToCC[0].uw32_DelayTime = 100;
        SetCCToCC[0].DacMove.DacFlag = ACTIVE_DAC_CUR_NEAR_ZERO;
        SetCCToCC[0].RangeChangedFlag = 1;
        
        SetCCToCC[1].uw32_DelayTime = 150;
        SetCCToCC[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
        SetCCToCC[1].RangeChangedFlag = 1;
        
        SetCCToCC[2].uw32_DelayTime = 50;
		SetCCToCC[2].DacMove.DacFlag = ACTIVE_DAC_CUR_NEAR_ZERO;  //++
       
		SetCCToCC[3].uw32_DelayTime = 600;
		SetCCToCC[3].RelayMove.ActiveFlag = IBT31162_RLY_R_MODE;
        
        SetCCToCC[4].uw32_DelayTime = 300;
        SetCCToCC[4].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

    }
      
    // 关通道
	SetCtrlCloseCh[0].uw32_DelayTime = 100;
	//SetCtrlCloseCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_NEAR_ZERO| ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
    
	if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_20)
	{
	     SetCtrlCloseCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
	}
    else
    {
		SetCtrlCloseCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_NEAR_ZERO;
    }

	SetCtrlCloseCh[1].uw32_DelayTime = 50;
	SetCtrlCloseCh[1].RelayMove.InactiveFlag = IBT31162_RLY_R_OUT;

	SetCtrlCloseCh[2].uw32_DelayTime = 50;
	SetCtrlCloseCh[2].RelayMove.InactiveFlag = IBT31162_RLY_R_CH_EN | IBT31162_RLY_R_MODE;

    // 休息到关闭切换
	SetRstToOff[0].uw32_DelayTime = 100;
	SetRstToOff[0].RelayMove.InactiveFlag = IBT31162_RLY_R_OUT;  //为防止并联时从通道状态被意外设置成CCS_OFF ,保险起见这个地方也处理继电器关闭  dirui 2015/09/16
	//SetRstToOff[0].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT; //需要放开clamp;

	//SetRstToOff[1].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
	SetRstToOff[1].RelayMove.InactiveFlag = IBT31162_RLY_R_CH_EN;
		
	// ACIM切换	
	SetCtrlOpenAcim[0].uw32_DelayTime = 100;
	SetCtrlOpenAcim[0].RelayMove.ActiveFlag = IBT31162_RLY_R_ACIM;

	SetCtrlCloseAcim[0].uw32_DelayTime = 100;
	SetCtrlCloseAcim[0].RelayMove.InactiveFlag = IBT31162_RLY_R_ACIM;	
}
/******************************************************************************
** Function name:		DAC_out
**
** Descriptions:	        typeA DAC output
**			     
** parameters:			CH_Number:0->Max Channel Number; Data:32bit value
** Returned value:		0/1
** 
******************************************************************************/
char IBT31162_Base::DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type)
{ 
    char chn = (char)(ChannelIdx);
	unsigned long value = Data;    //强制保护
	
    switch(type)
	{
	    case ACTIVE_DAC_CUR:
//          float f_Value = Data - DAC_OUT_PUT_ZERO;	 //  双边变成单边 0V---2.5V
//          f_Value = abs(f_Value); 
//          Data = (unsigned long) f_Value;
	
			ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_u32DacValue = Data;    //此值为偏移码
            IBT31162_ADC5DAC::Dac_out_Cur(ChannelIdx,Data);
			break;

		//comment out chen
// 		case ACTIVE_DAC_VLT:
// 			IBT31162_ADC5DAC::Dac_out_Vlt(chn,value);
// 			break;
			///////////////////钳位
		case ACTIVE_DAC_VH:
			IBT31162_ADC5DAC::Dac_out_Clamp_Vlt(4,value);    //VOUTA - DACH
			break;
		case ACTIVE_DAC_VL:
			IBT31162_ADC5DAC::Dac_out_Clamp_Vlt(1,value);    //VOUTC - DACL
            break;
		default:
			break;
	}
	return 1;
}


char IBT31162_Base::SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange)
{
	switch(CurrRange)
	{
        case CURRENT_RANGE_1:
        case CURRENT_RANGE_3:
        case CURRENT_RANGE_4:    
			IBT31162_relaystatus[ChannelIdx].Bits.R_L = 0;
            break;
        case CURRENT_RANGE_2:
			IBT31162_relaystatus[ChannelIdx].Bits.R_L = 1;
			break;
				
	    case CURRENT_RANGE_5:      // 负向放电对应量程高、低
        case CURRENT_RANGE_7:
        case CURRENT_RANGE_8:  
			IBT31162_relaystatus[ChannelIdx].Bits.R_L = 0;
            break;
        case CURRENT_RANGE_6:
			IBT31162_relaystatus[ChannelIdx].Bits.R_L = 1;
			break;
				
		default:
			break;
	}
	CPLD_FUNC::WriteDOBus(ChannelIdx,IBT31162_relaystatus[ChannelIdx].Reg);
    
	return 1;
}
void IBT31162_Base::SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali)
{
	St_IBT31162_Relay MyRly;

	IBT31162_relaystatus[ChannelIdx].Bits.R_ACAL = bAutoCali;

	MyRly.Reg = SetRly;
    if(MyRly.Bits.R_CH_EN)
    {
		IBT31162_relaystatus[ChannelIdx].Bits.R_CH_EN = 1;  // AD8450使能
        //Step_Manager_task::Unipolar.ModeChange[ChannelIdx] = 1; 
    }
    
	if(MyRly.Bits.R_MODE)
    {
        //if(Step_Manager_task::Unipolar.BoardFlag)
        //{  
			
           if(Step_Manager_task::Unipolar.Charge[ChannelIdx])           
		       IBT31162_relaystatus[ChannelIdx].Bits.R_MODE = 1;  // 1：充电；0：放电
           else
               IBT31162_relaystatus[ChannelIdx].Bits.R_MODE = 0;  // 1：充电；0：放电
           
        //}
    }
   
	if(MyRly.Bits.R_ACIM)
	{
		IBT31162_relaystatus[ChannelIdx].Bits.R_ACIM = 1;
		CPLD_FUNC::WriteDOBus(ChannelIdx,IBT31162_relaystatus[ChannelIdx].Reg);
	}
    
	if(MyRly.Bits.R_OUT)
		IBT31162_relaystatus[ChannelIdx].Bits.R_OUT = 1;
    
	//CPLD_FUNC::WriteDOBus(ChannelIdx,IBT31162_relaystatus[ChannelIdx].Reg);
    
	MyRly.Reg = ClrRly;

	if(MyRly.Bits.R_OUT)
		IBT31162_relaystatus[ChannelIdx].Bits.R_OUT = 0;
	
	if(MyRly.Bits.R_CH_EN)
	    IBT31162_relaystatus[ChannelIdx].Bits.R_CH_EN = 0;
	if(MyRly.Bits.R_MODE)
    {      
	     Step_Manager_task::Unipolar.Charge[ChannelIdx] =1;
       IBT31162_relaystatus[ChannelIdx].Bits.R_MODE = 1;  // 1：充电；0：放电
    }
	
	if(MyRly.Bits.R_ACIM)
	{
		IBT31162_relaystatus[ChannelIdx].Bits.R_ACIM = 0;
		CPLD_FUNC::WriteDOBus(ChannelIdx,IBT31162_relaystatus[ChannelIdx].Reg);
	}
	
    CPLD_FUNC::WriteDOBus(ChannelIdx,IBT31162_relaystatus[ChannelIdx].Reg);

}
void IBT31162_Base::Adjust_ADC_Gain(char type,char chnType)
{
    St_IBT_PGA MyReg;
    
    MyReg.Bits.AdcGain_I = 0;
	MyReg.Bits.AdcGain_V = 0;
	MyReg.Bits.AdcGainEnable = 1;
	CPLD_FUNC::Write(NBUS_WRITE_PGA_GAIN,MyReg.Reg);

	MyReg.Bits.AdcGainEnable = 0;
	CPLD_FUNC::Write(NBUS_WRITE_PGA_GAIN,MyReg.Reg);

	MyReg.Bits.AdcGainEnable = 1;
	CPLD_FUNC::Write(NBUS_WRITE_PGA_GAIN,MyReg.Reg);
}

void IBT31162_Base::ADC_ADG_open(unsigned char ChannelIdx) 
{
	CPLD_FUNC::WriteADGBus(ChannelIdx,0x01);
}

void IBT31162_Base::ADG_Reset(void)
{
//     ControlPin::SetMeLow(IBT31162_Base::PinGroup[IBT31162_Pin_AdgRst]);
// 	ARBUS_DELAY(5);
// 	ControlPin::SetMeHigh(IBT31162_Base::PinGroup[IBT31162_Pin_AdgRst]);
}

void IBT31162_Base::changeADCRange(unsigned char range)
{
	ARBUS_DELAY(10);

	IBT31162_Base::Adjust_ADC_Gain(1,ADC_TYPE_I);
}

void IBT31162_Base::Feed_CpldWD(void)
{
    return;
    
// 	ControlPin::SetMeHigh(IBT31162_Base::PinGroup[IBT31162_Pin_WD]);
// 	TIMERSFunc::DELAY_SYS(200, 0);
// 	ControlPin::SetMeLow(IBT31162_Base::PinGroup[IBT31162_Pin_WD]);
}

 
//#if(FAN_SAFETY_MODE==1)
char IBT31162_Base::FanCheckSafety(unsigned long CH_Number,unsigned long type)
{
	return (excheck::ExFanCheckSafety(PinGroup[IBT31162_Pin_FanCheckSignal]));	
}
//#endif
char IBT31162_Base::FanAdjOperate(unsigned long DutyRatio,unsigned long FanAdjMod)
{

    if(FanAdjMod!=PWMMOD)
    {
        board_config::bFanAdj=0;
        return 0;
    }
   
	for(unsigned char uc_i=0;uc_i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;uc_i++)//计算各通道满功率
	{
	  float V_High = ChannelInfo_Manager::BoardCFG.m_Channel[uc_i].m_VRange[VOLTAGE_RANGE_HIGH].m_Bound.m_High;
	  float I_Low = ChannelInfo_Manager::BoardCFG.m_Channel[uc_i].m_IRange[ChannelInfo_Manager::ChannelInfo[uc_i].m_ucCurrentRange].m_Bound.m_Low;
	  
	  FanAdj_task::ChannelPerAdjPower[uc_i] = (FanAdj_task::VR-V_High)*I_Low;		
	}	
	for(unsigned char uc_i=0;uc_i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;uc_i++)
	{
		 float FanCtrlValue[8]={30,40,50,60,70,80,90,100};
			switch(ChannelInfo_Manager::ChannelInfo[uc_i].m_ucCurrentRange)
    {
				case CURRENT_RANGE_2:
				case CURRENT_RANGE_3:
                        for(unsigned char uc_j=0;uc_j<8;uc_j++)
                            FanCtrlValue[uc_j] = 30;
						if(ChannelInfo_Manager::BoardCFG.m_Channel[uc_i].m_IRange[ChannelInfo_Manager::ChannelInfo[uc_i].m_ucCurrentRange].m_Bound.m_High>0.5f)
						{
							FanCtrlValue[6]=40;
							FanCtrlValue[7]=40;
						}
            break;
				case CURRENT_RANGE_4:
				       for(unsigned char uc_j=0;uc_j<8;uc_j++)
                            FanCtrlValue[uc_j] = 30;
            break;
        default:
            break;
			}
			FanAdj_task::Channel_Duty_Ratio[uc_i]=FanCtrlValue[FanAdj_task::Channel_m_Level[uc_i]];//获得各通道对应风扇速度
  }

  FanAdj_task::MAX_Ratio = 30;//先设成最小
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
