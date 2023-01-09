/*****************************************************************************
// 程序文件      :TH_SIEMENS_S7_226CN.H    定义温控箱中各种处理头文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/
/** TH_SIEMENS_S7_226CN.h **/

#ifndef TH_SIEMENS_S7_226CN_H
#define TH_SIEMENS_S7_226CN_H 1


#define S7_226CN_ASCII_FIRST         (0X30)  // '0'   串口板第一个开始的串口编号  西门子S7-200 比较特别从0开始


//  回应包长度限制
#define TH_S7_226CN_MIN_RESPONSE_CMD_NUM        1 // 3帧   // 回应包最小数量限制
#define TH_S7_226CN_MAX_RESPONSE_CMD_NUM        9 // 9帧   // 回应包最大数量限制
#define TH_S7_226CN_MIN_RESPONSE_CMD_LEN		(TH_S7_226CN_MIN_RESPONSE_CMD_NUM*TH_S7_226CN_MAX_COM_READ_REPLY)    // 3帧*N  // 回应包最小长度限制
#define TH_S7_226CN_MAX_RESPONSE_CMD_LEN		(TH_S7_226CN_MAX_RESPONSE_CMD_NUM*TH_S7_226CN_MAX_COM_READ_REPLY)    // 9帧*N  // 回应包最大长度限制




// 发送长度和接收长度定义：
#define TH_S7_226CN_MAX_COM_WRITE_CMD	     (10+8)   // CMD(10 byte)+DATA(8 byte)    
#define TH_S7_226CN_MAX_COM_WRITE_REPLY	     (10)     // REPLY(10 byte)  
#define TH_S7_226CN_MAX_COM_READ_CMD	     (10)     // CMD(10 byte)  
#define TH_S7_226CN_MAX_COM_READ_REPLY	     (10+128)    // REPLY(10 byte)+DATA(128 byte)  
#define TH_S7_226CN_MAX_COM_RUN_CMD	         (10+8)   // CMD(10 byte)+DATA(8 byte)      
#define TH_S7_226CN_MAX_COM_STOP_CMD	     (10+8)   // CMD(10 byte)+DATA(8 byte)      

#define HUMI_INTERVALTIME	     (6000)   // 湿度间隔时间6秒，如里没设置自动切换温度模式

class TH_S7_226CN
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
