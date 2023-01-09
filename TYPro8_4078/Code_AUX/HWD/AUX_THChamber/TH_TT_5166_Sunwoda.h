/*****************************************************************************
// 程序文件      :TH_TT_5166_Sunwoda.H    定义温控箱中各种处理头文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2017.10.10
*****************************************************************************/
/** TH_TT_5166_Sunwoda.h **/

#ifndef TH_TT_5166_Sunwoda_H
#define TH_TT_5166_Sunwoda_H 1

//  回应包长度限制
#define TH_TT_5166_Sunwoda_MIN_RESPONSE_CMD_NUM        1 // 3帧   // 回应包最小数量限制
#define TH_TT_5166_Sunwoda_MAX_RESPONSE_CMD_NUM        9 // 9帧   // 回应包最大数量限制
#define TH_TT_5166_Sunwoda_MIN_RESPONSE_CMD_LEN		(TH_TT_5166_Sunwoda_MIN_RESPONSE_CMD_NUM*TH_TT_5166_Sunwoda_MAX_COM_READ_REPLY)    // 3帧*N  // 回应包最小长度限制
#define TH_TT_5166_Sunwoda_MAX_RESPONSE_CMD_LEN		(TH_TT_5166_Sunwoda_MAX_RESPONSE_CMD_NUM*TH_TT_5166_Sunwoda_MAX_COM_READ_REPLY)    // 9帧*N  // 回应包最大长度限制




// 发送长度和接收长度定义：
#define TH_TT_5166_Sunwoda_MAX_COM_WRITE_CMD	     (6+2)   // CMD(6 byte)+CRC(2 byte)    
#define TH_TT_5166_Sunwoda_MAX_COM_WRITE_REPLY	     (6+2)   // REPLY(6 byte)+CRC(2 byte)   
#define TH_TT_5166_Sunwoda_MAX_COM_READ_CMD	         (6+2)   // CMD(6 byte)+CRC(2 byte)    
#define TH_TT_5166_Sunwoda_MAX_COM_READ_REPLY	     (5+2)   // REPLY(5 byte)+CRC(2 byte)  
#define TH_TT_5166_Sunwoda_MAX_COM_RUN_CMD	         (6+2)   // CMD(6 byte)+CRC(2 byte)    
#define TH_TT_5166_Sunwoda_MAX_COM_STOP_CMD	         (6+2)   // CMD(6 byte)+CRC(2 byte)    


class TH_TT_5166_Sunwoda
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
