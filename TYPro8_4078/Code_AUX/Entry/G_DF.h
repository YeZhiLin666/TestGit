#pragma once

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <h>App Configuration
// =====================
//

//   <q>Demo Mode Enable:
//   <i> Demo Mode On/Off.
//   <i> Default: Off
#define DEMO_MODE                        0

//   <q>Debug Trace Record Enable:
//   <i> Debug Trace On/Off.
//   <i> Default: Off
#define DEBUG_TRACE_RECORD               0           //0：功能被禁止；  1：功能被启用；
//   <o>Debug trace record depth:    <2-9999>
//   <i> Default:    300
#define DEBUG_TRACE_RECORD_DEPTH         300         //定义DEBUG信息堆栈深度；

//   <q>Thread Task Time Statistic Enable:
//   <i> Thread Task Time statistic On/Off.
//   <i> Default: Off
#define THREAD_TASK_TIME_STATISTIC          0           //0：功能被禁止；  1：功能被启用；

//   <q>Debug Task Statistic Enable:
//   <i> Debug task statistic On/Off.
//   <i> Default: Off
#define DEBUG_TASK_STATISTIC          0           //0：功能被禁止；  1：功能被启用；

//   <o>OS Tick Interval <200=> 200us <400=> 400us <800=> 800us
//   <i> Can be 200/400/800us
//   <i> Default:    800us
#define OS_TICK_INTERVAL              800         //定义mini-OS系统调度时间间隔

//   <q>Debug TEST COMMDATA Enable:
//   <i> Debug TST COMMDATA On/Off.
//   <i> Default: Off
#define DEBUG_TEST_COMMDATA_MODE         0           //0：功能被禁止；  1：功能被启用；


//   <q>Hardware SDRAM 32M Enable:
//   <i> Hardware SDRAM 32M On/Off.
//   <i> Default: On  SDRAM 32M
#define HARDWARE_SDRAM_32M            1            //0：64M；  1：32M；

//   <q> COM INTERVAL WRITE AND READ Enable:
//   <i> COM INTERVAL WRITE AND READ On/Off.
//   <i> Default: On
#define  COM_INTERVAL_WRITE_AND_READ                 1         //是否使用串口间隔写和读功能 为1表示使用间隔写和读功能，为0表示不使用间隔写和读功能

//   <q>EXTERNAL HIGHT LONG LED
//   <i> Default: Off
#define EXTERNAL_HIGHT_LONG_LED                 1         //为0 表示为关闭外部三色长条型LED灯，　 为1表示使用外部三色长条型LED灯。

//   <q>FUNCTION FOR DEBUG READ MEMORY
//   <i> Default: On
#define READ_MEMORY_FUNCTION                 1         //

//   <q>MCU WATCHDOG ENABLE
//   <i> Default: Off
#define MCU_WATCHDOG_ENABLE                      1            //是否使用MCU自身的复位看门狗 1=使用 0=不使用  //DKQ 06_17_2014

//#define REDUNDANT_PIN     0
//#define REDUNDANT_M0      1
//   SUPPORT REDUNDANT_VOLTAGE_MODE
//   REDUNDANT_VOLT_MODE   <0=> REDUNDANT_PIN     <1=> REDUNDANT_M0
//   Default: REDUNDANT_PIN
//#define REDUNDANT_VOLT_MODE             REDUNDANT_PIN				//为0是Redundant冗余电压采用端口管脚报警，为1是冗余电压采用M0串口报警。


#define  USE_AS_AUXMCU                 1         //是否使用AUX通信协议功能 为1表示使用AUX功能，为0表示不使用AUX功能




/*********************************************************************************
**                            End Of File
*********************************************************************************/

