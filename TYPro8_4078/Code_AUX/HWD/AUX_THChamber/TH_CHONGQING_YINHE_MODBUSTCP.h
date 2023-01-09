/*****************************************************************************
// 程序文件      :TH_CHONGQING_YINHE_MODBUSTCP.H    定义温控箱中各种处理头文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2017.10.10
*****************************************************************************/
/** TH_CHONGQING_YINHE_MODBUSTCP.h **/

#ifndef TH_CHONGQING_YINHE_MODBUSTCP_H
#define TH_YINHE_MODBUSTCP_H 1

//  回应包长度限制
#define TH_YINHE_MODBUSTCP_MIN_RESPONSE_CMD_NUM        1 // 1帧   // 回应包最小数量限制
#define TH_YINHE_MODBUSTCP_MAX_RESPONSE_CMD_NUM        9 // 9帧   // 回应包最大数量限制
#define TH_YINHE_MODBUSTCP_MIN_RESPONSE_CMD_LEN		(TH_YINHE_MODBUSTCP_MIN_RESPONSE_CMD_NUM*TH_YINHE_MODBUSTCP_MAX_COM_READ_REPLY)    // 1帧*N  // 回应包最小长度限制
#define TH_YINHE_MODBUSTCP_MAX_RESPONSE_CMD_LEN		(TH_YINHE_MODBUSTCP_MAX_RESPONSE_CMD_NUM*TH_YINHE_MODBUSTCP_MAX_COM_READ_REPLY)    // 9帧*N  // 回应包最大长度限制




// 发送长度和接收长度定义：
#define TH_YINHE_MODBUSTCP_MAX_COM_WRITE_CMD	         (7+10)  // MBAP(7 byte)+CMD(10 byte)  
#define TH_YINHE_MODBUSTCP_MAX_COM_WRITE_REPLY	       (7+5)   // MBAP(7 byte)+REPLY(5 byte)
#define TH_YINHE_MODBUSTCP_MAX_COM_READ_CMD	           (7+5)   // MBAP(7 byte)+CMD(5 byte)  
#define TH_YINHE_MODBUSTCP_MAX_COM_READ_REPLY	         (7+6)   // MBAP(7 byte)+REPLY(6 byte)
#define TH_YINHE_MODBUSTCP_MAX_COM_RUN_CMD	           (7+8)   // MBAP(7 byte)+CMD(8 byte) 
#define TH_YINHE_MODBUSTCP_MAX_COM_STOP_CMD	           (7+8)   // MBAP(7 byte)+CMD(8 byte)
#define TH_YINHE_MODBUSTCP_MAX_COM_RUNorSTOP_REPLY	   (7+5)   // MBAP(7 byte)+REPLY(5 byte)


class TH_YINHE_MODBUSTCP
{
public:
    static void Init(unsigned char ChanNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);

    static unsigned char SetPresentValue_Check(unsigned char ChanNo, short int Value);
};

#endif
