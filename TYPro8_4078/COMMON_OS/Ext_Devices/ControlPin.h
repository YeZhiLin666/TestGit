/*****************************************************************************
// �����ļ�      :CP.h     ��Ҫ���ڽ�MCU��GPIO��CPLD��PIN�ڣ�38����������� ͳ���һ��ͳһ�ӿ�
// �ļ�����      :ΪMCU board����
// ��д����      :2012.07.13
*****************************************************************************/

#pragma once

typedef struct
{
    unsigned char m_BitIdx;
    unsigned long m_Address;
    unsigned long m_Value;    //���CPLD�ĳ���Ƚ���֣�����W�˿ڣ�ֻ��д�����ܶ���
} St_CP_CPLD;

typedef struct
{
    unsigned char  m_Port_Index;
    unsigned char  m_Pin_Index;
} St_CP_MCU_GPIO;

typedef struct
{
    unsigned char  m_Port_Index;
    unsigned char  m_Pin_Index;
    unsigned char  m_Function;
} St_CP_MCU;

typedef struct
{
    unsigned char Port;
    unsigned char Bits;
    unsigned char PortType;
    unsigned char Function;

    unsigned long Address;    //CPLD��ַ
} St_Port;

typedef struct
{
    unsigned char  m_Type;

    union
    {
        St_CP_MCU_GPIO   m_GPIO;
        St_CP_CPLD       m_CPLD;
    } m_IO;
} St_CP;

enum eCPTYPE
{
    eCPTYPE_MCUGPIO,
    eCPTYPE_CPLD,
    eCPTYPE_38CODE,
    eCPTYPE_NONE,
};

enum eCONVCODETYPE
{
    eCONVCODETYPE_BIPOLAR,
    eCONVCODETYPE_UNIPOLAR,
};

class ControlPin
{
public:
    static void          SetIObyCP  (St_Port MyPort);
    static void          SetMeHigh  (St_Port MyPort);//,unsigned char Value);
    static void          SetMeLow   (St_Port MyPort);//,unsigned char Value);
    static unsigned char ReadMe     (St_Port MyPort);

    static void          ReInitPullUP(void);
};





