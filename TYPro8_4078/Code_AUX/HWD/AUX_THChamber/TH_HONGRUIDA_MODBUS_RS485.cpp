/*****************************************************************************
// �����ļ�      :TH_SAMWONTECH_TEMI2500.c    �����¿����и��ִ����ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"


extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function ����ָ��   //��ʽ  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

unsigned char TH_HONGRUIDA_MODBUS_RS485::m_RecvDataType;

void TH_HONGRUIDA_MODBUS_RS485::Init(unsigned char ChanNo)
{
// ��ʼ������ָ�롣
    unsigned char uc_i; // ��ֹ�����������ȫ����ʼ��һ�κ���ָ�롣
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_HONGRUIDA_MODBUS_RS485::SendReadCmd);   //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_HONGRUIDA_MODBUS_RS485::SendWriteCmd);  //  1-M)�����¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_HONGRUIDA_MODBUS_RS485::SendReadCmd);    //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_HONGRUIDA_MODBUS_RS485::SendRunCmd);      //  3-M)Ҫ�����������������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_HONGRUIDA_MODBUS_RS485::SendStopCmd);    //  4-M)Ҫ��ֹͣ�����������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_HONGRUIDA_MODBUS_RS485::ParseResult);  //  5-S)��ȡ�¶ȴӻ��ظ���������  (���ֽڲ���)
}

void TH_HONGRUIDA_MODBUS_RS485::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// �������յĽ������
    short int PresentValue = 0;
    unsigned short int uw16_i = 0;
		unsigned int uw16_Total;

		uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

    if( uw16_Total < TH_HONGRUIDA_MODBUS_RS485_MIN_RESPONSE_CMD_LEN)
        return;  // �����������ϴ���һ����Ϣ�����ޱ���
		
		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_i = 0;
    do
    {
        if((MTCI_CTRL::gR_CmdBuf[uw16_i%MAX_UART_BUF_SIZE] == 0x01) && \
                (MTCI_CTRL::gR_CmdBuf[(uw16_i+1)%MAX_UART_BUF_SIZE] == 0x03) && \
                (MTCI_CTRL::gR_CmdBuf[(uw16_i+2)%MAX_UART_BUF_SIZE] == 0x02))
        {
            PresentValue = (MTCI_CTRL::gR_CmdBuf[uw16_i+3] << 8) & 0xff00;//���ֽ�
            PresentValue |= MTCI_CTRL::gR_CmdBuf[uw16_i+4] & 0xff;//���ֽ�
						if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
							PresentValue /= 10;

            MTCI_CTRL::SetPresentValue(ChanNo,PresentValue,m_RecvDataType,FIRST_CHAMBER_CHANNEL);
            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);

            return;
        }

        uw16_i++;
    } while((uw16_i + 3 < uw16_Total) && (uw16_i + 3 < MAX_UART_BUF_SIZE));
		
		return;
}



void TH_HONGRUIDA_MODBUS_RS485::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    short temp;
		unsigned short crc16;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��
    if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
    {
        unsigned char SetTempCmd[] = {0x01,0x10,0x00,0x87,0x00,0x01,0x02,0xff,0xff,0xff,0xff};//д1��ͨ���¶��趨ֵ
        unsigned short uw16_len = sizeof(SetTempCmd);

        temp = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
        SetTempCmd[uw16_len - 4] = (temp >> 8) & 0xff;
        SetTempCmd[uw16_len - 3] = temp & 0xff;
				
				crc16 = MTCI_datatypes::CRC16(SetTempCmd,uw16_len-2);
				SetTempCmd[uw16_len - 2] = (unsigned char) (crc16 >>8);
				SetTempCmd[uw16_len - 1] = (unsigned char) (crc16 & 0xff);				

        UARTFunc::uart_putcmd(ChanNo,SetTempCmd,uw16_len);
    }
		else if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
		{
        unsigned char SetHumiCmd[] = {0x01,0x10,0x00,0x89,0x00,0x01,0x02,0xff,0xff,0xff,0xff};//д1��ͨ���¶��趨ֵ
        unsigned short uw16_len = sizeof(SetHumiCmd);

        temp = MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL]*10;
        SetHumiCmd[uw16_len - 4] = (temp >> 8) & 0xff;
        SetHumiCmd[uw16_len - 3] = temp & 0xff;
				
				crc16 = MTCI_datatypes::CRC16(SetHumiCmd,uw16_len-2);
				SetHumiCmd[uw16_len - 2] = (unsigned char) (crc16 >>8);
				SetHumiCmd[uw16_len - 1] = (unsigned char) (crc16 & 0xff);				

        UARTFunc::uart_putcmd(ChanNo,SetHumiCmd,uw16_len);
		}
		
		return;
}

void TH_HONGRUIDA_MODBUS_RS485::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
		unsigned short crc16;
    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��
    if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
    {
				m_RecvDataType = SETPOINTVALUE_TEMPERATURE;
        unsigned char ReadTempCmd[] = {0x01,0x03,0x00,0x79,0x00,0x01,0xff,0xff};//��ȡ1��ͨ���¶�ֵ
        unsigned short uw16_len = sizeof(ReadTempCmd);
				
				crc16 = MTCI_datatypes::CRC16(ReadTempCmd,6);
				ReadTempCmd[6] = (unsigned char) (crc16 >>8);
				ReadTempCmd[7] = (unsigned char) (crc16 & 0xff);
				
        UARTFunc::uart_putcmd(ChanNo,ReadTempCmd,uw16_len);
    }
    else if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
    {
				m_RecvDataType = SETPOINTVALUE_HUMI;
        unsigned char ReadHumiCmd[] = {0x01,0x03,0x00,0x81,0x00,0x01,0xff,0xff};//��ȡ1��ͨ������ֵ
        unsigned short uw16_len = sizeof(ReadHumiCmd);
				
				crc16 = MTCI_datatypes::CRC16(ReadHumiCmd,6);
				ReadHumiCmd[6] = (unsigned char) (crc16 >>8);
				ReadHumiCmd[7] = (unsigned char) (crc16 & 0xff);				

        UARTFunc::uart_putcmd(ChanNo,ReadHumiCmd,uw16_len);
    }
		
		return;
}

void TH_HONGRUIDA_MODBUS_RS485::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char MbBuf[] = {0x01,0x10,0x00,0x0E,0x00,0x01,0x02,0x01,0x01,0x67,0x2E};//������ֵ1#
    unsigned short uw16_len = sizeof(MbBuf);

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��
    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
		
		return;
}

void TH_HONGRUIDA_MODBUS_RS485::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char MbBuf[] = {0x01,0x10,0x00,0x0E,0x00,0x01,0x02,0x00,0x00,0xA7,0x7E};//ֹͣ��ֵ1#
    unsigned short uw16_len = sizeof(MbBuf);

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��
    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
		
		return;
}



