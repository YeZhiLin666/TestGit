#pragma once
#define LBT21162_RISING_TIME (50) // Rising time,  unit is 100us
#define  LBT21162_Tpid_Tsys           1          //(PID contorl Interval)/(System Sample time)     Tpid/Tsys
class LBT21162_Base : public LBT21XXX_Base
{
public:
    static void FeatureEnable_Init(void);
    static void AdditionalEngine(void);
    static void EngineStuff(void);
    //һ����ʱ��
    static St_DisposableTask SetCtrlOpenCh[5];    //��ͨ��
    static St_DisposableTask SetCtrlCloseCh[4];    //�ر�ͨ��
    static St_DisposableTask SetCCToCC[4];         //CC to CC
    static St_DisposableTask SetRstToOff[2];       //do nothing
    static St_DisposableTask SetCtrlOpenAcim[1];	//Open ACIM Relay
    static St_DisposableTask SetCtrlCloseAcim[1];	//Close ACIM Relay
	static St_DisposableTask SetAutoRangeSwitch[4];
};


