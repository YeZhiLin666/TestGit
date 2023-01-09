/*****************************************************************************
// �����ļ�      :TH_BINDER_MB1.C    �����¿����и��ִ����ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"





extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function ����ָ��   //��ʽ  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];


//------------------------------------------------------------------------------------------------
// MB1  ����ͨ�ŷ�ʽ  MODBUS  ���䷽ʽ��RTUģʽ CRCУ�� //
//

// Ĭ�ϳ���ʱ�¶�ֵΪ22���
// Ĭ�ϳ�̬ʱʪ��ֵΪ55%RH

//  1-M)�����¶��������ӻ� (����Word����)
//  CMD: {[controller address (1 byte)] [write command (0x10)] [Address of Word(2 bytes)][Number of Words(2 bytes)] [Number of bytes(1 byte)][Word value (x byte)] [CRC low byte] [CRC high byte]}
//  MODEL: 01 10 15 81 00 02 04 XX XX XX XX YY YY    (XX����ֵ�ȴ���λ�ٴ���λ; YYУ��ֵ�ȴ���λ�ٴ���λ)
const unsigned char uc_COM_WriteTempCmd_MB1[TC_MB1_MAX_COM_WRITE_CMD] = {
    0x01, 0x10, 0x15, 0x81, 0x00, 0x02, 0x04, 0x00, 0x00, 0x41, 0xB0, 0xF9, 0x77
};     // write Master to  Slave cmd  (�����¶�22�� float��)

//  �ӻ��ظ�ʱ��
//  Reply CMD: {[controller address (1 byte)] [write command (0x10)] [Address of Word(2 bytes)] [Number of Word(2 bytes)][CRC low byte] [CRC high byte]}
//  Reply MODEL: 01 10 15 81 00 02 YY YY   (YYУ��ֵ�ȴ���λ�ٴ���λ)
const unsigned char uc_COM_WriteReply_MB1[TC_MB1_MAX_COM_WRITE_CMD] = {
    0x01, 0x10, 0x15, 0x81, 0x00, 0x02, 0x15, 0xEC
};     // write Slave to  Master reply ((YYУ��ֵ�ȴ���λ�ٴ���λ))


//  2-M)��ȡ�¶��������ӻ�  (����Word����)
//  CMD: {[controller address (1 byte)] [read command (0x03)] [Address first word (2 byte)] [Number of words (2 byte)][CRC low byte] [CRC high byte]}
//  MODEL: 01 03 11 A9 00 02 YY YY   (YYУ��ֵ�ȴ���λ�ٴ���λ)(��ȡ�¶�)
const unsigned char uc_COM_ReadTempCmd_MB1[TC_MB1_MAX_COM_READ_CMD] = {
    0x01, 0x03, 0x11, 0xA9, 0x00, 0x02, 0x11, 0x17
};    // read Master to  Slave cmd (��ȡ1��float�¶�ֵ)


//  �ӻ��ظ�ʱ��Replyʹ��float��ʽ��
//  Reply CMD:  {[controller address (1 byte)] [read command (0x03)] [number of bytes (1 byte)] [Word values (x byte)][CRC low byte] [CRC high byte]}
//  Reply MODEL: 01 03 04 XX XX XX XX YY YY   (XX�¶�ֵ�ȴ���λ�ٴ���λ; YYУ��ֵ�ȴ���λ�ٴ���λ)
const unsigned char uc_COM_ReadReply_MB1[TC_MB1_MAX_COM_READ_REPLY] = {
    0x01, 0x03, 0x04, 0x00, 0x00, 0x41, 0xB0, 0xCB, 0xD7
};     // read Slave to  Master reply  //(�ظ��¶�22.0�ȣ�float��)


//  3-M)Ҫ�����������������ӻ�
//  CMD: {[controller address (1 byte)] [read command (0x03)] [Address first word (2 byte)] [Number of words (2 byte)][CRC low byte] [CRC high byte]}
//  MODEL: 01 03 11 A9 00 02 YY YY   (YYУ��ֵ�ȴ���λ�ٴ���λ)(��ȡ�¶�)
//  �ӻ��ظ�ʱ��Replyʹ��float��ʽ��
//  Reply CMD:  {[controller address (1 byte)] [read command (0x03)] [number of bytes (1 byte)] [Word values (x byte)][CRC low byte] [CRC high byte]}
//  Reply MODEL: 01 03 04 XX XX XX XX YY YY   (XX�¶�ֵ�ȴ���λWORD�ٴ���λWORD; YYУ��ֵ�ȴ���λ�ٴ���λ)
//  ʹ��float��
const unsigned char uc_COM_RunCmd_MB1[TC_MB1_MAX_COM_RUN_CMD] =  {
    0x01, 0x03, 0x11, 0xA9, 0x00, 0x02, 0x11, 0x17
};    // read Master to  Slave cmd (��ȡ1��float��ֵ)


//  4-M)Ҫ��ֹͣ�����������ӻ�����
//  CMD: {[controller address (1 byte)] [write command (0x10)] [Address of Word(2 bytes)] [Number of Word(2 byte)] [Number of bytes(1 byte)][Word value (x byte)][CRC low byte] [CRC high byte]}
//  MODEL: 01 10 15 81 00 02 04 XX XX XX XX YY YY    (XX����ֵ�ȴ���λ�ٴ���λ; YYУ��ֵ�ȴ���λ�ٴ���λ)
//  �ӻ��ظ�ʱ��Replyһģһ����
//  Reply CMD: {[controller address (1 byte)] [write command (0x06)] [register high byte] [register low byte] [data high byte] [data low byte] [CRC low byte] [CRC high byte]}
//  Reply MODEL: 01 10 15 81 00 02 YY YY   (XX����ֵ�ȴ���λ�ٴ���λ; YYУ��ֵ�ȴ���λ�ٴ���λ)

const unsigned char uc_COM_StopCmd_MB1[TC_MB1_MAX_COM_STOP_CMD] =  {
    0x01, 0x10, 0x15, 0x81, 0x00, 0x02, 0x04, 0x00, 0x00, 0x41, 0xB0, 0xF9, 0x77
};     // write Master to  Slave cmd  (�����¶�22.0��)

//
// MB1  ����ͨ�ŷ�ʽ  MODBUS  ���䷽ʽ��RTUģʽ CRCУ�� //
//----------------------------------------------------------------------------------------------------
void TC_MB1::Init(unsigned char ChanNo)
{
// ��ʼ������ָ�롣
    unsigned char uc_i; // ��ֹ�����������ȫ����ʼ��һ�κ���ָ�롣
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_MB1::SendReadCmd);   //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_MB1::SendWriteCmd);  //  1-M)�����¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_MB1::SendReadCmd);    //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_MB1::SendRunCmd);      //  3-M)Ҫ�����������������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_MB1::SendStopCmd);    //  4-M)Ҫ��ֹͣ�����������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_MB1::ParseResult);  //  5-S)��ȡ�¶ȴӻ��ظ���������  (���ֽڲ���)

}

/*----*/
unsigned char TC_MB1::SetPresentValue_Check(unsigned char ChanNo, short int Value)
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

