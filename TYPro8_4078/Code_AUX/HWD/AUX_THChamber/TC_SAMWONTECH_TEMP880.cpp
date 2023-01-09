/*****************************************************************************
// �����ļ�      :TC_SAMWONTECH_TEMP880.c    �����¿����и��ִ����ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"





extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function ����ָ��   //��ʽ  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];


//------------------------------------------------------------------------------------------------
//  SAMWONTECH(������Ԫ)TEMP880 controller //
//  ����ͨ�ŷ�ʽ  SumCheck  ���䷽ʽ��ASCIIģʽ ��SUMУ��
//  �ر�ע�⣺��ʹ��ֹͣ���������������д���һ��ʱ�����Ʊ����޸ġ�


// �Ķ�����˵����
// I)Page    33/123
// A)Operation Mode:  Select Fix mode.
// B)Time Mode:  Select OFF mode.
// II)Page   29/123
// 2.6.3 Fix Stop Screen
// III)Page  31/123
// 2.6.4 Fix Run Screen


// Ĭ�ϳ���ʱ�¶�ֵΪ22���

// communication command structure:
// //-------------------------------------------------------
// (1)  (2)     (3)      (4)  (5)   (6)   (7) (8)
// STX  Address Command   ,   Data  SUM   CR   LF
// //-------------------------------------------------------
// (1) STX(Start of Text)
// Beginning letter of Communication Command
// Indicate the beginning of Communication Command having a code value (0x02) with a STX (Start of Text), which is a letter of Ascii.
// (2) Address
// Indicate the ADDRESS which is a number of TEMP880 that you intend to communicate with.
// (3) Command
// Specify the command from an upper device (See 4.3.2~4.3.9).
// (4) ��,��Divider
// Indicate the Divider which separates the Command and Data by comma (',').
// (5) Data part
// Indicate the letter line which has a regular form of a letter line followed the Communication Command Rule
// (6) Check Sum
// Indicate the inferior rank of 2-byte to D by adding all letters from the letter of after STX to the letter
// of before SUM using the Ascii code and Use only when the protocol of TEMP880 is ��the basic
// protocol of ��PCL!�� + Check Sum��
// check sum = HEX[(2)+(3)+(4)+(5)]
// (7,8) CR,LF Vertical Section letter
// Indicate the end of Communication Command as CR(0x0D), LF(0x0A) by using the Ascii code.
// ([stx] = 0x02, [cr] = 0x0d, [lf] = 0x0a)
// - Para. number : 1~32.
// - dddd : Indicates a character string in hexadecimal format.


//-------------------------------------------------------
//  1-M)�����¶��������ӻ� (���ֽڲ���)
// {{[STX (1 byte)][Addr (2 byte)][WRD (3�ֽ�)][,(1�ֽ�)][No. of contents (2�ֽ�)][,(1�ֽ�)][D-Reg.No1 (4�ֽ�)][,(1�ֽ�)][dddd-1 (4�ֽ�)][,(1�ֽ�)][dddd-? (4�ֽ�)][,(1�ֽ�)][dddd-? (4�ֽ�)][SUM (2�ֽ�)][CR (1�ֽ�)][LR (1�ֽ�)]}
// Transmission : [stx]01WRD,01,0102,00DCDD[cr][lf]
// 02 30 31 57 52 44 2C 30 31 2C 30 31 30 32 2C 30 30 44 43 44 44 0D 0A

// 02 30 31 57 52 44 2C 30 32 2C 30 31 30 32 2C XX XX XX XX [SUM] 0D 0A
// (XXXXXXXX�¶�ֵ�ȴ���λ�ٴ���λ)
// ʹ��full������ʽʱ����ʾֵ=�¶�ֵ/10;    �磺22(220/10=DC/A)��
// [�������ַ�ʽ: (45��*10)  450 = 0X01C2; (-60��*10) -600 = 0XFDA8 (0x10000-0xFDA8=0X258 = 600)]
//
// Reply CMD: {{[STX (1 byte)][Addr (2 byte)][WRD (3�ֽ�)][,(1�ֽ�)][OK (2�ֽ�)][SUM (2�ֽ�)][CR (1�ֽ�)][LR (1�ֽ�)]}
// Response : [stx]01WRD,OK14[cr][lf]
// 02 30 31 57 52 44 2C 4F 4B 31 34 0D 0A
//
const unsigned char uc_COM_WriteCmd_TEMP880[TC_TEMP880_MAX_COM_WRITE_CMD] = {
    0x02, 0x30, 0x31, 0x57, 0x52, 0x44, 0x2C, 0x30, 0x31, 0x2C,
    0x30, 0x31, 0X30, 0X32, 0x2C, 0x30, 0x30, 0x44, 0x43, 0X44,
    0X44, 0x0D, 0x0A
};     // write Master to  Slave cmd  (�����¶�22��)

//  �ӻ��ظ�ʱ��Reply����ֵ��
const unsigned char uc_COM_WriteReply_TEMP880[TC_TEMP880_MAX_COM_WRITE_REPLY] = {
    0x02, 0x30, 0x31, 0x57, 0x52, 0x44, 0x2C, 0x4F, 0x4B, 0X31,
    0X34, 0x0D, 0x0A
};     // write Slave to  Master reply (�ظ����óɹ�)

//  2-M)��ȡ�¶��������ӻ�  (���ֽڲ���)
// {{[STX (1 byte)][Addr (2 byte)][RRD (3�ֽ�)][,(1�ֽ�)][No. of contents (2�ֽ�)][,(1�ֽ�)][D-Reg.No1 (4�ֽ�)][,(1�ֽ�)][D-Reg.No? (4�ֽ�)][SUM (2�ֽ�)][CR (1�ֽ�)][LR (1�ֽ�)]}
// Transmission : [stx]01RRD,01,0001C3[cr][lf]
// 02 30 31 52 52 44 2C 30 31 2C 30 30 30 31 43 33 0D 0A
//
// �ӻ��ظ�ʱ��
// Reply CMD:  {{[STX (1 byte)][Addr (2 byte)][RRD (3�ֽ�)][,(1�ֽ�)][OK (2�ֽ�)][,(1�ֽ�)][dddd-1 (4�ֽ�)][,(1�ֽ�)][dddd-? (4�ֽ�)][SUM (2�ֽ�)][CR (1�ֽ�)][LR (1�ֽ�)]}
// Response : [stx]01RRD,OK,00DC22[cr][lf]
// 02 30 31 52 52 44 2C 4F 4B 2C 30 30 44 43 32 32 0D 0A
// 02 30 31 52 52 44 2C 4F 4B 2C XX XX XX XX [SUM] 0D 0A
// (XXXXXXXX�¶�ֵ�ȴ���λ�ٴ���λ)
// ʹ��full������ʽʱ����ʾֵ=�¶�ֵ/10;    �磺22(220/10=DC/A)��
// [�������ַ�ʽ: (45��*10)  450 = 0X01C2; (-60��*10) -600 = 0XFDA8 (0x10000-0xFDA8=0X258 = 600)]
//
const unsigned char uc_COM_ReadCmd_TEMP880[TC_TEMP880_MAX_COM_READ_CMD] = {
    0x02, 0x30, 0x31, 0x52, 0x52, 0x44, 0x2C, 0x30, 0x31, 0x2C,
    0x30, 0x30, 0X30, 0X31, 0x43, 0X33, 0x0D, 0x0A
};    // read Master to  Slave cmd (��ȡ)

//  �ӻ��ظ�ʱ��Reply
const unsigned char uc_COM_ReadReply_TEMP880[TC_TEMP880_MAX_COM_READ_REPLY] = {
    0x02, 0x30, 0x31, 0x52, 0x52, 0x44, 0x2C, 0x4F, 0x4B, 0x2C,
    0x30, 0x30, 0X44, 0X43, 0X32, 0X32, 0x0D, 0x0A
};     // read Slave to  Master reply  //(�ظ��¶�22�ȣ�����220)
//

//  3-M)Ҫ�����������������ӻ�
// {{[STX (1 byte)][Addr (2 byte)][WRD (3�ֽ�)][,(1�ֽ�)][No. of contents (2�ֽ�)][,(1�ֽ�)][D-Reg.No1 (4�ֽ�)][,(1�ֽ�)][dddd-1 (4�ֽ�)][,(1�ֽ�)][dddd-? (4�ֽ�)][,(1�ֽ�)][dddd-? (4�ֽ�)][SUM (2�ֽ�)][CR (1�ֽ�)][LR (1�ֽ�)]}
// Transmission : [stx]01WRD,02,0104,0001,0101,000195[cr][lf]
// 02 30 31 57 52 44 2C 30 32 2C 30 31 30 34 2C 30 30 30 31 2C 30 31 30 31 2C 30 30 30 31 39 35 0D 0A
//
// Reply CMD: {{[STX (1 byte)][Addr (2 byte)][WRD (3�ֽ�)][,(1�ֽ�)][OK (2�ֽ�)][SUM (2�ֽ�)][CR (1�ֽ�)][LR (1�ֽ�)]}
// Response : [stx]01WRD,OK14[cr][lf]
// 02 30 31 57 52 44 2C 4F 4B 31 34 0D 0A
//

const unsigned char uc_COM_RunCmd_TEMP880[TC_TEMP880_MAX_COM_RUN_CMD] = {
    0x02, 0x30, 0x31, 0x57, 0x52, 0x44, 0x2C, 0x30, 0x32, 0x2C,
    0x30, 0x31, 0X30, 0X34, 0x2C, 0x30, 0x30, 0x30, 0x31, 0x2C,
    0x30, 0x31, 0X30, 0X31, 0x2C, 0x30, 0x30, 0x30, 0x31, 0x39,
    0x35, 0x0D, 0x0A
};    // read Master to  Slave cmd

//  4-M)Ҫ��ֹͣ�����������ӻ�����
// {{[STX (1 byte)][Addr (2 byte)][WRD (3�ֽ�)][,(1�ֽ�)][Para.number (2�ֽ�)][,(1�ֽ�)][D-Reg.No1 (4�ֽ�)][,(1�ֽ�)][dddd-1 (4�ֽ�)][,(1�ֽ�)][dddd-? (4�ֽ�)][,(1�ֽ�)][dddd-? (4�ֽ�)][SUM (2�ֽ�)][CR (1�ֽ�)][LR (1�ֽ�)]}
// Transmission : [stx]01WRD,02,0104,0001,0101,000498[cr][lf]
// 02 30 31 57 52 44 2C 30 32 2C 30 31 30 34 2C 30 30 30 31 2C 30 31 30 31 2C 30 30 30 34 39 38 0D 0A

// Reply CMD: {{[STX (1 byte)][Addr (2 byte)][WRD (3�ֽ�)][,(1�ֽ�)][OK (2�ֽ�)][SUM (2�ֽ�)][CR (1�ֽ�)][LR (1�ֽ�)]}
// Response : [stx]01WRD,OK14[cr][lf]
// 02 30 31 57 52 44 2C 4F 4B 31 34 0D 0A

const unsigned char uc_COM_StopCmd_TEMP880[TC_TEMP880_MAX_COM_STOP_CMD] = {
    0x02, 0x30, 0x31, 0x57, 0x52, 0x44, 0x2C, 0x30, 0x32, 0x2C,
    0x30, 0x31, 0X30, 0X34, 0x2C, 0x30, 0x30, 0x30, 0x31, 0x2C,
    0x30, 0x31, 0X30, 0X31, 0x2C, 0x30, 0x30, 0x30, 0x34, 0x39,
    0x38, 0x0D, 0x0A
};    // read Master to  Slave cmd


//
//  SAMWONTECH(������Ԫ)TEMP880   ����ͨ�ŷ�ʽ  SumCheck  ���䷽ʽ��ASCIIģʽ ��SUMУ��   //
//----------------------------------------------------------------------------------------------------



void TC_TEMP880::Init(unsigned char ChanNo)
{
// ��ʼ������ָ�롣
    unsigned char uc_i; // ��ֹ�����������ȫ����ʼ��һ�κ���ָ�롣
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_TEMP880::SendReadCmd);   //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_TEMP880::SendWriteCmd);  //  1-M)�����¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_TEMP880::SendReadCmd);    //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_TEMP880::SendRunCmd);      //  3-M)Ҫ�����������������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_TEMP880::SendStopCmd);    //  4-M)Ҫ��ֹͣ�����������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_TEMP880::ParseResult);  //  5-S)��ȡ�¶ȴӻ��ظ���������  (���ֽڲ���)
}

void TC_TEMP880::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// �������յĽ������

    //static  unsigned long  uw32_Test[100];
    //static  unsigned char  uc_Test_s;

    unsigned char *pCmd;
    //unsigned char *p;
    //unsigned char uc_MoveFlag = 0;
    //bool b_is_temp;
    //bool NoZero;
    short int PresentValue = 0;
    unsigned short int uw16_i;
    //unsigned short int uw16_j;
    //unsigned short int uw16_k;
    //unsigned char uc_Buffer[4][6];
    unsigned long int uw32_DO = 0;
    unsigned int uw16_Total = PresentValue;

    uw16_Total = UARTFunc::uart_bytenum(ChanNo);
    if( uw16_Total < TC_TEMP880_MIN_RESPONSE_CMD_LEN)
        return;  // �����������ϴ���һ����Ϣ�����ޱ���

    if(uw16_Total > MAX_UART_BUF_SIZE )   // ȡ��������󻺳�����������Ϊ��������ѭ��ʹ�õİ취,���������һЩ�ռ䲻�����
    {
        uw16_Total = MAX_UART_BUF_SIZE;   // ȡ��������󻺳�����������Ϊ��������ѭ��ʹ�õİ취,���������һЩ�ռ䲻�����

    }

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
		uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_i = 0;
    do { // ȥ��[STX (1 byte)][Addr (2 byte)]
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == uc_COM_ReadReply_TEMP880[3] && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_TEMP880[4] && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_TEMP880[5]
                && MTCI_CTRL::gR_CmdBuf[uw16_i+4] == uc_COM_ReadReply_TEMP880[7] && MTCI_CTRL::gR_CmdBuf[uw16_i+5] == uc_COM_ReadReply_TEMP880[8] )
        {   // ���ð�ͷ���β�ж�����ֵ��

            //if(gR_CmdBuf[uw16_i+13] == uc_COM_ReadReply[16] && gR_CmdBuf[uw16_i+14] == uc_COM_ReadReply[17])  // ��β�жϡ�
            if(MTCI_CTRL::gR_CmdBuf[uw16_i+13] == uc_COM_ReadReply_TEMP880[16] && MTCI_CTRL::gR_CmdBuf[uw16_i+14] == uc_COM_ReadReply_TEMP880[17])  // ��β�жϡ�
            {   //  ��������,���д���

                pCmd = &MTCI_CTRL::gR_CmdBuf[uw16_i+7]; // [READ_REPLY_POINT_TEMPVAlUE - 3]=7   // ȥ��[STX (1 byte)][Addr (2 byte)]
                MTCI_datatypes::ctrl_String2Value_Hex(pCmd, &MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[0]);   //  �¶�
                //PresentValue = (short int) (atof(pCmd)*10);  // �Ŵ�10���ϴ�
                MTCI_CTRL::SetPresentValue(ChanNo, MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[FIRST_CHAMBER_CHANNEL],SETPOINTVALUE_TEMPERATURE,FIRST_CHAMBER_CHANNEL);
                CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);

                return;
            }
        }

        uw16_i++;
    } while(uw16_i < uw16_Total);
}

void TC_TEMP880::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����д����ֵ����
    unsigned int  uw16_len = TC_TEMP880_MAX_COM_WRITE_CMD;
    short int  register_data;
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];
    //unsigned long int  uw32_DO;
    //unsigned char XcrcHi,XcrcLo;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    //if(gR_Updata_Flag[ChanNo] == FALSE)
    //    return;

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteCmd_TEMP880[MbChkSum];

    //register_data = gW_TemperatureValue[ChanNo];
    MTCI_datatypes::ctrl_Value2String_Hex(&MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL],MbBuf + TC_TEMP880_WRITE_CMD_POINT_TEMPVAlUE);

    //ctrl_Value2String(register_data,MbBuf+3+6);
    //gcvt((double) (register_data/10), 6, MbBuf+3+6);    // ��С10������ */

    //register_data = gW_HumiValue[ChanNo];
    //ctrl_Value2String(&gW_HumiValue[ChanNo],MbBuf + WRITE_CMD_POINT_HUMIVAlUE);
