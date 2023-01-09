#pragma once

class LEDFunc
{
public:
    static UWord32       uw32_GotoBootMode;	 //������ڵ�һλ
    static unsigned char uc_NOP[60];  //   �ճ�60�ֽڿռ����
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


