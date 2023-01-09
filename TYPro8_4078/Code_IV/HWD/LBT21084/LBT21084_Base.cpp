
#include "../../Entry/includes.h"

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
St_DisposableTask LBT21084_Base::SetCtrlOpenCh[5];
St_DisposableTask LBT21084_Base::SetCtrlCloseCh[4];
St_DisposableTask LBT21084_Base::SetCCToCC[6];    //加入了多range切换，所以这里扩充到10步
St_DisposableTask LBT21084_Base::SetRstToOff[3];
St_DisposableTask LBT21084_Base::SetCtrlOpenAcim[1];
St_DisposableTask LBT21084_Base::SetCtrlCloseAcim[1];
#if (IV_AUTORANGE == 1)
St_DisposableTask LBT21084_Base::SetAutoRangeSwitch[6];
#endif
St_DisposableTask LBT21084_Base::SetOffToRst[3];
St_DisposableTask LBT21084_Base::SetOffToCur[4];
St_DisposableTask LBT21084_Base::SetOffToVlt[5];

St_DisposableTask LBT21084_Base::SetCurToOff[3];
// St_DisposableTask LBT21084_Base::SetCurToRst[3];
St_DisposableTask LBT21084_Base::SetCurToCur[7];
St_DisposableTask LBT21084_Base::SetCurToVlt[9];

St_DisposableTask LBT21084_Base::SetVltToOff[3];
// St_DisposableTask LBT21084_Base::SetVltToRst[3];
St_DisposableTask LBT21084_Base::SetVltToVlt[8];
St_DisposableTask LBT21084_Base::SetVltToCur[6];
#pragma arm section

void LBT21084_Base::FeatureEnable_Init(void)
{
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion>=BOARDTYPE_VER_E)
    {
        LBT21XXX_SampleManage::SetADG_DelayTickCount(eADG_DelayTickCountFor100us);
    }

    board_config::Feature_Enable_init.OS_Tick_Interval =800;
    board_config::Feature_Enable_init.Redundant_Safety_Mode =1;
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {

        board_config::Feature_Enable_init.RedundantVoltMode =REDUNDANT_PIN;
    }
    else
    {
        board_config::Feature_Enable_init.RedundantVoltMode =REDUNDANT_M0;
    }


    board_config::DacInfo.m_bCurrRngCount = 4;



// 	board_config::DacInfo.m_bChannelBasedClampVdac = 0;
//	board_config::DacInfo.m_uSettingTime = 1000;
    board_config::DacInfo.m_uSettingTime = 50;
    board_config::DacInfo.m_uRisingTime = LBT21084_RISING_TIME;// Rising time,  unit is 100us
    board_config::DacInfo.m_fPID_Coefficient = LBT21084_Tpid_Tsys;

    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion<=BOARDTYPE_VER_C)
    {
        BoardFeature.FINE_TUNE_MINERRORFACTOR = 2E-5;
        BoardFeature.FINE_TUNE_STEPFACTOR =6E-6;
    }
    else
    {
        BoardFeature.FINE_TUNE_MINERRORFACTOR = 1E-4;
        BoardFeature.FINE_TUNE_STEPFACTOR =5E-5;
    }

    BoardFeature.FINE_TUNE_MAXADJUSTFACTOR = 0.03F;
    if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType==BOARDTYPE_LBT21044)
        BoardFeature.FINE_TUNE_PARAMasterWaitTime=0.3584;
    else if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType==BOARDTYPE_LBT21084)
    {
        if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion <= BOARDTYPE_VER_D )
            BoardFeature.FINE_TUNE_PARAMasterWaitTime =0.7168;
        else
            BoardFeature.FINE_TUNE_PARAMasterWaitTime =0.3072;	//300us * 8chn * 128(filterdepth)
    }
    else
        BoardFeature.FINE_TUNE_PARAMasterWaitTime =0.7168;

    BoardFeature.BoolFlag.DAC16bit_AD5360_Enable = true;
