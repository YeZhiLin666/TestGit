#include "includes.h"


/*****************BoardConfig Class******************************************
** Start:         BoardConfig Class
******************************************************************************/
My_Func_Ptr_Void board_config::FeedWatchDog;

St_DacInfo       board_config::DacInfo;
float            board_config::Accuracy;
bool             board_config::bHaveRedundantCheck;
bool			       board_config::bFanAdj=0;

unsigned short    board_config::Voltage_Range_Change_Time;

// unsigned char    board_config::ClampH_Outputflag;
// unsigned char    board_config::ClampL_Outputflag;
// volatile St_ACR  board_config::st_ACR;    //记录ACR的channel状态
// volatile St_DCS  board_config::st_DCS;    //记录DCS状态
My_Func_Ptr_Ext  board_config::Func_Dac_out;
My_Func_Ptr      board_config::Func_Sample;
My_Func_Ptr_Ext  board_config::Func_SetRange;
My_Func_Ptr_Ext2 board_config::Func_SetRelay;
My_Func_Ptr_Ext7 board_config::Func_Set_Volt_Range;
My_Func_Ptr_Ext1 board_config::Func_SetAutoRange;
//My_Func_ptr_Ext3 board_config::Func_TempSample;   // 20140613  温度测量
My_Func_Ptr      board_config::Func_CheckError;    //add Wzh
My_Func_Ptr      board_config::Func_RedundantSafety;   //冗余电压保护
My_Func_Ptr      board_config::Func_ReleaseRedundantSafety;  //冗余恢复操作
My_Func_Ptr      board_config::Func_FanSafety;   //风扇保护
My_Func_Ptr      board_config::Func_TemperatureSafety;//温度保护
//My_Func_Ptr_Void board_config::Func_Avoid_Hot;

My_Func_Ptr_Ext  board_config::Func_UnitDO;       //模块内部DIDO支持
My_Func_Ptr_Ext  board_config::Func_UnitDI;
My_Func_Ptr      board_config::Func_UnitDO_Reset;
My_Func_Ptr_Void board_config::Func_UnitDI_ReadAll;
My_Func_Ptr_Void board_config::Func_BuildInVolt_ReadAll;
My_Func_Ptr      board_config::Func_RTCProcedureForIV;  //RTC 输出控制   2015/03/30
My_Func_Ptr		   board_config::Func_FanAdj;    //fan adjust
My_Func_Ptr      board_config::Func_ACR;
//unsigned short board_config::IVBoardType;
//unsigned short board_config::IVBoardTypeversion;
St_Hardware_Feature_Enable  board_config::Feature_Enable_init;

const St_Port    board_config::Pin_ACIM      = {1,        9,         eCPTYPE_CPLD,      CPLD_O,                NBUS_WRITE_DCS};

unsigned char board_config::getBID(void)
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
char board_config::DAC_out(unsigned long CH_Number,unsigned long Data,unsigned long type)
{
    char temp = 0;
    temp=( *board_config::Func_Dac_out)(CH_Number,Data,type);
    return temp;
}

char board_config::FanSafetyCheck(unsigned long CH_Number,unsigned long type)
{
    char temp = 0;
    temp=(* board_config::Func_FanSafety)(CH_Number,type);
    return temp;
}
char board_config::TemperatureSafetyCheck(unsigned long CH_Number,unsigned long type)
{
    char temp = 0;
    temp=(* board_config::Func_TemperatureSafety)(CH_Number,type);
    return temp;
}
char board_config::RedundantSafety(unsigned long CH_Number,unsigned long type)
{
    if(bHaveRedundantCheck)
    {
        char temp = 0;
        temp=(* board_config::Func_RedundantSafety)(CH_Number,type);
        return temp;
    }
    else
        return 0;   //没有雍余电压监测的硬件，返回0，表示没有问题发生
}
char board_config::ReleaseRedundantSafety(unsigned long CH_Number,unsigned long type)
{
    if(bHaveRedundantCheck)
    {
        char temp = 0;
        temp=(* board_config::Func_ReleaseRedundantSafety)(CH_Number,type);
        return temp;
    }
    else
        return 1;  //这个返回值看到没有被使用，这里暂时被设置为1，表示执行正常
}

