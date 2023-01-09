#pragma once

// stb lib
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <float.h>


// COMMON     常用库、公共宏定义文件夹:
#include "../../COMMON_OS/Data_Manipulation/Datatypes.h"
#include "../Entry/G_DF.h"
#include "includes.h"
#include "../../COMMON_OS/Data_Manipulation/CircularQueue.h"
#include "../../COMMON_OS/Data_Manipulation/CircularQueue_Lin.h"
#include "../../COMMON_OS/Data_Manipulation/BitArray.h"
#include "../../COMMON_OS/Data_Manipulation/Lib_Mem.h"
#include "../../common_os/protocols/SF_Hwf.h"
#include "../../COMMON_OS/Data_Manipulation/Time_Count.h"


// APP/MANAGE       AUX管理程序文件夹:
#include "../Aux_Tasks/BSP.h"
#include "../Aux_Tasks/AUXDataDefine.h"
#include "../Aux_Tasks/AUXDatamanager.h"
#include "../Aux_Tasks/AuxSDL.h"
#include "../Aux_Tasks/AUXSampleProcess.h"
#include "../Aux_Tasks/AUXSampleTask.h"
#include "../Aux_Tasks/AUXCheckLimit_4.h"
#include "../Aux_Tasks/AUXCheckLimit_8.h"
#include "../Aux_Tasks/AuxCConfig.h"
#include "../AUX_Tasks/Aux_SafeCheck.h"
#include "../AUX_Tasks/Aux_ThirdParty.h"
#include "../AUX_Tasks/AuxConfigRSN.h"
#include "../AUX_Tasks/Aux_RedundantSample.h"
#include "../AUX_Tasks/AuxReadDI.h"
#include "../AUX_Tasks/AuxWriteDO.h"
#include "../AUX_Tasks/AuxSample_2ndV_LC.h"
#include "../AUX_Tasks/Aux_RedundantSafetyCheck.h"
#include "../AUX_Tasks/AuxMetavariable.h"
#include "../AUX_Tasks/AUX_PID.h"
#include "../AUX_Tasks/AuxACIMTest.h"
#include "../AUX_Tasks/Aux_SysCtrl.h"
#include "../AUX_Tasks/POWERADJ.h"

//AUX					辅助通道
#include "../HWD/Aux_P_TC_509138/Aux_P_TC_509138_Base.h"


#include "../HWD/Aux_2ndV_HIM_511898_WMADC602540/Aux_2ndV_HIM_511898_WMADC602540_Base.h"
#include "../HWD/Aux_2ndV_LC_601310/Aux_2ndV_LC_601310_Base.h"

#include "../HWD/Aux_2ndV_TR_509146/Aux_2ndV_TR_509146_Base.h"

#include "../HWD/Aux_P_TC_509138/Aux_P_TC_509138_Base.h"

#include "../HWD/BatteryCells/BatteryCells_Base.h"
#include "../HWD/BatteryCells/BatteryCells_GPIO.h"
#include "../HWD/BatteryCells/BatteryCells_SampleManage.h"
#include "../HWD/BatteryCells/BatteryCells_ADC5DAC.h"

#include "../HWD/Aux_MultiFunc_514194/Aux_MultiFunc_514194_Base.h"

