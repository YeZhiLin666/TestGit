/*****************************************************************************
// 程序文件      :TC_KSI_THS2005.h    定义温控箱中各种控制命令头文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2011.12.01
*****************************************************************************/

//#define TC_THS2005_MIN_RESPONSE_CMD_LEN		30
//#define TC_THS2005_MAX_RESPONSE_CMD_LEN		90

#define TC_THS2005_MAX_COM_READ_REPLY	    70  // by dekui, for a typical response for asking temperature             // (6+44)    // REPLY(44byte)+Head(6 byte)    
#define TC_THS2005_MIN_RESPONSE_CMD_LEN		2*TC_THS2005_MAX_COM_READ_REPLY    // 15  // 回应包最小长度限制
#define TC_THS2005_MAX_RESPONSE_CMD_LEN		6*TC_THS2005_MAX_COM_READ_REPLY  //  4*MAX_COM_READ_REPLY    // 40  // 回应包最大长度限制

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


