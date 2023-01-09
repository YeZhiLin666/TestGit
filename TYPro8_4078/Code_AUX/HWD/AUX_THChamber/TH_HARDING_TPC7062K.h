/*****************************************************************************
// 程序文件      :TH_HARDING_TPC7062K.h    定义温控箱中各种处理头文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2014.04.08
// 针对特定设备  :ARBIN控制重庆哈丁科技TPC7062K说明
*****************************************************************************/
/** TC_HARDING_TPC7062K.h **/

#ifndef TH_HARDING_TPC7062K_H
#define TH_HARDING_TPC7062K_H 1




//  回应包长度限制
#define TH_TPC7062K_MIN_RESPONSE_CMD_NUM        3 // 3帧   // 回应包最小数量限制
#define TH_TPC7062K_MAX_RESPONSE_CMD_NUM        9 // 9帧   // 回应包最大数量限制
#define TH_TPC7062K_MIN_RESPONSE_CMD_LEN		(TH_TPC7062K_MIN_RESPONSE_CMD_NUM*TH_TPC7062K_MAX_COM_READ_REPLY)    // 3帧*N  // 回应包最小长度限制
#define TH_TPC7062K_MAX_RESPONSE_CMD_LEN		(TH_TPC7062K_MAX_RESPONSE_CMD_NUM*TH_TPC7062K_MAX_COM_READ_REPLY)    // 9帧*N  // 回应包最大长度限制



#define TH_TPC7062K_MAX_COM_MODE_CMD	     (1+2+14)    // CMD(14 byte)+HEAD(1 byte)+End(2 byte)        
#define TH_TPC7062K_MAX_COM_MODE_REPLY	     (1+2+8)     // REPLY(8 byte)+HEAD(1 byte)+End(2 byte)   
#define TH_TPC7062K_MAX_COM_WRITE_CMD	     (1+2+18)    // CMD(18 byte)+HEAD(1 byte)+End(2 byte)        
#define TH_TPC7062K_MAX_COM_WRITE_REPLY	     (1+2+8)     // REPLY(8 byte)+HEAD(1 byte)+End(2 byte)          
#define TH_TPC7062K_MAX_COM_READ_CMD	     (1+2+14)    // CMD(14 byte)+HEAD(1 byte)+End(2 byte)        
#define TH_TPC7062K_MAX_COM_READ_REPLY	     (1+2+32)    // REPLY(32 byte)+HEAD(1 byte)+End(2 byte)     
#define TH_TPC7062K_MAX_COM_RUN_CMD	         (1+2+14)    // CMD(14 byte)+HEAD(1 byte)+End(2 byte)        
#define TH_TPC7062K_MAX_COM_STOP_CMD	     (1+2+14)    // CMD(14 byte)+HEAD(1 byte)+End(2 byte)         

#define TH_TPC7062K_WRITE_CMD_POINT_TEMPVAlUE	 (9)    // Write TEMPVAlUE Value point (9)        
#define TH_TPC7062K_READ_REPLY_POINT_TEMPVAlUE	 (8)     // Read REPLY TEMPVAlUE Value point (11-3)    
#define TH_TPC7062K_WRITE_CMD_POINT_HUMIVAlUE	 (13)    // Write HUMIVAlUE Value point (13)        
#define TH_TPC7062K_READ_REPLY_POINT_HUMIVAlUE	 (16)    // Read REPLY HUMIVAlUE Value point (19-3)   

/*
//  特别注意：当使用运行和停止命令，跟读和写命令不一致时，机制必须修改。打开这个定义。
#define COM_RUN4READ_CMD_DIFFER	     (1) // 停与写，行与读不一致时,开启。Open this define for Difference.
#define COM_TEMP4HUMI_CMD	         (1) // 既控制温度又控制湿度时。Open this define temperature and humi.
#define COM_CMD_CHANNEL_DIFFER       (1) // 使用多个不同通道号时开启标示1到4，否则默认为1号通道。Open this define multi channel.
*/

class TH_TPC7062K
{
public:
    static void Init(unsigned char ChanNo);
    static void ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendModeCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWarnLimitCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag);
    static unsigned char LoadCheckSum(unsigned char CMD_Code,unsigned char *puchMsg, unsigned int usDataLen);
};

#endif
