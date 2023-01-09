#include "../Entry/includes.h"

//public:
unsigned char   AuxCConfig::bConfigValid;
UInt16          AuxCConfig::Priority = PRIORITYBG;
UInt16          AuxCConfig::TaskID = EEPROM_TASK;

//private:
unsigned char   AuxCConfig::bWriting;
unsigned short  AuxCConfig::u16Byte;
unsigned char   AuxCConfig::ucBit;
unsigned long   AuxCConfig::u32ByteCount;
unsigned short  AuxCConfig::u16FramAddress;
unsigned long   AuxCConfig::u32CfgCount;
unsigned long   AuxCConfig::u32CfgRelativeBaseAddress;
unsigned long   AuxCConfig::u32CfgAbsoluteBaseAddress;
St_FramMessage  AuxCConfig::MyMessage;
St_FramMessages AuxCConfig::Messages;
St_FramHeader   AuxCConfig::FramHeader;
unsigned short  AuxCConfig::uwOutOf_FRAM_error = 0;

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM",zidata = "SRAM"		   // ???????????????	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // ???????????????	   dynamic memory  0XA0000000  0x03E80000
#endif
ST_AUX_BOARD_INFO  AuxCConfig::m_AuxBoards;
St_AUXChannelCFG   AuxCConfig::AUX_I2C_ReadCfg;
#pragma arm section //????????

