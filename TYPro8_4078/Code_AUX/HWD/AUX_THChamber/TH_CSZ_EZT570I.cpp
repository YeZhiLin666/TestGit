/*****************************************************************************
// �����ļ�      :TH_CSZ_EZT570I.C    �����¿����и��ִ����ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/


#include "../../Entry/includes.h"





extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function ����ָ��   //��ʽ  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];


//------------------------------------------------------------------------------------------------
// EZT570I  ����ͨ�ŷ�ʽ  MODBUS  ���䷽ʽ��RTUģʽ CRCУ�� //
//

// Ĭ�ϳ���ʱ�¶�ֵΪ22���
// Ĭ�ϳ�̬ʱʪ��ֵΪ55%RH

//  1-M)�����¶��������ӻ� (����Word����)
//  CMD: {[controller address (1 byte)] [write command (0x06)] [register high byte] [register low byte] [data high byte] [data low byte] [CRC low byte] [CRC high byte]}
//  MODEL: 01 06 00 3C XX XX YY YY    (XX����ֵ�ȴ���λ�ٴ���λ; YYУ��ֵ�ȴ���λ�ٴ���λ)
const unsigned char uc_COM_WriteTempCmd_EZT570I[TH_EZT570I_MAX_COM_WRITE_CMD] = {
    0x01, 0x06, 0x00, 0x3C, 0x00, 0xDC, 0x48, 0x5F
};     // write Master to  Slave cmd  (�����¶�22�ȣ�����220)

//  MODEL: 01 06 00 48 XX XX YY YY   (XX����ֵ�ȴ���λ�ٴ���λ; YYУ��ֵ�ȴ���λ�ٴ���λ)
//01 06 00 48 XX XX YY YY    (XX����ֵ�ȴ���λ�ٴ���λ; YYУ��ֵ�ȴ���λ�ٴ���λ)
//����ֵ=ʪ��ֵ*10;    �磺55(55*10),85(85*10) %ʪ��Humi.
const unsigned char uc_COM_WriteHumiCmd_EZT570I[TH_EZT570I_MAX_COM_WRITE_CMD] = {
    0x01, 0x06, 0x00, 0x48, 0x02, 0x26, 0x89, 0x66
};     // write Master to  Slave cmd  (����ʪ��55%������550)

//  �ӻ��ظ�ʱ��Replyһģһ����
//  Reply CMD: {[controller address (1 byte)] [write command (0x06)] [register high byte] [register low byte] [data high byte] [data low byte] [CRC low byte] [CRC high byte]}
//  Reply MODEL: 01 06 00 3C XX XX YY YY   (XX����ֵ�ȴ���λ�ٴ���λ; YYУ��ֵ�ȴ���λ�ٴ���λ)
const unsigned char uc_COM_WriteReply_EZT570I[TH_EZT570I_MAX_COM_WRITE_CMD] = {
    0x01, 0x06, 0x00, 0x3C, 0x00, 0xDC, 0x48, 0x5F
};     // write Slave to  Master reply (�ظ������¶�22�ȣ�����220)

//  2-M)��ȡ�¶��������ӻ�  (����Word����)
//  CMD: {[controller address (1 byte)] [read command (0x03)] [starting register high byte] [starting register low byte] [number of registers high byte (0x00)] [number of registers low byte] [CRC low byte] [CRC high byte]}
//  MODEL: 01 03 00 3D 00 01 YY YY   (YYУ��ֵ�ȴ���λ�ٴ���λ)
const unsigned char uc_COM_ReadTempCmd_EZT570I[TH_EZT570I_MAX_COM_READ_CMD] = {
    0x01, 0x03, 0x00, 0x3D, 0x00, 0x01, 0x15, 0xC6
};    // read Master to  Slave cmd (��ȡ1����)

//  MODEL: 01 03 00 3D 00 01 YY YY   (YYУ��ֵ�ȴ���λ�ٴ���λ)
const unsigned char uc_COM_ReadHumiCmd_EZT570I[TH_EZT570I_MAX_COM_READ_CMD] = {
    0x01, 0x03, 0x00, 0x49, 0x00, 0x01, 0x55, 0xDC
};    // read Master to  Slave cmd (��ȡ1����)

//  �ӻ��ظ�ʱ��Replyʹ��full������ʽ����ʹ��integer������ʽ��
//  Reply CMD:  {[controller address (1 byte)] [read command (0x03)] [number of bytes (1 byte)] [first register data high byte] [first register data low byte] [CRC low byte] [CRC high byte]}
//  Reply MODEL: 01 03 02 XX XX YY YY   (XX�¶�ֵ�ȴ���λ�ٴ���λ; YYУ��ֵ�ȴ���λ�ٴ���λ)
//  ʹ��full������ʽʱ����ʾֵ=�¶�ֵ/10;    �磺60(600/10)��,22(220/10)��
const unsigned char uc_COM_ReadReply_EZT570I[TH_EZT570I_MAX_COM_READ_REPLY] = {
    0x01, 0x03, 0x02, 0x00, 0xDC, 0xB9, 0xDD
};     // read Slave to  Master reply  //(�ظ��¶�22�ȣ�����220)

//  3-M)Ҫ�����������������ӻ�
//  CMD: {[controller address (1 byte)] [read command (0x03)] [starting register high byte] [starting register low byte] [number of registers high byte (0x00)] [number of registers low byte] [CRC low byte] [CRC high byte]}
//  MODEL: 01 03 00 3D 00 01 YY YY   (YYУ��ֵ�ȴ���λ�ٴ���λ)
//  �ӻ��ظ�ʱ��Replyʹ��full������ʽ����ʹ��integer������ʽ��
//  Reply CMD:  {[controller address (1 byte)] [read command (0x03)] [number of bytes (1 byte)] [first register data high byte] [first register data low byte] [CRC low byte] [CRC high byte]}
//  Reply MODEL: 01 03 02 XX XX YY YY   (XX�¶�ֵ�ȴ���λ�ٴ���λ; YYУ��ֵ�ȴ���λ�ٴ���λ)
//  ʹ��full������ʽʱ����ʾֵ=�¶�ֵ/10;    �磺60(600/10)��,22(220/10)��


const unsigned char uc_COM_RunCmd_EZT570I[TH_EZT570I_MAX_COM_RUN_CMD] =  {
    0x01, 0x03, 0x00, 0x3D, 0x00, 0x01, 0x15, 0xC6
};    // read Master to  Slave cmd (��ȡ1����)

//  4-M)Ҫ��ֹͣ�����������ӻ�����
//  CMD: {[controller address (1 byte)] [write command (0x06)] [register high byte] [register low byte] [data high byte] [data low byte] [CRC low byte] [CRC high byte]}
//  MODEL: 01 06 00 3C XX XX YY YY    (XX����ֵ�ȴ���λ�ٴ���λ; YYУ��ֵ�ȴ���λ�ٴ���λ)
//  �ӻ��ظ�ʱ��Replyһģһ����
//  Reply CMD: {[controller address (1 byte)] [write command (0x06)] [register high byte] [register low byte] [data high byte] [data low byte] [CRC low byte] [CRC high byte]}
//  Reply MODEL: 01 06 00 3C XX XX YY YY   (XX����ֵ�ȴ���λ�ٴ���λ; YYУ��ֵ�ȴ���λ�ٴ���λ)

const unsigned char uc_COM_StopCmd_EZT570I[TH_EZT570I_MAX_COM_STOP_CMD] =  {
    0x01, 0x06, 0x00, 0x3C, 0x00, 0xDC, 0x48, 0x5F
};     // write Master to  Slave cmd  (�����¶�22�ȣ�����220)

//
// EZT570I  ����ͨ�ŷ�ʽ  MODBUS  ���䷽ʽ��RTUģʽ CRCУ�� //
//----------------------------------------------------------------------------------------------------
void TH_EZT570I::Init(unsigned char ChanNo)
{
// ��ʼ������ָ�롣
    unsigned char uc_i; // ��ֹ�����������ȫ����ʼ��һ�κ���ָ�롣
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_EZT570I::SendReadCmd);   //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_EZT570I::SendWriteCmd);  //  1-M)�����¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_EZT570I::SendReadCmd);    //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_EZT570I::SendRunCmd);      //  3-M)Ҫ�����������������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_EZT570I::SendStopCmd);    //  4-M)Ҫ��ֹͣ�����������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_EZT570I::ParseResult);  //  5-S)��ȡ�¶ȴӻ��ظ���������  (���ֽڲ���)
}

/*----*/
unsigned char TH_EZT570I::SetPresentValue_Check(unsigned char ChanNo, short int Value)
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