#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[2] = ChanNo + ASCII_FIRST_UART_NUM;
#endif

    register_data = MTCI_datatypes::CheckSum(MbBuf + 1,TC_TEMP880_MAX_COM_WRITE_CMD - 5) & 0X00FF;
    MTCI_datatypes::ctrl_Value2String_Hex(&register_data,MbBuf + TC_TEMP880_MAX_COM_WRITE_CMD - 4);
    MbBuf[TC_TEMP880_MAX_COM_WRITE_CMD - 4] = MbBuf[TC_TEMP880_MAX_COM_WRITE_CMD-2];
    MbBuf[TC_TEMP880_MAX_COM_WRITE_CMD - 3] = MbBuf[TC_TEMP880_MAX_COM_WRITE_CMD-1];  // ��������
    MbBuf[TC_TEMP880_MAX_COM_WRITE_CMD - 2] = uc_COM_WriteCmd_TEMP880[TC_TEMP880_MAX_COM_WRITE_CMD-2];
    MbBuf[TC_TEMP880_MAX_COM_WRITE_CMD - 1] = uc_COM_WriteCmd_TEMP880[TC_TEMP880_MAX_COM_WRITE_CMD-1]; // ��д0X0D,0X0A ��β

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::gW_WriteCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;   // ���¿��Ʊ���ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}


