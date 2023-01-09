#include "../../Entry/includes.h"

// 1-M)Ҫ������������������(MCT��)���ӻ����¿����� (���ֽڲ���)
// д 1 �Ż������豸���� 		
// ����֡��4f 55 54 5f 4d 4f 44 45 5f 31 30 5f 31 0d 0a			OUT_MODE_10_1<CR><LF>
// ����֡��4f 4b 0d 0a																			OK<CR><LF>
static const unsigned char uc_COM_RunCmd_MEMMERTGMBH[TH_MEMMERTGMBH_RS232_START_CMD_LEN] = {
0x4f, 0x55, 0x54, 0x5f, 0x4d, 0x4f, 0x44, 0x45, 0x5f, 0x31, 0x30, 0x5f, 0x31, 0x0d, 0x0a};    // Run Master to  Slave cmd 


// 2-M)Ҫ������ֹͣ������������(MCT��)���ӻ����¿�����   (���ֽڲ���)
// д 1 �Ż������豸ֹͣ
// ����֡��4f 55 54 5f 4d 4f 44 45 5f 31 30 5f 30 0d 0a			OUT_MODE_10_0<CR><LF>
// ����֡��4f 4b 0d 0a																			OK<CR><LF>
//
static const unsigned char uc_COM_StopCmd_MEMMERTGMBH[TH_MEMMERTGMBH_RS232_STOP_CMD_LEN] = {
0x4f, 0x55, 0x54, 0x5f, 0x4d, 0x4f, 0x44, 0x45, 0x5f, 0x31, 0x30, 0x5f, 0x30, 0x0d, 0x0a};    // Run Master to  Slave cmd 


// 3-M)Ҫ�������¶�/ʪ����������(MCT��)���ӻ����¿����� (���ֽڲ���)
// д 1 �Ż���ֵ��ʽ�µ����� 
// �¶�22������֡��4f 55 54 5f 53 50 5f 31 31 5f 32 32 0D 0A		OUT_SP_11_22<CR><LF>
// ����֡��4f 4b 0d 0a																					OK<CR><LF>
//
static const unsigned char uc_COM_WriteTCmd_MEMMERTGMBH[TH_MEMMERTGMBH_RS232_SET_TMEP_CMD_LEN] = {
0x4f, 0x55, 0x54, 0x5f, 0x53, 0x50, 0x5f, 0x31, 0x31, 0x5f, 0x32, 0x32, 0x0d, 0x0a};    // Run Master to  Slave cmd 

// ʪ��55%������֡��4f 55 54 5f 53 50 5f 31 33 5f 35 35 0d 0a		OUT_SP_13_55<CR><LF>
// ����֡��4f 4b 0d 0a		
static const unsigned char uc_COM_WriteHCmd_MEMMERTGMBH[TH_MEMMERTGMBH_RS232_SET_HUMI_CMD_LEN] = {
0x4f, 0x55, 0x54, 0x5f, 0x53, 0x50, 0x5f, 0x31, 0x33, 0x5f, 0x35, 0x35, 0x0d, 0x0a};    // Run Master to  Slave cmd 


//  �ӻ��ظ�ʱ��Reply����ֵ��
//reply:OK<CR><LF>
static const unsigned char uc_COM_WriteReply_MEMMERTGMBH[TH_MEMMERTGMBH_RS232_MAX_COM_WRITE_REPLY] = {
0x4f, 0x4b, 0x0d, 0x0a};     // Slave to  Master reply (�ظ����óɹ�) 


// 4-M)Ҫ���ȡ�¶�/ʪ����������(MCT��)���ӻ����¿�����  (���ֽڲ���) 
// ��ȡ 1 �Ż����� 
//�¶�  �ظ�22��			
// ����֡  49 4e 5f 50 56 5f 31 31 0d 0a				IN_PV_11<CR><LF>
// ��Ӧ֡��4f 4b 0d 0a		OK<CR><LF>
//         32 32 0d 0a    22<CR><LF>
//
static const unsigned char uc_COM_ReadTCmd_MEMMERTGMBH[TH_MEMMERTGMBH_RS232_MAX_COM_READ_CMD] = {
0x49, 0x4e, 0x5f, 0x50, 0x56, 0x5f, 0x31, 0x31, 0x0d, 0x0a};    // read Master to  Slave cmd (��ȡ) 


