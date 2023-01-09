
#include "../../Entry/includes.h"
#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
St_DisposableTask LBT22023_Base::SetRstToOff[1];
St_DisposableTask LBT22023_Base::SetRstToCur[5];
St_DisposableTask LBT22023_Base::SetRstToVlt[6];

St_DisposableTask LBT22023_Base::SetOffToRst[1];
St_DisposableTask LBT22023_Base::SetOffToCur[5];
St_DisposableTask LBT22023_Base::SetOffToVlt[6];

St_DisposableTask LBT22023_Base::SetCurToOff[5];
St_DisposableTask LBT22023_Base::SetCurToRst[5];
St_DisposableTask LBT22023_Base::SetCurToCur[9];
St_DisposableTask LBT22023_Base::SetCurToVlt[10];


St_DisposableTask LBT22023_Base::SetVltToOff[4];
St_DisposableTask LBT22023_Base::SetVltToRst[4];
St_DisposableTask LBT22023_Base::SetVltToVlt[10];
St_DisposableTask LBT22023_Base::SetVltToCur[9];
#pragma arm section

void LBT22023_Base::FeatureEnable_Init(void)
{  
		if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion == BOARDTYPE_VER_B)
		{
			LBT22XXX_Base::m_bHaveVoltageDac = 1;
		}
	  Engine();
    AdditionalEngine();
    EngineStuff();
}
void LBT22023_Base::Engine(void)
{
  OS_LogicProcess::SetCtrlRstToCur.st_Disposabletask_ptr =& LBT22023_Base::SetRstToCur[0];
	OS_LogicProcess::SetCtrlRstToCur.stepLen = sizeof(SetRstToCur)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlRstToVlt.st_Disposabletask_ptr =&LBT22023_Base::SetRstToVlt[0];
	OS_LogicProcess::SetCtrlRstToVlt.stepLen = sizeof(SetRstToVlt)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlRstToOff.st_Disposabletask_ptr =&LBT22023_Base::SetRstToOff[0];;
  OS_LogicProcess::SetCtrlRstToOff.stepLen = sizeof(SetRstToOff)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlOffToRst.st_Disposabletask_ptr =&LBT22023_Base::SetOffToRst[0];
	OS_LogicProcess::SetCtrlOffToRst.stepLen = sizeof(SetOffToRst)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlOffToCur.st_Disposabletask_ptr =& LBT22023_Base::SetOffToCur[0];
	OS_LogicProcess::SetCtrlOffToCur.stepLen = sizeof(SetOffToCur)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlOffToVlt.st_Disposabletask_ptr =&LBT22023_Base::SetOffToVlt[0];
	OS_LogicProcess::SetCtrlOffToVlt.stepLen = sizeof(SetOffToVlt)	/	sizeof(St_DisposableTask);

  OS_LogicProcess::SetCtrlCurToCur.st_Disposabletask_ptr =&LBT22023_Base::SetCurToCur[0];
	OS_LogicProcess::SetCtrlCurToCur.stepLen = sizeof(SetCurToCur)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlCurToVlt.st_Disposabletask_ptr =&LBT22023_Base::SetCurToVlt[0];
	OS_LogicProcess::SetCtrlCurToVlt.stepLen = sizeof(SetCurToVlt)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlCurToOff.st_Disposabletask_ptr =&LBT22023_Base::SetCurToOff[0];
	OS_LogicProcess::SetCtrlCurToOff.stepLen = sizeof(SetCurToOff)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlCurToRst.st_Disposabletask_ptr =&LBT22023_Base::SetCurToRst[0];
	OS_LogicProcess::SetCtrlCurToRst.stepLen = sizeof(SetCurToRst)	/	sizeof(St_DisposableTask);

  OS_LogicProcess::SetCtrlVltToVlt.st_Disposabletask_ptr =&LBT22023_Base::SetVltToVlt[0];
	OS_LogicProcess::SetCtrlVltToVlt.stepLen = sizeof(SetVltToVlt)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlVltToCur.st_Disposabletask_ptr =&LBT22023_Base::SetVltToCur[0];
	OS_LogicProcess::SetCtrlVltToCur.stepLen = sizeof(SetVltToCur)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlVltToOff.st_Disposabletask_ptr =&LBT22023_Base::SetVltToOff[0];
	OS_LogicProcess::SetCtrlVltToOff.stepLen = sizeof(SetVltToOff)	/	sizeof(St_DisposableTask);
	
  OS_LogicProcess::SetCtrlVltToRst.st_Disposabletask_ptr =&LBT22023_Base::SetVltToRst[0];
	OS_LogicProcess::SetCtrlVltToRst.stepLen = sizeof(SetVltToRst)	/	sizeof(St_DisposableTask);  
 
}

