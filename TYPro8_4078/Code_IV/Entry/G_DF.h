#pragma once

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <h>App Configuration
// =====================
//

//   <q>Debug TEST COMMDATA Enable:
//   <i> Debug TST COMMDATA On/Off.
//   <i> Default: Off
#define DEBUG_TEST_COMMDATA_MODE         0           //0：功能被禁止；  1：功能被启用；

//   <q>Debug Trace Record Enable:
//   <i> Debug Trace On/Off.
//   <i> Default: Off
#define DEBUG_TRACE_RECORD               0           //0：功能被禁止；  1：功能被启用；

//   <o>Debug trace record depth:    <2-9999>
//   <i> Default:    300
#define DEBUG_TRACE_RECORD_DEPTH         300         //定义DEBUG信息堆栈深度；

//   <q>Debug Task Statistic Enable:
//   <i> Debug task statistic On/Off.
//   <i> Default: Off
#define DEBUG_TASK_STATISTIC          0           //0：功能被禁止；  1：功能被启用；

//   <q>Thread Task Time Statistic Enable:
//   <i> Thread Task Time statistic On/Off.
//   <i> Default: Off
#define THREAD_TASK_TIME_STATISTIC          0           //0：功能被禁止；  1：功能被启用；

//   <q>Log Debug Data Enable:
//   <i> Log Debug Data On/Off.
//   <i> Default: Off
#define DEBUG_TEST_MODE         0           //0：功能被禁止；  1：功能被启用；

//   <q>Hardware SDRAM 32M Enable:
//   <i> Hardware SDRAM 32M On/Off.
//   <i> Default: On  SDRAM 32M
#define HARDWARE_SDRAM_32M            1            //0：64M；         1：32M；


#define BOARDTYPE_BT2208SINGLE           0     //= 1 2208 八个通道并联成1个通道

//   <q>SCH STATUS REPORT ENABLE
//   <i> Default: Off
#define SCH_STATUS_REPORT_ENABLE                 0         //为0 表示为关闭松耦合报告状态，　 为1表示使用松耦合报告状态。

//   <q>EXTERNAL HIGHT LONG LED
//   <i> Default: Off
#define EXTERNAL_HIGHT_LONG_LED                 1         //为0 表示为关闭外部三色长条型LED灯，　 为1表示使用外部三色长条型LED灯。

//   <q>IV Auto Range
//   <i> Default: Off
#define IV_AUTORANGE                            0         //为0 表示为关闭自动量程，　 为1表示开启自动量程。

//   <q>ENABLE FINE TUNE
//   <i> Default: On
//For those IV channel used to calibrate the ACB box, the Fine Tune needs to be disabled
//在使用IV通道校准ACB时,将ENABLE_FINE_TUNE设为0
#define  ENABLE_FINE_TUNE									1   //bool, Default 1: Enable Fine Tune, 0: Disable Fine Tune

//   <q>FUNCTION FOR DEBUG READ MEMORY
//   <i> Default: On
#define READ_MEMORY_FUNCTION                 1         //为0 表示关闭debug读取内存功能

//   <q>ADC CS5560 ENABLE
//   <i> Default: Off
#define ADC_CS5560_ENABLE                      0             //是否支持CS5560 1=支持 0=不支持

//   <q>MCU WATCHDOG ENABLE
//   <i> Default: Off
#define MCU_WATCHDOG_ENABLE                      1             //是否使用MCU自身的复位看门狗 1=使用 0=不使用  //DKQ 06_17_2014

//-----------------------------------------------------------------------------------

//#if MAXWELL_MACHINE == 1
//#define MCU_PWMFAN_ENABLE                      1     //enable PWM fan
//#define HIGH_RNG_DISCHARGE_MODE                1     //= 1 电流高量程只能放电模式
// #define REDUNDANT_SAFETY_MODE                  1     // 冗余电压保护处理
//#else
//#define MCU_PWMFAN_ENABLE                      0
//#define HIGH_RNG_DISCHARGE_MODE                0
//#define REDUNDANT_SAFETY_MODE                  0
//#endif

//do not modify the following defines:
#define USE_AS_AUXMCU                  0         //为0 表示为IVMCU，　 为1表示使用AUXMCU

/*********************************************************************************
**                            End Of File
*********************************************************************************/

