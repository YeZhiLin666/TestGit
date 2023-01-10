/*****************************************************************************
// 程序文件      :main.cpp     主要用于主程序循环
// 文件描述      :为MCU board服务
// 编写日期      :2012.07.11
*****************************************************************************/

#include "includes.h"

/*
#define extern
// This statment will disable the "extern" declaration in tcpip.h
// So all the variables will be defined only in main.cpp module
// and other modules only have external declaration.
*/

void IVboardtype_Init(void)
{
//   static const St_Port Pin_SCK0= {0,        15,        eCPTYPE_MCUGPIO,   SSP0_SCK,              0};;
//   static const St_Port Pin_MISO0= {0,        17,        eCPTYPE_MCUGPIO,   SSP0_MISO,             0};
//   static const St_Port Pin_MOSI0= {0,        18,        eCPTYPE_MCUGPIO,   SSP0_MOSI,             0};
//
//   ControlPin::SetIObyCP(Pin_MISO0);
//   ControlPin::SetIObyCP(Pin_MOSI0);
//   ControlPin::SetIObyCP(Pin_SCK0);
    FM25L256Func::MyInit();
    CConfig::ReadContent_ForFRAM((unsigned char *)&ChannelInfo_Manager::m_st_BoadType, FRAM_IV_BOARDTYPE, sizeof(IV_BOARDTYPE_CFG));//nfig是一切初始化的前提
    CConfig::IV_BoardType_OldTransformToNew();
//  board_config::IVBoardType = ChannelInfo_Manager::m_st_BoadType.m_u16BoardType;
//  board_config::IVBoardTypeversion = ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion;

    if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType  == BOARDTYPE_HS21044 )//202004291
        TIMERSFunc::Init_Timer(TIME_INTERVAL_200US,0);     //Timer0 init    //200us 中断一次,因为单通道，不用中断驱动采样
    else
        TIMERSFunc::Init_Timer(TIME_INTERVAL_50US,0);     //Timer0 init    //50us 中断一次

#if THREAD_TASK_TIME_STATISTIC == 1
    TIMERSFunc::Init_Timer(TIMER_COUNT_REGISER_MAX - 1,1);//与IR冲突
#endif
}

