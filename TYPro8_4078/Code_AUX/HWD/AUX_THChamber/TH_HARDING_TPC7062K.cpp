/*****************************************************************************
// �����ļ�      :TH_HARDING_TPC7062K.c    �����¿����и��ִ����ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2014.04.08
// ����ض��豸  :ARBIN������������Ƽ�TPC7062K˵��
*****************************************************************************/


#include "../../Entry/includes.h"



extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function ����ָ��   //��ʽ  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];




//------------------------------------------------------------------------------------------------
//  ����ض��豸  :ARBIN������������Ƽ�TPC7062K˵��
//  ����ͨ�ŷ�ʽ  Xor Sum Check  ���䷽ʽ��ASCIIģʽ ��XOR SUMУ��
//  �ر�ע�⣺��ʹ��ֹͣ���������������д���һ��ʱ�����Ʊ����޸ġ�

// �Ķ�����˵����   ��HLT610P-F˵����.pdf��
// A�����ݸ�ʽ�������ʣ�19200������λ 7��Ч��E(ż��)��ֹͣλ2

// Ĭ�ϳ���ʱ�¶�ֵΪ22���
// Ĭ�ϳ�̬ʱʪ��ֵΪ55%RH

// 1-M)Ҫ���������з�ʽ��������(MCT��)���ӻ����¿����� (���ֽڲ���)
// Transmission :
// ASCII�룺@01WD0061000257*
// 16���ƣ�40 30 31 57 44 30 30 36 31 30 30 30 32 35 37 2A 0D

// ���մ�Ӧ:(д����ȷ)
// ASCII�룺@01WD0052*
// 16���ƣ�40 30 31 57 44 30 30 35 32 2A 0D

const unsigned char uc_COM_ModeCmd_TPC7062K[TH_TPC7062K_MAX_COM_MODE_CMD] = {
    0x40, 0x30, 0x31, 0x57, 0x44, 0x30, 0x30, 0x36, 0x31, 0x30,
    0x30, 0x30, 0x32, 0X35, 0X37, 0x2A, 0x0D
};    // Mode Master to  Slave cmd

// 2-M)Ҫ������������������(MCT��)���ӻ����¿����� (���ֽڲ���)
// Transmission :
// ASCII�룺@01WD2550000151*
// 16���ƣ�40 30 31 57 44 32 35 35 30 30 30 30 31 35 31 2A 0D

// ���մ�Ӧ:(д����ȷ)
// ASCII�룺@01WD0052*
// 16���ƣ�40 30 31 57 44 30 30 35 32 2A 0D
//
const unsigned char uc_COM_RunCmd_TPC7062K[TH_TPC7062K_MAX_COM_RUN_CMD] = {
    0x40, 0x30, 0x31, 0x57, 0x44, 0x32, 0x35, 0x35, 0x30, 0x30,
    0x30, 0x30, 0x31, 0X35, 0X31, 0x2A, 0x0D
};    // Run Master to  Slave cmd

// 3-M)Ҫ������ֹͣ������������(MCT��)���ӻ����¿�����   (���ֽڲ���)
// Transmission :
// ASCII�룺@01WD2552000153*
// 16���ƣ�40 30 31 57 44 32 35 35 32 30 30 30 31 35 33 2A 0D

// ���մ�Ӧ:(д����ȷ)
// ASCII�룺@01WD0052*
// 16���ƣ�40 30 31 57 44 30 30 35 32 2A 0D
//
const unsigned char uc_COM_StopCmd_TPC7062K[TH_TPC7062K_MAX_COM_STOP_CMD] = {
    0x40, 0x30, 0x31, 0x57, 0x44, 0x32, 0x35, 0x35, 0x32, 0x30,
    0x30, 0x30, 0X31, 0X35, 0x33, 0x2A, 0x0D
};    // Stop Master to  Slave cmd

//-------------------------------------------------------
// 4-M)Ҫ�������¶�/ʪ����������(MCT��)���ӻ����¿����� (���ֽڲ���)
// Transmission :
// ASCII�룺@01WD140000DC022656*
// 16���ƣ�40 30 31 57 44 31 34 30 30 30 30 44 43 30 32 32 36 35 36 2A 0D