void TC_TEMP880::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ������������
    unsigned int  uw16_len = TC_TEMP880_MAX_COM_RUN_CMD;
    unsigned int  MbChkSum;
    unsigned char MbBuf[SEND_MAX_BUF];
    short int  register_data;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_RunCmd_TEMP880[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = ChanNo + ASCII_FIRST_UART_NUM;

        register_data = MTCI_datatypes::CheckSum(MbBuf + 1,TC_TEMP880_MAX_COM_WRITE_CMD - 5) & 0X00FF;
        MTCI_datatypes::ctrl_Value2String_Hex(&register_data,MbBuf + TC_TEMP880_MAX_COM_RUN_CMD - 4);
        MbBuf[TC_TEMP880_MAX_COM_RUN_CMD - 4] = MbBuf[TC_TEMP880_MAX_COM_RUN_CMD-2];
        MbBuf[TC_TEMP880_MAX_COM_RUN_CMD - 3] = MbBuf[TC_TEMP880_MAX_COM_RUN_CMD-1];  // ��������
        MbBuf[TC_TEMP880_MAX_COM_RUN_CMD - 2] = uc_COM_RunCmd_TEMP880[TC_TEMP880_MAX_COM_RUN_CMD-2];
        MbBuf[TC_TEMP880_MAX_COM_RUN_CMD - 1] = uc_COM_RunCmd_TEMP880[TC_TEMP880_MAX_COM_RUN_CMD-1]; // ��д0X0D,0X0A ��β
    }
