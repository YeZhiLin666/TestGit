/*****************************************************************************
// �����ļ�      :TC_THERMOTRON_8200.c    �����¿����и��ִ����ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"


extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function ����ָ��   //��ʽ  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

//------------------------------------------------------------------------------------------------
//  Thermotron&Industries(�����Ȳ��豸��ҵ��˾)8200 controller //
//  ����ͨ�ŷ�ʽ  MODBUS  ���䷽ʽ��ASCIIģʽ ��У��
//  �ر�ע�⣺��ʹ��ֹͣ���������������д���һ��ʱ�����Ʊ����޸ġ�

//  Multidrop address communication protocol (RS-485)
//  The host computer sends:
//  [FF]#aCMND<t> or [FF]#aCMND<data><t>
//  The 8200 responds with:
//  [FF]#a<data><t>
//  Where:
//  [FF] = prefix character (FF hexadecimal)
//  # = ASCII pound sign (35 decimal  0X23)
//  a = One-byte address of the 8200 (1...127)
//  CMND = command mnemonic
//  <data> = data or character string sent with command mnemonic
//  <t> = computer terminator, for example a carriage return <cr>

//  1-M)�����¶��������ӻ� (���ֽڲ���)
//  Syntax: SETPn? or SETPn,data
//  Operation example: SETP1,22.000
//  8200 response: 22.000      (22����ֵ)

//  ASCIIת16��������  ��53 45 54 50 31 2C 32 32 2E 30 30 30 0D
//  ASCIIת16���ƻظ�  ��32 32 2E 30 30 30 0D

//  Query example: SETP1?
//  8200 response: 22.000      (22����ֵ)

//   ASCIIת16�������� ��53 45 54 50 31 3F 0D
//   ASCIIת16���ƻظ� ��32 32 2E 30 30 30 0D
const unsigned char uc_COM_WriteCmd_8200[TC_8200_MAX_COM_WRITE_CMD] = {
    0x53, 0x45, 0x54, 0x50, 0x31, 0x2C, 0x32, 0x32, 0x2E, 0x30, 0x30, 0x30, 0X0D
};     // write Master to  Slave cmd  (�����¶�22��)
//0x53, 0x45, 0x54, 0x50, 0x31, 0x2C, 0x32, 0x32, 0x2E, 0x30};     // write Master to  Slave cmd  (�����¶�22.0��)
//  �ӻ��ظ�ʱ��Replyһģһ������ֵ��
const unsigned char uc_COM_WriteReply_8200[TC_8200_MAX_COM_WRITE_REPLY] = {
    0x32, 0x32, 0x2E, 0x30, 0x30, 0x30, 0X0D
};     // write Slave to  Master reply (�ظ������¶�22.0��)

//  2-M)��ȡ�¶��������ӻ�  (����Word����)
//  Syntax: PVARn?
//  Where n is the channel number (1 to 48). See Description.
//  1 through 4 External process variable channels 1 through 4
//  5 through 8 Internal process variable channels 5 through 8
//  9 through 12 Undefined
//  13 through 28 Monitor channels 1 through 16
//  29 through 32 Undefined
//  33 through 36 System Monitor temperature channels for refrigeration system 1
//  (high-stage suction, high-stage discharge, low-stage suction,
//  and low-stage discharge)
//  37 through 48 System Monitor channels for refrigeration systems 2, 3, and 4

//  Query example: PVAR1?
//  8200 response: 22.000

//  ASCIIת16�������� ��50 56 41 52 31 3F 0D
//  ASCIIת16���ƻظ� ��32 32 2E 30 30 30 0D

const unsigned char uc_COM_ReadCmd_8200[TC_8200_MAX_COM_READ_CMD] = {
    0x50, 0x56, 0x41, 0x52, 0x31, 0x3F, 0X0D
};    // read Master to  Slave cmd (��ȡ)

//  �ӻ��ظ�ʱ��Reply
const unsigned char uc_COM_ReadReply_8200[TC_8200_MAX_COM_READ_REPLY] = {
    0x32, 0x32, 0x2E, 0x30, 0x30, 0x30, 0X0D
};     // read Slave to  Master reply  //(�ظ��¶�22�ȣ�����220)

//  3-M)Ҫ�����������������ӻ�
//  Syntax: RUNM
//  8200 response: Places a stopped 8200 in run manual mode.

//  ASCIIת16�������� ��52 55 4E 4D 0D
//  ASCIIת16���ƻظ� ��

const unsigned char uc_COM_RunCmd_8200[TC_8200_MAX_COM_RUN_CMD] = {
    0x52, 0x55, 0x4E, 0x4D, 0X0D
};    // read Master to  Slave cmd

//  4-M)Ҫ��ֹͣ�����������ӻ�����
//  Syntax: STOP
//  8200 response: The 8200 goes into stop mode.

//  ASCIIת16�������� ��53 54 4F 50 0D
//  ASCIIת16���ƻظ� ��

const unsigned char uc_COM_StopCmd_8200[TC_8200_MAX_COM_STOP_CMD] = {
    0x53, 0x54, 0x4F, 0x50, 0X0D
};    // read Master to  Slave cmd


//
//  EZT570I  ����ͨ�ŷ�ʽ  MODBUS  ���䷽ʽ��RTUģʽ CRCУ�� //
//----------------------------------------------------------------------------------------------------

void TC_8200::Init(unsigned char ChanNo)
{
// ��ʼ������ָ�롣
    unsigned char uc_i; // ��ֹ�����������ȫ����ʼ��һ�κ���ָ�롣
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_8200::SendReadCmd);   //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_8200::SendWriteCmd);  //  1-M)�����¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_8200::SendReadCmd);    //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_8200::SendRunCmd);      //  3-M)Ҫ�����������������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_8200::SendStopCmd);    //  4-M)Ҫ��ֹͣ�����������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_8200::ParseResult);  //  5-S)��ȡ�¶ȴӻ��ظ���������  (���ֽڲ���)
}

void TC_8200::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
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


    if( UARTFunc::uart_bytenum(ChanNo) < TC_8200_MIN_RESPONSE_CMD_LEN)
        return;  // �����������ϴ���һ����Ϣ�����ޱ���

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
		UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    pCmd = MTCI_CTRL::gR_CmdBuf; //DATA FROM CHAMBER

    uw16_j = 0;
    uw16_k = 0;
    uc_MoveFlag = 0;
    for(uw16_i = 0; uw16_i < 2*TC_8200_MAX_COM_READ_REPLY; uw16_i ++)
    {   //  ��������400�ȸ�200�ȡ�  // Temperature range -200��C to +400��C (-328��F to +725��F) //Resolution 0.1��C or ��F, 0.1% RH, or 0.01 for other linear applications
        if((MTCI_CTRL::gR_CmdBuf[uw16_i]) == '.')  // Ѱ��С���㣬���˶����λ����['CR'Ϊ0X0D]
        {
            if(uw16_i < 1) // ����һ��ʼ���ҵ�ΪС��������,�����ݲ����� ��[.7'CR'5.7'CR'5.7'CR']
                continue;

            if(uw16_j == 0)    // �磺 [-22.2'CR'-22.2'CR'-22.2'CR']   (j=3,k=9;����-22: k-j-3=3����3������Ϊ��ȥ����һ����ЧС���͵�һ�Ļ��б�־'CR'���Լ��ڶ���С����)
            {
                uw16_j = uw16_i;  //  ��һ�β鵽С���㡣   ������ֵ��С�����жϡ�
                uc_MoveFlag = 1;
            }
            else
            {
                uw16_k = uw16_i;  //  �ڶ��β鵽С���㡣
                uc_MoveFlag = 2;
                break;
            }
        }
    }

    if((uc_MoveFlag < 2)||(uw16_k == (2*TC_8200_MAX_COM_READ_REPLY-1))||(uw16_i == 2*TC_8200_MAX_COM_READ_REPLY))
        return;   // �Ҳ����������С��λ���Զ��˳���

    uw16_k = uw16_k - uw16_j - 3;   // ����ڶ���ֵ�������������  // �磺 [-22.2'CR'-22.2'CR'-22.2'CR']   (j=3,k=9;����-22: 9-3-3=3����3������Ϊ��ȥ����һ����ЧС���͵�һ�Ļ��б�־'CR'���Լ��ڶ���С����)

    for(uw16_i = 0; uw16_i < TC_8200_MAX_COM_READ_REPLY; uw16_i ++)
    {   //  ��������400�ȸ�200�ȡ�  // Temperature range -200��C to +400��C (-328��F to +725��F) //Resolution 0.1��C or ��F, 0.1% RH, or 0.01 for other linear applications
        MTCI_CTRL::gR_CmdBuf[uw16_i] =  MTCI_CTRL::gR_CmdBuf[uw16_i + uw16_j + 3];  // ���õڶ����ҵ�������Ϊ�������������һ��ֵ��С��λ�ͻ��б�־'CR'(0x0D)ƽ�����ݡ�ָ�����������ĵط���
        if(uw16_i >= uw16_k + 2) // ��2������Ϊ�˱����������Ⱥ�С�����һλ��Ч��
            MTCI_CTRL::gR_CmdBuf[uw16_i] = '0'; // С�����һλ��Ч��������д�㲹�䡣
    }

    pCmd = MTCI_CTRL::gR_CmdBuf; //DATA FROM CHAMBER
    p = MTCI_CTRL::gR_CmdBuf; //DATA FROM CHAMBER

    if( (*pCmd) == 0 )    //  Ϊ���˳���
        return;

    if ( (*pCmd) == '+' || (*pCmd) == '-' || ( (*pCmd) >= '0' && (*pCmd) <= '9' ) )
    {
        p=MTCI_datatypes::ctrl_String2Value_Signed(pCmd, &PresentValue);
        //PresentValue = (short int) (atof(pCmd)*10);  // �Ŵ�10���ϴ�

        if( p == NULL )
            return;

        MTCI_CTRL::SetPresentValue(ChanNo, PresentValue,SETPOINTVALUE_TEMPERATURE,FIRST_CHAMBER_CHANNEL);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);
    }
    else
    {
        return;
    }

}

void TC_8200::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����д����
    short int  register_data;
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];

    for(MbChkSum = 0; MbChkSum < TC_8200_MAX_COM_WRITE_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteCmd_8200[MbChkSum];
    register_data = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];

    MbChkSum = MTCI_datatypes::ctrl_Value2String_Dot(&register_data,MbBuf + TC_8200_WRITE_CMD_POINT_VAlUE);
    if(MbChkSum > (TC_8200_MAX_COM_WRITE_CMD - TC_8200_WRITE_CMD_POINT_VAlUE - 1))
        MbChkSum = (TC_8200_MAX_COM_WRITE_CMD - TC_8200_WRITE_CMD_POINT_VAlUE - 1);  // ���ݳ������6�ֽ�

    //ctrl_Value2String(register_data,MbBuf+3+6);
    //gcvt((double) (register_data/10), 6, MbBuf+3+6);    // ��С10������

    MbBuf[TC_8200_WRITE_CMD_POINT_VAlUE + MbChkSum] = uc_COM_WriteCmd_8200[TC_8200_MAX_COM_WRITE_CMD - 1]; // ��д0X0D��β
    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_8200_WRITE_CMD_POINT_VAlUE + MbChkSum + 1);

    //MTCI_CTRL::gW_WriteCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;   // ���¿��Ʊ���ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}


void TC_8200::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ������������
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];
    for(MbChkSum = 0; MbChkSum < TC_8200_MAX_COM_RUN_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_RunCmd_8200[MbChkSum];
    //uart_putcmd(ChanNo,"\x01\x03\x00\x64\x00\x01\xC5\xD5");
    //uart_putcmd(ChanNo,uc_COM_ReadCmd,MAX_COM_READ_CMD); // ʵ���ϸ����Ͷ�����һ���Ƕ�ȡ�¶�����
    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_8200_MAX_COM_RUN_CMD);

    //MTCI_CTRL::gW_RunCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;  // ���¿��Ʊ���ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}


void TC_8200::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ���Ͷ�����
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];

    for(MbChkSum = 0; MbChkSum < TC_8200_MAX_COM_READ_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_ReadCmd_8200[MbChkSum];

    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_8200_MAX_COM_READ_CMD);

    //MTCI_CTRL::gW_ReadCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// ���¿��Ʊ���ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����

}

void TC_8200::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����ֹͣ����

    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];

    for(MbChkSum = 0; MbChkSum < TC_8200_MAX_COM_STOP_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_8200[MbChkSum];

    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_8200_MAX_COM_STOP_CMD);

    //MTCI_CTRL::gW_StopCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// ���¿��Ʊ���ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}


