/*****************************************************************************
// 程序文件      :IVCH_Main.C     LPC2468  外部通道IVCH处理主函数程序
// 文件描述      :为LPC2468 IV-MPU board服务
// 编写日期      :2009.12.01
*****************************************************************************/
#include "../../COMMON/includes.h"


//-------------------------------------------------
	UWord32 RTCFunc::uw32_MSTimeCount;                       //  用于秒的毫秒计数器
	unsigned char RTCFunc::uc_Time48Lock;                    //  48位计数器锁定键，1为锁
	unsigned char RTCFunc::uc_SSFailNum;                     //   秒更新失败次数
    RTCTime  RTCFunc::Str_LocalTime;   // 初如化 时钟   
    RTCTime  RTCFunc::Str_RctTemp;     // 暂存计算时间
//-------------------------------------------------


/******************************************************************************
** Function name:        RTCHandler
**
** Descriptions:         RTC中断对时程序
**                               
** parameters:           None
**                                      
** Returned value:       None
** 
** Created Date:         2009/12/01    
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
void RTCHandler (void) __irq 
{  
//  RTC interrupt handler, it executes based on  the alarm setting

	RTC_ILR |= ILR_RTCCIF;		// clear interrupt flag //
    
    OS_ServeProcess::RTCHandlerISR();
	
	VICVectAddr = 0;		// Acknowledge Interrupt //
}

/******************************************************************************
** Function name:		RTC_InitAndStart
**
** Descriptions:		RTC 初始化并启用
**                               
** parameters:		None
**                                      
** Returned value:	None
** 
** Created Date:      2009/12/01    
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
void RTCFunc::RTC_InitAndStart()
{
    #ifdef _SYSTEM_RTC_
    RTCInit();
    RTCSetTime();		/* Set local time */   
    RTCSetAlarm();		/* set alarm time  */
    RTCStart();
    #endif

}

/******************************************************************************
** Function name:        RTCInit
**
** Descriptions:         RTC 初始化
**                               
** parameters:           None
**                                      
** Returned value:       None
** 
** Created Date:         2009/12/01    
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
//有必要在这里初始化下RTC的各个计时寄存器
void RTCFunc::RTCInit( void )
{
    //Initialize RTC timer  registers
    // Power Up the PCRTC controller. //
	//LPC_SC->PCONP      |= PCRTC;   // Turn On PCRTC Power //
    //alarm_on = 0;  
	RTC_AMR  = 0;      // 报警屏蔽
    RTC_CIIR = 0;      // 计数器增强中断     
    RTC_CCR  = CCR_CTCRST;
    RTC_CCR  = 0;      //时钟控制

	
	//RTC时钟源 二者选一
	//RTC_CCR |= CCR_CLKSRC;   // 如果使用内部晶振则屏蔽，否则开放使用外部32.768KHZ晶振。
    RTC_PREINT  = PREINT_RTC; //使用PCLK 基准预分频器   
    RTC_PREFRAC = PREFRAC_RTC;


   	
    // Setting Timer calibration  
    // Calibration value =  5s;  Direction = Forward calibration
	// So after each 5s, calibration logic can periodically adjust the time counter by incrementing the counter by 2 instead of 1
	//LPC_RTC_CALIBRATION = ((5 - 1) & RTC_CALIBRATION_CALVAL_MASK)|RTC_CALIB_DIR_FORWARD;
	RTC_SEC = OS_ServeProcess::OS_Time.Second;
	RTC_MIN = OS_ServeProcess::OS_Time.Minute;
	RTC_HOUR = OS_ServeProcess::OS_Time.Hour;
	//RTC_DOM =
	//RTC_DOW
	RTC_DOY = OS_ServeProcess::OS_Time.Day;   //日期
	RTC_YEAR = OS_ServeProcess::OS_Time.Year;


    // mask off  alarm  mask,  turn  on  IMSEC  in  the  counter  increment   interrupt  register //
    //RTCSetAlarmMask(AMRSEC|AMRMIN|AMRHOUR|AMRDOM|AMRDOW|AMRDOY|AMRMON|AMRYEAR);
	//RTC_AMR  = (AMRSEC|AMRMIN|AMRHOUR|AMRDOM|AMRDOW|AMRDOY|AMRMON|AMRYEAR);  //alarm not interrupt  // 20091026
    RTC_CISS &= ~(0x01 << 7);   // 必需禁止次秒级中断,因为该寄存器的值不受复位影响  //  20091026
   	RTC_ILR = ILR_RTCCIF;	  //指定计时器增量中断 

	RTC_CIIR = IMSEC ;     //  increment of the Second

	    // install RTC timer handler mainly for alarm control //
    IRQFunc::install_irq( RTC_INT, (void *) RTCHandler,HIGHEST_PRIORITY + 5);
		
    uw32_MSTimeCount = 0;
	RTC_CTCReset();   //  20091026
}

/******************************************************************************
** Function name:		RTCStart
**
** Descriptions:		RTC 启用
**                               
** parameters:		None
**                                      
** Returned value:	None
** 
** Created Date:      2009/12/01    
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
void RTCFunc::RTCStart( void ) 
{
    //  Start RTC timer  counters 
    RTC_CCR = CCR_CTCRST;
    RTC_CCR = 0;
	
	//RTC时钟源 二者选一
	//RTC_CCR |= CCR_CLKSRC;   // 如果使用内部晶振则屏蔽，否则开放使用外部32.768KHZ晶振。
	
    RTC_CCR |= CCR_CLKEN;
    RTC_ILR  = ILR_RTCCIF;   //指定计时器增量中断 
    uw32_MSTimeCount = 0;
	RTC_CTCReset();   //  20091026
}

/******************************************************************************
** Function name:		RTCStop
**
** Descriptions:		RTC 停止
**                               
** parameters:		None
**                                      
** Returned value:	None
** 
** Created Date:      2009/12/01    
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
void RTCFunc::RTCStop( void )
{   
//  Stop RTC timer  counters 
    RTC_CCR &= ~CCR_CLKEN;
} 

/******************************************************************************
** Function name:		RTC_CTCReset
**
** Descriptions:		RTC 复位
**                               
** parameters:		None
**                                      
** Returned value:	None
** 
** Created Date:      2009/12/01    
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
void RTCFunc::RTC_CTCReset( void )
{   
//  Reset RTC clock tick counter
    RTC_CCR |= CCR_CTCRST;
    RTC_CCR ^= CCR_CTCRST;
}

/******************************************************************************
** Function name:		RTCSetTime
**
** Descriptions:		RTC 设置时间
**                               
** parameters:		None
**                                      
** Returned value:	None
** 
** Created Date:      2009/12/01    
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
void RTCFunc::RTCSetTime( void) 
{
   /* Str_LocalTime.RTC_Sec = 0;
    Str_LocalTime.RTC_Min =  0;
    Str_LocalTime.RTC_Hour = 17;
    Str_LocalTime.RTC_Mday = 21;
    Str_LocalTime.RTC_Wday = 3;
    Str_LocalTime.RTC_Yday = 142;		// current date 05/21/2008 
    Str_LocalTime.RTC_Mon =  5;
    Str_LocalTime.RTC_Year = 2008;*/
    
