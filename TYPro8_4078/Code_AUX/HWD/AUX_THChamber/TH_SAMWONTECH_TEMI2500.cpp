/*****************************************************************************
// �����ļ�      :TH_SAMWONTECH_TEMI2500.c    �����¿����и��ִ����ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"





extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function ����ָ��   //��ʽ  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];


//------------------------------------------------------------------------------------------------
//  SAMWONTECH(������Ԫ)TEMI2500 controller //
//  ����ͨ�ŷ�ʽ  SumCheck  ���䷽ʽ��ASCIIģʽ ��SUMУ��
//  �ر�ע�⣺��ʹ��ֹͣ���������������д���һ��ʱ�����Ʊ����޸ġ�


// �Ķ�����˵����
// I)Page    31/123
// A)Operation Mode:  Select Fix mode.
// B)Time Mode:  Select OFF mode.
// II)Page   29/123
// 2.6.3 Fix Stop Screen
// III)Page  30/123
// 2.6.4 Fix Run Screen


// Ĭ�ϳ���ʱ�¶�ֵΪ22���
// Ĭ�ϳ�̬ʱʪ��ֵΪ55%RH

// communication command structure:
// //-------------------------------------------------------
// (1)  (2)     (3)      (4)  (5)   (6)   (7) (8)
// STX  Address Command   ,   Data  SUM   CR   LF
// //-------------------------------------------------------
// (1) STX(Start of Text)
// This code indicates the start of a command string.
// (2) Address
// Communication address to identify the instruments.
// (3) Command
// Specify the command from an upper device (See 4.3.2~4.3.9).
// (4) ��,��(Separation character)
// Character for separating command and data by ��,��.
// (5) Data
// Data by Command rule.
// (6) Check Sum
// It is only using when the protocol is ��PCL1��.(basic protocol + check sum)
// check sum = HEX[(2)+(3)+(4)+(5)]
// (7,8) CR,LF
// This control code indicates the end of a command.
// ([stx] = 0x02, [cr] = 0x0d, [lf] = 0x0a)
// - Para. number : 1~32.
// - dddd : Indicates a character string in hexadecimal format.


//-------------------------------------------------------
//  1-M)�����¶��������ӻ� (���ֽڲ���)
//{{[STX (1 byte)][Addr (2 byte)][WRD (3�ֽ�)][,(1�ֽ�)][Count Number (2�ֽ�)][,(1�ֽ�)][D-Reg.-1 (4�ֽ�)][,(1�ֽ�)][Data-1 (4�ֽ�)][,(1�ֽ�)][D-Reg.-n (4�ֽ�)][,(1�ֽ�)][Data-n(4�ֽ�)][SUM (2�ֽ�)][CR (1�ֽ�)][LF (1�ֽ�)]}
//Transmission : [stx]01WRD,02,0102,00DC,0103,0226C4[cr][lf]
//02 30 31 57 52 44 2C 30 32 2C 30 31 30 32 2C 30 30 44 43 2C 30 31 30 33 2C 30 32 32 36 43 34 0D 0A

//02 30 31 57 52 44 2C 30 32 2C 30 31 30 32 2C XX XX XX XX 2C 30 31 30 33 2C YY YY YY YY [SUM] 0D 0A
//(XXXXXXXX�¶�ֵ�ȴ���λ�ٴ���λ; YYYYYYYYYʪ��ֵ�ȴ���λ�ٴ���λ)
//ʹ��full������ʽʱ����ʾֵ=�¶�ֵ/10;    �磺22(220/10=DC/A)��,55(550/10=226/A)%ʪ��Humi.
//[�������ַ�ʽ: (45��*10)  450 = 0X01C2; (-60��*10) -600 = 0XFDA8 (0x10000-0xFDA8=0X258 = 600)]

//Reply CMD: {{[STX (1 byte)][Addr (2 byte)][WRD (3�ֽ�)][,(1�ֽ�)][OK (2�ֽ�)][SUM (2�ֽ�)][CR (1�ֽ�)][LF (1�ֽ�)]}
//Response : [stx]01WRD,OK14[cr][lf]
//02 30 31 57 52 44 2C 4F 4B 31 34 0D 0A
//
const unsigned char uc_COM_WriteCmd_TEMI2500[TH_TEMI2500_MAX_COM_WRITE_CMD] = {
    0x02, 0x30, 0x31, 0x57, 0x52, 0x44, 0x2C, 0x30, 0x32, 0x2C,
    0x30, 0x31, 0X30, 0X32, 0x2C, 0x30, 0x30, 0x44, 0x43, 0x2C,
    0x30, 0x31, 0X30, 0X33, 0x2C, 0x30, 0x32, 0x32, 0x36, 0x43,
    0x34, 0x0D, 0x0A
};     // write Master to  Slave cmd  (�����¶�22��)

//  �ӻ��ظ�ʱ��Reply����ֵ��
const unsigned char uc_COM_WriteReply_TEMI2500[TH_TEMI2500_MAX_COM_WRITE_REPLY] = {
    0x02, 0x30, 0x31, 0x57, 0x52, 0x44, 0x2C, 0x4F, 0x4B, 0x31,
    0x34, 0x0D, 0x0A
};     // write Slave to  Master reply (�ظ����óɹ�)

//  2-M)��ȡ�¶��������ӻ�  (���ֽڲ���)
//{{[STX (1 byte)][Addr (2 byte)][RRD (3�ֽ�)][,(1�ֽ�)][Count Number (2�ֽ�)][,(1�ֽ�)][D-Reg.-1 (4�ֽ�)][,(1�ֽ�)][D-Reg.-n (4�ֽ�)][SUM (2�ֽ�)][CR (1�ֽ�)][LF (1�ֽ�)]}
//Transmission : [stx]01RRD,02,0001,0005B5[cr][lf]
//02 30 31 52 52 44 2C 30 32 2C 30 30 30 31 2C 30 30 30 35 42 35 0D 0A

//�ӻ��ظ�ʱ��
//Reply CMD:  {{[STX (1 byte)][Addr (2 byte)][RRD (3�ֽ�)][,(1�ֽ�)][OK (2�ֽ�)][,(1�ֽ�)][Data-1 (4�ֽ�)][,(1�ֽ�)][Data-n (4�ֽ�)][SUM (2�ֽ�)][CR (1�ֽ�)][LF (1�ֽ�)]}
//Response : [stx]01RRD,OK,00DC,022618[cr][lf]
//02 30 31 52 52 44 2C 4F 4B 2C 30 30 44 43 2C 30 32 32 36 31 38 0D 0A

//02 30 31 52 52 44 2C 4F 4B 2C XX XX XX XX 2C YY YY YY YY [SUM] 0D 0A
//(XXXXXXXX�¶�ֵ�ȴ���λ�ٴ���λ; YYYYYYYYYʪ��ֵ�ȴ���λ�ٴ���λ)
//ʹ��full������ʽʱ����ʾֵ=�¶�ֵ/10;    �磺22(220/10=DC/A)��,55(550/10=226/A)%ʪ��Humi.
// [�������ַ�ʽ: (45��*10)  450 = 0X01C2; (-60��*10) -600 = 0XFDA8 (0x10000-0xFDA8=0X258 = 600)]
//
const unsigned char uc_COM_ReadCmd_TEMI2500[TH_TEMI2500_MAX_COM_READ_CMD] = {
    0x02, 0x30, 0x31, 0x52, 0x52, 0x44, 0x2C, 0x30, 0x32, 0x2C,
    0x30, 0x30, 0X30, 0X31, 0x2C, 0x30, 0x30, 0x30, 0x35, 0x42,
    0x35, 0x0D, 0x0A
};    // read Master to  Slave cmd (��ȡ)

//  �ӻ��ظ�ʱ��Reply
const unsigned char uc_COM_ReadReply_TEMI2500[TH_TEMI2500_MAX_COM_READ_REPLY] = {
    0x02, 0x30, 0x31, 0x52, 0x52, 0x44, 0x2C, 0x4F, 0x4B, 0x2C,
    0x30, 0x30, 0X44, 0X43, 0x2C, 0x30, 0x32, 0x32, 0x36, 0x31,
    0x38, 0x0D, 0x0A
};     // read Slave to  Master reply  //(�ظ��¶�22�ȣ�����220)

//  3-M)Ҫ�����������������ӻ�
//{{[STX (1 byte)][Addr (2 byte)][WRD (3�ֽ�)][,(1�ֽ�)][Count Number (2�ֽ�)][,(1�ֽ�)][D-Reg.-1 (4�ֽ�)][,(1�ֽ�)][Data-1 (4�ֽ�)][,(1�ֽ�)][D-Reg.-n (4�ֽ�)][,(1�ֽ�)][Data-n(4�ֽ�)][SUM (2�ֽ�)][CR (1�ֽ�)][LF (1�ֽ�)]}
//Transmission : [stx]01WRD,02,0104,0001,0101,000195[cr][lf]
//02 30 31 57 52 44 2C 30 32 2C 30 31 30 34 2C 30 30 30 31 2C 30 31 30 31 2C 30 30 30 31 39 35 0D 0A

//Reply CMD: {{[STX (1 byte)][Addr (2 byte)][WRD (3�ֽ�)][,(1�ֽ�)][OK (2�ֽ�)][SUM (2�ֽ�)][CR (1�ֽ�)][LF (1�ֽ�)]}
//Response : [stx]01WRD,OK14[cr][lf]
//02 30 31 57 52 44 2C 4F 4B 31 34 0D 0A
//
const unsigned char uc_COM_RunCmd_TEMI2500[TH_TEMI2500_MAX_COM_RUN_CMD] = {
    0x02, 0x30, 0x31, 0x57, 0x52, 0x44, 0x2C, 0x30, 0x32, 0x2C,
    0x30, 0x31, 0X30, 0X34, 0x2C, 0x30, 0x30, 0x30, 0x31, 0x2C,
    0x30, 0x31, 0X30, 0X31, 0x2C, 0x30, 0x30, 0x30, 0x31, 0x39,
    0x35, 0x0D, 0x0A
};    // read Master to  Slave cmd

//  4-M)Ҫ��ֹͣ�����������ӻ�����
//{{[STX (1 byte)][Addr (2 byte)][WRD (3�ֽ�)][,(1�ֽ�)][Count Number (2�ֽ�)][,(1�ֽ�)][D-Reg.-1 (4�ֽ�)][,(1�ֽ�)][Data-1 (4�ֽ�)][,(1�ֽ�)][D-Reg.-n (4�ֽ�)][,(1�ֽ�)][Data-n(4�ֽ�)][SUM (2�ֽ�)][CR (1�ֽ�)][LF (1�ֽ�)]}
//Transmission : [stx]01WRD,02,0104,0001,0101,000498[cr][lf]
//02 30 31 57 52 44 2C 30 32 2C 30 31 30 34 2C 30 30 30 31 2C 30 31 30 31 2C 30 30 30 34 39 38 0D 0A

//Reply CMD: {{[STX (1 byte)][Addr (2 byte)][WRD (3�ֽ�)][,(1�ֽ�)][OK (2�ֽ�)][SUM (2�ֽ�)][CR (1�ֽ�)][LF (1�ֽ�)]}
//Response : [stx]01WRD,OK14[cr][lf]
//02 30 31 57 52 44 2C 4F 4B 31 34 0D 0A
//
const unsigned char uc_COM_StopCmd_TEMI2500[TH_TEMI2500_MAX_COM_STOP_CMD] = {
    0x02, 0x30, 0x31, 0x57, 0x52, 0x44, 0x2C, 0x30, 0x32, 0x2C,
    0x30, 0x31, 0X30, 0X34, 0x2C, 0x30, 0x30, 0x30, 0x31, 0x2C,
    0x30, 0x31, 0X30, 0X31, 0x2C, 0x30, 0x30, 0x30, 0x34, 0x39,
    0x38, 0x0D, 0x0A
};    // read Master to  Slave cmd


//
//  SAMWONTECH(������Ԫ)TEMI2500  ����ͨ�ŷ�ʽ  SumCheck  ���䷽ʽ��ASCIIģʽ ��SUMУ��   //
//----------------------------------------------------------------------------------------------------


void TH_TEMI2500::Init(unsigned char ChanNo)
{
// ��ʼ������ָ�롣
    unsigned char uc_i; // ��ֹ�����������ȫ����ʼ��һ�κ���ָ�롣
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_TEMI2500::SendReadCmd);   //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_TEMI2500::SendWriteCmd);  //  1-M)�����¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_TEMI2500::SendReadCmd);    //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_TEMI2500::SendRunCmd);      //  3-M)Ҫ�����������������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_TEMI2500::SendStopCmd);    //  4-M)Ҫ��ֹͣ�����������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_TEMI2500::ParseResult);  //  5-S)��ȡ�¶ȴӻ��ظ���������  (���ֽڲ���)
}


void TH_TEMI2500::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
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
// 	unsigned long int uw32_DO = 0;
    unsigned int uw16_Total = PresentValue;
    uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

    if( uw16_Total < TH_TEMI2500_MIN_RESPONSE_CMD_LEN)
        return;  // �����������ϴ���һ����Ϣ�����ޱ���

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_i = 0;
    do { // ȥ��[STX (1 byte)][Addr (2 byte)]
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == uc_COM_ReadReply_TEMI2500[3] && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_TEMI2500[4] && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_TEMI2500[5]
                && MTCI_CTRL::gR_CmdBuf[uw16_i+4] == uc_COM_ReadReply_TEMI2500[7] && MTCI_CTRL::gR_CmdBuf[uw16_i+5] == uc_COM_ReadReply_TEMI2500[8] )
        {   // ���ð�ͷ���β�ж�����ֵ��

            if(MTCI_CTRL::gR_CmdBuf[uw16_i+18] == uc_COM_ReadReply_TEMI2500[21] && MTCI_CTRL::gR_CmdBuf[uw16_i+19] == uc_COM_ReadReply_TEMI2500[22])  // ��β�жϡ�
            {   //  ��������,���д���

                //PresentValue = ((short int)(gR_CmdBuf[uw16_i+7]) << 12 |gR_CmdBuf[uw16_i+8]) << 8 |gR_CmdBuf[uw16_i+9]) << 4 | (gR_CmdBuf[uw16_i+10]) );

                pCmd = &MTCI_CTRL::gR_CmdBuf[uw16_i+7]; // [READ_REPLY_POINT_TEMPVAlUE - 3]=7   // ȥ��[STX (1 byte)][Addr (2 byte)]
                MTCI_datatypes::ctrl_String2Value_Hex(pCmd, &MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[0]);   //  �¶�
                //PresentValue = (short int) (atof(pCmd)*10);  // �Ŵ�10���ϴ�
                MTCI_CTRL::SetPresentValue(ChanNo, MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[0],SETPOINTVALUE_TEMPERATURE,FIRST_CHAMBER_CHANNEL);

                pCmd = &MTCI_CTRL::gR_CmdBuf[uw16_i+12];   // [READ_REPLY_POINT_HUMIVAlUE - 3]=12 // ȥ��[STX (1 byte)][Addr (2 byte)]
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



void TH_TEMI2500::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����д����ֵ����

    short int  register_data;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_TEMI2500_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];
// 		unsigned long int  uw32_DO;
    //unsigned char XcrcHi,XcrcLo;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    //if(gR_Updata_Flag[ChanNo] == FALSE)
    //    return;

    for(MbChkSum = 0; MbChkSum < TH_TEMI2500_MAX_COM_WRITE_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteCmd_TEMI2500[MbChkSum];

    //register_data = gW_TemperatureValue[ChanNo];
    MTCI_datatypes::ctrl_Value2String_Hex(&MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL],MbBuf + TH_TEMI2500_WRITE_CMD_POINT_TEMPVAlUE);

    //ctrl_Value2String(register_data,MbBuf+3+6);
    //gcvt((double) (register_data/10), 6, MbBuf+3+6);    // ��С10������ */

    //register_data = gW_HumiValue[ChanNo];
    MTCI_datatypes::ctrl_Value2String_Hex(&MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL],MbBuf + TH_TEMI2500_WRITE_CMD_POINT_HUMIVAlUE);

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[2] = ChanNo + ASCII_FIRST_UART_NUM;
#endif

    register_data = MTCI_datatypes::CheckSum(MbBuf + 1,TH_TEMI2500_MAX_COM_WRITE_CMD - 5) & 0X00FF;
    MTCI_datatypes::ctrl_Value2String_Hex(&register_data,MbBuf + TH_TEMI2500_MAX_COM_WRITE_CMD - 4);
    MbBuf[TH_TEMI2500_MAX_COM_WRITE_CMD - 4] = MbBuf[TH_TEMI2500_MAX_COM_WRITE_CMD-2];
    MbBuf[TH_TEMI2500_MAX_COM_WRITE_CMD - 3] = MbBuf[TH_TEMI2500_MAX_COM_WRITE_CMD-1];  // ��������
    MbBuf[TH_TEMI2500_MAX_COM_WRITE_CMD - 2] = uc_COM_WriteCmd_TEMI2500[TH_TEMI2500_MAX_COM_WRITE_CMD-2];
    MbBuf[TH_TEMI2500_MAX_COM_WRITE_CMD - 1] = uc_COM_WriteCmd_TEMI2500[TH_TEMI2500_MAX_COM_WRITE_CMD-1]; // ��д0X0D,0X0A ��β

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::gW_WriteCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}


