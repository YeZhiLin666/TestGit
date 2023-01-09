/*****************************************************************************
// 程序文件      :TH_CLCD_9700.h    定义温控箱中各种处理头文件
// 文件描述      :为LPC4078 AUX-MPU board服务
// 编写日期      :2015.08.13
// 编写人        ：张翼飞
*****************************************************************************/
/** TH_CLCD_9700.h **/

#ifndef TH_CLCD_9700_H
#define TH_CLCD_9700_H 1



//  回应包长度限制
#define TH_CLCD_9700_MIN_RESPONSE_CMD_NUM        3 // 3帧   // 回应包最小数量限制
#define TH_CLCD_9700_MAX_RESPONSE_CMD_NUM        9 // 9帧   // 回应包最大数量限制
#define TH_CLCD_9700_MIN_RESPONSE_CMD_LEN		(TH_CLCD_9700_MIN_RESPONSE_CMD_NUM*TH_CLCD_9700_MAX_COM_READ_REPLY)    // 3帧*N  // 回应包最小长度限制
#define TH_CLCD_9700_MAX_RESPONSE_CMD_LEN		(TH_CLCD_9700_MAX_RESPONSE_CMD_NUM*TH_CLCD_9700_MAX_COM_READ_REPLY)    // 9帧*N  // 回应包最大长度限制


#define TH_CLCD_9700_MAX_COM_WRITE_CMD	     		(35+1)    // CMD(35 byte)+End(1 byte)        
#define TH_CLCD_9700_MAX_COM_WRITE_REPLY	     	(10+2)    // REPLY(10 byte)+End(2 byte)      
#define TH_CLCD_9700_MAX_COM_READ_CMD	     		(8+1)    // CMD(8 byte)+End(1 byte)        
#define TH_CLCD_9700_MAX_COM_READ_REPLY	     		(78+2)    // REPLY(78 byte)+End(2 byte)      
#define TH_CLCD_9700_MAX_COM_RUN_CMD	         	(11+1)    // CMD(11 byte)+End(1 byte)        
#define TH_CLCD_9700_MAX_COM_STOP_CMD_1	     		(11+1)    // CMD(11 byte)+End(1 byte)
#define TH_CLCD_9700_MAX_COM_STOP_CMD_2	     		(11+1)    // CMD(11 byte)+End(1 byte)


/*
//  特别注意：当使用运行和停止命令，跟读和写命令不一致时，机制必须修改。打开这个定义。
#define COM_RUN4READ_CMD_DIFFER	     (1) // 停与写，行与读不一致时,开启。Open this define for Difference.
#define COM_TEMP4HUMI_CMD	         (1) // 既控制温度又控制湿度时。Open this define temperature and humi.
#define COM_CMD_CHANNEL_DIFFER       (1) // 使用多个不同通道号时开启标示1到4，否则默认为1号通道。Open this define multi channel.
*/

class TH_CLCD_9700
{
public:
    static void Init(unsigned char ChanNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static short int asc2int(unsigned char asc1,unsigned char asc2,unsigned char asc3,unsigned char asc4);
    static void AddTemp(short int * pValue, unsigned char* str);
    static void AddCheck(unsigned int bits, unsigned char* str);

    //bool uart_getcmd (unsigned char ChanNo,unsigned char *pCmd, unsigned int len);
};



#endif
