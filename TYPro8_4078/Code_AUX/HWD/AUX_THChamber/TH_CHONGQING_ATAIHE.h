/*****************************************************************************
// 程序文件      :TH_ATAIHE.h    定义温控箱中各种处理头文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/
/** ctrl.h **/

#ifndef TH_ATAIHE_H
#define TH_ATAIHE_H 1

//  回应包长度限制
#define TH_ATAIHE_MIN_RESPONSE_CMD_LEN		2*TH_VOETSCH_MAX_COM_READ_REPLY    // 15  // 回应包最小长度限制
#define TH_ATAIHE_MAX_RESPONSE_CMD_LEN		6*TH_VOETSCH_MAX_COM_READ_REPLY  //  4*MAX_COM_READ_REPLY    // 40  // 回应包最大长度限制



#define TH_ATAIHE_MAX_COM_WRITE_CMD	       (29+1)    // CMD(29 byte)+""(1 byte)       
#define TH_ATAIHE_MAX_COM_WRITE_REPLY	     (58+1)    // REPLY(58byte)+""(1 byte)    
#define TH_ATAIHE_MAX_COM_READ_CMD	       (21+1)   // CMD(21 byte)+""(1 byte)        
#define TH_ATAIHE_MAX_COM_READ_REPLY	     (37+1)    // REPLY(37)+""(1 byte)    

#define TH_ATAIHE_MAX_COM_STOP_CMD	       (19+1)    // CMD(19 byte)+""(1 byte)      
#define TH_ATAIHE_MAX_COM_RUN_CMD	         (19+1)    // CMD(19 byte)+""(1 byte)       

#define TH_ATAIHE_READ_REPLY_POINT_VAlUE_T	 (21)    // Read REPLY Value point T (21)       
#define TH_ATAIHE_READ_REPLY_POINT_VAlUE_H	 (29)    // Read REPLY Value point H (29)   

#define TH_ATAIHE_WRITE_CMD_POINT_VAlUE_T	 (20)    // Write Value point T (20)       
#define TH_ATAIHE_WRITE_CMD_POINT_VAlUE_H	 (20)    // Write Value point H (20)   

/*
//  富奇温控箱运行和停止命令，跟读和写命令一致。
//  特别注意：当使用运行和停止命令，跟读和写命令不一致时，机制必须修改。打开这个定义。
#define COM_RUN4READ_CMD_DIFFER	     (1) // 停与写，行与读不一致时,开启。Open this define for Difference.
//#define COM_TEMP4HUMI_CMD	         (1) // 既控制温度又控制湿度时。Open this define temperature and humi.
//#define COM_CMD_CHANNEL_DIFFER       (1) // 使用多个不同通道号时开启标示1到4，否则默认为1号通道。Open this define multi channel.
*/

class TH_ATAIHE
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