#include "../HWD/Aux_Redun_2ndV_TC_601082/Aux_Redun_2ndV_TC_601082_Base.h"
// AUX  Temperature  Humidity
#include "../HWD/AUX_THChamber/MTCI_CTRL.H"
#include "../HWD/AUX_THChamber/MTCI_datatypes.h"
#include "../HWD/AUX_THChamber/TH_ESPEC_SCP220.h"
#include "../HWD/AUX_THChamber/TH_ESPEC_GPL_4.h"
#include "../HWD/AUX_THChamber/TC_SUN_EC1X.h"
#include "../HWD/AUX_THChamber/TC_EUROTHERM_2204E.h"
#include "../HWD/AUX_THChamber/TC_EUROTHERM_3216.h"
#include "../HWD/AUX_THChamber/TC_FUTURE_FDCP91.h"
#include "../HWD/AUX_THChamber/TC_KSI_THS2005.h"
#include "../HWD/AUX_THChamber/TC_SAMWONTECH_TEMP880.h"
#include "../HWD/AUX_THChamber/TC_SAMWONTECH_TEMP2500.h"
#include "../HWD/AUX_THChamber/TC_TFS_PC200.h"
#include "../HWD/AUX_THChamber/TC_THERMOTRON_8200.h"
#include "../HWD/AUX_THChamber/TC_WATLOW_F4.h"
#include "../HWD/AUX_THChamber/TH_CSZ_EZT570I.h"
#include "../HWD/AUX_THChamber/TH_CSZ_EZT430I.h"
#include "../HWD/AUX_THChamber/TC_BINDER_MB1.h"
#include "../HWD/AUX_THChamber/TH_HARDING_TPC7062K.h"
#include "../HWD/AUX_THChamber/TH_SAMWONTECH_TEMI2500.h"
#include "../HWD/AUX_THChamber/TH_VOETSCH.h"
#include "../HWD/AUX_THChamber/TH_VOETSCH_VC_L4006.h"
#include "../HWD/AUX_THChamber/TH_SIEMENS_S7_226CN.h"
#include "../HWD/AUX_THChamber/TH_CLCD_9700.h"
#include "../HWD/AUX_THChamber/TH_SHANGHAI_PINDUN_TEMI880.h"
#include "../HWD/AUX_THChamber/TH_TT_5166.h"
#include "../HWD/AUX_THChamber/TH_GUANGZHOU_WUSUO.h"
#include "../HWD/AUX_THChamber/TH_DONGGUAN_HUANCE_UMC1200.h"
#include "../HWD/AUX_THChamber/TH_CHONGQING_YINHE_MODBUSTCP.h"
#include "../HWD/AUX_THChamber/TC_HARDINGS7_HLT403P-II.h"
#include "../HWD/AUX_THChamber/TH_HARDINGS7_HLT504P.h"
#include "../HWD/AUX_THChamber/TC_JULABO_FP50MA_F4.h"
#include "../HWD/AUX_THChamber/TH_CHONGQING_ATAIHE.h"
#include "../HWD/AUX_THChamber/TH_JUFU_9700S.h"
#include "../HWD/AUX_THChamber/TC_JULABO_A45T.h"
#include "../HWD/AUX_THChamber/TC_TM221CE24T.h"
#include "../HWD/AUX_THChamber/TH_US_9256P_ACCU3.h"
#include "../HWD/AUX_THChamber/TH_SIEMENS_S7_SAIA.h"
#include "../HWD/AUX_THChamber/TH_9700S_2C.h"
#include "../HWD/AUX_THChamber/TH_SIEMENS_S7_XC1001A_SAIA.h"
#include "../HWD/AUX_THChamber/TC_LAUDA_PRO_P2E.h"
#include "../HWD/AUX_THChamber/TH_HITACHI_EC15MHP.h"
#include "../HWD/AUX_THChamber/TC_TEMI2500_MODBUS2.h"
#include "../HWD/AUX_THChamber/TH_VOETSCH_NETWORK.h"
#include "../HWD/AUX_THChamber/TC_HARDINGS7_HLT504P.h"
#include "../HWD/AUX_THChamber/TH_MemmertGmbH_RS232.h"
#include "../HWD/AUX_THChamber/TC_MT8071_MODBUS.h"
#include "../HWD/AUX_THChamber/TC_ESPEC_SCP220.h"
#include "../HWD/AUX_THChamber/TC_ESPEC_GPL_4.h"
#include "../HWD/AUX_THChamber/TC_BINDER_MB2.h"
#include "../HWD/AUX_THChamber/TC_U740.h"
#include "../HWD/AUX_THChamber/TC_SIMPAC.h"
#include "../HWD/AUX_THChamber/TH_TT_5166_Sunwoda.h"
#include "../HWD/AUX_THChamber/TH_SONGHUA_2PT_BD.h"
#include "../HWD/AUX_THChamber/TC_9700S_2C.h"
#include "../HWD/AUX_THChamber/TC_DONGDA_MEWTO.h"
#include "../HWD/AUX_THChamber/TC_SIEMENS_S7_XC1001A_SAIA.h"
#include "../HWD/AUX_THChamber/TH_MT8071_MODBUS.h"
#include "../HWD/AUX_THChamber/TC_BIA_CLIMATIC.h"
#include "../HWD/AUX_THChamber/TC_SigmaSystemsC4.h"
#include "../HWD/AUX_THChamber/TC_WATLOW_F4T_MODBUS_TCP.h"
#include "../HWD/AUX_THChamber/TH_HONGRUIDA_MODBUS_TCP.h"
#include "../HWD/AUX_THChamber/TH_SPI2_P1.h"
#include "../HWD/AUX_THChamber/TH_ZUNDAR_AZ1000.h"
#include "../HWD/AUX_THChamber/TC_PolyScience.h"
#include "../HWD/AUX_THChamber/TH_PLC_700_ASCII.h"
#include "../HWD/AUX_THChamber/TC_HARDINGS7_V0_45A1-II.h"
#include "../HWD/AUX_THChamber/TC_HARDING_THREE_LAYERS_MODBUS_TCP.h"
#include "../HWD/AUX_THChamber/TH_SIMENSE_S7_200.h"
#include "../HWD/AUX_THChamber/TH_HONGRUIDA_MODBUS_RS485.h"
#include "../HWD/AUX_THChamber/TC_MT8071_MODBUS_DOUBLE_LAYER.h"
#include "../HWD/AUX_THChamber/TH_GT9700S.h"
#include "../HWD/AUX_THChamber/TC_WATLOW_F4T_MODBUS_FLOAT.h"
#include "../HWD/AUX_THChamber/TC_H_OYO1000.h"

