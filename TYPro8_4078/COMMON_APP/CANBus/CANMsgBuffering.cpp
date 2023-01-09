/*****************************************************************************
// �����ļ�                   :EventsBuffering.cpp     ��Ҫ�����¼�������ط�
// �ļ�����                   :Ϊ MCU board����
// ��д����(MM.DD.YYYY)       :08.05.2013
*****************************************************************************/

/*
#include "../Entry/includes.h"

UInt16 CANMsgBuffering::Priority = PRIORITY4;  //  PRIORITYBG
UInt16 CANMsgBuffering::TaskID = 	CANMSGBUFFERING_TASK;

MP_CAN_MSG CANMsgBuffering::ReSend_CAN_Msg;
// ���ñ������������
#if HARDWARE_SDRAM_32M == 1
    #pragma arm section rwdata = "SRAM",zidata = "SRAM"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x01F40000
#else
    #pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x03E80000
#endif

// =====CAN��=====
ST_CAN_CONFIRM_BUFFER    CANMsgBuffering:: Confirm_Resend_Buf[MAX_CAN_PORTS][2][CAN_CONFIRM_BUFFER_SIZE];
int                 CANMsgBuffering:: Buf_Active[MAX_CAN_PORTS]; // ��Ծ��ר���ڴ洢���͵��¼�
int					CANMsgBuffering::Buf_Inactive_CAN[MAX_CAN_PORTS];  // �ǻ�Ծ��ר�����ط���
int                 CANMsgBuffering::InUse_Idx_CAN[MAX_CAN_PORTS]; // ��Ծ��ָ���λ��
unsigned char       CANMsgBuffering::Retry_Msg_Cnt[MAX_CAN_PORTS][MAX_MSG_NO];
MsTime              CANMsgBuffering::MyCounter_CAN[MAX_CAN_PORTS];
#pragma arm section //��������������

//
int ResendCnt = 0;
int    EnterReCheckCnt[2] = {0,0};
//���� CAN1 = 0;CAN2 = 1
void CANMsgBuffering::MyInit(void)
{
	int i;
	CommonFunc::GetTimeTicksUpdate(&MyCounter_CAN[CAN1]);
	CommonFunc::GetTimeTicksUpdate(&MyCounter_CAN[CAN2]);
	Buf_Active[CAN1] = 0;
	Buf_Active[CAN2] = 0;

	for (i = 0; i < CAN_CONFIRM_BUFFER_SIZE; i ++)
	{ // ��Ծ��ר���ڴ洢���͵��¼����ǻ�Ծ��ר�����ط���
		Confirm_Resend_Buf[0][0][i].MsgNo = 0;
		Confirm_Resend_Buf[0][1][i].MsgNo = 0;
		Confirm_Resend_Buf[1][0][i].MsgNo = 0;
		Confirm_Resend_Buf[1][1][i].MsgNo = 0;
	}
	InUse_Idx_CAN[CAN1] = 0;
	InUse_Idx_CAN[CAN2] = 0;
}

void CANMsgBuffering::DoWork(void)
{
#if DEBUG_TASK_STATISTIC == 1
	OS_ServeProcess::Task_Count[COUNT_CANMSG_BUFFER] ++;
#endif

	//---------------------------
	// CAN������
	for(unsigned char uc_i =0;uc_i<MAX_CAN_PORTS;uc_i++)
	{
		if(CommonFunc::GetTimeTicks( &MyCounter_CAN[uc_i]) < 2000)	// perform only every 200ms
		{
			return;
		}
		SendDataPack(uc_i);
	}
}

void CANMsgBuffering::SendDataPack(unsigned char CANx)
{
	int i = 1, buf_Inactive2, empty_Idx;
	if(CANx!=CAN1 &&CANx!=CAN2)
		return;
	CommonFunc::GetTimeTicksUpdate(&MyCounter_CAN[CANx]);	 // reset counter
	// ��Ծ��ר���ڴ洢���͵��¼����ǻ�Ծ��ר�����ط���
	buf_Inactive2 = !(Buf_Active[CANx]);							// init variables for following operation
	//Buf_Inactive = buf_Inactive2;		// ��Ծ���л����ǻ�Ծ��
	empty_Idx = InUse_Idx_CAN[CANx]; // ��Ծ��ָ���λ��

	//NVIC_DisableIRQ(CAN_IRQn);
	EnterReCheckCnt[buf_Inactive2]++;
	for (i = 0; i < CAN_CONFIRM_BUFFER_SIZE; i ++)
	{

		if (Confirm_Resend_Buf[CANx][buf_Inactive2][i].MsgNo != 0)		// find the next event in Inactive buffer that is not confirmed
		{
			unsigned short UnConfMsgNo = Confirm_Resend_Buf[CANx][buf_Inactive2][i].MsgNo;
			ResendCnt++;
			Retry_Msg_Cnt[CANx][UnConfMsgNo]++;
			if(	Retry_Msg_Cnt[CANx][UnConfMsgNo] <10)  //��ʱδ����UnConfMsgNo>5000��CANЭ��MAX4096)
			{
				memcpy( (void*)&ReSend_CAN_Msg.FrameType,(void*)&Confirm_Resend_Buf[CANx][buf_Inactive2][i].EventContent,sizeof(MP_CAN_MSG));
				//ReSend_CAN_Msg.Content.Data[7] = 0xcc;          //�����Ƿ��ط���ȥ
				CANFunc::CAN_Push_to_Send(CANx,&ReSend_CAN_Msg);
				empty_Idx = FindNextEmpty(CANx,empty_Idx);				// find the next empty slot in active buffer
				if (empty_Idx != -1)								// if there's no empty space, dump this event.
				{
					MoveMsg2Active(CANx,i, empty_Idx);					// move the event from inactive buffer to Active buffer empty slot.
					//EV_Buf[buf_Inactive2][i].MsgNo = 0;		    // the event in inactive buffer is erased.
				}
				else
				{
					Confirm_Resend_Buf[CANx][buf_Inactive2][i].MsgNo = 0;		    // the event in inactive buffer is erased.
				}
			}
			else
			{
				Retry_Msg_Cnt[CANx][UnConfMsgNo] = 0;
				Confirm_Resend_Buf[CANx][buf_Inactive2][i].MsgNo = 0;
			}

		}
	}
	Buf_Active[CANx] = !Buf_Active[CANx];									// Swap the buffers   	// ��Ծ���л����ǻ�Ծ��
	//NVIC_EnableIRQ(CAN_IRQn);
	InUse_Idx_CAN[CANx] = 0;												// reset index pointer
}

int CANMsgBuffering::ConfirmMsg(unsigned char CANx ,uint32 MsgNo)
{
	if(CANx!=CAN1 &&CANx!=CAN2)
		return -1;
	int i;
	for (i = 0; i < CAN_CONFIRM_BUFFER_SIZE; i ++)
	{    // ��Ծ��ר���ڴ洢���͵��¼����ǻ�Ծ��ר�����ط��� ͬʱ��ȷ�ϵ��¼���
		if(  Confirm_Resend_Buf[CANx][0][i].MsgNo== MsgNo)
		{
			Confirm_Resend_Buf[CANx][0][i].MsgNo = 0;
			Retry_Msg_Cnt[CANx][MsgNo] = 0;
			return 1;
		}

		if(Confirm_Resend_Buf[CANx][1][i].MsgNo == MsgNo)
		{
			Confirm_Resend_Buf[CANx][1][i].MsgNo = 0;
			Retry_Msg_Cnt[CANx][MsgNo] = 0;
			return 1;
		}
	}
	return -1;
}

int CANMsgBuffering::Push2ConfirmBuf(unsigned char CANx,uint8 * content, int MsgLen, uint32 MsgNo)
{
	if(CANx!=CAN1 &&CANx!=CAN2)
		return -1;
	signed int EmptyIdx;
	if(MsgLen > CAN_MSG_LENGTH)
//    if( MsgLen != sizeof(MP_CAN_MSG) ) // 20160426 yifei
		return -1;
	unsigned int Active =  Buf_Active[CANx];   //û���ȥ
	if(InUse_Idx_CAN[CANx]<  CAN_CONFIRM_BUFFER_SIZE)
	{
		Confirm_Resend_Buf[CANx][Active][ InUse_Idx_CAN[CANx]].MsgNo = MsgNo;  // ��Ծ��ר���ڴ洢���͵��¼��������¼�
		Confirm_Resend_Buf[CANx][Active][ InUse_Idx_CAN[CANx]].MsgLen = MsgLen;
		memcpy((void*)&Confirm_Resend_Buf[CANx][Active][InUse_Idx_CAN[CANx]].EventContent,(void*)content,MsgLen);
		InUse_Idx_CAN[CANx]++;  // ��Ծ��ר���ڴ洢���͵��¼����ƶ�ָ�뵽��λ�á�
	}
	else
	{
		EmptyIdx  = FindNextEmpty(CANx,0);
		if(EmptyIdx == -1)
			return -1;
		Confirm_Resend_Buf[CANx][Active][EmptyIdx].MsgNo = MsgNo;
		Confirm_Resend_Buf[CANx][Active][EmptyIdx].MsgLen = MsgLen;
		memcpy((void*)&Confirm_Resend_Buf[CANx][Active][EmptyIdx].EventContent,content,MsgLen);
	}
	return 0;
}


void CANMsgBuffering::MoveMsg2Active(unsigned char CANx,int source, int destination)
{
	if(CANx!=CAN1 &&CANx!=CAN2)
		return ;
	unsigned int Active =  Buf_Active[CANx];
	// ��Ծ��ר���ڴ洢���͵��¼���ת�Ƶ��ǻ�Ծ��ר�����ط���
	memcpy( (void*)&Confirm_Resend_Buf[CANx][Active][destination],
		(void*)&Confirm_Resend_Buf[CANx][!(Active)][source],
		sizeof(ST_CAN_CONFIRM_BUFFER));

	Confirm_Resend_Buf[CANx][!(Active)][source].MsgNo = 0;
}


int CANMsgBuffering::FindNextEmpty(unsigned char CANx,int startPoint)
{
	if(CANx!=CAN1 &&CANx!=CAN2)
		return -1;
	int i;

	if (startPoint == -1)										// if already checked one full round in Active buffer and no empty left.
		return startPoint;
	unsigned int Active =  Buf_Active[CANx];
	unsigned int Used_Idx = InUse_Idx_CAN[CANx];
	if (startPoint >= Used_Idx)								// if the buffer is not used up yet, look into unused slots.
	{
		for(i = startPoint; i < CAN_CONFIRM_BUFFER_SIZE; i ++)
		{
			if( Confirm_Resend_Buf[CANx][Active][i].MsgNo == 0)  // ��Ծ��ר���ڴ洢���͵��¼����ҵ�û��Ŀ���λ�á�
				return i;
		}
	}
	for (i = 0; i < Used_Idx; i ++)							// if buffer used up, search confirmed slot which is available
	{
		if( Confirm_Resend_Buf[CANx][Active][i].MsgNo == 0)  // ��Ծ��ר���ڴ洢���͵��¼����ҵ�û��Ŀ���λ�á�
			return i;
	}

	return -1;

}
*/