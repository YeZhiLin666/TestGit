/*****************************************************************************
// �����ļ�      :TC_FUTURE_FDCP91.c    �����¿����и��ִ����ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"



extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function ����ָ��   //��ʽ  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];



//------------------------------------------------------------------------------------------------
//Future Design Controls P91 T-CONTROLLER�¿�������( CSZ M1.2 Chamber)
//����ͨ�ŷ�ʽ  MODBUS  ���䷽ʽ��RTUģʽ CRCУ��
//

//  1-M)�����¶��������ӻ� (����Word����)
//  CMD: {[controller address (1 byte)] [write command (0x06)] [register high byte] [register low byte] [data high byte] [data low byte] [CRC low byte] [CRC high byte]}
//  MODEL: 01 06 00 3C XX XX YY YY    (XX����ֵ�ȴ���λ�ٴ���λ; YYУ��ֵ�ȴ���λ�ٴ���λ)
const unsigned char uc_COM_WriteCmd_FDCP91[TC_FDCP91_MAX_COM_WRITE_CMD] = {
    0x01, 0x06, 0x00, 0x00, 0x00, 0xDC, 0x88, 0x53
};     // write Master to  Slave cmd  (�����¶�22�ȣ�����220)

//  �ӻ��ظ�ʱ��Replyһģһ����
//  Reply CMD: {[controller address (1 byte)] [write command (0x06)] [register high byte] [register low byte] [data high byte] [data low byte] [CRC low byte] [CRC high byte]}
//  Reply MODEL: 01 06 00 3C XX XX YY YY   (XX����ֵ�ȴ���λ�ٴ���λ; YYУ��ֵ�ȴ���λ�ٴ���λ)
const unsigned char uc_COM_WriteReply_FDCP91[TC_FDCP91_MAX_COM_WRITE_CMD] = {
    0x01, 0x06, 0x00, 0x00, 0x00, 0xDC, 0x88, 0x53
};     // write Slave to  Master reply (�ظ������¶�22�ȣ�����220)

//  2-M)��ȡ�¶��������ӻ�  (����Word����)
//  CMD: {[controller address (1 byte)] [read command (0x03)] [starting register high byte] [starting register low byte] [number of registers high byte (0x00)] [number of registers low byte] [CRC low byte] [CRC high byte]}
//  MODEL: 01 03 00 3D 00 01 YY YY   (YYУ��ֵ�ȴ���λ�ٴ���λ)
const unsigned char uc_COM_ReadCmd_FDCP91[TC_FDCP91_MAX_COM_READ_CMD] = {
    0x01, 0x03, 0x00, 0x80, 0x00, 0x01, 0x85, 0xE2
};    // read Master to  Slave cmd (��ȡ1����)

//  �ӻ��ظ�ʱ��Replyʹ��full������ʽ����ʹ��integer������ʽ��
//  Reply CMD:  {[controller address (1 byte)] [read command (0x03)] [number of bytes (1 byte)] [first register data high byte] [first register data low byte] [CRC low byte] [CRC high byte]}
//  Reply MODEL: 01 03 02 XX XX YY YY   (XX�¶�ֵ�ȴ���λ�ٴ���λ; YYУ��ֵ�ȴ���λ�ٴ���λ)
//  ʹ��full������ʽʱ����ʾֵ=�¶�ֵ/10;    �磺60(600/10)��,22(220/10)��
const unsigned char uc_COM_ReadReply_FDCP91[TC_FDCP91_MAX_COM_READ_REPLY] = {
    0x01, 0x03, 0x02, 0x00, 0xDC, 0xB9, 0xDD
};     // read Slave to  Master reply  //(�ظ��¶�22�ȣ�����220)


//  3-M)Ҫ�����������������ӻ�
//  (��ȡ�¶�)
//01 03 00 80 00 01 YY YY   (YYУ��ֵ�ȴ���λ�ٴ���λ)
//(��ȡ�¶�)01 03 00 80 00 01 85 E2


const unsigned char uc_COM_RunCmd_FDCP91[TC_FDCP91_MAX_COM_RUN_CMD] = {
    0x01, 0x03, 0x00, 0x80, 0x00, 0x01, 0x85, 0xE2
};    // read Master to  Slave cmd (��ȡ)

//  4-M)Ҫ��ֹͣ�����������ӻ�����
//  (�����¶�22�ȣ�����220)
//{[controller address (1 byte)] [write command (0x06)] [register high byte] [register low byte] [data high byte] [data low byte] [CRC low byte] [CRC high byte]}
//01 06 00 00 00 DC YY YY   (XX����ֵ�ȴ���λ�ٴ���λ; YYУ��ֵ�ȴ���λ�ٴ���λ)

const unsigned char uc_COM_StopCmd_FDCP91[TC_FDCP91_MAX_COM_STOP_CMD] = {
    0x01, 0x06, 0x00, 0x00, 0x00, 0xDC, 0x88, 0x53
};    // write Master to  Slave cmd  (�����¶�22��)

//
// EZT570I  ����ͨ�ŷ�ʽ  MODBUS  ���䷽ʽ��RTUģʽ CRCУ�� //
//----------------------------------------------------------------------------------------------------

void TC_FDCP91::Init(unsigned char ChanNo)
{
// ��ʼ������ָ�롣
    unsigned char uc_i; // ��ֹ�����������ȫ����ʼ��һ�κ���ָ�롣
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_FDCP91::SendReadCmd);   //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_FDCP91::SendWriteCmd);  //  1-M)�����¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_FDCP91::SendReadCmd);    //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_FDCP91::SendRunCmd);      //  3-M)Ҫ�����������������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_FDCP91::SendStopCmd);    //  4-M)Ҫ��ֹͣ�����������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_FDCP91::ParseResult);  //  5-S)��ȡ�¶ȴӻ��ظ���������  (���ֽڲ���)
}

void TC_FDCP91::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// �������յĽ������
    /*unsigned char *pCmd;
    unsigned char *p;
    unsigned char uc_MoveFlag = 0;
    //bool b_is_temp;
    //bool NoZero;

    unsigned short int uw16_i;
    unsigned short int uw16_j;
    unsigned short int uw16_k;
    unsigned char uc_Buffer[4][6];
    unsigned long int uw32_DO = 0;*/
    short int PresentValue = 0;
    unsigned short int uw16_i = 0;

    if( UARTFunc::uart_bytenum(ChanNo) < TC_FDCP91_MIN_RESPONSE_CMD_LEN)
        return;  // �����������ϴ���һ����Ϣ�����ޱ���

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
		UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);


    do {
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == uc_COM_ReadReply_FDCP91[0] && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_FDCP91[1]
                && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_FDCP91[2])  // && gR_CmdBuf[TC_FDCP91_MAX_COM_READ_REPLY] == 0x00)
        {
            PresentValue = ((short int)(uw16_i+MTCI_CTRL::gR_CmdBuf[TC_FDCP91_READ_REPLY_POINT_VAlUE]) << 8 | (short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+TC_FDCP91_READ_REPLY_POINT_VAlUE+1]) );

            MTCI_CTRL::SetPresentValue(ChanNo, PresentValue,SETPOINTVALUE_TEMPERATURE,FIRST_CHAMBER_CHANNEL);

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);
            return;
        }
        //     else
        // 	{
        //        return;
        //     }

        uw16_i++;
    } while(uw16_i < TC_FDCP91_MIN_RESPONSE_CMD_LEN);
}