//   Setup RTC timer value
    RTC_SEC = Str_LocalTime.RTC_Sec;
    RTC_MIN = Str_LocalTime.RTC_Min;
    RTC_HOUR = Str_LocalTime.RTC_Hour;
    //RTC_DOM = Str_LocalTime.RTC_Mday;
    //RTC_DOW = Str_LocalTime.RTC_Wday;
    RTC_DOY = Str_LocalTime.RTC_Yday;
    //RTC_MONTH = Str_LocalTime.RTC_Mon;
    RTC_YEAR = Str_LocalTime.RTC_Year; 
}

/******************************************************************************
** Function name:		RTCSetAlarm
**
** Descriptions:		RTC 设置报警
**                               
** parameters:		None
**                                      
** Returned value:	None
** 
** Created Date:      2009/12/01    
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
void RTCFunc::RTCSetAlarm( void)
{   
//  Initialize RTC timer
    RTC_ALSEC = 59;
    RTC_ALMIN = 59;
    RTC_ALHOUR = 23;
    RTC_ALDOM = 31;
    RTC_ALDOW = 6;
    RTC_ALDOY = 365;
    RTC_ALMON = 12;
    RTC_ALYEAR = 2089;    
}

/******************************************************************************
** Function name:		RTCGetTime
**
** Descriptions:		RTC 取得时间
**                               
** parameters:		None
**                                      
** Returned value:	None
** 
** Created Date:      2009/12/01    
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
void  RTCFunc::RTCGetTime( void ) 
{
    //   Get RTC timer value
    //   Returned value: The data structure of the RTC time table
    // RTCTime LocalTime;
    Str_LocalTime.RTC_Sec  = RTC_SEC;
    Str_LocalTime.RTC_Min  = RTC_MIN;
    Str_LocalTime.RTC_Hour = RTC_HOUR;
    // LocalTime.RTC_Mday = RTC_DOM;
    // LocalTime.RTC_Wday = RTC_DOW;
    Str_LocalTime.RTC_Yday = RTC_DOY;
    // LocalTime.RTC_Mon = RTC_MONTH;
    Str_LocalTime.RTC_Year = RTC_YEAR; 
}

/******************************************************************************
** Function name:		RTCSetAlarmMask
**
** Descriptions:		RTC SetAlarmMask
**                               
** parameters:		None
**                                      
** Returned value:	None
** 
** Created Date:      2009/12/01    
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
void RTCFunc::RTCSetAlarmMask( UWord32 AlarmMask ) 
{
//  Set RTC timer alarm mask          AlarmMask:Alarm mask setting
    RTC_AMR = AlarmMask;
}


/*****************************************************************************
**                            End Of File
******************************************************************************/

