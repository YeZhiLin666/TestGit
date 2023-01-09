/*****************************************************************************
// �����ļ�      :TC_KSI_THS2005.h    �����¿����и��ֿ�������ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2011.12.01
*****************************************************************************/

//#define TC_THS2005_MIN_RESPONSE_CMD_LEN		30
//#define TC_THS2005_MAX_RESPONSE_CMD_LEN		90

#define TC_THS2005_MAX_COM_READ_REPLY	    70  // by dekui, for a typical response for asking temperature             // (6+44)    // REPLY(44byte)+Head(6 byte)    
#define TC_THS2005_MIN_RESPONSE_CMD_LEN		2*TC_THS2005_MAX_COM_READ_REPLY    // 15  // ��Ӧ����С��������
#define TC_THS2005_MAX_RESPONSE_CMD_LEN		6*TC_THS2005_MAX_COM_READ_REPLY  //  4*MAX_COM_READ_REPLY    // 40  // ��Ӧ����󳤶�����

class TC_THS2005
{
public:
    static void Init(unsigned char ChanNo);
    static void Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
};

// extern char* ctrl_Value2String(short value, char* s);
// extern char* ctrl_String2Value( char* s, short int* pValue);
// extern bool MTCI_datatypes::ctrl_strncmp(unsigned char* Src, unsigned char* Dest, unsigned int Len);


