/*****************************************************************************
// �����ļ�      :TH_DONGGUAN_HUANCE_UMC1200.Cpp    �����¿����и��ִ����ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2017.10.10
*****************************************************************************/


#include "../../Entry/includes.h"





extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function ����ָ��   //��ʽ  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];


//------------------------------------------------------------------------------------------------
// DONGGUAN_HUANCE_UMC1200  ����ͨ�ŷ�ʽ  MODBUS TCP/IP  ���䷽ʽ��RTUģʽ //
//

// Ĭ�ϳ���ʱ�¶�ֵΪ22���
// Ĭ�ϳ�̬ʱʪ��ֵΪ55%RH

//  1-M)�����¶��������ӻ� (����Word����)
//  CMD: {[MBAP (7 byte)] [write command (0x10)] [Starting Address high byte] [Starting Address low byte]
//         [Quantity of Registers  high byte] [Quantity of Registers  low byte] [Byte Count byte] [Registers Value high byte] [Registers Value low byte]}
//  MODEL: 00 00 00 00 00 0B 00 10 00 43 00 02 04 XX XX YY YY     (XX�����¶�ֵ�ȴ���λ�ٴ���λ,YY����ʪ��ֵ�ȴ���λ�ٴ���λ)
const unsigned char uc_COM_WriteTemp4HumiCmd_HUANCE_UMC1200[TH_HUANCE_UMC1200_MAX_COM_WRITE_CMD] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x10, 0x00, 0x2B,
    0x00, 0x02, 0x04, 0x00, 0xDC, 0x02, 0x26
};     // write Master to  Slave cmd  (�����¶�22�ȣ�����220) (�¶�22��,ʪ��55%)


//  �ӻ��ظ�ʱ��Replyһģһ����
//  Reply CMD: {[MBAP (7 byte)] [write command (0x10)] [Starting Address high byte] [Starting Address low byte]
//         [Quantity of Registers  high byte] [Quantity of Registers  low byte] }
//  Reply MODEL: 00 00 00 00 00 0B 00 10 00 43 00 02      (XX�����¶�ֵ�ȴ���λ�ٴ���λ,YY����ʪ��ֵ�ȴ���λ�ٴ���λ)
const unsigned char uc_COM_WriteReply_HUANCE_UMC1200[TH_HUANCE_UMC1200_MAX_COM_WRITE_CMD] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x00, 0x10, 0x00, 0x2B,
    0x00, 0x02
};     // write Slave to  Master reply (�ظ������¶�22�ȣ�����220)

//  2-M)��ȡ�¶��������ӻ�  (����Word����)
//  CMD: {[MBAP (7 byte)] [read command (0x03)] [Starting Address high byte] [Starting Address low byte]
//         [Quantity of Registers  high byte] [Quantity of Registers  low byte] [Byte Count byte] [Registers Value high byte] [Registers Value low byte]}
//  MODEL: 00 00 00 00 00 06 00 03 00 00 00 02
const unsigned char uc_COM_ReadTemp4HumiCmd_HUANCE_UMC1200[TH_HUANCE_UMC1200_MAX_COM_READ_CMD] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x03, 0x00, 0x00,
    0x00, 0x02
};    // read Master to  Slave cmd (��ȡ2����)


//  �ӻ��ظ�ʱ��Replyʹ��full������ʽ����ʹ��integer������ʽ��
//  Reply CMD:  {[MBAP (7 byte)] [read command (0x03)] [Quantity of Registers  high byte] [Quantity of Registers  low byte] [Byte Count byte]
//         [Registers Value high byte] [Registers Value low byte]}
//  Reply MODEL: 00 00 00 00 00 07 00 03 04 XX XX YY YY   (XX�¶�ֵ�ȴ���λ�ٴ���λ; YYʪ��ֵ�ȴ���λ�ٴ���λ)
//  ʹ��full������ʽʱ����ʾֵ=�¶�ֵ/10;    �磺60.0(600/10)��,22.0(220/10)��
const unsigned char uc_COM_ReadReply_HUANCE_UMC1200[TH_HUANCE_UMC1200_MAX_COM_READ_REPLY] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x03, 0x04, 0x00,
    0xDC, 0x02, 0x26
};     // read Slave to  Master reply  //(�ظ��¶�22�ȣ�����2200) (�¶�22.0��,ʪ��55%)