#endif


    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::gW_RunCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;  // ���¿��Ʊ���ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}

void TC_TEMP880::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ���Ͷ�����

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TC_TEMP880_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];
    short int  register_data;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
    {
        MbBuf[MbChkSum] = uc_COM_ReadCmd_TEMP880[MbChkSum];
    }

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = ChanNo + ASCII_FIRST_UART_NUM;

        register_data = MTCI_datatypes::CheckSum(MbBuf + 1,TC_TEMP880_MAX_COM_READ_CMD - 5) & 0X00FF;
        MTCI_datatypes::ctrl_Value2String_Hex(&register_data,MbBuf + TC_TEMP880_MAX_COM_READ_CMD - 4);
        MbBuf[TC_TEMP880_MAX_COM_READ_CMD - 4] = MbBuf[TC_TEMP880_MAX_COM_READ_CMD-2];
        MbBuf[TC_TEMP880_MAX_COM_READ_CMD - 3] = MbBuf[TC_TEMP880_MAX_COM_READ_CMD-1];  // ��������
        MbBuf[TC_TEMP880_MAX_COM_READ_CMD - 2] = uc_COM_ReadCmd_TEMP880[TC_TEMP880_MAX_COM_READ_CMD-2];
        MbBuf[TC_TEMP880_MAX_COM_READ_CMD - 1] = uc_COM_ReadCmd_TEMP880[TC_TEMP880_MAX_COM_READ_CMD-1]; // ��д0X0D,0X0A ��β
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::gW_ReadCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// ���¿��Ʊ���ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}

