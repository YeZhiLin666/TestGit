/*****************************************************************************
// �����ļ�      :TH_SIEMENS_S7_SAIA.Cpp    �����¿����и��ִ����ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2017.10.10
*****************************************************************************/


#include "../../Entry/includes.h"



extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function ����ָ��   //��ʽ  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];


//------------------------------------------------------------------------------------------------
// TH_SIEMENS_S7_SAIA  ����ͨ�ŷ�ʽ  MODBUS TCP/IP  ���䷽ʽ��RTUģʽ //
//

// Ĭ�ϳ���ʱ�¶�ֵΪ22���
// Ĭ�ϳ�̬ʱʪ��ֵΪ55%RH

//  1-M)�����¶��������ӻ� (�Ը���������)
//  CMD: {[MBAP (8 byte)] [ORG ID] [DB Number] [Starting Address high byte] [Starting Address low byte]
//         [Quantity of Registers  high byte] [Quantity of Registers  low byte] [Empty field] [Length Empty field] [Registers Value high byte] [Registers Value low byte]}
//  MODEL: 53 35 10 01 03 03 03 08 01 3E 03 F0 00 02 FF 02 XX XX XX XX     (XX�����¶ȸ������ȴ���λ�ٴ���λ)
//  MODEL: 53 35 10 01 03 03 03 08 01 3E 03 F8 00 02 FF 02 XX XX XX XX     (XX����ʪ�ȸ������ȴ���λ�ٴ���λ)
const unsigned char uc_COM_WriteTemp_SIEMENS_S7_SAIA[TH_SIEMENS_S7_SAIA_MAX_COM_WRITE_CMD] = {
    0x53, 0x35, 0x10, 0x01, 0x03, 0x03, 0x03, 0x08, 0x01, 0x3E, 0x03, 0xF0,
    0x00, 0x02, 0xFF, 0x02, 0x41, 0xB0, 0x00, 0x00
};     // write Master to  Slave cmd  (�����¶�22��)

const unsigned char uc_COM_WriteHumi_SIEMENS_S7_SAIA[TH_SIEMENS_S7_SAIA_MAX_COM_WRITE_CMD] = {
    0x53, 0x35, 0x10, 0x01, 0x03, 0x03, 0x03, 0x08, 0x01, 0x3E, 0x03, 0xF8,
    0x00, 0x02, 0xFF, 0x02, 0x42, 0x5C, 0x00, 0x00
};     // write Master to  Slave cmd  (����ʪ��55%)

//  �ӻ��ظ�ʱ��Replyһģһ����
//  Reply CMD: {[MBAP (8 byte)] [Error Field] [Empty Field] [Length Empty field] }
//  Reply MODEL: 53 35 10 01 03 04 0F 03 00 FF 07
const unsigned char uc_COM_WriteReply_SIEMENS_S7_SAIA[TH_SIEMENS_S7_SAIA_MAX_COM_WRITE_REPLY] = {
    0x53, 0x35, 0x10, 0x01, 0x03, 0x04, 0x0F, 0x03, 0x00, 0xFF, 0x07
};     // write Slave to  Master reply (�ظ������¶�)

//  2-M)��ȡ�¶��������ӻ�  (����Word����)
//  CMD: {[MBAP (8 byte)] [ORG ID] [DB Number] [Starting Address high byte] [Starting Address low byte]
//         [Quantity of Registers  high byte] [Quantity of Registers  low byte] [Empty field] [Length Empty field] }
//  MODEL: 53 35 10 01 03 05 03 08 01 3E 00 40 00 02 FF 02 �¶�
//  MODEL: 53 35 10 01 03 05 03 08 01 3E 00 44 00 02 FF 02 ʪ��
const unsigned char uc_COM_ReadTempCmd_SIEMENS_S7_SAIA[TH_SIEMENS_S7_SAIA_MAX_COM_READ_CMD] = {
    0x53, 0x35, 0x10, 0x01, 0x03, 0x05, 0x03, 0x08, 0x01, 0x3E, 0x00, 0x40,
    0x00, 0x02, 0xFF, 0x02
};    // read Master to  Slave cmd