void TC_FDCP91::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����д����
    short int  register_data;
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];

    for(MbChkSum = 0; MbChkSum < TC_FDCP91_MAX_COM_WRITE_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteCmd_FDCP91[MbChkSum];
    //MbBuf[TC_FDCP91_MAX_COM_WRITE_CMD-1] = 0X0D;  // ���������־<CR>
    register_data = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
    MbBuf[TC_FDCP91_WRITE_CMD_POINT_VAlUE] = (unsigned char) (register_data >> 8);
    MbBuf[TC_FDCP91_WRITE_CMD_POINT_VAlUE+1] = (unsigned char) (register_data & 255);
#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[1] = (ChanNo + FIRST_UART_NUM);
#endif

    MbChkSum = MTCI_datatypes::CRC16(MbBuf,TC_FDCP91_MAX_COM_WRITE_CMD-2);
    MbBuf[TC_FDCP91_MAX_COM_WRITE_CMD-2] = (unsigned char)(MbChkSum >> 8);
    MbBuf[TC_FDCP91_MAX_COM_WRITE_CMD-1] = (unsigned char)(MbChkSum & 255);

    //MbBuf[TC_FDCP91_MAX_COM_WRITE_CMD-1] = MTCI_datatypes::CalcCheckSum_Byte(&MbBuf[1],MAX_COM_WRITE_CMD-2);
    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_FDCP91_MAX_COM_WRITE_CMD);

    //MTCI_CTRL::gW_WriteCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;   // ���¿��Ʊ���ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}


