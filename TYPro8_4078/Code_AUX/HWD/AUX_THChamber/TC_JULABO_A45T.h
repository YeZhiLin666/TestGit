/*****************************************************************************
// 程序文件      :TC_JULABO_A45T.h    定义温控箱中各种控制命令头文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2018.10.11
*****************************************************************************/

/** Modify from Chamber.h **/
#define TC_JULABO_A45T_START_CMD_LEN 14
#define TC_JULABO_A45T_SELECT_CMD_LEN 14
#define TC_JULABO_A45T_SET_TMEP_CMD_LEN 16
#define TC_JULABO_A45T_READ_TMEP_CMD_LEN 9
#define TC_JULABO_A45T_STOP_CMD_LEN 14

#define TC_JULABO_A45T_MIN_RESPONSE_CMD_LEN		14
#define TC_JULABO_A45T_MAX_RESPONSE_CMD_LEN		15

#define TC_JULABO_A45T_INPUT_FIRST		1    //  第一通道输入   // 写 0x012C = 300   读 0x0064 = 100
#define TC_JULABO_A45T_INPUT_THIRD 		3    //  第三通道输入   // 写 0x0272 = 626   读 0x006C = 108

#define TC_JULABO_A45T_INPUT_CH		TC_JULABO_A45T_INPUT_FIRST

class TC_JULABO_A45T
{
public:
    static void Init(unsigned char ChanNo);
    static void Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
};


