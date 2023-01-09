/*****************************************************************************
// �����ļ�      :TH_VOETSCH_VC_L4006.c    �����¿����и��ִ����ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"

extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function ����ָ��   //��ʽ  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

bool TH_SPI2_P1::SetModeFlag;

void TH_SPI2_P1::Init(unsigned char ChanNo)
{
    // ��ʼ������ָ�롣
    unsigned char uc_i; // ��ֹ�����������ȫ����ʼ��һ�κ���ָ�롣
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_SPI2_P1::SendReadCmd);   //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_SPI2_P1::SendWriteCmd);  //  1-M)�����¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_SPI2_P1::SendReadCmd);    //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_SPI2_P1::SendRunCmd);      //  3-M)Ҫ�����������������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_SPI2_P1::SendStopCmd);    //  4-M)Ҫ��ֹͣ�����������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_SPI2_P1::ParseResult);  //  5-S)��ȡ�¶ȴӻ��ظ���������  (���ֽڲ���)
}

void TH_SPI2_P1::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    char *TempPtr = NULL;
    char *HumPtr = NULL;
    short PresentValue = 0;
    unsigned short int uw16_i = 0;

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    TempPtr = strstr((char*)MTCI_CTRL::gR_CmdBuf,"TCuve=");
    HumPtr = strstr((char*)MTCI_CTRL::gR_CmdBuf,"HumCuve=");

    if(TempPtr != NULL)//���¶�
    {
        PresentValue = (short)(atof((const char*)(TempPtr + 6))*10);
        MTCI_CTRL::SetPresentValue(ChanNo,PresentValue,SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);     //update time out stamp
        return;
    }
    else if(HumPtr != NULL)//��ʪ��
    {
        PresentValue = (short)(atof((const char*)(HumPtr + 8))*10);
        MTCI_CTRL::SetPresentValue(ChanNo,PresentValue,SETPOINTVALUE_HUMI, FIRST_CHAMBER_CHANNEL);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);     //update time out stamp
        return;
    }
}


void TH_SPI2_P1::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned short uw16_len = 0;

    if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)//�¶�
    {
        unsigned char TempSetBuf[20] = {"ConsCuve = "};//�趨�¶�ͨ��ֵ

        short temp = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
        float fSetPoint = (float)temp/10;
        unsigned char SetBuf[10] = {0};

        sprintf((char *)SetBuf,"%.1f\r\n",fSetPoint);
        strncat((char *)TempSetBuf+11,(char *)SetBuf,strlen((char *)SetBuf));

        uw16_len = strlen((char*)TempSetBuf);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��
        UARTFunc::uart_putcmd(ChanNo,TempSetBuf,uw16_len);
    }
    else if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)//�¶�
    {
        unsigned char HumSetBuf[20] = {"ConsHum = "};//�趨�¶�ͨ��ֵ

        short temp = MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL];
        float fSetPoint = (float)temp/10;
        unsigned char SetBuf[10] = {0};

        sprintf((char *)SetBuf,"%.1f\r\n",fSetPoint);
        strncat((char *)HumSetBuf+10,(char *)SetBuf,strlen((char *)SetBuf));

        uw16_len = strlen((char*)HumSetBuf);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��
        UARTFunc::uart_putcmd(ChanNo,HumSetBuf,uw16_len);
    }
}


void TH_SPI2_P1::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char SendBuf[] = {0x50,0x72,0x6f,0x67,0x72,0x61,0x6d,0x6d,0x65,0x20,0x65,0x6e,0x20,0x63,0x6f,0x75,0x72,0x73,0x20,0x3d,0x20,0x22,0x4d,0x61,0x6e,0x75,0x61,0x6c,0x2d,0x6d,0x6f,0x64,0x65,0x22,0x0d,0x0a};
    unsigned short uw16_len = sizeof(SendBuf);

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��
    UARTFunc::uart_putcmd(ChanNo,SendBuf,uw16_len);

    SetModeFlag = true;
}

void TH_SPI2_P1::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    if(SetModeFlag)
    {
        SetModeFlag = false;
        unsigned char SendBuf[] = {"Marche_arret = 1\r\n"};//����
        unsigned short uw16_len = strlen((char*)SendBuf);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��
        UARTFunc::uart_putcmd(ChanNo,SendBuf,uw16_len);

        return;
    }

    if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)//
    {
        unsigned char SendBuf[] = {"?TCuve\r\n"};//���¶�
        unsigned short uw16_len = strlen((char*)SendBuf);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��
        UARTFunc::uart_putcmd(ChanNo,SendBuf,uw16_len);
    }
    else if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)//
    {
        unsigned char SendBuf[] = {"?HumCuve\r\n"};//��ʪ��
        unsigned short uw16_len = strlen((char*)SendBuf);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��
        UARTFunc::uart_putcmd(ChanNo,SendBuf,uw16_len);
    }
}

void TH_SPI2_P1::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char SendBuf[] = {"Marche_arret = 0\r\n"};//ֹͣ
    unsigned short uw16_len = strlen((char*)SendBuf);

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��
    UARTFunc::uart_putcmd(ChanNo,SendBuf,uw16_len);
}

