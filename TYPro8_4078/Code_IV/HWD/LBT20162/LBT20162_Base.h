#pragma once
#define LBT20162_RISING_TIME (50)// Rising time,  unit is 100us
#define  LBT20162_Tpid_Tsys           1          //(PID contorl Interval)/(System Sample time)     Tpid/Tsys

class LBT20162_Base : public LBT21XXX_Base
{
public:
    static void FeatureEnable_Init(void);
    static void AdditionalEngine(void);
    static void EngineStuff(void);
    //一次性时序
    static St_DisposableTask SetCtrlOpenCh[5];    //打开通道
    static St_DisposableTask SetCtrlCloseCh[3];    //关闭通道
    static St_DisposableTask SetCCToCC[4];         //CC to CC
    static St_DisposableTask SetRstToOff[3];       //do nothing
    static St_DisposableTask SetCtrlOpenAcim[1];	//Open ACIM Relay
    static St_DisposableTask SetCtrlCloseAcim[1];	//Close ACIM Relay
    static St_DisposableTask SetAutoRangeSwitch[4];
};