//public:
unsigned char AuxCConfig::MyInit(void)
{
    FM25L256Func::MyInit();

    Messages.m_u32Start = 0;
    Messages.m_u32Length = 0;
    bWriting = 0;

    // Begin 预检查FRAM地址是否出现越区情况  2016.09.20
    unsigned short int  uw16_CFG[MAX_ADDRESS] =
    {   ( FRAM_BASE + MAXAUXMCUCHANNE * sizeof(St_DOWNLOAD_MAP_FROM_PC) ),
        FRAM_REMAIN_LENGTH
    };

    if ((uw16_CFG[AUX_MAP] > FRAM_REMAIN_LENGTH)||(uw16_CFG[AUX_MAP] > FRAM_AUXMAP_LENGTH))
        uwOutOf_FRAM_error |= 1<<AUX_MAP;  // Please check FRAM_BASE and size

    //if (uw16_CFG[REMAIN_EMPTY] < 1)
    //    uw16_error |= 1<<REMAIN_EMPTY;  // Please check FRAM_AUXMAP_BASE and size

//    if(uw16_error != 0)  // Please check FRAM size
//    {
//        while(1);  // Please check FRAM size
//    }
    // End 预检查FRAM地址是否出现越区情况  2016.09.20

    return 1;
}
unsigned char AuxCConfig::LoadSysConfig(void)
{
    unsigned char uc_Result = 0;
// 	if(FM25L256Func::checkEEPROMBusy())
// 		return 0;        //SysConfig已经load过一次了，现在针对FRAM的所有操作都需要经过消息机制;
// 	else
// 	{
    readConfig(0);  //开机或MCU重启load system config;直接load，无需经过消息机制;
    Aux_DataManager::Aux_GetMapfromPC( &uc_Result);    // 转换PCMap表以便管理
// 		for(unsigned char i=0;i<128;i++)
// 		{
// 			memset(&EthernetParse::m_btPackDataBuffer[2071+i*6],0,sizeof(St_DOWNLOAD_MAP_FROM_PC));     //清缓冲区
// 		}
    // 	Aux_DataManager::Aux_TranslateMapFromPC();  // add by qjm 2015/5/18

    uc_Result = Load_I2C_Config();  //读取E2中的板信息和通道信息
    ///////////////////////////////////////////////////////////////
    //手动写入电池包FRAM信息, Yifei
    //
    ///////////////////////////////////////////////////////////////
    /*
    for (unsigned char i = 0;i<MAX_BOARD_PER_AUXMCU;i++)
    {   // 清理一次旧的信息,避免上位机写板工具继续提示旧信息,没进行更新操作.
    memset(&Aux_DataManager::m_AllAuxBoards[i],0,sizeof(ST_AUX_BOARD_INFO));
    // Aux_DataManager::m_AllAuxBoards[i].m_BoardInfo.m_BType.m_AuxType = NUM_OF_AUX_CHAN_TYPE_NULL;
    }

    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_valid =1;						//通道有效

    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_BID = 0;	//拨码
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_BoardType = Aux_BC_2ndV_508746;	//辅助板型
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_AuxOtherType = 0;
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_Total_Channels = 60;   //实际有效的通道总数。这个用来计算data  group  index
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_Max_Channels = 96;    //该辅助板所支持的最大通道数（电气特性决定）
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_Total_ADC = 0;       //ADC 总数
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_Total_CANBMS = 0;  //CANBMS通道总数， 1 or 0
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_Total_UART = 0;    //UART 总数
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_Total_DI = 0;       // DI 总数
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_Total_DO = 0;      //DO 总数
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_Total_I2C = 0;     //i2c
    Aux_DataManager::m_AllAuxBoards[0].m_Start_InUnit = 0;
    Aux_DataManager::m_AllAuxBoards[0].m_End_InUnit = 0;
    for(int k=0;k<60;k++)
    {
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_chn[k].m_Populated  = 1;
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_chn[k].m_AuxType = AUX_VOLTAGE;   //辅助类型， 比如是温度，电压，还是压力，pH,
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_chn[k].m_AuxSubType = BatteryPack_2ndV;  //辅助的次级类型，也就是传感器类型，比如是T，K，J 热电偶， Pt100热电阻。
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_chn[k].m_AuxSensorCoreInfo.m_PHY_RangeL = 0;	//用于转换数据的传感器相关的信息
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_chn[k].m_AuxSensorCoreInfo.m_PHY_RangeH = 5;
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_chn[k].m_AuxSensorCoreInfo.m_ADC_Gain = 1;
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_chn[k].m_AuxSensorCoreInfo.m_ADC_Offset = 0;
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_chn[k].m_AuxSensorCoreInfo.m_PHY_Gain = 1;
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_chn[k].m_AuxSensorCoreInfo.m_PHY_Offset = 0;
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_chn[k].m_avg_num = 24;  //平滑滤波的数据个数
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_chn[k].m_Controllable = 0 ;   //是只读的传感器，还是可以控制的执行器，比如温控箱，流量控制器等。
    }
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_assemble.m_ConsistentOrder = 0;    //该板电气通道号是否从左到右 1—>8 这么排的
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_assemble.m_SlaveGoesFirst = 0;  // 该板的取向是否在副板再前的取向
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_assemble.m_WithSlave = 1;      // 该板是否带有副板
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_RSN = 5678;     //板在AUXMC中的注册信息
    */
    /***************************************************************************************************************************************************************/

    // for(int m = 1;m<8;m++)
    // {
    // Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_valid =0;						//通道无效
    // }
    /*

    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_globe.m_BID = 1;	//拨码
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_globe.m_BoardType = Aux_BC_2ndV_508746;	//辅助板型
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_globe.m_AuxOtherType = 0;
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_globe.m_Total_Channels = 60;   //实际有效的通道总数。这个用来计算data  group  index
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_globe.m_Max_Channels = 96;    //该辅助板所支持的最大通道数（电气特性决定）
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_globe.m_Total_ADC = 0;       //ADC 总数
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_globe.m_Total_CANBMS = 0;  //CANBMS通道总数， 1 or 0
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_globe.m_Total_UART = 0;    //UART 总数
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_globe.m_Total_DI = 0;       // DI 总数
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_globe.m_Total_DO = 0;      //DO 总数
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_globe.m_Total_I2C = 0;     //i2c
    for(int k=0;k<60;k++)
    {
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_chn[k].m_Populated  = 0;
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_chn[k].m_AuxType = AUX_BATTERYCELLS;   //辅助类型， 比如是温度，电压，还是压力，pH,
    //Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_globe.m_abi_chn[k].m_AuxSubType;  //辅助的次级类型，也就是传感器类型，比如是T，K，J 热电偶， Pt100热电阻。
    //Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_chn[k].m_AuxSensorCoreInfo;  //用于转换数据的传感器相关的信息
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_chn[k].m_avg_num = 64;  //平滑滤波的数据个数
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_chn[k].m_Controllable = 0 ;   //是只读的传感器，还是可以控制的执行器，比如温控箱，流量控制器等。
    }
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_assemble.m_ConsistentOrder = 0;    //该板电气通道号是否从左到右 1—>8 这么排的
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_assemble.m_SlaveGoesFirst = 0;  // 该板的取向是否在副板再前的取向
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_assemble.m_WithSlave = 0;      // 该板是否带有副板
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_RSN = 1234;     //板在AUXMC中的注册信息
    }
    */

    ///////////////////////////////////////////////////////////////
    //手动写入电池包FRAM信息, Yifei
    //
    ///////////////////////////////////////////////////////////////

    return uc_Result;
// 	}
}
void AuxCConfig::DoWork(void)
{
    unsigned char ucByteContent = 0;

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_FRAM] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_FRAM);
#endif

    if (bWriting)         //正在进行一个写任务
    {
        u16Byte = (u32CfgRelativeBaseAddress + u32CfgCount) / 8;  // 32K列表中，一个0XFF代表8个字节。 取列表数据组下标[即FramConfigDictionary[X]]。
        ucBit = (u32CfgRelativeBaseAddress + u32CfgCount) % 8;   // 32K列表中，一个0XFF代表8个字节。 取列表数据组中值位域[即0XFF]从0到7。

        ucByteContent = Aux_DataManager::FramConfigDictionary[u16Byte];  //  取列表数据组中值位域[即0XFF]。
        if ((ucByteContent >> ucBit)&0x01)   //  取列表数据组中值位域[即0XFF]从0到7。
        {
            FM25L256Func::writePage(u16FramAddress,(unsigned char *)(u32CfgAbsoluteBaseAddress + u32CfgCount),1);//单字节写入
            u16FramAddress++; //单字节地址变化
        }
        u32CfgCount++;   //单字节计数器变化
        if (u32CfgCount >= u32ByteCount)    //写操作结束了
        {
            FM25L256Func::writePage(8,(unsigned char *)0,1);  // 在存储器中最前面处预留了128字节。仅作为写一标志作用
            bWriting = 0;
#if REPORT_FRAM_EVENT == 1
            EthernetParse::MPReport_WriteFram_Event(MyMessage, &EthernetParse::m_pReport->m_Report_Write_Fram);
#endif

        }
    }
    else
    {
        if(PopFramMessage(&MyMessage) == 1)   //Check if full
        {
            u32CfgRelativeBaseAddress = (unsigned long)MyMessage.m_StartAddress - (unsigned long)MyMessage.m_StartAddress;  //将传递进来的绝对地址转换为相对地址  ： 绝对-绝对=0地址
            u32CfgAbsoluteBaseAddress = (unsigned long)MyMessage.m_StartAddress; //  绝对地址 = &Aux_DataManager::m_AuxUnitCFG
            u32ByteCount = MyMessage.m_u16Length;
            GetFramAddress();   //找到config地址对应的fram地址
            if (MyMessage.m_bIsWrite)
            {
                FM25L256Func::writePage(8,(unsigned char *)1,1);   // 在存储器中最前面处预留了128字节。仅作为写一标志作用
                bWriting = 1;
                u32CfgCount = 0;
            }
            else
            {
                u32CfgCount = 0;
                //read something to config running struct from fram;
            }
        }
    }

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count_Time[COUNT_FRAM] = CommonFunc::TimeDiff(OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
		Time_Count::thread_time(COUNT_FRAM);
#endif
}
int AuxCConfig::PopFramMessage(St_FramMessage * myMessage)
{
    unsigned char bEmpty;
    St_FramMessage message;

    if (IS_QUEUE_EMPTY(Messages))
        return -1;   //stand for empty;

    DEQUEUE(Messages,FRAM_MESSAGE_DEPTH,message,bEmpty);

    *myMessage = message;

    return 1;
}
void AuxCConfig::PushFramMessage(St_FramMessage myMessage)
{
    ENQUEUE(Messages,FRAM_MESSAGE_DEPTH,myMessage);
}
//private:
void AuxCConfig::readConfig(unsigned char Bid)
{
    unsigned char ucByteContent = 0;

    WriteContent_ForFRAM((unsigned char *)&FramHeader, 0, sizeof(FramHeader));
    FM25L256Func::readPage(0,(unsigned char *)&FramHeader,sizeof(FramHeader));

    MP_AllIVMap * AllIVMap = (MP_AllIVMap *) &EthernetParse::m_btPackDataBuffer[0];

    u16Byte = 0;
    u16FramAddress = FRAM_BASE;   // 在存储器中最前面处预留了128字节。
    u32CfgRelativeBaseAddress = 0;   // 相对地址  写时为0地址
    u32CfgAbsoluteBaseAddress = (unsigned long) AllIVMap;  // 绝对地址 内存地址
    ucBit = 0;

    /*
    	unsigned long uw32_length = (sizeof(MP_AllIVMap) - sizeof(St_DOWNLOAD_MAP_FROM_PC)* (MAX_AUX_TYPE_PER_MCU)); // 计算一次包中前面管理信息长度
        unsigned long uw32_GetIndex = MAX_AUX_TYPE_PER_MCU;  // 512;   // 用于最长存储FRAM的保护，暂定最大为512组一维数据。  // 扩大到5.7K存MAP表
        uw32_length += (uw32_GetIndex * sizeof(St_DOWNLOAD_MAP_FROM_PC));  // 计算出实际一维字节数量   以IV的最大11字节计算预留空间，不以AUX实际6字节计算预留。
    */
    //for(u32CfgCount = 0; u32CfgCount < (sizeof(MP_AllIVMap)); u32CfgCount++)   //读config可以一口气读出来，
    unsigned long uw32_length = sizeof(MP_AllIVMap);  // 计算出实际一维字节数量   以IV的最大11字节计算预留空间，不以AUX实际6字节计算预留。
    //for(u32CfgCount = 0; u32CfgCount <  MAX_MULTPACK_SIZE; u32CfgCount++)   //读config可以一口气读出来，
    for(u32CfgCount = 0; u32CfgCount <  uw32_length; u32CfgCount++)   //读config可以一口气读出来，
    {
        u16Byte = u32CfgCount / 8;
        ucBit = u32CfgCount % 8;

        ucByteContent = Aux_DataManager::FramConfigDictionary[u16Byte];
        if ((ucByteContent >> ucBit)&0x01)
        {
            FM25L256Func::readPage(u16FramAddress,(unsigned char *)(u32CfgAbsoluteBaseAddress + u32CfgCount),1); // 一个一个字节读出
            u16FramAddress++;
        }
    }

    // 	if(AuxGlobalCFG->m_u16AuxChannelCount > MAX_AUXCH_PER_AUXMCU)
    // 		AuxGlobalCFG->m_u16AuxChannelCount = MAX_AUXCH_PER_AUXMCU;

#if DEMO_MODE == 1
    Aux_DataManager::m_AuxUnitCFG.m_u16AuxChannelCount = MAX_AUXCH_PER_AUXMCU;
#endif

}
void AuxCConfig::GetFramAddress(void)
{
    unsigned long u32Count;
    unsigned char ucByteContent = 0;

    u16FramAddress = FRAM_BASE;  // 在存储器中最前面处预留了128字节。
    for(u32Count = 0; u32Count < u32CfgRelativeBaseAddress; u32Count++)
    {
        u16Byte = u32Count / 8;   // 32K列表中，一个0XFF代表8个字节。 取列表数据组下标[即FramConfigDictionary[X]]。
        ucBit = u32Count % 8;   // 32K列表中，一个0XFF代表8个字节。 取列表数据组中值位域[即0XFF]从0到7。

        ucByteContent = Aux_DataManager::FramConfigDictionary[u16Byte];  //  取列表数据组中值位域[即0XFF]。
        if ((ucByteContent >> ucBit)&0x01)  // 取列表数据组中值位域[即0XFF]从0到7。
            u16FramAddress++;   //  更改存储器中存储的首地址。
    }
}


