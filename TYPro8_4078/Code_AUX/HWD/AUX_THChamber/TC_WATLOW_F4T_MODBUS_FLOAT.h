/*****************************************************************************
// 程序文件      :TC_WATLOW_F4T_MODBUS_FLOAT.h    定义温控箱中各种控制命令头文件
// 文件描述      :为LPC2468 AUX-MPU board服务
// 编写日期      :2011.11.04
*****************************************************************************/

/** Modify from Chamber.h **/


#define TC_WATLOW_F4T_MODBUS_FLOAT_MIN_RESPONSE_CMD_LEN		9
#define TC_WATLOW_F4T_MODBUS_FLOAT_MAX_RESPONSE_CMD_LEN		40

#define TC_WATLOW_F4T_MODBUS_FLOAT_INPUT_FIRST		1    //  第一通道输入   // 写 0x012C = 300   读 0x0064 = 100
#define TC_WATLOW_F4T_MODBUS_FLOAT_INPUT_THIRD 		3    //  第三通道输入   // 写 0x0272 = 626   读 0x006C = 108

#define TC_WATLOW_F4T_MODBUS_FLOAT_INPUT_CH		TC_WATLOW_F4T_MODBUS_FLOAT_INPUT_FIRST

class TC_WATLOW_F4T_MODBUS_FLOAT
{
public:
    static void Init(unsigned char ChanNo);
    static void Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
};



