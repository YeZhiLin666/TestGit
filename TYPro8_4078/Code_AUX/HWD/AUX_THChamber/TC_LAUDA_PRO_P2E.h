/*****************************************************************************
// �����ļ�      :TC_LAUDA_PRO_P2E.h    �����¿����и��ֿ�������ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2019.04.02
*****************************************************************************/

/** Modify from Chamber.h **/
#define TC_LAUDA_PRO_P2E_START_CMD_LEN 9
#define TC_LAUDA_PRO_P2E_SELECT_CMD_LEN 16
#define TC_LAUDA_PRO_P2E_SET_TMEP_CMD_LEN 19
#define TC_LAUDA_PRO_P2E_READ_TMEP_CMD_LEN 11
#define TC_LAUDA_PRO_P2E_SET_HUMI_CMD_LEN 14
#define TC_LAUDA_PRO_P2E_READ_HUMI_CMD_LEN 11
#define TC_LAUDA_PRO_P2E_STOP_CMD_LEN 7

#define TC_LAUDA_PRO_P2E_MIN_RESPONSE_CMD_LEN		14
#define TC_LAUDA_PRO_P2E_MAX_RESPONSE_CMD_LEN		15

#define TC_LAUDA_PRO_P2E_INPUT_FIRST		1    //  ��һͨ������   // д 0x012C = 300   �� 0x0064 = 100
#define TC_LAUDA_PRO_P2E_INPUT_THIRD 		3    //  ����ͨ������   // д 0x0272 = 626   �� 0x006C = 108

#define TC_LAUDA_PRO_P2E_INPUT_CH		TC_LAUDA_PRO_P2E_INPUT_FIRST

class TC_LAUDA_PRO_P2E
{
public:
	static void Init(unsigned char ChanNo);
	static void Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
	static void SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
	static void SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
	static void SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
	static void SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag);
public:
	static unsigned char TE_AuxChannalNo;
	static unsigned int CountHumi;
  static unsigned int CountTempInternal;
  static unsigned int CountTempExternal;
};
//bool TC_JULABO_FP50MA_F4_getcmd (unsigned char ChanNo,unsigned char *pCmd, unsigned int len);