void TH_TEMI2500::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ������������
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_TEMI2500_MAX_COM_RUN_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];
    short int  register_data;
    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < TH_TEMI2500_MAX_COM_RUN_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_RunCmd_TEMI2500[MbChkSum];
    uw16_len = TH_TEMI2500_MAX_COM_RUN_CMD;


#ifdef COM_CMD_CHANNEL_DIFFER

    if( ChanNo != 0 )
    {
        MbBuf[2] = ChanNo + ASCII_FIRST_UART_NUM;
        register_data = MTCI_datatypes::CheckSum(MbBuf + 1,TH_TEMI2500_MAX_COM_RUN_CMD - 5) & 0X00FF;
        MTCI_datatypes::ctrl_Value2String_Hex(&register_data,MbBuf + TH_TEMI2500_MAX_COM_RUN_CMD - 4);
        MbBuf[TH_TEMI2500_MAX_COM_RUN_CMD - 4] = MbBuf[TH_TEMI2500_MAX_COM_RUN_CMD-2];
        MbBuf[TH_TEMI2500_MAX_COM_RUN_CMD - 3] = MbBuf[TH_TEMI2500_MAX_COM_RUN_CMD-1];  // ��������
        MbBuf[TH_TEMI2500_MAX_COM_RUN_CMD - 2] = uc_COM_RunCmd_TEMI2500[TH_TEMI2500_MAX_COM_RUN_CMD-2];
        MbBuf[TH_TEMI2500_MAX_COM_RUN_CMD - 1] = uc_COM_RunCmd_TEMI2500[TH_TEMI2500_MAX_COM_RUN_CMD-1]; // ��д0X0D,0X0A ��β
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::gW_RunCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}

