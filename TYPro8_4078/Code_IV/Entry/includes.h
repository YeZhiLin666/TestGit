#pragma once

// stb lib
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <float.h>

// COMMON     常用库、公共宏定义文件夹:
#include "../../COMMON_OS/Data_Manipulation/Datatypes.h"
#include "../Entry/G_DF.h"
#include "../Entry/includes.h"
#include "../../COMMON_OS/Data_Manipulation/CircularQueue.h"
#include "../../COMMON_OS/Data_Manipulation/CircularQueue_Lin.h"
#include "../../COMMON_OS/Data_Manipulation/BitArray.h"
#include "../../COMMON_OS/Data_Manipulation/Lib_Mem.h"


// APP        应用程序文件夹:(里面再分文件夹）
#include "../Entry/main.h"

// APP/LAN        协议解析文件夹:
#include "../../common_os/protocols/SF_Hwf.h"
#include "../../common_OS/protocols/CAN_Hwf.h"
#include "../../common_OS/protocols/Uart_Hwf.h"


#include "../IV_Tasks/BSP.h"
#include "../IV_Tasks/SampleProcess.h"
#include "../IV_Tasks/SafeCheck.h"
#include "../IV_Tasks/Metavariable.h"
#include "../IV_Tasks/ThirdParty.h"
#include "../IV_Tasks/DataManage.h"
#include "../IV_Tasks/SimuBuffer.h"
#include "../IV_Tasks/IVSDL.h"
#include "../IV_Tasks/ScheduleManage.h"
#include "../IV_Tasks/StepManage.h"
#include "../IV_Tasks/CheckLimit_4.h"
#include "../IV_Tasks/CheckLimit_8.h"
#include "../IV_Tasks/ScheduleDataDefine.h"
#include "../IV_Tasks/PID.h"
#include "../IV_Tasks/IR.h"
#include "../IV_Tasks/PowerAdj.h"
//#include "../IV_Tasks/fuzzyPID.h"

#include "../IV_Tasks/ScheduleStatusReport.h"

#include "../IV_Tasks/Pulse.h"
#include "../IV_Tasks/SampleProcess_Pulse.h"
#include "../IV_Tasks/Pulse_LogLimit.h"
#include "../IV_Tasks/Pulse_MV.h"
#include "../IV_Tasks/TEC_Ctrl.h"
#include "../IV_Tasks/FanAdj.h"
// #include "../IV_Tasks/SystemDelay.h"
#include "../IV_Tasks/EXCheck.h"
#include "../IV_Tasks/BurstModeSampling.h"
#include "../IV_Tasks/IndependentVoltageCtrl.h"


// APP/LAN        网络解析程序文件夹:
#include "../../COMMON_APP/EthernetParse/DebugReadMemory.h"
#include "../../COMMON_APP/EthernetParse/EthernetParse.h"
#include "../../COMMON_APP/EthernetParse/EventsBuffering.h"
#include "../../COMMON_APP/EthernetParse/MultiMcuCommunication.h"
#include "../../COMMON_APP/SMBus/SMBus.h"
#include "../../COMMON_OS/Data_Manipulation/BackupBuffering.h"


// OS         操作系统文件夹:
#include "../IV_Tasks/OS_LogicProcess.h"
#include "../../COMMON_OS/Task_Dispatcher/OS_ServeProcess.h"


// HWD        硬件相关文件夹: (里面再分文件夹）
// HWD/HWDLIB      硬件芯片相关驱动文件夹:
#include "../../COMMON_OS/Ext_Devices/CPLD.h"
#include "../../COMMON_OS/Ext_Devices/ControlPin.h"
#include "../../COMMON_OS/Ext_Devices/ADC24bit_CS5560.h"
#include "../../COMMON_OS/Ext_Devices/ADC16bit_AD7656.h"
#include "../../COMMON_OS/Ext_Devices/ADC32bit_LTC2449.h"
#include "../../COMMON_OS/Ext_Devices/ADC32bit_LTC2498.h"
#include "../../COMMON_OS/Ext_Devices/DAC16bit_AD5754.h"
#include "../../COMMON_OS/Ext_Devices/DAC16bit_AD5764.h"
#include "../../COMMON_OS/Ext_Devices/DAC16bit_AD5360.h"
#include "../../COMMON_OS/Ext_Devices/TMP75C.h"
// #include "../../COMMON_OS/Ext_Devices/LM75B.h"
#include "../../COMMON_OS/Ext_Devices/FM25L256.h"
#include "../IV_Tasks/CConfig.h"
#include "../../COMMON_OS/Ext_Devices/ADC16bit_AD7606.h"
#include "../../COMMON_OS/Ext_Devices/ADC24bit_AD7175.h"
#include "../../COMMON_OS/Ext_Devices/LEDFunc.h"
#include "../../COMMON_OS/Ext_Devices/ADC24bit_AD7190.h"    //comment out chen 20130506
#include "../../COMMON_OS/Ext_Devices/DS18B20.h"
#include "../../COMMON_OS/Ext_Devices/PCA9501.h"
#include "../../COMMON_OS/Ext_Devices/ADC24bit_ADS1259.h"
#include "../../COMMON_OS/Ext_Devices/M0Redundant.h"
//#include "../../COMMON_OS/Ext_Devices/DAC16bit_AD5676.h"
//#include "../../COMMON_OS/Ext_Devices/DAC16bit_AD5686.h"