void Hardware_Init(void)
{
    //unsigned short BoardType;

    IVboardtype_Init();
    if(ChannelInfo_Manager::m_st_BoadType.m_btBUILTIN == 1)
    {
        CConfig::Load_I2C_Config();
    }
    //ChannelInfo_Manager::BoardCFG.m_Global.m_u16BoardType = board_config::IVBoardType; // _BOARD_TYPE_;

    UChar8 myIP = 0;
    myIP = board_config::getBID();

// 	if(myIP == 99)
//		myIP = 98;	// 当前unit数远小于100，如果读出的拨码大于99，我们认为是拨码错误，强行将IP写为196.168.1.100，使其与上位机冲突以提醒使用者

    if(myIP >99)
        myIP = 99;	// 当前unit数远小于100，如果读出的拨码大于99，我们认为是拨码错误，强行将IP写为196.168.1.100，使其与上位机冲突以提醒使用者


    EthernetParse::SetRemoteIP(100);          //  PC: 196.168.1.100
    EthernetParse::SetLocalIP5MAC(myIP+1);    //  MCUIP: 196.168.1.X and MCUMAC: 00.00.00.00.00.X
    CANFunc::SourceBoardID = myIP + 1;
    Smart_Battery::m_SMBUnitID=myIP;
    ChannelInfo_Manager::InitUnitInfo(myIP);
    CPLD_FUNC::Init();
    CConfig::MyInit();       //这个初始化比较特殊，load config是一切初始化的前提
    //Built_in CANBMS  Start//
    CANFunc::InitMyCAN();
    CANFunc::CAN_ChangeACCF();//根据读取的config情况重新对CAN端口滤波
    //Built_in CANBMS  End//
    ChannelInfo_Manager::Init_BoardCFG();
    memset(&board_config::Feature_Enable_init,0,sizeof(St_Hardware_Feature_Enable));  // 初始化为0 add by qjm20171129
    //ChannelInfo_Manager::BoardCFG.m_Global.m_u16BoardType = board_config::IVBoardType;

    switch (ChannelInfo_Manager::m_st_BoadType.m_u16BoardType)
    {
//   case BOARDTYPE_IBT:
//     IBT_Base::Hardware_Init();
//     break;
//   case BOARDTYPE_TESLA_PWM:
//     TESLA_Base::Hardware_Init();
//     break;
    case BOARDTYPE_PWM:
        PWM_Base::Hardware_Init();
        break;
//   case BOARDTYPE_AUTO_CALI:
//     break;
//   case BOARDTYPE_BT2401:
//     LBT2401_Base::Hardware_Init();
//     break;
//  case BOARDTYPE_BT2208:
    //  BT2208_Base::Hardware_Init();
//    break;
//   case BOARDTYPE_BT2204:
//     LBT2204_Base::Hardware_Init();
//     break;
    case BOARDTYPE_LBT21024:
        LBT21024_Base::FeatureEnable_Init();
        LBT21024_Base::Hardware_Init();
        break;
//  case BOARDTYPE_BT11:
//    BT11_Base::Hardware_Init();
//    break;
    case BOARDTYPE_LBT21084:
    case BOARDTYPE_LBT21044:
        LBT21084_Base::FeatureEnable_Init();
        LBT21084_Base::Hardware_Init();
        break;
//   case BOARDTYPE_LBT22043:
//     LBT22043_Base::Hardware_Init();
//     break;
    //   case BOARDTYPE_LBT21164:
    //     LBT21164_Base::Hardware_Init();
    //     break;
    case BOARDTYPE_LBT22013:
        LBT22013_Base::Hardware_Init();
        break;
    /*  case BOARDTYPE_LBT20083:
           LBT20083_Base::Hardware_Init();
           break;*/
    case BOARDTYPE_LBT22023:
        LBT22023_Base::FeatureEnable_Init();
        LBT22023_Base::Hardware_Init();
        break;
    /*  case BOARDTYPE_HPT200:
        HPT200_Base::Hardware_Init();   //  04.27.2018 ZC
        break;*/
    case BOARDTYPE_MSTAT21044:
        MSTAT21044_Base::FeatureEnable_Init();
        MSTAT21044_Base::Hardware_Init();
        break;
//   case BOARDTYPE_LBT22013FR:
//     LBT22013FR_Base::Hardware_Init();
//     break;
    case BOARDTYPE_HS21044:
        HS21044_Base::Hardware_Init();
        break;
    case BOARDTYPE_LBT22043:
        LBT22043_Base::FeatureEnable_Init();
        LBT22043_Base::Hardware_Init();
        break;
    /*  case BOARDTYPE_HPM4:
           HPM4_Base::Hardware_Init();
           break;*/
//   case BOARDTYPE_LBT21084VB:
//     LBT21084VB_Base::Hardware_Init();
//     break;
//   case BOARDTYPE_LBT21024FR:
//     LBT21024FR_Base::Hardware_Init();
//     break;
//   case BOARDTYPE_IBT31162:
// 	IBT31162_Base::Hardware_Init();
// 	break;
    case BOARDTYPE_LBT21162:
        LBT21162_Base::FeatureEnable_Init();
        LBT21162_Base::Hardware_Init();
        break;
// 	case BOARDTYPE_LBT20083VB:
// 		LBT20083VB_Base::Hardware_Init();
// 		break;
// 	case BOARDTYPE_LBT21024FRVB:
//     LBT21024FRVB_Base::Hardware_Init();
//     break;
    case BOARDTYPE_LBT21014:
        LBT21014_Base::FeatureEnable_Init();
        LBT21014_Base::Hardware_Init();
        break;
    case BOARDTYPE_LBT20084:
        LBT20084_Base::FeatureEnable_Init();
        LBT20084_Base::Hardware_Init();
        break;
    case BOARDTYPE_LBT20162:
        LBT20162_Base::FeatureEnable_Init();
        LBT20162_Base::Hardware_Init();
        break;
    //and so on...
    default:
        break;
    }
    TIMERSFunc::DELAY_SYS( 10000, 0 );	//delay 5ms

//   if((ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_LBT21084) || (ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_LBT21044)|| (ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_MSTAT21044)||(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_LBT20084))
//   {
//     ACR::Init();
//     board_config::ACR_En(0,0);
//   }
    if(ChannelInfo_Manager::m_st_BoadType.m_btBUILTIN == 1)
    {
//       CConfig::Load_I2C_Config();
// 		  CConfig::BuiltIn_Board_Init();
        BuiltIn_DataManager::BuiltIn_IO_Init();
        BuiltIn_DataManager::BuiltIn_HardwareInit();


    }
}
void Software_Init(void)
{
    ChannelInfo_Manager::Init();    // 大数据体初始化保护要先于硬件的软件初始化。 修复冗余电压上下限保护  yy 20160215

    switch (ChannelInfo_Manager::m_st_BoadType.m_u16BoardType)
    {
//   case BOARDTYPE_IBT:
//     IBT_Base::Software_Init();
//     break;
//   case BOARDTYPE_TESLA_PWM:
//     TESLA_Base::Software_Init();
//     break;
    case BOARDTYPE_PWM:
        PWM_Base::Software_Init();
        break;
//   case BOARDTYPE_AUTO_CALI:
//     //Auto_Cali_Base::Software_Init();
//     break;
//  case BOARDTYPE_BT2208:
    //    BT2208_Base::Software_Init();
//    break;
//   case BOARDTYPE_BT2204:
//     LBT2204_Base::Software_Init();
//     break;
//   case BOARDTYPE_BT2401:
//     LBT2401_Base::Software_Init();
//     break;
    //   case BOARDTYPE_LELOAD2501:
    //     LELOAD2501_Base::Software_Init();
    //     break;
    case BOARDTYPE_LBT21024:
        LBT21024_Base::Software_Init();
        break;
//  case BOARDTYPE_BT11:
//    BT11_Base::Software_Init();
//    break;
    case BOARDTYPE_LBT21084:
    case BOARDTYPE_LBT21044:
        LBT21084_Base::Software_Init();
        break;
//   case BOARDTYPE_LBT22043:
//     LBT22043_Base::Software_Init();
//     break;
    //   case BOARDTYPE_LBT21164:
    //     LBT21164_Base::Software_Init();
    //     break;
    case BOARDTYPE_LBT22013:
        LBT22013_Base::Software_Init();
        break;
    /*  case BOARDTYPE_LBT20083:
    		LBT20083_Base::Software_Init();
    		break;*/
    case BOARDTYPE_LBT22023:
        LBT22023_Base::Software_Init();
        break;
    /*  case BOARDTYPE_HPT200:
        HPT200_Base::Software_Init();    //  04.27.2018 ZC
        break;*/
    case BOARDTYPE_MSTAT21044:
        MSTAT21044_Base::Software_Init();
        break;
//   case BOARDTYPE_LBT22013FR:
//     LBT22013FR_Base::Software_Init();
//     break;
    case BOARDTYPE_HS21044:
        HS21044_Base::Software_Init();
        break;
    case BOARDTYPE_LBT22043:
        LBT22043_Base::Software_Init();
        break;
    /*  case BOARDTYPE_HPM4:
    		HPM4_Base::Software_Init();
    		break;*/
//   case BOARDTYPE_LBT21084VB:
//     LBT21084VB_Base::Software_Init();
//     break;
//     case BOARDTYPE_LBT21024FR:
//     LBT21024FR_Base::Software_Init();
//     break;
//   case BOARDTYPE_IBT31162:
//         IBT31162_Base::Software_Init();
// 		break;
    case BOARDTYPE_LBT21162:
        LBT21162_Base::Software_Init();
        break;
// 	case BOARDTYPE_LBT20083VB:
// 		LBT20083VB_Base::Software_Init();
// 		break;
//   case BOARDTYPE_LBT21024FRVB:
//     LBT21024FRVB_Base::Software_Init();
//     break;
    case BOARDTYPE_LBT21014:
        LBT21014_Base::Software_Init();
        break;
    case BOARDTYPE_LBT20084:
        LBT20084_Base::Software_Init();
        break;
    case BOARDTYPE_LBT20162:
        LBT20162_Base::Software_Init();
        break;
    default:
        break;
    }

    if(board_config::Accuracy <= 0)
        board_config::Accuracy = 0.0005f;

    //   ////////////////////////////////OS////////////////////////////////
    OS_ServeProcess::OS_Init();//for OS
    //创建任务仓库
    //                            任务执行函数                     任务初始化函数                  任务优先级                          任务ID
    OS_ServeProcess::OSCreateTask(&EthernetParse::DoWork,          &EthernetParse::MyInit,         EthernetParse::Priority,            EthernetParse::TaskID);  // 网络
    OS_ServeProcess::OSCreateTask(&Sample_Pro::DoWork,             &Sample_Pro::Init,              Sample_Pro::Priority,               Sample_Pro::TaskID);    //采样任务
    OS_ServeProcess::OSCreateTask(&thirdParty::DoWork,             &thirdParty::Init,              thirdParty::Priority,               thirdParty::TaskID);    //第三方
    OS_ServeProcess::OSCreateTask(&Step_Manager_task::DoWork,      &Step_Manager_task::MyInit,     Step_Manager_task::Priority,        Step_Manager_task::TaskID);  //step task
    OS_ServeProcess::OSCreateTask(&Check_Limit4_task::DoWork,      &Check_Limit4_task::MyInit,     Check_Limit4_task::Priority,        Check_Limit4_task::TaskID);  //CheckLimit 4 task
    //  OS_ServeProcess::OSCreateTask(&Sample_Pro::DoWork,             &Sample_Pro::Init,              Sample_Pro::Priority,               Sample_Pro::TaskID);    //采样任务
    OS_ServeProcess::OSCreateTask(&Schedule_Manager_task::DoWork,  &Schedule_Manager_task::MyInit, Schedule_Manager_task::Priority,    Schedule_Manager_task::TaskID);   //schedule task
//  OS_ServeProcess::OSCreateTask(&IVSDL::DoWork,                  &IVSDL::Init,                   IVSDL::Priority,                    IVSDL::TaskID);  //180524 zyx
    OS_ServeProcess::OSCreateTask(&EventsBuffering::DoWork,        &EventsBuffering::MyInit,       EventsBuffering::Priority,          EventsBuffering::TaskID);      // 事件缓存和重发
    OS_ServeProcess::OSCreateTask(&CANBusParse::DoWork,            &CANBusParse::Init,             CANBusParse::Priority,              CANBusParse::TaskID);  //
//  OS_ServeProcess::OSCreateTask(&CANMsgBuffering::DoWork,        &CANMsgBuffering::MyInit,       CANMsgBuffering::Priority,          CANMsgBuffering::TaskID);
    OS_ServeProcess::OSCreateTask(&OS_LogicProcess::DoWork,        &OS_LogicProcess::Init,         OS_LogicProcess::Priority,          OS_LogicProcess::TaskID);    //comment out chen 20130817
    OS_ServeProcess::OSCreateTask(&safety_check::DoWork,           &safety_check::Init,            safety_check::Priority,             safety_check::TaskID);
    OS_ServeProcess::OSCreateTask(&Check_Limit8_task::DoWork,      &Check_Limit8_task::MyInit,     Check_Limit8_task::Priority,        Check_Limit8_task::TaskID);
    OS_ServeProcess::OSCreateTask(&MetaVariable_Task::DoWork,      &MetaVariable_Task::Init,       MetaVariable_Task::Priority,        MetaVariable_Task::TaskID);  //MV task comment out chen 20130415
    OS_ServeProcess::OSCreateTask(&LEDFunc::DoWork,                &LEDFunc::Init,                 LEDFunc::Priority,                  LEDFunc::TaskID);
//   OS_ServeProcess::OSCreateTask(&SystemDelayTask::DoWork,        &SystemDelayTask::MyInit,       SystemDelayTask::Priority,          SystemDelayTask::TaskID);
    OS_ServeProcess::OSCreateTask(&CConfig::DoWork,                &CConfig::MyInit,               CConfig::Priority,                  CConfig::TaskID);  //Fram task


    if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_LBT22013
            || ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_LBT22023
            || ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_LBT22043)
    {
        OS_ServeProcess::OSCreateTask(&Smart_Battery::DoWork,      &Smart_Battery::MyInit,       Smart_Battery::Priority,        Smart_Battery::TaskID);

    }
    OS_ServeProcess::OSCreateTask(&CheckLimit_Non_V_task::DoWork,  &CheckLimit_Non_V_task::MyInit, CheckLimit_Non_V_task::Priority,    CheckLimit_Non_V_task::TaskID);
    OS_ServeProcess::OSCreateTask(&Check_V_Limit_task::DoWork,     &Check_V_Limit_task::MyInit,    Check_V_Limit_task::Priority,       Check_V_Limit_task::TaskID);
    OS_ServeProcess::OSCreateTask(&Pulse_LogLimit_task::DoWork,    &Pulse_LogLimit_task::MyInit,   Pulse_LogLimit_task::Priority,      Pulse_LogLimit_task::TaskID);
    OS_ServeProcess::OSCreateTask(&PULSE_MV_Task::DoWork,          &PULSE_MV_Task::Init,           PULSE_MV_Task::Priority,            PULSE_MV_Task::TaskID);
    OS_ServeProcess::OSCreateTask(&PULSE_safety_check::DoWork,     &PULSE_safety_check::Init,      PULSE_safety_check::Priority,       PULSE_safety_check::TaskID);
    OS_ServeProcess::OSCreateTask(&SampleProcess_Pulse::DoWork,    &SampleProcess_Pulse::Init,     SampleProcess_Pulse::Priority,      SampleProcess_Pulse::TaskID);
