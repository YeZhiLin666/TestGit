#pragma once

class LBT22023_Base : public LBT22XXX_Base
{
public:
	  static St_DisposableTask SetRstToOff[1];
    static St_DisposableTask SetRstToCur[5];
    static St_DisposableTask SetRstToVlt[6];

    static St_DisposableTask SetOffToRst[1];
    static St_DisposableTask SetOffToCur[5];
    static St_DisposableTask SetOffToVlt[6];

    static St_DisposableTask SetCurToOff[5];
    static St_DisposableTask SetCurToRst[5];
    static St_DisposableTask SetCurToCur[9];
    static St_DisposableTask SetCurToVlt[10];


    static St_DisposableTask SetVltToOff[4];
    static St_DisposableTask SetVltToRst[4];
    static St_DisposableTask SetVltToVlt[10];
    static St_DisposableTask SetVltToCur[9];

    static void FeatureEnable_Init(void);
    static void AdditionalEngine(void);
    static void EngineStuff(void);
		static void Engine(void);

};
