/*****************************************************************************
// �����ļ�      :IVCH_Main.C     LPC2468  �ⲿͨ��IVCH��������������
// �ļ�����      :ΪLPC2468 IV-MPU board����
// ��д����      :2009.12.01
*****************************************************************************/
#include "../../COMMON/includes.h"


//-------------------------------------------------
	UWord32 RTCFunc::uw32_MSTimeCount;                       //  ������ĺ��������
	unsigned char RTCFunc::uc_Time48Lock;                    //  48λ��������������1Ϊ��
	unsigned char RTCFunc::uc_SSFailNum;                     //   �����ʧ�ܴ���
    RTCTime  RTCFunc::Str_LocalTime;   // ���绯 ʱ��   
    RTCTime  RTCFunc::Str_RctTemp;     // �ݴ����ʱ��
//-------------------------------------------------


/******************************************************************************
** Function name:        RTCHandler
**
** Descriptions:         RTC�ж϶�ʱ����
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
** Descriptions:		RTC ��ʼ��������
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
** Descriptions:         RTC ��ʼ��
**                               
** parameters:           None
**                                      
** Returned value:       None
** 
** Created Date:         2009/12/01    
** 
** Revision Dates:                                                                                                                  

******************************************************************************/
//�б�Ҫ�������ʼ����RTC�ĸ�����ʱ�Ĵ���
void RTCFunc::RTCInit( void )
{
    //Initialize RTC timer  registers
    // Power Up the PCRTC controller. //
	//LPC_SC->PCONP      |= PCRTC;   // Turn On PCRTC Power //
    //alarm_on = 0;  
	RTC_AMR  = 0;      // ��������
    RTC_CIIR = 0;      // ��������ǿ�ж�     
    RTC_CCR  = CCR_CTCRST;
    RTC_CCR  = 0;      //ʱ�ӿ���

	
	//RTCʱ��Դ ����ѡһ
	//RTC_CCR |= CCR_CLKSRC;   // ���ʹ���ڲ����������Σ����򿪷�ʹ���ⲿ32.768KHZ����
    RTC_PREINT  = PREINT_RTC; //ʹ��PCLK ��׼Ԥ��Ƶ��   
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
	RTC_DOY = OS_ServeProcess::OS_Time.Day;   //����
	RTC_YEAR = OS_ServeProcess::OS_Time.Year;


    // mask off  alarm  mask,  turn  on  IMSEC  in  the  counter  increment   interrupt  register //
    //RTCSetAlarmMask(AMRSEC|AMRMIN|AMRHOUR|AMRDOM|AMRDOW|AMRDOY|AMRMON|AMRYEAR);
	//RTC_AMR  = (AMRSEC|AMRMIN|AMRHOUR|AMRDOM|AMRDOW|AMRDOY|AMRMON|AMRYEAR);  //alarm not interrupt  // 20091026
    RTC_CISS &= ~(0x01 << 7);   // �����ֹ���뼶�ж�,��Ϊ�üĴ�����ֵ���ܸ�λӰ��  //  20091026
   	RTC_ILR = ILR_RTCCIF;	  //ָ����ʱ�������ж� 

	RTC_CIIR = IMSEC ;     //  increment of the Second

	    // install RTC timer handler mainly for alarm control //
    IRQFunc::install_irq( RTC_INT, (void *) RTCHandler,HIGHEST_PRIORITY + 5);
		
    uw32_MSTimeCount = 0;
	RTC_CTCReset();   //  20091026
}

/******************************************************************************
** Function name:		RTCStart
**
** Descriptions:		RTC ����
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
	
	//RTCʱ��Դ ����ѡһ
	//RTC_CCR |= CCR_CLKSRC;   // ���ʹ���ڲ����������Σ����򿪷�ʹ���ⲿ32.768KHZ����
	
    RTC_CCR |= CCR_CLKEN;
    RTC_ILR  = ILR_RTCCIF;   //ָ����ʱ�������ж� 
    uw32_MSTimeCount = 0;
	RTC_CTCReset();   //  20091026
}

/******************************************************************************
** Function name:		RTCStop
**
** Descriptions:		RTC ֹͣ
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
** Descriptions:		RTC ��λ
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
** Descriptions:		RTC ����ʱ��
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
** Descriptions:		RTC ���ñ���
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
** Descriptions:		RTC ȡ��ʱ��
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

