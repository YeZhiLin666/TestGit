/*****************************************************************************
// 程序文件      :TIMERS.h     主要用于定时器硬件驱动。
// 文件描述      :为LPC4078 MCU board服务
// 编写日期      :2012.07.11
*****************************************************************************/

#ifndef   _TIMERS_H_
#define   _TIMERS_H_

#include "../Data_Manipulation/Datatypes.h"
#include "../Task_Dispatcher/OS_ServeProcess.h"

////Timer////

//user define can change
#define TIME_INTERVAL_0	 (6000)    //60M 100us一次中断

/*==========================================================================
#define TIME_INTERVAL	(60000000/100 - 1)
//depending on the CCLK and PCLK setting, CCLK = 120Mhz, PCLK = 1/2 CCLK=60M
//10mSec = 600.000-1 counts //
//#define TIME_INTERVAL	149999
#define TIME_INTERVAL1ms     (60000000/1000 - 1)    // 1毫秒    (60000-1)=59999
#define TIME_INTERVAL1us      (60000000/1000000 - 1)     // 1微秒 (60-1)=59
============================================================================*/

//#define TIME_INTERVAL_T0	 (5999)    //60M,100us一次中断  (6000-1)=5999
#define TIME_INTERVAL_T0	 (2999)    //60M,50us一次中断  (3000-1)=2999
#define TIME_INTERVAL_T2     (599999)  //60M,10毫秒一次中断  (600000-1)=599999 
#define TIME_INTERVAL_T3     (5999999)  //60M,100毫秒一次中断  (6000000-1)=5999999 
#define TIME_INTERVAL_50US 	 (2999)    //60M,50us????????  (3000-1)=2999
#define TIME_INTERVAL_25US   (1499)         //60M, 25us? (1500-1)=1499
#define TIME_INTERVAL_200US  (11999)    //60M,200us (12000 - 1) = 11999

//comment out chen


////end Timer////

//void RTCHandlerISR(void) __irq;
//void Timer0HandlerISR(void) __irq;
//

///////////////////////////////////////////////////////////////////
//everything in this class should be put into OS_1778 class later//
///////////////////////////////////////////////////////////////////

class TIMERSFunc
{

private:

public:
    static My_Func_Ptr_Void  Timer1Handler;
    static My_Func_Ptr_Void  Timer3Handler;

    static unsigned char Init_Timer(UWord32 timerInterval ,UChar8 timer_num);
    static void Reset_Timer(UChar8 timer_num);
    static void Disable_Timer(UChar8 timer_num);
    static void Enable_Timer(UChar8 timer_num);
    static void DELAY_SYS( UWord32  delayInMs ,unsigned char uc_EnableMs);

    static unsigned long Timer3_GetTick_NoPre(float TimeInMs); //?????   //????????,????? ??tick?;
    static unsigned long Timer3_GetTick_Pre(float TimeInMs);   //????     //????????,????? ??tick?;
    static void Timer3_SetTick_NoPre(unsigned long Tick);      //??????0,?????Tick
    static void Timer3_SetTick_Pre(unsigned long Tick);        //??????59,?????Tick
};



#endif