const unsigned char uc_COM_ReadHumiCmd_SIEMENS_S7_SAIA[TH_SIEMENS_S7_SAIA_MAX_COM_READ_CMD] = {
    0x53, 0x35, 0x10, 0x01, 0x03, 0x05, 0x03, 0x08, 0x01, 0x3E, 0x00, 0x44,
    0x00, 0x02, 0xFF, 0x02
};    // read Master to  Slave cmd

//  �ӻ��ظ�ʱ��
//  Reply CMD:  {[MBAP (8 byte)] [Error Field] [Empty Field] [Length Empty field]
//         [Registers Value high byte] [Registers Value low byte]}
//  Reply MODEL: 53 35 10 01 03 06 0F 03 00 FF 07 XX XX XX XX   (XX �¶�/ʪ�� ������ �ȴ���λ�ٴ���λ)
const unsigned char uc_COM_ReadReply_SIEMENS_S7_SAIA[TH_SIEMENS_S7_SAIA_MAX_COM_READ_REPLY] = {
    0x53, 0x35, 0x10, 0x01, 0x03, 0x06, 0x0F, 0x03, 0x00, 0xFF, 0x07,
    0x41,0xB0, 0x00, 0x00
};     // read Slave to  Master reply  //(�ظ��¶�22��)

//  3-M)Ҫ�����������������ӻ�
//  CMD: {[MBAP (8 byte)] [ORG ID] [DB Number] [Starting Address high byte] [Starting Address low byte]
//         [Quantity of Registers  high byte] [Quantity of Registers  low byte] [Empty field] [Length Empty field] [Registers Value high byte] [Registers Value low byte]}
//  MODEL: 53 35 10 01 03 03 03 08 01 3E 03 E8 00 01 FF 02 03 01

const unsigned char uc_COM_RunCmd_SIEMENS_S7_SAIA[TH_SIEMENS_S7_SAIA_MAX_COM_RUN_CMD] =  {
    0x53, 0x35, 0x10, 0x01, 0x03, 0x03, 0x03, 0x08, 0x01, 0x3E, 0x03, 0xE8,
    0x00, 0x01, 0xFF, 0x02, 0x03, 0x01
};    // read Master to  Slave cmd

//  4-M)Ҫ��ֹͣ�����������ӻ�����
//  CMD: {[MBAP (8 byte)] [ORG ID] [DB Number] [Starting Address high byte] [Starting Address low byte]
//         [Quantity of Registers  high byte] [Quantity of Registers  low byte] [Empty field] [Length Empty field] [Registers Value high byte] [Registers Value low byte]}
//  MODEL: 53 35 10 01 03 03 03 08 01 3E 03 E8 00 01 FF 02 00 00

const unsigned char uc_COM_StopCmd_SIEMENS_S7_SAIA[TH_SIEMENS_S7_SAIA_MAX_COM_STOP_CMD] =  {
    0x53, 0x35, 0x10, 0x01, 0x03, 0x03, 0x03, 0x08, 0x01, 0x3E, 0x03, 0xE8,
    0x00, 0x01, 0xFF, 0x02, 0x00, 0x00
};     // write Master to  Slave cmd

//  �ӻ��ظ�ʱ��Replyһģһ����
//  Reply CMD: {[MBAP (7 byte)] [write command (0x10)] [Starting Address high byte] [Starting Address low byte]
//         [Quantity of Registers  high byte] [Quantity of Registers  low byte] }
//  Run  Reply MODEL: 00 00 00 00 00 06 01 10 00 C8 00 01
//  Stop Reply MODEL: 00 00 00 00 00 06 01 10 00 C8 00 01
// const unsigned char uc_COM_RUNorStopReply_SIEMENS_S7_SAIA[TH_SIEMENS_S7_SAIA_MAX_COM_RUNorSTOP_REPLY] = {
//     0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x01, 0x10, 0x00, 0xC8,
//     0x00, 0x01
// };     // write Slave to  Master reply (�ظ������¶�)

//
// TH_SIEMENS_S7_SAIA  ����ͨ�ŷ�ʽ  MODBUS TCP/IP  ���䷽ʽ��RTUģʽ //
//----------------------------------------------------------------------------------------------------

