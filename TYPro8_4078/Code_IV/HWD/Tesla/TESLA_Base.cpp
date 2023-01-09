
//---------------------------------------------------
// Version: TESLA   Instrument Version.    20130906
//   ͨ������:  16��ͨ��һ���
//   ÿ��ͨ����������:  1����(-5~5A)   pwmģʽ  noV--
//   ÿ��ͨ����ѹ����:  1����(0V~5V)     ��ѹpid
//
//---------------------------------------------------



//��Ϊ��Tesla��֧��
//���ư���ͬ
//ͨ����ļ̵�����ͬ
//comment out chen 20130407


#include "../../Entry/includes.h"

#define  TESLA_FINE_TUNE_STARTTIME          0.1f
#define  TESLA_FINE_TUNE_MINERRORFACTOR     2E-5
#define  TESLA_FINE_TUNE_STEPFACTOR         6E-6
#define  TESLA_FINE_TUNE_MAXADJUSTFACTOR    0.06f

#define  TESLA_ACCURACY                     0.0005f   //����
#define  TESLA_CTRL_VALUE_ERROR_FACTOR      100       //��������100��������Ϊ�ǿ���ֵ�쳣����

#define  TESLA_SETTING_TIME                 1000       //����������ʱ�䣬��λΪ100us

//ע�⣬ʵ���ϲ����ܿ���ͨ��    
//cpu_iowr_16(NBUS_WRITE_E_ADD, 0&chn );    //����CPLD����ʱADD�����
//�����й�CPLD ADD�Ĳ��������д��� comment out chen 20130106

/*Format
//�ṹ����  ÿ��ʱ����Ҫ����Сmove�����
// Disposable һ����
******************************************************************************/
//ʱ�� ������line384+

#if HARDWARE_SDRAM_32M == 1
    #pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x01F40000
#else
    #pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x03E80000
#endif  
St_DisposableTask TESLA_Base::SetCtrlOpenCh[5];
St_DisposableTask TESLA_Base::SetCtrlCloseCh[2];
St_DisposableTask TESLA_Base::SetCCToCC[2];
St_DisposableTask TESLA_Base::SetRstToOff[2]; 
//St_DisposableTask TESLA_Base::SetCVToCV[2];
St_DisposableTask TESLA_Base::VCaliWithNOEn_On[2];
St_DisposableTask TESLA_Base::VCaliWithNOEn_Off[2];

SSPI_Para  TESLA_Base::Telsa_SSP0VDAC;    //DAC
SSPI_Para  TESLA_Base::Tesla_SSP0VCDAC;    //VCLAMP
SSPI_Para  TESLA_Base::Tesla_SSP1IADC;     //�����ž��ܵ����費ռʱ�䣬������SSP��CS�ڣ�comment out chen 20130702
#pragma arm section 

// unsigned char TESLA_Base::SetCtrlOpenCh_Len = 5;
// unsigned char TESLA_Base::SetCtrlCloseCh_Len = 2;
// unsigned char TESLA_Base::SetCCToCC_Len = 2;
// unsigned char TESLA_Base::SetRstToOff_Len = 2;
// unsigned char TESLA_Base::SetCVToCV_Len = 2;
// unsigned char TESLA_Base::VCaliWithNOEn_On_Len = 2;
// unsigned char TESLA_Base::VCaliWithNOEn_Off_Len = 2;