// void AuxCConfig::SaveAuxUnitCFG(St_AuxBoardGlobalCFG * m_AuxUnitCFG)
// {
/*unsigned char ucByteContent = 0;

u32CfgRelativeBaseAddress = 0;  //将传递进来的绝对地址转换为相对地址  ： 绝对-绝对=0地址
u32CfgAbsoluteBaseAddress = (unsigned long) m_AuxUnitCFG; //  绝对地址 = &Aux_DataManager::m_AuxUnitCFG
u32ByteCount = sizeof(St_AuxBoardGlobalCFG);
GetFramAddress();   //找到config地址对应的fram地址
u32CfgCount = 0;

while(u32ByteCount > 0)
{
u16Byte = (u32CfgRelativeBaseAddress + u32CfgCount) / 8;  // 32K列表中，一个0XFF代表8个字节。 取列表数据组下标[即FramConfigDictionary[X]]。
ucBit = (u32CfgRelativeBaseAddress + u32CfgCount) % 8;   // 32K列表中，一个0XFF代表8个字节。 取列表数据组中值位域[即0XFF]从0到7。

ucByteContent = Aux_DataManager::FramConfigDictionary[u16Byte];  //  取列表数据组中值位域[即0XFF]。
if ((ucByteContent >> ucBit)&0x01)   //  取列表数据组中值位域[即0XFF]从0到7。
{
FM25L256Func::writePage(u16FramAddress,(unsigned char *)(u32CfgAbsoluteBaseAddress + u32CfgCount),1);//单字节写入
u16FramAddress++; //单字节地址变化
}
u32CfgCount++;   //单字节计数器变化
if (u32CfgCount >= u32ByteCount)    //写操作结束了
{
#if REPORT_FRAM_EVENT == 1
EthernetParse::MPReport_WriteFram_Event(MyMessage, &EthernetParse::m_pReport->m_Report_Write_Fram);
#endif
return;
}
}*/
// }


