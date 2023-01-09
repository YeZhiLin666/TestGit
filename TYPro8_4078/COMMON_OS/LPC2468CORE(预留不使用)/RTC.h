/*****************************************************************************
// 程序文件      :IVCH_Main.C     外部通道IVCH处理主函数程序
// 文件描述      :为MPU board服务
// 编写日期      :2012.04.05
*****************************************************************************/
#ifndef __RTC_H 
#define __RTC_H

#include "../../COMMON/Datatypes.h"

// 包含了RTC和Timer的 初始化

////RTC////
typedef struct 
{
	UWord16 Year;	// Year value - [0,4095]
	UWord16 Day;	// Day of year value - [1,365]
	UWord16 Hour;	// Hour value - [0,23]
	UWord16 Minute;	// Minute value - [0,59]
	UWord16 Second;	// Second value - [0,59]
	UWord16 Us100;	// 100us units value - [0, 10000]
	//UInt32  InSeconds;
	UWord64 InSeconds;
	UWord16 IcmpTick;    //ICMP发起Ticks

	unsigned char m_bTimeOK;

} SysTime;

typedef struct
{
	UWord16 Us100;
	//UWord32 Second;
	UWord64 Second;
} MsTime;


/*
typedef struct {
    UWord32 RTC_Sec;     // Second value - [0,59] //
    UWord32 RTC_Min;     // Minute value - [0,59]//
    UWord32 RTC_Hour;    // Hour value - [0,23] //
    UWord32 RTC_Mday;    // Day of the month value - [1,31]//
    UWord32 RTC_Mon;     // Month value - [1,12] //
    UWord32 RTC_Year;    // Year value - [0,4095] //
    UWord32 RTC_Wday;    // Day of week value - [0,6] //
    UWord32 RTC_Yday;    // Day of year value - [1,365] //
} RTCTime;
  */

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


#define PREINT_RTC	0x00000224  // Prescaler value, integer portion, 
				   // PCLK = 18Mhz //

#define PREFRAC_RTC	0x00002880  // Prescaler value, fraction portion, 
				    //PCLK = 18Mhz // 

#define ILR_RTCCIF	0x01
#define ILR_RTCALF	0x02

#define CCR_CLKEN	0x01
#define CCR_CTCRST	0x02
#define CCR_CLKSRC	0x10

////end RTC////

 

/*void RTCHandler (void) __irq;
void RTC_InitAndStart(void);
void RTCInit( void );
void RTCStart( void );
void RTCStop( void );
void RTC_CTCReset( void );
void RTCSetTime( void);
void RTCGetTime( void );
void RTCSetAlarm( void);
void RTCSetAlarmMask( UWord32 AlarmMask );*/

struct  RTCTime 
{
    UWord32 RTC_Sec;     // Second value - [0,59] //
    UWord32 RTC_Min;     // Minute value - [0,59]//
    UWord32 RTC_Hour;    // Hour value - [0,23] //
    UWord32 RTC_Mday;    // Day of the month value - [1,31]//
    UWord32 RTC_Mon;     // Month value - [1,12] //
    UWord32 RTC_Year;    // Year value - [0,4095] //
    UWord32 RTC_Wday;    // Day of week value - [0,6] //
    UWord32 RTC_Yday;    // Day of year value - [1,365] //
}; 

class  RTCFunc  
{
//Attribute
public:
	static UWord32 uw32_MSTimeCount;                       //  用于秒的毫秒计数器
	static unsigned char uc_Time48Lock;                    //  48位计数器锁定键，1为锁
	static unsigned char uc_SSFailNum;                     //   秒更新失败次数

    static RTCTime  Str_LocalTime;   // 初如化 时钟   
    static RTCTime  Str_RctTemp;     // 暂存计算时间

//private:

//protected:


//Operation
public:
	static void RTC_InitAndStart(void);
	static void RTCInit( void );
	static void RTCStart( void );
	static void RTCStop( void );
	static void RTC_CTCReset( void );
	static void RTCSetTime( void);
	static void RTCGetTime( void );
	static void RTCSetAlarm( void);
	static void RTCSetAlarmMask( UWord32 AlarmMask );

//private:

protected:
//	void RTCHandler (void) __irq;
}; 
	
#endif /* end __RTC_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
