#include "../Entry/includes.h"


//public:
unsigned char   CConfig::bConfigValid;
UInt16          CConfig::Priority = PRIORITYBG;
UInt16          CConfig::TaskID = EEPROM_TASK;
WORD			CConfig::uwOutOf_FRAM_error; //1610142 zyx

//private:
unsigned char   CConfig::bWriting;
unsigned short  CConfig::u16Byte;
unsigned char   CConfig::ucBit;
unsigned short  CConfig::u16ByteCount;
unsigned short  CConfig::u16FramAddress;
unsigned short  CConfig::u16CfgCount;
unsigned long   CConfig::u32CfgRelativeBaseAddress;
unsigned long   CConfig::u32CfgAbsoluteBaseAddress;
St_FramMessage  CConfig::MyMessage;
St_FramMessages CConfig::Messages;
St_FramHeader   CConfig::FramHeader;

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM",zidata = "SRAM"		   // ???????????????	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // ???????????????	   dynamic memory  0XA0000000  0x03E80000
#endif

St_AUXChannelCFG  CConfig::I2C_ReadCfg;
ST_AUX_BOARD_INFO_SAVE   CConfig::BuiltIn_m_Readcfg;

#pragma arm section

//public:
void CConfig::MyInit(void)
{
    FM25L256Func::MyInit();

    Messages.m_u32Start = 0;
    Messages.m_u32Length = 0;
    bWriting = 0;

    // Begin Ԥ���FRAM��ַ�Ƿ����Խ�����  2016.09.20
    unsigned short int  uw16_CFG[MAX_ADDRESS] =
    {   ( FRAM_BASE + sizeof(St_BoardGlobalCFG) + sizeof(St_ChannelCFG) * MAXCHANNELNO ),
        FRAM_NONE_BASE,
        ( FRAM_CANBMS_MAP_BASE + sizeof(ST_CANBMS_MAP) ),
        ( FRAM_PARALLEL_BASE + sizeof(MP_BATCH_GROUP) ),
        ( FRAM_IV_BOARDTYPE + sizeof(IV_BOARDTYPE_CFG) ),
        ( FRAM_PID_BASE + sizeof(St_IV_ChannelPID)*MAXCHANNELNO),
        FRAM_REMAIN_LENGTH
    };
//1610142 zyx
    uwOutOf_FRAM_error = 0;

    if (uw16_CFG[CHANNEL_CFG] > FRAM_NONE_BASE)
        uwOutOf_FRAM_error |= 1<<CHANNEL_CFG;  // Please check FRAM_BASE and size
    if (uw16_CFG[NONE_EMPTY] > FRAM_CANBMS_MAP_BASE)
        uwOutOf_FRAM_error |= 1<<NONE_EMPTY;  // Please check FRAM_NONE_BASE and size
    if ( uw16_CFG[CANBMS_MAP] > FRAM_PARALLEL_BASE)
        uwOutOf_FRAM_error |= 1<<CANBMS_MAP;  // Please check FRAM_CANBMS_MAP_BASE and size
    if ( uw16_CFG[PARALLEL_BATCH] > FRAM_IV_BOARDTYPE)
        uwOutOf_FRAM_error |= 1<<PARALLEL_BATCH; // Please check FRAM_PARALLEL_BASE and size
    if ( uw16_CFG[BOARDTYPE_CFG] > FRAM_PID_BASE)
        uwOutOf_FRAM_error |= 1<<BOARDTYPE_CFG;  // Please check FRAM_IV_BOARDTYPE and size
    if ( uw16_CFG[PID_CFG] > FRAM_AUXMAP_BASE)
        uwOutOf_FRAM_error |= 1<<PID_CFG;  // Please check FRAM_PID_BASE and size
    //if (uw16_CFG[REMAIN_EMPTY] < 1)
    //    uwOutOf_FRAM_error |= 1<<REMAIN_EMPTY;  // Please check FRAM_AUXMAP_BASE and size
//1610142 zyx
    // End Ԥ���FRAM��ַ�Ƿ����Խ�����  2016.09.20

// 	LoadSysConfig();   //˳���߼���ǰ����  hepeiqing
    readConfig(0);  //������MCU����load system config;ֱ��load�����辭����Ϣ����;
}
// unsigned char CConfig::LoadSysConfig(void)
// {
// 	if(FM25L256Func::checkEEPROMBusy())
// 		return 0;        //SysConfig�Ѿ�load��һ���ˣ��������FRAM�����в�������Ҫ������Ϣ����;
// 	else
// 	{
// 		readConfig(0);  //������MCU����load system config;ֱ��load�����辭����Ϣ����;
// 		return 1;
// 	}
// }   //delete by qjm 20171228
void CConfig::DoWork(void)
{
    unsigned char ucByteContent = 0;

#if DEBUG_TASK_STATISTIC == 1
    OS_ServeProcess::Task_Count[COUNT_FRAM] ++;
    CommonFunc::GetTimeTicksUpdate(&OS_ServeProcess::Task_Count_Time[MAX_TASK_COUNT+1]);
#endif
#if THREAD_TASK_TIME_STATISTIC == 1
	Time_Count::thread_time_init(COUNT_FRAM);
#endif

    if (bWriting)         //���ڽ���һ��д����
    {
        u16Byte = ((u32CfgRelativeBaseAddress + u16CfgCount) >>3);
        ucBit = ((u32CfgRelativeBaseAddress + u16CfgCount) & 7);

        ucByteContent = 0xFF;//ChannelInfo_Manager::FramConfigDictionary[u16Byte];
        if ((ucByteContent >> ucBit)&0x01)
        {
            FM25L256Func::writePage(u16FramAddress,(unsigned char *)(u32CfgAbsoluteBaseAddress + u16CfgCount),1);
            u16FramAddress++;
        }
        u16CfgCount++;
        if (u16CfgCount >= u16ByteCount)    //д����������
        {
            FM25L256Func::writePage(8,(unsigned char *)0,1);
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
            if (MyMessage.m_bPID)
            {
                WritePID(MyMessage.m_u16ChannelIdx);
#if REPORT_FRAM_EVENT == 1
                EthernetParse::MPReport_WriteFram_Event(MyMessage, &EthernetParse::m_pReport->m_Report_Write_Fram);
#endif
            }
            else
            {
                u32CfgRelativeBaseAddress = (unsigned long)MyMessage.m_StartAddress - (unsigned long)&ChannelInfo_Manager::BoardCFG;  //�����ݽ����ľ��Ե�ַת��Ϊ��Ե�ַ
                u32CfgAbsoluteBaseAddress = (unsigned long)MyMessage.m_StartAddress;
                u16ByteCount = MyMessage.m_u16Length;
                GetFramAddress();   //�ҵ�config��ַ��Ӧ��fram��ַ
                if (MyMessage.m_bIsWrite)
                {
                    FM25L256Func::writePage(8,(unsigned char *)1,1);
                    bWriting = 1;
                    u16CfgCount = 0;
                }
                else
                {
                    u16CfgCount = 0;
                    //read something to config running struct from fram;
                }
            }
        }
        else
        {
            if((ChannelInfo_Manager::m_st_BoadType.m_btBUILTIN == 1) && (BuiltIn_DataManager::m_WriteFRAM_Flag ==true))
            {
                BuiltIn_DataManager::m_Sample_Flag =true;
                BuiltIn_DataManager::m_WriteFRAM_Flag =false;
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
int CConfig::PopFramMessage(St_FramMessage * myMessage)
{
    unsigned char bEmpty;
    St_FramMessage message;

    if (IS_QUEUE_EMPTY(Messages))
        return -1;   //stand for empty;

    DEQUEUE(Messages,IV_FRAM_MESSAGE_DEPTH,message,bEmpty);

    *myMessage = message;

    return 1;
}
void CConfig::PushFramMessage(St_FramMessage myMessage)
{
    ENQUEUE(Messages,IV_FRAM_MESSAGE_DEPTH,myMessage);
}
//private:
void CConfig::readConfig(unsigned char Bid)
{
    unsigned char ucByteContent = 0;

    ReadContent_ForFRAM((unsigned char *)&FramHeader, 0, sizeof(FramHeader));
    //FM25L256Func::readPage(0,(unsigned char *)&FramHeader,sizeof(FramHeader));

    u16Byte = 0;
    u16FramAddress = FRAM_BASE;
    u32CfgRelativeBaseAddress = 0;
    u32CfgAbsoluteBaseAddress = (unsigned long)&ChannelInfo_Manager::BoardCFG;
    ucBit = 0;

    for(u16CfgCount = 0; u16CfgCount < sizeof(St_BoardCFG); u16CfgCount++)   //?��config?����?��??��???��3?�����?
    {
        u16Byte = (u16CfgCount >>3);
        ucBit = (u16CfgCount &7);

        ucByteContent = 0xFF;//ChannelInfo_Manager::FramConfigDictionary[u16Byte];
        if ((ucByteContent >> ucBit)&0x01)
        {
            FM25L256Func::readPage(u16FramAddress,(unsigned char *)(u32CfgAbsoluteBaseAddress + u16CfgCount),1);
            u16FramAddress++;
        }
    }

    if(ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount > MAXCHANNELNO)
        ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount = MAXCHANNELNO;

    for (unsigned short ChannelIdx = 0; ChannelIdx < ChannelInfo_Manager::BoardCFG.m_Global.m_u16IVChannelCount; ChannelIdx++)
    {
        ReadPID(ChannelIdx);

        for(unsigned char RngIdx = 0; RngIdx < NUM_OF_IV_RANGE_I; RngIdx++)
        {
            St_PID * myPID =	&ChannelInfo_Manager::ChannelPID[ChannelIdx].m_VoltagePID[RngIdx];
            if(  (myPID->m_fKp <= 0 && myPID->m_fKi <= 0 && myPID->m_fKd <= 0)
                    ||(IsFiniteNumber(myPID->m_fKp) == 0)
                    ||(IsFiniteNumber(myPID->m_fKi) == 0)
                    ||(IsFiniteNumber(myPID->m_fKd) == 0)) //����ֵ�������
            {
                myPID->m_fKp = 0;
                myPID->m_fKi = 0.01;
                myPID->m_fKd = 0;
            }
        }
    }
    //#if (SUPPORT_PARALLEL == 1)
    CConfig::ReadContent_ForFRAM(&EthernetParse::m_btPackDataBuffer[0], FRAM_PARALLEL_BASE, sizeof(MP_BATCH_GROUP));
    EthernetParse::MPBatchGrouping((MP_BATCH_GROUP *)(&EthernetParse::m_btPackDataBuffer[0]),false);
    //#endif

    //#if (USE_AS_AUXMCU == 0  && SUPPORT_CAN_BMS==1)
#if (USE_AS_AUXMCU == 0 )
    CConfig::ReadContent_ForFRAM((unsigned char *)&ChannelInfo_Manager::m_CANMapInfo.m_AllChnNum, FRAM_CANBMS_MAP_BASE, sizeof(ST_CANBMS_MAP));
    CConfig::ReadContent_ForFRAM((unsigned char *)&ChannelInfo_Manager::m_st_BoadType, FRAM_IV_BOARDTYPE, sizeof(IV_BOARDTYPE_CFG));
    CConfig::IV_BoardType_OldTransformToNew();
    CANBusParse::CheckCanBmsMap();
    MultiMcuComm::Scan_IV_CANBMSMAP();
#endif

//#if SUPPORT_AUX == 1
    Channel_MapAuxInfo::ReadAuxMap_ForFRAM();
//#endif
}
void CConfig::GetFramAddress(void)
{
    unsigned long u32Count;
    unsigned char ucByteContent = 0;

    u16FramAddress = FRAM_BASE;
    for(u32Count = 0; u32Count < u32CfgRelativeBaseAddress; u32Count++)
    {
        u16Byte = (u32Count >>3);
        ucBit = (u32Count &7);

        ucByteContent = 0xFF;//ChannelInfo_Manager::FramConfigDictionary[u16Byte];
        if ((ucByteContent >> ucBit)&0x01)
            u16FramAddress++;
    }
}
void CConfig::WritePID(unsigned short u16_ChannelIdx)
{
    if(u16_ChannelIdx >= MAXCHANNELNO)
        return;

    u16FramAddress = FRAM_PID_BASE + sizeof(St_IV_ChannelPID)*u16_ChannelIdx;
    u32CfgAbsoluteBaseAddress = MyMessage.m_StartAddress;

    WriteContent_ForFRAM((unsigned char *)u32CfgAbsoluteBaseAddress, u16FramAddress, sizeof(St_IV_ChannelPID));
    //FM25L256Func::writePage(u16FramAddress,(unsigned char *)u32CfgAbsoluteBaseAddress,(unsigned char)(sizeof(St_IV_ChannelPID)));
}
void CConfig::ReadPID(unsigned short u16_ChannelIdx)
{
    if(u16_ChannelIdx >= MAXCHANNELNO)
        return;

    u16FramAddress = FRAM_PID_BASE + sizeof(St_IV_ChannelPID)*u16_ChannelIdx;

    ReadContent_ForFRAM((unsigned char *)&ChannelInfo_Manager::ChannelPID[u16_ChannelIdx], u16FramAddress, sizeof(St_IV_ChannelPID));
    //FM25L256Func::readPage(u16FramAddress,(unsigned char *)&ChannelInfo_Manager::ChannelPID[u16_ChannelIdx],(unsigned char)(sizeof(St_IV_ChannelPID)));
}



void CConfig::WriteContent_ForFRAM(unsigned char * Source, UWord16  u16BaseAddress, unsigned long uw32_DataLength)
{
    // һ���Դ���FRAM���档

    unsigned long uw32_length = uw32_DataLength;
    if(uw32_length > (32000)) // ���32K��ȡ
    {
        return;
    }

    UWord32  u32CfgRelativeBaseAddress = 0;  //�����ݽ����ľ��Ե�ַת��Ϊ��Ե�ַ  �� ����-����=0��ַ
    UWord32  u32CfgAbsoluteBaseAddress = (unsigned long) (Source); //  ���Ե�ַ = &Address
    UWord32  u32ByteCount = uw32_length;
    UWord32  u32CfgCount = 0;
    UWord16  u16FramAddress = u16BaseAddress;
    unsigned char ucByteContent = 0;
    //    unsigned char u16Byte = 0;
    unsigned char ucBit = 0;

    while(u32CfgCount < u32ByteCount)
    {
        //        u16Byte = (u32CfgRelativeBaseAddress + u32CfgCount) / 8;  // 32K�б��У�һ��0XFF����8���ֽڡ� ȡ�б��������±�[��FramConfigDictionary[X]]��
        ucBit = (u32CfgRelativeBaseAddress + u32CfgCount) % 8;   // 32K�б��У�һ��0XFF����8���ֽڡ� ȡ�б���������ֵλ��[��0XFF]��0��7��

        ucByteContent = 0xFF;//ChannelInfo_Manager::FramConfigDictionary[u16Byte];  //  ȡ�б���������ֵλ��[��0XFF]��
        if ((ucByteContent >> ucBit)&0x01)   //  ȡ�б���������ֵλ��[��0XFF]��0��7��
        {
            FM25L256Func::writePage(u16FramAddress,(unsigned char *)(u32CfgAbsoluteBaseAddress + u32CfgCount),1);//���ֽ�д��
            u16FramAddress++; //���ֽڵ�ַ�仯
        }
        u32CfgCount++;   //���ֽڼ������仯
        if (u32CfgCount >= u32ByteCount)    //д����������
        {
            return;
        }
    }

}

void CConfig::ReadContent_ForFRAM(unsigned char * Dest, UWord16  u16BaseAddress, unsigned long uw32_DataLength)
{
    // һ���Զ���FRAM�����ض����ݡ�
    unsigned long  uw32_length = uw32_DataLength;


    UWord32  u32CfgRelativeBaseAddress = 0;  //�����ݽ����ľ��Ե�ַת��Ϊ��Ե�ַ  �� ����-����=0��ַ
    UWord32  u32CfgAbsoluteBaseAddress = (unsigned long) (Dest); //  ���Ե�ַ = &Address
    UWord32  u32CfgCount = 0;
    UWord16  u16FramAddress = u16BaseAddress;
    unsigned char ucByteContent = 0;
    //    unsigned char u16Byte = 0;
    unsigned char ucBit = 0;

    while(u32CfgCount < uw32_length)
    {
        //        u16Byte = (u32CfgRelativeBaseAddress + u32CfgCount) / 8;  // 32K�б��У�һ��0XFF����8���ֽڡ� ȡ�б��������±�[��FramConfigDictionary[X]]��
        ucBit = (u32CfgRelativeBaseAddress + u32CfgCount) % 8;   // 32K�б��У�һ��0XFF����8���ֽڡ� ȡ�б���������ֵλ��[��0XFF]��0��7��

        ucByteContent = 0xFF;//ChannelInfo_Manager::FramConfigDictionary[u16Byte];  //  ȡ�б���������ֵλ��[��0XFF]��
        if ((ucByteContent >> ucBit)&0x01)   //  ȡ�б���������ֵλ��[��0XFF]��0��7��
        {
            FM25L256Func::readPage(u16FramAddress,(unsigned char *)(u32CfgAbsoluteBaseAddress + u32CfgCount),1); // һ��һ���ֽڶ���
            u16FramAddress++; //���ֽڵ�ַ�仯
        }
        u32CfgCount++;   //���ֽڼ������仯
    }
}

void CConfig::IV_BoardType_OldTransformToNew(void)
{
    if(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType<40)
    {
        switch(ChannelInfo_Manager::m_st_BoadType.m_u16BoardType)
        {
        case BOARDTYPE_OLD_TESLA_PWM:
            ChannelInfo_Manager::m_st_BoadType.m_u16BoardType = BOARDTYPE_TESLA_PWM;
            ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion = BOARDTYPE_VER_A;
            break;
        case BOARDTYPE_OLD_PWM:
            ChannelInfo_Manager::m_st_BoadType.m_u16BoardType = BOARDTYPE_PWM;
            break;
        case BOARDTYPE_OLD_LBT21024:
            ChannelInfo_Manager::m_st_BoadType.m_u16BoardType = BOARDTYPE_LBT21024;
            break;
        case BOARDTYPE_OLD_LBT2108:
            ChannelInfo_Manager::m_st_BoadType.m_u16BoardType = BOARDTYPE_LBT21084;
            switch(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion)
            {
            case BOARDTYPE_VER_E:
                ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion = BOARDTYPE_VER_C;
                ChannelInfo_Manager::m_st_BoadType.m_PowerLevel = POWER_LEVEL_LC;
                break;
            case BOARDTYPE_VER_HC:
                ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion = BOARDTYPE_VER_C;
                ChannelInfo_Manager::m_st_BoadType.m_PowerLevel = POWER_LEVEL_HC;
                break;
            }
            break;
        case BOARDTYPE_OLD_LBT22013:
            ChannelInfo_Manager::m_st_BoadType.m_u16BoardType = BOARDTYPE_LBT22013;
            break;
        case BOARDTYPE_OLD_LBT20083:
            ChannelInfo_Manager::m_st_BoadType.m_u16BoardType = BOARDTYPE_LBT20083;
            ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion = BOARDTYPE_VER_A;
            break;
        case BOARDTYPE_OLD_HPT200:
            ChannelInfo_Manager::m_st_BoadType.m_u16BoardType = BOARDTYPE_HPT200;
            ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion = BOARDTYPE_VER_A;
            break;
        case BOARDTYPE_OLD_LBT22024:
            ChannelInfo_Manager::m_st_BoadType.m_u16BoardType = BOARDTYPE_LBT22023;
            ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion = BOARDTYPE_VER_A;
            break;
        case BOARDTYPE_OLD_MSTAT21044:
            ChannelInfo_Manager::m_st_BoadType.m_u16BoardType = BOARDTYPE_MSTAT21044;
            break;
        case BOARDTYPE_OLD_LBT22013FR:
            ChannelInfo_Manager::m_st_BoadType.m_u16BoardType = BOARDTYPE_LBT22013;
            ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion = BOARDTYPE_VER_B;
            break;
        case BOARDTYPE_OLD_HS21044:
            ChannelInfo_Manager::m_st_BoadType.m_u16BoardType = BOARDTYPE_HS21044;
            ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion = BOARDTYPE_VER_A;
            break;
        case BOARDTYPE_OLD_LBT22044:
            ChannelInfo_Manager::m_st_BoadType.m_u16BoardType = BOARDTYPE_LBT22043;
            break;
        case BOARDTYPE_OLD_HPM4:
            ChannelInfo_Manager::m_st_BoadType.m_u16BoardType = BOARDTYPE_HPM4;
            ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion = BOARDTYPE_VER_C;
            break;
        case BOARDTYPE_OLD_LBT21084VB:
            ChannelInfo_Manager::m_st_BoadType.m_u16BoardType = BOARDTYPE_LBT21084;
            switch(ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion)
            {
            case BOARDTYPE_VER_A:
                ChannelInfo_Manager::m_st_BoadType.m_PowerLevel = POWER_LEVEL_LC;
                break;
            case BOARDTYPE_VER_HC:
                ChannelInfo_Manager::m_st_BoadType.m_PowerLevel = POWER_LEVEL_HC;
                break;
            }
            ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion = BOARDTYPE_VER_D;
            break;
        case BOARDTYPE_OLD_LBT21024FR:
            ChannelInfo_Manager::m_st_BoadType.m_u16BoardType = BOARDTYPE_LBT21024;
            ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion = BOARDTYPE_VER_C;
            break;
        case BOARDTYPE_OLD_LBT20083VB:
            ChannelInfo_Manager::m_st_BoadType.m_u16BoardType = BOARDTYPE_LBT20083;
            ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion = BOARDTYPE_VER_B;
            break;
        case BOARDTYPE_OLD_LBT21024FRVB:
            ChannelInfo_Manager::m_st_BoadType.m_u16BoardType = BOARDTYPE_LBT21024;
            ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion = BOARDTYPE_VER_D;
            break;
        case BOARDTYPE_OLD_LBT21162:
            ChannelInfo_Manager::m_st_BoadType.m_u16BoardType = BOARDTYPE_LBT21162;
            break;
        case BOARDTYPE_OLD_LBT21044:
            ChannelInfo_Manager::m_st_BoadType.m_u16BoardType = BOARDTYPE_LBT21044;
            break;
        case BOARDTYPE_OLD_LBT21014FRVB:
            ChannelInfo_Manager::m_st_BoadType.m_u16BoardType = BOARDTYPE_LBT21014;
            ChannelInfo_Manager::m_st_BoadType.m_btBoardTypeVersion = BOARDTYPE_VER_D;
            break;
        case BOARDTYPE_OLD_LBT20084:
            ChannelInfo_Manager::m_st_BoadType.m_u16BoardType = BOARDTYPE_LBT20084;
            break;
        case BOARDTYPE_OLD_LBT20162:
            ChannelInfo_Manager::m_st_BoadType.m_u16BoardType = BOARDTYPE_LBT20162;
            break;
        default:
            break;
        }
    }
}
//#if SUPPORT_AUX == 1
//void CConfig::WriteAuxMap(unsigned long uw32_Length)
//{
//	//AuxMapParse();  //dirui add 0313

//    UWord32  u32CfgRelativeBaseAddress = 0;  //�����ݽ����ľ��Ե�ַת��Ϊ��Ե�ַ  �� ����-����=0��ַ
//    UWord32  u32CfgAbsoluteBaseAddress = (unsigned long) (&EthernetParse::m_btPackDataBuffer[0]); //  ���Ե�ַ = &Aux_DataManager::m_AuxUnitCFG
//    //UWord32  u32ByteCount = sizeof(MP_IVUnit4AuxMap);
//    UWord32  u32ByteCount = uw32_Length;
//    UWord32  u32CfgCount = 0;
//    u16FramAddress = FRAM_PID_BASE + sizeof(St_IV_ChannelPID)*MAXCHANNELNO;
//    unsigned char ucByteContent;
//
//    while(u32CfgCount < u32ByteCount)
//    {
//        u16Byte = (u32CfgRelativeBaseAddress + u32CfgCount) / 8;  // 32K�б��У�һ��0XFF����8���ֽڡ� ȡ�б��������±�[��FramConfigDictionary[X]]��
//        ucBit = (u32CfgRelativeBaseAddress + u32CfgCount) % 8;   // 32K�б��У�һ��0XFF����8���ֽڡ� ȡ�б���������ֵλ��[��0XFF]��0��7��

//        ucByteContent = 0xFF;//ChannelInfo_Manager::FramConfigDictionary[u16Byte];  //  ȡ�б���������ֵλ��[��0XFF]��
//        if ((ucByteContent >> ucBit)&0x01)   //  ȡ�б���������ֵλ��[��0XFF]��0��7��
//        {
//          FM25L256Func::writePage(u16FramAddress,(unsigned char *)(u32CfgAbsoluteBaseAddress + u32CfgCount),1);//���ֽ�д��
//          u16FramAddress++; //���ֽڵ�ַ�仯
//        }
//        u32CfgCount++;   //���ֽڼ������仯
//        if (u32CfgCount >= u32ByteCount)    //д����������
//        {
//          #if REPORT_FRAM_EVENT == 1
//          EthernetParse::MPReport_WriteFram_Event(MyMessage, &EthernetParse::m_pReport->m_Report_Write_Fram);
//          #endif
//          return;
//        }
//    }
//}
//#endif
//  ����ΪI2C��EPPROM����
unsigned char CConfig::Load_I2C_Config(void)
{
    unsigned char uc_Result;
    //static St_AUXChannelCFG  ReadCfg;

		memset(&CConfig::I2C_ReadCfg,0,sizeof(St_AUXChannelCFG));
    memset(&BuiltIn_DataManager::Aux_BuiltIn_BoardInfo,0,sizeof(ST_BUILTIN_BOARD_INFO_IN_EEPROM)) ;    // ���

    uc_Result = CConfig::Reg_BUILTINBoardInfo(0);  //������Ϣ
// 	 unsigned char uc_I2CSuccessFlag[MAX_CH_IN_AUX_UNIT];
    if(BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_valid == 1)
    {
        for(unsigned char uc_Chid = 0; uc_Chid < BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_Max_Channels; uc_Chid ++)
        {
            //  ������ֵ�����⴦��  Ӳ��������������  // ����һ���û������� �������On��1�� Off ��0�����ң�����Լ��������unit�ڵı�Ŵ� ��BID��ſ�ʼ���� ��Ϊ�����ȽϷ���ϰ�ߡ�
            //   S2����ONΪ0�����ONΪ1               // ��ˣ�����һ��ʼ����ŵ�ʱ����һ�´���ʹ�ð��ֵ�Ͳ���ġ�����ֵ�� һ�¡��Ժ��ͳһ�����������ֵ������ţ�����֤һ���ԡ�

// 			 	ReadCfg = ReadBuiltInChannelCfg(0,uc_Chid,&uc_I2CSuccessFlag[uc_Chid]);
            ReadBuiltInChannelCfg(0,uc_Chid);

            // Aux_DataManager::m_AllAuxBoards[uc_Bid].m_AuxSensor_ChInfo[uc_Chid] = ReadCfg.m_AuxChInfo;
            // unsigned char uc_ch=ReadCfg.m_ChannelNumber;
            //�����Ƕ�ͨ����Ϣ�����ǽṹ��Ҳ�а��ȫ����Ϣ����������Ӧ���ǺͲ���m_AllAUxBoards,ͳһ�����������
            BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_Chid].m_Populated = CConfig::I2C_ReadCfg.m_valid;
            BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_Chid].m_ChnCoreInfo.m_AuxSensorCoreInfo = CConfig::I2C_ReadCfg.m_ChnCoreInfo.m_AuxSensorCoreInfo;
            BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_Chid].m_AuxType= CConfig::I2C_ReadCfg.m_AuxType;
            BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_Chid].m_AuxSubType =CConfig::I2C_ReadCfg.m_AuxSubType;
            BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_Chid].m_avg_num = CConfig::I2C_ReadCfg.m_AVG_num;
        }

    }