#include "../HWD/Aux_ExtCharge_601618/Aux_ExtCharge_601618_Base.h"
#include "../HWD/Aux_Monitor_601618/Aux_Monitor_601618_Base.h"
#include "../HWD/Aux_TR_PT100_601314/Aux_TR_PT100_601314_Base.h"
#include "../HWD/Aux_DIDO_601196_WMADC602518/Aux_DIDO_601196_WMADC602518_Base.h"
#include "../HWD/Aux_OnlineRSM_602026/Aux_OnlineRSM_602026_Base.h"
#include "../HWD/Aux_AIO_DIO_602178_WMADC602548/Aux_AIO_DIO_602178_WMADC602548_Base.h"
#include "../HWD/Aux_TC_MAD602280_WMADC602524/Aux_TC_MAD602280_WMADC602524_Base.h"
#include "../HWD/Aux_TC_HS_MADC602176_WMADC602484/Aux_TC_HS_MADC602176_WMADC602484_Base.h"
#include "../HWD/Aux_WMADC_PT100_602532/Aux_WMADC_PT100_602532_Base.h"
#include "../HWD/Aux_WMADC_2ndV_TR_602536/Aux_WMADC_2ndV_TR_602536_Base.h"
#include "../HWD/Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130/Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base.h"
#include "../HWD/Aux_DDS_ACIM_ACR_408888/Aux_DDS_ACIM_ACR_408888_Base.h"
#include "../HWD/Aux_DDS_ACIM_ACR_408888/Aux_DDS_ACIM_ACR_408888_SampleManage.h"
#include "../HWD/Aux_PowerADJ_200A_412206/Aux_PowerADJ_200A_412206_Base.h"
#include "../HWD/Aux_PowerADJ_2US6K_422108/Aux_PowerADJ_2US6K_422108_Base.h"
//=========Added by ZC 09/29/2016===============
#include "../HWD/Aux_Chamber_TEC/Aux_Chamber_TEC_Base.h"
#include "../HWD/Aux_Chamber_TEC/TMP_KEYFun.h"
#include "../HWD/Aux_Chamber_TEC/HT16K33.h"
#include "../HWD/Aux_Chamber_TEC/TMP75_TEC.h"

//========Added by dirui 08/06/2018
#include "../HWD/Aux_SystemCtrlBoard_415016/Aux_SystemCtrlBoard_415016_Base.h"
//=================End==================
//=======================Cold Plate by  ZC 04.04.2018==========================
#include "../HWD/Aux_MTC/Aux_MTC_Base.h"
//===================================End=======================================

//--------()通信部分-Begin-------------------------------------------------
// APP/CAN        CAN解析程序文件夹:
#include "../../common_OS/protocols/CAN_Hwf.h"
#include "../../common_OS/protocols/Uart_Hwf.h"


