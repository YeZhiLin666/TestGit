
#include "../../Entry/includes.h"


#pragma arm section rwdata = "SRAM",zidata = "SRAM" 
St_DisposableTask BT2208_Base::SetCtrlOpenCh[5];
St_DisposableTask BT2208_Base::SetCtrlCloseCh[4];
St_DisposableTask BT2208_Base::SetCCToCC[10];    //加入了多range切换，所以这里扩充到10步
St_DisposableTask BT2208_Base::SetRstToOff[2]; 

SSPI_Para  BT2208_Base::BT2208_SSP0DAC;
SSPI_Para  BT2208_Base::BT2208_SSP1ADC;
unsigned char BT2208_Base::whichRange;    //哪个量程
#pragma arm section 

unsigned char BT2208_Base::SetCtrlOpenCh_Len = 5;
unsigned char BT2208_Base::SetCtrlCloseCh_Len = 4;
unsigned char BT2208_Base::SetCCToCC_Len = 10;
unsigned char BT2208_Base::SetRstToOff_Len = 2;
unsigned char BT2208_Base::SetOpenISO_Len = 2;
unsigned char BT2208_Base::SetCloseISO_Len = 2;
unsigned char BT2208_Base::SetCVToCV_Len = 2;

const St_Port BT2208_Base::PinGroup[BT2208_Total_Pins] =	//注意，此处的管脚顺序必须和头文件中enum的管脚顺序高度一致
{//Port      Bits       PortType           Function               Address               
	{1,        28,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{2,        12,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
	{2,        13,        eCPTYPE_MCUGPIO,   GPIO_I,                0},
	{1,        21,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{0,        6,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{1,        26,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{1,        0,         eCPTYPE_CPLD,      CPLD_O,               NBUS_WRITE_DCS},
	{1,        25,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{1,        3,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{1,        2,         eCPTYPE_MCUGPIO,   GPIO_I,                0},
	{3,        18,        eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{0,        15,        eCPTYPE_MCUGPIO,   SSP0_SCK,              0},
	{0,        17,        eCPTYPE_MCUGPIO,   SSP0_MISO,             0},
	{0,        18,        eCPTYPE_MCUGPIO,   SSP0_MOSI,             0},
	{0,        7,         eCPTYPE_MCUGPIO,   SSP1_SCK,              0},
	{0,        8,         eCPTYPE_MCUGPIO,   SSP1_MISO,             0},
	{0,        9,         eCPTYPE_MCUGPIO,   SSP1_MOSI,             0},
	{2,        3,         eCPTYPE_MCUGPIO,   GPIO_O,                0},
	{1,        2,         eCPTYPE_CPLD,      CPLD_O,                NBUS_WRITE_DCS},
};

const unsigned char BT2208_Base::m_bHaveVoltageDac = 0;
const unsigned char BT2208_Base::m_bHaveClampVdac = 0;
const unsigned char BT2208_Base::m_bChannelBasedClampVdac = 0;
//缺一个自校准

volatile St_BT2208_Relay BT2208_Base::BT2208_relaystatus[MAXCHANNELNO];   

void BT2208_Base::Hardware_Init(void)
{
	IO_Init();    
	Feed_CpldWD(); 

	board_config::bHaveRedundantCheck = 0;

	CPLD_FUNC::Init();
	memset((void *)(&BT2208_relaystatus[0]), 0, MAXCHANNELNO);
	Feed_CpldWD();  // ?11·D?o?

	for(unsigned char ChannelIdx = 0;ChannelIdx<ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount;ChannelIdx++)
		Relay_on(ChannelIdx,ACTIVE_V_I_R);

	//此处powerOn是否还需要
	ControlPin::SetMeHigh(PinGroup[BT2208_Pin_PowerOn]);

	//SSP0 for DAC    AD5360
	BT2208_SSP0DAC.uc_Module_Name =SSP0_DEVICE1;    //name
	BT2208_SSP0DAC.uc_Speed = 5;
	BT2208_SSP0DAC.uc_CPOL = 0;
	BT2208_SSP0DAC.uc_CPHA = 1;
	BT2208_SSP0DAC.uc_MSTR = 1;    //master
	BT2208_SSP0DAC.uc_BITS = 8;
	BT2208_SSP0DAC.uc_LSBF = 0;

	//SSP1 for ADC
	BT2208_SSP1ADC.uc_Module_Name = SSP1_DEVICE1;    //name
	BT2208_SSP1ADC.uc_Speed = 5;
	BT2208_SSP1ADC.uc_CPOL = 0;
	BT2208_SSP1ADC.uc_CPHA = 0;       //rising trig
	BT2208_SSP1ADC.uc_MSTR = 1;    //master
	BT2208_SSP1ADC.uc_BITS = 8;
	BT2208_SSP1ADC.uc_LSBF = 0;

	TIMERSFunc::DELAY_SYS( 30000 );	//delay 5ms
	Feed_CpldWD(); 

	ADC_Init();
	DAC_Init();

	ControlPin::ReInitPullUP();

	Feed_CpldWD(); 

	for(unsigned char ChannelIdx =0;ChannelIdx<MAXCHANNELNO;ChannelIdx++)
		BT2208_Base::Adjust_CHN_Gain(ChannelIdx,10);   

	BT2208_Base::Adjust_ADC_Gain(0,0);
	//	BT2208_TempSample::AD_Init(8);  //根据硬件调整通道数值 add by qjm20140613
	OS_LogicProcess::InitBoardEngine(	&SetCtrlOpenCh[0],	sizeof(SetCtrlOpenCh)		/		sizeof(St_DisposableTask),
		&SetCtrlCloseCh[0], sizeof(SetCtrlCloseCh)	/		sizeof(St_DisposableTask),
		&SetCCToCC[0], 			sizeof(SetCCToCC)				/		sizeof(St_DisposableTask),
		&SetRstToOff[0],		sizeof(SetRstToOff)			/		sizeof(St_DisposableTask)
		);
	//	AdditionalEngine();
}
void BT2208_Base::IO_Init(void)
{
	for(int i=0;i<BT2208_Total_Pins;i++)
		ControlPin::SetIObyCP(PinGroup[i]);
} 

void BT2208_Base::ADC_Init(void)
{	
	BT2208_ADC5DAC::Adc_Init();
}
void BT2208_Base::DAC_Init(void)
{
	BT2208_ADC5DAC::Dac_Init();
	board_config::DacInfo.m_bCurrRngCount = 3;
	board_config::DacInfo.m_bVoltRngCount = 1;
	board_config::DacInfo.m_bHaveVoltageDac = m_bHaveVoltageDac;
	board_config::DacInfo.m_bHaveClampVdac = m_bHaveClampVdac;
	board_config::DacInfo.m_bChannelBasedClampVdac = m_bChannelBasedClampVdac;
	ARBUS_DELAY(10000);    //300us reset time  AD5360
}

void BT2208_Base::Software_Init(void)
{
	Func_Init();
	EngineStuff(); 

	BT2208_SampleManage::Init();
	whichRange = CURRENT_RANGE_1;   
}

void BT2208_Base::Func_Init(void)
{
	board_config::Func_Dac_out=&BT2208_Base::DAC_out;
	board_config::Func_Sample = &BT2208_SampleManage::SampleISR;
	//	board_config::Func_TempSample = &BT2208_TempSample::Temp_rd;
}

//void BT2208_Base::AdditionalEngine(void)
//{

//}

//void BT2208_Base::regCHNInfo(unsigned char CH_Number,unsigned long type)
//{
//
//}
//void BT2208_Base::regEngine(unsigned char CH_Number,unsigned long type)
//{
//
//}
void BT2208_Base::EngineStuff(void)
{
	//---------------------------------------------//
	memset(&SetCtrlOpenCh,		0,    sizeof(SetCtrlOpenCh));
	memset(&SetCtrlCloseCh,		0,    sizeof(SetCtrlCloseCh));
	memset(&SetCCToCC,				0,    sizeof(SetCCToCC));
	memset(&SetRstToOff,			0,    sizeof(SetRstToOff));

	SetCtrlOpenCh[0].uw32_DelayTime = 100;    
	//SetCtrlOpenCh[0].RelayMove.ActiveFlag = 0;						
	SetCtrlOpenCh[0].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;  
	SetCtrlOpenCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
	//SetCtrlOpenCh[0].RangeChangedFlag = 0;

	SetCtrlOpenCh[1].uw32_DelayTime = 100;  
	SetCtrlOpenCh[1].RelayMove.ActiveFlag = ACTIVE_P_R;						
	//SetCtrlOpenCh[1].RelayMove.RelaySet = 0;  
	SetCtrlOpenCh[1].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
	//SetCtrlOpenCh[1].RangeChangedFlag = 0;

	SetCtrlOpenCh[2].uw32_DelayTime = 100;
	SetCtrlOpenCh[2].RelayMove.ActiveFlag = (ACTIVE_OUT_RELAY | ACTIVE_P_R);  
	//SetCtrlOpenCh[2].RelayMove.RelaySet = 0;    
	SetCtrlOpenCh[2].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
	//SetCtrlOpenCh[2].RangeChangedFlag = 0;   

	SetCtrlOpenCh[3].uw32_DelayTime = 100;
	SetCtrlOpenCh[3].RelayMove.ActiveFlag =  ACTIVE_OUT_RELAY;   
	//SetCtrlOpenCh[3].RelayMove.RelaySet = 0;   
	SetCtrlOpenCh[3].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
	//SetCtrlOpenCh[3].RangeChangedFlag = 0;

	//SetCtrlOpenCh[4].uw32_DelayTime = 0;
	SetCtrlOpenCh[4].RelayMove.ActiveFlag = ACTIVE_OUT_RELAY;   
	//SetCtrlOpenCh[4].RelayMove.RelaySet = 0;   
	SetCtrlOpenCh[4].DacMove.DacFlag = ACTIVE_DAC_CUR;
	//SetCtrlOpenCh[4].RangeChangedFlag = 0;    

	//---------------------------------------------//
	SetCtrlCloseCh[0].uw32_DelayTime = 100;
	SetCtrlCloseCh[0].RelayMove.ActiveFlag = ACTIVE_OUT_RELAY;
	SetCtrlCloseCh[0].RelayMove.RelaySet = ACTIVE_CLOSE_CURRENT_RELAY;
	SetCtrlCloseCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
	//SetCtrlCloseCh[0].RangeChangedFlag = 0;

	SetCtrlCloseCh[1].uw32_DelayTime = 100;
	SetCtrlCloseCh[1].RelayMove.ActiveFlag = (ACTIVE_OUT_RELAY | ACTIVE_P_R);    //打开电流电压端检测
	SetCtrlCloseCh[1].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
	//SetCtrlCloseCh[1].RangeChangedFlag = 0;

	SetCtrlCloseCh[2].uw32_DelayTime = 100;
	SetCtrlCloseCh[2].RelayMove.ActiveFlag = ACTIVE_P_R;    //打开电流电压端检测
	SetCtrlCloseCh[2].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
	//SetCtrlCloseCh[2].RangeChangedFlag = 0;

	//SetCtrlCloseCh[3].uw32_DelayTime = 0;
	//SetCtrlCloseCh[3].RelayMove.ActiveFlag = 0;    //打开电流电压端检测
	SetCtrlCloseCh[3].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
	//SetCtrlCloseCh[3].RangeChangedFlag = 0;


	//---------------------------------------------//
	//加入range 切换
	//这里的要点就是先关闭所有通道的，然后根据量程选择再对应打开
	SetCCToCC[0].uw32_DelayTime = 100;
	SetCCToCC[0].RelayMove.ActiveFlag = ACTIVE_OUT_RELAY;    
	SetCCToCC[0].RelayMove.RelaySet = ACTIVE_CLOSE_CURRENT_RELAY;
	SetCCToCC[0].DacMove.DacFlag = ACTIVE_DAC_NONE;
	//SetCCToCC[0].RangeChangedFlag = 0;

	SetCCToCC[1].uw32_DelayTime = 500;
	SetCCToCC[1].RelayMove.ActiveFlag = ACTIVE_OUT_RELAY;    
	SetCCToCC[1].RelayMove.RelaySet = ACTIVE_CLOSE_CURRENT_RELAY;
	SetCCToCC[1].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;    //dac to 0
	SetCCToCC[1].RangeChangedFlag = 1;

	SetCCToCC[2].uw32_DelayTime = 500;
	SetCCToCC[2].RelayMove.ActiveFlag = ACTIVE_OUT_RELAY | ACTIVE_P_R;    
	SetCCToCC[2].RelayMove.RelaySet = ACTIVE_CLOSE_CURRENT_RELAY;
	SetCCToCC[2].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;    //dac to 0
	SetCCToCC[2].RangeChangedFlag = 1;

	SetCCToCC[3].uw32_DelayTime = 100;
	SetCCToCC[3].RelayMove.ActiveFlag = ACTIVE_P_R;    
	SetCCToCC[3].RelayMove.RelaySet = ACTIVE_CLOSE_CURRENT_RELAY;    //next range relay
	SetCCToCC[3].DacMove.DacFlag = ACTIVE_DAC_NONE;
	SetCCToCC[3].RangeChangedFlag = 1;

	SetCCToCC[4].uw32_DelayTime = 100;
	//SetCCToCC[4].RelayMove.ActiveFlag = 0;    
	SetCCToCC[4].RelayMove.RelaySet = ACTIVE_CLOSE_CURRENT_RELAY;    //close pr
	SetCCToCC[4].DacMove.DacFlag = ACTIVE_DAC_NONE;
	SetCCToCC[4].RangeChangedFlag = 1;

	SetCCToCC[5].uw32_DelayTime = 100;
	//SetCCToCC[5].RelayMove.ActiveFlag = 0;    
	SetCCToCC[5].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;    //next
	SetCCToCC[5].DacMove.DacFlag = ACTIVE_DAC_NONE;
	SetCCToCC[5].RangeChangedFlag = 1;

	SetCCToCC[6].uw32_DelayTime = 100;
	SetCCToCC[6].RelayMove.ActiveFlag = ACTIVE_P_R;                   //open next range pr
	SetCCToCC[6].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;   
	SetCCToCC[6].DacMove.DacFlag = ACTIVE_DAC_NONE;
	SetCCToCC[6].RangeChangedFlag = 1;

	SetCCToCC[7].uw32_DelayTime = 100;
	SetCCToCC[7].RelayMove.ActiveFlag = ACTIVE_P_R | ACTIVE_OUT_RELAY;    
	SetCCToCC[7].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
	SetCCToCC[7].DacMove.DacFlag = ACTIVE_DAC_NONE;
	SetCCToCC[7].RangeChangedFlag = 1;

	SetCCToCC[8].uw32_DelayTime = 100;
	SetCCToCC[8].RelayMove.ActiveFlag = ACTIVE_OUT_RELAY;    
	SetCCToCC[8].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
	SetCCToCC[8].DacMove.DacFlag = ACTIVE_DAC_NONE;
	SetCCToCC[8].RangeChangedFlag = 1;

	//SetCCToCC[9].uw32_DelayTime = 0;
	SetCCToCC[9].RelayMove.ActiveFlag = ACTIVE_OUT_RELAY;    //保持通道   
	SetCCToCC[9].DacMove.DacFlag = ACTIVE_DAC_CUR;
	SetCCToCC[9].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;    //直接输出下一量程
	//SetCCToCC[9].RangeChangedFlag = 0;

	//---------------------------------------------//
	SetRstToOff[0].uw32_DelayTime = 100;
	//SetRstToOff[0].RelayMove.ActiveFlag = 0;
	//SetRstToOff[0].DacMove.DacFlag = 0;
	//SetRstToOff[0].RangeChangedFlag = 0;

	//SetRstToOff[1].uw32_DelayTime = 0;
	//SetRstToOff[1].RelayMove.ActiveFlag = 0;
	//SetRstToOff[1].DacMove.DacFlag = 0;
	//SetRstToOff[1].RangeChangedFlag = 0;

	// 	SetOpenISO[0].uw32_DelayTime = 100;
	// 	SetOpenISO[0].RelayMove.ActiveFlag = 0;
	// 	SetOpenISO[0].DacMove.DacFlag = ACTIVE_DAC_CUR;
	// 	SetOpenISO[0].RangeChangedFlag = 0;

	// 	SetOpenISO[1].uw32_DelayTime = 0;
	// 	SetOpenISO[1].RelayMove.ActiveFlag = 0;
	// 	SetOpenISO[1].DacMove.DacFlag = ACTIVE_DAC_CUR;
	// 	SetOpenISO[1].RangeChangedFlag = 0;

	// 	SetCloseISO[0].uw32_DelayTime = 100;
	// 	SetCloseISO[0].RelayMove.ActiveFlag = 0;
	// 	SetCloseISO[0].DacMove.DacFlag = ACTIVE_DAC_CUR;
	// 	SetCloseISO[0].RangeChangedFlag = 0;

	// 	SetCloseISO[1].uw32_DelayTime = 0;
	// 	SetCloseISO[1].RelayMove.ActiveFlag = 0;
	// 	SetCloseISO[1].DacMove.DacFlag = ACTIVE_DAC_CUR;
	// 	SetCloseISO[1].RangeChangedFlag = 0;
}

//end init

/******************************************************************************
** Function name:		DAC_out
**
** Descriptions:	        typeA DAC output
**			     
** parameters:			CH_Number:0->Max Channel Number; Data:32bit value
** Returned value:		0/1
** 
******************************************************************************/
char BT2208_Base::DAC_out(unsigned long ChannelIdx,unsigned long Data,unsigned long type)
{
	//	unsigned char testJ = 8;

	switch(type)
	{
	case ACTIVE_DAC_CUR:
		ChannelInfo_Manager::ChannelInfo[ChannelIdx].m_u32DacValue = Data;    //此值为偏移码
		SSPIFunc::ConfirmDevice(0, &BT2208_Base::BT2208_SSP0DAC);

#if BOARDTYPE_BT2208SINGLE==1
		//根据量程，做不同处理
		//类似于relay
		if(whichRange == CURRENT_RANGE_1)
		{
			testJ = 8;
			ChannelIdx = 0;
		}
		else if(whichRange == CURRENT_RANGE_2)
		{
			testJ = 2;
			ChannelIdx = 0;
		}
		else if(whichRange == CURRENT_RANGE_3)
		{
			testJ = 1;
			ChannelIdx = 0;
		}

		//因为量程是200A，所以8个输出正好弥补过来，这里就不用更改了
		for(unsigned char i = 0;i<testJ;i++)
		{
			DAC16bit_AD5360::Dac_out(i,Data);	

			//如果加入dac同步，需要使用load  20130918
		}

		return 1;
#endif

		if( (ChannelIdx == BT2208_821_CHN_NUM)&&(Data == DAC_OUT_PUT_ZERO) )    //全部通道输出0
		{
			for(unsigned char i = 0;i<8;i++)
			{
				DAC16bit_AD5360::Dac_out(i,Data);	
				//如果加入dac同步，需要使用load  20130918
			}
			return 1;
		}
		else 
		{
			if(ChannelIdx == BT2208_821_CHN_NUM)
			{
				//连续计算码字
				unsigned long ul_sum[8];
				float temp = 0;
				if(ChannelInfo_Manager::ChannelInfo[ChannelIdx].chn_Running_Flag == CHN_LOCK_THIRT_PARTY)
				{
					temp = thirdParty::f_desired_value * 0.125f;    // x = sum/8
					for(unsigned char i = 0;i<8;i++)
					{
						ul_sum[i] = ChannelInfo_Manager::CalibrateCurrentDAC(i,CURRENT_RANGE_1,temp);    //不清楚耗时多少
					}
				}
				else if(ChannelInfo_Manager::ChannelInfo[ChannelIdx].chn_Running_Flag == CHN_LOCK_SCH)
				{
					temp = Step_Manager_task::m_FSet[ChannelIdx] * 0.125f;
					for(unsigned char i = 0;i<8;i++)
					{   
						ul_sum[i] = ChannelInfo_Manager::CalibrateCurrentDAC(i,CURRENT_RANGE_1,temp);    //不清楚耗时多少
					}
				}
				else
				{
					for(unsigned char i = 0;i<8;i++)
					{   
						ul_sum[i] = Data;
					}
				}

				//连续输出
				for(unsigned char i = 0;i<8;i++)
				{
					DAC16bit_AD5360::Dac_out(i,ul_sum[i]);	    //应该不会溢出
				}
			}
			else
			{
				DAC16bit_AD5360::Dac_out(ChannelIdx,Data);	
			}
		}

		break;
		//         case ACTIVE_DAC_VLT:
		//             BT2208_ADC5DAC::Dac_out_Vlt(ChannelIdx,Data);
		//             break;
		//         case ACTIVE_DAC_VH:
		//             BT2208_ADC5DAC::Dac_out_Clamp_Vlt(ChannelIdx,Data);
		//             break;
	default:
		break;
	}
	return 1;
}

char BT2208_Base::Relay_on(unsigned long ChannelIdx,unsigned long type)
{
	unsigned char testJ = 1;
#if BOARDTYPE_BT2208SINGLE==1
	ChannelIdx = BT2208_821_CHN_NUM;
#endif

	switch(type)
	{
	case ACTIVE_CUR_RELAY_HIGH:
		whichRange = CURRENT_RANGE_1;
		return 1;
		//break;
	case ACTIVE_CUR_RELAY_MIDDLE:
		whichRange = CURRENT_RANGE_2;
		return 1;
		//break;
	case ACTIVE_CUR_RELAY_LOW:
		whichRange = CURRENT_RANGE_3;
		return 1;
		//break;
	default:
		break;
	}

	if(ChannelIdx == BT2208_821_CHN_NUM)
	{
		if(whichRange == CURRENT_RANGE_1)
		{
			testJ = 8;
			ChannelIdx = 0;
		}
		else if(whichRange == CURRENT_RANGE_2)
		{
			testJ = 2;
			ChannelIdx = 0;
		}
		else if(whichRange == CURRENT_RANGE_3)
		{
			testJ = 1;
			ChannelIdx = 0;
		}
	}

	do
	{		
		switch(type)
		{
		case ACTIVE_VOL_RANGE:    //X_VIN   //?aà?￡?êμ?êé?BT2208ó??a??BIT??×÷?aCALID?o?￡???2?ê?VOLTAGED?o??￡
			BT2208_relaystatus[ChannelIdx].Bits.R_Cali = 1;
			CPLD_FUNC::WriteDOBus(ChannelIdx,BT2208_relaystatus[ChannelIdx].Reg);
			break;
			//return 1;

		case ACTIVE_P_R:              //P_R
			BT2208_relaystatus[ChannelIdx].Bits.P_R = 1;
			CPLD_FUNC::WriteDOBus(ChannelIdx,BT2208_relaystatus[ChannelIdx].Reg);
			break;
			//return 1;

		case ACTIVE_OUT_RELAY:     //X_CHL
			BT2208_relaystatus[ChannelIdx].Bits.R_CHL = 1;
			CPLD_FUNC::WriteDOBus(ChannelIdx,BT2208_relaystatus[ChannelIdx].Reg);
			break;
			//return 1;

		default:
			break;
			//return 0;
		}
		testJ--;
		ChannelIdx++;
	}while(testJ>0);

	return 1;
}
char BT2208_Base::Relay_off(unsigned long ChannelIdx,unsigned long type)
{
	unsigned char testJ = 1;
#if BOARDTYPE_BT2208SINGLE==1
	ChannelIdx = BT2208_821_CHN_NUM;
#endif
	if(ChannelIdx == BT2208_821_CHN_NUM)
	{
		if(whichRange == CURRENT_RANGE_1)    //默认为high
		{
			testJ = 8;
			ChannelIdx = 0;
		}
		else if(whichRange == CURRENT_RANGE_2)
		{
			testJ = 2;
			ChannelIdx = 0;
		}
		else if(whichRange == CURRENT_RANGE_3)
		{
			testJ = 1;
			ChannelIdx = 0;
		}
	}

	do
	{
		switch(type)
		{
		case ACTIVE_VOL_RANGE:    //X_VIN
			BT2208_relaystatus[ChannelIdx].Bits.R_Cali = 0;
			CPLD_FUNC::WriteDOBus(ChannelIdx,BT2208_relaystatus[ChannelIdx].Reg);
			break;
			//return 1;
		case ACTIVE_P_R:              //I/S/O
			BT2208_relaystatus[ChannelIdx].Bits.P_R = 0;
			CPLD_FUNC::WriteDOBus(ChannelIdx,BT2208_relaystatus[ChannelIdx].Reg);
			break;
			//return 1;
		case ACTIVE_OUT_RELAY:     //X_CHL
			BT2208_relaystatus[ChannelIdx].Bits.R_CHL = 0;
			CPLD_FUNC::WriteDOBus(ChannelIdx,BT2208_relaystatus[ChannelIdx].Reg);
			break;
			//return 1;
		default:
			break;
			//return 0;
		}
		testJ--;
		ChannelIdx++;
	}while(testJ>0);

	return 1;
}

void BT2208_Base::Adjust_CHN_Gain(unsigned char ChannelIdx,unsigned long Gain)    
{
	switch(Gain)
	{
	case 1:
		BT2208_relaystatus[ChannelIdx].Bits.AdcGain = 0;
		break;
	case 10:
		BT2208_relaystatus[ChannelIdx].Bits.AdcGain = 1;
		break;
	case 100:
		BT2208_relaystatus[ChannelIdx].Bits.AdcGain = 2;
		break;
	case 1000:
		BT2208_relaystatus[ChannelIdx].Bits.AdcGain = 3;
		break;
	default:
		BT2208_relaystatus[ChannelIdx].Bits.AdcGain = 3;
		break;
	}
	BT2208_relaystatus[ChannelIdx].Bits.AdcGainEnable = 1;

	CPLD_FUNC::WriteDOBus(ChannelIdx,BT2208_relaystatus[ChannelIdx].Reg);
	BT2208_relaystatus[ChannelIdx].Bits.AdcGainEnable = 0;
	CPLD_FUNC::WriteDOBus(ChannelIdx,BT2208_relaystatus[ChannelIdx].Reg);
}

void BT2208_Base::Adjust_ADC_Gain(unsigned char type,unsigned char Gain)
{
	St_BT2208_PGA MyReg;

	MyReg.Bits.AdcGain_I = 0;
	MyReg.Bits.AdcGain_V = 0;
	MyReg.Bits.AdcGainEnable = 1;
	CPLD_FUNC::Write(NBUS_WRITE_PGA_GAIN,MyReg.Reg);

	MyReg.Bits.AdcGainEnable = 0;
	CPLD_FUNC::Write(NBUS_WRITE_PGA_GAIN,MyReg.Reg);

	MyReg.Bits.AdcGainEnable = 1;
	CPLD_FUNC::Write(NBUS_WRITE_PGA_GAIN,MyReg.Reg);
}


void BT2208_Base::ADC_ADG_open(unsigned char ChannelIdx) 
{
	CPLD_FUNC::WriteADGBus(ChannelIdx,0x01);
}

void BT2208_Base::ADG_Reset(void)
{
	ControlPin::SetMeLow(BT2208_Base::PinGroup[BT2208_Pin_AdgRst]);
	ARBUS_DELAY(5);
	ControlPin::SetMeHigh(BT2208_Base::PinGroup[BT2208_Pin_AdgRst]);
}

void BT2208_Base::changeADCRange(unsigned char ChannelIdx,unsigned char range)
{
	ARBUS_DELAY(10);
	switch(range)
	{
	case CURRENT_RANGE_1:
		BT2208_Base::Adjust_CHN_Gain(ChannelIdx,10);   
		break;
	case CURRENT_RANGE_2:
		BT2208_Base::Adjust_CHN_Gain(ChannelIdx,100);   
		break;
	case CURRENT_RANGE_3:
		BT2208_Base::Adjust_CHN_Gain(ChannelIdx,1000);   
		break;
	default:
		BT2208_Base::Adjust_CHN_Gain(ChannelIdx,10);   
		break;			
	}

}

void BT2208_Base::Feed_CpldWD(void)
{
	ControlPin::SetMeHigh(PinGroup[BT2208_Pin_WD]);
	TIMERSFunc::DELAY_SYS(200);
	ControlPin::SetMeLow(PinGroup[BT2208_Pin_WD]);
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
