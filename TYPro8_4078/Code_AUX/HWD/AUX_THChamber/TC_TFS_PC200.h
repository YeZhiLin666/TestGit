/*****************************************************************************
// 程序文件      :TC_TFS_PC200.h    定义温控箱中各种处理头文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/
/** TC_TFS_PC200.h **/

#ifndef TC_TFS_PC200_H
#define TC_TFS_PC200_H 1


#define TC_PC200_MIN_RESPONSE_CMD_LEN		2*TC_PC200_MAX_COM_READ_REPLY    // 15  // 回应包最小长度限制
#define TC_PC200_MAX_RESPONSE_CMD_LEN		9*TC_PC200_MAX_COM_READ_REPLY    // 40  // 回应包最大长度限制



// 发送长度和接收长度定义：
#define TC_PC200_MAX_COM_WRITE_CMD	     (1+7)    // CMD(7 byte)+Head(1 byte)      
#define TC_PC200_MAX_COM_WRITE_REPLY	 (1+8)    // REPLY(8byte)+Head(1 byte)    
#define TC_PC200_MAX_COM_READ_CMD	     (1+5)    // CMD(5 byte)+Head(1 byte)      
#define TC_PC200_MAX_COM_READ_REPLY	     (1+8)    // REPLY(8byte)+Head(1 byte)    

#define TC_PC200_MAX_COM_STOP_CMD	     (1+7)    // CMD(7 byte)+Head(1 byte)     
#define TC_PC200_MAX_COM_RUN_CMD	     (1+5)    // CMD(5 byte)+Head(1 byte)     

#define TC_PC200_WRITE_CMD_POINT_VAlUE	     (5)    // Write Value point (5)       
#define TC_PC200_READ_REPLY_POINT_VAlUE	     (6)    // Read REPLY Value point (6)    


/*
//  PC200温控箱运行和停止命令，跟读和写命令一致。

//  特别注意：当使用运行和停止命令，跟读和写命令不一致时，机制必须修改。打开这个定义。
//#define COM_RUNANDSTOP_CMD_DIFFER	     (1) // 不一致时,开启。Open this define for Difference.
*/

class TC_PC200
{
public:
    static void Init(unsigned char ChanNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
};

//bool TC_EZT570I_uart_getcmd (unsigned char ChanNo,unsigned char *pCmd, unsigned int len);




#endif
