/*****************************************************************************
// �����ļ�      :TH_TT_5166_Sunwoda.C    �����¿����и��ִ����ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2017.10.10
*****************************************************************************/


#include "../../Entry/includes.h"





extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function ����ָ��   //��ʽ  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

//------------------------------------------------------------------------------------------------
// TT_5166  ����ͨ�ŷ�ʽ  MODBUS  ���䷽ʽ��RTUģʽ CRCУ�� //
//

// Ĭ�ϳ���ʱ�¶�ֵΪ22���
// Ĭ�ϳ�̬ʱʪ��ֵΪ55%RH

//  1-M)�����¶��������ӻ� (����Word����)
//  CMD: {[controller address (1 byte)] [write command (0x06)] [register high byte] [register low byte] [data high byte] [data low byte] [CRC low byte] [CRC high byte]}
//  MODEL: 00 06 00 26 XX XX YY YY    (XX����ֵ�ȴ���λ�ٴ���λ; YYУ��ֵ�ȴ���λ�ٴ���λ)
const unsigned char uc_COM_WriteTempCmd_TT_5166[TH_TT_5166_Sunwoda_MAX_COM_WRITE_CMD] = {
    0x01, 0x06, 0x03, 0x8E, 0x00, 0xDC, 0x68, 0x49
};     // write Master to  Slave cmd  (�����¶�22�ȣ�����220)

//  MODEL: 00 06 00 27 XX XX YY YY   (XX����ֵ�ȴ���λ�ٴ���λ; YYУ��ֵ�ȴ���λ�ٴ���λ)
//01 06 00 48 XX XX YY YY    (XX����ֵ�ȴ���λ�ٴ���λ; YYУ��ֵ�ȴ���λ�ٴ���λ)
//����ֵ=ʪ��ֵ*10;    �磺55(55*10),85(85*10) %ʪ��Humi.
const unsigned char uc_COM_WriteHumiCmd_TT_5166[TH_TT_5166_Sunwoda_MAX_COM_WRITE_CMD] = {
    0x01, 0x06, 0x00, 0x27, 0x02, 0x26, 0xB8, 0xAA
};     // write Master to  Slave cmd  (����ʪ��55%������550)

//  �ӻ��ظ�ʱ��Replyһģһ����
//  Reply CMD: {[controller address (1 byte)] [write command (0x06)] [register high byte] [register low byte] [data high byte] [data low byte] [CRC low byte] [CRC high byte]}
//  Reply MODEL: 00 06 00 26 XX XX YY YY   (XX����ֵ�ȴ���λ�ٴ���λ; YYУ��ֵ�ȴ���λ�ٴ���λ)
const unsigned char uc_COM_WriteReply_TT_5166[TH_TT_5166_Sunwoda_MAX_COM_WRITE_CMD] = {
    0x01, 0x06, 0x00, 0x26, 0x00, 0xDC, 0x68, 0x49
};     // write Slave to  Master reply (�ظ������¶�22�ȣ�����220)

//  2-M)��ȡ�¶��������ӻ�  (����Word����)
//  CMD: {[controller address (1 byte)] [read command (0x03)] [starting register high byte] [starting register low byte] [number of registers high byte (0x00)] [number of registers low byte] [CRC low byte] [CRC high byte]}
//  MODEL: 00 03 00 00 00 01 YY YY   (YYУ��ֵ�ȴ���λ�ٴ���λ)
const unsigned char uc_COM_ReadTempCmd_TT_5166[TH_TT_5166_Sunwoda_MAX_COM_READ_CMD] = {
    0x01, 0x03, 0x00, 0x14, 0x00, 0x01, 0xc4, 0x0e
};    // read Master to  Slave cmd (��ȡ1����)

//  MODEL: 00 03 00 03 00 01 YY YY   (YYУ��ֵ�ȴ���λ�ٴ���λ)
const unsigned char uc_COM_ReadHumiCmd_TT_5166[TH_TT_5166_Sunwoda_MAX_COM_READ_CMD] = {
    0x01, 0x03, 0x00, 0x03, 0x00, 0x01, 0x75, 0xDB
};    // read Master to  Slave cmd (��ȡ1����)