//   BoardFeature.BoolFlag.DAC16bit_AD5754_Enable = true;
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion<=BOARDTYPE_VER_C)
    {
        BoardFeature.BoolFlag.Adc24bit_CS5560_Enable = true;
    }
    else
    {
        BoardFeature.BoolFlag.Adc24bit_AD7175_Enable = true;
    }

    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion<=BOARDTYPE_VER_B)
    {
        BoardFeature.BoolFlag.FAN_IIC_Enable = true;
        BoardFeature.BoolFlag.DAC16bit_AD5754_Enable = true;
    }
    else
    {
        BoardFeature.BoolFlag.FAN_PWM_Enable = true;
    }

    BoardFeature.BoolFlag.UnitDIDO_Enable = true;
    BoardFeature.BoolFlag.R_VM_Enable = true;
    BoardFeature.BoolFlag.ACIM_Enable = true;

// 	if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_D)
// 	{
// 		BoardFeature.BoolFlag.ACIM_Enable = false;  // 20180313 yy
// 	}

//   BoardFeature.BoolFlag.ACTIVE_DAC_CUR_NeedNegation_Enable = false;
//   BoardFeature.BoolFlag.ACTIVE_DAC_VH_VL_NeedNegation_Enable = false;
//   BoardFeature.BoolFlag.ClampV_Concern_Parallel_Enable = false;

    BoardFeature.ACTIVE_DAC_CUR_Chan_JumpMode = LBT21XXX_Chan_JumpMode_21084CUR;
    BoardFeature.ACTIVE_DAC_VLT_Chan_JumpMode = LBT21XXX_Chan_JumpMode_21084CUR;
    BoardFeature.ACTIVE_DAC_VH_Chan_JumpMode =  LBT21XXX_Chan_JumpMode_21084VH;
    BoardFeature.ACTIVE_DAC_VL_Chan_JumpMode =  LBT21XXX_Chan_JumpMode_21084VL;

    BoardFeature.LBT21XXXMINIMUMPLSE = 1;
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion<=BOARDTYPE_VER_E)
    {
        BoardFeature.BoolFlag.Have_DAC_Vlt_Enable = false;
        BoardFeature.BoolFlag.Have_DAC_Vclamp_Enable = false;
    }
    else if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion<=BOARDTYPE_VER_F)
    {

        BoardFeature.BoolFlag.Have_DAC_Vlt_Enable = true;
        if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType==BOARDTYPE_LBT21044)
            BoardFeature.BoolFlag.Have_DAC_Vclamp_Enable = true;
    }