// 40 30 31 57 44 31 34 30 30 XX XX XX XX YY YY YY YY 35 36 2A 0D
//  (XX XX XX XX�¶�ֵ�ȴ���λ�ٴ���λ; YY YY YY YY ʪ��ֵ�ȴ���λ�ٴ���λ)
// ��ʾֵ=�¶�ֵ/10;    �磺22(220/10=DC/A)��,55(550/10=226/A)%ʪ��Humi.
// [�������ַ�ʽ: (45��*10)  450 = 0X01C2; (-60��*10) -600 = 0XFDA8 (0x10000-0xFDA8=0X258 = 600)]

// Response :
// ���մ�Ӧ:(д����ȷ)
// ASCII�룺@01WD0052*
// 16���ƣ�40 30 31 57 44 30 30 35 32 2A 0D
//
const unsigned char uc_COM_WriteCmd_TPC7062K[TH_TPC7062K_MAX_COM_WRITE_CMD] = {
    0x40, 0x30, 0x31, 0x57, 0x44, 0x31, 0x34, 0x30, 0x30, 0x30,
    0x30, 0x44, 0X43, 0X30, 0x32, 0x32, 0x36, 0x35, 0x36, 0x2A,
    0x0D
};     // write Master to  Slave cmd  (�����¶�22��,ʪ��55%)

//  �ӻ��ظ�ʱ��Reply����ֵ��
const unsigned char uc_COM_WriteReply_TPC7062K[TH_TPC7062K_MAX_COM_WRITE_REPLY] = {
    0x40, 0x30, 0x31, 0x57, 0x44, 0x30, 0x30, 0x35, 0x32, 0x2A,
    0x0D
};     // write Slave to  Master reply (�ظ����óɹ�)

// 5-M)Ҫ���ȡ�¶�/ʪ����������(MCT��)���ӻ����¿�����  (���ֽڲ���)
// Transmission :
// ASCII�룺@01RD2500000656*
// 16���ƣ�40 30 31 52 44 32 35 30 30 30 30 30 36 35 36 2A 0D

// �ӻ��ظ�ʱ��
// Response :
// ASCII�룺@01RD000222FF9C00FA00000000F7CC59*
// 16���ƣ�40 30 31 52 44 30 30 30 32 32 32 46 46 39 43 30 30 46 41 30 30 30 30 30 30 30 30 46 37 43 43 35 39 2A 0D
//
const unsigned char uc_COM_ReadCmd_TPC7062K[TH_TPC7062K_MAX_COM_READ_CMD] = {
    0x40, 0x30, 0x31, 0x52, 0x44, 0x32, 0x35, 0x30, 0x30, 0x30,
    0x30, 0x30, 0X36, 0X35, 0x36, 0x2A, 0x0D
};    // read Master to  Slave cmd (��ȡ)

//  �ӻ��ظ�ʱ��Reply
const unsigned char uc_COM_ReadReply_TPC7062K[TH_TPC7062K_MAX_COM_READ_REPLY] = {
    0x40, 0x30, 0x31, 0x52, 0x44, 0x30, 0x30, 0x30, 0x32, 0x32,
    0x32, 0x46, 0X46, 0X39, 0x43, 0x30, 0x30, 0x46, 0x41, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0X30, 0X30, 0x46, 0x37, 0x43,
    0x43, 0x35, 0x39, 0x2A, 0x0D
};     // read Slave to  Master reply  //(�ظ��¶�-10��)


//
//  ARBIN������������Ƽ�TPC7062K˵��  ����ͨ�ŷ�ʽ  Xor Sum Check  ���䷽ʽ��ASCIIģʽ ��XOR SUMУ��    //
//----------------------------------------------------------------------------------------------------