void AuxCConfig::SaveAuxUnit2IVMap(MP_AllIVMap * m_AuxUnit_IVMap, UWord32 uw32_Length)
{
    unsigned char ucByteContent = 0;

    u32CfgRelativeBaseAddress = 0;  //将传递进来的绝对地址转换为相对地址  ： 绝对-绝对=0地址
    u32CfgAbsoluteBaseAddress = (unsigned long) m_AuxUnit_IVMap; //  绝对地址 = &Aux_DataManager::m_AuxUnitCFG
    u32ByteCount = uw32_Length;
    GetFramAddress();   //找到config地址对应的fram地址
    u32CfgCount = 0;

    while(u32CfgCount < u32ByteCount)
    {
        u16Byte = (u32CfgRelativeBaseAddress + u32CfgCount) / 8;  // 32K列表中，一个0XFF代表8个字节。 取列表数据组下标[即FramConfigDictionary[X]]。
        ucBit = (u32CfgRelativeBaseAddress + u32CfgCount) % 8;   // 32K列表中，一个0XFF代表8个字节。 取列表数据组中值位域[即0XFF]从0到7。

        ucByteContent = Aux_DataManager::FramConfigDictionary[u16Byte];  //  取列表数据组中值位域[即0XFF]。
        if ((ucByteContent >> ucBit)&0x01)   //  取列表数据组中值位域[即0XFF]从0到7。
        {
            FM25L256Func::writePage(u16FramAddress,(unsigned char *)(u32CfgAbsoluteBaseAddress + u32CfgCount),1);//单字节写入
            u16FramAddress++; //单字节地址变化
        }
        u32CfgCount++;   //单字节计数器变化
        if (u32CfgCount >= u32ByteCount)    //写操作结束了
        {
#if REPORT_FRAM_EVENT == 1
            EthernetParse::MPReport_WriteFram_Event(MyMessage, &EthernetParse::m_pReport->m_Report_Write_Fram);
#endif
            break;
        }
    }
// 	for(unsigned char i=0;i<128;i++)    //add by qjm 05/25/2015
// 	{
// 		memset(&EthernetParse::m_btPackDataBuffer[2071+i*6],0,sizeof(St_DOWNLOAD_MAP_FROM_PC));     //清缓冲区
// 	}
    // 		unsigned long u32Count;
    // 	   unsigned char ucByteContent = 0;

    // 	u16FramAddress = FRAM_BASE;  // 在存储器中最前面处预留了128字节。

}

UChar8 AuxCConfig::WriteRSN(UChar8 uc_BoardID, UWord16 * uw16_RSN)
{
    // 写入FRAM的I2C配置管理任务RSN号
    //if(uc_BoardID >= MAX_BOARD_PER_AUXMCU)
    //	return 0;

    u16FramAddress = FRAM_I2CRSN_BASE + uc_BoardID*sizeof(UWord16);
    //u32CfgAbsoluteBaseAddress = MyMessage.m_StartAddress;

    uc_BoardID = FM25L256Func::writePage(u16FramAddress,(unsigned char *) uw16_RSN,sizeof(UWord16));
    return uc_BoardID;
}

UChar8 AuxCConfig::ReadRSN(UChar8 uc_BoardID, UWord16 * uw16_RSN)
{
    // 从FRAM读I2C配置管理任务RSN号
    //if(uc_BoardID >= MAX_BOARD_PER_AUXMCU)
    //	return;

    u16FramAddress = FRAM_I2CRSN_BASE + uc_BoardID*sizeof(UWord16);

    uc_BoardID = FM25L256Func::readPage(u16FramAddress,(unsigned char *) uw16_RSN,sizeof(UWord16));
    return uc_BoardID;
}

void AuxCConfig::WriteRSNStata(UChar8 uc_BoardID, UChar8 * uc_Stata)
{
    // 写入FRAM的I2C配置管理任务状态信息
    if(uc_BoardID >= MAX_BOARD_PER_AUXMCU)
        return;

    u16FramAddress = FRAM_I2CRSN_STATABASE + uc_BoardID;
    //u32CfgAbsoluteBaseAddress = MyMessage.m_StartAddress;

    FM25L256Func::writePage(u16FramAddress,uc_Stata,sizeof(UChar8));
}

void AuxCConfig::ReadRSNStata(UChar8 uc_BoardID, UChar8 * uc_Stata)
{
    // 从FRAM读I2C配置管理任务状态信息
    if(uc_BoardID >= MAX_BOARD_PER_AUXMCU)
        return;

    u16FramAddress = FRAM_I2CRSN_STATABASE + uc_BoardID;

    FM25L256Func::readPage(u16FramAddress,uc_Stata,sizeof(UChar8));
}