//  OS_ServeProcess::OSCreateTask(&TEC_Ctrl::DoWork,               &TEC_Ctrl::Init,                TEC_Ctrl::Priority,                 TEC_Ctrl::TaskID); //TEC 控制
    OS_ServeProcess::OSCreateTask(&FanAdj_task::DoWork,               &FanAdj_task::MyInit,                FanAdj_task::Priority,                 FanAdj_task::TaskID); //fan控制
    OS_ServeProcess::OSCreateTask(&CAN_WatchdogTask::DoWork,       &CAN_WatchdogTask::MyInit,      CAN_WatchdogTask::Priority,         CAN_WatchdogTask::TaskID);
    OS_ServeProcess::OSCreateTask(&IndependentVoltageCtrl_task::DoWork,               &IndependentVoltageCtrl_task::MyInit,                IndependentVoltageCtrl_task::Priority,                 IndependentVoltageCtrl_task::TaskID);
    OS_ServeProcess::OSCreateTask(&MultiMcuComm::DoWork,              &MultiMcuComm::MyInit,             MultiMcuComm::Priority,                MultiMcuComm::TaskID);
    OS_ServeProcess::OSCreateTask(&HS21044_SampleManage::DoWork,   &HS21044_SampleManage::Init,    HS21044_SampleManage::Priority,     HS21044_SampleManage::TaskID);  //BT11 sample task  //Add by DKQ 08.20.2014
    OS_ServeProcess::OSCreateTask(&PowerAdj::DoWork,               &PowerAdj::MyInit,                PowerAdj::Priority,                 PowerAdj::TaskID); //功率调解控制

    if(board_config::Feature_Enable_init.RedundantVoltMode == REDUNDANT_M0)
        OS_ServeProcess::OSCreateTask(&UartParse::DoWork,              &UartParse::MyInit,               UartParse::Priority,                  UartParse::TaskID);  //Fram task
    if(board_config::Feature_Enable_init.Power_Off_Enable==1)
        OS_ServeProcess::OSCreateTask(&PowerLostCheck::DoWork,         &PowerLostCheck::Init,          PowerLostCheck::Priority,           PowerLostCheck::TaskID );   //chen 20131016 add
    //将任务安装到任务盘，并获得在线任务ID，如果需要的话
    //                         任务ID                           指定任务优先级                      是否初始化
    OS_ServeProcess::MountTask(EthernetParse::TaskID,      EthernetParse::Priority,            1);
    OS_ServeProcess::OTID_4_Reg_SamplePro =
        OS_ServeProcess::MountTask(Sample_Pro::TaskID,      Sample_Pro::Priority,               1);
    OS_ServeProcess::MountTask(thirdParty::TaskID,        thirdParty::Priority,               1);
    OS_ServeProcess::MountTask(Step_Manager_task::TaskID,       Step_Manager_task::Priority,        1);
    OS_ServeProcess::OTID_4_Reg_CheckLimit4 =
        OS_ServeProcess::MountTask(Check_Limit4_task::TaskID,   Check_Limit4_task::Priority,        1);
    OS_ServeProcess::MountTask(Schedule_Manager_task::TaskID,   Schedule_Manager_task::Priority,    1);
