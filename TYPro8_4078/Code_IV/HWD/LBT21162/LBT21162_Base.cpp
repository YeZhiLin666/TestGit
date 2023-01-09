
#include "../../Entry/includes.h"

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
St_DisposableTask LBT21162_Base::SetCtrlOpenCh[5];
St_DisposableTask LBT21162_Base::SetCtrlCloseCh[4];
St_DisposableTask LBT21162_Base::SetCCToCC[4];    //加入了多range切换，所以这里扩充到10步
St_DisposableTask LBT21162_Base::SetRstToOff[2];
St_DisposableTask LBT21162_Base::SetCtrlOpenAcim[1];
St_DisposableTask LBT21162_Base::SetCtrlCloseAcim[1];
#if (IV_AUTORANGE == 1)
St_DisposableTask LBT21162_Base::SetAutoRangeSwitch[4];
#endif
#pragma arm section

void LBT21162_Base::FeatureEnable_Init(void)
{
		if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion>=BOARDTYPE_VER_E)
		{
			LBT21XXX_SampleManage::SetADG_DelayTickCount(eADG_DelayTickCountFor100us);
		}

    board_config::Feature_Enable_init.Redundant_Safety_Mode =1;
    board_config::Feature_Enable_init.RedundantVoltMode = REDUNDANT_M0;
    board_config::Feature_Enable_init.OS_Tick_Interval =800;



    board_config::DacInfo.m_bCurrRngCount = 2;



// 	board_config::DacInfo.m_bChannelBasedClampVdac = 0;
//	board_config::DacInfo.m_uSettingTime = 1000;
    board_config::DacInfo.m_uSettingTime = 50;
    board_config::DacInfo.m_uRisingTime = LBT21162_RISING_TIME;// Rising time,  unit is 100us
    board_config::DacInfo.m_fPID_Coefficient = LBT21162_Tpid_Tsys;

    BoardFeature.FINE_TUNE_MINERRORFACTOR =2E-5;
    BoardFeature.FINE_TUNE_STEPFACTOR = 6E-6;
    BoardFeature.FINE_TUNE_MAXADJUSTFACTOR = 0.03f;
	if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion <= BOARDTYPE_VER_D)
		BoardFeature.FINE_TUNE_PARAMasterWaitTime = 1.4336;
	else
		BoardFeature.FINE_TUNE_PARAMasterWaitTime = 0.6144;


// 	BoardFeature.BoolFlag.Adc24bit_CS5560_Enable = false;
    BoardFeature.BoolFlag.Adc24bit_AD7175_Enable = true;
    BoardFeature.BoolFlag.DAC16bit_AD5360_Enable = true;
// 	BoardFeature.BoolFlag.DAC16bit_AD5754_Enable = false;

// 	BoardFeature.BoolFlag.FAN_IIC_Enable = false;
    BoardFeature.BoolFlag.FAN_PWM_Enable = true;

// 	BoardFeature.BoolFlag.UnitDIDO_Enable = false;
    BoardFeature.BoolFlag.R_VM_Enable = true;
    BoardFeature.BoolFlag.ACIM_Enable = true;

// 	BoardFeature.BoolFlag.ACTIVE_DAC_CUR_NeedNegation_Enable = false;
// 	BoardFeature.BoolFlag.ACTIVE_DAC_VH_VL_NeedNegation_Enable = false;
// 	BoardFeature.BoolFlag.ClampV_Concern_Parallel_Enable = false;
//   BoardFeature.ACTIVE_DAC_CUR_Chan_JumpMode = LBT21XXX_Chan_JumpMode_None;
// 	BoardFeature.ACTIVE_DAC_VH_Chan_JumpMode =  LBT21XXX_Chan_JumpMode_None;
// 	BoardFeature.ACTIVE_DAC_VL_Chan_JumpMode =  LBT21XXX_Chan_JumpMode_None;
// 	BoardFeature.ACTIVE_DAC_VLT_Chan_JumpMode = LBT21XXX_Chan_JumpMode_None;
    BoardFeature.LBT21XXXMINIMUMPLSE = 1;
    BoardFeature.BoolFlag.Have_DAC_Vlt_Enable = true;
//   BoardFeature.BoolFlag.Have_DAC_Vclamp_Enable = false;

// 	BoardFeature.BoolFlag.PowerOffCheck_NeedNegation_Enable = false;

// 	BoardFeature.I_ADG_Address_Change_Mode = LBT21XXX_I_ADG_Address_Change_Mode_None;
    
    unsigned char FanCtrlValue[8]= {50,50,50,60,70,80,90,100};
    memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_1], &FanCtrlValue[0], 8);
    unsigned char FanCtrlValue1[8]= {50,50,50,50,50,50,50,50};
    memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_2], &FanCtrlValue1[0], 8);
    //memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_3], &FanCtrlValue1[0], 8);
    //memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_4], &FanCtrlValue1[0], 8);
    //Engine();
    OS_LogicProcess::InitBoardEngine(	&SetCtrlOpenCh[0],	sizeof(SetCtrlOpenCh)		/		sizeof(St_DisposableTask),
                                        &SetCtrlCloseCh[0],        sizeof(SetCtrlCloseCh)	/		sizeof(St_DisposableTask),
                                        &SetCCToCC[0], 			sizeof(SetCCToCC)				/		sizeof(St_DisposableTask),
                                        &SetRstToOff[0],		sizeof(SetRstToOff)			/		sizeof(St_DisposableTask)
                                    );
    AdditionalEngine();
    EngineStuff();
}
void LBT21162_Base::AdditionalEngine(void)
{
    // ACIM Engine
    OS_LogicProcess::SetCtrlAcimOn.st_Disposabletask_ptr = &LBT21162_Base::SetCtrlOpenAcim[0];
    OS_LogicProcess::SetCtrlAcimOn.stepLen = sizeof(SetCtrlOpenAcim) / sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlAcimOff.st_Disposabletask_ptr = &LBT21162_Base::SetCtrlCloseAcim[0];
    OS_LogicProcess::SetCtrlAcimOff.stepLen = sizeof(SetCtrlCloseAcim) / sizeof(St_DisposableTask);
}

