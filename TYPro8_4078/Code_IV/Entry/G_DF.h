#pragma once

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <h>App Configuration
// =====================
//

//   <q>Debug TEST COMMDATA Enable:
//   <i> Debug TST COMMDATA On/Off.
//   <i> Default: Off
#define DEBUG_TEST_COMMDATA_MODE         0           //0�����ܱ���ֹ��  1�����ܱ����ã�

//   <q>Debug Trace Record Enable:
//   <i> Debug Trace On/Off.
//   <i> Default: Off
#define DEBUG_TRACE_RECORD               0           //0�����ܱ���ֹ��  1�����ܱ����ã�

//   <o>Debug trace record depth:    <2-9999>
//   <i> Default:    300
#define DEBUG_TRACE_RECORD_DEPTH         300         //����DEBUG��Ϣ��ջ��ȣ�

//   <q>Debug Task Statistic Enable:
//   <i> Debug task statistic On/Off.
//   <i> Default: Off
#define DEBUG_TASK_STATISTIC          0           //0�����ܱ���ֹ��  1�����ܱ����ã�

//   <q>Thread Task Time Statistic Enable:
//   <i> Thread Task Time statistic On/Off.
//   <i> Default: Off
#define THREAD_TASK_TIME_STATISTIC          0           //0�����ܱ���ֹ��  1�����ܱ����ã�

//   <q>Log Debug Data Enable:
//   <i> Log Debug Data On/Off.
//   <i> Default: Off
#define DEBUG_TEST_MODE         0           //0�����ܱ���ֹ��  1�����ܱ����ã�

//   <q>Hardware SDRAM 32M Enable:
//   <i> Hardware SDRAM 32M On/Off.
//   <i> Default: On  SDRAM 32M
#define HARDWARE_SDRAM_32M            1            //0��64M��         1��32M��


#define BOARDTYPE_BT2208SINGLE           0     //= 1 2208 �˸�ͨ��������1��ͨ��

//   <q>SCH STATUS REPORT ENABLE
//   <i> Default: Off
#define SCH_STATUS_REPORT_ENABLE                 0         //Ϊ0 ��ʾΪ�ر�����ϱ���״̬���� Ϊ1��ʾʹ������ϱ���״̬��

//   <q>EXTERNAL HIGHT LONG LED
//   <i> Default: Off
#define EXTERNAL_HIGHT_LONG_LED                 1         //Ϊ0 ��ʾΪ�ر��ⲿ��ɫ������LED�ƣ��� Ϊ1��ʾʹ���ⲿ��ɫ������LED�ơ�

//   <q>IV Auto Range
//   <i> Default: Off
#define IV_AUTORANGE                            0         //Ϊ0 ��ʾΪ�ر��Զ����̣��� Ϊ1��ʾ�����Զ����̡�

//   <q>ENABLE FINE TUNE
//   <i> Default: On
//For those IV channel used to calibrate the ACB box, the Fine Tune needs to be disabled
//��ʹ��IVͨ��У׼ACBʱ,��ENABLE_FINE_TUNE��Ϊ0
#define  ENABLE_FINE_TUNE									1   //bool, Default 1: Enable Fine Tune, 0: Disable Fine Tune

//   <q>FUNCTION FOR DEBUG READ MEMORY
//   <i> Default: On
#define READ_MEMORY_FUNCTION                 1         //Ϊ0 ��ʾ�ر�debug��ȡ�ڴ湦��

//   <q>ADC CS5560 ENABLE
//   <i> Default: Off
#define ADC_CS5560_ENABLE                      0             //�Ƿ�֧��CS5560 1=֧�� 0=��֧��

//   <q>MCU WATCHDOG ENABLE
//   <i> Default: Off
#define MCU_WATCHDOG_ENABLE                      1             //�Ƿ�ʹ��MCU����ĸ�λ���Ź� 1=ʹ�� 0=��ʹ��  //DKQ 06_17_2014

//-----------------------------------------------------------------------------------

//#if MAXWELL_MACHINE == 1
//#define MCU_PWMFAN_ENABLE                      1     //enable PWM fan
//#define HIGH_RNG_DISCHARGE_MODE                1     //= 1 ����������ֻ�ܷŵ�ģʽ
// #define REDUNDANT_SAFETY_MODE                  1     // �����ѹ��������
//#else
//#define MCU_PWMFAN_ENABLE                      0
//#define HIGH_RNG_DISCHARGE_MODE                0
//#define REDUNDANT_SAFETY_MODE                  0
//#endif

//do not modify the following defines:
#define USE_AS_AUXMCU                  0         //Ϊ0 ��ʾΪIVMCU���� Ϊ1��ʾʹ��AUXMCU

/*********************************************************************************
**                            End Of File
*********************************************************************************/

