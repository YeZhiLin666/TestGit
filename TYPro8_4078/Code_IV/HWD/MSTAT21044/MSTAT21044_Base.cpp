#include "../../Entry/includes.h"

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
St_DisposableTask MSTAT21044_Base::SetCtrlOpenCh[5];
St_DisposableTask MSTAT21044_Base::SetCtrlCloseCh[3];
St_DisposableTask MSTAT21044_Base::SetCCToCC[6];    //加入了多range切换，所以这里扩充到10步
St_DisposableTask MSTAT21044_Base::SetRstToOff[2];

St_DisposableTask MSTAT21044_Base::SetRstToCur[5];
St_DisposableTask MSTAT21044_Base::SetRstToVlt[5];

St_DisposableTask MSTAT21044_Base::SetOffToVlt[5];
St_DisposableTask MSTAT21044_Base::SetCurToVlt[7];
St_DisposableTask MSTAT21044_Base::SetVltToOff[3];
St_DisposableTask MSTAT21044_Base::SetVltToRst[3];
St_DisposableTask MSTAT21044_Base::SetVltToVlt[6];
St_DisposableTask MSTAT21044_Base::SetVltToCur[8];
St_DisposableTask MSTAT21044_Base::SetCtrlOpenAcim[1];
St_DisposableTask MSTAT21044_Base::SetCtrlCloseAcim[1];
#if (IV_AUTORANGE == 1)
St_DisposableTask MSTAT21044_Base::SetAutoRangeSwitch[6];
#endif
#pragma arm section

