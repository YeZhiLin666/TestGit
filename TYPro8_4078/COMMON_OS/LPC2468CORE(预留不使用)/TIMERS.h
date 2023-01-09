
#ifndef   _TIMERS_H_
#define   _TIMERS_H_

#include "../../COMMON/Datatypes.h"

//包含了RTC和Timer的 初始化

////RTC////
/*typedef struct 
{
	UInt16 Year;	// Year value - [0,4095]
	UInt16 Day;		// Day of year value - [1,365]
	UInt16 Hour;	// Hour value - [0,23]
	UInt16 Minute;	// Minute value - [0,59]
	UInt16 Second;	// Second value - [0,59]
	UInt16 Us100;	// 100us units value - [0, 10000]
	UInt32 InSeconds;
	UInt16 IcmpTick;

} SysTime;

typedef struct
{
	UInt16 Us100;
	UInt32 Second;
} MsTime;*/



#define IMSEC		0x00000001
#define IMMIN		0x00000002
#define IMHOUR		0x00000004
#define IMDOM		0x00000008
#define IMDOW		0x00000010
#define IMDOY		0x00000020
#define IMMON		0x00000040
#define IMYEAR		0x00000080

#define AMRSEC		0x00000001  /* Alarm mask for Seconds */
#define AMRMIN		0x00000002  /* Alarm mask for Minutes */
#define AMRHOUR		0x00000004  /* Alarm mask for Hours */
#define AMRDOM		0x00000008  /* Alarm mask for Day of Month */
#define AMRDOW		0x00000010  /* Alarm mask for Day of Week */
#define AMRDOY		0x00000020  /* Alarm mask for Day of Year */
#define AMRMON		0x00000040  /* Alarm mask for Month */
#define AMRYEAR		0x00000080  /* Alarm mask for Year */

//#define PREINT_RTC	0x00000224//0x000001C8  /* Prescaler value, integer portion,   PCLK18M               PCLK = 15Mhz */
//#define PREFRAC_RTC	0x00002880//0x000061C0  /* Prescaler value, fraction portion,  PCLK18M               PCLK = 15Mhz */
/*
#define ILR_RTCCIF	0x01
#define ILR_RTCALF	0x02

#define CCR_CLKEN	0x01
#define CCR_CTCRST	0x02
#define CCR_CLKSRC	0x10*/
////end RTC////

////Timer////

//user define can change
//#define TIME_INTERVAL_0	  (6000)     //60M 100us一次中断
//#define TIME_INTERVAL_0	  1800       //18M 100us一次中断   
#define TIME_INTERVAL_0	      (Fpclk/10000 - 1) // 100us一次中断   

//#define TIME_INTERVAL	      (60000000/100 - 1) // 10毫秒
#define TIME_INTERVAL	      (Fpclk/100 - 1) // 10毫秒
//depending on the CCLK and PCLK setting, CCLK = 120Mhz, PCLK = 1/2 CCLK=60M
//10mSec = 600.000-1 counts //
//#define TIME_INTERVAL	149999	
//#define TIME_INTERVAL1ms      (60000000/1000 - 1)    // 1毫秒
//#define TIME_INTERVAL1us      (60000000/1000000 - 1)     // 1微秒
#define TIME_INTERVAL1ms      (Fpclk/1000 - 1)    // 1毫秒
#define TIME_INTERVAL1us      (Fpclk/1000000 - 1)     // 1微秒
//comment out chen


////end Timer////

//void RTCHandlerISR(void) __irq;
//void Timer0HandlerISR(void) __irq;
//

///////////////////////////////////////////////////////////////////
//everything in this class should be put into OS_2468 class later//
///////////////////////////////////////////////////////////////////

class TIMERSFunc
{
private:

	//Timer0和RTC已经转移到os_2468
	//static void RTCInit(void);
	//static void RTCStart(void);
	//static void RTCStop(void);

	//static void RTCHandlerISR(void) __irq;    //RTC ISR
    //static void Timer0HandlerISR(void) __irq;    //timer0 ISR 
  
    //static void Timer1HandlerISR(void) __irq;    //timer1 ISR
	//static void Timer2HandlerISR(void) __irq;    //timer2 ISR
	//static void Timer3HandlerISR(void) __irq;    //timer3 ISR

	//static void TimeCalibration(void);
	//static UInt16 TimeAdd(UInt16 operand1, UInt16 operand2, UInt16 opLimit, UInt16 opFlag);
	//static UInt16 IfLeap(UInt16 myYear);
public:

	
	//相关的RTC对时操作已经转移到os_2468中
	//Revised User Interface
	//static void RTCInit(void);
	//static void RTCStart(void);
	//static void RTCStop(void);
  
	static UWord32 Init_Timer(UWord32 timerInterval ,UChar8 timer_num);
	static void Reset_Timer(UChar8 timer_num);
	static void Disable_Timer(UChar8 timer_num);
	static void Enable_Timer(UChar8 timer_num);
	static void DELAY_SYS(UWord32  delayInMs);

	// <<<End>>>Revised User Interface
};



#endif

