
#ifndef _CANMSGBUFFERING_H_
#define _CANMSGBUFFERING_H_


#define CAN_CONFIRM_BUFFER_SIZE	1000
#define CAN_MSG_LENGTH		16
#define  MAX_MSG_NO        4096
#define MAX_CAN_PORTS	2
struct ST_CAN_CONFIRM_BUFFER
{
    uint32 MsgNo;
    int  MsgLen;
    uint8 EventContent[CAN_MSG_LENGTH];

};

class CANMsgBuffering
{
public:

    static UInt16 Priority  ;  //  PRIORITYBG
    static UInt16 TaskID ;
    static MP_CAN_MSG ReSend_CAN_Msg;
    // ���ñ������������
    /*#if HARDWARE_SDRAM_32M == 1
        #pragma arm section rwdata = "SRAM",zidata = "SRAM"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x01F40000
    #else
        #pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x03E80000
    #endif */

    // =====CAN��=====
    static MsTime              MyCounter_CAN[MAX_CAN_PORTS];
    static ST_CAN_CONFIRM_BUFFER    Confirm_Resend_Buf[MAX_CAN_PORTS][2][CAN_CONFIRM_BUFFER_SIZE];
    static int                 Buf_Active[MAX_CAN_PORTS]; // ��Ծ��ר���ڴ洢���͵��¼�
    static int					Buf_Inactive_CAN[MAX_CAN_PORTS];  // �ǻ�Ծ��ר�����ط���
    static int                 InUse_Idx_CAN[MAX_CAN_PORTS]; // ��Ծ��ָ���λ��
    static unsigned char        Retry_Msg_Cnt[MAX_CAN_PORTS][MAX_MSG_NO];
    //	#pragma arm section //��������������

    static void MyInit(void);
    static void DoWork(void);
    static int ConfirmMsg(unsigned char CANx,uint32 eventNo );
    static void MoveMsg2Active(unsigned char CANx,int source, int destination);	// move(not copy) from inactive buffer to active buffer.
    static int FindNextEmpty(unsigned char CANx,int startPoint );									// only in Active Buffer
    static void SendDataPack(unsigned char CANx);
    static int Push2ConfirmBuf(unsigned char CANx,uint8 * content, int MsgLen, uint32 MsgNo);
private:

};
#endif
