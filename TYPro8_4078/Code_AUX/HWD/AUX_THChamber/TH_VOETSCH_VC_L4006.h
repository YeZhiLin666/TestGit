/*****************************************************************************
// 程序文件      :TH_VOETSCH_VC_L4006.h    定义温控箱中各种处理头文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/
/** ctrl.h **/

#ifndef TH_VOETSCH_VC_L4006_H
#define TH_VOETSCH_VC_L4006_H 1

//  回应包长度限制
#define TH_VOETSCH_L4006_MIN_RESPONSE_CMD_LEN		2*TH_VOETSCH_MAX_COM_READ_REPLY    // 15  // 回应包最小长度限制
#define TH_VOETSCH_L4006_MAX_RESPONSE_CMD_LEN		6*TH_VOETSCH_MAX_COM_READ_REPLY  //  4*MAX_COM_READ_REPLY    // 40  // 回应包最大长度限制



#define TH_VOETSCH_L4006_MAX_COM_WRITE_CMD	     (52)    // CMD(50 byte)+Head(2 byte)       
#define TH_VOETSCH_L4006_MAX_COM_WRITE_REPLY	     (82)    // REPLY(81byte)+Head(1 byte)    
#define TH_VOETSCH_L4006_MAX_COM_READ_CMD	         (2+3)   // CMD(3 byte)+Head(2 byte)        
#define TH_VOETSCH_L4006_MAX_COM_READ_REPLY	     (61)    // REPLY(60)+Head(1 byte)    

#define TH_VOETSCH_L4006_MAX_COM_STOP_CMD	         (52)    // CMD(50 byte)+Head(2 byte)      
#define TH_VOETSCH_L4006_MAX_COM_RUN_CMD	         (2+3)    // CMD(3 byte)+Head(2 byte)       

#define TH_VOETSCH_L4006_WRITE_CMD_POINT_VAlUE	 (4)    // Write Value point (4)       
#define TH_VOETSCH_L4006_READ_REPLY_POINT_VAlUE	 (6)    // Read REPLY Value point (6)   

/*
//  富奇温控箱运行和停止命令，跟读和写命令一致。
//  特别注意：当使用运行和停止命令，跟读和写命令不一致时，机制必须修改。打开这个定义。
#define COM_RUN4READ_CMD_DIFFER	     (1) // 停与写，行与读不一致时,开启。Open this define for Difference.
//#define COM_TEMP4HUMI_CMD	         (1) // 既控制温度又控制湿度时。Open this define temperature and humi.
//#define COM_CMD_CHANNEL_DIFFER       (1) // 使用多个不同通道号时开启标示1到4，否则默认为1号通道。Open this define multi channel.
*/

class TH_VOETSCH_L4006
{
public:
    static void Init(unsigned char ChanNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
};

//bool TH_EZT570I_uart_getcmd (unsigned char ChanNo,unsigned char *pCmd, unsigned int len);




#endif