//   BoardFeature.BoolFlag.PowerOffCheck_NeedNegation_Enable = false;


    BoardFeature.I_ADG_Address_Change_Mode = LBT21XXX_I_ADG_Address_Change_Mode_21084;

    unsigned char FanCtrlValue[8]= {50,50,50,60,70,80,90,100};
    memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_1], &FanCtrlValue[0], 8);
    unsigned char FanCtrlValue1[8]= {50,50,50,50,50,50,50,50};
    memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_2], &FanCtrlValue1[0], 8);
    memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_3], &FanCtrlValue1[0], 8);
    memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_4], &FanCtrlValue1[0], 8);

    AdditionalEngine();


    if((ChannelInfo_Manager::m_st_BoadType.m_u16BoardType==BOARDTYPE_LBT21044) && (ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_F))
    {
        board_config::DacInfo.m_bHaveVoltageDac = 1;
        BoardFeature.BoolFlag.Have_DAC_Vlt_Enable = true;
        BoardFeature.BoolFlag.Have_DAC_Vclamp_Enable = true;
        BoardFeature.BoolFlag.ACTIVE_DAC_VH_VL_NeedNegation_Enable = true;
// 		BoardFeature.ACTIVE_DAC_VH_Chan_JumpMode =  LBT21XXX_Chan_JumpMode_21044VH;
        BoardFeature.ACTIVE_DAC_CUR_Chan_JumpMode = LBT21XXX_Chan_JumpMode_21044CUR;
        BoardFeature.ACTIVE_DAC_VLT_Chan_JumpMode = LBT21XXX_Chan_JumpMode_21044VLT;
        BoardFeature.ACTIVE_DAC_VH_Chan_JumpMode =  LBT21XXX_Chan_JumpMode_21044VH;
        BoardFeature.ACTIVE_DAC_VL_Chan_JumpMode =  LBT21XXX_Chan_JumpMode_21044VL;
// 		BoardFeature.BoolFlag.R_HR_H_Enable =true;
        Engine();
    }
    else if ((ChannelInfo_Manager::m_st_BoadType.m_u16BoardType==BOARDTYPE_LBT21084) && (ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_F))
    {
        board_config::DacInfo.m_bHaveVoltageDac = 1;
        BoardFeature.BoolFlag.Have_DAC_Vlt_Enable = true;
        BoardFeature.BoolFlag.Have_DAC_Vclamp_Enable = false;
        BoardFeature.BoolFlag.ACTIVE_DAC_VLT_NeedNegation_Enable = true;
        Engine();
    }
    else
    {
        OS_LogicProcess::InitBoardEngine(	&SetCtrlOpenCh[0],	sizeof(SetCtrlOpenCh)		/		sizeof(St_DisposableTask),
                                            &SetCtrlCloseCh[0], sizeof(SetCtrlCloseCh)	/		sizeof(St_DisposableTask),
                                            &SetCCToCC[0], 			sizeof(SetCCToCC)				/		sizeof(St_DisposableTask),
                                            &SetRstToOff[0],		sizeof(SetRstToOff)			/		sizeof(St_DisposableTask)
                                        );

    }
    EngineStuff();
}
void LBT21084_Base::AdditionalEngine(void)
{
    // ACIM Engine
    OS_LogicProcess::SetCtrlAcimOn.st_Disposabletask_ptr = &LBT21084_Base::SetCtrlOpenAcim[0];
    OS_LogicProcess::SetCtrlAcimOn.stepLen = sizeof(SetCtrlOpenAcim) / sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlAcimOff.st_Disposabletask_ptr = &LBT21084_Base::SetCtrlCloseAcim[0];
    OS_LogicProcess::SetCtrlAcimOff.stepLen = sizeof(SetCtrlCloseAcim) / sizeof(St_DisposableTask);

#if (IV_AUTORANGE == 1)
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion <=BOARDTYPE_VER_F)
    {
// 		if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType==BOARDTYPE_LBT21084)
// 		{
        OS_LogicProcess::SetAutoRangeSwitch.st_Disposabletask_ptr = &LBT21084_Base::SetAutoRangeSwitch[0];
        OS_LogicProcess::SetAutoRangeSwitch.stepLen = sizeof(SetAutoRangeSwitch)/sizeof(St_DisposableTask);
// 		}
    }
