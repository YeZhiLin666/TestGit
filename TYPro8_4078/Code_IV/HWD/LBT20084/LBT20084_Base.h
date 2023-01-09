#pragma once
#define LBT20084_RISING_TIME (50)// Rising time,  unit is 100us
#define  LBT20084_Tpid_Tsys           1          //(PID contorl Interval)/(System Sample time)     Tpid/Tsys

class LBT20084_Base : public LBT21XXX_Base
{
public:
    static void FeatureEnable_Init(void);
    static void AdditionalEngine(void);
    static void EngineStuff(void);
    static void Engine(void);
    //一次性时序
    static St_DisposableTask SetCtrlOpenCh[6];    //打开通道
    static St_DisposableTask SetCtrlCloseCh[4];    //关闭通道
    static St_DisposableTask SetCCToCC[5];         //CC to CC
    static St_DisposableTask SetRstToOff[3];       //do nothing
    static St_DisposableTask SetCtrlOpenAcim[1];	//Open ACIM Relay
    static St_DisposableTask SetCtrlCloseAcim[1];	//Close ACIM Relay
    static  St_DisposableTask SetRstToCur[5];
    static  St_DisposableTask SetRstToVlt[6];

    static  St_DisposableTask SetOffToRst[3];
    static  St_DisposableTask SetOffToCur[5];
    static  St_DisposableTask SetOffToVlt[6];

    static  St_DisposableTask SetCurToOff[4];
    static St_DisposableTask SetCurToRst[4];
    static St_DisposableTask SetCurToCur[8];
    static St_DisposableTask SetCurToVlt[10];


    static St_DisposableTask SetVltToOff[4];
    static St_DisposableTask SetVltToRst[4];
    static St_DisposableTask SetVltToVlt[9];
    static St_DisposableTask SetVltToCur[11];
	static St_DisposableTask SetAutoRangeSwitch[8];
};

