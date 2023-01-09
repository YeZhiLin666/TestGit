/*****************************************************************************
// 程序文件      :TC_EUROTHERM_3216.H    定义温控箱中各种处理头文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/
/** TC_EUROTHERM_3216.h **/

#ifndef TC_EUROTHERM_3216_H
#define TC_EUROTHERM_3216 1



#define TC_3216_MIN_RESPONSE_CMD_LEN		15
#define TC_3216_MAX_RESPONSE_CMD_LEN		40



#define TC_3216_MAX_COM_WRITE_CMD	     (11+2)   // CMD(11 byte)+CRC(2 byte) 
#define TC_3216_MAX_COM_WRITE_REPLY	     (11+2)   // REPLY(11 byte)+CRC(2 byte) 
#define TC_3216_MAX_COM_READ_CMD	     (6+2)   // CMD(6 byte)+CRC(2 byte) 
#define TC_3216_MAX_COM_READ_REPLY	     (5+2)   // REPLY(5 byte)+CRC(2 byte)  

class TC_3216
{
public:
    static void Init(unsigned char ChanNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendPreWrCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);

    static bool uart_getcmd (unsigned char ChanNo,unsigned char *pCmd, unsigned int len);
};


#endif