// 	 	Aux_DataManager::Aux_ObtainGainOffset();  // �ⲿ����Ҫ���� 2017 �뾲��
    return uc_Result;

}

// void  CConfig::BuiltIn_Board_Init(void)
// {
//    unsigned char uc_Result = 0;
// 	 St_AUXChannelCFG  ReadCfg;
//
//    memset(&BuiltIn_DataManager::Aux_BuiltIn_BoardInfo,0,sizeof(ST_BUILTIN_BOARD_INFO_IN_EEPROM)) ;    // ���
//    memset(&ReadCfg,0,sizeof(St_AUXChannelCFG)) ;
//
//    BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_BoardType = 	Aux_BulitIn_16V_409386;
// 	 BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_SlaveBoardType = 	Aux_BI_VL_16V_409932;
// 	 BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_Total_Channels =16;
// 	 BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_assemble.m_ConsistentOrder=1;
// 	 BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_assemble.m_WithSlave=0;
// 	 BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_assemble.m_SlaveGoesFirst=0;
//
// 	 BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_valid =1;
//
//    ReadCfg.m_valid=1;
// 	 ReadCfg.m_AuxType =	AUX_VOLTAGE;
// 	 ReadCfg.m_AuxSubType = Regular_2ndV;
// 	 ReadCfg.m_AVG_num =1;
// 	 ReadCfg.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeH =10;
// 	 ReadCfg.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_RangeL=-10;
// 	 ReadCfg.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Gain =1;
// 	 ReadCfg.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_ADC_Offset=0;
// 	 ReadCfg.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Gain=1;
// 	 ReadCfg.m_ChnCoreInfo.m_AuxSensorCoreInfo.m_PHY_Offset=0;
//
// 	  for(unsigned char uc_Chid = 0; uc_Chid < BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe.m_Total_Channels; uc_Chid ++)
// 		{
//         BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_Chid].m_Populated = ReadCfg.m_valid;
// 				BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_Chid].m_ChnCoreInfo.m_AuxSensorCoreInfo = ReadCfg.m_ChnCoreInfo.m_AuxSensorCoreInfo;
// 				BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_Chid].m_AuxType= ReadCfg.m_AuxType;
// 				BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_Chid].m_AuxSubType =ReadCfg.m_AuxSubType;
// 				BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_chn[uc_Chid].m_avg_num = ReadCfg.m_AVG_num;
//     }