// HWD/CORTEX-M4CORE    文件夹: (预留不使用）
#include "../../COMMON_OS/LPC4078CORE/LPC407x_8x_177x_8x.h"
#include "../../COMMON_OS/LPC4078CORE/system_LPC407x_8x_177x_8x.h"
#include "../../COMMON_OS/LPC4078CORE/Arbin_LPC4078.h"
#include "../../COMMON_OS/LPC4078CORE/GPIO.h"
#include "../../COMMON_OS/LPC4078CORE/IRQ.h"
#include "../../COMMON_OS/LPC4078CORE/EXTINT.h"
#include "../../COMMON_OS/LPC4078CORE/Sdram.h"
#include "../../COMMON_OS/LPC4078CORE/SSPI.h"
#include "../../COMMON_OS/LPC4078CORE/I2CNew.h"
#include "../../COMMON_OS/LPC4078CORE/TIMERS.h"
#include "../../COMMON_OS/LPC4078CORE/WDT.h"
// #include "../../COMMON_OS/LPC4078CORE/ADC.h"
#include "../../COMMON_OS/LPC4078CORE/RTC.h"
#include "../../COMMON_OS/LPC4078CORE/CAN.h"
#include "../../COMMON_OS/LPC4078CORE/PWM.h"
#include "../../COMMON_OS/LPC4078CORE/UART.h"
#include "../../COMMON_OS/Data_Manipulation/Time_Consum.h"
#include "../../COMMON_OS/Data_Manipulation/Time_Count.h"

#include "../../COMMON_OS/Ext_Devices/EEPROM_24LC128.h"
// APP/CAN        CAN解析程序文件夹:
#include "../../COMMON_APP/CANBus/CANBusParse.h"
#include "../../COMMON_APP/CANBus/CANMsgBuffering.h"
#include "../../COMMON_APP/CANBus/CanbusWatchdog.h"
#include "../../COMMON_APP/UartParse/UartParse.h"




