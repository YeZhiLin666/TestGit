/*****************************************************************************
// �����ļ�      :TH_SAMWONTECH_TEMI2500.c    �����¿����и��ִ����ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"


extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function ����ָ��   //��ʽ  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

void TH_MT8071_MODBUS::Init(unsigned char ChanNo)
{
// ��ʼ������ָ�롣
    unsigned char uc_i; // ��ֹ�����������ȫ����ʼ��һ�κ���ָ�롣
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_MT8071_MODBUS::SendReadCmd);   //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_MT8071_MODBUS::SendWriteCmd);  //  1-M)�����¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_MT8071_MODBUS::SendReadCmd);    //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_MT8071_MODBUS::SendRunCmd);      //  3-M)Ҫ�����������������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_MT8071_MODBUS::SendStopCmd);    //  4-M)Ҫ��ֹͣ�����������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_MT8071_MODBUS::ParseResult);  //  5-S)��ȡ�¶ȴӻ��ظ���������  (���ֽڲ���)

    MTCI_CTRL::uw16_PUBLIC_REPEAT_NUM = 1; // PUBLIC_REPEAT_NUM;
    MTCI_CTRL::uw32_CMD_INTERVALTIME = 200*10; //CMD_INTERVALTIME;
}

void TH_MT8071_MODBUS::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// �������յĽ������
    short int PresentValue = 0;
    unsigned short int uw16_i;
    unsigned int uw16_Total;
    uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

    if( uw16_Total < TH_MT8071_MODBUS_MIN_RESPONSE_CMD_LEN)
        return;  // �����������ϴ���һ����Ϣ�����ޱ���

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_i = 0;
    do
    {
        if((MTCI_CTRL::gR_CmdBuf[uw16_i] == 0x01) && (MTCI_CTRL::gR_CmdBuf[uw16_i+1] == 0x03) && (MTCI_CTRL::gR_CmdBuf[uw16_i+2] == 0x02))
        {
            PresentValue = (MTCI_CTRL::gR_CmdBuf[uw16_i+3] << 8) & 0xff00;//���ֽ�
            PresentValue |= MTCI_CTRL::gR_CmdBuf[uw16_i+4] & 0xff;//���ֽ�

            MTCI_CTRL::SetPresentValue(ChanNo,PresentValue,uc_Temp4Humi_Flag,FIRST_CHAMBER_CHANNEL);
            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);

            return;
        }

        uw16_i++;
    } while((uw16_i < uw16_Total) && (uw16_i < MAX_UART_BUF_SIZE - 3));
}



void TH_MT8071_MODBUS::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char SetTempCmd[] = {0x01,0x06,0x1F,0xA4,0x00,0x00,0xff,0xff};//�趨�¶�ͨ��ֵ
    unsigned char SetHumiCmd[] = {0x01,0x06,0x1F,0xA5,0x00,0x00,0xff,0xff};//�趨ʪ��ͨ��ֵ
    unsigned short crc16;
    unsigned short uw16_len = sizeof(SetTempCmd);
    short temp;

    if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
    {
        temp = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
        SetTempCmd[4] = (temp >> 8) & 0xff;
        SetTempCmd[5] = temp & 0xff;

        crc16 = MTCI_datatypes::CRC16(SetTempCmd,6);
        SetTempCmd[6] = (unsigned char) (crc16 >>8);
        SetTempCmd[7] = (unsigned char) (crc16 & 0xff);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��
        UARTFunc::uart_putcmd(ChanNo,SetTempCmd,uw16_len);
    }
    else if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
    {
        temp = MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL];
        SetHumiCmd[4] = (temp >> 8) & 0xff;
        SetHumiCmd[5] = temp & 0xff;

        crc16 = MTCI_datatypes::CRC16(SetHumiCmd,6);
        SetHumiCmd[6] = (unsigned char) (crc16 >>8);
        SetHumiCmd[7] = (unsigned char) (crc16 & 0xff);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��
        UARTFunc::uart_putcmd(ChanNo,SetHumiCmd,uw16_len);
    }
}

void TH_MT8071_MODBUS::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char ReadTempCmd[] = {0x01,0x03,0x1F,0x37,0x00,0x01,0xff,0xff};//��ȡ1��ͨ���¶�ֵ
    unsigned char ReadHumiCmd[] = {0x01,0x03,0x1F,0x38,0x00,0x01,0xff,0xff};//��ȡ1��ͨ��ʪ��ֵ
    unsigned short crc16;
    unsigned short uw16_len = sizeof(ReadTempCmd);

    if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
    {
        crc16 = MTCI_datatypes::CRC16(ReadTempCmd,6);
        ReadTempCmd[6] = (unsigned char) (crc16 >>8);
        ReadTempCmd[7] = (unsigned char) (crc16 & 0xff);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��
        UARTFunc::uart_putcmd(ChanNo,ReadTempCmd,uw16_len);
    }
    else if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
    {
        crc16 = MTCI_datatypes::CRC16(ReadHumiCmd,6);
        ReadHumiCmd[6] = (unsigned char) (crc16 >>8);
        ReadHumiCmd[7] = (unsigned char) (crc16 & 0xff);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��
        UARTFunc::uart_putcmd(ChanNo,ReadHumiCmd,uw16_len);
    }
}

void TH_MT8071_MODBUS::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char MbBuf[] = {0x01,0x05,0x1F,0x40,0xFF,0x00,0xff,0xff};//������ֵ
    unsigned short crc16;
    unsigned short uw16_len = sizeof(MbBuf);

    crc16 = MTCI_datatypes::CRC16(MbBuf,6);
    MbBuf[6] = (unsigned char) (crc16 >>8);
    MbBuf[7] = (unsigned char) (crc16 & 0xff);

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��
    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}

void TH_MT8071_MODBUS::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char MbBuf[] = {0x01,0x05,0x1F,0x41,0xFF,0x00,0xff,0xff};//ֹͣ��ֵ
    unsigned short crc16;
    unsigned short uw16_len = sizeof(MbBuf);

    crc16 = MTCI_datatypes::CRC16(MbBuf,6);
    MbBuf[6] = (unsigned char) (crc16 >>8);
    MbBuf[7] = (unsigned char) (crc16 & 0xff);

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��
    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}