//  以上为SPI的FRAM操作
//  以下为I2C的EPPROM操作
unsigned char AuxCConfig::Load_I2C_Config(void)
{
    unsigned char uc_Result = 0;
    unsigned char uc_UARTCh = 0;
    unsigned char uc_DACCh = 0;
    unsigned char uc_I2CCh = 0;  //add by zc 04.20.2018
    //     unsigned char uc_FirstFlag = 0;

    for (unsigned char i = 0; i<MAX_BOARD_PER_AUXMCU; i++)
    {   // 清理一次旧的信息,避免上位机写板工具继续提示旧信息,没进行更新操作.
        memset(&Aux_DataManager::m_AllAuxBoards[i],0,sizeof(ST_AUX_BOARD_INFO));
    }

    uc_Result = AuxCConfig::Reg_AUXBoardInfo();  //读板信息

    //static St_AUXChannelCFG  ReadCfg;
		memset(&AuxCConfig::AUX_I2C_ReadCfg,0,sizeof(St_AUXChannelCFG));
    unsigned char uc_I2CSuccessFlag[MAX_CH_IN_AUX_UNIT];
    //unsigned char uc_TotalChid = 0;
    for(unsigned char uc_Bid = 0; uc_Bid < MAX_BOARD_PER_AUXMCU; uc_Bid ++)
    {
        //Aux_DataManager::m_AllAuxBoards[uc_Bid] = E2PROM_24LC128::ReadAUXBoardCfg(uc_Bid);
        if((Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_valid == BEUSED)&&(Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_globe.m_Total_Channels != 0))
        {
            if(Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_globe.m_Max_Channels > MAX_CH_IN_AUX_UNIT)
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_globe.m_Max_Channels = MAX_CH_IN_AUX_UNIT; // 增加保护避免死机 06.16.2017 yy

            //for(unsigned char uc_Chid = 0; uc_Chid < Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_globe.m_Total_Channels; uc_Chid ++)
            for(unsigned char uc_Chid = 0; uc_Chid < Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_globe.m_Max_Channels; uc_Chid ++)
            {
                //  “表像值”特殊处理  硬件问题用软件解决  // 对于一般用户来讲。 容易理解On是1， Off 是0，并且，我们约定辅助在unit内的编号从 低BID板号开始排起。 因为这样比较符合习惯。
                //   S2器件ON为0处理成ON为1               // 因此，请在一开始读板号的时候，做一下处理，使得板号值和拨码的“表像值” 一致。以后就统一用这个“表像值”做板号，来保证一致性。

                ReadAUXChannelCfg((MAX_BOARD_PER_AUXMCU-1-uc_Bid),uc_Chid,&uc_I2CSuccessFlag[uc_Chid]);


                // Aux_DataManager::m_AllAuxBoards[uc_Bid].m_AuxSensor_ChInfo[uc_Chid] = ReadCfg.m_AuxChInfo;
                // unsigned char uc_ch=ReadCfg.m_ChannelNumber;
                //下面是读通道信息。可是结构里也有板的全局信息。。。，这应考虑和并到m_AllAUxBoards,统一管理，避免出错。
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_chn[uc_Chid].m_Populated = AuxCConfig::AUX_I2C_ReadCfg.m_valid;
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_chn[uc_Chid].m_ChnCoreInfo.m_AuxSensorCoreInfo = AuxCConfig::AUX_I2C_ReadCfg.m_ChnCoreInfo.m_AuxSensorCoreInfo;
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_chn[uc_Chid].m_AuxType= AuxCConfig::AUX_I2C_ReadCfg.m_AuxType;
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_chn[uc_Chid].m_AuxSubType =AuxCConfig::AUX_I2C_ReadCfg.m_AuxSubType;
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_chn[uc_Chid].m_avg_num = AuxCConfig::AUX_I2C_ReadCfg.m_AVG_num;
            }
        }
        if((Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_valid == BEUSED)&&(Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_globe.m_Total_UART !=0))
        {
            for(unsigned char uc_Chid = 0; uc_Chid < 8; uc_Chid ++)
            {
                //  “表像值”特殊处理  硬件问题用软件解决  // 对于一般用户来讲。 容易理解On是1， Off 是0，并且，我们约定辅助在unit内的编号从 低BID板号开始排起。 因为这样比较符合习惯。
                //   S2器件ON为0处理成ON为1               // 因此，请在一开始读板号的时候，做一下处理，使得板号值和拨码的“表像值” 一致。以后就统一用这个“表像值”做板号，来保证一致性。
                uc_UARTCh =uc_Chid +MAX_CH_IN_AUX_UNIT;
                ReadAUXChannelCfg((MAX_BOARD_PER_AUXMCU-1-uc_Bid),uc_UARTCh,&uc_I2CSuccessFlag[uc_Chid]);


                // Aux_DataManager::m_AllAuxBoards[uc_Bid].m_AuxSensor_ChInfo[uc_Chid] = ReadCfg.m_AuxChInfo;
                // unsigned char uc_ch=ReadCfg.m_ChannelNumber;
                //下面是读通道信息。可是结构里也有板的全局信息。。。，这应考虑和并到m_AllAUxBoards,统一管理，避免出错。
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo. m_abi_uart_chn[uc_Chid].m_Populated = AuxCConfig::AUX_I2C_ReadCfg.m_valid;
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo. m_abi_uart_chn[uc_Chid].m_ChnCoreInfo.m_AuxSensorCoreInfo = AuxCConfig::AUX_I2C_ReadCfg.m_ChnCoreInfo.m_AuxSensorCoreInfo;
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo. m_abi_uart_chn[uc_Chid].m_AuxType= AuxCConfig::AUX_I2C_ReadCfg.m_AuxType;
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo. m_abi_uart_chn[uc_Chid].m_AuxSubType = UART;	// = 1000, 上位机未写该参数，此处下位机手动写入，是否应修改？ Yifei
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo. m_abi_uart_chn[uc_Chid].m_avg_num = AuxCConfig::AUX_I2C_ReadCfg.m_AVG_num;
            }
        }
        if((Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_valid == BEUSED)&&(Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_globe.m_Total_DAC!=0))
        {
            for(unsigned char uc_Chid = 0; uc_Chid < 8; uc_Chid ++)
            {
                //  “表像值”特殊处理  硬件问题用软件解决  // 对于一般用户来讲。 容易理解On是1， Off 是0，并且，我们约定辅助在unit内的编号从 低BID板号开始排起。 因为这样比较符合习惯。
                //   S2器件ON为0处理成ON为1               // 因此，请在一开始读板号的时候，做一下处理，使得板号值和拨码的“表像值” 一致。以后就统一用这个“表像值”做板号，来保证一致性。
                uc_DACCh =uc_Chid +MAX_CH_IN_AUX_UNIT+8;
                ReadAUXChannelCfg((MAX_BOARD_PER_AUXMCU-1-uc_Bid),uc_DACCh,&uc_I2CSuccessFlag[uc_Chid]);


                // Aux_DataManager::m_AllAuxBoards[uc_Bid].m_AuxSensor_ChInfo[uc_Chid] = ReadCfg.m_AuxChInfo;
                // unsigned char uc_ch=ReadCfg.m_ChannelNumber;
                //下面是读通道信息。可是结构里也有板的全局信息。。。，这应考虑和并到m_AllAUxBoards,统一管理，避免出错。
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo. m_abi_DAC_chn[uc_Chid].m_Populated = AuxCConfig::AUX_I2C_ReadCfg.m_valid;
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo. m_abi_DAC_chn[uc_Chid].m_ChnCoreInfo.m_AuxSensorCoreInfo = AuxCConfig::AUX_I2C_ReadCfg.m_ChnCoreInfo.m_AuxSensorCoreInfo;
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo. m_abi_DAC_chn[uc_Chid].m_AuxType= AuxCConfig::AUX_I2C_ReadCfg.m_AuxType;
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo. m_abi_DAC_chn[uc_Chid].m_AuxSubType = AuxCConfig::AUX_I2C_ReadCfg.m_AuxSubType;
// 				Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo. m_abi_DAC_chn[uc_Chid].m_avg_num = ReadCfg.m_AVG_num;
            }
        }
// I2C add by zc 04.20.2018
        if((Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_valid == BEUSED)&&(Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_globe.m_SlaveBoardType !=0))
        {
            for(unsigned char uc_Chid = 0; uc_Chid < 8; uc_Chid ++)
            {
                //  “表像值”特殊处理  硬件问题用软件解决  // 对于一般用户来讲。 容易理解On是1， Off 是0，并且，我们约定辅助在unit内的编号从 低BID板号开始排起。 因为这样比较符合习惯。
                //   S2器件ON为0处理成ON为1               // 因此，请在一开始读板号的时候，做一下处理，使得板号值和拨码的“表像值” 一致。以后就统一用这个“表像值”做板号，来保证一致性。
                uc_UARTCh =uc_Chid +MAX_CH_IN_AUX_UNIT;
                ReadAUXChannelCfg((MAX_BOARD_PER_AUXMCU-1-uc_Bid),uc_UARTCh,&uc_I2CSuccessFlag[uc_Chid]);


                // Aux_DataManager::m_AllAuxBoards[uc_Bid].m_AuxSensor_ChInfo[uc_Chid] = ReadCfg.m_AuxChInfo;
                // unsigned char uc_ch=ReadCfg.m_ChannelNumber;
                //下面是读通道信息。可是结构里也有板的全局信息。。。，这应考虑和并到m_AllAUxBoards,统一管理，避免出错。
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo. m_abi_uart_chn[uc_Chid].m_Populated = AuxCConfig::AUX_I2C_ReadCfg.m_valid;
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo. m_abi_uart_chn[uc_Chid].m_ChnCoreInfo.m_AuxSensorCoreInfo = AuxCConfig::AUX_I2C_ReadCfg.m_ChnCoreInfo.m_AuxSensorCoreInfo;
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo. m_abi_uart_chn[uc_Chid].m_AuxType= AuxCConfig::AUX_I2C_ReadCfg.m_AuxType;
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo. m_abi_uart_chn[uc_Chid].m_AuxSubType = AuxCConfig::AUX_I2C_ReadCfg.m_AuxSubType;	//
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo. m_abi_uart_chn[uc_Chid].m_avg_num = AuxCConfig::AUX_I2C_ReadCfg.m_AVG_num;
            }
        }
    }



    Aux_DataManager::Aux_ObtainGainOffset();  // 对接更新采样校正值

    return uc_Result;
}