#endif
}
void LBT21084_Base::EngineStuff(void)
{
    memset(&SetCtrlOpenAcim,	0,    sizeof(SetCtrlOpenAcim));
    memset(&SetCtrlCloseAcim,	0,    sizeof(SetCtrlCloseAcim));
    memset(&SetRstToOff,     0,    sizeof(SetRstToOff));
    memset(&SetOffToRst,     0,    sizeof(SetOffToRst));
#if (IV_AUTORANGE == 1)
    memset(&SetAutoRangeSwitch,	0,    sizeof(SetAutoRangeSwitch));
#endif
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion>=BOARDTYPE_VER_F)
    {
// 	  memset(&SetRstToCur,   0,    sizeof(SetRstToCur));
// 	  memset(&SetRstToVlt,  0,    sizeof(SetRstToVlt));
        memset(&SetOffToCur,       0,    sizeof(SetOffToCur));
        memset(&SetOffToVlt,     0,    sizeof(SetOffToVlt));

        memset(&SetCurToCur,   0,    sizeof(SetCurToCur));
        memset(&SetCurToVlt,  0,    sizeof(SetCurToVlt));
        memset(&SetCurToOff,       0,    sizeof(SetCurToOff));
// 	  memset(&SetCurToRst,     0,    sizeof(SetCurToRst));

        memset(&SetVltToVlt,   0,    sizeof(SetVltToVlt));
        memset(&SetVltToCur,  0,    sizeof(SetVltToCur));
        memset(&SetVltToOff,       0,    sizeof(SetVltToOff));
// 	  memset(&SetVltToRst,     0,    sizeof(SetVltToRst));
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx//
        //rst--cur ok	/////////////////////////////////////////////////////////////////////////////////////////////////
// 		SetRstToCur[0].uw32_DelayTime = 100;
// 		SetRstToCur[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO | ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
// 		SetRstToCur[0].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect | LBT21XXX_RLY_DAMPING;
// 		SetRstToVlt[3].DacMove.DacFlag = ACTIVE_DAC_VLT;
//
// 		SetRstToVlt[4].uw32_DelayTime = 100;
// 		SetRstToVlt[4].RelayMove.ActiveFlag = LBT21XXX_RLY_IVS;

        //off-cur ok/////////////////////////////////////////////////////////////////////////////////////////////
        SetOffToCur[0].uw32_DelayTime = 100;
        SetOffToCur[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO | ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
        SetOffToCur[0].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect | LBT21XXX_RLY_DAMPING;

        SetOffToCur[1].uw32_DelayTime = 150;
        SetOffToCur[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;

        SetOffToCur[2].uw32_DelayTime = 100;
        SetOffToCur[2].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;

        SetOffToCur[3].uw32_DelayTime = 50;
        SetOffToCur[3].DacMove.DacFlag = ACTIVE_DAC_CUR | ACTIVE_DAC_VH | ACTIVE_DAC_VL;

        //off-VLT ok//////////////////////////////////////////////////////////////////////////////////////
        SetOffToVlt[0].uw32_DelayTime = 100;
        SetOffToVlt[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO | ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
        SetOffToVlt[0].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect | LBT21XXX_RLY_DAMPING;

        SetOffToVlt[1].uw32_DelayTime = 150;
        SetOffToVlt[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;

        SetOffToVlt[2].uw32_DelayTime = 100;
        SetOffToVlt[2].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;

        SetOffToVlt[3].uw32_DelayTime = 50;
        SetOffToVlt[3].DacMove.DacFlag = ACTIVE_DAC_VLT;

        SetOffToVlt[4].uw32_DelayTime = 100;
        SetOffToVlt[4].RelayMove.ActiveFlag = LBT21XXX_RLY_IVS;

        //CC-CC ok///////////////////////////////////////////////////////////////////////////////////////////
        SetCurToCur[0].uw32_DelayTime = 100;
        SetCurToCur[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
        SetCurToCur[0].RangeChangedFlag = 1;

        SetCurToCur[1].uw32_DelayTime = 150;
        SetCurToCur[1].RelayMove.ActiveFlag = LBT21XXX_RLY_DAMPING;

        SetCurToCur[2].uw32_DelayTime = 100;
        SetCurToCur[2].RelayMove.InactiveFlag = LBT21XXX_RLY_Output;
        SetCurToCur[2].RangeChangedFlag = 1;

        SetCurToCur[3].uw32_DelayTime = 100;
        SetCurToCur[3].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect;
        SetCurToCur[3].RangeChangedFlag = 1;

        SetCurToCur[4].uw32_DelayTime = 100;
        SetCurToCur[4].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
        SetCurToCur[4].RangeChangedFlag = 1;

        SetCurToCur[5].uw32_DelayTime = 150;
        SetCurToCur[5].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;
        SetCurToCur[5].RangeChangedFlag = 1;

        SetCurToCur[6].uw32_DelayTime = 100;
        SetCurToCur[6].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

        //CC-CV ok//////////////////////////////////////////////////////////////////////////////////////////
        SetCurToVlt[0].uw32_DelayTime = 100;
        SetCurToVlt[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO |ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
        SetCurToVlt[0].RangeChangedFlag = 1;

        SetCurToVlt[1].uw32_DelayTime = 150;
        SetCurToVlt[1].RelayMove.ActiveFlag = LBT21XXX_RLY_DAMPING;

        SetCurToVlt[2].uw32_DelayTime = 150;
        SetCurToVlt[2].RelayMove.InactiveFlag = LBT21XXX_RLY_Output;
        SetCurToVlt[2].RangeChangedFlag = 1;

        SetCurToVlt[3].uw32_DelayTime = 100;
        SetCurToVlt[3].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect ;
        SetCurToVlt[3].RangeChangedFlag = 1;

        SetCurToVlt[4].uw32_DelayTime = 100;
        SetCurToVlt[4].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
        SetCurToVlt[4].RangeChangedFlag = 1;

        SetCurToVlt[5].uw32_DelayTime = 100;
        SetCurToVlt[5].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;
        SetCurToVlt[5].RangeChangedFlag = 1;

        SetCurToVlt[6].uw32_DelayTime = 20;
        SetCurToVlt[6].DacMove.DacFlag = ACTIVE_DAC_VLT;

        SetCurToVlt[7].uw32_DelayTime = 50;
        SetCurToVlt[7].RelayMove.ActiveFlag = LBT21XXX_RLY_IVS;

        SetCurToVlt[8].uw32_DelayTime = 100;
        SetCurToVlt[8].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

        //CC-OFF ok///////////////////////////////////////////////////////////////////////////////////////
        SetCurToOff[0].uw32_DelayTime = 50;
        SetCurToOff[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO | ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

        SetCurToOff[1].uw32_DelayTime = 150;
        SetCurToOff[1].RelayMove.InactiveFlag = LBT21XXX_RLY_Output;

        SetCurToOff[2].uw32_DelayTime = 50;
        SetCurToOff[2].RelayMove.InactiveFlag =  LBT21XXX_RLY_DAMPING | LBT21XXX_RLY_ACIM ;

        //CC-RST ok////////////////////////////////////////////////////////////////////////////////////////
// 		SetCurToRst[0].uw32_DelayTime = 50;
// 		SetCurToRst[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO|ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
//
// 		SetCurToRst[1].uw32_DelayTime = 150;
// 		SetCurToRst[1].RelayMove.InactiveFlag = LBT21XXX_RLY_Output;

// 		SetCurToRst[2].uw32_DelayTime = 50;
// 		SetCurToRst[2].RelayMove.InactiveFlag =  LBT21XXX_RLY_DAMPING;
// 		SetCurToRst[2].RelayMove.InactiveFlag = LBT21XXX_RLY_ACIM ;
        //CV-CV ok///////////////////////////////////////////////////////////////////////////////////////
        SetVltToVlt[0].uw32_DelayTime = 100;
        SetVltToVlt[0].RelayMove.InactiveFlag = LBT21XXX_RLY_IVS;
        SetVltToVlt[0].RangeChangedFlag = 1;

        SetVltToVlt[1].uw32_DelayTime = 100;
        SetVltToVlt[1].RelayMove.InactiveFlag = LBT21XXX_RLY_Output;
        SetVltToVlt[1].RangeChangedFlag = 1;

        SetVltToVlt[2].uw32_DelayTime = 100;
        SetVltToVlt[2].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect;
        SetVltToVlt[2].RangeChangedFlag = 1;

        SetVltToVlt[3].uw32_DelayTime = 100;
        SetVltToVlt[3].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
        SetVltToVlt[3].RangeChangedFlag = 1;

        SetVltToVlt[4].uw32_DelayTime = 100;
        SetVltToVlt[4].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;
        SetVltToVlt[4].RangeChangedFlag = 1;

        SetVltToVlt[5].uw32_DelayTime = 50;
        SetVltToVlt[5].DacMove.DacFlag = ACTIVE_DAC_VLT;
        SetVltToVlt[5].RelayMove.ActiveFlag = LBT21XXX_RLY_DAMPING;

        SetVltToVlt[6].uw32_DelayTime = 100;
        SetVltToVlt[6].RelayMove.ActiveFlag = LBT21XXX_RLY_IVS;
        SetVltToVlt[6].RangeChangedFlag = 1;

        SetVltToVlt[7].uw32_DelayTime = 0;

        //CV-CC ok///////////////////////////////////////////////////////////////////////////////////////////////
        SetVltToCur[0].uw32_DelayTime = 100;
        //SetVltToCur[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO | ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
        SetVltToCur[0].RelayMove.ActiveFlag = LBT21XXX_RLY_DAMPING;
        SetVltToCur[0].RelayMove.InactiveFlag =  LBT21XXX_RLY_IVS;

        SetVltToCur[1].uw32_DelayTime = 100;
        SetVltToCur[1].RelayMove.InactiveFlag = LBT21XXX_RLY_Output;
        SetVltToCur[1].RangeChangedFlag = 1;

        SetVltToCur[2].uw32_DelayTime = 100;
        SetVltToCur[2].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect;
        SetVltToCur[2].RangeChangedFlag = 1;

        SetVltToCur[3].uw32_DelayTime = 100;
        SetVltToCur[3].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
        SetVltToCur[3].RangeChangedFlag = 1;

        SetVltToCur[4].uw32_DelayTime = 100;
        SetVltToCur[4].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;
        SetVltToCur[4].RangeChangedFlag = 1;

        SetVltToCur[5].uw32_DelayTime = 50;
        SetVltToCur[5].DacMove.DacFlag = ACTIVE_DAC_CUR | ACTIVE_DAC_VLT_ZERO | ACTIVE_DAC_VH | ACTIVE_DAC_VL;

        //CV-OFF ok////////////////////////////////////////////////////////////////////////////////////
        SetVltToOff[0].uw32_DelayTime = 50;
        SetVltToOff[0].RelayMove.InactiveFlag = LBT21XXX_RLY_IVS;

        SetVltToOff[1].uw32_DelayTime = 150;
        SetVltToOff[1].DacMove.DacFlag = ACTIVE_DAC_VLT_ZERO;
        SetVltToOff[1].RelayMove.InactiveFlag = LBT21XXX_RLY_Output;

        SetVltToOff[2].uw32_DelayTime = 150;
        SetVltToOff[2].RelayMove.InactiveFlag = LBT21XXX_RLY_DAMPING| LBT21XXX_RLY_ACIM ;

        //CV-RST ok//////////////////////////////////////////////////////////////////////////////////////
// 		SetVltToRst[0].uw32_DelayTime = 50;
// 		SetVltToRst[0].RelayMove.InactiveFlag = LBT21XXX_RLY_IVS;

// 		SetVltToRst[1].uw32_DelayTime = 150;
// 		SetVltToRst[1].RelayMove.InactiveFlag = LBT21XXX_RLY_Output;
//
// 		SetVltToRst[2].uw32_DelayTime = 150;
// 		SetVltToRst[2].RelayMove.InactiveFlag = LBT21XXX_RLY_DAMPING;

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx//
    }
    else
    {
        memset(&SetCtrlOpenCh,   0,    sizeof(SetCtrlOpenCh));
        memset(&SetCtrlCloseCh,  0,    sizeof(SetCtrlCloseCh));
        memset(&SetCCToCC,       0,    sizeof(SetCCToCC));

#if (IV_AUTORANGE == 1)
        memset(&SetAutoRangeSwitch,	0,    sizeof(SetAutoRangeSwitch));
#endif
        SetCtrlOpenCh[0].uw32_DelayTime = 10;
        SetCtrlOpenCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

        SetCtrlOpenCh[1].uw32_DelayTime = 50;
        SetCtrlOpenCh[1].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect;

        SetCtrlOpenCh[2].uw32_DelayTime = 100;
        SetCtrlOpenCh[2].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;

        SetCtrlOpenCh[3].uw32_DelayTime = 50;
        SetCtrlOpenCh[3].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;

        SetCtrlOpenCh[4].uw32_DelayTime = 10;
        SetCtrlOpenCh[4].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

        SetCtrlCloseCh[0].uw32_DelayTime = 100;
        SetCtrlCloseCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
        //	SetCtrlCloseCh[0].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect;

        SetCtrlCloseCh[1].uw32_DelayTime = 100;
        SetCtrlCloseCh[1].RelayMove.InactiveFlag = LBT21XXX_RLY_Output;

        //	SetCtrlCloseCh[2].uw32_DelayTime = 50;
        //	SetCtrlCloseCh[2].RelayMove.InactiveFlag = LBT210_RLY_Protect;

        SetCtrlCloseCh[2].uw32_DelayTime = 50;
        SetCtrlCloseCh[2].RelayMove.InactiveFlag = LBT21XXX_RLY_ACIM;

        SetCtrlCloseCh[3].uw32_DelayTime = 10;
        SetCtrlCloseCh[3].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

        SetCCToCC[0].uw32_DelayTime = 50;
        SetCCToCC[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
        //SetCCToCC[0].RelayMove.InactiveFlag = LBT21XXX_RLY_Output;
        //SetCCToCC[0].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect;
        SetCCToCC[0].RangeChangedFlag = 1;

        SetCCToCC[1].uw32_DelayTime = 100;
        SetCCToCC[1].RelayMove.InactiveFlag = LBT21XXX_RLY_Output;
        SetCCToCC[1].RangeChangedFlag = 1;

        SetCCToCC[2].uw32_DelayTime = 50;
        SetCCToCC[2].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect;
        SetCCToCC[2].RangeChangedFlag = 1;

        SetCCToCC[3].uw32_DelayTime = 100;
        SetCCToCC[3].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
        SetCCToCC[3].RangeChangedFlag = 1;

        SetCCToCC[4].uw32_DelayTime = 50;
        SetCCToCC[4].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;
        SetCCToCC[4].RangeChangedFlag = 1;

        SetCCToCC[5].uw32_DelayTime = 100;  //50;
        SetCCToCC[5].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

    }
    SetRstToOff[0].uw32_DelayTime = 100;
    SetRstToOff[0].RelayMove.InactiveFlag = LBT21XXX_RLY_Output;  //为防止并联时从通道状态被意外设置成CCS_OFF ,保险起见这个地方也处理继电器关闭  dirui 2015/09/16
    SetRstToOff[0].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT; //需要放开clamp;
    SetRstToOff[0].RangeChangedFlag = 0;

    SetRstToOff[1].uw32_DelayTime = 50;
    SetRstToOff[1].RelayMove.InactiveFlag = LBT21XXX_RLY_ACIM;

    SetOffToRst[0].uw32_DelayTime =50;
    SetOffToRst[0].RelayMove.InactiveFlag = LBT21XXX_RLY_Output;
    SetOffToRst[0].RangeChangedFlag = 0;


    SetCtrlOpenAcim[0].uw32_DelayTime = 100;
    SetCtrlOpenAcim[0].RelayMove.ActiveFlag = LBT21XXX_RLY_ACIM;

    SetCtrlCloseAcim[0].uw32_DelayTime = 100;
    SetCtrlCloseAcim[0].RelayMove.InactiveFlag = LBT21XXX_RLY_ACIM;
#if (IV_AUTORANGE == 1)
    //自动量程切换时序
    SetAutoRangeSwitch[0].uw32_DelayTime = 50;
    SetAutoRangeSwitch[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
    SetAutoRangeSwitch[0].RangeChangedFlag = 1;

    SetAutoRangeSwitch[1].uw32_DelayTime = 100;
    SetAutoRangeSwitch[1].RelayMove.InactiveFlag = LBT21XXX_RLY_Output;
    SetAutoRangeSwitch[1].RangeChangedFlag = 1;

    SetAutoRangeSwitch[2].uw32_DelayTime = 50;
    SetAutoRangeSwitch[2].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect;
    SetAutoRangeSwitch[2].RangeChangedFlag = 1;

    SetAutoRangeSwitch[3].uw32_DelayTime = 100;
    SetAutoRangeSwitch[3].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
    SetAutoRangeSwitch[3].RangeChangedFlag = 1;

    SetAutoRangeSwitch[4].uw32_DelayTime = 50;
    SetAutoRangeSwitch[4].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;
    SetAutoRangeSwitch[4].RangeChangedFlag = 1;

    SetAutoRangeSwitch[5].uw32_DelayTime = 10;  //50;
    SetAutoRangeSwitch[5].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;
#endif
}
void LBT21084_Base::Engine(void)
{
    OS_LogicProcess::SetCtrlRstToCur.st_Disposabletask_ptr =&LBT21084_Base::SetOffToCur[0];        //&LBT21084_Base::SetRstToCur[0];
    OS_LogicProcess::SetCtrlRstToCur.stepLen =  sizeof(SetOffToCur)	/	sizeof(St_DisposableTask); //sizeof(SetRstToCur)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlRstToVlt.st_Disposabletask_ptr =&LBT21084_Base::SetOffToVlt[0];   //&LBT21084_Base::SetRstToVlt[0];
    OS_LogicProcess::SetCtrlRstToVlt.stepLen = sizeof(SetOffToVlt)	/	sizeof(St_DisposableTask);  //sizeof(SetRstToVlt)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlRstToOff.st_Disposabletask_ptr =&LBT21084_Base::SetRstToOff[0];;
    OS_LogicProcess::SetCtrlRstToOff.stepLen = sizeof(SetRstToOff)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlOffToRst.st_Disposabletask_ptr =&LBT21084_Base::SetOffToRst[0];
    OS_LogicProcess::SetCtrlOffToRst.stepLen = sizeof(SetOffToRst)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlOffToCur.st_Disposabletask_ptr =& LBT21084_Base::SetOffToCur[0];
    OS_LogicProcess::SetCtrlOffToCur.stepLen = sizeof(SetOffToCur)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlOffToVlt.st_Disposabletask_ptr =&LBT21084_Base::SetOffToVlt[0];
    OS_LogicProcess::SetCtrlOffToVlt.stepLen = sizeof(SetOffToVlt)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlCurToCur.st_Disposabletask_ptr =&LBT21084_Base::SetCurToCur[0];
    OS_LogicProcess::SetCtrlCurToCur.stepLen = sizeof(SetCurToCur)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlCurToVlt.st_Disposabletask_ptr =&LBT21084_Base::SetCurToVlt[0];
    OS_LogicProcess::SetCtrlCurToVlt.stepLen = sizeof(SetCurToVlt)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlCurToOff.st_Disposabletask_ptr =&LBT21084_Base::SetCurToOff[0];
    OS_LogicProcess::SetCtrlCurToOff.stepLen = sizeof(SetCurToOff)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlCurToRst.st_Disposabletask_ptr = &LBT21084_Base::SetCurToOff[0];  //&LBT21084_Base::SetCurToRst[0];
    OS_LogicProcess::SetCtrlCurToRst.stepLen =  sizeof(SetCurToOff)	/	sizeof(St_DisposableTask); //sizeof(SetCurToRst)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlVltToVlt.st_Disposabletask_ptr =&LBT21084_Base::SetVltToVlt[0];
    OS_LogicProcess::SetCtrlVltToVlt.stepLen = sizeof(SetVltToVlt)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlVltToCur.st_Disposabletask_ptr =&LBT21084_Base::SetVltToCur[0];
    OS_LogicProcess::SetCtrlVltToCur.stepLen = sizeof(SetVltToCur)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlVltToOff.st_Disposabletask_ptr =&LBT21084_Base::SetVltToOff[0];
    OS_LogicProcess::SetCtrlVltToOff.stepLen = sizeof(SetVltToOff)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlVltToRst.st_Disposabletask_ptr =&LBT21084_Base::SetVltToOff[0];  //&LBT21084_Base::SetVltToRst[0];
    OS_LogicProcess::SetCtrlVltToRst.stepLen = sizeof(SetVltToOff)	/	sizeof(St_DisposableTask); //sizeof(SetVltToRst)	/	sizeof(St_DisposableTask);

}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
