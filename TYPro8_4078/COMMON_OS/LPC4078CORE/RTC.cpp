/*****************************************************************************
// �����ļ�      :IVCH_Main.C     LPC4078  �ⲿͨ��IVCH��������������
// �ļ�����      :ΪLPC4078 IV-MPU board����
// ��д����      :2009.12.01
*****************************************************************************/
#include "includes.h"


extern void send_ICMP_TimeRequest(void);    //ICMP
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

void RTC_IRQHandler(void)
{
    //  RTC interrupt handler, it executes based on  the alarm setting
    LPC_RTC->ILR |= RTC_IRL_RTCCIF;		// clear interrupt flag //
    OS_ServeProcess::RTCHandlerISR();
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
// void RTCFunc::RTC_InitAndStart()
// {
// #ifdef _SYSTEM_RTC_
// 	RTCInit();
// 	RTCSetTime();		/* Set local time */
// 	RTCSetAlarm();		/* set alarm time  */
// 	RTCStart();
// #endif

// }

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
void RTCFunc::RTCInit( void )
{
    Arbin_SC->PCONP.BitReg.bPCRTC = 1;
    Arbin_RTC->AMR.Reg = 0;
    Arbin_RTC->CIIR.Reg = 0;
    Arbin_RTC->CCR.BitReg.CTCRST = 1;
    Arbin_RTC->CCR.Reg = 0;
    Arbin_RTC->ILR.Reg = 0;
    Arbin_RTC->SEC.BitReg.SECONDS = 0;  //OS_ServeProcess::OS_Time.Second;
    Arbin_RTC->MIN.BitReg.MINUTES = 0;  //OS_ServeProcess::OS_Time.Minute;
    Arbin_RTC->HOUR.BitReg.HOURS = 0;  //OS_ServeProcess::OS_Time.Hour;
    Arbin_RTC->DOY.BitReg.DOY = 0;     //OS_ServeProcess::OS_Time.Day;
    Arbin_RTC->YEAR.BitReg.YEAR = 2013;//OS_ServeProcess::OS_Time.Year;
    Arbin_RTC->ILR.BitReg.RTCCIF = 1;
    Arbin_RTC->CIIR.BitReg.bIMSEC = 1;
    Arbin_RTC->CCR.BitReg.CCALEN = 1;
    Arbin_RTC->CALIBRATION.BitReg.CALVAL = 0;
    Arbin_RTC->CALIBRATION.BitReg.CALDIR = 0;
    IRQFunc::install_irq( RTC_IRQn, HIGHEST_PRIORITY);// + 5

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
    Arbin_RTC->CCR.BitReg.CTCRST = 1;
    Arbin_RTC->CCR.Reg = 0;
    Arbin_RTC->CCR.BitReg.CLKEN = 1;
    Arbin_RTC->ILR.BitReg.RTCCIF = 1;
    Arbin_RTC->CCR.BitReg.CCALEN = 1;
    Arbin_RTC->CALIBRATION.BitReg.CALVAL = 0;
    Arbin_RTC->CALIBRATION.BitReg.CALDIR = 0;
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
// void RTCFunc::RTCStop( void )
// {
// 	Arbin_RTC->CCR.BitReg.CLKEN = 0;
// }

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
    Arbin_RTC->CCR.BitReg.CTCRST = 1;
    Arbin_RTC->CCR.BitReg.CTCRST ^= 1;       ////////////
    LPC_RTC->ILR |= RTC_IRL_RTCCIF;		// clear interrupt flag //
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
// void RTCFunc::RTCSetTime( void)
// {
// 	Arbin_RTC->SEC.BitReg.SECONDS = Str_LocalTime.RTC_Sec;
// 	Arbin_RTC->MIN.BitReg.MINUTES = Str_LocalTime.RTC_Min;
// 	Arbin_RTC->HOUR.BitReg.HOURS = Str_LocalTime.RTC_Hour;
// 	Arbin_RTC->DOY.BitReg.DOY = Str_LocalTime.RTC_Yday;
// 	Arbin_RTC->YEAR.BitReg.YEAR = Str_LocalTime.RTC_Year;
// }

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
// void RTCFunc::RTCSetAlarm( void)
// {
// 	Arbin_RTC->ALSEC.BitReg.SECONDS = 59;
// 	Arbin_RTC->ALMIN.BitReg.MINUTES = 59;
// 	Arbin_RTC->ALHOUR.BitReg.HOURS = 23;
// 	Arbin_RTC->ALDOM.BitReg.DOM = 31;
// 	Arbin_RTC->ALDOW.BitReg.DOW = 6;
// 	Arbin_RTC->ALDOY.BitReg.DOY = 365;
// 	Arbin_RTC->ALMON.BitReg.MONTH = 12;
// 	Arbin_RTC->ALYEAR.BitReg.YEAR = 2089;
// }

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
// void  RTCFunc::RTCGetTime( void )
// {
//     Str_LocalTime.RTC_Sec = Arbin_RTC->SEC.BitReg.SECONDS;
//     Str_LocalTime.RTC_Min = Arbin_RTC->MIN.BitReg.MINUTES;
//     Str_LocalTime.RTC_Hour = Arbin_RTC->HOUR.BitReg.HOURS;
//     Str_LocalTime.RTC_Yday = Arbin_RTC->DOY.BitReg.DOY;
//     Str_LocalTime.RTC_Year = Arbin_RTC->YEAR.BitReg.YEAR;

// }

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
// void RTCFunc::RTCSetAlarmMask( UWord32 AlarmMask )
// {
// 	Arbin_RTC->AMR.Reg = AlarmMask;
// }


void  RTCFunc::StopRTC( void )
{
    NVIC_DisableIRQ(RTC_IRQn);
    Arbin_RTC->CCR.BitReg.CLKEN = 0;
    RTC_CTCReset();
}

void  RTCFunc::ReStartRTC( void )
{
    RTC_CTCReset();
    Arbin_RTC->CCR.BitReg.CLKEN = 1;
    NVIC_EnableIRQ(RTC_IRQn);
}


/*****************************************************************************
**                            End Of File
******************************************************************************/