//  3-M)Ҫ�����������������ӻ�
//  CMD: {[MBAP (7 byte)] [write command (0x10)] [Starting Address high byte] [Starting Address low byte]
//         [Quantity of Registers  high byte] [Quantity of Registers  low byte] [Byte Count byte] [Registers Value high byte] [Registers Value low byte]}
//  MODEL: 00 00 00 00 00 09 00 10 00 47 00 01 02 00 01     (XX�����¶�ֵ�ȴ���λ�ٴ���λ,YY����ʪ��ֵ�ȴ���λ�ٴ���λ)

const unsigned char uc_COM_RunCmd_HUANCE_UMC1200[TH_HUANCE_UMC1200_MAX_COM_RUN_CMD] =  {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x10, 0x00, 0x2F,
    0x00, 0x01, 0x02, 0x00, 0x01
};    // read Master to  Slave cmd (д1����)

//  4-M)Ҫ��ֹͣ�����������ӻ�����
//  CMD: {[MBAP (7 byte)] [write command (0x10)] [Starting Address high byte] [Starting Address low byte]
//         [Quantity of Registers  high byte] [Quantity of Registers  low byte] [Byte Count byte] [Registers Value high byte] [Registers Value low byte]}
//  MODEL: 00 00 00 00 00 09 00 10 00 47 00 01 02 00 00     (XX�����¶�ֵ�ȴ���λ�ٴ���λ,YY����ʪ��ֵ�ȴ���λ�ٴ���λ)

const unsigned char uc_COM_StopCmd_HUANCE_UMC1200[TH_HUANCE_UMC1200_MAX_COM_STOP_CMD] =  {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x10, 0x00, 0x2F,
    0x00, 0x01, 0x02, 0x00, 0x00
};     // write Master to  Slave cmd  (д1����)

//
// DONGGUAN_HUANCE_UMC1200  ����ͨ�ŷ�ʽ  MODBUS  ���䷽ʽ��RTUģʽ CRCУ�� //
//----------------------------------------------------------------------------------------------------


void TH_HUANCE_UMC1200::Init(unsigned char ChanNo)
{
// ��ʼ������ָ�롣
    unsigned char uc_i; // ��ֹ�����������ȫ����ʼ��һ�κ���ָ�롣
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_HUANCE_UMC1200::SendReadCmd);   //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_HUANCE_UMC1200::SendWriteCmd);  //  1-M)�����¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_HUANCE_UMC1200::SendReadCmd);    //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_HUANCE_UMC1200::SendRunCmd);      //  3-M)Ҫ�����������������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_HUANCE_UMC1200::SendStopCmd);    //  4-M)Ҫ��ֹͣ�����������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_HUANCE_UMC1200::ParseResult);  //  5-S)��ȡ�¶ȴӻ��ظ���������  (���ֽڲ���)
}

/*----*/
unsigned char TH_HUANCE_UMC1200::SetPresentValue_Check(unsigned char ChanNo, short int Value)
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
    return  1;
}

void TH_HUANCE_UMC1200::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// �������յĽ������
    unsigned char *pCmd;
    //unsigned char *p;
    //unsigned char uc_MoveFlag = 0;
    //bool b_is_temp;
    //bool NoZero;
    short int PresentValue = 0;
    short int PresentValueHumi = 0;
    unsigned short int uw16_i;
    //unsigned short int uw16_j;
    //unsigned short int uw16_k;
    unsigned int uw16_Total;

