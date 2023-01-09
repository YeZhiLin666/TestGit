/*****************************************************************************
// 程序文件      :TH_SIEMENS_S7_SAIA.H    定义温控箱中各种处理头文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2017.10.10
*****************************************************************************/
/** TH_SIEMENS_S7_SAIA.h **/


//  回应包长度限制
#define TH_SIEMENS_S7_SAIA_MIN_RESPONSE_CMD_NUM        1 // 1帧   // 回应包最小数量限制
#define TH_SIEMENS_S7_SAIA_MAX_RESPONSE_CMD_NUM        9 // 9帧   // 回应包最大数量限制
#define TH_SIEMENS_S7_SAIA_MIN_RESPONSE_CMD_LEN		(TH_SIEMENS_S7_SAIA_MIN_RESPONSE_CMD_NUM*TH_SIEMENS_S7_SAIA_MAX_COM_READ_REPLY)    // 1帧*N  // 回应包最小长度限制
#define TH_SIEMENS_S7_SAIA_MAX_RESPONSE_CMD_LEN		(TH_SIEMENS_S7_SAIA_MAX_RESPONSE_CMD_NUM*TH_SIEMENS_S7_SAIA_MAX_COM_READ_REPLY)    // 9帧*N  // 回应包最大长度限制




// 发送长度和接收长度定义：
#define TH_SIEMENS_S7_SAIA_MAX_COM_WRITE_CMD	         (8+12)  // MBAP(8 byte)+CMD(12 byte)  
#define TH_SIEMENS_S7_SAIA_MAX_COM_WRITE_REPLY	       (8+3)   // MBAP(8 byte)+REPLY(3 byte)
#define TH_SIEMENS_S7_SAIA_MAX_COM_READ_CMD	           (8+8)   // MBAP(8 byte)+CMD(8 byte)  
#define TH_SIEMENS_S7_SAIA_MAX_COM_READ_REPLY	         (8+7)   // MBAP(8 byte)+REPLY(7 byte)
#define TH_SIEMENS_S7_SAIA_MAX_COM_RUN_CMD	           (8+10)   // MBAP(8 byte)+CMD(10 byte) 
#define TH_SIEMENS_S7_SAIA_MAX_COM_STOP_CMD	           (8+10)   // MBAP(8 byte)+CMD(10 byte)
//#define TH_SIEMENS_S7_SAIA_MAX_COM_RUNorSTOP_REPLY	   (8+3)   // MBAP(8 byte)+REPLY(3 byte)


class TH_SIEMENS_S7_SAIA
{
public:
    static void Init(unsigned char ChanNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);

    static unsigned char SetPresentValue_Check(unsigned char ChanNo, short int Value);
};