void LBT21162_Base::EngineStuff(void)
{
    memset(&SetCtrlOpenCh,   0,    sizeof(SetCtrlOpenCh));
    memset(&SetCtrlCloseCh,  0,    sizeof(SetCtrlCloseCh));
    memset(&SetCCToCC,       0,    sizeof(SetCCToCC));
    memset(&SetRstToOff,     0,    sizeof(SetRstToOff));
    memset(&SetCtrlOpenAcim,	0,    sizeof(SetCtrlOpenAcim));
    memset(&SetCtrlCloseAcim,	0,    sizeof(SetCtrlCloseAcim));
#if (IV_AUTORANGE == 1)
			memset(&SetAutoRangeSwitch,	0,    sizeof(SetAutoRangeSwitch));
		#endif

    SetCtrlOpenCh[0].uw32_DelayTime = 200;
    SetCtrlOpenCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

    SetCtrlOpenCh[1].uw32_DelayTime = 100;
    SetCtrlOpenCh[1].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect;

    SetCtrlOpenCh[2].uw32_DelayTime = 150;
    SetCtrlOpenCh[2].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;

    SetCtrlOpenCh[3].uw32_DelayTime = 50;
    SetCtrlOpenCh[3].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;

    SetCtrlOpenCh[4].uw32_DelayTime = 100;
    SetCtrlOpenCh[4].DacMove.DacFlag = ACTIVE_DAC_CUR;

    SetCtrlCloseCh[0].uw32_DelayTime = 100;
    SetCtrlCloseCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
    SetCtrlCloseCh[0].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect;

    SetCtrlCloseCh[1].uw32_DelayTime = 150;
    SetCtrlCloseCh[1].RelayMove.InactiveFlag = LBT21XXX_RLY_Output;

    SetCtrlCloseCh[2].uw32_DelayTime = 50;
    SetCtrlCloseCh[2].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect | LBT21XXX_RLY_ACIM;

    SetCtrlCloseCh[3].uw32_DelayTime = 10;
    SetCtrlCloseCh[3].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

    SetCCToCC[0].uw32_DelayTime = 100;
    SetCCToCC[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
    SetCCToCC[0].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect;
    SetCCToCC[0].RangeChangedFlag = 1;

    SetCCToCC[1].uw32_DelayTime = 150;
    SetCCToCC[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
    SetCCToCC[1].RangeChangedFlag = 1;

    SetCCToCC[2].uw32_DelayTime = 50;
    SetCCToCC[2].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;
    SetCCToCC[2].RangeChangedFlag = 1;

    SetCCToCC[3].uw32_DelayTime = 50;
    SetCCToCC[3].DacMove.DacFlag = ACTIVE_DAC_CUR;

    SetRstToOff[0].uw32_DelayTime = 100;
    SetRstToOff[0].RelayMove.InactiveFlag = LBT21XXX_RLY_Output | LBT21XXX_RLY_ACIM;  //为防止并联时从通道状态被意外设置成CCS_OFF ,保险起见这个地方也处理继电器关闭  dirui 2015/09/16
// 	SetRstToOff[0].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT; //需要放开clamp;
    SetRstToOff[0].RangeChangedFlag = 0;

    SetRstToOff[1].uw32_DelayTime = 0;
    SetRstToOff[1].RelayMove.ActiveFlag = 0;
// 	SetRstToOff[1].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
    SetRstToOff[1].RangeChangedFlag = 0;

    SetCtrlOpenAcim[0].uw32_DelayTime = 100;
    SetCtrlOpenAcim[0].RelayMove.ActiveFlag = LBT21XXX_RLY_ACIM;

    SetCtrlCloseAcim[0].uw32_DelayTime = 100;
    SetCtrlCloseAcim[0].RelayMove.InactiveFlag = LBT21XXX_RLY_ACIM;
		#if (IV_AUTORANGE == 1)
	SetAutoRangeSwitch[0].uw32_DelayTime = 100;
    SetAutoRangeSwitch[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
    SetAutoRangeSwitch[0].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect;
    SetAutoRangeSwitch[0].RangeChangedFlag = 1;

    SetAutoRangeSwitch[1].uw32_DelayTime = 150;
    SetAutoRangeSwitch[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
    SetAutoRangeSwitch[1].RangeChangedFlag = 1;

    SetAutoRangeSwitch[2].uw32_DelayTime = 50;
    SetAutoRangeSwitch[2].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;
    SetAutoRangeSwitch[2].RangeChangedFlag = 1;

    SetAutoRangeSwitch[3].uw32_DelayTime = 50;
    SetAutoRangeSwitch[3].DacMove.DacFlag = ACTIVE_DAC_CUR;
	#endif
}
/*********************************************************************************
**                            End Of File
*********************************************************************************/
