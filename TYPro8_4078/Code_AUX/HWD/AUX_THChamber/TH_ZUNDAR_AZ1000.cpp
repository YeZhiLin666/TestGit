/*****************************************************************************
// �����ļ�      :TH_SAMWONTECH_TEMI2500.c    �����¿����и��ִ����ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"


extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function ����ָ��   //��ʽ  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

void TH_ZUNDAR_AZ1000::Init(unsigned char ChanNo)
{
// ��ʼ������ָ�롣
    unsigned char uc_i; // ��ֹ�����������ȫ����ʼ��һ�κ���ָ�롣
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_ZUNDAR_AZ1000::SendReadCmd);   //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_ZUNDAR_AZ1000::SendWriteCmd);  //  1-M)�����¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_ZUNDAR_AZ1000::SendReadCmd);    //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_ZUNDAR_AZ1000::SendRunCmd);      //  3-M)Ҫ�����������������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_ZUNDAR_AZ1000::SendStopCmd);    //  4-M)Ҫ��ֹͣ�����������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_ZUNDAR_AZ1000::ParseResult);  //  5-S)��ȡ�¶ȴӻ��ظ���������  (���ֽڲ���)

    MTCI_CTRL::uw16_PUBLIC_REPEAT_NUM = 1; // PUBLIC_REPEAT_NUM;
    MTCI_CTRL::uw32_CMD_INTERVALTIME = 5000*10; //CMD_INTERVALTIME;
}

void TH_ZUNDAR_AZ1000::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// �������յĽ������
    short int PresentValue = 0;
    unsigned short int uw16_i;
    unsigned int uw16_Total;
    uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

    if( uw16_Total < TH_ZUNDAR_AZ1000_MIN_RESPONSE_CMD_LEN)
        return;  // �����������ϴ���һ����Ϣ�����ޱ���
		
		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_i = 0;
    do 
		{
				if((MTCI_CTRL::gR_CmdBuf[uw16_i%MAX_UART_BUF_SIZE] == 0x05) && \
					(MTCI_CTRL::gR_CmdBuf[(uw16_i+1)%MAX_UART_BUF_SIZE] == 0x00) && \
					(MTCI_CTRL::gR_CmdBuf[(uw16_i+2)%MAX_UART_BUF_SIZE] == 0x03) && \
					(MTCI_CTRL::gR_CmdBuf[(uw16_i+3)%MAX_UART_BUF_SIZE] == 0x02))
				{
					PresentValue = (MTCI_CTRL::gR_CmdBuf[uw16_i+4] << 8) & 0xff00;//���ֽ�
					PresentValue |= MTCI_CTRL::gR_CmdBuf[uw16_i+5] & 0xff;//���ֽ�
					
					MTCI_CTRL::SetPresentValue(ChanNo,PresentValue,uc_Temp4Humi_Flag,FIRST_CHAMBER_CHANNEL);
					CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);

					return;
				}

        uw16_i++;
    } while((uw16_i + 5 < uw16_Total) && (uw16_i + 5 < MAX_UART_BUF_SIZE));
}



void TH_ZUNDAR_AZ1000::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
	short temp;
	
	CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��
	if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
	{	
		unsigned char SetTempCmd[] = {0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x06,0x00,0x2e,0x00,0x00};//��ȡ1��ͨ���¶�ֵ
		unsigned short uw16_len = sizeof(SetTempCmd);
		
		temp = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
		SetTempCmd[uw16_len - 2] = (temp >> 8) & 0xff;
		SetTempCmd[uw16_len - 1] = temp & 0xff;
		
		UARTFunc::uart_putcmd(ChanNo,SetTempCmd,uw16_len);
	}
	else if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
	{	
		unsigned char SetHumiCmd[] = {0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x06,0x00,0x2f,0x00,0x00};//��ȡ1��ͨ��ʪ��ֵ
		unsigned short uw16_len = sizeof(SetHumiCmd);
		
		temp = MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL];
		SetHumiCmd[uw16_len - 2] = (temp >> 8) & 0xff;
		SetHumiCmd[uw16_len - 1] = temp & 0xff;		
		
		UARTFunc::uart_putcmd(ChanNo,SetHumiCmd,uw16_len);
	}		
}

void TH_ZUNDAR_AZ1000::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
	CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��
	if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
	{	
		unsigned char ReadTempCmd[] = {0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x03,0x00,0x04,0x00,0x01};//��ȡ1��ͨ���¶�ֵ
		unsigned short uw16_len = sizeof(ReadTempCmd);
		
		UARTFunc::uart_putcmd(ChanNo,ReadTempCmd,uw16_len);
	}
	else if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
	{	
		unsigned char ReadHumiCmd[] = {0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x03,0x00,0x05,0x00,0x01};//��ȡ1��ͨ��ʪ��ֵ
		unsigned short uw16_len = sizeof(ReadHumiCmd);
		
		UARTFunc::uart_putcmd(ChanNo,ReadHumiCmd,uw16_len);
	}	
}

void TH_ZUNDAR_AZ1000::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
  unsigned char MbBuf[] = {0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x06,0x00,0x01,0x00,0x02};//������ֵ
	unsigned short uw16_len = sizeof(MbBuf);
	
  CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��
  UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}

void TH_ZUNDAR_AZ1000::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
  unsigned char MbBuf[] = {0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x06,0x00,0x01,0x00,0x01};//ֹͣ��ֵ
	unsigned short uw16_len = sizeof(MbBuf);

  CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��
  UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}



