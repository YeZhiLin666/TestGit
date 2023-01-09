#pragma once
#define LBT21014_RISING_TIME (100)// Rising time,  unit is 100us
#define  LBT21014_Tpid_Tsys           0.3          //(PID contorl Interval)/(System Sample time)     Tpid/Tsys

class LBT21014_Base : public LBT21XXX_Base
{
public:
    static void FeatureEnable_Init(void);
    static void AdditionalEngine(void);
    static void EngineStuff(void);
    //一次性时序
    static St_DisposableTask SetCtrlOpenCh[5];    //打开通道
    static St_DisposableTask SetCtrlCloseCh[4];    //关闭通道
    static St_DisposableTask SetCCToCC[6];         //CC to CC
    static St_DisposableTask SetRstToOff[3];       //do nothing
    static St_DisposableTask SetOffToRst[2];       //do nothing
    static St_DisposableTask SetCtrlOpenAcim[1];
    static St_DisposableTask SetCtrlCloseAcim[1];
	static St_DisposableTask SetAutoRangeSwitch[6];
};


