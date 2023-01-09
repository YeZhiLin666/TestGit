/*****************************************************************************
// 程序文件      :TH_SHANGHAI_PINDUN_TEMI880.H    定义温控箱中各种处理头文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/
/** TH_SHANGHAI_PINDUN_TEMI880.h **/



#ifndef TH_SHANGHAI_PINDUN_TEMI880_H
#define TH_SHANGHAI_PINDUN_TEMI880_H 1


#define SHANGHAI_PINDUN_TEMI880_ASCII_FIRST         (0X01)  // '1'   串口板第一个开始的串口编号从1开始


//  回应包长度限制
#define TH_SHANGHAI_PINDUN_TEMI880_MIN_RESPONSE_CMD_NUM        1 // 3帧   // 回应包最小数量限制
#define TH_SHANGHAI_PINDUN_TEMI880_MAX_RESPONSE_CMD_NUM        9 // 9帧   // 回应包最大数量限制
#define TH_SHANGHAI_PINDUN_TEMI880_MIN_RESPONSE_CMD_LEN		(TH_SHANGHAI_PINDUN_TEMI880_MIN_RESPONSE_CMD_NUM*TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_READ_REPLY)    // 3帧*N  // 回应包最小长度限制
#define TH_SHANGHAI_PINDUN_TEMI880_MAX_RESPONSE_CMD_LEN		(TH_SHANGHAI_PINDUN_TEMI880_MAX_RESPONSE_CMD_NUM*TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_READ_REPLY)    // 9帧*N  // 回应包最大长度限制




// 发送长度和接收长度定义：
#define TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_WRITE_CMD	     (14+6)   // CMD(14 byte)+DATA(6 byte)    
#define TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_WRITE_REPLY	     (14+6)   // REPLY(20 byte)  
#define TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_READ_CMD	         (6)     // CMD(6 byte)  
#define TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_READ_REPLY	     (5+16)    // REPLY(5 byte)+DATA(16 byte)  
#define TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_RUN_CMD	         (10)   // CMD(10 byte)    
#define TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_STOP_CMD	         (10)   // CMD(10 byte)     

#define HUMI_INTERVALTIME	     (6000)   // 湿度间隔时间6秒，如里没设置自动切换温度模式

class TH_SHANGHAI_PINDUN_TEMI880
{
public:
    static void Init(unsigned char ChanNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);  // 入口
    static void WriteCmd_Code(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);  // 分支
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);

    static unsigned char SetPresentValue_Check(unsigned char ChanNo, short int Value);
};


#endif
