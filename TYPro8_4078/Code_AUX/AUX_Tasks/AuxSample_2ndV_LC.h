#pragma once


class Aux_Sample_2ndV_LC
{
public:
    static unsigned short                 Priority;
    static unsigned short                 TaskID;
    static unsigned char									ChanIdx;
    static unsigned char                  CS_Idx;
    static MsTime											 		TimeStamp;
    static unsigned short									TimeCount;   
    static MsTime													WaitTime;
    static unsigned char									TaskStatus;
    static St_Port                        Pin_MUXWR;
    static St_Port                        Pin_VAD0;
    static St_Port                        Pin_VAD1;
    static St_Port                        Pin_VAD2;
    static St_Port                        Pin_VAD3;
    static unsigned short                 Sample_Wait_Count;
    static unsigned short                 Sample_Wait_Count2;
    //static void (*m_ReadDI)(void);
    //static void (*m_InitDI)(void);
private:

public:
    static My_Func_Ptr_Void              Board_Adc_Trigger;
    static My_Func_Ptr_Ext1              Board_Adc_Read;
    static void Init(void);
    static void DoWork(void);
    static void SetChip(unsigned char ChIdx);

};