//     uw16_Total =  uart_bytenum(ChanNo);
    uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

    if( uw16_Total < TH_HUANCE_UMC1200_MIN_RESPONSE_CMD_LEN)
        return;  // �����������ϴ���һ����Ϣ�����ޱ���

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_i = 0;
    do {
//         #ifdef COM_CMD_CHANNEL_DIFFER
// 		if(MTCI_CTRL::gR_CmdBuf[uw16_i] == (ChanNo+1) && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_HUANCE_UMC1200[1] && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_HUANCE_UMC1200[2])
// 		#else
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == uc_COM_ReadReply_HUANCE_UMC1200[5] && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_HUANCE_UMC1200[7] && MTCI_CTRL::gR_CmdBuf[uw16_i+3] == uc_COM_ReadReply_HUANCE_UMC1200[8])
//         #endif
        {   // ���ð�ͷ���β�ж�����ֵ��
            PresentValue = ((short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+4]) << 8 | (short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+5]) );//  �¶�
            PresentValue /=10;  // ��ʾֵ=����ֵ/10;
            if( TH_HUANCE_UMC1200::SetPresentValue_Check(ChanNo, PresentValue) )
            {
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue, SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL);
            }

            PresentValue = ((short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+6]) << 8 | (short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+7]) );//  ʪ��
            //PresentValue /=10;  // ��ʾֵ=����ֵ/10;
            MTCI_CTRL::SetPresentValue(ChanNo, PresentValue, SETPOINTVALUE_HUMI, FIRST_CHAMBER_CHANNEL);

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // ���½��ճ�ʱʱ��

            return;
        }
        uw16_i++;
    } while(uw16_i < uw16_Total);

}

void TH_HUANCE_UMC1200::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����д����ֵ����

    short int  register_data;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_HUANCE_UMC1200_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��


    for(MbChkSum = 0; MbChkSum < (uw16_len-4); MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteTemp4HumiCmd_HUANCE_UMC1200[MbChkSum];
    register_data = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL]*10;

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        //MbBuf[0] = (ChanNo + FIRST_UART_NUM);
    }
#endif

    MbBuf[TH_HUANCE_UMC1200_MAX_COM_WRITE_CMD-4] = (unsigned char) (register_data >> 8);
    MbBuf[TH_HUANCE_UMC1200_MAX_COM_WRITE_CMD-3] = (unsigned char) (register_data & 255);

    register_data = MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL];
    MbBuf[TH_HUANCE_UMC1200_MAX_COM_WRITE_CMD-2] = (unsigned char) (register_data >> 8);
    MbBuf[TH_HUANCE_UMC1200_MAX_COM_WRITE_CMD-1] = (unsigned char) (register_data & 255);
    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);  // ���Ͷ�������
}


void TH_HUANCE_UMC1200::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ������������
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_HUANCE_UMC1200_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < TH_HUANCE_UMC1200_MAX_COM_RUN_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_RunCmd_HUANCE_UMC1200[MbChkSum];

    uw16_len = TH_HUANCE_UMC1200_MAX_COM_RUN_CMD;


#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        //MbBuf[0] = (ChanNo + FIRST_UART_NUM);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}

void TH_HUANCE_UMC1200::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ���Ͷ�����

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_HUANCE_UMC1200_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < TH_HUANCE_UMC1200_MAX_COM_READ_CMD; MbChkSum++)
    {
#ifdef COM_TEMP4HUMI_CMD
        MbBuf[MbChkSum] = uc_COM_ReadTemp4HumiCmd_HUANCE_UMC1200[MbChkSum];
#else
        MbBuf[MbChkSum] = uc_COM_ReadTemp4HumiCmd_HUANCE_UMC1200[MbChkSum];
#endif
    }

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        //MbBuf[0] = (ChanNo + FIRST_UART_NUM);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}

void TH_HUANCE_UMC1200::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����ֹͣ����

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_HUANCE_UMC1200_MAX_COM_STOP_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < TH_HUANCE_UMC1200_MAX_COM_STOP_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_HUANCE_UMC1200[MbChkSum];

    uw16_len = TH_HUANCE_UMC1200_MAX_COM_STOP_CMD;


#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        //MbBuf[0] = (ChanNo + FIRST_UART_NUM);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}