void TH_EZT570I::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
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

    uw16_Total =  UARTFunc::uart_bytenum(ChanNo);
//     uw16_Total =  TH_EZT570I::ParseResult_FirstCheck(ChanNo);

    if( uw16_Total < TH_EZT570I_MIN_RESPONSE_CMD_LEN)
        return;  // �����������ϴ���һ����Ϣ�����ޱ���

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_i = 0;
    do {
#ifdef COM_CMD_CHANNEL_DIFFER
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == (ChanNo+1) && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_EZT570I[1] && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_EZT570I[2])
#else
        if(gR_CmdBuf[uw16_i] == uc_COM_ReadReply_EZT570I[0] && gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_EZT570I[1] && gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_EZT570I[2])
#endif
        {   // ���ð�ͷ���β�ж�����ֵ��
// 	       if(gW_ReadRepeatNum[ChanNo] < TC_EZT570I_MIN_RESPONSE_CMD_NUM)
// 			   return;	// �����¶Ȼ�ʪ�Ƚ����������ʱ,���´���ֵ��

            PresentValue = ((short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+3]) << 8 | (short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+4]) );//  �¶�
            if( TH_EZT570I::SetPresentValue_Check(ChanNo, PresentValue) )
            {
                //gW_ReadCmd_Flag[ChanNo] = SETPOINTVALUE_TEMPERATURE;
							
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue, uc_Temp4Humi_Flag, FIRST_CHAMBER_CHANNEL);
            }

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // ���½��ճ�ʱʱ��

            return;
        }
        uw16_i++;
    } while(uw16_i < uw16_Total);

}