void TH_TPC7062K::Init(unsigned char ChanNo)
{
// ��ʼ������ָ�롣
    unsigned char uc_i; // ��ֹ�����������ȫ����ʼ��һ�κ���ָ�롣
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_TPC7062K::SendReadCmd);   //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_TPC7062K::SendWriteCmd);  //  1-M)�����¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_TPC7062K::SendReadCmd);    //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_TPC7062K::SendRunCmd);      //  3-M)Ҫ�����������������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_TPC7062K::SendStopCmd);    //  4-M)Ҫ��ֹͣ�����������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_TPC7062K::ParseResult);  //  5-S)��ȡ�¶ȴӻ��ظ���������  (���ֽڲ���)
}

void TH_TPC7062K::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// �������յĽ������
    unsigned char *pCmd;
    unsigned char *p;
    unsigned char uc_MoveFlag = 0;
    //bool b_is_temp;
    //bool NoZero;
    short int PresentValue = 0;
    unsigned short int uw16_i;
    unsigned short int uw16_j;
    unsigned short int uw16_k;
    unsigned char uc_Buffer[4][6];
    unsigned long int uw32_DO = 0;
    unsigned int uw16_Total = PresentValue;
    uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

    if( uw16_Total < TH_TPC7062K_MIN_RESPONSE_CMD_LEN)
        return;  // �����������ϴ���һ����Ϣ�����ޱ���

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_i = 0;
    do { // ȥ��[STX (1 byte)][Addr (2 byte)]
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == uc_COM_ReadReply_TPC7062K[3] && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_TPC7062K[4]
                && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_TPC7062K[5] && MTCI_CTRL::gR_CmdBuf[uw16_i+4] == uc_COM_ReadReply_TPC7062K[6])
        {   // ���ð�ͷ���β�ж�����ֵ��

            if(MTCI_CTRL::gR_CmdBuf[uw16_i+30] == uc_COM_ReadReply_TPC7062K[33] && MTCI_CTRL::gR_CmdBuf[uw16_i+31] == uc_COM_ReadReply_TPC7062K[34])  // ��β�жϡ�
            {   //  ��������,���д���

                //PresentValue = ((short int)(gR_CmdBuf[uw16_i+8]) << 12 |gR_CmdBuf[uw16_i+9]) << 8 |gR_CmdBuf[uw16_i+10]) << 4 | (gR_CmdBuf[uw16_i+11]) );

                pCmd = &MTCI_CTRL::gR_CmdBuf[uw16_i+TH_TPC7062K_READ_REPLY_POINT_TEMPVAlUE]; // [11 - 3]=8   // ȥ��[STX (1 byte)][Addr (2 byte)]
                MTCI_datatypes::ctrl_String2Value_Hex(pCmd, &MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[0]);   //  �¶�
                //PresentValue = (short int) (atof(pCmd)*10);  // �Ŵ�10���ϴ�
                MTCI_CTRL::SetPresentValue(ChanNo, MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[0],SETPOINTVALUE_TEMPERATURE,FIRST_CHAMBER_CHANNEL);

                pCmd = &MTCI_CTRL::gR_CmdBuf[uw16_i+TH_TPC7062K_READ_REPLY_POINT_HUMIVAlUE];   // [19 - 3]=16 // ȥ��[STX (1 byte)][Addr (2 byte)]
                MTCI_datatypes::ctrl_String2Value_Hex(pCmd, &MTCI_CTRL::str_Uart[ChanNo].gR_PresentHumiValue[0]); // ʪ��
                //PresentValue = (short int) (atof(pCmd)*10);  // �Ŵ�10���ϴ�
                MTCI_CTRL::SetPresentValue(ChanNo, MTCI_CTRL::str_Uart[ChanNo].gR_PresentHumiValue[FIRST_CHAMBER_CHANNEL],SETPOINTVALUE_HUMI,FIRST_CHAMBER_CHANNEL);

                CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);

                return;
            }
        }

        uw16_i++;
    } while(uw16_i < uw16_Total);


}