void TH_TEMI2500::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ���Ͷ�����

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_TEMI2500_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];
    short int  register_data;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < TH_TEMI2500_MAX_COM_READ_CMD; MbChkSum++)
    {
        MbBuf[MbChkSum] = uc_COM_ReadCmd_TEMI2500[MbChkSum];
    }

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = ChanNo + ASCII_FIRST_UART_NUM;
        register_data = MTCI_datatypes::CheckSum(MbBuf + 1,TH_TEMI2500_MAX_COM_READ_CMD - 5) & 0X00FF;
        MTCI_datatypes::ctrl_Value2String_Hex(&register_data,MbBuf + TH_TEMI2500_MAX_COM_READ_CMD - 4);
        MbBuf[TH_TEMI2500_MAX_COM_READ_CMD - 4] = MbBuf[TH_TEMI2500_MAX_COM_READ_CMD-2];
        MbBuf[TH_TEMI2500_MAX_COM_READ_CMD - 3] = MbBuf[TH_TEMI2500_MAX_COM_READ_CMD-1];  // ��������
        MbBuf[TH_TEMI2500_MAX_COM_READ_CMD - 2] = uc_COM_ReadCmd_TEMI2500[TH_TEMI2500_MAX_COM_READ_CMD-2];
        MbBuf[TH_TEMI2500_MAX_COM_READ_CMD - 1] = uc_COM_ReadCmd_TEMI2500[TH_TEMI2500_MAX_COM_READ_CMD-1]; // ��д0X0D,0X0A ��β
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::gW_ReadCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}