const St_Port TESLA_Base::PinGroup[TESLA_Total_Pins] =	//ע�⣬�˴��Ĺܽ�˳������ͷ�ļ���enum�Ĺܽ�˳��߶�һ��
{//Port      Bits			  PortType					 Function		            Address            
	{1,        28,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{2,        12,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
	{2,        13,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
	{1,        21,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{0,        6,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{1,        26,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{1,        0,         eCPTYPE_CPLD,      CPLD_O,                NBUS_WRITE_DCS},
	{1,        25,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{1,        3,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{1,        2,         eCPTYPE_MCUGPIO,   GPIO_I,                0},
	{1,        27,        eCPTYPE_MCUGPIO,   GPIO_O,                1},
	{1,        1,         eCPTYPE_CPLD,      CPLD_O,                NBUS_WRITE_DCS},
	{1,        19,        eCPTYPE_MCUGPIO,   GPIO_O,                1},
	{1,        18,        eCPTYPE_MCUGPIO,   GPIO_O,                1},
	{3,        18,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
//	{0,        15,        eCPTYPE_MCUGPIO,   SSP0_SCK,              0},
//	{0,        17,        eCPTYPE_MCUGPIO,   SSP0_MISO,             0},
//	{0,        18,        eCPTYPE_MCUGPIO,   SSP0_MOSI,             0},
	{0,        7,         eCPTYPE_MCUGPIO,   SSP1_SCK,              0},
	{0,        8,         eCPTYPE_MCUGPIO,   SSP1_MISO,             0},
	{0,        9,         eCPTYPE_MCUGPIO,   SSP1_MOSI,             0},
	{2,        0,         eCPTYPE_MCUGPIO,   GPIO_I,             0},
	{1,        2,         eCPTYPE_CPLD,      CPLD_O,                NBUS_WRITE_DCS},
};

const unsigned char TESLA_Base::m_bHaveVoltageDac = 0;
const unsigned char TESLA_Base::m_bHaveClampVdac = 1;
// const unsigned char TESLA_Base::m_bChannelBasedClampVdac = 0;

volatile St_Tesla_Relay TESLA_Base::Tesla_relaystatus[MAXCHANNELNO];    //��¼�����relay״̬

void TESLA_Base::Hardware_Init(void)
{  
  TESLA_FeatureEnable_Init();
	IO_Init();    //�ڲ�IO��ʼ��
	Feed_CpldWD();  // ι���ź�

	board_config::bHaveRedundantCheck = 0;
	board_config::ClampH_Outputflag = 0;
	board_config::ClampL_Outputflag = 0;

	CPLD_FUNC::Init();
	memset((void *)(&Tesla_relaystatus[0]), 0, MAXCHANNELNO);  
	for (unsigned char i = 0;i<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;i++)
	{
		Tesla_relaystatus[i].Bits.Vin = 1;    //Vin����ͳ�Ƶ�relay_on��
	}

	Feed_CpldWD();  // ι���ź�

	//SSP0    comment out chen 20130702
	Telsa_SSP0VDAC.uc_Module_Name = SSP0_DEVICE1;    //name
	Telsa_SSP0VDAC.uc_Speed = 7;//7;
	Telsa_SSP0VDAC.uc_CPOL = 0;
	Telsa_SSP0VDAC.uc_CPHA = 1;
	Telsa_SSP0VDAC.uc_MSTR = 1;    //master
	Telsa_SSP0VDAC.uc_BITS = 8;
	Telsa_SSP0VDAC.uc_LSBF = 0;

	Tesla_SSP0VCDAC.uc_Module_Name = SSP0_DEVICE3;    //name
	Tesla_SSP0VCDAC.uc_Speed = 7;//5;
	Tesla_SSP0VCDAC.uc_CPOL = 0;
	Tesla_SSP0VCDAC.uc_CPHA = 1;
	Tesla_SSP0VCDAC.uc_MSTR = 1;    //master
	Tesla_SSP0VCDAC.uc_BITS = 8;
	Tesla_SSP0VCDAC.uc_LSBF = 0;

	//SSP1 for ADC
	Tesla_SSP1IADC.uc_Module_Name = SSP1_DEVICE1;    //name
	Tesla_SSP1IADC.uc_Speed = 5;
	Tesla_SSP1IADC.uc_CPOL = 0;
	Tesla_SSP1IADC.uc_CPHA = 0;       //rising trigg
	Tesla_SSP1IADC.uc_MSTR = 1;    //master
	Tesla_SSP1IADC.uc_BITS = 8;
	Tesla_SSP1IADC.uc_LSBF = 0;

	TIMERSFunc::DELAY_SYS( 30000, 0 );	//delay 5ms
	Feed_CpldWD();  // ι���ź�

	ADC_Init();
	DAC_Init();

	ControlPin::ReInitPullUP();

	Feed_CpldWD();  // ι���ź�
	TESLA_Base::Adjust_ADC_Gain(1,ADC_TYPE_I);     //ADCʹ�õ���
	TESLA_Base::Adjust_ADC_Gain(1,ADC_TYPE_V);    //
	//	TESLA_TempSample::AD_Init(8);   //����Ӳ������ͨ����ֵ add by qjm20140613
	OS_LogicProcess::InitBoardEngine(	&SetCtrlOpenCh[0],	sizeof(SetCtrlOpenCh)		/		sizeof(St_DisposableTask),
		&SetCtrlCloseCh[0], sizeof(SetCtrlCloseCh)	/		sizeof(St_DisposableTask),
		&SetCCToCC[0], 			sizeof(SetCCToCC)				/		sizeof(St_DisposableTask),
		&SetRstToOff[0],		sizeof(SetRstToOff)			/		sizeof(St_DisposableTask)
		);
	AdditionalEngine();

	TESLA_Base::PowerOn();    //�����PowerOn����

	////////����ǯѹ
	TESLA_Base::DAC_out(0,DAC_OUT_PUT_MAX,ACTIVE_DAC_VH);
	TESLA_Base::DAC_out(0,DAC_OUT_PUT_MIN,ACTIVE_DAC_VL);
}

void TESLA_Base::TESLA_FeatureEnable_Init(void) 
{
// 	static St_Hardware_Feature_Enable para;
// 	memset(&para,0,sizeof(para));
// 	
// // 	if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_B)
// // 	{
// 	para.Power_Off_Enable=1;
// 	para.OS_Tick_Interval=800;		
// //	}
// 	
// 	board_config::Feature_Enable_init=&para;
	  board_config::Feature_Enable_init.Power_Off_Enable =1;
    board_config::Feature_Enable_init.OS_Tick_Interval =200;
} 

void TESLA_Base::IO_Init(void)
{
	for(int i=0;i<TESLA_Total_Pins;i++)
		ControlPin::SetIObyCP(PinGroup[i]);
} 

void TESLA_Base::ADC_Init(void)
{	
	TESLA_ADC5DAC::Adc_Init();    //Tesla_ADC_Init
}

void TESLA_Base::DAC_Init(void)
{
	//CS5560    and    AD5754
	TESLA_ADC5DAC::Dac_Init();
	board_config::DacInfo.m_bCurrRngCount = 1;
	board_config::DacInfo.m_bVoltRngCount = 1;
	board_config::DacInfo.m_bHaveVoltageDac = m_bHaveVoltageDac;
	board_config::DacInfo.m_bHaveClampVdac = m_bHaveClampVdac;
// 	board_config::DacInfo.m_bChannelBasedClampVdac = m_bChannelBasedClampVdac;
	board_config::DacInfo.m_uSettingTime = TESLA_SETTING_TIME;

	TIMERSFunc::DELAY_SYS( 10000, 0 );	//delay 5ms
}

void TESLA_Base::Software_Init(void)
{
	Func_Init();//����ָ���ʼ��  �ĸ���������
	EngineStuff();   //����ṹָ���ʼ��

	// 	for(unsigned char i = 0; i < MAXCHANNELNO; i++)    //��0��ʼ
	// 	{
	// 		regCHNInfo(i,board_config::st_channel_info[i].Channel_type);//ע��chn info
	// 		regEngine(i,board_config::st_channel_info[i].Channel_type);//ע������			
	// 	}

	Step_Manager_task::m_CurFineTuneFactor.m_fStartTime = TESLA_FINE_TUNE_STARTTIME;
	Step_Manager_task::m_CurFineTuneFactor.m_fMinError  = TESLA_FINE_TUNE_MINERRORFACTOR;
	Step_Manager_task::m_CurFineTuneFactor.m_fMaxAdjust = TESLA_FINE_TUNE_MAXADJUSTFACTOR;
	Step_Manager_task::m_CurFineTuneFactor.m_fStep      = TESLA_FINE_TUNE_STEPFACTOR;        //  20140617 ΢��
	

		IR::Set_LowPulseDacTime=  TESLASETLOWPULSEDACTIME;
		IR::Set_LowPulseAdcTime=  TESLASETLOWPULSEADCTIME;
		IR::Set_HighPulseDacTime= TESLASETHIGHPULSEDACTIME;
		IR::Set_HighPulseAdcTime= TESLASETHIGHPULSEADCTIME;
		IR::Set_MinimumPulseTime= TESLAMINIMUMPULSE;

	safety_check::m_CtrlValueSetting.fErrorFactor       = TESLA_ACCURACY * TESLA_CTRL_VALUE_ERROR_FACTOR;

	board_config::Accuracy = TESLA_ACCURACY;

	TESLA_SampleManage::Init();
}

/******************************************************************************
** Function name:		Func_Init
**				
** Descriptions:	        INitialize  type_A::Func_Adc_rd/Func_Dac_out/
** 															      Func_Relay_on/Func_Relay_off
**			     
** parameters:			none
** Returned value:  none
** 
******************************************************************************/
void TESLA_Base::Func_Init(void)    //Tesla change
{
	// 	board_config::Func_Adc_rd=&TESLA_Base::ADC_rd;    //����ɾ�� 
	board_config::Func_Dac_out=&TESLA_Base::DAC_out;
	board_config::Func_SetRange=&TESLA_Base::SetRange;
	board_config::Func_SetRelay=&TESLA_Base::SetRelay;

	board_config::Func_Sample = &TESLA_SampleManage::SampleISR;    //����    �����жϴ�����
	//	board_config::Func_TempSample = &TESLA_TempSample::Temp_rd;   // 20140613  �¶Ȳ���

	board_config::FeedWatchDog = &TESLA_Base::Feed_CpldWD;
	
		IR::pIR_ADC = &TESLA_SampleManage::IR_ADC;
		IR::pRenew_IR_ADC = &TESLA_SampleManage::renew_IR_ADC;
		IR::pIR_Read_ADC = &TESLA_SampleManage::IR_Read_ADC;
	  PowerLostCheck::Func_PowerOffSafetCheck = &TESLA_Base::PowerOffCheck;
};

void TESLA_Base::AdditionalEngine(void)
{
	OS_LogicProcess::SetAutoCaliOpen.st_Disposabletask_ptr = &TESLA_Base::VCaliWithNOEn_On[0];
	OS_LogicProcess::SetAutoCaliOpen.stepLen = sizeof(VCaliWithNOEn_On)/sizeof(St_DisposableTask); //TESLA_Base::VCaliWithNOEn_On_Len;
	OS_LogicProcess::SetAutoCaliClose.st_Disposabletask_ptr = &TESLA_Base::VCaliWithNOEn_Off[0];
	OS_LogicProcess::SetAutoCaliClose.stepLen = sizeof(VCaliWithNOEn_Off)/sizeof(St_DisposableTask);//TESLA_Base::VCaliWithNOEn_Off_Len;
}
void TESLA_Base::EngineStuff(void)
{
	memset(&SetCtrlOpenCh,      0,    sizeof(SetCtrlOpenCh));
	memset(&SetCtrlCloseCh,     0,    sizeof(SetCtrlCloseCh));
	memset(&SetCCToCC,          0,    sizeof(SetCCToCC));
	memset(&SetRstToOff,        0,    sizeof(SetRstToOff));
	//	memset(&SetCVToCV,          0,    sizeof(SetCVToCV));
	memset(&VCaliWithNOEn_On,   0,    sizeof(VCaliWithNOEn_On));
	memset(&VCaliWithNOEn_Off,  0,    sizeof(VCaliWithNOEn_Off));

	// channel starting len = 5 modified by philip 11/6/2013
	SetCtrlOpenCh[0].uw32_DelayTime = 300;                                         //1 open PRT 30ms to 2
	SetCtrlOpenCh[0].RelayMove.ActiveFlag = TESLA_RLY_PRT;    
	SetCtrlOpenCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
	SetCtrlOpenCh[1].uw32_DelayTime = 1300;                                        //2 open EN 130ms to 3
	SetCtrlOpenCh[1].RelayMove.ActiveFlag = (TESLA_RLY_EN);    
	SetCtrlOpenCh[2].uw32_DelayTime = 600;                                         //3 open IH 60ms to 4
	SetCtrlOpenCh[2].RelayMove.ActiveFlag = (TESLA_RLY_IH);    
	SetCtrlOpenCh[3].uw32_DelayTime = 500;                                         //4 close PRT 50ms to 5
	SetCtrlOpenCh[3].RelayMove.InactiveFlag = (TESLA_RLY_PRT); 
	SetCtrlOpenCh[4].uw32_DelayTime = 100;                                           //5 Iset
	SetCtrlOpenCh[4].DacMove.DacFlag = ACTIVE_DAC_CUR | ACTIVE_DAC_VH | ACTIVE_DAC_VL;    //??Iset VH VL    comment out chen 20130416

	//channel stop
	SetCtrlCloseCh[0].uw32_DelayTime = 100;                            //1 Iset to zero  10ms to 2
	SetCtrlCloseCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
	SetCtrlCloseCh[1].uw32_DelayTime = 100;                             //2 close all relay
	SetCtrlCloseCh[1].RelayMove.InactiveFlag = (TESLA_RLY_EN | TESLA_RLY_IH );    //�򿪵�����ѹ�˼��

	//CCtoCC    
	//comment out chen 20130128  ��ʱû�м���ٵ�ADC�л�����
	SetCCToCC[0].uw32_DelayTime = 100;
	//SetCCToCC[1].uw32_DelayTime = 0;
	SetCCToCC[1].DacMove.DacFlag = ACTIVE_DAC_CUR | ACTIVE_DAC_VH | ACTIVE_DAC_VL;

	//---------------------------------------------//
	SetRstToOff[0].uw32_DelayTime = 100;
	//SetRstToOff[1].uw32_DelayTime = 0;

	// 	//CVtoCV   
	// 	SetCVToCV[0].uw32_DelayTime = 100;
	// 	SetCVToCV[1].uw32_DelayTime = 0;
	// 	SetCVToCV[1].DacMove.DacFlag = ACTIVE_DAC_CUR | ACTIVE_DAC_VH | ACTIVE_DAC_VL;

	/////
	VCaliWithNOEn_On[0].uw32_DelayTime = 100;
	VCaliWithNOEn_On[0].RelayMove.ActiveFlag = TESLA_RLY_IH;    //��һ����cali
	//VCaliWithNOEn_On[0].DacMove.DacFlag = 0;
	//VCaliWithNOEn_On[1].uw32_DelayTime = 0;
	VCaliWithNOEn_On[1].RelayMove.ActiveFlag = TESLA_RLY_IH; 
	//VCaliWithNOEn_On[1].DacMove.DacFlag = 0;

	VCaliWithNOEn_Off[0].uw32_DelayTime = 100;
	VCaliWithNOEn_Off[0].RelayMove.ActiveFlag = TESLA_RLY_IH;
	//VCaliWithNOEn_Off[0].DacMove.DacFlag = 0;
	//VCaliWithNOEn_Off[1].uw32_DelayTime = 0;
	//VCaliWithNOEn_Off[1].RelayMove.ActiveFlag = 0; 
	//VCaliWithNOEn_Off[1].DacMove.DacFlag = 0;
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
//Tesla change
char TESLA_Base::DAC_out(unsigned long CH_Number,unsigned long Data,unsigned long type)
{
	char chn = (char)(CH_Number);
	unsigned long value = Data;    //ǿ�Ʊ���

	switch(type)
	{
	case ACTIVE_DAC_CUR:
		ChannelInfo_Manager::ChannelInfo[CH_Number].m_u32DacValue = Data;    //��ֵΪƫ����
		SSPIFunc::ConfirmDevice(0, &TESLA_Base::Telsa_SSP0VDAC);
		DAC16bit_AD5360::Dac_out(chn,value);
		break;

		//ǯλ
	case ACTIVE_DAC_VH:
		TESLA_ADC5DAC::Dac_out_Clamp_Vlt(0,value);    //VOUTA - DACH
		break;
	case ACTIVE_DAC_VL:
		TESLA_ADC5DAC::Dac_out_Clamp_Vlt(2,value);    //VOUTC - DACL
		break;
	default:
		break;
	}
	return 1;
}
char TESLA_Base::SetRange(unsigned long ChannelIdx,unsigned long CurrRange,unsigned long VoltRange)
{
	return 1;
}
void TESLA_Base::SetRelay(unsigned char ChannelIdx,unsigned short SetRly,unsigned short ClrRly,unsigned char bAutoCali)
{
	Tesla_relaystatus[ChannelIdx].Reg |= SetRly;
	Tesla_relaystatus[ChannelIdx].Reg &= ~ClrRly;

	if(bAutoCali)
		Tesla_relaystatus[ChannelIdx].Bits.IH = 0;

	CPLD_FUNC::WriteDOBus(ChannelIdx,Tesla_relaystatus[ChannelIdx].Reg);
}

//writeDOBus�Ȳ����Ѿ�����
//comment out chen 20130704

//����adc���ϵ�gain
void TESLA_Base::Adjust_ADC_Gain(char type,char chnType)
{
	if(chnType == ADC_TYPE_I)
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

//ADCģ���Ŵ�
void TESLA_Base::ADC_ADG_open(char chn) 
{
	CPLD_FUNC::WriteADGBus(chn,0x01);
}

void TESLA_Base::ADG_Reset(void)
{
	ControlPin::SetMeLow(TESLA_Base::PinGroup[TESLA_Pin_AdgRst]);
	ARBUS_DELAY(5);
	ControlPin::SetMeHigh(TESLA_Base::PinGroup[TESLA_Pin_AdgRst]);
}

void TESLA_Base::Feed_CpldWD(void)
{
	ControlPin::SetMeHigh(PinGroup[TESLA_Pin_WD]);
	TIMERSFunc::DELAY_SYS(100, 0);
	ControlPin::SetMeLow(PinGroup[TESLA_Pin_WD]);
}


//ÿ���ϵ�orsafety��  ���ã�����һ��ʱ��������
//VINӦ�����ṩ���ʵ�Դ�ļ̵���
//16*30ms = 600ms
unsigned char TESLA_Base::PowerOn(void)
{
	unsigned char i = 0;
	MsTime sys_time_temp;
	MsTime ms_time_temp;
	unsigned char status = 0;

	/////////////////
	do
	{
		if(status == 0)
		{
			CPLD_FUNC::WriteDOBus(i,Tesla_relaystatus[i].Reg);
			OS_ServeProcess::recordCurTime(&sys_time_temp);
			status = 1;    //delay 30 ms
		}
		else if(status == 1)
		{
			ms_time_temp = OS_ServeProcess::TimeDiff(sys_time_temp);
			//��������Ӳ������ʦ  �ĳ�100ms
			//20130828�¶��޸�
			if(ms_time_temp.Us100 >= 1000)    //�ĳ�100ms
			{
				status = 0;
				i++;
			}
		}
	}
	while(i<MAXCHANNELNO);

	return 1;
}
unsigned char TESLA_Base::PowerOffCheck(void)
{
  unsigned char temp;
// 	temp = GPIOFunc::RD_G_Port(TESLA_POWER_OFF_PORT,TESLA_POWER_OFF_PIN);
	temp = ControlPin::ReadMe(TESLA_Base::PinGroup[TESLA_Pin_PowerOff]);
	return temp;
}
/*********************************************************************************
**                            End Of File
*********************************************************************************/