void TH_TPC7062K::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����д����ֵ����
    unsigned int  uw16_len = TH_TPC7062K_MAX_COM_WRITE_CMD;
    unsigned int  MbChkSum;
    //unsigned short int  register_data;
    unsigned char MbBuf[SEND_MAX_BUF];
    //unsigned long int  uw32_DO;
    //unsigned char XcrcHi,XcrcLo;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��


    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteCmd_TPC7062K[MbChkSum];

    //register_data = gW_TemperatureValue[ChanNo];
    MTCI_datatypes::ctrl_Value2String_Hex(&MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL],MbBuf + TH_TPC7062K_WRITE_CMD_POINT_TEMPVAlUE);

    MTCI_datatypes::ctrl_Value2String_Hex(&MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL],MbBuf + TH_TPC7062K_WRITE_CMD_POINT_HUMIVAlUE);

#ifdef COM_CMD_CHANNEL_DIFFER
    MbBuf[2] = ChanNo + ASCII_FIRST_UART_NUM;
#endif

    TH_TPC7062K::LoadCheckSum(SEND_COM_WRITE_CMD, MbBuf, uw16_len);  // ����У�������β��־��

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::gW_WriteCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}

void TH_TPC7062K::SendModeCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����ģʽ��������
    unsigned int  uw16_len = TH_TPC7062K_MAX_COM_MODE_CMD;
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_ModeCmd_TPC7062K[MbChkSum];
    uw16_len = TH_TPC7062K_MAX_COM_MODE_CMD;

#ifdef COM_CMD_CHANNEL_DIFFER
    MbBuf[2] = ChanNo + ASCII_FIRST_UART_NUM;
#endif

    TH_TPC7062K::LoadCheckSum(SEND_COM_MODE_CMD, MbBuf, uw16_len);  // ����У�������β��־��

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}

void TH_TPC7062K::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ������������
    unsigned int  uw16_len = TH_TPC7062K_MAX_COM_RUN_CMD;
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_RunCmd_TPC7062K[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    MbBuf[2] = ChanNo + ASCII_FIRST_UART_NUM;
#endif

    TH_TPC7062K::LoadCheckSum(SEND_COM_RUN_CMD, MbBuf, uw16_len);  // ����У�������β��־��

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::gW_RunCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}

void TH_TPC7062K::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ���Ͷ�����

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_TPC7062K_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
    {
        MbBuf[MbChkSum] = uc_COM_ReadCmd_TPC7062K[MbChkSum];
    }

#ifdef COM_CMD_CHANNEL_DIFFER
    MbBuf[2] = ChanNo + ASCII_FIRST_UART_NUM;
#endif

    TH_TPC7062K::LoadCheckSum(SEND_COM_READ_CMD, MbBuf, uw16_len);  // ����У�������β��־��

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::gW_ReadCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}

void TH_TPC7062K::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����ֹͣ����

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_TPC7062K_MAX_COM_STOP_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];
    unsigned long int  uw32_DO;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_TPC7062K[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    MbBuf[2] = ChanNo + ASCII_FIRST_UART_NUM;
#endif

    TH_TPC7062K::LoadCheckSum(SEND_COM_STOP_CMD, MbBuf, uw16_len);  // ����У�������β��־��

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::gW_StopCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}


unsigned char TH_TPC7062K::LoadCheckSum(unsigned char CMD_Code,unsigned char *puchMsg, unsigned int usDataLen)
{
//  ����У��ֵ�����Ͱ����档
    int uw32_Count = usDataLen - 4;
    unsigned short int  register_data;
    if(uw32_Count <= 0)
        return (0);

    register_data = MTCI_datatypes::CheckXorSum(puchMsg,usDataLen - 4) & 0X00FF;
    MTCI_datatypes::ctrl_Value2String_Hex((short int *)&register_data,puchMsg + usDataLen - 4);
    puchMsg[usDataLen - 4] = puchMsg[usDataLen - 2];
    puchMsg[usDataLen - 3] = puchMsg[usDataLen - 1];  // ��������
    puchMsg[usDataLen - 2] = uc_COM_WriteCmd_TPC7062K[TH_TPC7062K_MAX_COM_WRITE_CMD - 2];
    puchMsg[usDataLen - 1] = uc_COM_WriteCmd_TPC7062K[TH_TPC7062K_MAX_COM_WRITE_CMD - 1]; // ��д0X2A,0X0D ��β

    return (1);
}