void MSTAT21044_Base::FeatureEnable_Init(void)
{
    board_config::Feature_Enable_init.OS_Tick_Interval =400;	//niu

    board_config::DacInfo.m_bCurrRngCount = 4;//niu

// 	board_config::DacInfo.m_bChannelBasedClampVdac = 1;
//	board_config::DacInfo.m_uSettingTime = 1000;
    board_config::DacInfo.m_uSettingTime = 50;
    board_config::DacInfo.m_uRisingTime = MSTAT21044_RISING_TIME;// Rising time 5ms, unit is 100us
    board_config::DacInfo.m_fPID_Coefficient = MSTAT21044_Tpid_Tsys;
    board_config::DacInfo.m_bHaveBuildInVolt = 1;
    board_config::DacInfo.m_bHaveVoltageDac = 1;

    BoardFeature.FINE_TUNE_MINERRORFACTOR =2E-5;
    BoardFeature.FINE_TUNE_STEPFACTOR = 6E-6;
    BoardFeature.FINE_TUNE_MAXADJUSTFACTOR = 0.03F;
	BoardFeature.FINE_TUNE_PARAMasterWaitTime =0.3584;

    //BoardFeature.BoolFlag.Adc24bit_CS5560_Enable = true;
    BoardFeature.BoolFlag.Adc24bit_AD7175_Enable = true;
    BoardFeature.BoolFlag.DAC16bit_AD5360_Enable = true;
// 	BoardFeature.BoolFlag.DAC16bit_AD5754_Enable = false;

// 	BoardFeature.BoolFlag.FAN_IIC_Enable = false;
    BoardFeature.BoolFlag.FAN_PWM_Enable = true;

// 	BoardFeature.BoolFlag.UnitDIDO_Enable = false;
    BoardFeature.BoolFlag.R_VM_Enable = true;
    BoardFeature.BoolFlag.ACIM_Enable = true;

// 	BoardFeature.BoolFlag.ACTIVE_DAC_CUR_NeedNegation_Enable = false;
    BoardFeature.BoolFlag.ACTIVE_DAC_VH_VL_NeedNegation_Enable = true;
    BoardFeature.BoolFlag.ACTIVE_DAC_VLT_NeedNegation_Enable = true;
    BoardFeature.BoolFlag.ClampV_Concern_Parallel_Enable = true;
    BoardFeature.ACTIVE_DAC_CUR_Chan_JumpMode = LBT21XXX_Chan_JumpMode_MSTAT21044CUR;
    BoardFeature.ACTIVE_DAC_VLT_Chan_JumpMode = LBT21XXX_Chan_JumpMode_MSTAT21044VLT;
    BoardFeature.ACTIVE_DAC_VH_Chan_JumpMode =  LBT21XXX_Chan_JumpMode_MSTAT21044VH;
    BoardFeature.ACTIVE_DAC_VL_Chan_JumpMode =  LBT21XXX_Chan_JumpMode_MSTAT21044VL;

    BoardFeature.LBT21XXXMINIMUMPLSE = 50;
    BoardFeature.BoolFlag.Have_DAC_Vlt_Enable = true;
    BoardFeature.BoolFlag.Have_DAC_Vclamp_Enable = true;

    //BoardFeature.BoolFlag.PowerOffCheck_NeedNegation_Enable = true;

// 	BoardFeature.I_ADG_Address_Change_Mode = LBT21XXX_I_ADG_Address_Change_Mode_None;

    unsigned char FanCtrlValue[8]= {60,80,90,100,100,100,100,100};
    memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_1], &FanCtrlValue[0], 8);
    unsigned char FanCtrlValue1[8]= {30,30,30,30,30,30,40,50};
    memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_2], &FanCtrlValue1[0], 8);
    memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_3], &FanCtrlValue1[0], 8);
    unsigned char FanCtrlValue2[8]= {30,30,30,30,30,30,30,30};
    memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_4], &FanCtrlValue2[0], 8);

    OS_LogicProcess::InitBoardEngine(	&SetCtrlOpenCh[0],	sizeof(SetCtrlOpenCh)		/		sizeof(St_DisposableTask),
                                        &SetCtrlCloseCh[0], sizeof(SetCtrlCloseCh)	/		sizeof(St_DisposableTask),
                                        &SetCCToCC[0], 			sizeof(SetCCToCC)				/		sizeof(St_DisposableTask),
                                        &SetRstToOff[0],		sizeof(SetRstToOff)			/		sizeof(St_DisposableTask)
                                    );
    AdditionalEngine();
    EngineStuff();
}
void MSTAT21044_Base::AdditionalEngine()
{
	
  OS_LogicProcess::SetCtrlRstToVlt.st_Disposabletask_ptr =&MSTAT21044_Base::SetRstToVlt[0];
	OS_LogicProcess::SetCtrlRstToVlt.stepLen = sizeof(SetRstToVlt)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlOffToVlt.st_Disposabletask_ptr =&MSTAT21044_Base::SetOffToVlt[0];
	OS_LogicProcess::SetCtrlOffToVlt.stepLen = sizeof(SetOffToVlt)	/	sizeof(St_DisposableTask);

  OS_LogicProcess::SetCtrlCurToVlt.st_Disposabletask_ptr =&MSTAT21044_Base::SetCurToVlt[0];
	OS_LogicProcess::SetCtrlCurToVlt.stepLen = sizeof(SetCurToVlt)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlVltToVlt.st_Disposabletask_ptr =&MSTAT21044_Base::SetVltToVlt[0];
	OS_LogicProcess::SetCtrlVltToVlt.stepLen = sizeof(SetVltToVlt)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlVltToCur.st_Disposabletask_ptr =&MSTAT21044_Base::SetVltToCur[0];
	OS_LogicProcess::SetCtrlVltToCur.stepLen = sizeof(SetVltToCur)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlVltToOff.st_Disposabletask_ptr =&MSTAT21044_Base::SetVltToOff[0];
	OS_LogicProcess::SetCtrlVltToOff.stepLen = sizeof(SetVltToOff)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlVltToRst.st_Disposabletask_ptr =&MSTAT21044_Base::SetVltToRst[0];
	OS_LogicProcess::SetCtrlVltToRst.stepLen = sizeof(SetVltToRst)	/	sizeof(St_DisposableTask);  

	// ACIM Engine
    OS_LogicProcess::SetCtrlAcimOn.st_Disposabletask_ptr = &MSTAT21044_Base::SetCtrlOpenAcim[0];
    OS_LogicProcess::SetCtrlAcimOn.stepLen = sizeof(SetCtrlOpenAcim) / sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlAcimOff.st_Disposabletask_ptr = &MSTAT21044_Base::SetCtrlCloseAcim[0];
    OS_LogicProcess::SetCtrlAcimOff.stepLen = sizeof(SetCtrlCloseAcim) / sizeof(St_DisposableTask);
}

