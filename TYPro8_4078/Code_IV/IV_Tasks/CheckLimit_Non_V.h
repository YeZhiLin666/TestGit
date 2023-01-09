// *****************************************************
// File Name: CheckLimit_Non_V.h
// Version:        V1.0
// Date:                2012/11/9
// Description:
//                         检查脉冲与电压无关的Limit
// Others:
// History：
//
// ******************************************************/

#pragma once
#define CHECK_NON_V_LIMITCOUNT  1
class CheckLimit_Non_V_task
{
public:
    static UInt16 MyStatus;                                                  //任务状态
    static UInt16 TaskID;                                                    //任务ID
    static UInt16 Priority;                                                  //优先级
private:
    static unsigned char m_CheckedChannelCount_LL;
public:
    static unsigned char m_StartCheckLimitID;
public:
    static void DoWork(void);
    static void MyInit(void);
    static void EndTask(void);
private:
    static signed char FindNextLogLimit(void);
    static void S_Non_V_LimitCheck(unsigned char MaxLimitCount);
    static void PULSE_LL_Check(unsigned char MaxLimitCount);
    static void PULSE_LL_DoLog(unsigned char ChannelIdx);
//  static void S_Non_V_CalculateValue(unsigned char uc_ChannelNo,unsigned char ucValueType,float fValue,float * CompValue[2]);
	static void S_Non_V_CalculateValue(unsigned char uc_ChannelNo,unsigned char ucValueType,unsigned char uc_Operator,float fCoefficient,float fValue,float * CompValue[2]);
};