void TC_MB1::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
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

    if( uw16_Total < TC_MB1_MIN_RESPONSE_CMD_LEN)
        return;  // �����������ϴ���һ����Ϣ�����ޱ���

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_i = 0;
    do {
#ifdef COM_CMD_CHANNEL_DIFFER
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == (ChanNo+1) && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_MB1[1] && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_MB1[2])
#else
        if(gR_CmdBuf[uw16_i] == uc_COM_ReadReply_MB1[0] && gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_MB1[1] && gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_MB1[2])
#endif
        {   // ���ð�ͷ���β�ж�����ֵ��
// 	       if(gW_ReadRepeatNum[ChanNo] < TC_MB1_MIN_RESPONSE_CMD_NUM)
// 			   return;	// �����¶Ȼ�ʪ�Ƚ����������ʱ,���´���ֵ��
            unsigned char Tmp[4];
            memset(Tmp, 0, 4);
            Tmp[3] = MTCI_CTRL::gR_CmdBuf[uw16_i+ 5];
            Tmp[2] = MTCI_CTRL::gR_CmdBuf[uw16_i+ 6];
            Tmp[1] = MTCI_CTRL::gR_CmdBuf[uw16_i+ 3];
            Tmp[0] = MTCI_CTRL::gR_CmdBuf[uw16_i+ 4];
            PresentValue = MTCI_datatypes::Convert_FloatToInt(Tmp);

            if( TC_MB1::SetPresentValue_Check(ChanNo, PresentValue) ) //���⽻�����0ֵ
            {
                //gW_ReadCmd_Flag[ChanNo] = SETPOINTVALUE_TEMPERATURE;
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue, SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL); //uc_Temp4Humi_Flag
            }

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // ���½��ճ�ʱʱ��

            return;
        }
        uw16_i++;
    } while(uw16_i < uw16_Total);

}

void TC_MB1::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����д����ֵ����

    short int  register_data;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TC_MB1_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

//         if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
//     	{
// 	    	for(MbChkSum = 0;MbChkSum < (uw16_len-6);MbChkSum++)
// 				MbBuf[MbChkSum] = uc_COM_WriteHumiCmd_MB1[MbChkSum]; //��ȡдHum��ͷ�����̶��ֽ�
// 	        register_data = MTCI_CTRL::gW_HumiValue[ChanNo];       //��ȡдHum��ֵ
//     	}
// 		else if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
// 		{
    for(MbChkSum = 0; MbChkSum < (uw16_len-6); MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteTempCmd_MB1[MbChkSum];
    register_data = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
// 		}
// 		else
// 			return;


#ifdef COM_CMD_CHANNEL_DIFFER //ѡ���͸��ĸ��¿���
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
    }
