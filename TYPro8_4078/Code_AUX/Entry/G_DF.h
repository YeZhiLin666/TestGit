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
#define DEBUG_TRACE_RECORD               0           //0�����ܱ���ֹ��  1�����ܱ����ã�
//   <o>Debug trace record depth:    <2-9999>
//   <i> Default:    300
#define DEBUG_TRACE_RECORD_DEPTH         300         //����DEBUG��Ϣ��ջ��ȣ�

//   <q>Thread Task Time Statistic Enable:
//   <i> Thread Task Time statistic On/Off.
//   <i> Default: Off
#define THREAD_TASK_TIME_STATISTIC          0           //0�����ܱ���ֹ��  1�����ܱ����ã�

//   <q>Debug Task Statistic Enable:
//   <i> Debug task statistic On/Off.
//   <i> Default: Off
#define DEBUG_TASK_STATISTIC          0           //0�����ܱ���ֹ��  1�����ܱ����ã�

//   <o>OS Tick Interval <200=> 200us <400=> 400us <800=> 800us
//   <i> Can be 200/400/800us
//   <i> Default:    800us
#define OS_TICK_INTERVAL              800         //����mini-OSϵͳ����ʱ����

//   <q>Debug TEST COMMDATA Enable:
//   <i> Debug TST COMMDATA On/Off.
//   <i> Default: Off
#define DEBUG_TEST_COMMDATA_MODE         0           //0�����ܱ���ֹ��  1�����ܱ����ã�


//   <q>Hardware SDRAM 32M Enable:
//   <i> Hardware SDRAM 32M On/Off.
//   <i> Default: On  SDRAM 32M
#define HARDWARE_SDRAM_32M            1            //0��64M��  1��32M��

//   <q> COM INTERVAL WRITE AND READ Enable:
//   <i> COM INTERVAL WRITE AND READ On/Off.
//   <i> Default: On
#define  COM_INTERVAL_WRITE_AND_READ                 1         //�Ƿ�ʹ�ô��ڼ��д�Ͷ����� Ϊ1��ʾʹ�ü��д�Ͷ����ܣ�Ϊ0��ʾ��ʹ�ü��д�Ͷ�����

//   <q>EXTERNAL HIGHT LONG LED
//   <i> Default: Off
#define EXTERNAL_HIGHT_LONG_LED                 1         //Ϊ0 ��ʾΪ�ر��ⲿ��ɫ������LED�ƣ��� Ϊ1��ʾʹ���ⲿ��ɫ������LED�ơ�

//   <q>FUNCTION FOR DEBUG READ MEMORY
//   <i> Default: On
#define READ_MEMORY_FUNCTION                 1         //

//   <q>MCU WATCHDOG ENABLE
//   <i> Default: Off
#define MCU_WATCHDOG_ENABLE                      1            //�Ƿ�ʹ��MCU����ĸ�λ���Ź� 1=ʹ�� 0=��ʹ��  //DKQ 06_17_2014

//#define REDUNDANT_PIN     0
//#define REDUNDANT_M0      1
//   SUPPORT REDUNDANT_VOLTAGE_MODE
//   REDUNDANT_VOLT_MODE   <0=> REDUNDANT_PIN     <1=> REDUNDANT_M0
//   Default: REDUNDANT_PIN
//#define REDUNDANT_VOLT_MODE             REDUNDANT_PIN				//Ϊ0��Redundant�����ѹ���ö˿ڹܽű�����Ϊ1�������ѹ����M0���ڱ�����


#define  USE_AS_AUXMCU                 1         //�Ƿ�ʹ��AUXͨ��Э�鹦�� Ϊ1��ʾʹ��AUX���ܣ�Ϊ0��ʾ��ʹ��AUX����




/*********************************************************************************
**                            End Of File
*********************************************************************************/

