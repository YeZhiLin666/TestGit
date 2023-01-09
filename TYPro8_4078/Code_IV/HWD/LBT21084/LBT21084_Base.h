#pragma once
#define LBT21084_RISING_TIME (10)// Rising time,  unit is 100us
#define  LBT21084_Tpid_Tsys           1          //(PID contorl Interval)/(System Sample time)     Tpid/Tsys

class LBT21084_Base : public LBT21XXX_Base
{
public:
    static void FeatureEnable_Init(void);
    static void AdditionalEngine(void);
    static void EngineStuff(void);
	static void Engine(void);
    //一次性时序
    static St_DisposableTask SetCtrlOpenCh[5];    //打开通道
    static St_DisposableTask SetCtrlCloseCh[4];    //关闭通道
    static St_DisposableTask SetCCToCC[6];         //CC to CC
    static St_DisposableTask SetRstToOff[3];       //do nothing
    static St_DisposableTask SetCtrlOpenAcim[1];
    static St_DisposableTask SetCtrlCloseAcim[1];
#if (IV_AUTORANGE == 1)
    static St_DisposableTask SetAutoRangeSwitch[6];
#endif
	static  St_DisposableTask SetOffToRst[3];
	static  St_DisposableTask SetOffToCur[4];
	static  St_DisposableTask SetOffToVlt[5];

	static  St_DisposableTask SetCurToOff[3];
// 	static St_DisposableTask SetCurToRst[3];
	static St_DisposableTask SetCurToCur[7];
	static St_DisposableTask SetCurToVlt[9];


	static St_DisposableTask SetVltToOff[3];
// 	static St_DisposableTask SetVltToRst[3];
	static St_DisposableTask SetVltToVlt[8];
	static St_DisposableTask SetVltToCur[6];
};


