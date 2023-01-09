/*****************************************************************************
// �����ļ�      :TC_WATLOW_F4T_MODBUS_TCP.h    �����¿����и��ֿ�������ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2011.11.04
*****************************************************************************/

/** Modify from Chamber.h **/



#define TC_WATLOW_F4T_MODBUS_TCP_MIN_RESPONSE_CMD_LEN		11
#define TC_WATLOW_F4T_MODBUS_TCP_MAX_RESPONSE_CMD_LEN		40

#define TC_WATLOW_F4T_MODBUS_TCP_INPUT_FIRST		1    //  ��һͨ������   // д 0x012C = 300   �� 0x0064 = 100
#define TC_WATLOW_F4T_MODBUS_TCP_INPUT_THIRD 		3    //  ����ͨ������   // д 0x0272 = 626   �� 0x006C = 108

//20150324�ӵ�һͨ�������ɵ���ͨ������
//#define TC_WATLOW_F4T_MODBUS_TCP_INPUT_CH		TC_WATLOW_F4T_MODBUS_TCP_INPUT_THIRD     //  // 20150324  YY
#define TC_WATLOW_F4T_MODBUS_TCP_INPUT_CH		TC_WATLOW_F4T_MODBUS_TCP_INPUT_FIRST

class TC_WATLOW_F4T_MODBUS_TCP
{
public:
    static void Init(unsigned char ChanNo);
    static void Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
    static void SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
};

//bool TC_EZT570I_uart_getcmd (unsigned char ChanNo,unsigned char *pCmd, unsigned int len);