void TC_TEMP880::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����ֹͣ����

    short int  register_data;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TC_TEMP880_MAX_COM_STOP_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];
    //unsigned long int  uw32_DO;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_TEMP880[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = ChanNo + ASCII_FIRST_UART_NUM;

        register_data = MTCI_datatypes::CheckSum(MbBuf + 1,TC_TEMP880_MAX_COM_STOP_CMD - 5) & 0X00FF;
        MTCI_datatypes::ctrl_Value2String_Hex(&register_data,MbBuf + TC_TEMP880_MAX_COM_WRITE_CMD - 4);
        MbBuf[TC_TEMP880_MAX_COM_STOP_CMD - 4] = MbBuf[TC_TEMP880_MAX_COM_STOP_CMD-2];
        MbBuf[TC_TEMP880_MAX_COM_STOP_CMD - 3] = MbBuf[TC_TEMP880_MAX_COM_STOP_CMD-1];  // ��������
        MbBuf[TC_TEMP880_MAX_COM_STOP_CMD - 2] = uc_COM_StopCmd_TEMP880[TC_TEMP880_MAX_COM_STOP_CMD-2];
        MbBuf[TC_TEMP880_MAX_COM_STOP_CMD - 1] = uc_COM_StopCmd_TEMP880[TC_TEMP880_MAX_COM_STOP_CMD-1]; // ��д0X0D,0X0A ��β
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::gW_StopCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// ���¿��Ʊ���ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}

unsigned char TC_TEMP880::LoadCheckSum(unsigned char CMD_Code,unsigned char *puchMsg, unsigned int usDataLen)
{
//  ����У��ֵ�����Ͱ����档
    int uw32_Count = usDataLen - 5;
    short int  register_data;
    if(uw32_Count <= 0)
        return (0);

    //register_data = MTCI_datatypes::CheckXorSum(&puchMsg[1],usDataLen - 5) & 0X00FF;
    register_data = MTCI_datatypes::CheckSum(&puchMsg[1],usDataLen - 5) & 0X00FF;
    MTCI_datatypes::ctrl_Value2String_Hex(&register_data,(puchMsg + usDataLen - 4));
    puchMsg[usDataLen - 4] = puchMsg[usDataLen - 2];
    puchMsg[usDataLen - 3] = puchMsg[usDataLen - 1];  // ��������
    puchMsg[usDataLen - 2] = uc_COM_WriteCmd_TEMP880[TC_TEMP880_MAX_COM_WRITE_CMD - 2];
    puchMsg[usDataLen - 1] = uc_COM_WriteCmd_TEMP880[TC_TEMP880_MAX_COM_WRITE_CMD - 1]; // ��д0X2A,0X0D ��β

    return (1);
}



