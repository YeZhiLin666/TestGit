#pragma once

#define FRAM_VALID_SIGN          0x12345678
#define FRAM_I2CRSN_BASE         0x60      //Fram�洢I2C RSN����ʼ��ַ...
#define FRAM_I2CRSN_STATABASE    0x70      //Fram�洢I2C RSN Stata����ʼ��ַ...
#define FRAM_BASE                0x80      //Fram�洢Config����ʼ��ַ...
#define FRAM_MESSAGE_DEPTH       20        //����FRAM TASK����Ϣ�������

#define FRAM_REMAIN_LENGTH                   (0X8000 - FRAM_BASE -  FRAM_AUXMAP_LENGTH)   // 256K/8=32K    ʣ���С�ռ�27K
#define FRAM_AUXCANBMS_MAP_BASE     0x7FE0    //Fram�洢AUXCANBMS_MAP_BASE����ʼ��ַ   16�ֽ� 
#define FRAM_AUXCANBMS_MAP_LENGTH   0x0C      //Fram�洢AUXCANBMS_MAPʵ��   12�ֽ� 
#define FRAM_TEMP_REC            0x7FFD    //Fram�洢TEC Chamber����ʼ��ַ Added by ZC 09/29/2016

//����5.7K��MAP��,512*11=5632   �൱��һ��IVMCU����֧��4��AUXMCU������512��ͨ���� ƽ��512/16=32 ,ÿ��IV�ɴ�32��Auxͨ����
#define MAXAUXMCUCHANNE       (512) // (4*8*16)    //IVMCU����֧��4��AUXMCU������512��ͨ��
#define FRAM_AUXMAP_LENGTH    (0X1770) // 6000    //IVMCU����֧��4��AUXMCU������512��ͨ��,ÿͨ��11���ֽ�  // 40+2816[16*16*11]+512*11=5928
// MAP������ʵ��һά�ֽ�����   ��IV�����11�ֽڼ���Ԥ���ռ䣬����AUXʵ��6�ֽڼ���Ԥ����


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
    unsigned char   m_bPID;       //�Ƿ���PID����
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
    static unsigned char Reg_AUXBoardInfo(void);  //��ע�����еĸ���ͨ���壬����BID˳�����������ձ�
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


