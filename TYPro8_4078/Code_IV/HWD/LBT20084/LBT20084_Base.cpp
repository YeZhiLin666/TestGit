
#include "../../Entry/includes.h"

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
St_DisposableTask LBT20084_Base::SetCtrlOpenCh[6];
St_DisposableTask LBT20084_Base::SetCtrlCloseCh[4];
St_DisposableTask LBT20084_Base::SetCCToCC[5];    //加入了多range切换，所以这里扩充到10步
St_DisposableTask LBT20084_Base::SetRstToOff[3];
St_DisposableTask LBT20084_Base::SetCtrlOpenAcim[1];
St_DisposableTask LBT20084_Base::SetCtrlCloseAcim[1];
St_DisposableTask LBT20084_Base::SetRstToCur[5];
St_DisposableTask LBT20084_Base::SetRstToVlt[6];

St_DisposableTask LBT20084_Base::SetOffToRst[3];
St_DisposableTask LBT20084_Base::SetOffToCur[5];
St_DisposableTask LBT20084_Base::SetOffToVlt[6];

St_DisposableTask LBT20084_Base::SetCurToOff[4];
St_DisposableTask LBT20084_Base::SetCurToRst[4];
St_DisposableTask LBT20084_Base::SetCurToCur[8];
St_DisposableTask LBT20084_Base::SetCurToVlt[10];


St_DisposableTask LBT20084_Base::SetVltToOff[4];
St_DisposableTask LBT20084_Base::SetVltToRst[4];
St_DisposableTask LBT20084_Base::SetVltToVlt[9];
St_DisposableTask LBT20084_Base::SetVltToCur[11];
#if (IV_AUTORANGE == 1)
St_DisposableTask LBT20084_Base::SetAutoRangeSwitch[8];
#endif
#pragma arm section

void LBT20084_Base::FeatureEnable_Init(void)
{
    board_config::Feature_Enable_init.Redundant_Safety_Mode =1;

		if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion>=BOARDTYPE_VER_E)
		{
			LBT21XXX_SampleManage::SetADG_DelayTickCount(eADG_DelayTickCountFor100us);
		}

    board_config::Feature_Enable_init.RedundantVoltMode =REDUNDANT_M0;
    board_config::Feature_Enable_init.OS_Tick_Interval =800;

// 	if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
// 	{
// 		board_config::DacInfo.m_bHaveVoltageDac = 0;
// 	}
// 	else if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_B)
// 	{
// 		board_config::DacInfo.m_bHaveVoltageDac = 1;
//   }
    board_config::DacInfo.m_bCurrRngCount = 4;



// 	board_config::DacInfo.m_bChannelBasedClampVdac = 0;
//	board_config::DacInfo.m_uSettingTime = 1000;
    board_config::DacInfo.m_uSettingTime = 50;
    board_config::DacInfo.m_uRisingTime = LBT20084_RISING_TIME;// Rising time,  unit is 100us
    board_config::DacInfo.m_fPID_Coefficient = LBT20084_Tpid_Tsys;

    BoardFeature.FINE_TUNE_MINERRORFACTOR = 2E-5;
    BoardFeature.FINE_TUNE_STEPFACTOR =6E-6;
    BoardFeature.FINE_TUNE_MAXADJUSTFACTOR = 0.03F;
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion <= BOARDTYPE_VER_D )
        BoardFeature.FINE_TUNE_PARAMasterWaitTime =0.7168;
    else
        BoardFeature.FINE_TUNE_PARAMasterWaitTime =0.3072;	//300us * 8chn * 128(filterdepth)

    BoardFeature.BoolFlag.DAC16bit_AD5360_Enable = true;
    BoardFeature.BoolFlag.DAC16bit_AD5754_Enable = true;

// 	BoardFeature.BoolFlag.Adc24bit_CS5560_Enable = false;

    BoardFeature.BoolFlag.Adc24bit_AD7175_Enable = true;

    BoardFeature.BoolFlag.FAN_IIC_Enable = true;
    BoardFeature.BoolFlag.FAN_PWM_Enable = true;

    BoardFeature.BoolFlag.UnitDIDO_Enable = true;
    BoardFeature.BoolFlag.R_VM_Enable = true;
    BoardFeature.BoolFlag.ACIM_Enable = true;

