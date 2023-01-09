// *****************************************************
// File Name: Check_V_Limit.h
// Version:        V1.0
// Date:                2012/11/9
// Description:
//                         ����������ѹ��ص�Limit
// Others:
// History��
//
// ******************************************************/

#pragma once

#define PULSE_V_LIMITCHECK_COUNT  2
enum
{
    MAXCOMPVALUE,
    MINCOMPVALUE
};
class Check_V_Limit_task
{
public:
    static UInt16 MyStatus;                                                  //����״̬
    static UInt16 TaskID;                                                    //����ID
    static UInt16 Priority;                                                  //���ȼ�

    static float Max_PulseVoltage;
    static float Min_PulseVoltage;

    static float Max_PulseCurrent;
    static float Min_PulseCurrent;

    static float Max_PulsePower;
    static float Min_PulsePower;
private:
    static unsigned char m_CheckedChannelCount_SL;
public:
    static void DoWork(void);
    static void MyInit(void);
    static void EndTask(void);



public:
    static signed char FindNextStepLimit(void);
    static void S_V_LimitCheck(unsigned char MaxLimitCount);
    static void S_V_CalculateValue(unsigned char ucValueType,float fValue,float * CompValue[2]);
    static void S_V_ChannelIdx(void);

};
