/*****************************************************************************
// 程序文件      :TH_SAMWONTECH_TEMI2500.h    定义温控箱中各种处理头文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2010.12.01
*****************************************************************************/
/** TH_SAMWONTECH_TEMI2500.h **/

#ifndef TH_MT8071_MODBUS_H
#define TH_MT8071_MODBUS_H

#define TH_MT8071_MODBUS_MIN_RESPONSE_CMD_LEN 7


class TH_MT8071_MODBUS
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
