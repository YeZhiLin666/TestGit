#pragma once

class LEDFunc
{
public:
    static UWord32       uw32_GotoBootMode;	 //必须放在第一位
    static unsigned char uc_NOP[60];  //   空出60字节空间外存
    static UWord32       uw16_LedTimer;
    static UInt16        Priority;
    static UInt16        TaskID;
    static unsigned char uc_TimeLED;
    static const St_Port Pin_HeartBeat;
    static unsigned short m_SysStatus;

public:
    static void Init(void);
    static void DoWork(void);
};