unsigned char AuxCConfig::Reg_AUXBoardInfo(void)  //先注册所有的辅助通道板，按照BID顺序建立索引对照表
{
    ST_AUX_BOARD_INFO_SAVE m_Readcfg;
    //     UWord16  uw16_ChCount = 0;
    unsigned char uc_I2CSuccessBoardFlag;

    for(unsigned char uc_i=0; uc_i<8; uc_i++)
    {
        if(AUX_IfLoadBoard(uc_i))
        {
            E2PROM_24LC128::BoardCfgBit|=(1<<uc_i);
            m_Readcfg=ReadAUXBoardCfg(uc_i,&uc_I2CSuccessBoardFlag);
            //  “表像值”特殊处理  硬件问题用软件解决  // 对于一般用户来讲。 容易理解On是1， Off 是0，并且，我们约定辅助在unit内的编号从 低BID板号开始排起。 因为这样比较符合习惯。
            //   S2器件ON为0处理成ON为1               // 因此，请在一开始读板号的时候，做一下处理，使得板号值和拨码的“表像值” 一致。以后就统一用这个“表像值”做板号，来保证一致性。

            if(m_Readcfg.m_valid == BEUSED)	// 首先判断板是否valid，若 valid == 1, 拷贝所有信息到m_AllAuxBoards
            {
                Aux_DataManager::m_AllAuxBoards[MAX_BOARD_PER_AUXMCU - 1 - uc_i].m_BoardInfo.m_abi_globe = m_Readcfg.m_abi_globe;
                Aux_DataManager::m_AllAuxBoards[MAX_BOARD_PER_AUXMCU - 1 - uc_i].m_BoardInfo.m_assemble = m_Readcfg.m_assemble;
                Aux_DataManager::m_AllAuxBoards[MAX_BOARD_PER_AUXMCU - 1 - uc_i].m_BoardInfo.m_valid = m_Readcfg.m_valid;
                Aux_DataManager::m_AllAuxBoards[MAX_BOARD_PER_AUXMCU - 1 - uc_i].m_BoardInfo.m_RSN=m_Readcfg.m_RSN;
                // 			     Aux_DataManager::m_AllAuxBoards[MAX_BOARD_PER_AUXMCU - 1 - uc_i].m_Start_InUnit = m_Readcfg.m_Start_InUnit;
                // 			     Aux_DataManager::m_AllAuxBoards[MAX_BOARD_PER_AUXMCU - 1 - uc_i].m_End_InUnit = m_Readcfg.m_End_InUnit;
            }
        }
        else
            E2PROM_24LC128::BoardCfgBit&=~(1<<uc_i);

    }

    if(E2PROM_24LC128::BoardCfgBit==0)
        return 0;            // 没有找到任何辅助通道板
    else
        return 1;

}