#endif

    unsigned char tmp[4];
    memset(tmp,0,4);
    MTCI_datatypes::Convert_IntToFloat(tmp,register_data);
    MbBuf[TC_MB1_MAX_COM_WRITE_CMD-3] = tmp[2];
    MbBuf[TC_MB1_MAX_COM_WRITE_CMD-4] = tmp[3];
    MbBuf[TC_MB1_MAX_COM_WRITE_CMD-5] = tmp[0];
    MbBuf[TC_MB1_MAX_COM_WRITE_CMD-6] = tmp[1];

    //MbBuf[TC_MB1_MAX_COM_WRITE_CMD-4] = (unsigned char) (register_data >> 8);  //data high bytes
    //MbBuf[TC_MB1_MAX_COM_WRITE_CMD-3] = (unsigned char) (register_data & 255);  //data low bytes
    MbChkSum = MTCI_datatypes::CRC16(MbBuf,uw16_len-2);  //CRC���㺯�����Ѿ��Զ�����ߵ�λת����
    MbBuf[TC_MB1_MAX_COM_WRITE_CMD-2] = (unsigned char)(MbChkSum >> 8);
    MbBuf[TC_MB1_MAX_COM_WRITE_CMD-1] = (unsigned char)(MbChkSum & 255);
    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);  // ���Ͷ�������

    //gW_WriteCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
    //gW_WriteCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// �¡�ʪ����Ϊ��ָͬ��ʱ�������־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}


void TC_MB1::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ������������
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TC_MB1_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��


//         if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
//         {
//             for(MbChkSum = 0;MbChkSum < TC_MB1_MAX_COM_READ_CMD;MbChkSum++)
//                 MbBuf[MbChkSum] = uc_COM_ReadHumiCmd_MB1[MbChkSum];
//         }
//         else
//         {
    for(MbChkSum = 0; MbChkSum < TC_MB1_MAX_COM_READ_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_ReadTempCmd_MB1[MbChkSum];
//         }
    uw16_len = TC_MB1_MAX_COM_READ_CMD;


#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
        MbChkSum = MTCI_datatypes::CRC16(MbBuf,uw16_len-2);
        MbBuf[TC_MB1_MAX_COM_READ_CMD-2] = (unsigned char)(MbChkSum >> 8);
        MbBuf[TC_MB1_MAX_COM_READ_CMD-1] = (unsigned char)(MbChkSum & 255);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}
void TC_MB1::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TC_MB1_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];
    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��
    for(MbChkSum = 0; MbChkSum < TC_MB1_MAX_COM_READ_CMD; MbChkSum++)
    {

        MbBuf[MbChkSum] = uc_COM_ReadTempCmd_MB1[MbChkSum];
    }
#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
        MbChkSum = MTCI_datatypes::CRC16(MbBuf,uw16_len-2);
        MbBuf[TC_MB1_MAX_COM_READ_CMD-2] = (unsigned char)(MbChkSum >> 8);
        MbBuf[TC_MB1_MAX_COM_READ_CMD-1] = (unsigned char)(MbChkSum & 255);
    }
#endif
    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
    //gW_ReadCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
    //gW_ReadCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// �¡�ʪ����Ϊ��ָͬ��ʱ�������־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}

void TC_MB1::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����ֹͣ����

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TC_MB1_MAX_COM_STOP_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��


//     if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
//     {
//         for(MbChkSum = 0;MbChkSum < TC_MB1_MAX_COM_WRITE_CMD;MbChkSum++)
//             MbBuf[MbChkSum] = uc_COM_WriteHumiCmd_MB1[MbChkSum];
//     }
//     else
//     {
    for(MbChkSum = 0; MbChkSum < TC_MB1_MAX_COM_WRITE_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteTempCmd_MB1[MbChkSum];
//     }
    uw16_len = TC_MB1_MAX_COM_WRITE_CMD;


#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[0] = (ChanNo + FIRST_UART_NUM);
        MbChkSum = MTCI_datatypes::CRC16(MbBuf,uw16_len-2);
        MbBuf[TC_MB1_MAX_COM_WRITE_CMD-2] = (unsigned char)(MbChkSum >> 8);
        MbBuf[TC_MB1_MAX_COM_WRITE_CMD-1] = (unsigned char)(MbChkSum & 255);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //gW_StopCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
    //gW_StopCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// �¡�ʪ����Ϊ��ָͬ��ʱ�������־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}



