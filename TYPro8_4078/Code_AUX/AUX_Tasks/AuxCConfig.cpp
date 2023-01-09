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

    // Begin Ԥ���FRAM��ַ�Ƿ����Խ�����  2016.09.20
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
    // End Ԥ���FRAM��ַ�Ƿ����Խ�����  2016.09.20

    return 1;
}
unsigned char AuxCConfig::LoadSysConfig(void)
{
    unsigned char uc_Result = 0;
// 	if(FM25L256Func::checkEEPROMBusy())
// 		return 0;        //SysConfig�Ѿ�load��һ���ˣ��������FRAM�����в�������Ҫ������Ϣ����;
// 	else
// 	{
    readConfig(0);  //������MCU����load system config;ֱ��load�����辭����Ϣ����;
    Aux_DataManager::Aux_GetMapfromPC( &uc_Result);    // ת��PCMap���Ա����
// 		for(unsigned char i=0;i<128;i++)
// 		{
// 			memset(&EthernetParse::m_btPackDataBuffer[2071+i*6],0,sizeof(St_DOWNLOAD_MAP_FROM_PC));     //�建����
// 		}
    // 	Aux_DataManager::Aux_TranslateMapFromPC();  // add by qjm 2015/5/18

    uc_Result = Load_I2C_Config();  //��ȡE2�еİ���Ϣ��ͨ����Ϣ
    ///////////////////////////////////////////////////////////////
    //�ֶ�д���ذ�FRAM��Ϣ, Yifei
    //
    ///////////////////////////////////////////////////////////////
    /*
    for (unsigned char i = 0;i<MAX_BOARD_PER_AUXMCU;i++)
    {   // ����һ�ξɵ���Ϣ,������λ��д�幤�߼�����ʾ����Ϣ,û���и��²���.
    memset(&Aux_DataManager::m_AllAuxBoards[i],0,sizeof(ST_AUX_BOARD_INFO));
    // Aux_DataManager::m_AllAuxBoards[i].m_BoardInfo.m_BType.m_AuxType = NUM_OF_AUX_CHAN_TYPE_NULL;
    }

    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_valid =1;						//ͨ����Ч

    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_BID = 0;	//����
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_BoardType = Aux_BC_2ndV_508746;	//��������
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_AuxOtherType = 0;
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_Total_Channels = 60;   //ʵ����Ч��ͨ�������������������data  group  index
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_Max_Channels = 96;    //�ø�������֧�ֵ����ͨ�������������Ծ�����
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_Total_ADC = 0;       //ADC ����
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_Total_CANBMS = 0;  //CANBMSͨ�������� 1 or 0
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_Total_UART = 0;    //UART ����
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_Total_DI = 0;       // DI ����
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_Total_DO = 0;      //DO ����
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_globe.m_Total_I2C = 0;     //i2c
    Aux_DataManager::m_AllAuxBoards[0].m_Start_InUnit = 0;
    Aux_DataManager::m_AllAuxBoards[0].m_End_InUnit = 0;
    for(int k=0;k<60;k++)
    {
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_chn[k].m_Populated  = 1;
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_chn[k].m_AuxType = AUX_VOLTAGE;   //�������ͣ� �������¶ȣ���ѹ������ѹ����pH,
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_chn[k].m_AuxSubType = BatteryPack_2ndV;  //�����Ĵμ����ͣ�Ҳ���Ǵ��������ͣ�������T��K��J �ȵ�ż�� Pt100�ȵ��衣
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_chn[k].m_AuxSensorCoreInfo.m_PHY_RangeL = 0;	//����ת�����ݵĴ�������ص���Ϣ
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_chn[k].m_AuxSensorCoreInfo.m_PHY_RangeH = 5;
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_chn[k].m_AuxSensorCoreInfo.m_ADC_Gain = 1;
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_chn[k].m_AuxSensorCoreInfo.m_ADC_Offset = 0;
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_chn[k].m_AuxSensorCoreInfo.m_PHY_Gain = 1;
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_chn[k].m_AuxSensorCoreInfo.m_PHY_Offset = 0;
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_chn[k].m_avg_num = 24;  //ƽ���˲������ݸ���
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_abi_chn[k].m_Controllable = 0 ;   //��ֻ���Ĵ����������ǿ��Կ��Ƶ�ִ�����������¿��䣬�����������ȡ�
    }
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_assemble.m_ConsistentOrder = 0;    //�ð����ͨ�����Ƿ������ 1��>8 ��ô�ŵ�
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_assemble.m_SlaveGoesFirst = 0;  // �ð��ȡ���Ƿ��ڸ�����ǰ��ȡ��
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_assemble.m_WithSlave = 1;      // �ð��Ƿ���и���
    Aux_DataManager::m_AllAuxBoards[0].m_BoardInfo.m_RSN = 5678;     //����AUXMC�е�ע����Ϣ
    */
    /***************************************************************************************************************************************************************/

    // for(int m = 1;m<8;m++)
    // {
    // Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_valid =0;						//ͨ����Ч
    // }
    /*

    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_globe.m_BID = 1;	//����
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_globe.m_BoardType = Aux_BC_2ndV_508746;	//��������
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_globe.m_AuxOtherType = 0;
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_globe.m_Total_Channels = 60;   //ʵ����Ч��ͨ�������������������data  group  index
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_globe.m_Max_Channels = 96;    //�ø�������֧�ֵ����ͨ�������������Ծ�����
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_globe.m_Total_ADC = 0;       //ADC ����
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_globe.m_Total_CANBMS = 0;  //CANBMSͨ�������� 1 or 0
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_globe.m_Total_UART = 0;    //UART ����
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_globe.m_Total_DI = 0;       // DI ����
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_globe.m_Total_DO = 0;      //DO ����
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_globe.m_Total_I2C = 0;     //i2c
    for(int k=0;k<60;k++)
    {
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_chn[k].m_Populated  = 0;
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_chn[k].m_AuxType = AUX_BATTERYCELLS;   //�������ͣ� �������¶ȣ���ѹ������ѹ����pH,
    //Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_globe.m_abi_chn[k].m_AuxSubType;  //�����Ĵμ����ͣ�Ҳ���Ǵ��������ͣ�������T��K��J �ȵ�ż�� Pt100�ȵ��衣
    //Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_chn[k].m_AuxSensorCoreInfo;  //����ת�����ݵĴ�������ص���Ϣ
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_chn[k].m_avg_num = 64;  //ƽ���˲������ݸ���
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_abi_chn[k].m_Controllable = 0 ;   //��ֻ���Ĵ����������ǿ��Կ��Ƶ�ִ�����������¿��䣬�����������ȡ�
    }
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_assemble.m_ConsistentOrder = 0;    //�ð����ͨ�����Ƿ������ 1��>8 ��ô�ŵ�
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_assemble.m_SlaveGoesFirst = 0;  // �ð��ȡ���Ƿ��ڸ�����ǰ��ȡ��
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_assemble.m_WithSlave = 0;      // �ð��Ƿ���и���
    Aux_DataManager::m_AllAuxBoards[m].m_BoardInfo.m_RSN = 1234;     //����AUXMC�е�ע����Ϣ
    }
    */

    ///////////////////////////////////////////////////////////////
    //�ֶ�д���ذ�FRAM��Ϣ, Yifei
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

    if (bWriting)         //���ڽ���һ��д����
    {
        u16Byte = (u32CfgRelativeBaseAddress + u32CfgCount) / 8;  // 32K�б��У�һ��0XFF����8���ֽڡ� ȡ�б��������±�[��FramConfigDictionary[X]]��
        ucBit = (u32CfgRelativeBaseAddress + u32CfgCount) % 8;   // 32K�б��У�һ��0XFF����8���ֽڡ� ȡ�б���������ֵλ��[��0XFF]��0��7��

        ucByteContent = Aux_DataManager::FramConfigDictionary[u16Byte];  //  ȡ�б���������ֵλ��[��0XFF]��
        if ((ucByteContent >> ucBit)&0x01)   //  ȡ�б���������ֵλ��[��0XFF]��0��7��
        {
            FM25L256Func::writePage(u16FramAddress,(unsigned char *)(u32CfgAbsoluteBaseAddress + u32CfgCount),1);//���ֽ�д��
            u16FramAddress++; //���ֽڵ�ַ�仯
        }
        u32CfgCount++;   //���ֽڼ������仯
        if (u32CfgCount >= u32ByteCount)    //д����������
        {
            FM25L256Func::writePage(8,(unsigned char *)0,1);  // �ڴ洢������ǰ�洦Ԥ����128�ֽڡ�����Ϊдһ��־����
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
            u32CfgRelativeBaseAddress = (unsigned long)MyMessage.m_StartAddress - (unsigned long)MyMessage.m_StartAddress;  //�����ݽ����ľ��Ե�ַת��Ϊ��Ե�ַ  �� ����-����=0��ַ
            u32CfgAbsoluteBaseAddress = (unsigned long)MyMessage.m_StartAddress; //  ���Ե�ַ = &Aux_DataManager::m_AuxUnitCFG
            u32ByteCount = MyMessage.m_u16Length;
            GetFramAddress();   //�ҵ�config��ַ��Ӧ��fram��ַ
            if (MyMessage.m_bIsWrite)
            {
                FM25L256Func::writePage(8,(unsigned char *)1,1);   // �ڴ洢������ǰ�洦Ԥ����128�ֽڡ�����Ϊдһ��־����
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
    u16FramAddress = FRAM_BASE;   // �ڴ洢������ǰ�洦Ԥ����128�ֽڡ�
    u32CfgRelativeBaseAddress = 0;   // ��Ե�ַ  дʱΪ0��ַ
    u32CfgAbsoluteBaseAddress = (unsigned long) AllIVMap;  // ���Ե�ַ �ڴ��ַ
    ucBit = 0;

    /*
    	unsigned long uw32_length = (sizeof(MP_AllIVMap) - sizeof(St_DOWNLOAD_MAP_FROM_PC)* (MAX_AUX_TYPE_PER_MCU)); // ����һ�ΰ���ǰ�������Ϣ����
        unsigned long uw32_GetIndex = MAX_AUX_TYPE_PER_MCU;  // 512;   // ������洢FRAM�ı������ݶ����Ϊ512��һά���ݡ�  // ����5.7K��MAP��
        uw32_length += (uw32_GetIndex * sizeof(St_DOWNLOAD_MAP_FROM_PC));  // �����ʵ��һά�ֽ�����   ��IV�����11�ֽڼ���Ԥ���ռ䣬����AUXʵ��6�ֽڼ���Ԥ����
    */
    //for(u32CfgCount = 0; u32CfgCount < (sizeof(MP_AllIVMap)); u32CfgCount++)   //��config����һ������������
    unsigned long uw32_length = sizeof(MP_AllIVMap);  // �����ʵ��һά�ֽ�����   ��IV�����11�ֽڼ���Ԥ���ռ䣬����AUXʵ��6�ֽڼ���Ԥ����
    //for(u32CfgCount = 0; u32CfgCount <  MAX_MULTPACK_SIZE; u32CfgCount++)   //��config����һ������������
    for(u32CfgCount = 0; u32CfgCount <  uw32_length; u32CfgCount++)   //��config����һ������������
    {
        u16Byte = u32CfgCount / 8;
        ucBit = u32CfgCount % 8;

        ucByteContent = Aux_DataManager::FramConfigDictionary[u16Byte];
        if ((ucByteContent >> ucBit)&0x01)
        {
            FM25L256Func::readPage(u16FramAddress,(unsigned char *)(u32CfgAbsoluteBaseAddress + u32CfgCount),1); // һ��һ���ֽڶ���
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

    u16FramAddress = FRAM_BASE;  // �ڴ洢������ǰ�洦Ԥ����128�ֽڡ�
    for(u32Count = 0; u32Count < u32CfgRelativeBaseAddress; u32Count++)
    {
        u16Byte = u32Count / 8;   // 32K�б��У�һ��0XFF����8���ֽڡ� ȡ�б��������±�[��FramConfigDictionary[X]]��
        ucBit = u32Count % 8;   // 32K�б��У�һ��0XFF����8���ֽڡ� ȡ�б���������ֵλ��[��0XFF]��0��7��

        ucByteContent = Aux_DataManager::FramConfigDictionary[u16Byte];  //  ȡ�б���������ֵλ��[��0XFF]��
        if ((ucByteContent >> ucBit)&0x01)  // ȡ�б���������ֵλ��[��0XFF]��0��7��
            u16FramAddress++;   //  ���Ĵ洢���д洢���׵�ַ��
    }
}


// void AuxCConfig::SaveAuxUnitCFG(St_AuxBoardGlobalCFG * m_AuxUnitCFG)
// {
/*unsigned char ucByteContent = 0;

u32CfgRelativeBaseAddress = 0;  //�����ݽ����ľ��Ե�ַת��Ϊ��Ե�ַ  �� ����-����=0��ַ
u32CfgAbsoluteBaseAddress = (unsigned long) m_AuxUnitCFG; //  ���Ե�ַ = &Aux_DataManager::m_AuxUnitCFG
u32ByteCount = sizeof(St_AuxBoardGlobalCFG);
GetFramAddress();   //�ҵ�config��ַ��Ӧ��fram��ַ
u32CfgCount = 0;

while(u32ByteCount > 0)
{
u16Byte = (u32CfgRelativeBaseAddress + u32CfgCount) / 8;  // 32K�б��У�һ��0XFF����8���ֽڡ� ȡ�б��������±�[��FramConfigDictionary[X]]��
ucBit = (u32CfgRelativeBaseAddress + u32CfgCount) % 8;   // 32K�б��У�һ��0XFF����8���ֽڡ� ȡ�б���������ֵλ��[��0XFF]��0��7��

ucByteContent = Aux_DataManager::FramConfigDictionary[u16Byte];  //  ȡ�б���������ֵλ��[��0XFF]��
if ((ucByteContent >> ucBit)&0x01)   //  ȡ�б���������ֵλ��[��0XFF]��0��7��
{
FM25L256Func::writePage(u16FramAddress,(unsigned char *)(u32CfgAbsoluteBaseAddress + u32CfgCount),1);//���ֽ�д��
u16FramAddress++; //���ֽڵ�ַ�仯
}
u32CfgCount++;   //���ֽڼ������仯
if (u32CfgCount >= u32ByteCount)    //д����������
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

    u32CfgRelativeBaseAddress = 0;  //�����ݽ����ľ��Ե�ַת��Ϊ��Ե�ַ  �� ����-����=0��ַ
    u32CfgAbsoluteBaseAddress = (unsigned long) m_AuxUnit_IVMap; //  ���Ե�ַ = &Aux_DataManager::m_AuxUnitCFG
    u32ByteCount = uw32_Length;
    GetFramAddress();   //�ҵ�config��ַ��Ӧ��fram��ַ
    u32CfgCount = 0;

    while(u32CfgCount < u32ByteCount)
    {
        u16Byte = (u32CfgRelativeBaseAddress + u32CfgCount) / 8;  // 32K�б��У�һ��0XFF����8���ֽڡ� ȡ�б��������±�[��FramConfigDictionary[X]]��
        ucBit = (u32CfgRelativeBaseAddress + u32CfgCount) % 8;   // 32K�б��У�һ��0XFF����8���ֽڡ� ȡ�б���������ֵλ��[��0XFF]��0��7��

        ucByteContent = Aux_DataManager::FramConfigDictionary[u16Byte];  //  ȡ�б���������ֵλ��[��0XFF]��
        if ((ucByteContent >> ucBit)&0x01)   //  ȡ�б���������ֵλ��[��0XFF]��0��7��
        {
            FM25L256Func::writePage(u16FramAddress,(unsigned char *)(u32CfgAbsoluteBaseAddress + u32CfgCount),1);//���ֽ�д��
            u16FramAddress++; //���ֽڵ�ַ�仯
        }
        u32CfgCount++;   //���ֽڼ������仯
        if (u32CfgCount >= u32ByteCount)    //д����������
        {
#if REPORT_FRAM_EVENT == 1
            EthernetParse::MPReport_WriteFram_Event(MyMessage, &EthernetParse::m_pReport->m_Report_Write_Fram);
#endif
            break;
        }
    }
// 	for(unsigned char i=0;i<128;i++)    //add by qjm 05/25/2015
// 	{
// 		memset(&EthernetParse::m_btPackDataBuffer[2071+i*6],0,sizeof(St_DOWNLOAD_MAP_FROM_PC));     //�建����
// 	}
    // 		unsigned long u32Count;
    // 	   unsigned char ucByteContent = 0;

    // 	u16FramAddress = FRAM_BASE;  // �ڴ洢������ǰ�洦Ԥ����128�ֽڡ�

}

UChar8 AuxCConfig::WriteRSN(UChar8 uc_BoardID, UWord16 * uw16_RSN)
{
    // д��FRAM��I2C���ù�������RSN��
    //if(uc_BoardID >= MAX_BOARD_PER_AUXMCU)
    //	return 0;

    u16FramAddress = FRAM_I2CRSN_BASE + uc_BoardID*sizeof(UWord16);
    //u32CfgAbsoluteBaseAddress = MyMessage.m_StartAddress;

    uc_BoardID = FM25L256Func::writePage(u16FramAddress,(unsigned char *) uw16_RSN,sizeof(UWord16));
    return uc_BoardID;
}

UChar8 AuxCConfig::ReadRSN(UChar8 uc_BoardID, UWord16 * uw16_RSN)
{
    // ��FRAM��I2C���ù�������RSN��
    //if(uc_BoardID >= MAX_BOARD_PER_AUXMCU)
    //	return;

    u16FramAddress = FRAM_I2CRSN_BASE + uc_BoardID*sizeof(UWord16);

    uc_BoardID = FM25L256Func::readPage(u16FramAddress,(unsigned char *) uw16_RSN,sizeof(UWord16));
    return uc_BoardID;
}

void AuxCConfig::WriteRSNStata(UChar8 uc_BoardID, UChar8 * uc_Stata)
{
    // д��FRAM��I2C���ù�������״̬��Ϣ
    if(uc_BoardID >= MAX_BOARD_PER_AUXMCU)
        return;

    u16FramAddress = FRAM_I2CRSN_STATABASE + uc_BoardID;
    //u32CfgAbsoluteBaseAddress = MyMessage.m_StartAddress;

    FM25L256Func::writePage(u16FramAddress,uc_Stata,sizeof(UChar8));
}

void AuxCConfig::ReadRSNStata(UChar8 uc_BoardID, UChar8 * uc_Stata)
{
    // ��FRAM��I2C���ù�������״̬��Ϣ
    if(uc_BoardID >= MAX_BOARD_PER_AUXMCU)
        return;

    u16FramAddress = FRAM_I2CRSN_STATABASE + uc_BoardID;

    FM25L256Func::readPage(u16FramAddress,uc_Stata,sizeof(UChar8));
}


//  ����ΪSPI��FRAM����
//  ����ΪI2C��EPPROM����
unsigned char AuxCConfig::Load_I2C_Config(void)
{
    unsigned char uc_Result = 0;
    unsigned char uc_UARTCh = 0;
    unsigned char uc_DACCh = 0;
    unsigned char uc_I2CCh = 0;  //add by zc 04.20.2018
    //     unsigned char uc_FirstFlag = 0;

    for (unsigned char i = 0; i<MAX_BOARD_PER_AUXMCU; i++)
    {   // ����һ�ξɵ���Ϣ,������λ��д�幤�߼�����ʾ����Ϣ,û���и��²���.
        memset(&Aux_DataManager::m_AllAuxBoards[i],0,sizeof(ST_AUX_BOARD_INFO));
    }

    uc_Result = AuxCConfig::Reg_AUXBoardInfo();  //������Ϣ

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
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_globe.m_Max_Channels = MAX_CH_IN_AUX_UNIT; // ���ӱ����������� 06.16.2017 yy

            //for(unsigned char uc_Chid = 0; uc_Chid < Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_globe.m_Total_Channels; uc_Chid ++)
            for(unsigned char uc_Chid = 0; uc_Chid < Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_globe.m_Max_Channels; uc_Chid ++)
            {
                //  ������ֵ�����⴦��  Ӳ��������������  // ����һ���û������� �������On��1�� Off ��0�����ң�����Լ��������unit�ڵı�Ŵ� ��BID��ſ�ʼ���� ��Ϊ�����ȽϷ���ϰ�ߡ�
                //   S2����ONΪ0�����ONΪ1               // ��ˣ�����һ��ʼ����ŵ�ʱ����һ�´���ʹ�ð��ֵ�Ͳ���ġ�����ֵ�� һ�¡��Ժ��ͳһ�����������ֵ������ţ�����֤һ���ԡ�

                ReadAUXChannelCfg((MAX_BOARD_PER_AUXMCU-1-uc_Bid),uc_Chid,&uc_I2CSuccessFlag[uc_Chid]);


                // Aux_DataManager::m_AllAuxBoards[uc_Bid].m_AuxSensor_ChInfo[uc_Chid] = ReadCfg.m_AuxChInfo;
                // unsigned char uc_ch=ReadCfg.m_ChannelNumber;
                //�����Ƕ�ͨ����Ϣ�����ǽṹ��Ҳ�а��ȫ����Ϣ����������Ӧ���ǺͲ���m_AllAUxBoards,ͳһ�����������
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
                //  ������ֵ�����⴦��  Ӳ��������������  // ����һ���û������� �������On��1�� Off ��0�����ң�����Լ��������unit�ڵı�Ŵ� ��BID��ſ�ʼ���� ��Ϊ�����ȽϷ���ϰ�ߡ�
                //   S2����ONΪ0�����ONΪ1               // ��ˣ�����һ��ʼ����ŵ�ʱ����һ�´���ʹ�ð��ֵ�Ͳ���ġ�����ֵ�� һ�¡��Ժ��ͳһ�����������ֵ������ţ�����֤һ���ԡ�
                uc_UARTCh =uc_Chid +MAX_CH_IN_AUX_UNIT;
                ReadAUXChannelCfg((MAX_BOARD_PER_AUXMCU-1-uc_Bid),uc_UARTCh,&uc_I2CSuccessFlag[uc_Chid]);


                // Aux_DataManager::m_AllAuxBoards[uc_Bid].m_AuxSensor_ChInfo[uc_Chid] = ReadCfg.m_AuxChInfo;
                // unsigned char uc_ch=ReadCfg.m_ChannelNumber;
                //�����Ƕ�ͨ����Ϣ�����ǽṹ��Ҳ�а��ȫ����Ϣ����������Ӧ���ǺͲ���m_AllAUxBoards,ͳһ�����������
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo. m_abi_uart_chn[uc_Chid].m_Populated = AuxCConfig::AUX_I2C_ReadCfg.m_valid;
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo. m_abi_uart_chn[uc_Chid].m_ChnCoreInfo.m_AuxSensorCoreInfo = AuxCConfig::AUX_I2C_ReadCfg.m_ChnCoreInfo.m_AuxSensorCoreInfo;
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo. m_abi_uart_chn[uc_Chid].m_AuxType= AuxCConfig::AUX_I2C_ReadCfg.m_AuxType;
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo. m_abi_uart_chn[uc_Chid].m_AuxSubType = UART;	// = 1000, ��λ��δд�ò������˴���λ���ֶ�д�룬�Ƿ�Ӧ�޸ģ� Yifei
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo. m_abi_uart_chn[uc_Chid].m_avg_num = AuxCConfig::AUX_I2C_ReadCfg.m_AVG_num;
            }
        }
        if((Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_valid == BEUSED)&&(Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo.m_abi_globe.m_Total_DAC!=0))
        {
            for(unsigned char uc_Chid = 0; uc_Chid < 8; uc_Chid ++)
            {
                //  ������ֵ�����⴦��  Ӳ��������������  // ����һ���û������� �������On��1�� Off ��0�����ң�����Լ��������unit�ڵı�Ŵ� ��BID��ſ�ʼ���� ��Ϊ�����ȽϷ���ϰ�ߡ�
                //   S2����ONΪ0�����ONΪ1               // ��ˣ�����һ��ʼ����ŵ�ʱ����һ�´���ʹ�ð��ֵ�Ͳ���ġ�����ֵ�� һ�¡��Ժ��ͳһ�����������ֵ������ţ�����֤һ���ԡ�
                uc_DACCh =uc_Chid +MAX_CH_IN_AUX_UNIT+8;
                ReadAUXChannelCfg((MAX_BOARD_PER_AUXMCU-1-uc_Bid),uc_DACCh,&uc_I2CSuccessFlag[uc_Chid]);


                // Aux_DataManager::m_AllAuxBoards[uc_Bid].m_AuxSensor_ChInfo[uc_Chid] = ReadCfg.m_AuxChInfo;
                // unsigned char uc_ch=ReadCfg.m_ChannelNumber;
                //�����Ƕ�ͨ����Ϣ�����ǽṹ��Ҳ�а��ȫ����Ϣ����������Ӧ���ǺͲ���m_AllAUxBoards,ͳһ�����������
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
                //  ������ֵ�����⴦��  Ӳ��������������  // ����һ���û������� �������On��1�� Off ��0�����ң�����Լ��������unit�ڵı�Ŵ� ��BID��ſ�ʼ���� ��Ϊ�����ȽϷ���ϰ�ߡ�
                //   S2����ONΪ0�����ONΪ1               // ��ˣ�����һ��ʼ����ŵ�ʱ����һ�´���ʹ�ð��ֵ�Ͳ���ġ�����ֵ�� һ�¡��Ժ��ͳһ�����������ֵ������ţ�����֤һ���ԡ�
                uc_UARTCh =uc_Chid +MAX_CH_IN_AUX_UNIT;
                ReadAUXChannelCfg((MAX_BOARD_PER_AUXMCU-1-uc_Bid),uc_UARTCh,&uc_I2CSuccessFlag[uc_Chid]);


                // Aux_DataManager::m_AllAuxBoards[uc_Bid].m_AuxSensor_ChInfo[uc_Chid] = ReadCfg.m_AuxChInfo;
                // unsigned char uc_ch=ReadCfg.m_ChannelNumber;
                //�����Ƕ�ͨ����Ϣ�����ǽṹ��Ҳ�а��ȫ����Ϣ����������Ӧ���ǺͲ���m_AllAUxBoards,ͳһ�����������
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo. m_abi_uart_chn[uc_Chid].m_Populated = AuxCConfig::AUX_I2C_ReadCfg.m_valid;
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo. m_abi_uart_chn[uc_Chid].m_ChnCoreInfo.m_AuxSensorCoreInfo = AuxCConfig::AUX_I2C_ReadCfg.m_ChnCoreInfo.m_AuxSensorCoreInfo;
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo. m_abi_uart_chn[uc_Chid].m_AuxType= AuxCConfig::AUX_I2C_ReadCfg.m_AuxType;
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo. m_abi_uart_chn[uc_Chid].m_AuxSubType = AuxCConfig::AUX_I2C_ReadCfg.m_AuxSubType;	//
                Aux_DataManager::m_AllAuxBoards[uc_Bid].m_BoardInfo. m_abi_uart_chn[uc_Chid].m_avg_num = AuxCConfig::AUX_I2C_ReadCfg.m_AVG_num;
            }
        }
    }



    Aux_DataManager::Aux_ObtainGainOffset();  // �ԽӸ��²���У��ֵ

    return uc_Result;
}

unsigned char AuxCConfig::Reg_AUXBoardInfo(void)  //��ע�����еĸ���ͨ���壬����BID˳�����������ձ�
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
            //  ������ֵ�����⴦��  Ӳ��������������  // ����һ���û������� �������On��1�� Off ��0�����ң�����Լ��������unit�ڵı�Ŵ� ��BID��ſ�ʼ���� ��Ϊ�����ȽϷ���ϰ�ߡ�
            //   S2����ONΪ0�����ONΪ1               // ��ˣ�����һ��ʼ����ŵ�ʱ����һ�´���ʹ�ð��ֵ�Ͳ���ġ�����ֵ�� һ�¡��Ժ��ͳһ�����������ֵ������ţ�����֤һ���ԡ�

            if(m_Readcfg.m_valid == BEUSED)	// �����жϰ��Ƿ�valid���� valid == 1, ����������Ϣ��m_AllAuxBoards
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
        return 0;            // û���ҵ��κθ���ͨ����
    else
        return 1;

}

unsigned char AuxCConfig::AUX_IfLoadBoard(unsigned char uc_DeviceAddr)
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
UChar8 AuxCConfig::AUX_IfWrite(void)
{
    //  �жϻ��������Ƿ�������ռ��24LC128 оƬ�洢����EEPROM
    //  Returned value:		TRUE:��ʾ��ʹ�û�������FALSE:��ʾ����ʹ�û�����
    if(E2PROM_24LC128::uc_EEPROM_OnWriteBuf)
    {
        return 0;  //ռ���˲���д
    }
    else
    {
        return 1;
    }
}

UChar8 AuxCConfig::AUX_IfRead(void)
{
    //  �жϻ��������Ƿ�������ռ��24LC128 оƬ�洢����EEPROM
    //  Returned value:		 1:��ʾ��ʹ�û�������0:��ʾ����ʹ�û�����
    if(E2PROM_24LC128::uc_EEPROM_OnReadBuf)
    {
        return 0;    //ռ���˲��ܶ�
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
    UWord16 StartAddr=AUXBOARDCHANNELCFGADDR;  // ����Ȼ��ͬ�������ĵط���һ���ġ�
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
    UWord16 StartAddr=AUXBOARDCFGSRARTADDR;  // ����Ȼ��ͬ�������ĵط���һ���� //��ԭ��24�ֽڣ����16�ֽ�
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
    UWord16 StartAddr=AUXBOARDCFGSRARTADDR;  // ����Ȼ��ͬ��������Ϣ��ĵط���һ���ġ�
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



UChar8 AuxCConfig::WriteAUXBoardRSN(UChar8 uc_DeviceAddr,UWord16 * uw16_RSN)
{
    UWord16 StartAddr=AUXBOARDRSNSRARTADDR;   // ����Ȼ��ͬ�������ĵط���һ���ġ�

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
    UWord16 StartAddr=AUXBOARDRSNSRARTADDR;   // ����Ȼ��ͬ�������ĵط���һ���ġ�

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

void AuxCConfig::ReadContent_ForFRAM(unsigned char * Dest, UWord16  u16BaseAddress, unsigned long uw32_DataLength)
{   // һ���Զ���FRAM�����ض����ݡ�
    unsigned long  uw32_length = uw32_DataLength;

    UWord32  u32CfgRelativeBaseAddress = 0;  //�����ݽ����ľ��Ե�ַת��Ϊ��Ե�ַ  �� ����-����=0��ַ
    UWord32  u32CfgAbsoluteBaseAddress = (unsigned long) (Dest); //  ���Ե�ַ = &Address
    UWord32  u32CfgCount = 0;
    UWord16  u16FramAddress = u16BaseAddress;
    unsigned char ucByteContent = 0;
    unsigned char ucBit = 0;

    while(u32CfgCount < uw32_length)
    {
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