// ע�⣺�������²���  #define  COM_INTERVAL_WRITE_AND_READ��ʹ�ü��д�Ͷ�����
//   <q> COM INTERVAL WRITE AND READ Enable:  Off
// #define  COM_INTERVAL_WRITE_AND_READ                 0         //�Ƿ�ʹ�ô��ڼ��д�Ͷ����� Ϊ1��ʾʹ�ü��д�Ͷ����ܣ�Ϊ0��ʾ��ʹ�ü��д�Ͷ�����
// #define PUBLIC_REPEAT_NUM	      (3)    // ������󹫹��ظ�����
// #define CMD_INTERVALTIME	     (200*10)   // ÿ�η���������ʱ�� 200����

//static unsigned short uw16_V1=0,uw16_V2=0,uw16_V3=0,uw16_V4=0;
static unsigned char uc_T_Update_Limit_Counter=0;
static unsigned char uc_H_Update_Limit_Counter=0;

void TH_SIEMENS_S7_SAIA::Init(unsigned char ChanNo)
{
// ��ʼ������ָ�롣
    unsigned char uc_i; // ��ֹ�����������ȫ����ʼ��һ�κ���ָ�롣
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_SIEMENS_S7_SAIA::SendReadCmd);   //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_SIEMENS_S7_SAIA::SendWriteCmd);  //  1-M)�����¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_SIEMENS_S7_SAIA::SendReadCmd);    //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_SIEMENS_S7_SAIA::SendRunCmd);      //  3-M)Ҫ�����������������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_SIEMENS_S7_SAIA::SendStopCmd);    //  4-M)Ҫ��ֹͣ�����������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_SIEMENS_S7_SAIA::ParseResult);  //  5-S)��ȡ�¶ȴӻ��ظ���������  (���ֽڲ���)

    //MTCI_CTRL::uw16_PUBLIC_REPEAT_NUM = 3; // PUBLIC_REPEAT_NUM;
    //MTCI_CTRL::uw32_CMD_INTERVALTIME = 200*10; //CMD_INTERVALTIME;
}

/*----*/
unsigned char TH_SIEMENS_S7_SAIA::SetPresentValue_Check(unsigned char ChanNo, short int Value)
{
// ������ʱȡ�ص�ֵ

    if(	Value == 0)
    {   // �ܹ�������ֵ���ֵ��ʾ���֡�
        if( MTCI_CTRL::gR_Zero_Counter[ChanNo]++ <= 5)
            return  0;
        else
            MTCI_CTRL::gR_Zero_Counter[ChanNo] = 0;
    }
    else
    {
        MTCI_CTRL::gR_Zero_Counter[ChanNo] = 0;
    }


    // �ܹ�������ֵĶ�����ֵ��ʾ��
    unsigned short int uw16_V = 0;
    if(MTCI_CTRL::gR_ReadCmd_Flag[ChanNo] == SETPOINTVALUE_TEMPERATURE)
    {
        uw16_V = abs(MTCI_CTRL::str_Uart[ChanNo].gR_TempLastValue[0] - Value);
        if(uw16_V > 200)  // 2���Ӳ���10��
        {
            if(++uc_T_Update_Limit_Counter < 5)
                return 0;
        }
        else
            uc_T_Update_Limit_Counter = 0;
    }
    else if(MTCI_CTRL::gR_ReadCmd_Flag[ChanNo] == SETPOINTVALUE_HUMI)
    {
        uw16_V = abs(MTCI_CTRL::str_Uart[ChanNo].gR_HumiLastValue[0] - Value);
        if(uw16_V > 100)
        {
            if(++uc_H_Update_Limit_Counter < 5)
                return 0;
        }
        uc_H_Update_Limit_Counter = 0;
    }
    return  1;
}