//commnet out chen 20130216
//void MSTAT21044_Base::AdditionalEngine(void)
//{
//}
void MSTAT21044_Base::EngineStuff(void)
{
    memset(&SetCtrlOpenCh,   0,    sizeof(SetCtrlOpenCh));
    memset(&SetCtrlCloseCh,  0,    sizeof(SetCtrlCloseCh));
    memset(&SetCCToCC,       0,    sizeof(SetCCToCC));
    memset(&SetRstToOff,     0,    sizeof(SetRstToOff));
    memset(&SetCtrlOpenAcim,	0,    sizeof(SetCtrlOpenAcim));
    memset(&SetCtrlCloseAcim,	0,    sizeof(SetCtrlCloseAcim));

    SetCtrlOpenCh[0].uw32_DelayTime = 200;
	SetCtrlOpenCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

    SetCtrlOpenCh[1].uw32_DelayTime = 100;
    SetCtrlOpenCh[1].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect;

    SetCtrlOpenCh[2].uw32_DelayTime = 150;
    SetCtrlOpenCh[2].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;

    SetCtrlOpenCh[3].uw32_DelayTime = 50;
    SetCtrlOpenCh[3].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;

    SetCtrlOpenCh[4].uw32_DelayTime = 100;
    SetCtrlOpenCh[4].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

    SetCtrlCloseCh[0].uw32_DelayTime = 100;
    SetCtrlCloseCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
// 	SetCtrlCloseCh[0].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect;

    SetCtrlCloseCh[1].uw32_DelayTime = 150;
    SetCtrlCloseCh[1].RelayMove.InactiveFlag = LBT21XXX_RLY_Output;

    SetCtrlCloseCh[2].uw32_DelayTime = 50;
    SetCtrlCloseCh[2].RelayMove.InactiveFlag =  LBT21XXX_RLY_ACIM;


    SetCCToCC[0].uw32_DelayTime = 100;
    SetCCToCC[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
// 	SetCCToCC[0].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect;
    SetCCToCC[0].RangeChangedFlag = 1;

    SetCCToCC[1].uw32_DelayTime = 100;//
    SetCCToCC[1].RelayMove.InactiveFlag = LBT21XXX_RLY_Output;//
    SetCCToCC[1].RangeChangedFlag = 1;//

    SetCCToCC[2].uw32_DelayTime = 50;//
    SetCCToCC[2].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect;//
    SetCCToCC[2].RangeChangedFlag = 1;//

    SetCCToCC[3].uw32_DelayTime = 150;
    SetCCToCC[3].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
    SetCCToCC[3].RangeChangedFlag = 1;

    SetCCToCC[4].uw32_DelayTime = 50;
    SetCCToCC[4].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;
    SetCCToCC[4].RangeChangedFlag = 1;

    SetCCToCC[5].uw32_DelayTime = 50;
    SetCCToCC[5].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

    SetRstToOff[0].uw32_DelayTime = 100;
    //SetRstToOff[0].RelayMove.ActiveFlag = 0;
    SetRstToOff[0].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT; //需要放开clamp;
    //SetRstToOff[0].RangeChangedFlag = 0;

    //SetRstToOff[1].uw32_DelayTime = 0;
    //SetRstToOff[1].RelayMove.ActiveFlag = 0;
    SetRstToOff[1].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
    SetRstToOff[1].RelayMove.InactiveFlag = LBT21XXX_RLY_ACIM;
    //SetRstToOff[1].RangeChangedFlag = 0;
	
	
		//rst--Vlt ok
		SetRstToVlt[0].uw32_DelayTime = 100;
		SetRstToVlt[0].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
		
		SetRstToVlt[1].uw32_DelayTime = 150;
		SetRstToVlt[1].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect ;//| LBT21XXX_RLY_DAMPING;		

// 		SetRstToVlt[2].uw32_DelayTime = 150;
// 		SetRstToVlt[2].RelayMove.ActiveFlag = LBT21XXX_RLY_CHON;		
// 		
		SetRstToVlt[2].uw32_DelayTime = 50;
		SetRstToVlt[2].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;

		SetRstToVlt[3].uw32_DelayTime = 20;
		SetRstToVlt[3].DacMove.DacFlag = ACTIVE_DAC_VLT;
		
		SetRstToVlt[4].uw32_DelayTime = 100;
		SetRstToVlt[4].RelayMove.ActiveFlag = LBT21XXX_RLY_IVS;			
		
		
		
	
		
		 //off-VLT ok
		
			SetOffToVlt[0].uw32_DelayTime = 100;
		SetOffToVlt[0].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
		
		SetOffToVlt[1].uw32_DelayTime = 150;
		SetOffToVlt[1].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect ;//| LBT21XXX_RLY_DAMPING;		

// 		SetOffToVlt[2].uw32_DelayTime = 150;
// 		SetOffToVlt[2].RelayMove.ActiveFlag = LBT21XXX_RLY_CHON;		
// 		
		SetOffToVlt[2].uw32_DelayTime = 50;
		SetOffToVlt[2].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;

		SetOffToVlt[3].uw32_DelayTime = 20;
		SetOffToVlt[3].DacMove.DacFlag = ACTIVE_DAC_VLT;
		
		SetOffToVlt[4].uw32_DelayTime = 100;
		SetOffToVlt[4].RelayMove.ActiveFlag = LBT21XXX_RLY_IVS;			
		
		//CC-CV ok
		SetCurToVlt[0].uw32_DelayTime = 100;
		SetCurToVlt[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
		SetCurToVlt[0].RangeChangedFlag = 1;//
		
// 		SetCurToVlt[1].uw32_DelayTime = 150;
// 		SetCurToVlt[1].RelayMove.InactiveFlag = LBT21XXX_RLY_CHON;//
// 		SetCurToVlt[1].RangeChangedFlag = 1;
		
		SetCurToVlt[1].uw32_DelayTime = 100;		
		SetCurToVlt[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
		SetCurToVlt[1].RangeChangedFlag = 1;
		
		SetCurToVlt[2].uw32_DelayTime = 50;
		SetCurToVlt[2].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect ;
		SetCurToVlt[2].RangeChangedFlag = 1;
		
		
// 		SetCurToVlt[3].uw32_DelayTime = 150;
// 		SetCurToVlt[3].RelayMove.ActiveFlag = LBT21XXX_RLY_CHON;
// 		SetCurToVlt[3].RangeChangedFlag = 1;

		SetCurToVlt[3].uw32_DelayTime = 50;
		SetCurToVlt[3].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;
		SetCurToVlt[3].RangeChangedFlag = 1;

		SetCurToVlt[4].uw32_DelayTime = 20;
		SetCurToVlt[4].DacMove.DacFlag = ACTIVE_DAC_VLT;
		
		SetCurToVlt[5].uw32_DelayTime = 50;
		SetCurToVlt[5].RelayMove.ActiveFlag = LBT21XXX_RLY_IVS;
		
		SetCurToVlt[6].uw32_DelayTime = 100;
		SetCurToVlt[6].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

		
		//CV-CV ok
		SetVltToVlt[0].uw32_DelayTime = 100;		
		SetVltToVlt[0].RelayMove.InactiveFlag = LBT21XXX_RLY_IVS;
		SetVltToVlt[0].RangeChangedFlag = 1;
		

		
// 		SetVltToVlt[1].uw32_DelayTime = 150;
// 		SetVltToVlt[1].RelayMove.InactiveFlag = LBT21XXX_RLY_CHON;//
// 		SetVltToVlt[1].RangeChangedFlag = 1;
		
		SetVltToVlt[1].uw32_DelayTime = 100;		
		SetVltToVlt[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
		SetVltToVlt[1].RangeChangedFlag = 1;
		
		SetVltToVlt[2].uw32_DelayTime = 50;
		SetVltToVlt[2].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect ;
		SetVltToVlt[2].RangeChangedFlag = 1;
		
// 		SetVltToVlt[3].uw32_DelayTime = 50;	
// 		SetVltToVlt[3].RelayMove.ActiveFlag =  LBT21XXX_RLY_DAMPING;
		
// 		SetVltToVlt[5].uw32_DelayTime = 150;
// 		SetVltToVlt[5].RelayMove.ActiveFlag = LBT21XXX_RLY_CHON;
// 		SetVltToVlt[5].RangeChangedFlag = 1;

		SetVltToVlt[3].uw32_DelayTime = 50;
		SetVltToVlt[3].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;
		SetVltToVlt[3].RangeChangedFlag = 1;

		SetVltToVlt[4].uw32_DelayTime = 50;
		SetVltToVlt[4].DacMove.DacFlag = ACTIVE_DAC_VLT;
		
		SetVltToVlt[5].uw32_DelayTime = 100;		
		SetVltToVlt[5].RelayMove.ActiveFlag = LBT21XXX_RLY_IVS;
		SetVltToVlt[5].RangeChangedFlag = 1;
		
		//CV-CC ok		
		SetVltToCur[0].uw32_DelayTime = 10;		
		SetVltToCur[0].DacMove.DacFlag = ACTIVE_DAC_CUR;
		
		SetVltToCur[1].uw32_DelayTime = 100;		
		SetVltToCur[1].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
		SetVltToCur[1].RangeChangedFlag = 1;
		
		SetVltToCur[2].uw32_DelayTime = 100;
		SetVltToCur[2].RelayMove.InactiveFlag =  LBT21XXX_RLY_IVS;
		
		SetVltToCur[3].uw32_DelayTime = 100;		
		SetVltToCur[3].DacMove.DacFlag = ACTIVE_DAC_VLT_ZERO;
		
// 		SetVltToCur[4].uw32_DelayTime = 150;
// 		SetVltToCur[4].RelayMove.InactiveFlag = LBT21XXX_RLY_CHON;
// 		SetVltToCur[4].RangeChangedFlag = 1;
// 		
		SetVltToCur[4].uw32_DelayTime = 100;		
		SetVltToCur[4].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
		SetVltToCur[4].RangeChangedFlag = 1;
		
		SetVltToCur[5].uw32_DelayTime = 50;
		SetVltToCur[5].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect;
		SetVltToCur[5].RangeChangedFlag = 1;
		
// 		SetVltToCur[7].uw32_DelayTime = 50;
// 		SetVltToCur[7].RelayMove.ActiveFlag = LBT21XXX_RLY_DAMPING;
		
// 		SetVltToCur[8].uw32_DelayTime = 150;
// 		SetVltToCur[8].RelayMove.ActiveFlag = LBT21XXX_RLY_CHON;//
// 		SetVltToCur[8].RangeChangedFlag = 1;	

		SetVltToCur[6].uw32_DelayTime = 50;
		SetVltToCur[6].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;
		SetVltToCur[6].RangeChangedFlag = 1;

		SetVltToCur[7].uw32_DelayTime = 50;		
		SetVltToCur[7].DacMove.DacFlag = ACTIVE_DAC_CUR;
		SetVltToCur[7].RangeChangedFlag = 1;
		//CV-OFF ok
		SetVltToOff[0].uw32_DelayTime = 100;
		SetVltToOff[0].RelayMove.InactiveFlag = LBT21XXX_RLY_IVS;		

		SetVltToOff[1].uw32_DelayTime = 150;
		SetVltToOff[1].DacMove.DacFlag = ACTIVE_DAC_VLT_ZERO;
		
// 		SetVltToOff[2].uw32_DelayTime = 50;
// 		SetVltToOff[2].RelayMove.InactiveFlag = LBT21XXX_RLY_CHON;
// 		
		SetVltToOff[2].uw32_DelayTime = 150;
		SetVltToOff[2].RelayMove.InactiveFlag = LBT21XXX_RLY_Output;// | LBT21XXX_RLY_DAMPING;	
		
		//CV-RST ok
		SetVltToRst[0].uw32_DelayTime = 100;
		SetVltToRst[0].RelayMove.InactiveFlag = LBT21XXX_RLY_IVS;		

		SetVltToRst[1].uw32_DelayTime = 150;
		SetVltToRst[1].DacMove.DacFlag = ACTIVE_DAC_VLT_ZERO;
		
// 		SetVltToRst[2].uw32_DelayTime = 50;
// 		SetVltToRst[2].RelayMove.InactiveFlag = LBT21XXX_RLY_CHON;
		
		SetVltToRst[2].uw32_DelayTime = 150;
		SetVltToRst[2].RelayMove.InactiveFlag = LBT21XXX_RLY_Output;// | LBT21XXX_RLY_DAMPING;	
	
	
	// ACIM切换	
    SetCtrlOpenAcim[0].uw32_DelayTime = 100;
    SetCtrlOpenAcim[0].RelayMove.ActiveFlag = LBT21XXX_RLY_R_ACIM;

    SetCtrlCloseAcim[0].uw32_DelayTime = 100;
    SetCtrlCloseAcim[0].RelayMove.InactiveFlag = LBT21XXX_RLY_R_ACIM;
		#if (IV_AUTORANGE == 1)
	SetAutoRangeSwitch[0].uw32_DelayTime = 100;
    SetAutoRangeSwitch[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
// 	SetAutoRangeSwitch[0].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect;
    SetAutoRangeSwitch[0].RangeChangedFlag = 1;

    SetAutoRangeSwitch[1].uw32_DelayTime = 100;//
    SetAutoRangeSwitch[1].RelayMove.InactiveFlag = LBT21XXX_RLY_Output;//
    SetAutoRangeSwitch[1].RangeChangedFlag = 1;//

    SetAutoRangeSwitch[2].uw32_DelayTime = 50;//
    SetAutoRangeSwitch[2].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect;//
    SetAutoRangeSwitch[2].RangeChangedFlag = 1;//

    SetAutoRangeSwitch[3].uw32_DelayTime = 150;
    SetAutoRangeSwitch[3].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
    SetAutoRangeSwitch[3].RangeChangedFlag = 1;

    SetAutoRangeSwitch[4].uw32_DelayTime = 50;
    SetAutoRangeSwitch[4].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;
    SetAutoRangeSwitch[4].RangeChangedFlag = 1;

    SetAutoRangeSwitch[5].uw32_DelayTime = 50;
    SetAutoRangeSwitch[5].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;
		#endif
}