char board_config::OUTPUT_UnitDO(unsigned long CH_Number,unsigned long DO_Idx,unsigned long Value)       //模块内部DIDO支持
{
    char temp = 0;
    if(board_config::Func_UnitDO != 0)
        temp=(* board_config::Func_UnitDO)(CH_Number,DO_Idx,Value);

    return temp;
}
// char board_config::INPUT_UnitDI(unsigned long CH_Number,unsigned long DI_Idx,unsigned long Value)
// {
// 	char temp = 0;
// 	if(board_config::Func_UnitDI != 0)
// 		temp=(* board_config::Func_UnitDI)(CH_Number,DI_Idx,Value);

// 	return temp;
// }
char board_config::RTCProcedureForIV(unsigned long CH_Number,unsigned long Value)
{
    char temp=0;
//#if (_BOARD_TYPE_  == BOARDTYPE_HPT200)
    if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_HPT200)
    {
        if(board_config::Func_RTCProcedureForIV != 0)
            temp=(* board_config::Func_RTCProcedureForIV)(CH_Number,Value);
    }
//#endif
    return temp;
}
// char board_config::FanAdj(unsigned long Level,unsigned long AdjMod)
// {
// 	char temp=0;
// 	if(board_config::bFanAdj)
// 		temp=(* board_config::Func_FanAdj)(Level,AdjMod);
// 	return temp;
// }




//#if (POWER_OFF_ENABLE == 1)
//PowerLostCheck start

UInt16 PowerLostCheck::skip_time;    //1s or 5ms
MsTime PowerLostCheck::last_Time;

unsigned char PowerLostCheck::first_check = 1;

unsigned char PowerLostCheck::powerLost;
unsigned char PowerLostCheck::powerOn;
unsigned short PowerLostCheck::powerLost_counter;
unsigned short PowerLostCheck::powerOn_counter;
unsigned char PowerLostCheck::ParallelPowerCheckFlag;

unsigned char PowerLostCheck::PowerIsOn = 1;
My_Func_Ptr_void1  PowerLostCheck::Func_PowerOffSafetCheck;
UInt16  PowerLostCheck::Priority = PRIORITY8;    //PRIORITY8
UInt16  PowerLostCheck::TaskID = POWERLOSTCHECK_TASK;    //POWERLOSTCHECK_TASK

void PowerLostCheck::Init(void)
{
    skip_time = 0;
    memset(&last_Time,0,sizeof(MsTime));
    first_check = 1;
    powerLost = 0;
    powerOn = 0;
    powerLost_counter = 0;
    powerOn_counter = 0;
    PowerIsOn = 1;
    ParallelPowerCheckFlag=0;
}


void PowerLostCheck::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_POWER_LOST] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time_init(COUNT_POWER_LOST);
#endif
	
    MsTime time_d;
    //  unsigned char uc_res;

    if(first_check == 1)
    {
        CommonFunc::GetTimeTicksUpdate(&last_Time);
        skip_time = 10000;     //1s checkPower
        first_check = 0;

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_POWER_LOST] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT + 1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time(COUNT_POWER_LOST);
#endif
			
        return;
    }

    time_d = CommonFunc::TimeDiff(last_Time);    //os_time > last time

    if( (time_d.Second*10000+time_d.Us100) > skip_time)
    {
        //    uc_res = checkPower();
        checkPower();

        skip_time = 5*10;    //5ms
        CommonFunc::GetTimeTicksUpdate(&last_Time);    //record time again
    }

    stateProcess();

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_POWER_LOST] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT + 1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
    Time_Count::thread_time(COUNT_POWER_LOST);
