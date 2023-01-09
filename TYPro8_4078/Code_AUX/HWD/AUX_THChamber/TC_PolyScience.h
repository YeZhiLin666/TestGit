/*****************************************************************************
// 程序文件      :TC_POLYSCIENCE.h    定义温控箱中各种控制命令头文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2011.12.01
*****************************************************************************/

#define TC_POLYSCIENCE_CA02A1P1_MIN_RESPONSE_CMD_LEN		7 //30
#define TC_POLYSCIENCE_CA02A1P1_MAX_RESPONSE_CMD_LEN		14 //90

#define TC_POLYSCIENCE_SEND_MAX_BUF_10BYTE    10  //

enum{
	READ_POLYSCIENCE_PV_TEMPERATURE= 0,
	READ_POLYSCIENCE_PROBE1_TEMPERATURE = 1,
	READ_POLYSCIENCE_PROBE2_TEMPERATURE = 2,
	READ_POLYSCIENCE_PSI_PRESSURE= 3,
	READ_POLYSCIENCE_KPA_PRESSURE= 4,
	READ_POLYSCIENCE_GPM_FLOW= 5,
	READ_POLYSCIENCE_LPM_FLOW= 6,
	READ_POLYSCIENCE_LINE_VOLTAGE= 7,
	READ_POLYSCIENCE_REMOTE_PROBE_TEMPERATURE = 8,
	READ_POLYSCIENCE_AMBIENT_TEMPERATURE = 9,
	READ_POLYSCIENCE_FLUID_LEVEL = 10,
	READ_POLYSCIENCE_COMPRESSOR_AMPERAGE = 11,
	READ_POLYSCIENCE_PUMP_AMPERAGE = 12,
	READ_POLYSCIENCE_RELATIVE_HUMIDITY = 13,
};

class TC_POLYSCIENCE_CA02A1P1
{
public:
	static unsigned char uc_Read_T_P_F_H_Cmd;
	static unsigned char uc_Read_OverTime;
	static unsigned char uc_COM_BoardID;
	static void Init(unsigned char ChanNo);
	static void Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
	static void SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
	static void SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
	static void SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
	static void SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
};