// APP/LAN        网络解析程序文件夹:
//#include "../../common_os/protocols/SF_Hwf.h"
#include "../../COMMON_APP/EthernetParse/EthernetParse.h"
#include "../../COMMON_APP/EthernetParse/EventsBuffering.h"
#include "../../COMMON_APP/EthernetParse/MultiMcuCommunication.h"
#include "../../COMMON_APP/EthernetParse/DebugReadMemory.h"
//--------()通信部分-End-------------------------------------------------


//--------()MCU内核部分-Begin-------------------------------------------------
// HWD/CORTEX-M4CORE    文件夹: (预留不使用）
#include "../../COMMON_os/LPC4078CORE/LPC407x_8x_177x_8x.h"
#include "../../COMMON_os/LPC4078CORE/system_LPC407x_8x_177x_8x.h"
#include "../../COMMON_os/LPC4078CORE/Arbin_LPC4078.h"
#include "../../COMMON_os/LPC4078CORE/GPIO.h"
#include "../../COMMON_os/LPC4078CORE/IRQ.h"
#include "../../COMMON_os/LPC4078CORE/EXTINT.h"
#include "../../COMMON_os/LPC4078CORE/Sdram.h"
#include "../../COMMON_os/LPC4078CORE/SSPI.h"
#include "../../COMMON_os/LPC4078CORE/I2CNew.h"
#include "../../COMMON_os/LPC4078CORE/TIMERS.h"
#include "../../COMMON_os/LPC4078CORE/WDT.h"
#include "../../COMMON_os/LPC4078CORE/UART.h"
#include "../../COMMON_os/LPC4078CORE/RTC.h"
#include "../../COMMON_os/LPC4078CORE/CAN.h"
#include "../../COMMON_os/Data_Manipulation/Time_Consum.h"
#include "../../COMMON_OS/LPC4078CORE/ADC.h"
#include "../../COMMON_APP/CANBus/CanbusWatchdog.h"
#include "../../COMMON_APP/CANBus/CANBusParse.h"
#include "../../COMMON_APP/CANBus/CANMsgBuffering.h"
#include "../../COMMON_APP/UartParse/Aux_UartParse.h"
#include "../../COMMON_os/LPC4078CORE/PWM.h" //Added by ZC 09/29/2016
#include "../../COMMON_APP/UartParse/DWIN_UartParse.h"  //Added by ZC 04/05/2018

// HWD        硬件相关文件夹: (里面再分文件夹）
// HWD/HWDLIB      硬件芯片相关驱动文件夹:
#include "../../COMMON_OS/Ext_Devices/CPLD.h"
#include "../../COMMON_os/Ext_Devices/ControlPin.h"
#include "../../COMMON_os/Ext_Devices/ADC32bit_LTC2449.h"
#include "../../COMMON_os/Ext_Devices/ADC32bit_LTC2498.h"
#include "../../COMMON_OS/Ext_Devices/DAC16bit_AD5754.h"
#include "../../COMMON_os/Ext_Devices/ADC24bit_ADS1259.h"
#include "../../COMMON_os/Ext_Devices/FM25L256.h"
#include "../../COMMON_os/Ext_Devices/EEPROM_24LC128.h"
#include "../../COMMON_os/Ext_Devices/LEDFunc.h"
#include "../../COMMON_os/Ext_Devices/EEPROM_24LC128.h"
#include "../../COMMON_os/Ext_Devices/ADC16bit_LTC68042.h"
#include "../../COMMON_os/Ext_Devices/ADC24bit_AD7175.h"
#include "../../COMMON_os/Ext_Devices/DDS_AD9834.h"
#include "../../COMMON_os/Ext_Devices/ADC101C027.h"
#include "../../COMMON_os/Ext_Devices/DAC6571.h"
//--------()操作部分-Begin-------------------------------------------------
// OS         操作系统文件夹:
#include "../../COMMON_os/Task_Dispatcher/OS_ServeProcess.h"
// APP        应用程序文件夹:(里面再分文件夹）
#include "../Entry/main.h"
//--------()操作部分-End-------------------------------------------------



