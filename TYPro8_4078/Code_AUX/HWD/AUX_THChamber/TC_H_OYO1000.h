/*****************************************************************************
// 程序文件      :TC_H_OYO1000.h    定义温控箱中各种处理头文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2022.10.10
*****************************************************************************/
/** TC_H_OYO1000.h **/

#ifndef TC_H_OYO1000_H
#define TC_H_OYO1000_H

#define TC_H_OYO1000_MIN_RESPONSE_CMD_LEN 7

class TC_H_OYO1000
{
public:
    static void Init(unsigned char ChanNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
};

#endif
