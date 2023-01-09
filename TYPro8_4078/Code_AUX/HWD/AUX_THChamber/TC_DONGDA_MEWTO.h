/*****************************************************************************
// 程序文件      :TC_DONGDA_MEWTO.H    定义温控箱中各种处理头文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2021.9.14
*****************************************************************************/
/** DONGDA.h **/



#ifndef T_DONGDA_H
#define T_DONGDA_H 1
//DONGDA  TCY1023

#define TC_DONGDA_ASCII_FIRST         (0X01)  // '1'   串口板第一个开始的串口编号从1开始


//  回应包长度限制
#define TC_DONGDA_MIN_RESPONSE_CMD_NUM        1 // 3帧   // 回应包最小数量限制
#define TC_DONGDA_MAX_RESPONSE_CMD_NUM        9 // 9帧   // 回应包最大数量限制
#define TC_DONGDA_MIN_RESPONSE_CMD_LEN		(TC_DONGDA_MIN_RESPONSE_CMD_NUM*TC_DONGDA_MAX_COM_READ_REPLY)    // 3帧*N  // 回应包最小长度限制
#define TC_DONGDA_MAX_RESPONSE_CMD_LEN		(TC_DONGDA_MAX_RESPONSE_CMD_NUM*TC_DONGDA_MAX_COM_READ_REPLY)    // 9帧*N  // 回应包最大长度限制




// 发送长度和接收长度定义：
#define TC_DONGDA_MAX_COM_WRITE_CMD	     (24)   // CMD(19 byte)+DATA(4 byte)    
#define TC_DONGDA_MAX_COM_WRITE_REPLY	     (23)   // REPLY(23 byte)  
#define TC_DONGDA_MAX_COM_READ_CMD	         (20)   // CMD(19 byte)  
#define TC_DONGDA_MAX_COM_READ_REPLY	     (12)   // REPLY(8 byte)+DATA(4 byte)  
#define TC_DONGDA_MAX_COM_RUN_CMD	         (24)   // CMD(23 byte)    
#define TC_DONGDA_MAX_COM_STOP_CMD	         (24)   // CMD(23 byte)     

#define HUMI_INTERVALTIME	     (6000)   // 湿度间隔时间6秒，如里没设置自动切换温度模式

class TC_DONGDA
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
