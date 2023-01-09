
#include "../../Entry/includes.h"

void LBT22043_Base::FeatureEnable_Init(void)
{
    BoardFeature.BoolFlag.OffGateM_Enable = true;
    //Engine();
    OS_LogicProcess::InitBoardEngine(	&SetCtrlOpenCh[0],	sizeof(SetCtrlOpenCh)		/		sizeof(St_DisposableTask),
                                        &SetCtrlCloseCh[0], sizeof(SetCtrlCloseCh)	/		sizeof(St_DisposableTask),
                                        &SetCCToCC[0], 			sizeof(SetCCToCC)				/		sizeof(St_DisposableTask),
                                        &SetRstToOff[0],		sizeof(SetRstToOff)			/		sizeof(St_DisposableTask)
                                    );
    AdditionalEngine();
    EngineStuff();
}
void LBT22043_Base::AdditionalEngine(void)
{
    OS_LogicProcess::SetCtrlOffToRst.st_Disposabletask_ptr = &SetOffToRst[0];
    OS_LogicProcess::SetCtrlOffToRst.stepLen = sizeof(SetOffToRst)/	sizeof(St_DisposableTask); //LBT22043_Base::SetRstToOff_Len;

    OS_LogicProcess::SetCtrlCurToRst.st_Disposabletask_ptr = &SetCtrlCloseChToRest[0];
    OS_LogicProcess::SetCtrlCurToRst.stepLen = sizeof(SetCtrlCloseChToRest)/	sizeof(St_DisposableTask); //LBT22043_Base::SetCtrlCloseChToRest_Len;

    OS_LogicProcess::SetCtrlVltToRst.st_Disposabletask_ptr = &SetCtrlCloseChToRest[0];
    OS_LogicProcess::SetCtrlVltToRst.stepLen = sizeof(SetCtrlCloseChToRest)/	sizeof(St_DisposableTask); //LBT22043_Base::SetCtrlCloseChToRest_Len;

    // ACIM Engine
    OS_LogicProcess::SetCtrlAcimOn.st_Disposabletask_ptr = &SetCtrlOpenAcim[0];
    OS_LogicProcess::SetCtrlAcimOn.stepLen = sizeof(SetCtrlOpenAcim) / sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlAcimOff.st_Disposabletask_ptr = &SetCtrlCloseAcim[0];
    OS_LogicProcess::SetCtrlAcimOff.stepLen = sizeof(SetCtrlCloseAcim) / sizeof(St_DisposableTask);
}