//ʪ��	 �ظ�55%		
// ����֡  49 4e 5f 50 56 5f 31 33 0d 0a				IN_PV_13<CR><LF>
// ��Ӧ֡��4f 4b 0d 0a		OK<CR><LF>
//         35 35 0d 0a    55<CR><LF>
static const unsigned char uc_COM_ReadHCmd_MEMMERTGMBH[TH_MEMMERTGMBH_RS232_MAX_COM_READ_CMD] = {
0x49, 0x4e, 0x5f, 0x50, 0x56, 0x5f, 0x31, 0x33, 0x0d, 0x0a};    // read Master to  Slave cmd (��ȡ) 


//  �ӻ��ظ�ʱ��Reply -222.2��
static const unsigned char uc_COM_ReadReply_HLT504P[TH_MEMMERTGMBH_RS232_MAX_COM_READ_REPLY] = {
0x4f, 0x4b, 0x0d, 0x0a, 0x2d, 0x32, 0x32, 0x32, 0x2e, 0x32, 0x0d, 0x0a};     // read Slave to  Master reply  //(�ظ��¶�-222.2��)




extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function ����ָ��   //��ʽ  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];


void TH_MEMMERTGMBH_RS232::Init(unsigned char ChanNo)
{
    // ��ʼ������ָ��
    unsigned char uc_i; // ��ֹ�����������ȫ����ʼ��һ�κ���ָ�롣
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_MEMMERTGMBH_RS232::SendRunCmd);   //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_MEMMERTGMBH_RS232::SendWriteCmd);  //  1-M)�����¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_MEMMERTGMBH_RS232::SendReadCmd);    //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_MEMMERTGMBH_RS232::SendRunCmd);      //  3-M)Ҫ�����������������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_MEMMERTGMBH_RS232::SendStopCmd);    //  4-M)Ҫ��ֹͣ�����������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_MEMMERTGMBH_RS232::Parse);  //  5-S)��ȡ�¶ȴӻ��ظ���������  (���ֽڲ���)

}