// OS_ServeProcess::MountTask(IVSDL::TaskID,                   IVSDL::Priority,                    1);  //180524 zyx
    OS_ServeProcess::MountTask(EventsBuffering::TaskID,         EventsBuffering::Priority,          1);
    OS_ServeProcess::MountTask(CANBusParse::TaskID,             CANBusParse::Priority,              1);
//  OS_ServeProcess::MountTask(CANMsgBuffering::TaskID,         CANMsgBuffering::Priority,          1);
    OS_ServeProcess::MountTask(OS_LogicProcess::TaskID,         OS_LogicProcess::Priority,          1);
    OS_ServeProcess::OTID_4_Reg_SafetyCheck =
        OS_ServeProcess::MountTask(safety_check::TaskID,        safety_check::Priority,             1);
    OS_ServeProcess::OTID_4_Reg_CheckLimit8 =
        OS_ServeProcess::MountTask(Check_Limit8_task::TaskID,   Check_Limit8_task::Priority,        1);
    OS_ServeProcess::OTID_4_Reg_MetaVariable =
        OS_ServeProcess::MountTask(MetaVariable_Task::TaskID,  MetaVariable_Task::Priority,        1);
    OS_ServeProcess::MountTask(LEDFunc::TaskID,                 LEDFunc::Priority,                  1);