void TH_SIEMENS_S7_SAIA::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// �������յĽ������
    unsigned char *pCmd;
    short int PresentValue = 0;
    short int PresentValueHumi = 0;
    unsigned short int uw16_i;
    unsigned char uc_n = 0;
    unsigned int uw16_Total;
    union UW_F
    {
        float f_member;
        unsigned char uc_member[4];  // ��λ��ǰ
    } F_data;
		
    uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

    if( uw16_Total < TH_SIEMENS_S7_SAIA_MIN_RESPONSE_CMD_LEN)
        return;  // �����������ϴ���һ����Ϣ�����ޱ���

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);


    uw16_i = 0;
    do {
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == uc_COM_ReadReply_SIEMENS_S7_SAIA[5]    //  0x06
                && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_SIEMENS_S7_SAIA[7] // 0x03
                && MTCI_CTRL::gR_CmdBuf[uw16_i+3] == uc_COM_ReadReply_SIEMENS_S7_SAIA[8]) // 0x00
        {   // ���ð�ͷ���β�ж�����ֵ��

            unsigned int uw16_k = uw16_Total - uw16_i + 5 ;  // ��uw16_i��ʼ������ͷ�����ֽ���
            unsigned int uw16_m = uw16_Total - (uw16_i + 6);    // ��uw16_i+3��ʼ������β�Ͳ���4�ֽ���4

            long g_Time = CommonFunc::GetTimeTicks( &MTCI_CTRL::gR_ReadCmd_Stamp[ChanNo]);  // ���ճ�ʱʱ��
            if((g_Time < 240)||(g_Time > MTCI_CTRL::uw32_CMD_INTERVALTIME*1.8))   //  С��[READ_REPLY*2*10] or ����һ��һд���ʱ��(N*2)
            {
                CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // ���½��ճ�ʱʱ��    600ms-(344)����ֵ   344/13=26.46
                return;
            }

            if((uw16_k < TH_SIEMENS_S7_SAIA_MAX_COM_READ_REPLY)
                    ||(uw16_m < 4)
                    ||(MTCI_CTRL::g_Cmd_Flag[ChanNo] == SEND_COM_RUN_CMD)
                    ||(MTCI_CTRL::g_Cmd_Flag[ChanNo] == SEND_COM_STOP_CMD)
                    ||(MTCI_CTRL::gR_ReadCmd_Flag[ChanNo] != uc_Temp4Humi_Flag)) // ��������¶���ʪ�Ƚ�����Ŵ���
            {
                CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // ���½��ճ�ʱʱ��
                return;
            }

            for(uc_n=0; uc_n<4; uc_n++)
                F_data.uc_member[uc_n]	=	MTCI_CTRL::gR_CmdBuf[uw16_i+9-uc_n];
            PresentValue = (short int)(F_data.f_member*10);//  �¶�/ʪ��

            if( TH_SIEMENS_S7_SAIA::SetPresentValue_Check(ChanNo, PresentValue) )
            {
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue, uc_Temp4Humi_Flag, FIRST_CHAMBER_CHANNEL);
            }

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // ���½��ճ�ʱʱ��
            return;
        }
        uw16_i++;
    } while(uw16_i < uw16_Total);

}

void TH_SIEMENS_S7_SAIA::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����д����ֵ����

    short int  register_data;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_SIEMENS_S7_SAIA_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];
    unsigned char uc_n = 0;
    union UW_F
    {
        float f_member;
        unsigned char uc_member[4]; // ��λ��ǰ
    } F_data;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
    {
        for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_WriteHumi_SIEMENS_S7_SAIA[MbChkSum];
        F_data.f_member = (float) (MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL]/10);
        for(uc_n=0; uc_n<4; uc_n++)
            MbBuf[uw16_len-1-uc_n] = F_data.uc_member[uc_n];
    }
    else
    {
        for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_WriteTemp_SIEMENS_S7_SAIA[MbChkSum];
        F_data.f_member = (float) (MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL]/10);
        for(uc_n=0; uc_n<4; uc_n++)
            MbBuf[uw16_len-1-uc_n] = F_data.uc_member[uc_n];
    }

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);  // ���Ͷ�������
}


void TH_SIEMENS_S7_SAIA::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ������������
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_SIEMENS_S7_SAIA_MAX_COM_RUN_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_RunCmd_SIEMENS_S7_SAIA[MbChkSum];

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}

void TH_SIEMENS_S7_SAIA::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ���Ͷ�����

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_SIEMENS_S7_SAIA_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
    {
#ifdef COM_TEMP4HUMI_CMD
        if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
        {
            MbBuf[MbChkSum] = uc_COM_ReadHumiCmd_SIEMENS_S7_SAIA[MbChkSum];
        }
        else
        {
            MbBuf[MbChkSum] = uc_COM_ReadTempCmd_SIEMENS_S7_SAIA[MbChkSum];
        }
#else
        MbBuf[MbChkSum] = uc_COM_ReadTempCmd_SIEMENS_S7_SAIA[MbChkSum];
#endif
    }

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}

void TH_SIEMENS_S7_SAIA::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����ֹͣ����

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_SIEMENS_S7_SAIA_MAX_COM_STOP_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_SIEMENS_S7_SAIA[MbChkSum];

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}



