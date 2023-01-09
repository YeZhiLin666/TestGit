#pragma once

#define FRAM_VALID_SIGN          0x12345678
#define FRAM_I2CRSN_BASE         0x60      //Fram存储I2C RSN的起始地址...
#define FRAM_I2CRSN_STATABASE    0x70      //Fram存储I2C RSN Stata的起始地址...
#define FRAM_BASE                0x80      //Fram存储Config的起始地址...
#define FRAM_MESSAGE_DEPTH       20        //定义FRAM TASK的消息队列深度

#define FRAM_REMAIN_LENGTH                   (0X8000 - FRAM_BASE -  FRAM_AUXMAP_LENGTH)   // 256K/8=32K    剩余大小空间27K
#define FRAM_AUXCANBMS_MAP_BASE     0x7FE0    //Fram存储AUXCANBMS_MAP_BASE的起始地址   16字节 
#define FRAM_AUXCANBMS_MAP_LENGTH   0x0C      //Fram存储AUXCANBMS_MAP实存   12字节 
#define FRAM_TEMP_REC            0x7FFD    //Fram存储TEC Chamber的起始地址 Added by ZC 09/29/2016

//扩大到5.7K存MAP表,512*11=5632   相当于一个IVMCU最大可支持4个AUXMCU到辅助512个通道， 平均512/16=32 ,每个IV可带32个Aux通道。
#define MAXAUXMCUCHANNE       (512) // (4*8*16)    //IVMCU最大可支持4个AUXMCU到辅助512个通道
#define FRAM_AUXMAP_LENGTH    (0X1770) // 6000    //IVMCU最大可支持4个AUXMCU到辅助512个通道,每通道11个字节  // 40+2816[16*16*11]+512*11=5928
// MAP表计算出实际一维字节数量   以IV的最大11字节计算预留空间，不以AUX实际6字节计算预留。


enum FRAM_ADDRESS
{
    AUX_MAP,
    REMAIN_EMPTY,
    MAX_ADDRESS
};

typedef struct //St_FramHeader
{
    unsigned long ConfigValid;
    unsigned long WritingAddress;
    unsigned char bIsWriting;

    float               m_fBootVersion;
    float               m_fUserVersion;

    unsigned char       m_ucDriverIp[4];
} St_FramHeader;

/*Message Define For Tasks*/
typedef struct //St_FramMessage
{
    unsigned char   m_bIsWrite;
    unsigned char   m_bPID;       //是否是PID操作
    unsigned short  m_u16ChannelIdx;
#ifdef REPORT_FRAM_EVENT
    WORD               m_wChannelBitIdx;
    WORD               m_wChannelCount;
    WORD               m_wFramType;
    MsTime             m_MsTime;
#endif
    unsigned short  m_u16Length;
    unsigned long   m_StartAddress;
} St_FramMessage;

typedef CIRCULAR_QUEUE(St_FramMessage,FRAM_MESSAGE_DEPTH) St_FramMessages;


class AuxCConfig
{
private:
    static unsigned char    bWriting;
    static unsigned short   u16Byte;
    static unsigned char    ucBit;
    static unsigned long    u32ByteCount;
    static unsigned short   u16FramAddress;
    static unsigned long    u32CfgCount;
    static unsigned long    u32CfgRelativeBaseAddress;
    static unsigned long    u32CfgAbsoluteBaseAddress;
    static St_FramMessage   MyMessage;
    static St_FramMessages  Messages;
    static St_FramHeader    FramHeader;

public:
    static unsigned char    bConfigValid;
    static UInt16           Priority;
    static UInt16           TaskID;
    static unsigned short   uwOutOf_FRAM_error;
		static ST_AUX_BOARD_INFO   m_AuxBoards;
		static St_AUXChannelCFG AUX_I2C_ReadCfg;

private:
    static void readConfig(unsigned char Bid);
    static void GetFramAddress(void);

public:
    static unsigned char MyInit(void);
    static unsigned char LoadSysConfig(void);
    static unsigned char Load_I2C_Config(void);

    // SPI FRAM
    static void DoWork(void);
    static int PopFramMessage(St_FramMessage * myMessage);
    static void PushFramMessage(St_FramMessage myMessage);
    //static void SaveAuxUnitCFG(St_AuxBoardGlobalCFG * m_AuxUnitCFG);
    static void SaveAuxUnit2IVMap(MP_AllIVMap * m_AuxUnit_IVMap, UWord32 uw32_Length);
    static UChar8 WriteRSN(UChar8 uc_BoardID, UWord16 * uw16_RSN);
    static UChar8 ReadRSN(UChar8 uc_BoardID, UWord16 * uw16_RSN);
    static void WriteRSNStata(UChar8 uc_BoardID, UChar8 * uc_Stata);
    static void ReadRSNStata(UChar8 uc_BoardID, UChar8 * uc_Stata);

    // I2C EPPROM
    static unsigned char IsLoadBoard(unsigned char DeviceAddr);
    static unsigned char Reg_AUXBoardInfo(void);  //先注册所有的辅助通道板，按照BID顺序建立索引对照表
    static UChar8 AUX_IfWrite(void);
    static UChar8 AUX_IfRead(void);
    static ST_AUX_BOARD_INFO_SAVE ReadAUXBoardCfg(UChar8 uc_DeviceAddr,UChar8 * uc_SuccessFlag);
    static UChar8 WriteAUXBoardCfg(UChar8 uc_DeviceAddr,ST_AUX_BOARD_INFO * m_AuxBoardInfo);
    static UChar8 WriteAUXChannelCfg(UChar8 uc_DeviceAddr,UChar8 uc_ChannelNo,St_AUXChannelCFG * m_AuxChannelCfg);
    static unsigned char AUX_IfLoadBoard(unsigned char uc_DeviceAddr);
    static void ReadAUXChannelCfg(unsigned char uc_DeviceAddr,unsigned char uc_ChannelNo, UChar8 * uc_SuccessFlag);
    // 	  static UChar8 Reg_IV2AUXChannel(unsigned char uc_IVUnit,unsigned char uc_IVChannel,unsigned char AuxType,unsigned char uc_VirturlChannel,St_AUXChannelCFG * m_AuxChannelCfg);
    static UChar8 WriteAUXBoardRSN(UChar8 uc_DeviceAddr,UWord16 * uw16_RSN);
    static UChar8 ReadAUXBoardRSN(UChar8 uc_DeviceAddr,UWord16 * uw16_RSN);
    static void WriteContent_ForFRAM(unsigned char * Source, UWord16  u16BaseAddress, unsigned long uw32_DataLength);
    static void ReadContent_ForFRAM(unsigned char * Dest, UWord16  u16BaseAddress, unsigned long uw32_DataLength);

};