//   BoardFeature.BoolFlag.ACTIVE_DAC_CUR_NeedNegation_Enable = false;
//   BoardFeature.BoolFlag.ACTIVE_DAC_VH_VL_NeedNegation_Enable = false;
//   BoardFeature.BoolFlag.ClampV_Concern_Parallel_Enable = false;
    BoardFeature.BoolFlag.ACTIVE_DAC_VLT_NeedNegation_Enable = true;

    BoardFeature.ACTIVE_DAC_CUR_Chan_JumpMode = LBT21XXX_Chan_JumpMode_21084CUR;
    BoardFeature.ACTIVE_DAC_VLT_Chan_JumpMode = LBT21XXX_Chan_JumpMode_20084VLT;
    BoardFeature.ACTIVE_DAC_VH_Chan_JumpMode =  LBT21XXX_Chan_JumpMode_21084VH;
    BoardFeature.ACTIVE_DAC_VL_Chan_JumpMode =  LBT21XXX_Chan_JumpMode_21084VL;

    BoardFeature.LBT21XXXMINIMUMPLSE = 1;
    BoardFeature.BoolFlag.Have_DAC_Vlt_Enable = true;
    BoardFeature.BoolFlag.Have_DAC_Vclamp_Enable = false;//true;


//   BoardFeature.BoolFlag.PowerOffCheck_NeedNegation_Enable = false;

    BoardFeature.I_ADG_Address_Change_Mode = LBT21XXX_I_ADG_Address_Change_Mode_21084;

    unsigned char FanCtrlValue[8]= {30,40,50,60,70,80,90,100};
    memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_1], &FanCtrlValue[0], 8);
    unsigned char FanCtrlValue1[8]= {30,30,30,30,30,30,40,40};
    memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_2], &FanCtrlValue1[0], 8);
    memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_3], &FanCtrlValue1[0], 8);
    unsigned char FanCtrlValue2[8]= {30,30,30,30,30,30,30,30};
    memcpy(&FanAdj_task::FanCtrlValue[CURRENT_RANGE_4], &FanCtrlValue2[0], 8);
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {
        board_config::DacInfo.m_bHaveVoltageDac = 0;
        OS_LogicProcess::InitBoardEngine(	&SetCtrlOpenCh[0],	sizeof(SetCtrlOpenCh)		/		sizeof(St_DisposableTask),
                                            &SetCtrlCloseCh[0],        sizeof(SetCtrlCloseCh)	/		sizeof(St_DisposableTask),
                                            &SetCCToCC[0], 			sizeof(SetCCToCC)				/		sizeof(St_DisposableTask),
                                            &SetRstToOff[0],		sizeof(SetRstToOff)			/		sizeof(St_DisposableTask)
                                        );
    }
    else if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion>=BOARDTYPE_VER_B)
    {
        board_config::DacInfo.m_bHaveVoltageDac = 1;
        Engine();
    }
    AdditionalEngine();
    EngineStuff();
}
void LBT20084_Base::AdditionalEngine(void)
{
    // ACIM Engine
    OS_LogicProcess::SetCtrlAcimOn.st_Disposabletask_ptr = &LBT20084_Base::SetCtrlOpenAcim[0];
    OS_LogicProcess::SetCtrlAcimOn.stepLen = sizeof(SetCtrlOpenAcim) / sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlAcimOff.st_Disposabletask_ptr = &LBT20084_Base::SetCtrlCloseAcim[0];
    OS_LogicProcess::SetCtrlAcimOff.stepLen = sizeof(SetCtrlCloseAcim) / sizeof(St_DisposableTask);
}
void LBT20084_Base::Engine(void)
{
    OS_LogicProcess::SetCtrlRstToCur.st_Disposabletask_ptr =& LBT20084_Base::SetRstToCur[0];
    OS_LogicProcess::SetCtrlRstToCur.stepLen = sizeof(SetRstToCur)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlRstToVlt.st_Disposabletask_ptr =&LBT20084_Base::SetRstToVlt[0];
    OS_LogicProcess::SetCtrlRstToVlt.stepLen = sizeof(SetRstToVlt)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlRstToOff.st_Disposabletask_ptr =&LBT20084_Base::SetRstToOff[0];;
    OS_LogicProcess::SetCtrlRstToOff.stepLen = sizeof(SetRstToOff)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlOffToRst.st_Disposabletask_ptr =&LBT20084_Base::SetOffToRst[0];
    OS_LogicProcess::SetCtrlOffToRst.stepLen = sizeof(SetOffToRst)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlOffToCur.st_Disposabletask_ptr =& LBT20084_Base::SetOffToCur[0];
    OS_LogicProcess::SetCtrlOffToCur.stepLen = sizeof(SetOffToCur)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlOffToVlt.st_Disposabletask_ptr =&LBT20084_Base::SetOffToVlt[0];
    OS_LogicProcess::SetCtrlOffToVlt.stepLen = sizeof(SetOffToVlt)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlCurToCur.st_Disposabletask_ptr =&LBT20084_Base::SetCurToCur[0];
    OS_LogicProcess::SetCtrlCurToCur.stepLen = sizeof(SetCurToCur)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlCurToVlt.st_Disposabletask_ptr =&LBT20084_Base::SetCurToVlt[0];
    OS_LogicProcess::SetCtrlCurToVlt.stepLen = sizeof(SetCurToVlt)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlCurToOff.st_Disposabletask_ptr =&LBT20084_Base::SetCurToOff[0];
    OS_LogicProcess::SetCtrlCurToOff.stepLen = sizeof(SetCurToOff)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlCurToRst.st_Disposabletask_ptr =&LBT20084_Base::SetCurToRst[0];
    OS_LogicProcess::SetCtrlCurToRst.stepLen = sizeof(SetCurToRst)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlVltToVlt.st_Disposabletask_ptr =&LBT20084_Base::SetVltToVlt[0];
    OS_LogicProcess::SetCtrlVltToVlt.stepLen = sizeof(SetVltToVlt)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlVltToCur.st_Disposabletask_ptr =&LBT20084_Base::SetVltToCur[0];
    OS_LogicProcess::SetCtrlVltToCur.stepLen = sizeof(SetVltToCur)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlVltToOff.st_Disposabletask_ptr =&LBT20084_Base::SetVltToOff[0];
    OS_LogicProcess::SetCtrlVltToOff.stepLen = sizeof(SetVltToOff)	/	sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlVltToRst.st_Disposabletask_ptr =&LBT20084_Base::SetVltToRst[0];
    OS_LogicProcess::SetCtrlVltToRst.stepLen = sizeof(SetVltToRst)	/	sizeof(St_DisposableTask);



}
void LBT20084_Base::EngineStuff(void)
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
    if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_A)
    {
        SetCtrlOpenCh[0].uw32_DelayTime = 200;
        SetCtrlOpenCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

        SetCtrlOpenCh[1].uw32_DelayTime = 100;
        SetCtrlOpenCh[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;


        SetCtrlOpenCh[2].uw32_DelayTime = 150;
        SetCtrlOpenCh[2].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect;

//新增
        SetCtrlOpenCh[3].uw32_DelayTime = 150;
        SetCtrlOpenCh[3].RelayMove.ActiveFlag = LBT21XXX_RLY_CHON;


        SetCtrlOpenCh[4].uw32_DelayTime = 50;
        SetCtrlOpenCh[4].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;

        SetCtrlOpenCh[5].uw32_DelayTime = 100;
        SetCtrlOpenCh[5].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

        SetCtrlCloseCh[0].uw32_DelayTime = 100;
        SetCtrlCloseCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
        SetCtrlCloseCh[0].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect;

        SetCtrlCloseCh[1].uw32_DelayTime = 150;
        SetCtrlCloseCh[1].RelayMove.InactiveFlag = LBT21XXX_RLY_CHON;

        SetCtrlCloseCh[2].uw32_DelayTime = 150;
        SetCtrlCloseCh[2].RelayMove.InactiveFlag = LBT21XXX_RLY_Output;



        SetCtrlCloseCh[3].uw32_DelayTime = 50;
        SetCtrlCloseCh[3].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect | LBT21XXX_RLY_ACIM;


        SetCCToCC[0].uw32_DelayTime = 100;
        SetCCToCC[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
        SetCCToCC[0].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
        SetCCToCC[0].RangeChangedFlag = 1;//

        SetCCToCC[1].uw32_DelayTime = 100;
        SetCCToCC[1].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect;
        SetCCToCC[1].RangeChangedFlag = 1;

        SetCCToCC[2].uw32_DelayTime = 150;
        SetCCToCC[2].RelayMove.ActiveFlag = LBT21XXX_RLY_CHON;//
        SetCCToCC[2].RangeChangedFlag = 1;

        SetCCToCC[3].uw32_DelayTime = 50;
        SetCCToCC[3].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;
        SetCCToCC[3].RangeChangedFlag = 1;

        SetCCToCC[4].uw32_DelayTime = 50;
        SetCCToCC[4].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

        SetRstToOff[0].uw32_DelayTime = 100;
        SetRstToOff[0].RelayMove.InactiveFlag = LBT21XXX_RLY_CHON;  //为防止并联时从通道状态被意外设置成CCS_OFF ,保险起见这个地方也处理继电器关闭  dirui 2015/09/16
        SetRstToOff[0].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT; //需要放开clamp;
        SetRstToOff[0].RangeChangedFlag = 0;

        //
        SetRstToOff[1].uw32_DelayTime = 50;
        SetRstToOff[1].RelayMove.InactiveFlag = LBT21XXX_RLY_Output | LBT21XXX_RLY_ACIM;
        SetRstToOff[1].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
        SetRstToOff[1].RangeChangedFlag = 0;

        SetRstToOff[2].uw32_DelayTime = 0;
        SetRstToOff[2].RelayMove.ActiveFlag = 0;
        SetRstToOff[2].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
        SetRstToOff[2].RangeChangedFlag = 0;

    }
    else if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion>=BOARDTYPE_VER_B)
    {
        //rst--cur ok
        SetRstToCur[0].uw32_DelayTime = 100;
        SetRstToCur[0].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;


        SetRstToCur[1].uw32_DelayTime = 150;
        SetRstToCur[1].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect | LBT21XXX_RLY_DAMPING;


        SetRstToCur[2].uw32_DelayTime = 150;
        SetRstToCur[2].RelayMove.ActiveFlag = LBT21XXX_RLY_CHON;


        SetRstToCur[3].uw32_DelayTime = 150;
        SetRstToCur[3].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;

        SetRstToCur[4].uw32_DelayTime = 100;
        SetRstToCur[4].DacMove.DacFlag = ACTIVE_DAC_CUR;

        //rst--Vlt ok
        SetRstToVlt[0].uw32_DelayTime = 100;
        SetRstToVlt[0].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;

        SetRstToVlt[1].uw32_DelayTime = 150;
        SetRstToVlt[1].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect | LBT21XXX_RLY_DAMPING;

        SetRstToVlt[2].uw32_DelayTime = 150;
        SetRstToVlt[2].RelayMove.ActiveFlag = LBT21XXX_RLY_CHON;

        SetRstToVlt[3].uw32_DelayTime = 50;
        SetRstToVlt[3].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;

        SetRstToVlt[4].uw32_DelayTime = 20;
        SetRstToVlt[4].DacMove.DacFlag = ACTIVE_DAC_VLT;

        SetRstToVlt[5].uw32_DelayTime = 100;
        SetRstToVlt[5].RelayMove.ActiveFlag = LBT21XXX_RLY_IVS;

        //rst--off
		SetRstToOff[0].uw32_DelayTime = 100;
        SetRstToOff[0].RelayMove.InactiveFlag = LBT21XXX_RLY_CHON;  //为防止并联时从通道状态被意外设置成CCS_OFF ,保险起见这个地方也处理继电器关闭  dirui 2015/09/16
       //SetRstToOff[0].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT; //需要放开clamp;
        SetRstToOff[0].RangeChangedFlag = 0;

        //
        SetRstToOff[1].uw32_DelayTime = 50;
        SetRstToOff[1].RelayMove.InactiveFlag = LBT21XXX_RLY_Output | LBT21XXX_RLY_ACIM;
        //SetRstToOff[1].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
        SetRstToOff[1].RangeChangedFlag = 0;

        SetRstToOff[2].uw32_DelayTime = 0;
        SetRstToOff[2].RelayMove.ActiveFlag = 0;
        //SetRstToOff[2].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
        SetRstToOff[2].RangeChangedFlag = 0;
		
// 		SetRstToOff[0].uw32_DelayTime = 100;
// 		SetRstToOff[0].RelayMove.InactiveFlag = LBT21XXX_RLY_CHON;  //为防止并联时从通道状态被意外设置成CCS_OFF ,保险起见这个地方也处理继电器关闭  dirui 2015/09/16
// 		SetRstToOff[0].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT; //需要放开clamp;
// 		SetRstToOff[0].RangeChangedFlag = 0;
//
// 		SetRstToOff[1].uw32_DelayTime = 50;
// 		SetRstToOff[1].RelayMove.InactiveFlag = LBT21XXX_RLY_Output | LBT21XXX_RLY_ACIM;
// 		SetRstToOff[1].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
// 		SetRstToOff[1].RangeChangedFlag = 0;
//
// 		SetRstToOff[2].uw32_DelayTime = 0;
// 		SetRstToOff[2].RelayMove.ActiveFlag = 0;
// 		SetRstToOff[2].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
// 		SetRstToOff[2].RangeChangedFlag = 0;

        //off--rst
// 		SetOffToRst[0].uw32_DelayTime = 100;
// 		SetOffToRst[0].RelayMove.InactiveFlag = LBT21XXX_RLY_CHON;  //为防止并联时从通道状态被意外设置成CCS_OFF ,保险起见这个地方也处理继电器关闭  dirui 2015/09/16
// 		SetOffToRst[0].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT; //需要放开clamp;
// 		SetOffToRst[0].RangeChangedFlag = 0;
//
// 		SetOffToRst[1].uw32_DelayTime = 50;
// 		SetOffToRst[1].RelayMove.InactiveFlag = LBT21XXX_RLY_Output | LBT21XXX_RLY_ACIM;
// 		SetOffToRst[1].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
// 		SetOffToRst[1].RangeChangedFlag = 0;
//
// 		SetOffToRst[2].uw32_DelayTime = 0;
// 		SetOffToRst[2].RelayMove.ActiveFlag = 0;
// 		SetOffToRst[2].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;
// 		SetOffToRst[2].RangeChangedFlag = 0;

        //off-cur ok
        SetOffToCur[0].uw32_DelayTime = 100;
        SetOffToCur[0].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;

        SetOffToCur[1].uw32_DelayTime = 150;
        SetOffToCur[1].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect | LBT21XXX_RLY_DAMPING;

        SetOffToCur[2].uw32_DelayTime = 150;
        SetOffToCur[2].RelayMove.ActiveFlag = LBT21XXX_RLY_CHON;


        SetOffToCur[3].uw32_DelayTime = 150;
        SetOffToCur[3].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;

        SetOffToCur[4].uw32_DelayTime = 100;
        SetOffToCur[4].DacMove.DacFlag = ACTIVE_DAC_CUR;

        //off-VLT ok

        SetOffToVlt[0].uw32_DelayTime = 100;
        SetOffToVlt[0].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;

        SetOffToVlt[1].uw32_DelayTime = 150;
        SetOffToVlt[1].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect | LBT21XXX_RLY_DAMPING;

        SetOffToVlt[2].uw32_DelayTime = 150;
        SetOffToVlt[2].RelayMove.ActiveFlag = LBT21XXX_RLY_CHON;

        SetOffToVlt[3].uw32_DelayTime = 50;
        SetOffToVlt[3].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;

        SetOffToVlt[4].uw32_DelayTime = 20;
        SetOffToVlt[4].DacMove.DacFlag = ACTIVE_DAC_VLT;

        SetOffToVlt[5].uw32_DelayTime = 100;
        SetOffToVlt[5].RelayMove.ActiveFlag = LBT21XXX_RLY_IVS;

        //CC-CC ok
        SetCurToCur[0].uw32_DelayTime = 100;
        SetCurToCur[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
        SetCurToCur[0].RangeChangedFlag = 1;

        SetCurToCur[1].uw32_DelayTime = 150;
        SetCurToCur[1].RelayMove.InactiveFlag = LBT21XXX_RLY_CHON;
        SetCurToCur[1].RangeChangedFlag = 1;

        SetCurToCur[2].uw32_DelayTime = 100;
        SetCurToCur[2].RelayMove.RelaySet =  ACTIVE_OUT_NEXT_RELAY;
        SetCurToCur[2].RangeChangedFlag = 1;

        SetCurToCur[3].uw32_DelayTime = 50;
        SetCurToCur[3].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect;
        SetCurToCur[3].RangeChangedFlag = 1;

        SetCurToCur[4].uw32_DelayTime = 50;
        SetCurToCur[4].RelayMove.ActiveFlag = LBT21XXX_RLY_DAMPING;

        SetCurToCur[5].uw32_DelayTime = 150;
        SetCurToCur[5].RelayMove.ActiveFlag = LBT21XXX_RLY_CHON;
        SetCurToCur[5].RangeChangedFlag = 1;

        SetCurToCur[6].uw32_DelayTime = 150;
        SetCurToCur[6].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;
        SetCurToCur[6].RangeChangedFlag = 1;

        SetCurToCur[7].uw32_DelayTime = 100;
        SetCurToCur[7].DacMove.DacFlag = ACTIVE_DAC_CUR;
			#if (IV_AUTORANGE == 1)
         //AutoSwitch ok
        SetAutoRangeSwitch[0].uw32_DelayTime = 100;
        SetAutoRangeSwitch[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
        SetAutoRangeSwitch[0].RangeChangedFlag = 1;

        SetAutoRangeSwitch[1].uw32_DelayTime = 150;
        SetAutoRangeSwitch[1].RelayMove.InactiveFlag = LBT21XXX_RLY_CHON;
        SetAutoRangeSwitch[1].RangeChangedFlag = 1;

        SetAutoRangeSwitch[2].uw32_DelayTime = 100;
        SetAutoRangeSwitch[2].RelayMove.RelaySet =  ACTIVE_OUT_NEXT_RELAY;
        SetAutoRangeSwitch[2].RangeChangedFlag = 1;

        SetAutoRangeSwitch[3].uw32_DelayTime = 50;
        SetAutoRangeSwitch[3].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect;
        SetAutoRangeSwitch[3].RangeChangedFlag = 1;

        SetAutoRangeSwitch[4].uw32_DelayTime = 50;
        SetAutoRangeSwitch[4].RelayMove.ActiveFlag = LBT21XXX_RLY_DAMPING;

        SetAutoRangeSwitch[5].uw32_DelayTime = 150;
        SetAutoRangeSwitch[5].RelayMove.ActiveFlag = LBT21XXX_RLY_CHON;
        SetAutoRangeSwitch[5].RangeChangedFlag = 1;

        SetAutoRangeSwitch[6].uw32_DelayTime = 150;
        SetAutoRangeSwitch[6].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;
        SetAutoRangeSwitch[6].RangeChangedFlag = 1;

        SetAutoRangeSwitch[7].uw32_DelayTime = 100;
        SetAutoRangeSwitch[7].DacMove.DacFlag = ACTIVE_DAC_CUR;
				#endif
        //CC-CV ok
        SetCurToVlt[0].uw32_DelayTime = 100;
        SetCurToVlt[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
        SetCurToVlt[0].RangeChangedFlag = 1;//

        SetCurToVlt[1].uw32_DelayTime = 150;
        SetCurToVlt[1].RelayMove.InactiveFlag = LBT21XXX_RLY_CHON;//
        SetCurToVlt[1].RangeChangedFlag = 1;

        SetCurToVlt[2].uw32_DelayTime = 100;
        SetCurToVlt[2].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
        SetCurToVlt[2].RangeChangedFlag = 1;

        SetCurToVlt[3].uw32_DelayTime = 50;
        SetCurToVlt[3].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect ;
        SetCurToVlt[3].RangeChangedFlag = 1;

        SetCurToVlt[4].uw32_DelayTime = 50;
        SetCurToVlt[4].RelayMove.ActiveFlag = LBT21XXX_RLY_DAMPING;

        SetCurToVlt[5].uw32_DelayTime = 150;
        SetCurToVlt[5].RelayMove.ActiveFlag = LBT21XXX_RLY_CHON;
        SetCurToVlt[5].RangeChangedFlag = 1;

        SetCurToVlt[6].uw32_DelayTime = 50;
        SetCurToVlt[6].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;
        SetCurToVlt[6].RangeChangedFlag = 1;

        SetCurToVlt[7].uw32_DelayTime = 20;
        SetCurToVlt[7].DacMove.DacFlag = ACTIVE_DAC_VLT;

        SetCurToVlt[8].uw32_DelayTime = 50;
        SetCurToVlt[8].RelayMove.ActiveFlag = LBT21XXX_RLY_IVS;

        SetCurToVlt[9].uw32_DelayTime = 100;
        SetCurToVlt[9].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

        //CC-OFF ok
        SetCurToOff[0].uw32_DelayTime = 100;
        SetCurToOff[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

        SetCurToOff[1].uw32_DelayTime = 150;
        SetCurToOff[1].RelayMove.InactiveFlag = LBT21XXX_RLY_CHON;

        SetCurToOff[2].uw32_DelayTime = 150;
        SetCurToOff[2].RelayMove.InactiveFlag = LBT21XXX_RLY_Output | LBT21XXX_RLY_DAMPING;

        SetCurToOff[3].uw32_DelayTime = 50;
        SetCurToOff[3].RelayMove.InactiveFlag = LBT21XXX_RLY_ACIM ;
        //CC-RST ok
        SetCurToRst[0].uw32_DelayTime = 100;
        SetCurToRst[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

        SetCurToRst[1].uw32_DelayTime = 150;
        SetCurToRst[1].RelayMove.InactiveFlag = LBT21XXX_RLY_CHON;

        SetCurToRst[2].uw32_DelayTime = 150;
        SetCurToRst[2].RelayMove.InactiveFlag = LBT21XXX_RLY_Output | LBT21XXX_RLY_DAMPING;

        SetCurToRst[3].uw32_DelayTime = 50;
        SetCurToRst[3].RelayMove.InactiveFlag = LBT21XXX_RLY_ACIM ;
        //CV-CV ok
        SetVltToVlt[0].uw32_DelayTime = 100;
        SetVltToVlt[0].RelayMove.InactiveFlag = LBT21XXX_RLY_IVS;
        SetVltToVlt[0].RangeChangedFlag = 1;

// 		SetVltToVlt[1].uw32_DelayTime = 100;
// 		SetVltToVlt[1].DacMove.DacFlag = ACTIVE_DAC_VLT_ZERO;
// 		SetVltToVlt[1].RangeChangedFlag = 1;

        SetVltToVlt[1].uw32_DelayTime = 150;
        SetVltToVlt[1].RelayMove.InactiveFlag = LBT21XXX_RLY_CHON;//
        SetVltToVlt[1].RangeChangedFlag = 1;

        SetVltToVlt[2].uw32_DelayTime = 100;
        SetVltToVlt[2].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
        SetVltToVlt[2].RangeChangedFlag = 1;

        SetVltToVlt[3].uw32_DelayTime = 50;
        SetVltToVlt[3].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect ;
        SetVltToVlt[3].RangeChangedFlag = 1;

        SetVltToVlt[4].uw32_DelayTime = 50;
        SetVltToVlt[4].RelayMove.ActiveFlag =  LBT21XXX_RLY_DAMPING;

        SetVltToVlt[5].uw32_DelayTime = 150;
        SetVltToVlt[5].RelayMove.ActiveFlag = LBT21XXX_RLY_CHON;
        SetVltToVlt[5].RangeChangedFlag = 1;

        SetVltToVlt[6].uw32_DelayTime = 50;
        SetVltToVlt[6].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;
        SetVltToVlt[6].RangeChangedFlag = 1;

        SetVltToVlt[7].uw32_DelayTime = 50;
        SetVltToVlt[7].DacMove.DacFlag = ACTIVE_DAC_VLT;

        SetVltToVlt[8].uw32_DelayTime = 100;
        SetVltToVlt[8].RelayMove.ActiveFlag = LBT21XXX_RLY_IVS;
        SetVltToVlt[8].RangeChangedFlag = 1;

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

        SetVltToCur[4].uw32_DelayTime = 150;
        SetVltToCur[4].RelayMove.InactiveFlag = LBT21XXX_RLY_CHON;
        SetVltToCur[4].RangeChangedFlag = 1;

        SetVltToCur[5].uw32_DelayTime = 100;
        SetVltToCur[5].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
        SetVltToCur[5].RangeChangedFlag = 1;

        SetVltToCur[6].uw32_DelayTime = 50;
        SetVltToCur[6].RelayMove.ActiveFlag = LBT21XXX_RLY_Protect;
        SetVltToCur[6].RangeChangedFlag = 1;

        SetVltToCur[7].uw32_DelayTime = 50;
        SetVltToCur[7].RelayMove.ActiveFlag = LBT21XXX_RLY_DAMPING;

        SetVltToCur[8].uw32_DelayTime = 150;
        SetVltToCur[8].RelayMove.ActiveFlag = LBT21XXX_RLY_CHON;//
        SetVltToCur[8].RangeChangedFlag = 1;

        SetVltToCur[9].uw32_DelayTime = 50;
        SetVltToCur[9].RelayMove.InactiveFlag = LBT21XXX_RLY_Protect;
        SetVltToCur[9].RangeChangedFlag = 1;

        SetVltToCur[10].uw32_DelayTime = 50;
        SetVltToCur[10].DacMove.DacFlag = ACTIVE_DAC_CUR;
        SetVltToCur[10].RangeChangedFlag = 1;
        //CV-OFF ok
        SetVltToOff[0].uw32_DelayTime = 100;
        SetVltToOff[0].RelayMove.InactiveFlag = LBT21XXX_RLY_IVS;

        SetVltToOff[1].uw32_DelayTime = 150;
        SetVltToOff[1].DacMove.DacFlag = ACTIVE_DAC_VLT_ZERO;

        SetVltToOff[2].uw32_DelayTime = 50;
        SetVltToOff[2].RelayMove.InactiveFlag = LBT21XXX_RLY_CHON;

        SetVltToOff[3].uw32_DelayTime = 150;
        SetVltToOff[3].RelayMove.InactiveFlag = LBT21XXX_RLY_Output | LBT21XXX_RLY_DAMPING;

        //CV-RST ok
        SetVltToRst[0].uw32_DelayTime = 100;
        SetVltToRst[0].RelayMove.InactiveFlag = LBT21XXX_RLY_IVS;

        SetVltToRst[1].uw32_DelayTime = 150;
        SetVltToRst[1].DacMove.DacFlag = ACTIVE_DAC_VLT_ZERO;

        SetVltToRst[2].uw32_DelayTime = 50;
        SetVltToRst[2].RelayMove.InactiveFlag = LBT21XXX_RLY_CHON;

        SetVltToRst[3].uw32_DelayTime = 150;
        SetVltToRst[3].RelayMove.InactiveFlag = LBT21XXX_RLY_Output | LBT21XXX_RLY_DAMPING;
    }
    SetCtrlOpenAcim[0].uw32_DelayTime = 100;
    SetCtrlOpenAcim[0].RelayMove.ActiveFlag = LBT21XXX_RLY_ACIM;

    SetCtrlCloseAcim[0].uw32_DelayTime = 100;
    SetCtrlCloseAcim[0].RelayMove.InactiveFlag = LBT21XXX_RLY_ACIM;
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
