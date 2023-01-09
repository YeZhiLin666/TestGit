/*****************************************************************************
// 程序文件      :IVCH_Main.C     为LPC4078  外部通道IVCH处理主函数程序
// 文件描述      :为LPC4078 MPU board服务
// 编写日期      :2012.04.05
*****************************************************************************/
#ifndef __RTC_H
#define __RTC_H

#include "../Data_Manipulation/Datatypes.h"

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
} SysTime;              //取消掉

// typedef struct
// {
// 	UWord16 Us100;
// 	//UWord32 Second;
// 	UWord64 Second;
// } MsTime;

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



/* Private Macros ------------------------------------------------------------- */
/** @defgroup RTC_Private_Macros RTC Private Macros
* @{
*/

/* ----------------------- BIT DEFINITIONS ----------------------------------- */
/* Miscellaneous register group --------------------------------------------- */
/**********************************************************************
* ILR register definitions
**********************************************************************/
/** ILR register mask */
#define RTC_ILR_BITMASK			((0x00000003))

/** Bit inform the source interrupt is counter increment*/
#define RTC_IRL_RTCCIF			((1<<0))

/** Bit inform the source interrupt is alarm match*/
#define RTC_IRL_RTCALF			((1<<1))

/**********************************************************************
* CCR register definitions
**********************************************************************/
/** CCR register mask */
#define RTC_CCR_BITMASK			((0x00000013))

/** Clock enable */
#define RTC_CCR_CLKEN			((1<<0))

/** Clock reset */
#define RTC_CCR_CTCRST			((1<<1))

/** Calibration counter enable */
#define RTC_CCR_CCALEN			((1<<4))

/**********************************************************************
* CIIR register definitions
**********************************************************************/
/** Counter Increment Interrupt bit for second */
#define RTC_CIIR_IMSEC			((1<<0))

/** Counter Increment Interrupt bit for minute */
#define RTC_CIIR_IMMIN			((1<<1))

/** Counter Increment Interrupt bit for hour */
#define RTC_CIIR_IMHOUR			((1<<2))

/** Counter Increment Interrupt bit for day of month */
#define RTC_CIIR_IMDOM			((1<<3))

/** Counter Increment Interrupt bit for day of week */
#define RTC_CIIR_IMDOW			((1<<4))

/** Counter Increment Interrupt bit for day of year */
#define RTC_CIIR_IMDOY			((1<<5))

/** Counter Increment Interrupt bit for month */
#define RTC_CIIR_IMMON			((1<<6))

/** Counter Increment Interrupt bit for year */
#define RTC_CIIR_IMYEAR			((1<<7))

/** CIIR bit mask */
#define RTC_CIIR_BITMASK		((0xFF))

/**********************************************************************
* AMR register definitions
**********************************************************************/
/** Counter Increment Select Mask bit for second */
#define RTC_AMR_AMRSEC			((1<<0))

/** Counter Increment Select Mask bit for minute */
#define RTC_AMR_AMRMIN			((1<<1))

/** Counter Increment Select Mask bit for hour */
#define RTC_AMR_AMRHOUR			((1<<2))

/** Counter Increment Select Mask bit for day of month */
#define RTC_AMR_AMRDOM			((1<<3))

/** Counter Increment Select Mask bit for day of week */
#define RTC_AMR_AMRDOW			((1<<4))

/** Counter Increment Select Mask bit for day of year */
#define RTC_AMR_AMRDOY			((1<<5))

/** Counter Increment Select Mask bit for month */
#define RTC_AMR_AMRMON			((1<<6))

/** Counter Increment Select Mask bit for year */
#define RTC_AMR_AMRYEAR			((1<<7))

/** AMR bit mask */
#define RTC_AMR_BITMASK			((0xFF))

/**********************************************************************
* RTC_AUX register definitions
**********************************************************************/
/** RTC Oscillator Fail detect flag */
#define RTC_AUX_RTC_OSCF		((1<<4))