void LBT22023_Base::AdditionalEngine(void)
{    
// ACIM Engine
    OS_LogicProcess::SetCtrlAcimOn.st_Disposabletask_ptr = &SetCtrlOpenAcim[0];
    OS_LogicProcess::SetCtrlAcimOn.stepLen = sizeof(SetCtrlOpenAcim) / sizeof(St_DisposableTask);

    OS_LogicProcess::SetCtrlAcimOff.st_Disposabletask_ptr = &SetCtrlCloseAcim[0];
    OS_LogicProcess::SetCtrlAcimOff.stepLen = sizeof(SetCtrlCloseAcim) / sizeof(St_DisposableTask);
}

void LBT22023_Base::EngineStuff(void)
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
		//rst--cur
    SetRstToCur[0].uw32_DelayTime = 100;
    SetRstToCur[0].RelayMove.ActiveFlag = LBT22XXX_RLY_Protect | LBT22XXX_RLY_DAMPING;
    SetRstToCur[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

    SetRstToCur[1].uw32_DelayTime = 100;
    SetRstToCur[1].RelayMove.ActiveFlag = LBT22XXX_RLY_VoltSet | LBT22XXX_RLY_PreC ;
    SetRstToCur[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
    SetRstToCur[1].DacMove.DacFlag =ACTIVE_DAC_VH | ACTIVE_DAC_VL;

    SetRstToCur[2].uw32_DelayTime = 100;
    SetRstToCur[2].RelayMove.InactiveFlag = LBT22XXX_RLY_Protect;

    SetRstToCur[3].uw32_DelayTime = 100;
    SetRstToCur[3].RelayMove.InactiveFlag = LBT22XXX_RLY_OffGate;

    SetRstToCur[4].uw32_DelayTime = 100;
    SetRstToCur[4].DacMove.DacFlag = ACTIVE_DAC_CUR;
	  
		//rst--Vlt
		SetRstToVlt[0].uw32_DelayTime = 100;
    SetRstToVlt[0].RelayMove.ActiveFlag = LBT22XXX_RLY_Protect | LBT22XXX_RLY_DAMPING;
//     SetRstToVlt[0].DacMove.DacFlag = ACTIVE_DAC_VLT_ZERO| ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

    SetRstToVlt[1].uw32_DelayTime = 100;
    SetRstToVlt[1].RelayMove.ActiveFlag = LBT22XXX_RLY_VoltSet | LBT22XXX_RLY_PreC;
    SetRstToVlt[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
    SetRstToVlt[1].DacMove.DacFlag =ACTIVE_DAC_VH | ACTIVE_DAC_VL;

    SetRstToVlt[2].uw32_DelayTime = 100;
    SetRstToVlt[2].RelayMove.InactiveFlag = LBT22XXX_RLY_Protect;

    SetRstToVlt[3].uw32_DelayTime = 100;
    SetRstToVlt[3].RelayMove.InactiveFlag = LBT22XXX_RLY_OffGate;

    SetRstToVlt[4].uw32_DelayTime = 10;
		SetRstToVlt[4].DacMove.DacFlag = ACTIVE_DAC_VLT;    
		
		SetRstToVlt[5].uw32_DelayTime = 100;
    SetRstToVlt[5].RelayMove.ActiveFlag = LBT22XXX_RLY_IVS;		
    
		//rst--off
		SetRstToOff[0].uw32_DelayTime = 10;    
    SetRstToOff[0].RelayMove.InactiveFlag = LBT22XXX_RLY_VoltSet | LBT22XXX_RLY_PreC | LBT22XXX_RLY_ACIM | LBT22XXX_RLY_DAMPING;
   
    //off--rst
    SetOffToRst[0].uw32_DelayTime = 10;    
    SetOffToRst[0].RelayMove.ActiveFlag  = LBT22XXX_RLY_VoltSet | LBT22XXX_RLY_DAMPING;
    
		//off--cur
    SetOffToCur[0].uw32_DelayTime = 100;
    SetOffToCur[0].RelayMove.ActiveFlag = LBT22XXX_RLY_Protect | LBT22XXX_RLY_DAMPING;
    SetOffToCur[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

    SetOffToCur[1].uw32_DelayTime = 100;
    SetOffToCur[1].RelayMove.ActiveFlag = LBT22XXX_RLY_VoltSet | LBT22XXX_RLY_PreC;
    SetOffToCur[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
    SetOffToCur[1].DacMove.DacFlag =ACTIVE_DAC_VH | ACTIVE_DAC_VL;

    SetOffToCur[2].uw32_DelayTime = 100;
    SetOffToCur[2].RelayMove.InactiveFlag = LBT22XXX_RLY_Protect;

    SetOffToCur[3].uw32_DelayTime = 100;
    SetOffToCur[3].RelayMove.InactiveFlag = LBT22XXX_RLY_OffGate;

    SetOffToCur[4].uw32_DelayTime = 100;
    SetOffToCur[4].DacMove.DacFlag = ACTIVE_DAC_CUR;
	  
		//off--Vlt
		SetOffToVlt[0].uw32_DelayTime = 100;
    SetOffToVlt[0].RelayMove.ActiveFlag = LBT22XXX_RLY_Protect | LBT22XXX_RLY_DAMPING;
//     SetOffToVlt[0].DacMove.DacFlag = ACTIVE_DAC_VLT_ZERO| ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

    SetOffToVlt[1].uw32_DelayTime = 100;
    SetOffToVlt[1].RelayMove.ActiveFlag = LBT22XXX_RLY_VoltSet | LBT22XXX_RLY_PreC;
    SetOffToVlt[1].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
    SetOffToVlt[1].DacMove.DacFlag =ACTIVE_DAC_VH | ACTIVE_DAC_VL;

    SetOffToVlt[2].uw32_DelayTime = 100;
    SetOffToVlt[2].RelayMove.InactiveFlag = LBT22XXX_RLY_Protect;

    SetOffToVlt[3].uw32_DelayTime = 100;
    SetOffToVlt[3].RelayMove.InactiveFlag = LBT22XXX_RLY_OffGate;

    SetOffToVlt[4].uw32_DelayTime = 10;
    SetOffToVlt[4].DacMove.DacFlag = ACTIVE_DAC_VLT;		
		
		SetOffToVlt[5].uw32_DelayTime = 100;		
		SetOffToVlt[5].RelayMove.ActiveFlag = LBT22XXX_RLY_IVS;
		
		//CC-CC
		SetCurToCur[0].uw32_DelayTime = 100;
    SetCurToCur[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH | ACTIVE_DAC_VL;
    SetCurToCur[0].RangeChangedFlag = 1;

    SetCurToCur[1].uw32_DelayTime = 100;
    SetCurToCur[1].RelayMove.ActiveFlag = LBT22XXX_RLY_OffGate;
    SetCurToCur[1].RangeChangedFlag = 1;
		
		SetCurToCur[2].uw32_DelayTime = 10;
    SetCurToCur[2].RelayMove.ActiveFlag =  LBT22XXX_RLY_DAMPING;

    SetCurToCur[3].uw32_DelayTime = 10;
    SetCurToCur[3].RelayMove.ActiveFlag = LBT22XXX_RLY_Protect;
    SetCurToCur[3].RangeChangedFlag = 1;

    SetCurToCur[4].uw32_DelayTime = 90;    

    SetCurToCur[5].uw32_DelayTime = 100;
    SetCurToCur[5].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
    SetCurToCur[5].RelayMove.ActiveFlag = LBT22XXX_RLY_VoltSet;
    SetCurToCur[5].RangeChangedFlag = 1;

    SetCurToCur[6].uw32_DelayTime = 100;
    SetCurToCur[6].RelayMove.InactiveFlag = LBT22XXX_RLY_Protect;
    SetCurToCur[6].RangeChangedFlag = 1;

    SetCurToCur[7].uw32_DelayTime = 100;
    SetCurToCur[7].RelayMove.InactiveFlag = LBT22XXX_RLY_OffGate;
    SetCurToCur[7].RangeChangedFlag = 1;

    SetCurToCur[8].uw32_DelayTime = 100;
    SetCurToCur[8].DacMove.DacFlag = ACTIVE_DAC_CUR| ACTIVE_DAC_VH | ACTIVE_DAC_VL;
		
		//CC-CV	 
    SetCurToVlt[0].uw32_DelayTime = 10;
    SetCurToVlt[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH | ACTIVE_DAC_VL;
    SetCurToVlt[0].RangeChangedFlag = 1;

    SetCurToVlt[1].uw32_DelayTime = 100;
    SetCurToVlt[1].RelayMove.ActiveFlag = LBT22XXX_RLY_OffGate;
    SetCurToVlt[1].RangeChangedFlag = 1;
		
		SetCurToVlt[2].uw32_DelayTime = 10;
    SetCurToVlt[2].RelayMove.ActiveFlag =  LBT22XXX_RLY_DAMPING;

    SetCurToVlt[3].uw32_DelayTime = 10;
    SetCurToVlt[3].RelayMove.ActiveFlag = LBT22XXX_RLY_Protect;
    SetCurToVlt[3].RangeChangedFlag = 1;

    SetCurToVlt[4].uw32_DelayTime = 90;
    
    SetCurToVlt[5].uw32_DelayTime = 100;
    SetCurToVlt[5].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
    SetCurToVlt[5].RelayMove.ActiveFlag = LBT22XXX_RLY_VoltSet;
    SetCurToVlt[5].RangeChangedFlag = 1;

    SetCurToVlt[6].uw32_DelayTime = 100;
    SetCurToVlt[6].RelayMove.InactiveFlag = LBT22XXX_RLY_Protect;
    SetCurToVlt[6].RangeChangedFlag = 1;

    SetCurToVlt[7].uw32_DelayTime = 100;
    SetCurToVlt[7].RelayMove.InactiveFlag = LBT22XXX_RLY_OffGate;
    SetCurToVlt[7].RangeChangedFlag = 1;
		
		SetCurToVlt[8].uw32_DelayTime = 10;		
    SetCurToVlt[8].DacMove.DacFlag = ACTIVE_DAC_VLT| ACTIVE_DAC_VH | ACTIVE_DAC_VL;

    SetCurToVlt[9].uw32_DelayTime = 100;
		SetCurToVlt[9].RelayMove.ActiveFlag = LBT22XXX_RLY_IVS;    
		
		
		
		//CC-OFF
		SetCurToOff[0].uw32_DelayTime = 100;
    SetCurToOff[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

    SetCurToOff[1].uw32_DelayTime = 100;
    SetCurToOff[1].RelayMove.ActiveFlag = LBT22XXX_RLY_OffGate;

    SetCurToOff[2].uw32_DelayTime = 100;
    SetCurToOff[2].RelayMove.ActiveFlag = LBT22XXX_RLY_Protect;

    SetCurToOff[3].uw32_DelayTime = 100;
    SetCurToOff[3].RelayMove.InactiveFlag = (LBT22XXX_RLY_Output | LBT22XXX_RLY_VoltSet | LBT22XXX_RLY_PreC);
    SetCurToOff[3].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

    SetCurToOff[4].uw32_DelayTime = 0;
    SetCurToOff[4].RelayMove.InactiveFlag = LBT22XXX_RLY_Protect | LBT22XXX_RLY_DAMPING | LBT22XXX_RLY_ACIM ;
    
		//CC-RST
		SetCurToRst[0].uw32_DelayTime = 100;
    SetCurToRst[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;

    SetCurToRst[1].uw32_DelayTime = 100;
    SetCurToRst[1].RelayMove.ActiveFlag = LBT22XXX_RLY_OffGate;

    SetCurToRst[2].uw32_DelayTime = 100;
    SetCurToRst[2].RelayMove.ActiveFlag = LBT22XXX_RLY_Protect;

    SetCurToRst[3].uw32_DelayTime = 100;
    SetCurToRst[3].RelayMove.InactiveFlag = LBT22XXX_RLY_Output;
    SetCurToRst[3].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

    SetCurToRst[4].uw32_DelayTime = 0;
    SetCurToRst[4].RelayMove.InactiveFlag = LBT22XXX_RLY_Protect;
		SetCurToRst[4].RelayMove.ActiveFlag = LBT22XXX_RLY_DAMPING;
		
		//CV-CV
    SetVltToVlt[0].uw32_DelayTime = 100;
    SetVltToVlt[0].RelayMove.ActiveFlag = LBT22XXX_RLY_OffGate;
    SetVltToVlt[0].RangeChangedFlag = 1;

    SetVltToVlt[1].uw32_DelayTime = 10;
    SetVltToVlt[1].RelayMove.ActiveFlag =  LBT22XXX_RLY_DAMPING;

    SetVltToVlt[2].uw32_DelayTime = 10;
    SetVltToVlt[2].RelayMove.ActiveFlag = LBT22XXX_RLY_Protect;
		SetVltToVlt[2].RelayMove.InactiveFlag = LBT22XXX_RLY_IVS;
		SetVltToVlt[2].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO;
    SetVltToVlt[2].RangeChangedFlag = 1;

    SetVltToVlt[3].uw32_DelayTime = 90;
    
    SetVltToVlt[4].uw32_DelayTime = 100;
    SetVltToVlt[4].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
    SetVltToVlt[4].RelayMove.ActiveFlag = LBT22XXX_RLY_VoltSet;
    SetVltToVlt[4].RangeChangedFlag = 1;

    SetVltToVlt[5].uw32_DelayTime = 100;
    SetVltToVlt[5].RelayMove.InactiveFlag = LBT22XXX_RLY_Protect;
    SetVltToVlt[5].RangeChangedFlag = 1;

    SetVltToVlt[6].uw32_DelayTime = 100;
    SetVltToVlt[6].RelayMove.InactiveFlag = LBT22XXX_RLY_OffGate;
    SetVltToVlt[6].RangeChangedFlag = 1;

    SetVltToVlt[7].uw32_DelayTime = 10;
    SetVltToVlt[7].DacMove.DacFlag = ACTIVE_DAC_VLT| ACTIVE_DAC_VH | ACTIVE_DAC_VL;
		
		SetVltToVlt[8].uw32_DelayTime = 10;
    SetVltToVlt[8].RelayMove.ActiveFlag = LBT22XXX_RLY_IVS;
		SetVltToVlt[8].RangeChangedFlag = 1;
		
		SetVltToVlt[9].uw32_DelayTime = 90;
    
		
		//CV-CC
		SetVltToCur[0].uw32_DelayTime = 100;
		SetVltToCur[0].RelayMove.ActiveFlag = LBT22XXX_RLY_OffGate;
		SetVltToCur[0].RangeChangedFlag = 1;
		
		SetVltToCur[1].uw32_DelayTime = 100;
		SetVltToCur[1].RelayMove.ActiveFlag =  LBT22XXX_RLY_DAMPING;
			
		SetVltToCur[2].uw32_DelayTime = 10;
		SetVltToCur[2].DacMove.DacFlag = ACTIVE_DAC_CUR;
		SetVltToCur[2].RelayMove.InactiveFlag = LBT22XXX_RLY_IVS;
		
		SetVltToCur[3].uw32_DelayTime = 10;
		SetVltToCur[3].DacMove.DacFlag = ACTIVE_DAC_VLT_ZERO;
		
		SetVltToCur[4].uw32_DelayTime = 100;
    SetVltToCur[4].RelayMove.ActiveFlag = LBT22XXX_RLY_Protect;
    SetVltToCur[4].RangeChangedFlag = 1;
		
		SetVltToCur[5].uw32_DelayTime = 100;
    SetVltToCur[5].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
		SetVltToCur[5].RangeChangedFlag = 1;
		
		SetVltToCur[6].uw32_DelayTime = 100;
    SetVltToCur[6].RelayMove.InactiveFlag = LBT22XXX_RLY_Protect;
		SetVltToCur[6].RangeChangedFlag = 1;
		
		SetVltToCur[7].uw32_DelayTime = 90;
    SetVltToCur[7].RelayMove.InactiveFlag = LBT22XXX_RLY_OffGate;
		SetVltToCur[7].RangeChangedFlag = 1;
		
		SetVltToCur[8].uw32_DelayTime = 10;
		
		//CV-OFF
		SetVltToOff[0].uw32_DelayTime = 100;
    SetVltToOff[0].RelayMove.ActiveFlag = LBT22XXX_RLY_OffGate;
		
    SetVltToOff[1].uw32_DelayTime = 100;
		SetVltToOff[1].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO | ACTIVE_DAC_VLT_ZERO;		
		SetVltToOff[1].RelayMove.InactiveFlag = LBT22XXX_RLY_IVS;
		SetVltToOff[1].RelayMove.ActiveFlag = LBT22XXX_RLY_Protect;  

    SetVltToOff[2].uw32_DelayTime = 100;
    SetVltToOff[2].RelayMove.InactiveFlag = (LBT22XXX_RLY_Output | LBT22XXX_RLY_VoltSet | LBT22XXX_RLY_PreC);
    SetVltToOff[2].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

    SetVltToOff[3].uw32_DelayTime = 0;
    SetVltToOff[3].RelayMove.InactiveFlag = LBT22XXX_RLY_Protect | LBT22XXX_RLY_DAMPING | LBT22XXX_RLY_ACIM;
    
		//CV-RST
		SetVltToRst[0].uw32_DelayTime = 100;
    SetVltToRst[0].RelayMove.ActiveFlag = LBT22XXX_RLY_OffGate;
		
		SetVltToRst[1].uw32_DelayTime = 100;
		SetVltToRst[1].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO | ACTIVE_DAC_VLT_ZERO;		
		SetVltToRst[1].RelayMove.InactiveFlag = LBT22XXX_RLY_IVS;
    SetVltToRst[1].RelayMove.ActiveFlag = LBT22XXX_RLY_Protect;    

    SetVltToRst[2].uw32_DelayTime = 100;
    SetVltToRst[2].RelayMove.InactiveFlag = LBT22XXX_RLY_Output;
    SetVltToRst[2].DacMove.DacFlag = ACTIVE_DAC_VH_DEFAULT | ACTIVE_DAC_VL_DEFAULT;

    SetVltToRst[3].uw32_DelayTime = 0;
    SetVltToRst[3].RelayMove.InactiveFlag = LBT22XXX_RLY_Protect;
		SetVltToRst[3].RelayMove.ActiveFlag = LBT22XXX_RLY_DAMPING;    
		    

    SetCtrlOpenAcim[0].uw32_DelayTime = 100;
    SetCtrlOpenAcim[0].RelayMove.ActiveFlag = LBT22XXX_RLY_ACIM;

    SetCtrlCloseAcim[0].uw32_DelayTime = 100;
    SetCtrlCloseAcim[0].RelayMove.InactiveFlag = LBT22XXX_RLY_ACIM;
		#if (IV_AUTORANGE == 1)
	SetAutoRangeSwitch[0].uw32_DelayTime = 200;
    SetAutoRangeSwitch[0].DacMove.DacFlag = ACTIVE_DAC_CUR_ZERO| ACTIVE_DAC_VH | ACTIVE_DAC_VL;
    SetAutoRangeSwitch[0].RangeChangedFlag = 1;

    SetAutoRangeSwitch[1].uw32_DelayTime = 200;
    SetAutoRangeSwitch[1].RelayMove.ActiveFlag = LBT22XXX_RLY_OffGate;
    SetAutoRangeSwitch[1].RangeChangedFlag = 1;

    SetAutoRangeSwitch[2].uw32_DelayTime = 200;
    SetAutoRangeSwitch[2].RelayMove.ActiveFlag = LBT22XXX_RLY_Protect | LBT22XXX_RLY_DAMPING;
    SetAutoRangeSwitch[2].RangeChangedFlag = 1;

    SetAutoRangeSwitch[3].uw32_DelayTime = 100;
    SetAutoRangeSwitch[3].RelayMove.ActiveFlag =  LBT22XXX_RLY_DAMPING;

    SetAutoRangeSwitch[4].uw32_DelayTime = 200;
    SetAutoRangeSwitch[4].RelayMove.RelaySet = ACTIVE_OUT_NEXT_RELAY;
    SetAutoRangeSwitch[4].RelayMove.ActiveFlag = LBT22XXX_RLY_VoltSet;
    SetAutoRangeSwitch[4].RangeChangedFlag = 1;

    SetAutoRangeSwitch[5].uw32_DelayTime = 200;
    SetAutoRangeSwitch[5].RelayMove.InactiveFlag = LBT22XXX_RLY_Protect;
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