void TH_EZT570I::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����д����ֵ����
    short int  register_data;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_EZT570I_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
    {
        for(MbChkSum = 0; MbChkSum < (uw16_len-4); MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_WriteHumiCmd_EZT570I[MbChkSum];
        register_data = MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL];
    }
    else if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
    {
        for(MbChkSum = 0; MbChkSum < (uw16_len-4); MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_WriteTempCmd_EZT570I[MbChkSum];
        register_data = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
    }
    else
        return;


#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
    }
#endif

    MbBuf[TH_EZT570I_MAX_COM_WRITE_CMD-4] = (unsigned char) (register_data >> 8);
    MbBuf[TH_EZT570I_MAX_COM_WRITE_CMD-3] = (unsigned char) (register_data & 255);
    MbChkSum = MTCI_datatypes::CRC16(MbBuf,uw16_len-2);
    MbBuf[TH_EZT570I_MAX_COM_WRITE_CMD-2] = (unsigned char)(MbChkSum >> 8);
    MbBuf[TH_EZT570I_MAX_COM_WRITE_CMD-1] = (unsigned char)(MbChkSum & 255);
    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);  // ���Ͷ�������

    //gW_WriteCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
    //gW_WriteCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// �¡�ʪ����Ϊ��ָͬ��ʱ�������־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}


void TH_EZT570I::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ������������
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_EZT570I_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��


    if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
    {
        for(MbChkSum = 0; MbChkSum < TH_EZT570I_MAX_COM_READ_CMD; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_ReadHumiCmd_EZT570I[MbChkSum];
    }
    else
    {
        for(MbChkSum = 0; MbChkSum < TH_EZT570I_MAX_COM_READ_CMD; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_ReadTempCmd_EZT570I[MbChkSum];
    }
    uw16_len = TH_EZT570I_MAX_COM_READ_CMD;


#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
        MbChkSum = MTCI_datatypes::CRC16(MbBuf,uw16_len-2);
        MbBuf[TH_EZT570I_MAX_COM_READ_CMD-2] = (unsigned char)(MbChkSum >> 8);
        MbBuf[TH_EZT570I_MAX_COM_READ_CMD-1] = (unsigned char)(MbChkSum & 255);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
    //gW_RunCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
    //gW_RunCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// �¡�ʪ����Ϊ��ָͬ��ʱ�������־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}

void TH_EZT570I::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ���Ͷ�����
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_EZT570I_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    //for(MbChkSum = 0;MbChkSum < (MAX_COM_READ_CMD-2);MbChkSum++)
    for(MbChkSum = 0; MbChkSum < TH_EZT570I_MAX_COM_READ_CMD; MbChkSum++)
    {
#ifdef COM_TEMP4HUMI_CMD
        if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
        {
            MbBuf[MbChkSum] = uc_COM_ReadHumiCmd_EZT570I[MbChkSum];
        }
        else
        {
            MbBuf[MbChkSum] = uc_COM_ReadTempCmd_EZT570I[MbChkSum];
        }
#else
        MbBuf[MbChkSum] = uc_COM_ReadTempCmd_EZT570I[MbChkSum];
#endif
    }
    //MbChkSum = CRC16(MbBuf,MAX_COM_READ_CMD-2);
    //MbBuf[MAX_COM_READ_CMD-2] = (unsigned char) (MbChkSum >>8);
    //MbBuf[MAX_COM_READ_CMD-1] = (unsigned char) (MbChkSum & 255);

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
        MbChkSum = MTCI_datatypes::CRC16(MbBuf,uw16_len-2);
        MbBuf[TH_EZT570I_MAX_COM_READ_CMD-2] = (unsigned char)(MbChkSum >> 8);
        MbBuf[TH_EZT570I_MAX_COM_READ_CMD-1] = (unsigned char)(MbChkSum & 255);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //gW_ReadCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
    //gW_ReadCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// �¡�ʪ����Ϊ��ָͬ��ʱ�������־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}

void TH_EZT570I::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����ֹͣ����
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_EZT570I_MAX_COM_STOP_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��


    if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
    {
        for(MbChkSum = 0; MbChkSum < TH_EZT570I_MAX_COM_WRITE_CMD; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_WriteHumiCmd_EZT570I[MbChkSum];
    }
    else
    {
        for(MbChkSum = 0; MbChkSum < TH_EZT570I_MAX_COM_WRITE_CMD; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_WriteTempCmd_EZT570I[MbChkSum];
    }
    uw16_len = TH_EZT570I_MAX_COM_WRITE_CMD;


#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
        MbChkSum = MTCI_datatypes::CRC16(MbBuf,uw16_len-2);
        MbBuf[TH_EZT570I_MAX_COM_WRITE_CMD-2] = (unsigned char)(MbChkSum >> 8);
        MbBuf[TH_EZT570I_MAX_COM_WRITE_CMD-1] = (unsigned char)(MbChkSum & 255);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //gW_StopCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
    //gW_StopCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// �¡�ʪ����Ϊ��ָͬ��ʱ�������־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}


