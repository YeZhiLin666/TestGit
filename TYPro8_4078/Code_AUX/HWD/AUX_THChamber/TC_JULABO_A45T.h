/*****************************************************************************
// �����ļ�      :TC_JULABO_A45T.h    �����¿����и��ֿ�������ͷ�ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2018.10.11
*****************************************************************************/

/** Modify from Chamber.h **/
#define TC_JULABO_A45T_START_CMD_LEN 14
#define TC_JULABO_A45T_SELECT_CMD_LEN 14
#define TC_JULABO_A45T_SET_TMEP_CMD_LEN 16
#define TC_JULABO_A45T_READ_TMEP_CMD_LEN 9
#define TC_JULABO_A45T_STOP_CMD_LEN 14

#define TC_JULABO_A45T_MIN_RESPONSE_CMD_LEN		14
#define TC_JULABO_A45T_MAX_RESPONSE_CMD_LEN		15

#define TC_JULABO_A45T_INPUT_FIRST		1    //  ��һͨ������   // д 0x012C = 300   �� 0x0064 = 100
#define TC_JULABO_A45T_INPUT_THIRD 		3    //  ����ͨ������   // д 0x0272 = 626   �� 0x006C = 108

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


