

#ifndef _MAIN_
#define _MAIN_

#include "../../COMMON_os/Ext_Devices/ControlPin.h"
class AUX_Unit_Base
{
public:
    // 				static SSPI_Para Temper_SSP0ADC;
    static const St_Port Pin_SCK0;
    static const St_Port Pin_MISO0;
    static const St_Port Pin_MOSI0;

    static const St_Port Pin_SCK1;
    static const St_Port Pin_MISO1;
    static const St_Port Pin_MOSI1;

    static const St_Port Pin_ECODE1;
    static const St_Port Pin_ECODE2;
    static const St_Port Pin_ECODE3;
    static const St_Port Pin_EOE;
    static const St_Port Pin_ADCBUSY;
    static bool  m_EnableISRsample; //Add by DKQ to enable or disabale ISR sample. for example with 2nd_LC board, should disable ISR sample

};

void PrepareForInitAUX(void) ;
void Software_Init(unsigned char Aux_BoardID);
unsigned char Aux_getBID(void);
void Hardware_Init(unsigned char Aux_BoardID);
void IO_Init(void);
void SelectiveMountTask(void);





#endif
