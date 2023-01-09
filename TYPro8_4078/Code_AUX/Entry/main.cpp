/*****************************************************************************
// 程序文件      :main.cpp     主要用于主程序循环
// 文件描述      :为MCU board服务
// 编写日期      :2012.07.11
*****************************************************************************/

#include "includes.h"

const St_Port AUX_Unit_Base::Pin_SCK0      = {0,        15,        eCPTYPE_MCUGPIO,   SSP0_SCK,              0};
const St_Port AUX_Unit_Base::Pin_MISO0     = {0,        17,        eCPTYPE_MCUGPIO,   SSP0_MISO,             0};
const St_Port AUX_Unit_Base::Pin_MOSI0     = {0,        18,        eCPTYPE_MCUGPIO,   SSP0_MOSI,             0};

const St_Port AUX_Unit_Base::Pin_SCK1      = {0,        7,         eCPTYPE_MCUGPIO,   SSP1_SCK,              0};
const St_Port AUX_Unit_Base::Pin_MISO1     = {0,        8,         eCPTYPE_MCUGPIO,   SSP1_MISO,             0};
const St_Port AUX_Unit_Base::Pin_MOSI1     = {0,        9,         eCPTYPE_MCUGPIO,   SSP1_MOSI,             0};

const St_Port AUX_Unit_Base::Pin_ECODE1		= {1,        18,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port AUX_Unit_Base::Pin_ECODE2		= {1,        19,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port AUX_Unit_Base::Pin_ECODE3		= {1,        20,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port AUX_Unit_Base::Pin_EOE		  = {1,        21,        eCPTYPE_MCUGPIO,   GPIO_O,                0};
const St_Port AUX_Unit_Base::Pin_ADCBUSY 	= {2,        12,        eCPTYPE_MCUGPIO,   GPIO_I,                0};
bool AUX_Unit_Base::m_EnableISRsample = false;
SSPI_Para st_Aux_FRAM_Para;
bool Be_SystemCtrlBoard=false;

//unsigned char Aux_getBID(void);



void Hardware_Init(unsigned char Aux_BoardID)
{
    switch(Aux_DataManager::m_AllAuxBoards[Aux_BoardID].m_BoardInfo.m_abi_globe.m_BoardType)   //这个结构应有说改变
    {
    case Aux_2ndV_TR_509146:
        Aux_2ndV_TR_509146_Base::Hardware_Init(Aux_BoardID);
        break;
    case Aux_P_TC_509130:
        Aux_P_TC_509138_Base::Hardware_Init(Aux_BoardID);
        break;
    case Aux_pH_TC_507074:
        // 					   Aux_Ph_TC_507074_Base::Hardware_Init(Aux_BoardID);
        break;
    case Aux_2ndV_HIR_511898:
    case Aux_WMADC_2ndV_HIR_602540:
	case Aux_WMADC_2ndV_HIR_441518:
        Aux_2ndV_HIR_511898_WMADC602540_Base::Hardware_Init(Aux_BoardID);
        break;
    case  Aux_2ndV_LC_601310:
        Aux_2ndV_LC_601310_Base::Hardware_Init(Aux_BoardID);
        break;

    case Aux_MultiFunc_514194:
    case Aux_WMultiFunc_415088:
		case Aux_WMultiFunc_430938:
	case Aux_WMultiFunc_441518:
        Aux_MultiFunc_514194_Base::Hardware_Init(Aux_BoardID);
        break;
    case Aux_Redun_2ndV_TC_601082:
        Aux_Redun_2ndV_TC_601082_Base::Hardware_Init(Aux_BoardID);
        break;
    case Aux_2ndV_BS_601440:
        BatteryCells_Base::Hardware_Init(Aux_BoardID);
        break;
    case Aux_ExtCharge_601618:
        Aux_ExtCharge_601618_Base::Hardware_Init(Aux_BoardID);
        break;
    case Aux_Monitor_601618:
        Aux_Monitor_601618_Base::Hardware_Init(Aux_BoardID);
        break;
    case Aux_TR_PT100_601314:
    case Aux_TEC_Chamber_601932:   //Add by DKQ 09_29_2016
        Aux_TR_PT100_601314_Base::Hardware_Init(Aux_BoardID);
        break;
    case Aux_DIDO_601196:
    case Aux_WMADC_DIDO_602518:
    case Aux_WMADC_DIDO_416714:
        Aux_DIDO_601196_WMADC602518_Base::Hardware_Init(Aux_BoardID);
        break;
    case Aux_OnlineRSM_602026:
        Aux_OnlineRSM_602026_Base::Hardware_Init(Aux_BoardID);
        break;
    case Aux_AIO_DIO_602178:
    case Aux_WMADC_AIO_DIO_602548:
        Aux_AIO_DIO_602178_WMADC602548_Base::Hardware_Init(Aux_BoardID);
        break;
    case Aux_WMADC_TC_415088:
        Adc32bit_LTC2498::IsExCrystalsFlag=1;   //该板型使用外部晶振
    case Aux_MADC_TC_602280:
    case Aux_WMADC_TC_602524:
        Aux_TC_MADC602280_WMADC602524_Base::Hardware_Init(Aux_BoardID);
        break;
    case Aux_MADC_TC_HS_602176:
    case Aux_WMADC_TC_HS_602484:
        Aux_TC_HS_MADC602176_WMADC602484_Base::Hardware_Init(Aux_BoardID);
        break;
    case Aux_WMADC_PT100_602532:
    case Aux_WMADC_PT100_603616:
    case Aux_WMADC_PT100_416772:
	  case Aux_WMADC_PT100_443948:
        Aux_WMADC_PT100_602532_Base::Hardware_Init(Aux_BoardID);
        break;
    case Aux_WMADC_2ndV_TR_602536:
        Aux_WMADC_2ndV_TR_602536_Base::Hardware_Init(Aux_BoardID);
        break;
    case Aux_WMADC_2ndV_LC_602528:
    case Aux_WMADC_2ndVM_LC_603072:
    case Aux_WMADC_2ndVL_LC_603130:
	  case Aux_WMADC_2ndV_VL_430834:
        Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::Hardware_Init(Aux_BoardID);
        break;
    case Aux_DDS_ACIM_ACR_408888:
    case Aux_DDS_ACIM_ACR_603612:
    case Aux_DDS_ACIM_415466:
		case Aux_DDS_Sinwave_Current_Source_604700:
        Aux_DDS_ACIM_ACR_408888_Base::Hardware_Init(Aux_BoardID);
        break;
//	#if MCU_BE_SYSTEMCTRLBOARD==1
// 	case Aux_SYSCTRLBOARD_415016:
// 			 Be_SystemCtrlBoard=true;
// 		   Aux_SystemCtrlBoard_415016_Base::Hardware_Init(Aux_BoardID);
// 		  break;
        //#endif
// 	#if MCU_BE_SYSTEMCTRLBOARD==1
    case Aux_Sys_Control_PWM_416326:
    case Aux_Sys_Control_Liner_416326:
        Be_SystemCtrlBoard=true;
        Aux_SystemCtrlBoard_415016_Base::Hardware_Init(Aux_BoardID);
        break;
    case Aux_MTC_Control_Board_415132:  //Add for Cold Plate by ZC 04/05/2018
		Aux_MTC_Base::Aux_MTC_Version = 1;  //Add for New 2nd version MZTC by ZC 12/013/2019
        Aux_MTC_Base::Hardware_Init(Aux_BoardID);
        break;
    case Aux_MZTC_Control_Board_417118:  //Add for New 2nd version MZTC by ZC 12/013/2019
		case Aux_MZTC_Control_Board_441324:
		case Aux_MZTC_Control_Board_444136:
		Aux_MTC_Base::Aux_MTC_Version = 2;
        Aux_MTC_Base::Hardware_Init(Aux_BoardID);
        break;
    case Aux_PowerADJ_200A_412206:
        Aux_PowerADJ_200A_412206_Base::Hardware_Init( Aux_BoardID);

		    break;
		case Aux_PowerADJ_2US6K_422108:
        Aux_PowerADJ_2US6K_422108_Base::Hardware_Init( Aux_BoardID);
        break;
// 	#endif
        //…//其他类型
    }
}


void IO_Init(void)
{
    ControlPin::SetIObyCP(AUX_Unit_Base::Pin_SCK0);
    ControlPin::SetIObyCP(AUX_Unit_Base::Pin_MISO0);
    ControlPin::SetIObyCP(AUX_Unit_Base::Pin_MOSI0);   //初始化SPI0端口

    ControlPin::SetIObyCP(AUX_Unit_Base::Pin_MISO1);
    ControlPin::SetIObyCP(AUX_Unit_Base::Pin_MOSI1);
    ControlPin::SetIObyCP(AUX_Unit_Base::Pin_SCK1);   //初始化SPI1端口

    ControlPin::SetIObyCP(AUX_Unit_Base::Pin_EOE);
    ControlPin::SetIObyCP(AUX_Unit_Base::Pin_ECODE1);
    ControlPin::SetIObyCP(AUX_Unit_Base::Pin_ECODE2);
    ControlPin::SetIObyCP(AUX_Unit_Base::Pin_ECODE3);
    ControlPin::SetIObyCP(AUX_Unit_Base::Pin_ADCBUSY); //初始化片选端口

    Aux_DataManager::SettingCS_High();
}
void Software_Init(unsigned char Aux_BoardID)
{
    switch(Aux_DataManager::m_AllAuxBoards[Aux_BoardID].m_BoardInfo.m_abi_globe.m_BoardType)
    {
    case Aux_2ndV_TR_509146:
        Aux_2ndV_TR_509146_Base::Software_Init(Aux_BoardID);
        break;
    case Aux_P_TC_509130:
        Aux_P_TC_509138_Base::Software_Init(Aux_BoardID);
        break;
    case Aux_pH_TC_507074:
        // 					   Aux_Ph_TC_507074::Software_Init(Aux_BoardID);
        break;
    case Aux_2ndV_HIR_511898:
    case Aux_WMADC_2ndV_HIR_602540:
	case Aux_WMADC_2ndV_HIR_441518:
        Aux_2ndV_HIR_511898_WMADC602540_Base::Software_Init(Aux_BoardID);
        break;
    case  Aux_2ndV_LC_601310:
        Aux_2ndV_LC_601310_Base::Software_Init(Aux_BoardID);
        break;

    case Aux_MultiFunc_514194:
    case Aux_WMultiFunc_415088:
		case Aux_WMultiFunc_430938:
	case Aux_WMultiFunc_441518:
        Aux_MultiFunc_514194_Base::Software_Init(Aux_BoardID);
        break;
    case Aux_Redun_2ndV_TC_601082:
        Aux_Redun_2ndV_TC_601082_Base::Software_Init(Aux_BoardID);
        break;
    case Aux_2ndV_BS_601440:
        BatteryCells_Base::Software_Init(Aux_BoardID);
        break;
    case Aux_ExtCharge_601618:
        Aux_ExtCharge_601618_Base::Software_Init(Aux_BoardID);
        break;
    case Aux_Monitor_601618:
        Aux_Monitor_601618_Base::Software_Init(Aux_BoardID);
        break;
    case Aux_TR_PT100_601314:
    case Aux_TEC_Chamber_601932:    //Add by DKQ 09_29_2016
        Aux_TR_PT100_601314_Base::Software_Init(Aux_BoardID);
        break;
    case Aux_DIDO_601196:
    case Aux_WMADC_DIDO_602518:
    case Aux_WMADC_DIDO_416714:
        Aux_DIDO_601196_WMADC602518_Base::Software_Init(Aux_BoardID);
        break;
    case Aux_OnlineRSM_602026:
        Aux_OnlineRSM_602026_Base::Software_Init(Aux_BoardID);
        break;
    case Aux_AIO_DIO_602178:
    case Aux_WMADC_AIO_DIO_602548:
        Aux_AIO_DIO_602178_WMADC602548_Base::Software_Init(Aux_BoardID);
        break;
    case Aux_MADC_TC_602280:
    case Aux_WMADC_TC_602524:
    case Aux_WMADC_TC_415088:
        Aux_TC_MADC602280_WMADC602524_Base::Software_Init(Aux_BoardID);
        break;
    case Aux_MADC_TC_HS_602176:
    case Aux_WMADC_TC_HS_602484:
        Aux_TC_HS_MADC602176_WMADC602484_Base::Software_Init(Aux_BoardID);
        break;
    case Aux_WMADC_PT100_602532:
    case Aux_WMADC_PT100_603616:
    case Aux_WMADC_PT100_416772:
	case Aux_WMADC_PT100_443948:
        Aux_WMADC_PT100_602532_Base::Software_Init(Aux_BoardID);
        break;
    case Aux_WMADC_2ndV_TR_602536:
        Aux_WMADC_2ndV_TR_602536_Base::Software_Init(Aux_BoardID);
        break;
    case Aux_WMADC_2ndV_LC_602528:
    case Aux_WMADC_2ndVM_LC_603072:
    case Aux_WMADC_2ndVL_LC_603130:
		case Aux_WMADC_2ndV_VL_430834:
        Aux_WMADC_2ndV_LC_602528_WMADC_603072_WMADC_603130_Base::Software_Init(Aux_BoardID);
        break;
    case Aux_DDS_ACIM_ACR_408888:
    case Aux_DDS_ACIM_ACR_603612:
    case Aux_DDS_ACIM_415466:
		case Aux_DDS_Sinwave_Current_Source_604700:
        Aux_DDS_ACIM_ACR_408888_Base::Software_Init(Aux_BoardID);
        break;
//	#if MCU_BE_SYSTEMCTRLBOARD==1
// 	case Aux_SYSCTRLBOARD_415016:
// 		   Aux_SystemCtrlBoard_415016_Base::Software_Init(Aux_BoardID);
// 		  break;
        //#endif
// 	#if MCU_BE_SYSTEMCTRLBOARD==1
    case Aux_Sys_Control_PWM_416326:
    case Aux_Sys_Control_Liner_416326:
        Aux_SystemCtrlBoard_415016_Base::Software_Init(Aux_BoardID);
        break;
    case Aux_MTC_Control_Board_415132:  //Add for Cold Plate by ZC 04/05/2018
		Aux_MTC_Base::Aux_MTC_Version = 1;  //Add for New 2nd version MZTC by ZC 12/013/2019
        Aux_MTC_Base::Software_Init(Aux_BoardID);
        break;
    case Aux_MZTC_Control_Board_417118:  //Add for New 2nd version MZTC by ZC 12/013/2019
		case Aux_MZTC_Control_Board_441324:
	case Aux_MZTC_Control_Board_444136:
		Aux_MTC_Base::Aux_MTC_Version = 2;
        Aux_MTC_Base::Software_Init(Aux_BoardID);
        break;
    case Aux_PowerADJ_200A_412206:
        Aux_PowerADJ_200A_412206_Base::Software_Init(Aux_BoardID);
        break;
		case Aux_PowerADJ_2US6K_422108:
        Aux_PowerADJ_2US6K_422108_Base::Software_Init( Aux_BoardID);
        break;
// 	#endif
        //…//其他类型
    }

}


void PrepareForInitAUX(void)     //准备初始化辅助通道操作
{
    CommonFunc::uc_Support_AUX = 1;
    IO_Init();  //Add by DKQ 05.04.2015   设置SPI0， SPI1，端口。 设置片选端口，设置ADC busy端口。
    //这些端口对于说有的板来说都是相同的，因此只需在这里设置即可。
    st_Aux_FRAM_Para.uc_Module_Name = SSP0_DEVICE1;
    st_Aux_FRAM_Para.uc_Speed = 2;
    st_Aux_FRAM_Para.uc_CPOL = 0;
    st_Aux_FRAM_Para.uc_CPHA = 0;
    st_Aux_FRAM_Para.uc_MSTR = 1;    //master
    st_Aux_FRAM_Para.uc_BITS = 8;
    st_Aux_FRAM_Para.uc_LSBF = 0;
    SSPIFunc::setPara(&st_Aux_FRAM_Para,0);
    SSPIFunc::my_Init(0);              //初始化  FRAM
    Aux_DataManager::Init(); //初始化Aux_DataManger里用到的变量，读出配置信息并初始化参数，这里直接调用

    memset(&Aux_DataManager::m_AuxBoardInfo4EEPROM,0,sizeof(Aux_DataManager::m_AuxBoardInfo4EEPROM));

    Aux_DataManager::Aux_ADC_TotalCnt=0;
    for(unsigned char uc_i=0; uc_i<MAX_BOARD_PER_AUXMCU; uc_i++)
    {
        if(Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_valid == BEUSED)
        {
            Hardware_Init(uc_i);    //初始化片外外设
            Software_Init(uc_i);    //用于软件的初始化
            if((Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_globe.m_BoardType !=Aux_2ndV_BS_601440 )&& \
                    (Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_globe.m_BoardType !=Aux_2ndV_LC_601310 )&& \
                    (Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_globe.m_BoardType !=Aux_WMADC_2ndV_LC_602528 )&&\
                    (Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_globe.m_BoardType !=Aux_WMADC_2ndVM_LC_603072)&&\
                    (Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_globe.m_BoardType !=Aux_WMADC_2ndVL_LC_603130) && \
						        (Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_globe.m_BoardType !=Aux_WMADC_2ndV_VL_430834) && \
                    (Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_globe.m_BoardType != Aux_DDS_ACIM_ACR_408888) && \
                    (Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_globe.m_BoardType !=Aux_DDS_ACIM_ACR_603612) &&\
                    (Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_globe.m_BoardType !=Aux_DDS_ACIM_415466)&&\
										(Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_globe.m_BoardType !=Aux_DDS_Sinwave_Current_Source_604700)&&\
										(Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_globe.m_BoardType !=Aux_PowerADJ_2US6K_422108)
							)

                Aux_DataManager::Aux_ADC_TotalCnt+= Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_globe.m_Total_ADC;
            else
            {
                if((Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_globe.m_BoardType == Aux_2ndV_LC_601310)||												
                        (Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_globe.m_BoardType == Aux_WMADC_2ndV_LC_602528)||\
                        (Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_globe.m_BoardType == Aux_WMADC_2ndVM_LC_603072)||\
                        (Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_globe.m_BoardType == Aux_WMADC_2ndVL_LC_603130)||\
								        (Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_globe.m_BoardType == Aux_WMADC_2ndV_VL_430834))
                {
                    if(Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_globe.m_BoardType == Aux_WMADC_2ndVL_LC_603130)
										{
											Aux_Sample_2ndV_LC::Sample_Wait_Count = 0;
                    }
										else if(Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_globe.m_BoardType == Aux_WMADC_2ndV_VL_430834)
										{
                        Aux_Sample_2ndV_LC::Sample_Wait_Count = 3;											  
										}
                    Aux_DataManager::m_Enable_2ndV_LC_Sample = true;
                    AUX_Unit_Base::m_EnableISRsample = true;
                }

                if(Aux_DataManager::m_AllAuxBoards[uc_i].m_BoardInfo.m_abi_globe.m_BoardType == Aux_2ndV_BS_601440)
                {
                    Aux_DataManager::m_Enable_2ndV_BC_Sample = true;
                    AUX_Unit_Base::m_EnableISRsample = false;
                }
            }
        }
    }
    Aux_DataManager::Aux_BuildLongSnake();
    Aux_DataManager::Aux_ConfirmContainAuxTypes();//辅助限制
    Aux_DataManager::Aux_TranslateMapFromPC();  // add by DKQ 2015/5/18
    for(unsigned char i=0; i<16; i++) //最多16个片选地址
    {
        if(Aux_DataManager::Aux_ADC_CS_Info[i].m_Valid==1)
        {
            OS_ServeProcess::AUX_ADC_Idx=i;   //初始化辅助ADC片选首地址  防止只有一块板，波码不是0
            break;
        }
        else
        {
            continue;
        }
    }


    if(Aux_DataManager::Aux_ADC_TotalCnt!=0)
        AUX_Unit_Base::m_EnableISRsample = true; // only if there is a ADC, then enable ISR sample
	
	#if THREAD_TASK_TIME_STATISTIC == 1
	TIMERSFunc::Init_Timer(TIMER_COUNT_REGISER_MAX - 1,1);
	#endif

}

void  OSCreateTaskForInit(void)     //准备初始化辅助通道操作
{
    UChar8 myIP = 0;
    myIP = Aux_getBID();     //20131209

    EthernetParse::SetRemoteIP(100);          //  PC: 196.168.1.100
    EthernetParse::SetLocalIP5MAC(myIP+1);    //  MCUIP: 196.168.1.X and MCUMAC: 00.00.00.00.00.X
    CANFunc::SourceBoardID = myIP + 1;

    OS_ServeProcess::OS_Init();//for OS

    // 	////////////////////////////////OS////////////////////////////////



    // 	//创建任务
    //                            任务执行函数                     任务初始化函数                  任务优先级                          任务ID
    OS_ServeProcess::OSCreateTask(&EthernetParse::DoWork,          &EthernetParse::MyInit,         EthernetParse::Priority,            EthernetParse::TaskID);  // 网络
    OS_ServeProcess::OSCreateTask(&AuxSDL::DoWork,                 &AuxSDL::Init,                  AuxSDL::Priority,                   AuxSDL::TaskID);
    OS_ServeProcess::OSCreateTask(&AUX_SampleProcess::DoWork,      &AUX_SampleProcess::Init,       PRIORITY4,                          AUXSAMPLE_TASK);
    OS_ServeProcess::OSCreateTask(&AUX_Sample_4_task::DoWork,      &AUX_Sample_4_task::Init,       PRIORITY4,                          AUX_SAMPLE_4_TASK);
    OS_ServeProcess::OSCreateTask(&Aux_Safety_Check::DoWork,       &Aux_Safety_Check::Init,        PRIORITY4,                          SAFETYCHECK_TASK);    //Aux safety check task
    OS_ServeProcess::OSCreateTask(&EventsBuffering::DoWork,        &EventsBuffering::MyInit,       EventsBuffering::Priority,          EventsBuffering::TaskID);      // 事件缓存和重发
    OS_ServeProcess::OSCreateTask(&CANBusParse::DoWork,            &CANBusParse::Init,             CANBusParse::Priority,              CANBusParse::TaskID);  //
    OS_ServeProcess::OSCreateTask(&AuxCheckLimit4_task::DoWork,    &AuxCheckLimit4_task::MyInit,   AuxCheckLimit4_task::Priority,      AuxCheckLimit4_task::TaskID);
//============================================DWIN GDGUS LCD SCREEN ADD FOR COLD PLATE BY ZC 04/05/2018========================
    OS_ServeProcess::OSCreateTask(&DWIN_UartParse::DoWork,            &DWIN_UartParse::Init,             DWIN_UartParse::Priority,              DWIN_UartParse::TaskID);
    OS_ServeProcess::OSCreateTask(&Aux_MTC_Base::DoWork,		  	&Aux_MTC_Base::Init,  			Aux_MTC_Base::Priority,           Aux_MTC_Base::TaskID);
//===============================================================================================================
    OS_ServeProcess::OSCreateTask(&AuxUartParse::DoWork,            &AuxUartParse::Init,             AuxUartParse::Priority,              AuxUartParse::TaskID);  //
    OS_ServeProcess::OSCreateTask(&AuxRedundantSafetyCheck::DoWork,    &AuxRedundantSafetyCheck::Init,   AuxRedundantSafetyCheck::Priority,      AuxRedundantSafetyCheck::TaskID);
    //  #if SUPPORT_CAN_BMS==0
    OS_ServeProcess::OSCreateTask(&AuxConfigRSN::DoWork,           &AuxConfigRSN::Init,            AuxConfigRSN::Priority,             AuxConfigRSN::TaskID);
    // #endif
    OS_ServeProcess::OSCreateTask(&AuxCheckLimit8_task::DoWork,    &AuxCheckLimit8_task::MyInit,   AuxCheckLimit8_task::Priority,      AuxCheckLimit8_task::TaskID);
    OS_ServeProcess::OSCreateTask(&I2CBusNew::DoWork,              &I2CBusNew::TaskInit,           I2CBusNew::Priority,                I2CBusNew::TaskID);  //
    OS_ServeProcess::OSCreateTask(&LEDFunc::DoWork,                &LEDFunc::Init,                 LEDFunc::Priority,                  LEDFunc::TaskID);
    OS_ServeProcess::OSCreateTask(&Aux_ReadDI::DoWork,                 &Aux_ReadDI::Init,                  Aux_ReadDI::Priority,                 Aux_ReadDI::TaskID);  //Read DI 的任务
    OS_ServeProcess::OSCreateTask(&Aux_WriteDO::DoWork,               &Aux_WriteDO::Init,                  Aux_WriteDO::Priority,                 Aux_WriteDO::TaskID);  //Write DO 的任务
    OS_ServeProcess::OSCreateTask(&MTCI_CTRL::DoWork,               	&MTCI_CTRL::MyInit,                  MTCI_CTRL::Priority,                 MTCI_CTRL::TaskID);  //Uart 的任务
    OS_ServeProcess::OSCreateTask(&MetaVariable_Task::DoWork,      &MetaVariable_Task::Init,       MetaVariable_Task::Priority,        MetaVariable_Task::TaskID);  //MV task comment out chen 20130415
    OS_ServeProcess::OSCreateTask(&CAN_WatchdogTask::DoWork,               &CAN_WatchdogTask::MyInit,                CAN_WatchdogTask::Priority,                 CAN_WatchdogTask::TaskID);
    OS_ServeProcess::OSCreateTask(&RedundantSample_Task::DoWork,		 	&RedundantSample_Task::Init,  	RedundantSample_Task::Priority,           RedundantSample_Task::TaskID);
    OS_ServeProcess::OSCreateTask(&Aux_Sample_2ndV_LC::DoWork,		  	&Aux_Sample_2ndV_LC::Init,  			Aux_Sample_2ndV_LC::Priority,           Aux_Sample_2ndV_LC::TaskID);
    OS_ServeProcess::OSCreateTask(&AuxPID_task::DoWork,		  	&AuxPID_task::MyInit,  			AuxPID_task::Priority,           AuxPID_task::TaskID);
    OS_ServeProcess::OSCreateTask(&Aux_Chamber_TEC_Base::KEY_DoWork,		  	&Aux_Chamber_TEC_Base::KEY_Init,  			Aux_Chamber_TEC_Base::KEY_Priority,           Aux_Chamber_TEC_Base::KEY_TaskID); //TEC Chamber ??????? Added by C.Zhong 03_15_2016
    OS_ServeProcess::OSCreateTask(&Aux_Chamber_TEC_Base::Chamber_TEC_DoWork,		  	&Aux_Chamber_TEC_Base::Init,  			Aux_Chamber_TEC_Base::Chamber_TEC_Priority,           Aux_Chamber_TEC_Base::Chamber_TEC_TaskID); //TEC PWM Control?? Added by C.Zhong 03_15_2016
    OS_ServeProcess::OSCreateTask(&Aux_ACIMTest::DoWork,                 &Aux_ACIMTest::Init,                  Aux_ACIMTest::Priority,                 Aux_ACIMTest::TaskID);  //ACIM  的任务
    OS_ServeProcess::OSCreateTask(&MultiMcuComm::DoWork,              &MultiMcuComm::MyInit,             MultiMcuComm::Priority,                MultiMcuComm::TaskID);
//#if MCU_BE_SYSTEMCTRLBOARD==1
    if(Be_SystemCtrlBoard)
        OS_ServeProcess::OSCreateTask(&AuxSysCtrl_task::DoWork,              &AuxSysCtrl_task::MyInit,             AuxSysCtrl_task::Priority,                AuxSysCtrl_task::TaskID);
    //#endif
    OS_ServeProcess::OSCreateTask(&POWERADJ::DoWork,                &POWERADJ::Init,                 POWERADJ::Priority,                  POWERADJ::TaskID);

    //                         任务ID                            指定任务优先级                      是否初始化
    OS_ServeProcess::MountTask(EthernetParse::TaskID,            EthernetParse::Priority,            1);
    OS_ServeProcess::MountTask(AuxSDL::TaskID,                   AuxSDL::Priority,                   1);
    OS_ServeProcess::MountTask(AUX_SampleProcess::TaskID,        AUX_SampleProcess::Priority,        1);
    //  OS_ServeProcess::MountTask(AUX_Sample_4_task::TaskID,    AUX_Sample_4_task::Priority,        1);
    OS_ServeProcess::MountTask(Aux_Safety_Check::TaskID,         Aux_Safety_Check::Priority,         1);
    OS_ServeProcess::MountTask(EventsBuffering::TaskID,         EventsBuffering::Priority,          1);
    OS_ServeProcess::MountTask(CANBusParse::TaskID,              CANBusParse::Priority,              1);
    OS_ServeProcess::MountTask(AuxCheckLimit4_task::TaskID,      AuxCheckLimit4_task::Priority,      1);
    OS_ServeProcess::MountTask(AuxConfigRSN::TaskID,             AuxConfigRSN::Priority,             1);
    OS_ServeProcess::MountTask(AuxCheckLimit8_task::TaskID,      AuxCheckLimit8_task::Priority,      1);
    OS_ServeProcess::MountTask(I2CBusNew::TaskID,                I2CBusNew::Priority,                1);
    OS_ServeProcess::MountTask(LEDFunc::TaskID,                  LEDFunc::Priority,                  1);
    OS_ServeProcess::MountTask(CAN_WatchdogTask::TaskID,         CAN_WatchdogTask::Priority,                  1);
    OS_ServeProcess::MountTask(MultiMcuComm::TaskID,      MultiMcuComm::Priority,            1);
    //#if MCU_BE_SYSTEMCTRLBOARD==1
    if(Be_SystemCtrlBoard)
        OS_ServeProcess::MountTask(AuxSysCtrl_task::TaskID,      AuxSysCtrl_task::Priority,            1);
    //#endif
    //  以下任务根据实际版型选择性装载。

    SelectiveMountTask();
    //===============END==================================
}
void SelectiveMountTask(void)
{
    unsigned char b_id = 0;
    if (Aux_DataManager::m_Enable_2ndV_BC_Sample)   // If there is Battery Cell Voltage Board, mount its sample task
    {
        if(!OS_ServeProcess::IsMounted(AUX_Sample_4_task::TaskID))
            OS_ServeProcess::MountTask(AUX_Sample_4_task::TaskID,        AUX_Sample_4_task::Priority,        1);

        // Not going to do any of the rest of the tasks
        //	return;
    }


    //DI
    for(b_id = 0; b_id<8; b_id++)
    {
        if(Aux_DataManager::m_AllAuxBoards[b_id].m_BoardInfo.m_abi_globe.m_Total_DI !=0)
            break;
        else
            continue;
    }
    if(b_id<8)
        if(!OS_ServeProcess::IsMounted(Aux_ReadDI::TaskID))
            OS_ServeProcess::MountTask(Aux_ReadDI::TaskID,				Aux_ReadDI::Priority,							1);

    //DO
    for(b_id = 0; b_id<8; b_id++)
    {
        if((Aux_DataManager::m_AllAuxBoards[b_id].m_BoardInfo.m_abi_globe.m_Total_DO !=0) )
            break;
        else
            continue;
    }
    if(b_id<8)
        if(!OS_ServeProcess::IsMounted(Aux_WriteDO::TaskID))
            OS_ServeProcess::MountTask(Aux_WriteDO::TaskID,				Aux_WriteDO::Priority,						1);

    //温控箱
    for(b_id = 0; b_id<8; b_id++)
    {
        if((Aux_DataManager::m_AllAuxBoards[b_id].m_BoardInfo.m_abi_globe.m_Total_UART ==0) \
                || ((Aux_DataManager::m_AllAuxBoards[b_id].m_BoardInfo.m_abi_globe.m_BoardType != Aux_MultiFunc_514194) \
                    &&(Aux_DataManager::m_AllAuxBoards[b_id].m_BoardInfo.m_abi_globe.m_BoardType != Aux_WMultiFunc_415088)
										&&(Aux_DataManager::m_AllAuxBoards[b_id].m_BoardInfo.m_abi_globe.m_BoardType != Aux_WMultiFunc_430938)
										&&(Aux_DataManager::m_AllAuxBoards[b_id].m_BoardInfo.m_abi_globe.m_BoardType != Aux_WMultiFunc_441518)))
            continue;
        else
            break;
    }
    if(b_id<8)
        if(!OS_ServeProcess::IsMounted(MTCI_CTRL::TaskID))
            OS_ServeProcess::MountTask(MTCI_CTRL::TaskID,				MTCI_CTRL::Priority,							1);

    //冗余保护
    for(b_id = 0; b_id<8; b_id++)
    {
        if(Aux_DataManager::m_AllAuxBoards[b_id].m_BoardInfo.m_abi_globe.m_BoardType != Aux_Redun_2ndV_TC_601082)
            continue;
        else
            break;
    }
    if(b_id<8)
    {
        if(!OS_ServeProcess::IsMounted(AuxUartParse::TaskID))
            OS_ServeProcess::MountTask(AuxUartParse::TaskID,               AuxUartParse::Priority,									1);
        if(!OS_ServeProcess::IsMounted(AuxRedundantSafetyCheck::TaskID))
            OS_ServeProcess::MountTask(AuxRedundantSafetyCheck::TaskID,		AuxRedundantSafetyCheck::Priority,			1);
        if(!OS_ServeProcess::IsMounted(RedundantSample_Task::TaskID))
            OS_ServeProcess::MountTask(RedundantSample_Task::TaskID,       RedundantSample_Task::Priority,					1);
    }
    //启动META 任务，进行电量容量计算

    MetaVariable_Task::C_E_Flag = 0;
    for(b_id = 0; b_id<8; b_id++)
    {
        if(Aux_DataManager::m_AllAuxBoards[b_id].m_BoardInfo.m_abi_globe.m_BoardType == Aux_ExtCharge_601618)
            MetaVariable_Task::C_E_Flag |= 0x01<<b_id;
    }
    if(!OS_ServeProcess::IsMounted(MetaVariable_Task::TaskID))
        OS_ServeProcess::MountTask(MetaVariable_Task::TaskID,       MetaVariable_Task::Priority,        1);
    //AO
    for(b_id = 0; b_id<8; b_id++)
    {
        if((Aux_DataManager::m_AllAuxBoards[b_id].m_BoardInfo.m_abi_globe.m_Total_DAC !=0) )
            break;
        else
            continue;
    }
    if(b_id<8)
        if(!OS_ServeProcess::IsMounted(AuxPID_task::TaskID))
            OS_ServeProcess::MountTask(AuxPID_task::TaskID,				AuxPID_task::Priority,						1);

    //=============Added by ZC 09/29/2016=================
    //If use TEC CHAMBER
    if(Aux_Chamber_TEC_Base::Aux_Chamber_TEC_Board)
    {
        if(!OS_ServeProcess::IsMounted(Aux_Chamber_TEC_Base::KEY_TaskID))
            OS_ServeProcess::MountTask(Aux_Chamber_TEC_Base::KEY_TaskID,          Aux_Chamber_TEC_Base::KEY_Priority,          1);
        if(!OS_ServeProcess::IsMounted(Aux_Chamber_TEC_Base::Chamber_TEC_TaskID))
            OS_ServeProcess::MountTask(Aux_Chamber_TEC_Base::Chamber_TEC_TaskID,    Aux_Chamber_TEC_Base::Chamber_TEC_Priority,  1);

    }

    //Aux_ACR
    for(b_id = 0; b_id<8; b_id++)
    {
        if((Aux_DataManager::m_AllAuxBoards[b_id].m_BoardInfo.m_abi_globe.m_BoardType !=Aux_DDS_ACIM_ACR_408888) && \
                (Aux_DataManager::m_AllAuxBoards[b_id].m_BoardInfo.m_abi_globe.m_BoardType !=Aux_DDS_ACIM_ACR_603612))
            continue;
        else
            break;
    }
    if(b_id<8)
    {
        TIMERSFunc::Init_Timer(TIME_INTERVAL_25US ,3);    //Timer3 init    //25us 中断一次
        if(!OS_ServeProcess::IsMounted(Aux_ACIMTest::TaskID))
        {
            OS_ServeProcess::MountTask(Aux_ACIMTest::TaskID,				Aux_ACIMTest::Priority,							1);
        }
        TIMERSFunc::Enable_Timer(3);    //使能Timer3
    }

    // Aux_ACIM
    for(b_id = 0; b_id<8; b_id++)
    {
        if(Aux_DataManager::m_AllAuxBoards[b_id].m_BoardInfo.m_abi_globe.m_BoardType !=Aux_DDS_ACIM_415466 && Aux_DataManager::m_AllAuxBoards[b_id].m_BoardInfo.m_abi_globe.m_BoardType != Aux_DDS_Sinwave_Current_Source_604700)
            continue;
        else
            break;
    }
    if(b_id<8)
    {
        if(!OS_ServeProcess::IsMounted(Aux_ACIMTest::TaskID))
        {
            OS_ServeProcess::MountTask(Aux_ACIMTest::TaskID,				Aux_ACIMTest::Priority,							1);
        }
        AUX_Unit_Base::m_EnableISRsample = false;
    }


    OS_ServeProcess::MountTask(MultiMcuComm::TaskID,      MultiMcuComm::Priority,            1);

//========Add for Cold Plate by ZC 04/05/2018================
    if(Aux_MTC_Base::Aux_MTC_Board)
    {

        if(!OS_ServeProcess::IsMounted(DWIN_UartParse::TaskID))
            OS_ServeProcess::MountTask(DWIN_UartParse::TaskID, DWIN_UartParse::Priority, 1);
        if(!OS_ServeProcess::IsMounted(Aux_MTC_Base::TaskID))
            OS_ServeProcess::MountTask(Aux_MTC_Base::TaskID, Aux_MTC_Base::Priority, 1);
    }
    for(b_id = 0; b_id<8; b_id++)
    {
        if((Aux_DataManager::m_AllAuxBoards[b_id].m_BoardInfo.m_abi_globe.m_BoardType!=Aux_PowerADJ_200A_412206)
         &&(Aux_DataManager::m_AllAuxBoards[b_id].m_BoardInfo.m_abi_globe.m_BoardType !=Aux_PowerADJ_2US6K_422108))
            continue;
        else
            break;

    }
    if(b_id<8)
        if(!OS_ServeProcess::IsMounted(POWERADJ::TaskID))
            OS_ServeProcess::MountTask(POWERADJ::TaskID,                 POWERADJ::Priority,                  1);
}

int main(void)
{
    // 	   AUX_Unit_Base::Aux_Init_Finished_flag = 0;
    AUX_Unit_Base::m_EnableISRsample = false;
    OS_ServeProcess::Init_Device_Firmware();      //初始化片内外设

    PrepareForInitAUX();     //辅助初始化准备
    OSCreateTaskForInit();  // OS初始化

    EventsBuffering::MyInit();
// 	CANMsgBuffering::MyInit();

    OS_ServeProcess::Init_NetWork();
	

#if MCU_WATCHDOG_ENABLE == 1
    WDTFunc::Watchdog_Init();     //Add by DKQ 06_17_2014
    WDTFunc::Watchdog_Feed();     //Add by DKQ 06_17_2014
#endif

    while(1)
    {
        OS_ServeProcess::OSAuthorize();
    }
}

unsigned char Aux_getBID(void)
{

    //BS0-BS7    P3.24-P3.31
    unsigned char BoardID = 0;

    St_Port MyPort;
    MyPort.PortType = eCPTYPE_MCUGPIO;
    MyPort.Function = GPIO_I;
    MyPort.Port = 3;
    MyPort.Bits = 24;

    for(unsigned char i = 0; i<8; i++)
    {
        ControlPin::SetIObyCP(MyPort);
        BoardID |= ControlPin::ReadMe(MyPort)<<i;

        MyPort.Bits++;
    }

    return BoardID;


}
/*********************************************************************************
**                            End Of File
*********************************************************************************/