//   OS_ServeProcess::MountTask(SystemDelayTask::TaskID,         SystemDelayTask::Priority,          1);
    OS_ServeProcess::MountTask(FanAdj_task::TaskID,             FanAdj_task::Priority,                  1);
    OS_ServeProcess::MountTask(CAN_WatchdogTask::TaskID,      CAN_WatchdogTask::Priority,                  1);
    OS_ServeProcess::MountTask(IndependentVoltageCtrl_task::TaskID,      IndependentVoltageCtrl_task::Priority,                  1);

    if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_LBT22013
            || ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_LBT22023
            || ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_LBT22043)
    {
        OS_ServeProcess::MountTask(Smart_Battery::TaskID,      Smart_Battery::Priority,                  1);
    }
    OS_ServeProcess::MountTask(CConfig::TaskID,               CConfig::Priority,                  0);
    OS_ServeProcess::MountTask(MultiMcuComm::TaskID,      MultiMcuComm::Priority,            1);
    OS_ServeProcess::MountTask(PowerAdj::TaskID,             PowerAdj::Priority,                  1);

    if(board_config::Feature_Enable_init.RedundantVoltMode == REDUNDANT_M0)
        OS_ServeProcess::MountTask(UartParse::TaskID,               UartParse::Priority,                  0);

    if(board_config::Feature_Enable_init.Power_Off_Enable==1)
        OS_ServeProcess::MountTask(PowerLostCheck::TaskID,          PowerLostCheck::Priority,           1);

    if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_HS21044)
        OS_ServeProcess::MountTask(HS21044_SampleManage::TaskID,HS21044_SampleManage::Priority,        0);
    /*  if ( ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_HPT200 || ChannelInfo_Manager::m_st_BoadType.m_u16BoardType == BOARDTYPE_HPM4)           // 03.28.2017 for HPM4
            OS_ServeProcess::MountTask(TEC_Ctrl::TaskID,                TEC_Ctrl::Priority,                 1 );*/

    //20150518
    //---------------------------
    ////////////////////////////////others////////////////////////////////
    TIMERSFunc::Timer1Handler = &IR::ProcessIRTimer1;
    TIMERSFunc::Timer3Handler = &Pulse::PulseISR3;
}


int main(void)
{
		//第二次修改
    OS_ServeProcess::Init_Device_Firmware();      //初始化片内外设

    //外设及软件初始化
    board_config::DacInfo.m_fPulseRiseTimeInMS = 10.0f;
    Hardware_Init();    //初始化片外外设
    Software_Init();    //用于软件的初始化
    OS_ServeProcess::Init_NetWork();

#if MCU_WATCHDOG_ENABLE == 1
    WDTFunc::Watchdog_Init();     //Add by DKQ 06_17_2014
    WDTFunc::Watchdog_Feed();     //Add by DKQ 06_17_2014
#endif
    M0RedundantFunc::SetM0ConfigInfo();
    while(1)
    {
        OS_ServeProcess::OSAuthorize();
    }
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/