#endif	
}
unsigned char PowerLostCheck::PowerOffSafety(void)
{
    unsigned char temp = 0;
    temp=(* PowerLostCheck::Func_PowerOffSafetCheck)();
    return temp;
}
unsigned char PowerLostCheck::checkPower()
{
    unsigned char checkHigh = 0;
// 	  bool  bcheckPort = 0;

    if(PowerLostCheck::PowerOffSafety != 0)
        checkHigh = PowerLostCheck::PowerOffSafety();
    /* if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_LBT21024)
      {
    		//check P1.11
    		bool  bcheckPort;
        bcheckPort = GPIOFunc::RD_G_Port(LBT21024_POWER_OFF_PORT,LBT21024_POWER_OFF_PIN);
    		checkHigh = bcheckPort;
    		if(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion==BOARDTYPE_VER_C )
    		{
         checkHigh = (!bcheckPort);
       }
    	}
     else if((ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_LBT21084)||(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_LBT21044))
    //check P1.11  //高电平有效
        checkHigh = GPIOFunc::RD_G_Port(LBT21084_POWER_OFF_PORT,LBT21084_POWER_OFF_PIN);
     else if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_MSTAT21044)
    //check P1.11  //高电平有效
        checkHigh = GPIOFunc::RD_G_Port(MSTAT21044_POWER_OFF_PORT,MSTAT21044_POWER_OFF_PIN);
      else if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_LBT22013)
      {//check P4.29   //低电平有效
        bcheckPort = GPIOFunc::RD_G_Port(LBT22013_POWER_OFF_PORT,LBT22013_POWER_OFF_PIN);
        checkHigh = (!bcheckPort);
     }
     else if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_LBT22023)
      {//check P4.29   //低电平有效
        bcheckPort = GPIOFunc::RD_G_Port(LBT22023_POWER_OFF_PORT,LBT22023_POWER_OFF_PIN);
        checkHigh = (!bcheckPort);
     }
      else if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_HPT200)
      {//check P2.3   //低电平有效
        bcheckPort = GPIOFunc::RD_G_Port(HPT200_POWER_OFF_PORT,HPT200_POWER_OFF_PIN);
        checkHigh = (!bcheckPort);
     }
      else if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_HS21044)
      {//check P4.29   //低电平有效
        bcheckPort = GPIOFunc::RD_G_Port(HS21044_POWER_OFF_PORT,HS21044_POWER_OFF_PIN);
        checkHigh = (bcheckPort);
     }
     else if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_LBT22043)
     {
        //check P4.29   //低电平有效
        bcheckPort = GPIOFunc::RD_G_Port(LBT22043_POWER_OFF_PORT,LBT22043_POWER_OFF_PIN);
        checkHigh = (!bcheckPort);
      }
    	else if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_LBT20084)
        checkHigh = GPIOFunc::RD_G_Port(LBT20084_POWER_OFF_PORT,LBT20084_POWER_OFF_PIN);
      else if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_HPM4)
      {
         bool  bcheckPort;
         bcheckPort = GPIOFunc::RD_G_Port(HPM4_POWER_OFF_PORT,HPM4_POWER_OFF_PIN);
         checkHigh = (!bcheckPort);
      }
      else if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_LBT21162)
       checkHigh = GPIOFunc::RD_G_Port(LBT21162_POWER_OFF_PORT,LBT21162_POWER_OFF_PIN);
      else if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_LBT21014)
      {
    	  checkHigh = GPIOFunc::RD_G_Port(LBT21014_POWER_OFF_PORT,LBT21014_POWER_OFF_PIN);
      }
      else if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_LBT20162)
      {
       checkHigh = GPIOFunc::RD_G_Port(LBT20162_POWER_OFF_PORT,LBT20162_POWER_OFF_PIN);
      }
     else
    //check P2.7
        checkHigh = GPIOFunc::RD_G_Port(TESLA_POWER_OFF_PORT,TESLA_POWER_OFF_PIN);
    		*/
    if(checkHigh == 1)
    {
        powerLost = 1;
        powerLost_counter++;
        powerOn = 0;
        powerOn_counter = 0;
    }
    else
    {
        powerLost = 0;
        powerLost_counter = 0;
        powerOn = 1;
        powerOn_counter++;
    }

    return 1;
}

