#pragma once


#define FRAM_VALID_SIGN          0x12345678

#define FRAM_I2CRSN_BASE         0x60      //Fram存储I2C RSN的起始地址...
#define FRAM_I2CRSN_STATABASE    0x70      //Fram存储I2C RSN Stata的起始地址...
#define FRAM_BASE                0x80      //Fram存储Config的起始地址...
#define FRAM_MESSAGE_DEPTH       20        //定义FRAM TASK的消息队列深度

// #define FRAM_REMAIN_LENGTH                   (0X8000 - FRAM_BASE -  FRAM_AUXMAP_LENGTH)   // 256K/8=32K    剩余大小空间27K
#define FRAM_TEMP_REC            0x7FFD    //Fram存储TEC Chamber的起始地址 Added by ZC 09/29/2016
typedef struct //St_FramHeader
{
    unsigned long ConfigValid;
    unsigned long WritingAddress;
    unsigned char bIsWriting;

    float               m_fBootVersion;
    float               m_fUserVersion;

    unsigned char       m_ucDriverIp[4];
} St_FramHeader;

typedef CIRCULAR_QUEUE(St_FramMessage,IV_FRAM_MESSAGE_DEPTH) St_FramMessages;

class CConfig
{
private:
    static unsigned char    bWriting;
    static unsigned short   u16Byte;
    static unsigned char    ucBit;
    static unsigned short   u16ByteCount;
    static unsigned short   u16FramAddress;
    static unsigned short   u16CfgCount;
    static unsigned long    u32CfgRelativeBaseAddress;
    static unsigned long    u32CfgAbsoluteBaseAddress;
    static St_FramMessage   MyMessage;
    static St_FramMessages  Messages;
    static St_FramHeader    FramHeader;

public:
		static St_AUXChannelCFG  I2C_ReadCfg;
		static ST_AUX_BOARD_INFO_SAVE   BuiltIn_m_Readcfg;	
    static unsigned char    bConfigValid;
    static UInt16           Priority;
    static UInt16           TaskID;
    static WORD				uwOutOf_FRAM_error;//1610142 zyx

private:
    static void readConfig(unsigned char Bid);
    static void GetFramAddress(void);
    static void WritePID(unsigned short u16_ChannelIdx);
    static void ReadPID(unsigned short u16_ChannelIdx);

public:
    static void MyInit(void);
// 	static unsigned char LoadSysConfig(void);
    static void DoWork(void);
    static int PopFramMessage(St_FramMessage * myMessage);
    static void PushFramMessage(St_FramMessage myMessage);
    static unsigned char IsLoadBoard(unsigned char DeviceAddr);

    static void WriteContent_ForFRAM(unsigned char * Source, UWord16  u16BaseAddress, unsigned long uw32_DataLength);
    static void ReadContent_ForFRAM(unsigned char * Dest, UWord16  u16BaseAddress, unsigned long uw32_DataLength);

    static unsigned char Load_I2C_Config(void);
    static unsigned char Reg_BUILTINBoardInfo(unsigned char BoardID);  //先注册所有的辅助通道板，按照BID顺序建立索引对照表
    static unsigned char AUX_IfLoadBoard(unsigned char uc_DeviceAddr);
//   static ST_AUX_BOARD_INFO_SAVE ReadBuiltInBoardCfg(UChar8 uc_DeviceAddr,UChar8 * uc_SuccessFlag);
    static ST_AUX_BOARD_INFO_SAVE ReadBuiltInBoardCfg(UChar8 uc_DeviceAddr);
// 	static St_AUXChannelCFG ReadBuiltInChannelCfg(unsigned char uc_DeviceAddr,unsigned char uc_ChannelNo, UChar8 * uc_SuccessFlag);
    static void ReadBuiltInChannelCfg(unsigned char uc_DeviceAddr,unsigned char uc_ChannelNo);
    static unsigned char WriteBuiltInBoardCfg(UChar8 uc_DeviceAddr,ST_BUILTIN_BOARD_INFO_IN_EEPROM * m_AuxBoardInfo);
    static unsigned char WriteBuiltInChannelCfg(UChar8 uc_DeviceAddr,UChar8 uc_ChannelNo,St_AUXChannelCFG * m_AuxChannelCfg);
    static void IV_BoardType_OldTransformToNew(void);
//  static void  BuiltIn_Board_Init(void);
    //  #if SUPPORT_AUX ==1
    //    	static void WriteAuxMap(unsigned long uw32_Length);
    //  #endif
};