//  �ӻ��ظ�ʱ��Replyʹ��full������ʽ����ʹ��integer������ʽ��
//  Reply CMD:  {[controller address (1 byte)] [read command (0x03)] [number of bytes (1 byte)] [first register data high byte] [first register data low byte] [CRC low byte] [CRC high byte]}
//  Reply MODEL: 00 03 02 XX XX YY YY   (XX�¶�ֵ�ȴ���λ�ٴ���λ; YYУ��ֵ�ȴ���λ�ٴ���λ)
//  ʹ��full������ʽʱ����ʾֵ=�¶�ֵ/100;    �磺60(6000/100)��,22(2200/100)��
const unsigned char uc_COM_ReadReply_TT_5166[TH_TT_5166_Sunwoda_MAX_COM_READ_REPLY] = {
    0x01, 0x03, 0x02, 0x08, 0x98, 0x83, 0xEE
};     // read Slave to  Master reply  //(�ظ��¶�22�ȣ�����2200)

//  3-M)Ҫ�����������������ӻ�
//  CMD: {[controller address (1 byte)] [read command (0x03)] [starting register high byte] [starting register low byte] [number of registers high byte (0x00)] [number of registers low byte] [CRC low byte] [CRC high byte]}
//  MODEL: 00 05 00 00 FF 00 YY YY   (YYУ��ֵ�ȴ���λ�ٴ���λ)


const unsigned char uc_COM_RunCmd_TT_5166[TH_TT_5166_Sunwoda_MAX_COM_RUN_CMD] =  {
    0x01, 0x05, 0x00, 0x00, 0xFF, 0x00, 0x8C, 0x3A
};    // read Master to  Slave cmd (��ȡ1����)

//  4-M)Ҫ��ֹͣ�����������ӻ�����
//  CMD: {[controller address (1 byte)] [write command (0x06)] [register high byte] [register low byte] [data high byte] [data low byte] [CRC low byte] [CRC high byte]}
//  MODEL: 00 05 00 01 FF 00 YY YY    (XX����ֵ�ȴ���λ�ٴ���λ; YYУ��ֵ�ȴ���λ�ٴ���λ)

const unsigned char uc_COM_StopCmd_TT_5166[TH_TT_5166_Sunwoda_MAX_COM_STOP_CMD] =  {
    0x01, 0x05, 0x00, 0x01, 0xFF, 0x00, 0xDD, 0xFA
};     // write Master to  Slave cmd  (�����¶�22�ȣ�����220)

//
// TT_5166  ����ͨ�ŷ�ʽ  MODBUS  ���䷽ʽ��RTUģʽ CRCУ�� //
//----------------------------------------------------------------------------------------------------


void TH_TT_5166_Sunwoda::Init(unsigned char ChanNo)
{
// ��ʼ������ָ�롣
    unsigned char uc_i; // ��ֹ�����������ȫ����ʼ��һ�κ���ָ�롣
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_TT_5166_Sunwoda::SendReadCmd);   //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_TT_5166_Sunwoda::SendWriteCmd);  //  1-M)�����¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_TT_5166_Sunwoda::SendReadCmd);    //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_TT_5166_Sunwoda::SendRunCmd);      //  3-M)Ҫ�����������������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_TT_5166_Sunwoda::SendStopCmd);    //  4-M)Ҫ��ֹͣ�����������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_TT_5166_Sunwoda::ParseResult);  //  5-S)��ȡ�¶ȴӻ��ظ���������  (���ֽڲ���)
}

/*----*/
unsigned char TH_TT_5166_Sunwoda::SetPresentValue_Check(unsigned char ChanNo, short int Value)
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

void TH_TT_5166_Sunwoda::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
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
	

// //     uw16_Total =  uart_bytenum(ChanNo);
//     uw16_Total =  TH_TT_5166_Sunwoda::ParseResult_FirstCheck(ChanNo);

//     if( uw16_Total < TH_TT_5166_Sunwoda_MIN_RESPONSE_CMD_LEN)
//         return;  // �����������ϴ���һ����Ϣ�����ޱ���

//     if( !TH_TT_5166_Sunwoda::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf,TH_TT_5166_Sunwoda_MAX_RESPONSE_CMD_LEN) )
//         return;   // ������Ϣ���������ޱ��� // ת������

		uw16_Total =  UARTFunc::uart_bytenum(ChanNo);
		
    if( uw16_Total < TH_TT_5166_Sunwoda_MIN_RESPONSE_CMD_LEN)
        return;  // �����������ϴ���һ����Ϣ�����ޱ���

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);
		
    uw16_i = 0;
    do {
//         #ifdef COM_CMD_CHANNEL_DIFFER
// 		if(MTCI_CTRL::gR_CmdBuf[uw16_i] == (ChanNo+1) && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_TT_5166[1] && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_TT_5166[2])
// 		#else
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == uc_COM_ReadReply_TT_5166[0] && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_TT_5166[1] && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_TT_5166[2])
//         #endif
        {   // ���ð�ͷ���β�ж�����ֵ��
            PresentValue = ((short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+3]) << 8 | (short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+4]) );//  �¶�
            if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
                PresentValue /=10;  // ��ʾֵ=����ֵ/100;
            if( TH_TT_5166_Sunwoda::SetPresentValue_Check(ChanNo, PresentValue) )
            {
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue, uc_Temp4Humi_Flag, FIRST_CHAMBER_CHANNEL);
            }

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // ���½��ճ�ʱʱ��

            return;
        }
        uw16_i++;
    } while(uw16_i < uw16_Total);

}