unsigned char PowerLostCheck::stateProcess()
{
    static unsigned char powerState = 0xC1;
    unsigned char Lost = 0;

    if( (powerLost == 1)&&(powerLost_counter > 200) )  //30
        Lost = 1;

//     if((ChannelInfo_Manager::m_st_BoadType.m_u16BoardType== BOARDTYPE_IBT31162)
//         &&(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion == BOARDTYPE_VER_A))
// 	{
//         if( (powerLost == 1)&&(powerLost_counter > 60) )
//             Lost = 1;
//         else
//             Lost = 0;
// 	}

    if(Lost)
    {
        PowerIsOn = 0;

        if(powerState == 0xC1)
        {
            powerState = 0xB1;
            sendPowerOff();
        }
        if((powerState==0XB1) &&(ParallelPowerCheckFlag==1))
        {
            ParallelPowerCheckFlag=0;
            sendPowerOff();
        }
        first_check = 1;
        powerLost_counter = 0;
    }

    if( (powerOn == 1)&&(powerOn_counter > 400) )
    {
        PowerIsOn = 1;

        powerOn_counter = 0;

        powerState = 0xC1;
				for(unsigned char chn = 0; chn < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; chn ++)
				{
						safety_check::ClrUnsafe(chn, IV_POWER_OFF_UNSAFE,IV_UNSAFE_TYPE);
				}
    }

    return 1;
}

unsigned char PowerLostCheck::sendPowerOff()
{
    //for(unsigned char chn = 0;chn < MAXCHANNELNO; chn++)
    for(unsigned char chn = 0; chn < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; chn ++)
    {
        //ErrorFlag  // m_btSafe_Code need to be re-explained in Schedule management task
        St_ScheduleMessage sch_msg;
        char if_third;

        if_third = thirdParty::getStatus(chn);
// 		if(if_third != THIRD_PARTY_STATUS_INACTIVE)	//Third part process
// 		{
        if(if_third == THIRD_PARTY_STATUS_OUTPUT)	// only stop when doing output, // I keep the original code without major modifying, only delete the error code clearing- philip
        {
            st_Third_Party_Msg TP_msg;
            TP_msg.chn = chn;
            TP_msg.TaskID = SAFETYCHECK_TASK;
            TP_msg.st_third_party_cmd.cmd = THIRD_PARTY_STOP;
            thirdParty::sendMsgToMe(&TP_msg);
        }
// 		}

        //<<<LATER  power off
        // send STOP_POWER_OFF to Sch
        // Packing Msg
        sch_msg.MsgNumber = Sch_OnUnsafe_Msg;    // stop
        sch_msg.m_Message.m_Assign.m_u16ChannelIdx = chn;
        Schedule_Manager_task::SendFinishFlag[chn].StopStatus = STOP_UNSAFE;
        //powerOff
				ChannelInfo_Manager::ChannelInfo[chn].m_UnsafeType = IV_UNSAFE_TYPE;
				ChannelInfo_Manager::ChannelInfo[chn].m_SafetyStatus = IV_POWER_OFF_UNSAFE;
				safety_check::SetUnsafe(chn, IV_POWER_OFF_UNSAFE, 0,IV_UNSAFE_TYPE);
        if (ChannelInfo_Manager::Serial2ParaMod_IsInterUnitSubmasterChannel(chn))
        {
            //send CAN msg to master chan
					CANBusParse::Parallel_SubmasterSend_ReportUnsafe(IV_POWER_OFF_UNSAFE, 0,chn);
        }
        else
				{			
					Schedule_Manager_task::SendMsgToSchedule(chn,sch_msg);
				}
		} 

    return 1;
}

//end PowerLostCheck
//#endif

/*****************BoardConfig Class******************************************
** End:           BoardConfig Class
******************************************************************************/

