/*****************************************************************************
// 程序文件      :TC_ESPEC_GPL_4.h    定义温控箱中各种控制命令头文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2011.12.01
*****************************************************************************/

#define TC_GPL_4_MIN_RESPONSE_CMD_LEN		30
#define TC_GPL_4_MAX_RESPONSE_CMD_LEN		90

class TC_GPL_4
{
public:
	static void Init(unsigned char ChanNo);
	static void Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
	static void SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
	static void SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
	static void SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
	static void SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
};