unsigned char AuxCConfig::AUX_IfLoadBoard(unsigned char uc_DeviceAddr)
{
    //   判断板是否接上系统 24LC128 芯片存储数据EEPROM
    //   device address: 跳线地址A0,A1,A2,   Returned value:		TRUE:接上板，FALSE:未接板

    E2PROM_24LC128::WriteBuf[0]='B';   //测试用
    E2PROM_24LC128::WriteBuf[1]='I';
    E2PROM_24LC128::WriteBuf[2]='D';
    E2PROM_24LC128::WriteBuf[3]=uc_DeviceAddr;
    //E2PROM_24LC128::Write24LC128_Page(uc_DeviceAddr,0x02,4);
    E2PROM_24LC128::Write24LC128(uc_DeviceAddr,0x02,4);
    E2PROM_24LC128::Read24LC128(uc_DeviceAddr,0x02,4);
    if(!memcmp(& E2PROM_24LC128::WriteBuf[0],&E2PROM_24LC128::ReadBuf[0],4))
        return 1;
    else
        return 0;

}
UChar8 AuxCConfig::AUX_IfWrite(void)
{
    //  判断缓冲区中是否有数据占用24LC128 芯片存储数据EEPROM
    //  Returned value:		TRUE:表示可使用缓冲区，FALSE:表示不能使用缓冲区
    if(E2PROM_24LC128::uc_EEPROM_OnWriteBuf)
    {
        return 0;  //占用了不能写
    }
    else
    {
        return 1;
    }
}

UChar8 AuxCConfig::AUX_IfRead(void)
{
    //  判断缓冲区中是否有数据占用24LC128 芯片存储数据EEPROM
    //  Returned value:		 1:表示可使用缓冲区，0:表示不能使用缓冲区
    if(E2PROM_24LC128::uc_EEPROM_OnReadBuf)
    {
        return 0;    //占用了不能读
    }
    else
    {
        return 1;
    }
}
void AuxCConfig::ReadAUXChannelCfg(unsigned char uc_DeviceAddr,unsigned char uc_ChannelNo,UChar8 * uc_SuccessFlag)
{
    //static St_AUXChannelCFG m_Readcfg;
		memset(&AUX_I2C_ReadCfg,0,sizeof(St_AUXChannelCFG));
    UWord16 StartAddr=AUXBOARDCHANNELCFGADDR;  // 板虽然不同，但板存的地方是一样的。
    * uc_SuccessFlag = 0;

    if(E2PROM_24LC128::uc_EEPROM_OnReadBuf)
    {
        memset(&AUX_I2C_ReadCfg.m_valid,0,sizeof(St_AUXChannelCFG));
    }
    else
    {
        E2PROM_24LC128::Read24LC128(uc_DeviceAddr,StartAddr+uc_ChannelNo*MAX_I2C_BUFFER,sizeof(St_AUXChannelCFG));

        memcpy(&AUX_I2C_ReadCfg.m_valid,&E2PROM_24LC128::ReadBuf[0],sizeof(St_AUXChannelCFG));
    }
//     return m_Readcfg;
}
UChar8 AuxCConfig::WriteAUXBoardCfg(UChar8 uc_DeviceAddr,ST_AUX_BOARD_INFO * m_AuxBoardInfo)
{
    UWord16 StartAddr=AUXBOARDCFGSRARTADDR;  // 板虽然不同，但板存的地方是一样的 //由原来24字节，变成16字节
    ST_AUX_BOARD_INFO_SAVE	WriteCfg;
    //ST_AUX_BOARD_INFO_SAVE	ReadCfg;

    WriteCfg.m_valid = m_AuxBoardInfo->m_BoardInfo.m_valid;
    WriteCfg.m_assemble = m_AuxBoardInfo->m_BoardInfo.m_assemble;
    WriteCfg.m_abi_globe = m_AuxBoardInfo->m_BoardInfo.m_abi_globe;
    WriteCfg.m_RSN = m_AuxBoardInfo->m_BoardInfo.m_RSN;

    if(E2PROM_24LC128::uc_EEPROM_OnWriteBuf)
        return 0;
    if(!AUX_IfLoadBoard(uc_DeviceAddr))
        return 0;

    E2PROM_24LC128::WriteI2cBuf(&WriteCfg.m_valid,sizeof(ST_AUX_BOARD_INFO_SAVE));


    uc_DeviceAddr = E2PROM_24LC128::Write24LC128(uc_DeviceAddr,StartAddr,sizeof(ST_AUX_BOARD_INFO_SAVE));

    return uc_DeviceAddr; //1;

}
ST_AUX_BOARD_INFO_SAVE AuxCConfig::ReadAUXBoardCfg(UChar8 uc_DeviceAddr,UChar8 * uc_SuccessFlag)
{
    static ST_AUX_BOARD_INFO_SAVE m_Readcfg;
    UWord16 StartAddr=AUXBOARDCFGSRARTADDR;  // 板虽然不同，但板信息存的地方是一样的。
    * uc_SuccessFlag = 0;

    if(E2PROM_24LC128::uc_EEPROM_OnReadBuf)
    {
        memset(&m_Readcfg,0,sizeof(ST_AUX_BOARD_INFO_SAVE));
    }
    else
    {

        //         if(!AUX_IfLoadBoard(uc_DeviceAddr))
        //         {
        //             memset(&m_Readcfg,0,sizeof(ST_AUX_BOARD_INFO_SAVE));
        //             return m_Readcfg;
        //         }

        E2PROM_24LC128::Read24LC128(uc_DeviceAddr,StartAddr,sizeof(ST_AUX_BOARD_INFO_SAVE));
        memcpy(&m_Readcfg.m_valid,&E2PROM_24LC128::ReadBuf[0],sizeof(ST_AUX_BOARD_INFO_SAVE));
        * uc_SuccessFlag = 1;
    }
    return m_Readcfg;
}
UChar8 AuxCConfig::WriteAUXChannelCfg(UChar8 uc_DeviceAddr,UChar8 uc_ChannelNo,St_AUXChannelCFG * m_AuxChannelCfg)
{
    UWord16 StartAddr=AUXBOARDCHANNELCFGADDR;  // 板虽然不同，但板/通道信息存的地方是一样的。
    //	St_AUXChannelCFG ReadCfg;

    if(E2PROM_24LC128::uc_EEPROM_OnWriteBuf)
        return 0;

    if(!AUX_IfLoadBoard(uc_DeviceAddr))
        return 0;

    E2PROM_24LC128::WriteI2cBuf( &m_AuxChannelCfg->m_valid,sizeof(St_AUXChannelCFG));
    uc_DeviceAddr = E2PROM_24LC128::Write24LC128(uc_DeviceAddr,StartAddr+uc_ChannelNo*MAX_I2C_BUFFER,sizeof(St_AUXChannelCFG));
    return uc_DeviceAddr; //1;
}