/**********************************************************************
* RTC_AUXEN register definitions
**********************************************************************/
/** Oscillator Fail Detect interrupt enable*/
#define RTC_AUXEN_RTC_OSCFEN	((1<<4))

/* Consolidated time register group ----------------------------------- */
/**********************************************************************
* Consolidated Time Register 0 definitions
**********************************************************************/
#define RTC_CTIME0_SECONDS_MASK		((0x3F))
#define RTC_CTIME0_MINUTES_MASK		((0x3F00))
#define RTC_CTIME0_HOURS_MASK		((0x1F0000))
#define RTC_CTIME0_DOW_MASK			((0x7000000))

/**********************************************************************
* Consolidated Time Register 1 definitions
**********************************************************************/
#define RTC_CTIME1_DOM_MASK			((0x1F))
#define RTC_CTIME1_MONTH_MASK		((0xF00))
#define RTC_CTIME1_YEAR_MASK		((0xFFF0000))

/**********************************************************************
* Consolidated Time Register 2 definitions
**********************************************************************/
#define RTC_CTIME2_DOY_MASK			((0xFFF))

/**********************************************************************
* Time Counter Group and Alarm register group
**********************************************************************/
/** SEC register mask */
#define RTC_SEC_MASK			(0x0000003F)

/** MIN register mask */
#define RTC_MIN_MASK			(0x0000003F)

/** HOUR register mask */
#define RTC_HOUR_MASK			(0x0000001F)

/** DOM register mask */
#define RTC_DOM_MASK			(0x0000001F)

/** DOW register mask */
#define RTC_DOW_MASK			(0x00000007)

/** DOY register mask */
#define RTC_DOY_MASK			(0x000001FF)

/** MONTH register mask */
#define RTC_MONTH_MASK			(0x0000000F)

/** YEAR register mask */
#define RTC_YEAR_MASK			(0x00000FFF)


/** Maximum value of second */
#define RTC_SECOND_MAX		59

/** Maximum value of minute*/
#define RTC_MINUTE_MAX		59

/** Maximum value of hour*/
#define RTC_HOUR_MAX		23

/** Minimum value of month*/
#define RTC_MONTH_MIN		1

/** Maximum value of month*/
#define RTC_MONTH_MAX		12

/** Minimum value of day of month*/
#define RTC_DAYOFMONTH_MIN 	1

/** Maximum value of day of month*/
#define RTC_DAYOFMONTH_MAX 	31

/** Maximum value of day of week*/
#define RTC_DAYOFWEEK_MAX	6

/** Minimum value of day of year*/
#define RTC_DAYOFYEAR_MIN	1

/** Maximum value of day of year*/
#define RTC_DAYOFYEAR_MAX	366

/** Maximum value of year*/
#define RTC_YEAR_MAX		4095

/**********************************************************************
* Calibration register
**********************************************************************/
/** Calibration value */
#define RTC_CALIBRATION_CALVAL_MASK		((0x1FFFF))

/** Calibration direction */
#define RTC_CALIBRATION_LIBDIR			((1<<17))

/** Calibration max value */
#define RTC_CALIBRATION_MAX				((0x20000))

/** Calibration definitions */
#define RTC_CALIB_DIR_FORWARD			(0) // (0)
#define RTC_CALIB_DIR_BACKWARD			(RTC_CALIBRATION_LIBDIR) // (1)

/**
* @}
*/


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
// 	static void RTC_InitAndStart(void);
    static void RTCInit( void );
    static void RTCStart( void );
// 	static void RTCStop( void );
    static void RTC_CTCReset( void );
// 	static void RTCSetTime( void);
//     static void RTCGetTime( void );
// 	static void RTCSetAlarm( void);
// 	static void RTCSetAlarmMask( UWord32 AlarmMask );
	static void StopRTC(void);
	static void ReStartRTC(void);
    //private:

protected:
    //	void RTCHandler (void) __irq;
};

#endif /* end __RTC_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
