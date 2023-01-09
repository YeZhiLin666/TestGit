#pragma once
#define MSTAT21044_RISING_TIME 50// Rising time 5ms, unit is 100us
#define  MSTAT21044_Tpid_Tsys           1          //(PID contorl Interval)/(System Sample time)     Tpid/Tsys
class MSTAT21044_Base : public LBT21XXX_Base
{
public:
    static void FeatureEnable_Init(void);
    static void AdditionalEngine(void);
    static void EngineStuff(void);
    static St_DisposableTask SetCtrlOpenCh[5];    //打开通道
    static St_DisposableTask SetCtrlCloseCh[3];    //关闭通道
    static St_DisposableTask SetCCToCC[6];         //CC to CC
    static St_DisposableTask SetRstToOff[2];       //do nothing
	
	static St_DisposableTask SetRstToCur[5];
	static St_DisposableTask SetRstToVlt[5];

	static St_DisposableTask SetOffToVlt[5];
	static St_DisposableTask SetCurToVlt[7];
	static St_DisposableTask SetVltToOff[3];
	static St_DisposableTask SetVltToRst[3];
	static St_DisposableTask SetVltToVlt[6];
	static St_DisposableTask SetVltToCur[8];
	
	static St_DisposableTask SetCtrlOpenAcim[1];
    static St_DisposableTask SetCtrlCloseAcim[1];
#if (IV_AUTORANGE == 1)
	static St_DisposableTask SetAutoRangeSwitch[6];
	#endif
};