UChar8 AuxCConfig::WriteAUXBoardRSN(UChar8 uc_DeviceAddr,UWord16 * uw16_RSN)
{
    UWord16 StartAddr=AUXBOARDRSNSRARTADDR;   // 板虽然不同，但板存的地方是一样的。

    if(E2PROM_24LC128::uc_EEPROM_OnWriteBuf)
        return 0;

    if(!AUX_IfLoadBoard(uc_DeviceAddr))
        return 0;

    E2PROM_24LC128::WriteI2cBuf((UChar8 *)(uw16_RSN),sizeof(UWord16));


    uc_DeviceAddr = E2PROM_24LC128::Write24LC128(uc_DeviceAddr,StartAddr,sizeof(UWord16));

    return uc_DeviceAddr;
}

UChar8 AuxCConfig::ReadAUXBoardRSN(UChar8 uc_DeviceAddr,UWord16 * uw16_RSN)
{
    UWord16 StartAddr=AUXBOARDRSNSRARTADDR;   // 板虽然不同，但板存的地方是一样的。

    if(E2PROM_24LC128::uc_EEPROM_OnReadBuf)
    {
        memset(uw16_RSN,0,sizeof(UWord16));
        return 0;
    }
    else
    {
        if(!AUX_IfLoadBoard(uc_DeviceAddr))
            return 0;

        uc_DeviceAddr = E2PROM_24LC128::Read24LC128(uc_DeviceAddr,StartAddr,sizeof(UWord16));
        memcpy(uw16_RSN,&E2PROM_24LC128::ReadBuf[0],sizeof(UWord16));
    }
    return uc_DeviceAddr;
}



void AuxCConfig::WriteContent_ForFRAM(unsigned char * Source, UWord16  u16BaseAddress, unsigned long uw32_DataLength)
{
    // 一次性存入FRAM里面。

    unsigned long uw32_length = uw32_DataLength;
    if(uw32_length > (32000)) // 最大32K存取
    {
        return;
    }

    UWord32  u32CfgRelativeBaseAddress = 0;  //将传递进来的绝对地址转换为相对地址  ： 绝对-绝对=0地址
    UWord32  u32CfgAbsoluteBaseAddress = (unsigned long) (Source); //  绝对地址 = &Address
    UWord32  u32ByteCount = uw32_length;
    UWord32  u32CfgCount = 0;
    UWord16  u16FramAddress = u16BaseAddress;
    unsigned char ucByteContent = 0;
    //    unsigned char u16Byte = 0;
    unsigned char ucBit = 0;

    while(u32CfgCount < u32ByteCount)
    {
        //        u16Byte = (u32CfgRelativeBaseAddress + u32CfgCount) / 8;  // 32K列表中，一个0XFF代表8个字节。 取列表数据组下标[即FramConfigDictionary[X]]。
        ucBit = (u32CfgRelativeBaseAddress + u32CfgCount) % 8;   // 32K列表中，一个0XFF代表8个字节。 取列表数据组中值位域[即0XFF]从0到7。

        ucByteContent = 0xFF;//ChannelInfo_Manager::FramConfigDictionary[u16Byte];  //  取列表数据组中值位域[即0XFF]。
        if ((ucByteContent >> ucBit)&0x01)   //  取列表数据组中值位域[即0XFF]从0到7。
        {
            FM25L256Func::writePage(u16FramAddress,(unsigned char *)(u32CfgAbsoluteBaseAddress + u32CfgCount),1);//单字节写入
            u16FramAddress++; //单字节地址变化
        }
        u32CfgCount++;   //单字节计数器变化
        if (u32CfgCount >= u32ByteCount)    //写操作结束了
        {
            return;
        }
    }

}

void AuxCConfig::ReadContent_ForFRAM(unsigned char * Dest, UWord16  u16BaseAddress, unsigned long uw32_DataLength)
{   // 一次性读出FRAM里面特定内容。
    unsigned long  uw32_length = uw32_DataLength;

    UWord32  u32CfgRelativeBaseAddress = 0;  //将传递进来的绝对地址转换为相对地址  ： 绝对-绝对=0地址
    UWord32  u32CfgAbsoluteBaseAddress = (unsigned long) (Dest); //  绝对地址 = &Address
    UWord32  u32CfgCount = 0;
    UWord16  u16FramAddress = u16BaseAddress;
    unsigned char ucByteContent = 0;
    unsigned char ucBit = 0;

    while(u32CfgCount < uw32_length)
    {
        ucBit = (u32CfgRelativeBaseAddress + u32CfgCount) % 8;   // 32K列表中，一个0XFF代表8个字节。 取列表数据组中值位域[即0XFF]从0到7。

        ucByteContent = 0xFF;//ChannelInfo_Manager::FramConfigDictionary[u16Byte];  //  取列表数据组中值位域[即0XFF]。
        if ((ucByteContent >> ucBit)&0x01)   //  取列表数据组中值位域[即0XFF]从0到7。
        {
            FM25L256Func::readPage(u16FramAddress,(unsigned char *)(u32CfgAbsoluteBaseAddress + u32CfgCount),1); // 一个一个字节读出
            u16FramAddress++; //单字节地址变化
        }
        u32CfgCount++;   //单字节计数器变化
    }
}