void TH_TT_5166_Sunwoda::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����д����ֵ����

    short int  register_data;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_TT_5166_Sunwoda_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

//     if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
//     {
//         for(MbChkSum = 0; MbChkSum < (uw16_len-4); MbChkSum++)
//             MbBuf[MbChkSum] = uc_COM_WriteHumiCmd_TT_5166[MbChkSum];
//         register_data = MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL];
//     }
//     else if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
//     {
        for(MbChkSum = 0; MbChkSum < (uw16_len-4); MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_WriteTempCmd_TT_5166[MbChkSum];
        register_data = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
//     }
//     else
//         return;


#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
    }
#endif

    MbBuf[TH_TT_5166_Sunwoda_MAX_COM_WRITE_CMD-4] = (unsigned char) (register_data >> 8);
    MbBuf[TH_TT_5166_Sunwoda_MAX_COM_WRITE_CMD-3] = (unsigned char) (register_data & 255);
    MbChkSum = MTCI_datatypes::CRC16(MbBuf,uw16_len-2);
    MbBuf[TH_TT_5166_Sunwoda_MAX_COM_WRITE_CMD-2] = (unsigned char)(MbChkSum >> 8);
    MbBuf[TH_TT_5166_Sunwoda_MAX_COM_WRITE_CMD-1] = (unsigned char)(MbChkSum & 255);
    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);  // ���Ͷ�������
}


void TH_TT_5166_Sunwoda::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ������������
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_TT_5166_Sunwoda_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < TH_TT_5166_Sunwoda_MAX_COM_RUN_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_RunCmd_TT_5166[MbChkSum];

    uw16_len = TH_TT_5166_Sunwoda_MAX_COM_RUN_CMD;


#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
        MbChkSum = MTCI_datatypes::CRC16(MbBuf,uw16_len-2);
        MbBuf[TH_TT_5166_Sunwoda_MAX_COM_READ_CMD-2] = (unsigned char)(MbChkSum >> 8);
        MbBuf[TH_TT_5166_Sunwoda_MAX_COM_READ_CMD-1] = (unsigned char)(MbChkSum & 255);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}

void TH_TT_5166_Sunwoda::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ���Ͷ�����

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_TT_5166_Sunwoda_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < TH_TT_5166_Sunwoda_MAX_COM_READ_CMD; MbChkSum++)
    {
#ifdef COM_TEMP4HUMI_CMD
//         if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
//         {
//             MbBuf[MbChkSum] = uc_COM_ReadHumiCmd_TT_5166[MbChkSum];
//         }
//         else
//         {
            MbBuf[MbChkSum] = uc_COM_ReadTempCmd_TT_5166[MbChkSum];
//         }
#else
        MbBuf[MbChkSum] = uc_COM_ReadTempCmd_TT_5166[MbChkSum];
#endif
    }

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
        MbChkSum = MTCI_datatypes::CRC16(MbBuf,uw16_len-2);
        MbBuf[TH_TT_5166_Sunwoda_MAX_COM_READ_CMD-2] = (unsigned char)(MbChkSum >> 8);
        MbBuf[TH_TT_5166_Sunwoda_MAX_COM_READ_CMD-1] = (unsigned char)(MbChkSum & 255);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}

void TH_TT_5166_Sunwoda::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����ֹͣ����

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_TT_5166_Sunwoda_MAX_COM_STOP_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < TH_TT_5166_Sunwoda_MAX_COM_STOP_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_TT_5166[MbChkSum];

    uw16_len = TH_TT_5166_Sunwoda_MAX_COM_STOP_CMD;


#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
        MbChkSum = MTCI_datatypes::CRC16(MbBuf,uw16_len-2);
        MbBuf[TH_TT_5166_Sunwoda_MAX_COM_WRITE_CMD-2] = (unsigned char)(MbChkSum >> 8);
        MbBuf[TH_TT_5166_Sunwoda_MAX_COM_WRITE_CMD-1] = (unsigned char)(MbChkSum & 255);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}



