/*****************************************************************************
// 程序文件      :TC_SAMWONTECH_TEMP2500.h    定义温控箱中各种处理头文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/
/** ctrl.h **/

#ifndef TC_SAMWONTECH_TEMP2500_H
#define TC_SAMWONTECH_TEMP2500_H 1



//  回应包长度限制
#define TC_TEMP2500_MIN_RESPONSE_CMD_NUM        1 // 3帧   // 回应包最小数量限制
#define TC_TEMP2500_MAX_RESPONSE_CMD_NUM        9 // 9帧   // 回应包最大数量限制
#define TC_TEMP2500_MIN_RESPONSE_CMD_LEN		(TC_TEMP2500_MIN_RESPONSE_CMD_NUM*TC_TEMP2500_MAX_COM_READ_REPLY)    // 3帧*N  // 回应包最小长度限制
#define TC_TEMP2500_MAX_RESPONSE_CMD_LEN		(TC_TEMP2500_MAX_RESPONSE_CMD_NUM*TC_TEMP2500_MAX_COM_READ_REPLY)    // 9帧*N  // 回应包最大长度限制


#define TC_TEMP2500_MAX_COM_WRITE_CMD	     (1+4+18)    // CMD(18 byte)+HEAD(1 byte)+End(4 byte)  
#define TC_TEMP2500_MAX_COM_WRITE_REPLY	     (1+4+8)     // REPLY(8 byte)+HEAD(1 byte)+End(4 byte)     
#define TC_TEMP2500_MAX_COM_READ_CMD	     (1+4+13)    // CMD(13 byte)+HEAD(1 byte)+End(4 byte)     
#define TC_TEMP2500_MAX_COM_READ_REPLY	     (1+4+13)    // REPLY(13 byte)+HEAD(1 byte)+End(4 byte)     
#define TC_TEMP2500_MAX_COM_RUN_CMD	         (1+4+28)    // CMD(28 byte)+HEAD(1 byte)+End(4 byte)       
#define TC_TEMP2500_MAX_COM_STOP_CMD	     (1+4+28)    // CMD(28 byte)+HEAD(1 byte)+End(4 byte)      

#define TC_TEMP2500_WRITE_CMD_POINT_TEMPVAlUE	 (15)    // Write Value point (15)        
#define TC_TEMP2500_READ_REPLY_POINT_TEMPVAlUE	 (10)    // Read REPLY Value point (10)   
#define TC_TEMP2500_WRITE_CMD_POINT_HUMIVAlUE	 (25)    // Write Value point (25)         
#define TC_TEMP2500_READ_REPLY_POINT_HUMIVAlUE	 (15)    // Read REPLY Value point (15)  

class TC_TEMP2500
{
public:
    static void Init(unsigned char ChanNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);

    static void SendPreWrCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendModeCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);

    static unsigned char LoadCheckSum(unsigned char CMD_Code,unsigned char *puchMsg, unsigned int usDataLen);
};

//bool TC_EZT570I_uart_getcmd (unsigned char ChanNo,unsigned char *pCmd, unsigned int len);

/*
void TC_TEMP2500_SendWarnLimitCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
void TC_TEMP2500_SendtwentyoneCmd(unsigned char ChanNo);
void TC_TEMP2500_SendeightCmd(unsigned char ChanNo);
void TC_TEMP2500_SendtwentythreeCmd(unsigned char ChanNo);
void TC_TEMP2500_SendCheckStatusCmd(unsigned char ChanNo);
void TC_TEMP2500_SendCheckSetpointCmd(unsigned char ChanNo);
void TC_TEMP2500_ExecStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
*/


unsigned char TC_TEMP2500_LoadCheckSum(unsigned char CMD_Code,unsigned char *puchMsg, unsigned int usDataLen);


#endif
