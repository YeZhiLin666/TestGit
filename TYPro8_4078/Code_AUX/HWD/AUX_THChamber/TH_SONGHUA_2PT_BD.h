/*****************************************************************************
// 程序文件      :TH_SONGHUA_2PT_BD.H    定义温控箱中各种处理头文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2018.12.04
*****************************************************************************/
/** TH_SONGHUA_2PT_BD.h **/



#ifndef TH_SONGHUA_2PT_BD_H
#define TH_SONGHUA_2PT_BD_H 1

//  回应包长度限制
#define TH_SONGHUA_2PT_BD_MIN_RESPONSE_CMD_NUM        1 // 3帧   // 回应包最小数量限制
#define TH_SONGHUA_2PT_BD_MIN_RESPONSE_CMD_LEN		(TH_SONGHUA_2PT_BD_MIN_RESPONSE_CMD_NUM*TH_SONGHUA_2PT_BD_MAX_COM_READ_REPLY)    // 3帧*N  // 回应包最小长度限制

// 发送长度和接收长度定义： 
#define TH_SONGHUA_2PT_BD_MAX_COM_READ_REPLY	     (7)    // REPLY(6 byte)+DATA(16 byte)   


class TH_SONGHUA_2PT_BD
{
public:
    static void Init(unsigned char ChanNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
private:
	static unsigned char m_RunState;
};


#endif
