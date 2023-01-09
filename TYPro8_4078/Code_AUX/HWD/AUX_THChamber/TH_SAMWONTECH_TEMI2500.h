/*****************************************************************************
// 程序文件      :TH_SAMWONTECH_TEMI2500.h    定义温控箱中各种处理头文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/
/** TH_SAMWONTECH_TEMI2500.h **/

#ifndef TH_SAMWONTECH_TEMI2500_H
#define TH_SAMWONTECH_TEMI2500_H 1



//  回应包长度限制
#define TH_TEMI2500_MIN_RESPONSE_CMD_NUM        3 // 3帧   // 回应包最小数量限制
#define TH_TEMI2500_MAX_RESPONSE_CMD_NUM        9 // 9帧   // 回应包最大数量限制
#define TH_TEMI2500_MIN_RESPONSE_CMD_LEN		(TH_TEMI2500_MIN_RESPONSE_CMD_NUM*TH_TEMI2500_MAX_COM_READ_REPLY)    // 3帧*N  // 回应包最小长度限制
#define TH_TEMI2500_MAX_RESPONSE_CMD_LEN		(TH_TEMI2500_MAX_RESPONSE_CMD_NUM*TH_TEMI2500_MAX_COM_READ_REPLY)    // 9帧*N  // 回应包最大长度限制


#define TH_TEMI2500_MAX_COM_WRITE_CMD	     (1+2+30)    // CMD(30 byte)+HEAD(1 byte)+End(2 byte)        
#define TH_TEMI2500_MAX_COM_WRITE_REPLY	     (1+2+10)    // REPLY(10 byte)+HEAD(1 byte)+End(2 byte)      
#define TH_TEMI2500_MAX_COM_READ_CMD	     (1+2+20)    // CMD(20 byte)+HEAD(1 byte)+End(2 byte)        
#define TH_TEMI2500_MAX_COM_READ_REPLY	     (1+2+20)    // REPLY(20 byte)+HEAD(1 byte)+End(2 byte)      
#define TH_TEMI2500_MAX_COM_RUN_CMD	         (1+2+30)    // CMD(30 byte)+HEAD(1 byte)+End(2 byte)        
#define TH_TEMI2500_MAX_COM_STOP_CMD	     (1+2+30)    // CMD(30 byte)+HEAD(1 byte)+End(2 byte)        

#define TH_TEMI2500_WRITE_CMD_POINT_TEMPVAlUE	 (15)    // Write Value point (15)       
#define TH_TEMI2500_READ_REPLY_POINT_TEMPVAlUE	 (10)    // Read REPLY Value point (10)   
#define TH_TEMI2500_WRITE_CMD_POINT_HUMIVAlUE	 (25)    // Write Value point (25)         
#define TH_TEMI2500_READ_REPLY_POINT_HUMIVAlUE	 (15)    // Read REPLY Value point (15)   

/*
//  特别注意：当使用运行和停止命令，跟读和写命令不一致时，机制必须修改。打开这个定义。
#define COM_RUN4READ_CMD_DIFFER	     (1) // 停与写，行与读不一致时,开启。Open this define for Difference.
#define COM_TEMP4HUMI_CMD	         (1) // 既控制温度又控制湿度时。Open this define temperature and humi.
#define COM_CMD_CHANNEL_DIFFER       (1) // 使用多个不同通道号时开启标示1到4，否则默认为1号通道。Open this define multi channel.
*/

class TH_TEMI2500
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