//IBT   硬件线IBT板驱动文件夹:   主要用于驱动IBT通道项目,1024通道项目板。
// #include "../HWD/IBT/IBT_ADC5DAC.h"
// #include "../HWD/IBT/IBT_Base.h"
// #include "../HWD/IBT/IBT_SampleManage.h"
//#include "../HWD/IBT/IBT_TempSample.h"
//LBT2401  硬件线LBT2401板驱动文件夹:   主要用于驱动LBT2401通道项目,1通道项目板。
// #include "../HWD/LBT2401/LBT2401_ADC5DAC.h"
// #include "../HWD/LBT2401/LBT2401_Base.h"
// #include "../HWD/LBT2401/LBT2401_SampleManage.h"
//#include "../HWD/LBT2401/LBT2401_TempSample.h"
//LBT2204  硬件线LBT2204板驱动文件夹:   主要用于驱动LBT2204通道项目,4通道项目板。
// #include "../HWD/LBT2204/LBT2204_ADC5DAC.h"
// #include "../HWD/LBT2204/LBT2204_Base.h"
// #include "../HWD/LBT2204/LBT2204_SampleManage.h"
//#include "../HWD/LBT2204/LBT2204_TempSample.h"
//LBT22043  硬件线LBT22043板驱动文件夹:   主要用于驱动LBT22043通道项目,4通道项目板。
// #include "../HWD/LBT22043/LBT22043_ADC5DAC.h"
// #include "../HWD/LBT22043/LBT22043_Base.h"
// #include "../HWD/LBT22043/LBT22043_SampleManage.h"
//#include "../HWD/LBT22043/LBT22043_TempSample.h"
//BT2208
//#include "../HWD/BT2208/BT2208_ADC5DAC.h"
//#include "../HWD/BT2208/BT2208_Base.h"
//#include "../HWD/BT2208/BT2208_SampleManage.h"
//#include "../HWD/BT2208/BT2208_TempSample.h"
#include "../HWD/LBT21XXXBase/LBT21XXX_ADC5DAC.h"
#include "../HWD/LBT21XXXBase/LBT21XXX_Base.h"
#include "../HWD/LBT21XXXBase/LBT21XXX_SampleManage.h"
#include "../HWD/LBT22XXXBase/LBT22XXX_ADC5DAC.h"
#include "../HWD/LBT22XXXBase/LBT22XXX_Base.h"
#include "../HWD/LBT22XXXBase/LBT22XXX_SampleManage.h"
//Tesla 驱动
// #include "../HWD/Tesla/TESLA_ADC5DAC.h"
// #include "../HWD/Tesla/TESLA_Base.h"
// #include "../HWD/Tesla/TESLA_SampleManage.h"
//#include "../HWD/Tesla/TESLA_TempSample.h"
//高压PWM驱动
#include "../HWD/PWM/PWM_ADC5DAC.h"
#include "../HWD/PWM/PWM_Base.h"
#include "../HWD/PWM/PWM_SampleManage.h"
//#include "../HWD/PWM/PWM_TempSample.h"
//自动校准模块驱动
// #include "../hwd/AutoCali/Auto_Base.h"
// #include "../hwd/AutoCali/Auto_ADC5DAC.h"
// #include "../hwd/AutoCali/Auto_SampleManage.h"
//LBT21024  硬件线LBT21024板驱动文件夹:   主要用于驱动LBT21024通道项目,2通道项目板。
#include "../HWD/LBT21024/LBT21024_ADC5DAC.h"
#include "../HWD/LBT21024/LBT21024_Base.h"
#include "../HWD/LBT21024/LBT21024_SampleManage.h"
//#include "../HWD/LBT21024/LBT21024_TempSample.h"
//BT11  硬件线BT11板驱动文件夹:   主要用于驱动BT11通道项目,单通道项目板。
// #include "../HWD/BT11/BT11_ADC5DAC.h"
// #include "../HWD/BT11/BT11_Base.h"
// #include "../HWD/BT11/BT11_SampleManage.h"
//LBT2108  硬件线LBT2108板驱动文件夹:   主要用于驱动LBT2108通道项目,8通道项目板。
#include "../HWD/LBT21084/LBT21084_ADC5DAC.h"
#include "../HWD/LBT21084/LBT21084_Base.h"
#include "../HWD/LBT21084/LBT21084_SampleManage.h"
//#include "../HWD/LBT2108/LBT2108_TempSample.h"
//LBT21164  硬件线LBT21164板驱动文件夹:   主要用于驱动LBT21164通道项目,16通道项目板。
// #include "../HWD/LBT21164/LBT21164_ADC5DAC.h"
// #include "../HWD/LBT21164/LBT21164_Base.h"
// #include "../HWD/LBT21164/LBT21164_SampleManage.h"
//LBT22013  硬件线LBT22013板驱动文件夹:   主要用于驱动LBT22013通道项目,1通道项目板。
#include "../HWD/LBT22013/LBT22013_ADC5DAC.h"
#include "../HWD/LBT22013/LBT22013_Base.h"
#include "../HWD/LBT22013/LBT22013_SampleManage.h"
//LBT20083  硬件线LBT20083板驱动文件夹:   主要用于驱动LBT20083通道项目,16通道项目板。
#include "../HWD/LBT20083/LBT20083_ADC5DAC.h"
#include "../HWD/LBT20083/LBT20083_Base.h"
#include "../HWD/LBT20083/LBT20083_SampleManage.h"
//LBT20083VB  硬件线LBT20083VB板驱动文件夹:   主要用于驱动LBT20083VB通道项目,16通道项目板。
// #include "../HWD/LBT20083VB/LBT20083VB_ADC5DAC.h"
// #include "../HWD/LBT20083VB/LBT20083VB_Base.h"
// #include "../HWD/LBT20083VB/LBT20083VB_SampleManage.h"
//LBT22024  硬件线LBT22024板驱动文件夹:   主要用于驱动LBT22024通道项目,单通道项目板。
#include "../HWD/LBT22023/LBT22023_ADC5DAC.h"
#include "../HWD/LBT22023/LBT22023_Base.h"
#include "../HWD/LBT22023/LBT22023_SampleManage.h"
//#include "../HWD/LBT22024/LBT22024_TempSample.h"
//HPT200  硬件线HPT200板驱动文件夹:   主要用于驱动HPT200通道项目,单通道项目板。
#include "../HWD/HPT200/HPT200_ADC5DAC.h"
#include "../HWD/HPT200/HPT200_Base.h"
#include "../HWD/HPT200/HPT200_SampleManage.h"
//MSTAT21044  硬件线MSTAT21044板驱动文件夹:   主要用于驱动MSTAT21044通道项目,4通道项目板。
#include "../HWD/MSTAT21044/MSTAT21044_ADC5DAC.h"
#include "../HWD/MSTAT21044/MSTAT21044_Base.h"
#include "../HWD/MSTAT21044/MSTAT21044_SampleManage.h"
//#include "../HWD/LBT21044/LBT21044_TempSample.h"
//LBT22013FR  硬件线LBT22013FR板驱动文件夹:   主要用于驱动LBT22013FR通道项目,单通道项目板。
// #include "../HWD/LBT22013FR/LBT22013FR_ADC5DAC.h"
// #include "../HWD/LBT22013FR/LBT22013FR_Base.h"
// #include "../HWD/LBT22013FR/LBT22013FR_SampleManage.h"
//#include "../HWD/LBT22013FR/LBT22013FR_TempSample.h"
//HS21044  硬件线HS21044板驱动文件夹:   主要用于驱动HS21044通道项目,8通道项目板。
#include "../HWD/HS21044/HS21044_ADC5DAC.h"
#include "../HWD/HS21044/HS21044_Base.h"
#include "../HWD/HS21044/HS21044_SampleManage.h"