/***************************************************************
*   Parse the feedback from temperature chamber                *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/
void TH_MEMMERTGMBH_RS232::Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
		short temp;
    short PresentValue = 0;
    unsigned short int uw16_i = 0;
		unsigned char* p;

    if( UARTFunc::uart_bytenum(uc_ChNo) < TH_MEMMERTGMBH_RS232_MIN_RESPONSE_CMD_LEN) //wait until MIN_LEN bytes have been read into the buffer
        return;

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    UARTFunc::uart_getcmd(uc_ChNo,MTCI_CTRL::gR_CmdBuf);

    do 
		{
				if((MTCI_CTRL::gR_CmdBuf[uw16_i] == 0x4f) && (MTCI_CTRL::gR_CmdBuf[uw16_i + 1] == 0x4b) && (MTCI_CTRL::gR_CmdBuf[uw16_i + 2] == 0x0d) && (MTCI_CTRL::gR_CmdBuf[uw16_i + 3] == 0x0a) && (MTCI_CTRL::gR_CmdBuf[uw16_i + 4] != 0x4f))//������
				{
						MTCI_datatypes::ctrl_String2Value_Signed(&MTCI_CTRL::gR_CmdBuf[uw16_i+4], &PresentValue);

						if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
						{
							MTCI_CTRL::SetPresentValue(uc_ChNo,PresentValue,SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL);
						}
						else if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
						{
							MTCI_CTRL::SetPresentValue(uc_ChNo,PresentValue,SETPOINTVALUE_HUMI, FIRST_CHAMBER_CHANNEL);
						}
						CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[uc_ChNo]);     //update time out stamp
						return;
				}

				uw16_i++;
			} while(uw16_i < TH_MEMMERTGMBH_RS232_MAX_RESPONSE_CMD_LEN);
}

/**************************************************************
*   Send "write" command to temperature chamber.              *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TH_MEMMERTGMBH_RS232::SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
  
	short temp = 0;
	unsigned short Count = 0;
	
	if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)//�¶�
	{
		unsigned char CmdBuf[TH_MEMMERTGMBH_RS232_SET_TMEP_CMD_LEN] = {0};
		memcpy(CmdBuf,uc_COM_WriteTCmd_MEMMERTGMBH,10);
		unsigned char  *p = &CmdBuf[10];			//OUT_SP_11_22<CR><LF>
		
		temp = MTCI_CTRL::gW_TemperatureValue[uc_ChNo][FIRST_CHAMBER_CHANNEL];
		
		Count = MTCI_datatypes::ctrl_Value2String_Dot(&temp,p);
		p = (unsigned char*)&CmdBuf[10 + Count];
		*p = 0x0d;
		 p++;
		*p=0x0a;
		UARTFunc::uart_putcmd(uc_ChNo,CmdBuf,10 + Count + 2);
	}
	else if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)//ʪ��
	{
		unsigned char CmdBuf[TH_MEMMERTGMBH_RS232_SET_HUMI_CMD_LEN] = {0};
		memcpy(CmdBuf,uc_COM_WriteHCmd_MEMMERTGMBH,TH_MEMMERTGMBH_RS232_SET_HUMI_CMD_LEN);
		unsigned char  *p = &CmdBuf[10];			//OUT_SP_11_22<CR><LF>
		
		temp = MTCI_CTRL::gW_HumiValue[uc_ChNo][FIRST_CHAMBER_CHANNEL]/10;
		if(temp > 99)
			temp = 99;
		
		*p = (temp/10)+'0';
		p++;
		*p = (temp%10)+'0';

		UARTFunc::uart_putcmd(uc_ChNo,CmdBuf,TH_MEMMERTGMBH_RS232_SET_HUMI_CMD_LEN);
	}
 
		CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[uc_ChNo]);
}

/**************************************************************
*   Send "read" command to temperature chamber.               *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TH_MEMMERTGMBH_RS232::SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char Buf[TH_MEMMERTGMBH_RS232_MAX_COM_READ_CMD] = {0};

		if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
		{
			memcpy(Buf,uc_COM_ReadHCmd_MEMMERTGMBH,TH_MEMMERTGMBH_RS232_MAX_COM_READ_CMD);
		}
		else
		{
			memcpy(Buf,uc_COM_ReadTCmd_MEMMERTGMBH,TH_MEMMERTGMBH_RS232_MAX_COM_READ_CMD);
		}
			
	
   UARTFunc::uart_putcmd(uc_ChNo,Buf,TH_MEMMERTGMBH_RS232_MAX_COM_READ_CMD);
	 CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[uc_ChNo]);
}

/**************************************************************
*   Send "run" command to temperature chamber.                *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TH_MEMMERTGMBH_RS232::SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
	unsigned char Buf[TH_MEMMERTGMBH_RS232_START_CMD_LEN] = {0};
	memcpy(Buf,uc_COM_RunCmd_MEMMERTGMBH,TH_MEMMERTGMBH_RS232_START_CMD_LEN);
	
	UARTFunc::uart_putcmd(uc_ChNo,Buf,TH_MEMMERTGMBH_RS232_START_CMD_LEN);
	CommonFunc::GetTimeTicksUpdate(&MTCI_CTRL::gW_WriteStamp[uc_ChNo]);
}

/**************************************************************
*   Send "stop" command to temperature chamber.               *
*   In : n/a                                                   *
*   Out: void                                                  *
***************************************************************/
void TH_MEMMERTGMBH_RS232::SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
	unsigned char Buf[TH_MEMMERTGMBH_RS232_STOP_CMD_LEN] = {0};
	memcpy(Buf,uc_COM_StopCmd_MEMMERTGMBH,TH_MEMMERTGMBH_RS232_STOP_CMD_LEN);

	UARTFunc::uart_putcmd(uc_ChNo,Buf,TH_MEMMERTGMBH_RS232_STOP_CMD_LEN);
	CommonFunc::GetTimeTicksUpdate(&MTCI_CTRL::gW_WriteStamp[uc_ChNo]);
}