void LBT22043_Base::EngineStuff(void)
{
    memset(&SetCtrlOpenCh,   0,    sizeof(SetCtrlOpenCh));
    memset(&SetCtrlCloseCh,  0,    sizeof(SetCtrlCloseCh));
    memset(&SetCCToCC,       0,    sizeof(SetCCToCC));
    memset(&SetRstToOff,     0,    sizeof(SetRstToOff));
    memset(&SetOffToRst,     0,    sizeof(SetOffToRst));
    memset(&SetCtrlCloseChToRest,     0,    sizeof(SetCtrlCloseChToRest));
    memset(&SetCtrlOpenAcim,	0,    sizeof(SetCtrlOpenAcim));
    memset(&SetCtrlCloseAcim,	0,    sizeof(SetCtrlCloseAcim));
#if (IV_AUTORANGE == 1)
			memset(&SetAutoRangeSwitch,	0,    sizeof(SetAutoRangeSwitch));
		#endif
    //---------------------------------------------//
    SetCtrlOpenCh[0].uw32_DelayTime = 200;
    SetCtrlOpenCh[0].RelayMove.ActiveFlag = LBT22XXX_RLY_Protect | LBT22XXX_RLY_OffGateM | LBT22XXX_RLY_DAMPING;
    SetCtrlOpenCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

    SetCtrlOpenCh[1].uw32_DelayTime = 200;
    SetCtrlOpenCh[1].RelayMove.ActiveFlag = LBT22XXX_RLY_VoltSet | LBT22XXX_RLY_PreC;
    SetCtrlOpenCh[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
    SetCtrlOpenCh[1].DacMove.DacFlag =ACTIVE_DAC_VH | ACTIVE_DAC_VL;

    SetCtrlOpenCh[2].uw32_DelayTime = 200;
    SetCtrlOpenCh[2].RelayMove.InactiveFlag = LBT22XXX_RLY_Protect | LBT22XXX_RLY_OffGateM;

    SetCtrlOpenCh[3].uw32_DelayTime = 200;
    SetCtrlOpenCh[3].RelayMove.InactiveFlag = LBT22XXX_RLY_OffGate;

    SetCtrlOpenCh[4].uw32_DelayTime = 100;
    SetCtrlOpenCh[4].DacMove.DacFlag = ACTIVE_DAC_CUR;

    //---------------------------------------------//
    //I to off v TO OFF
    SetCtrlCloseCh[0].uw32_DelayTime = 100;
    SetCtrlCloseCh[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

    SetCtrlCloseCh[1].uw32_DelayTime = 200;
    SetCtrlCloseCh[1].RelayMove.ActiveFlag = LBT22XXX_RLY_OffGate;

    SetCtrlCloseCh[2].uw32_DelayTime = 200;
    SetCtrlCloseCh[2].RelayMove.ActiveFlag = LBT22XXX_RLY_Protect | LBT22XXX_RLY_OffGateM;

    SetCtrlCloseCh[3].uw32_DelayTime = 200;
    SetCtrlCloseCh[3].RelayMove.InactiveFlag = (LBT22XXX_RLY_Output | LBT22XXX_RLY_VoltSet | LBT22XXX_RLY_PreC);
    SetCtrlCloseCh[3].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

    //SetCtrlCloseCh[4].uw32_DelayTime = 0;
    SetCtrlCloseCh[4].RelayMove.InactiveFlag = LBT22XXX_RLY_Protect | LBT22XXX_RLY_DAMPING | LBT22XXX_RLY_ACIM;

    //I To rest V TO rest
    SetCtrlCloseChToRest[0].uw32_DelayTime = 100;
    SetCtrlCloseChToRest[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

    SetCtrlCloseChToRest[1].uw32_DelayTime = 200;
    SetCtrlCloseChToRest[1].RelayMove.ActiveFlag = LBT22XXX_RLY_OffGate;

    SetCtrlCloseChToRest[2].uw32_DelayTime = 200;
    SetCtrlCloseChToRest[2].RelayMove.ActiveFlag = LBT22XXX_RLY_Protect;

    SetCtrlCloseChToRest[3].uw32_DelayTime = 200;
    SetCtrlCloseChToRest[3].RelayMove.InactiveFlag = LBT22XXX_RLY_Output;
    SetCtrlCloseChToRest[3].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

    //SetCtrlCloseChToRest[4].uw32_DelayTime = 0;
    SetCtrlCloseChToRest[4].RelayMove.InactiveFlag = LBT22XXX_RLY_Protect;

    //---------------------------------------------//



    SetCCToCC[0].uw32_DelayTime = 100;
    SetCCToCC[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH | ACTIVE_DAC_VL;
    SetCCToCC[0].RangeChangedFlag = 1;

    SetCCToCC[1].uw32_DelayTime = 200;
    SetCCToCC[1].RelayMove.ActiveFlag = LBT22XXX_RLY_OffGate ;
    SetCCToCC[1].RangeChangedFlag = 1;

    SetCCToCC[2].uw32_DelayTime = 200;
    SetCCToCC[2].RelayMove.ActiveFlag = LBT22XXX_RLY_Protect | LBT22XXX_RLY_OffGateM | LBT22XXX_RLY_DAMPING;
    SetCCToCC[2].RangeChangedFlag = 1;

    SetCCToCC[3].uw32_DelayTime = 100;
    SetCCToCC[3].RelayMove.ActiveFlag =  LBT22XXX_RLY_DAMPING;

    SetCCToCC[4].uw32_DelayTime = 200;
    SetCCToCC[4].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
    SetCCToCC[4].RelayMove.ActiveFlag = LBT22XXX_RLY_VoltSet;
    SetCCToCC[4].RangeChangedFlag = 1;

    SetCCToCC[5].uw32_DelayTime = 200;
    SetCCToCC[5].RelayMove.InactiveFlag = LBT22XXX_RLY_Protect | LBT22XXX_RLY_OffGateM;
    SetCCToCC[5].RangeChangedFlag = 1;

    SetCCToCC[6].uw32_DelayTime = 200;
    SetCCToCC[6].RelayMove.InactiveFlag = LBT22XXX_RLY_OffGate;
    SetCCToCC[6].RangeChangedFlag = 1;

    SetCCToCC[7].uw32_DelayTime = 200;
    SetCCToCC[7].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

    SetRstToOff[0].uw32_DelayTime = 10;
    //SetRstToOff[0].DacMove.DacFlag = 0;
    SetRstToOff[0].RelayMove.InactiveFlag = LBT22XXX_RLY_VoltSet | LBT22XXX_RLY_PreC | LBT22XXX_RLY_ACIM;
    //SetRstToOff[0].RangeChangedFlag = 0;

    SetOffToRst[0].uw32_DelayTime = 10;
    //SetOffToRst[0].DacMove.DacFlag = 0;
    SetOffToRst[0].RelayMove.ActiveFlag  = LBT22XXX_RLY_VoltSet;
    //SetOffToRst[0].RangeChangedFlag = 0;

    SetCtrlOpenAcim[0].uw32_DelayTime = 100;
    SetCtrlOpenAcim[0].RelayMove.ActiveFlag = LBT22XXX_RLY_ACIM;

    SetCtrlCloseAcim[0].uw32_DelayTime = 100;
    SetCtrlCloseAcim[0].RelayMove.InactiveFlag = LBT22XXX_RLY_ACIM;
		#if (IV_AUTORANGE == 1)
	SetAutoRangeSwitch[0].uw32_DelayTime = 100;
    SetAutoRangeSwitch[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH | ACTIVE_DAC_VL;
    SetAutoRangeSwitch[0].RangeChangedFlag = 1;

    SetAutoRangeSwitch[1].uw32_DelayTime = 200;
    SetAutoRangeSwitch[1].RelayMove.ActiveFlag = LBT22XXX_RLY_OffGate ;
    SetAutoRangeSwitch[1].RangeChangedFlag = 1;

    SetAutoRangeSwitch[2].uw32_DelayTime = 200;
    SetAutoRangeSwitch[2].RelayMove.ActiveFlag = LBT22XXX_RLY_Protect | LBT22XXX_RLY_OffGateM | LBT22XXX_RLY_DAMPING;
    SetAutoRangeSwitch[2].RangeChangedFlag = 1;

    SetAutoRangeSwitch[3].uw32_DelayTime = 100;
    SetAutoRangeSwitch[3].RelayMove.ActiveFlag =  LBT22XXX_RLY_DAMPING;

    SetAutoRangeSwitch[4].uw32_DelayTime = 200;
    SetAutoRangeSwitch[4].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
    SetAutoRangeSwitch[4].RelayMove.ActiveFlag = LBT22XXX_RLY_VoltSet;
    SetAutoRangeSwitch[4].RangeChangedFlag = 1;

    SetAutoRangeSwitch[5].uw32_DelayTime = 200;
    SetAutoRangeSwitch[5].RelayMove.InactiveFlag = LBT22XXX_RLY_Protect | LBT22XXX_RLY_OffGateM;
    SetAutoRangeSwitch[5].RangeChangedFlag = 1;

    SetAutoRangeSwitch[6].uw32_DelayTime = 200;
    SetAutoRangeSwitch[6].RelayMove.InactiveFlag = LBT22XXX_RLY_OffGate;
    SetAutoRangeSwitch[6].RangeChangedFlag = 1;

    SetAutoRangeSwitch[7].uw32_DelayTime = 200;
    SetAutoRangeSwitch[7].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;
		#endif
}
/*********************************************************************************
**                            End Of File
*********************************************************************************/