void TH_TEMI2500::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����ֹͣ����

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_TEMI2500_MAX_COM_STOP_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];
// 	unsigned long int  uw32_DO;
    short int  register_data;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < TH_TEMI2500_MAX_COM_STOP_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_TEMI2500[MbChkSum];
    uw16_len = TH_TEMI2500_MAX_COM_STOP_CMD;

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = ChanNo + ASCII_FIRST_UART_NUM;
        register_data = MTCI_datatypes::CheckSum(MbBuf + 1,TH_TEMI2500_MAX_COM_STOP_CMD - 5) & 0X00FF;
        MTCI_datatypes::ctrl_Value2String_Hex(&register_data,MbBuf + TH_TEMI2500_MAX_COM_STOP_CMD - 4);
        MbBuf[TH_TEMI2500_MAX_COM_STOP_CMD - 4] = MbBuf[TH_TEMI2500_MAX_COM_STOP_CMD-2];
        MbBuf[TH_TEMI2500_MAX_COM_STOP_CMD - 3] = MbBuf[TH_TEMI2500_MAX_COM_STOP_CMD-1];  // ��������
        MbBuf[TH_TEMI2500_MAX_COM_STOP_CMD - 2] = uc_COM_StopCmd_TEMI2500[TH_TEMI2500_MAX_COM_STOP_CMD-2];
        MbBuf[TH_TEMI2500_MAX_COM_STOP_CMD - 1] = uc_COM_StopCmd_TEMI2500[TH_TEMI2500_MAX_COM_STOP_CMD-1]; // ��д0X0D,0X0A ��β
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::MTCI_CTRL::gW_StopCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}