void TC_FDCP91::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ������������
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];
    for(MbChkSum = 0; MbChkSum < TC_FDCP91_MAX_COM_RUN_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_RunCmd_FDCP91[MbChkSum];
    //MbBuf[MAX_COM_RUN_CMD-1] = 0X0D;  // ���������־<CR>
    //uart_putcmd(ChanNo,"\x01\x03\x00\x64\x00\x01\xC5\xD5");
    //uart_putcmd(ChanNo,uc_COM_ReadCmd,MAX_COM_READ_CMD); // ʵ���ϸ����Ͷ�����һ���Ƕ�ȡ�¶�����

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[1] = (ChanNo + FIRST_UART_NUM);

        MbChkSum = MTCI_datatypes::CRC16(MbBuf,TC_FDCP91_MAX_COM_WRITE_CMD-2);
        MbBuf[TC_FDCP91_MAX_COM_RUN_CMD-2] = (unsigned char)(MbChkSum >> 8);
        MbBuf[TC_FDCP91_MAX_COM_RUN_CMD-1] = (unsigned char)(MbChkSum & 255);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_FDCP91_MAX_COM_RUN_CMD);

    //MTCI_CTRL::gW_RunCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;  // ���¿��Ʊ���ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}


void TC_FDCP91::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ���Ͷ�����
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];

    for(MbChkSum = 0; MbChkSum < TC_FDCP91_MAX_COM_READ_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_ReadCmd_FDCP91[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[1] = (ChanNo + FIRST_UART_NUM);
    }
#endif

    MbChkSum = MTCI_datatypes::CRC16(MbBuf,TC_FDCP91_MAX_COM_READ_CMD-2);
    MbBuf[TC_FDCP91_MAX_COM_READ_CMD-2] = (unsigned char) (MbChkSum >>8);
    MbBuf[TC_FDCP91_MAX_COM_READ_CMD-1] = (unsigned char) (MbChkSum & 255);
    //MbBuf[MAX_COM_READ_CMD-1] = MTCI_datatypes::CalcCheckSum_Byte(&MbBuf[1],MAX_COM_READ_CMD-2);
    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_FDCP91_MAX_COM_READ_CMD);

    //MTCI_CTRL::gW_ReadCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// ���¿��Ʊ���ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}

void TC_FDCP91::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����ֹͣ����
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];

    for(MbChkSum = 0; MbChkSum < TC_FDCP91_MAX_COM_STOP_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_FDCP91[MbChkSum];
    //MbBuf[MAX_COM_STOP_CMD-1] = 0X0D;  // ���������־<CR>

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[1] = (ChanNo + FIRST_UART_NUM);
    }
#endif

    MbChkSum = MTCI_datatypes::CRC16(MbBuf,TC_FDCP91_MAX_COM_STOP_CMD-2);
    MbBuf[TC_FDCP91_MAX_COM_STOP_CMD-2] = (unsigned char) (MbChkSum >> 8);
    MbBuf[TC_FDCP91_MAX_COM_STOP_CMD-1] = (unsigned char) (MbChkSum & 255);

    //if(gR_Updata_Flag[ChanNo] == FALSE)
    //	return;

    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_FDCP91_MAX_COM_STOP_CMD);

    //MTCI_CTRL::gW_StopCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// ���¿��Ʊ���ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}