//LBT22044  硬件线LBT22043板驱动文件夹:   主要用于驱动LBT22043通道项目,4通道项目板。
#include "../HWD/LBT22043/LBT22043_ADC5DAC.h"
#include "../HWD/LBT22043/LBT22043_Base.h"
#include "../HWD/LBT22043/LBT22043_SampleManage.h"
//#include "../HWD/LBT22043/LBT22043_TempSample.h"

//HPM4  硬件线HPM4板驱动文件夹:   主要用于驱动HPM4通道项目,4通道项目板。
#include "../HWD/HPM4/HPM4_ADC5DAC.h"
#include "../HWD/HPM4/HPM4_Base.h"
#include "../HWD/HPM4/HPM4_SampleManage.h"

//LBT2108  硬件线LBT2108板驱动文件夹:   主要用于驱动LBT2108通道项目,8通道项目板。
// #include "../HWD/LBT21084VB/LBT21084VB_ADC5DAC.h"
// #include "../HWD/LBT21084VB/LBT21084VB_Base.h"
// #include "../HWD/LBT21084VB/LBT21084VB_SampleManage.h"
//#include "../HWD/LBT21084VB/LBT21084VB_TempSample.h"

// #include "../HWD/LBT21024FR/LBT21024FR_ADC5DAC.h"
// #include "../HWD/LBT21024FR/LBT21024FR_Base.h"
// #include "../HWD/LBT21024FR/LBT21024FR_SampleManage.h"

//IBT31162  硬件线IBT31162板驱动文件夹:   主要用于驱动IBT31162、IBT2116通道项目,16通道项目板。
// #include "../HWD/IBT31162/IBT31162_ADC5DAC.h"
// #include "../HWD/IBT31162/IBT31162_Base.h"
// #include "../HWD/IBT31162/IBT31162_SampleManage.h"


//Pulse
#include "../IV_Tasks/CheckLimit_Non_V.h"
#include "../IV_Tasks/Check_V_Limit.h"
#include "../IV_Tasks/PULSE_Safety.h"


//LBT21162  硬件线LBT21162板驱动文件夹:   主要用于驱动LBT21162通道项目,16通道项目板。
#include "../HWD/LBT21162/LBT21162_ADC5DAC.h"
#include "../HWD/LBT21162/LBT21162_Base.h"
#include "../HWD/LBT21162/LBT21162_SampleManage.h"

// #include "../HWD/LBT21024FRVB/LBT21024FRVB_ADC5DAC.h"
// #include "../HWD/LBT21024FRVB/LBT21024FRVB_Base.h"
// #include "../HWD/LBT21024FRVB/LBT21024FRVB_SampleManage.h"

#include "../HWD/LBT21014/LBT21014_ADC5DAC.h"
#include "../HWD/LBT21014/LBT21014_Base.h"
#include "../HWD/LBT21014/LBT21014_SampleManage.h"

#include "../HWD/LBT20084/LBT20084_ADC5DAC.h"
#include "../HWD/LBT20084/LBT20084_Base.h"
#include "../HWD/LBT20084/LBT20084_SampleManage.h"
//#include "../HWD/LBT20084/LBT20084_TempSample.h"

//LBT20162  硬件线LBT21162板驱动文件夹:   主要用于驱动LBT20162通道项目,16通道项目板。
#include "../HWD/LBT20162/LBT20162_ADC5DAC.h"
#include "../HWD/LBT20162/LBT20162_Base.h"
#include "../HWD/LBT20162/LBT20162_SampleManage.h"
#include "../HWD/BUILT-IN_16V/BUILT-IN_16V_409386_Base.h"
#include "../HWD/BUILT-IN_16T/BUILT-IN_16T_409920_Base.h"