// }
unsigned char CConfig::AUX_IfLoadBoard(unsigned char uc_DeviceAddr)
{
    //   �жϰ��Ƿ����ϵͳ 24LC128 оƬ�洢����EEPROM
    //   device address: ���ߵ�ַA0,A1,A2,   Returned value:		TRUE:���ϰ壬FALSE:δ�Ӱ�

    E2PROM_24LC128::WriteBuf[0]='B';   //������
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
// ST_AUX_BOARD_INFO_SAVE CConfig::ReadBuiltInBoardCfg(UChar8 uc_DeviceAddr,UChar8 * uc_SuccessFlag)
ST_AUX_BOARD_INFO_SAVE CConfig::ReadBuiltInBoardCfg(UChar8 uc_DeviceAddr)
{
//     static ST_AUX_BOARD_INFO_SAVE   m_Readcfg;
    UWord16 StartAddr=AUXBOARDCFGSRARTADDR;  // ����Ȼ��ͬ��������Ϣ��ĵط���һ���ġ�
// 	   * uc_SuccessFlag = 0;
    if(E2PROM_24LC128::uc_EEPROM_OnReadBuf)
    {
        memset(&CConfig::BuiltIn_m_Readcfg,0,sizeof(ST_AUX_BOARD_INFO_SAVE));
    }
    else
    {
        E2PROM_24LC128::Read24LC128(uc_DeviceAddr,StartAddr,sizeof(ST_AUX_BOARD_INFO_SAVE));
        memcpy(&CConfig::BuiltIn_m_Readcfg.m_valid,&E2PROM_24LC128::ReadBuf[0],sizeof(ST_AUX_BOARD_INFO_SAVE));
// 		      * uc_SuccessFlag = 1;
    }
    return CConfig::BuiltIn_m_Readcfg;
}

unsigned char CConfig::Reg_BUILTINBoardInfo(unsigned char BoardID)
{
    ST_AUX_BOARD_INFO_SAVE m_Readcfg;
//    unsigned char uc_I2CSuccessBoardFlag;
    if(AUX_IfLoadBoard(BoardID))
    {
//        m_Readcfg = ReadBuiltInBoardCfg(0,&uc_I2CSuccessBoardFlag);
        m_Readcfg = ReadBuiltInBoardCfg(0);
        if(m_Readcfg.m_valid == 1)	// �����жϰ��Ƿ�valid���� valid == 1, ����������Ϣ
        {
            BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_abi_globe = m_Readcfg.m_abi_globe;
            BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_assemble = m_Readcfg.m_assemble;
            BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_valid = m_Readcfg.m_valid;
            BuiltIn_DataManager::Aux_BuiltIn_BoardInfo.m_RSN=m_Readcfg.m_RSN;
            return 1;
        }
        else
            return 0;
    }
    else
    {
        return 0;
    }

}
// St_AUXChannelCFG CConfig::ReadBuiltInChannelCfg(unsigned char uc_DeviceAddr,unsigned char uc_ChannelNo, UChar8 * uc_SuccessFlag)
void CConfig::ReadBuiltInChannelCfg(unsigned char uc_DeviceAddr,unsigned char uc_ChannelNo)
{
    //static St_AUXChannelCFG m_Readcfg;
    UWord16 StartAddr=AUXBOARDCHANNELCFGADDR;  // ����Ȼ��ͬ�������ĵط���һ���ġ�
// 	   * uc_SuccessFlag = 0;

    if(E2PROM_24LC128::uc_EEPROM_OnReadBuf)
    {
        memset(&CConfig::I2C_ReadCfg.m_valid,0,sizeof(St_AUXChannelCFG));
    }
    else
    {
        E2PROM_24LC128::Read24LC128(uc_DeviceAddr,StartAddr+uc_ChannelNo*MAX_I2C_BUFFER,sizeof(St_AUXChannelCFG));

        memcpy(&CConfig::I2C_ReadCfg.m_valid,&E2PROM_24LC128::ReadBuf[0],sizeof(St_AUXChannelCFG));
    }
//     return m_Readcfg;
}
unsigned char CConfig::WriteBuiltInBoardCfg(UChar8 uc_DeviceAddr,ST_BUILTIN_BOARD_INFO_IN_EEPROM * m_AuxBoardInfo)
{
    UWord16 StartAddr=AUXBOARDCFGSRARTADDR;  // ����Ȼ��ͬ�������ĵط���һ���� //��ԭ��24�ֽڣ����16�ֽ�
    ST_AUX_BOARD_INFO_SAVE	WriteCfg;

    WriteCfg.m_valid = m_AuxBoardInfo->m_valid;
    WriteCfg.m_assemble = m_AuxBoardInfo->m_assemble;
    WriteCfg.m_abi_globe = m_AuxBoardInfo->m_abi_globe;
    WriteCfg.m_RSN = m_AuxBoardInfo->m_RSN;

    if(E2PROM_24LC128::uc_EEPROM_OnWriteBuf)
        return 0;
    if(!AUX_IfLoadBoard(uc_DeviceAddr))
        return 0;

    E2PROM_24LC128::WriteI2cBuf(&WriteCfg.m_valid,sizeof(ST_AUX_BOARD_INFO_SAVE));


    uc_DeviceAddr = E2PROM_24LC128::Write24LC128(uc_DeviceAddr,StartAddr,sizeof(ST_AUX_BOARD_INFO_SAVE));

    return uc_DeviceAddr; //1;

}

unsigned char CConfig::WriteBuiltInChannelCfg(UChar8 uc_DeviceAddr,UChar8 uc_ChannelNo,St_AUXChannelCFG * m_AuxChannelCfg)
{
    UWord16 StartAddr=AUXBOARDCHANNELCFGADDR;  // ����Ȼ��ͬ������/ͨ����Ϣ��ĵط���һ���ġ�
    //	St_AUXChannelCFG ReadCfg;

    if(E2PROM_24LC128::uc_EEPROM_OnWriteBuf)
        return 0;

    if(!AUX_IfLoadBoard(uc_DeviceAddr))
        return 0;

    E2PROM_24LC128::WriteI2cBuf( &m_AuxChannelCfg->m_valid,sizeof(St_AUXChannelCFG));
    uc_DeviceAddr = E2PROM_24LC128::Write24LC128(uc_DeviceAddr,StartAddr+uc_ChannelNo*MAX_I2C_BUFFER,sizeof(St_AUXChannelCFG));
    return uc_DeviceAddr; //1;
}
