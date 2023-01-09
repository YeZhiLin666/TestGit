/*****************************************************************************
// �����ļ�      :TH_SIEMENS_S7_226CN.C    �����¿����и��ִ����ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"





extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function ����ָ��   //��ʽ  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];


#if(DEBUG_TRACE_RECORD== 1)
extern UWord32 uw32_TimeStamp_Test[100];
extern UWord16 uw16_Put;
extern UWord16 uw16_Get;
extern UWord32 uw32_Count[10];
#endif


//------------------------------------------------------------------------------------------------
// TH_SIEMENS_S7-226CN  ����ͨ�ŷ�ʽ  ACSII  ���䷽ʽ���޼��� //
//

// Ĭ�ϳ���ʱ�¶�ֵΪ22���
// Ĭ�ϳ�̬ʱʪ��ֵΪ55%RH

//  1-M)�����¶��������ӻ� (����Word����)
//  CMD: >MMWDUUUUDDDDDDDDr
//           MM������(�ڱ��ʱ���ת��Ϊ16��������)  WD��д����  UUUU��S7200 �ڲ�V �洢����ַ���ڱ��ʱ���ת��Ϊ16 �������ݣ�
//           DDDDDDDD����ָ����V��Ԫ��ʼ��4�ֽ����ݣ�ÿ2���ַ�����һ���ֽڡ�r���س��ַ�(0X0D)
//  MODEL: >00WD03C400DC0226r
const unsigned char uc_COM_WriteTempCmd_S7_226CN[TH_S7_226CN_MAX_COM_WRITE_CMD] = {
    0x3E, 0x30, 0x30, 0x57, 0x44, 0x30, 0x33, 0x43, 0x34, 0x30,
    0x30, 0x44, 0x43, 0x30, 0x32, 0x32, 0x36, 0x0D
};     // write Master to  Slave cmd  (�����¶�22�ȣ�����220)  // ������������ʽ

//  MODEL:>00WD035441B00000r
//0x3E, 0x30, 0x30, 0x57, 0x44, 0x30, 0x33, 0x35, 0x34, 0x34,
//0x31, 0x42, 0x30, 0x30, 0x30, 0x30, 0x30, 0x0D};     // write Master to  Slave cmd  (�����¶�22�ȣ�����220)  // ���ø�������ʽ


//����ֵ=ʪ��ֵ*10;    �磺55(55*10),85(85*10) %ʪ��Humi.
//  MODEL: >00WD03C602260000r
const unsigned char uc_COM_WriteHumiCmd_S7_226CN[TH_S7_226CN_MAX_COM_WRITE_CMD] = {
    0x3E, 0x30, 0x30, 0x57, 0x44, 0x30, 0x33, 0x43, 0x36, 0x30,
    0x32, 0x32, 0x36, 0x30, 0x30, 0x30, 0x30, 0x0D
};     // write Master to  Slave cmd  (����ʪ��55%������550)   // ������������ʽ

//  MODEL: >00WD035842480000r
//0x3E, 0x30, 0x30, 0x57, 0x44, 0x30, 0x33, 0x35, 0x38, 0x34,
//0x32, 0x34, 0x38, 0x30, 0x30, 0x30, 0x30, 0x0D};     // write Master to  Slave cmd  (����ʪ��55%������550)  // ���ø�������ʽ

//  �ӻ��ظ�ʱ��Replyһģһ����
//  Reply CMD: <MMVDUUUUaabbccddee��r      <MMVDUUUU����Ӧ����ͷ(0X3E)   aabbccddee����ָ����V��Ԫ��ʼ��128�ֽڣ�ÿ2���ַ�����һ���ֽڡ�  r���س��ַ�(0X0D)
//  MODEL: <00WD03C4r
const unsigned char uc_COM_WriteReply_S7_226CN[TH_S7_226CN_MAX_COM_WRITE_CMD] = {
    0x3E, 0x30, 0x30, 0x57, 0x44, 0x30, 0x33, 0x43, 0x34, 0x0D
};     // write Slave to  Master reply

//  2-M)��ȡ�¶��������ӻ�  (����Word����)
//  CMD: >MMVDUUUUr        >����ʼ�ַ�(0X3E)    MM������(�ڱ��ʱ���ת��Ϊ16��������)  VD��������  UUUU��S7200 �ڲ�V �洢����ַ���ڱ��ʱ���ת��Ϊ16 �������ݣ� r���س��ַ�(0X0D)
//  MODEL: >00VD0324r
const unsigned char uc_COM_ReadTempCmd_S7_226CN[TH_S7_226CN_MAX_COM_READ_CMD] = {
    0x3E, 0x30, 0x30, 0x56, 0x44, 0x30, 0x33, 0x32, 0x34, 0x0D
};    // read Master to  Slave cmd (��ȡ�¶�)

//  MODEL: >00VD035Cr
const unsigned char uc_COM_ReadHumiCmd_S7_226CN[TH_S7_226CN_MAX_COM_READ_CMD] = {
    0x3E, 0x30, 0x30, 0x56, 0x44, 0x30, 0x33, 0x35, 0x43, 0x0D
};    // read Master to  Slave cmd (��ȡʪ��)


//  Reply CMD:  <MMVDUUUUaabbccddee��r      <MMVDUUUU����Ӧ����ͷ(0X3C)    aabbccddee����ָ����V��Ԫ��ʼ��128�ֽڣ�ÿ2���ַ�����һ���ֽڡ�  r���س��ַ�(0X0D)
//  Reply MODEL: <00VD032441A00000000000000000000040E0000040E00000250014600400000040E000000000000000000000000000000000000000000000000100000000000000030000r
const unsigned char uc_COM_ReadReply_S7_226CN[TH_S7_226CN_MAX_COM_READ_REPLY] = {
    0x3C, 0x30, 0x30, 0x56, 0x44, 0x30, 0x33, 0x32, 0x34, 0x34,
    0x31, 0x41, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x34, 0x30, 0x45, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x34, 0x30, 0x45, 0x30, 0x30, 0x30, 0x30, 0x30, 0x32,
    0x35, 0x30, 0x30, 0x31, 0x34, 0x36, 0x30, 0x30, 0x34, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x34, 0x30, 0x45, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x33, 0x30, 0x30, 0x30, 0x30, 0x0D
};   // read Slave to  Master reply  //(�ظ��¶�22�ȣ�����220)

//  3-M)Ҫ�����������������ӻ�
//  MODEL: >00WD038800010000r
const unsigned char uc_COM_RunCmd_S7_226CN_T[TH_S7_226CN_MAX_COM_RUN_CMD] =  {
    0x3E, 0x30, 0x30, 0x57, 0x44, 0x30, 0x33, 0x38, 0x38, 0x30,
    0x30, 0x30, 0x31, 0x30, 0x30, 0x30, 0x30, 0x0D
};    // Run Master to  Slave cmd

//  MODEL: >00WD038800010002r
const unsigned char uc_COM_RunCmd_S7_226CN_TH[TH_S7_226CN_MAX_COM_RUN_CMD] =  {
    0x3E, 0x30, 0x30, 0x57, 0x44, 0x30, 0x33, 0x38, 0x38, 0x30,
    0x30, 0x30, 0x31, 0x30, 0x30, 0x30, 0x32, 0x0D
};    // Run Master to  Slave cmd

//  4-M)Ҫ��ֹͣ�����������ӻ�����
//  MODEL: >00WD038800000000r
const unsigned char uc_COM_StopCmd_S7_226CN_T[TH_S7_226CN_MAX_COM_STOP_CMD] =  {
    0x3E, 0x30, 0x30, 0x57, 0x44, 0x30, 0x33, 0x38, 0x38, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x0D
};     // Stop Master to  Slave cmd

//  MODEL: >00WD038800000002r
const unsigned char uc_COM_StopCmd_S7_226CN_TH[TH_S7_226CN_MAX_COM_STOP_CMD] =  {
    0x3E, 0x30, 0x30, 0x57, 0x44, 0x30, 0x33, 0x38, 0x38, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x32, 0x0D
};     // Stop Master to  Slave cmd

//  5-M)��ȡDIͨ�������������ӻ�����
//  MODEL: >00VD036Cr
const unsigned char uc_COM_ReadDICmd_S7_226CN[TH_S7_226CN_MAX_COM_READ_CMD] = {
    0x3E, 0x30, 0x30, 0x56, 0x44, 0x30, 0x33, 0x36, 0x43, 0x0D
};    // read Master to  Slave cmd (��ȡDI)


//
// S7_226CN  ����ͨ�ŷ�ʽ  ASCII  ���䷽ʽ����У�� //
//----------------------------------------------------------------------------------------------------


void TH_S7_226CN::Init(unsigned char ChanNo)
{
// ��ʼ������ָ�롣
    unsigned char uc_i; // ��ֹ�����������ȫ����ʼ��һ�κ���ָ�롣
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_S7_226CN::SendReadCmd);   //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_S7_226CN::SendWriteCmd);  //  1-M)�����¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_S7_226CN::SendReadCmd);    //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_S7_226CN::SendRunCmd);      //  3-M)Ҫ�����������������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_S7_226CN::SendStopCmd);    //  4-M)Ҫ��ֹͣ�����������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_S7_226CN::ParseResult);  //  5-S)��ȡ�¶ȴӻ��ظ���������  (���ֽڲ���)

    MTCI_CTRL::uc_RunTH[ChanNo] = 1;
    MTCI_CTRL::uc_RunFrist[ChanNo] = 0;
    MTCI_CTRL::uc_RunCount[ChanNo] = 0;
    MTCI_CTRL::uc_ReadCount[ChanNo] = 0;
    MTCI_CTRL::uc_LockModleT[ChanNo] = 0;
    MTCI_CTRL::gW_T_Back[ChanNo] =  210; // 220
    MTCI_CTRL::gR_PV_Back[ChanNo] =  210; // 220
}

/*----*/
unsigned char TH_S7_226CN::SetPresentValue_Check(unsigned char ChanNo, short int Value)
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

void TH_S7_226CN::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// �������յĽ������
    //unsigned char *p;
    //unsigned char uc_MoveFlag = 0;
    //bool b_is_temp;
    //bool NoZero;
    unsigned char  Modle = 1;
    short int PresentValue = 0;
    unsigned short int uw16_i;
    //unsigned short int uw16_j;
    //unsigned short int uw16_k;
    unsigned int uw16_Total;
    union  F_data f_Value;
    short int temp = 0;

//     uw16_Total =  uart_bytenum(ChanNo);
    uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

    if( uw16_Total < TH_S7_226CN_MIN_RESPONSE_CMD_LEN)
        return;  // �����������ϴ���һ����Ϣ�����ޱ���

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);


    uw16_i = 0;
    do {
#ifdef COM_CMD_CHANNEL_DIFFER
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == uc_COM_ReadReply_S7_226CN[0]     // <
                && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == (ChanNo+S7_226CN_ASCII_FIRST)
                && (MTCI_CTRL::gR_CmdBuf[uw16_i+7] == 0X32 || MTCI_CTRL::gR_CmdBuf[uw16_i+7] == 0X35)
                && (MTCI_CTRL::gR_CmdBuf[uw16_i+8] == 0X34 || MTCI_CTRL::gR_CmdBuf[uw16_i+8] == 0X43)   // VD 0324 035C��ȡ�¶ȡ���ȡʪ��
                && (MTCI_CTRL::gR_CmdBuf[uw16_i+137] == 0X0D))  // \r
#else
        if(gR_CmdBuf[uw16_i] == uc_COM_ReadReply_S7_226CN[0]     // <
                && gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_S7_226CN[2]
                && (gR_CmdBuf[uw16_i+7] == 0X32 || gR_CmdBuf[uw16_i+7] == 0X35)
                && (gR_CmdBuf[uw16_i+8] == 0X34 || gR_CmdBuf[uw16_i+8] == 0X43)    // VD 0324 035C��ȡ�¶ȡ���ȡʪ��
                && (gR_CmdBuf[uw16_i+137] == 0X0D))  // \r
#endif
        {   // ���ð�ͷ���β�ж�����ֵ��
// 	       if(gW_ReadRepeatNum[ChanNo] < TC_S7_226CN_MIN_RESPONSE_CMD_NUM)
// 			   return;	// �����¶Ȼ�ʪ�Ƚ����������ʱ,���´���ֵ��

            // ��ʮ������8λ�任��4λ��  ��'E' 0X45-0X30�任��0X0E
            f_Value.uc_d[0] = MTCI_datatypes::Convert_ASCIIToHEX(MTCI_CTRL::gR_CmdBuf[uw16_i+15]) << 4 | MTCI_datatypes::Convert_ASCIIToHEX(MTCI_CTRL::gR_CmdBuf[uw16_i+16]);
            f_Value.uc_d[1] = MTCI_datatypes::Convert_ASCIIToHEX(MTCI_CTRL::gR_CmdBuf[uw16_i+13]) << 4 | MTCI_datatypes::Convert_ASCIIToHEX(MTCI_CTRL::gR_CmdBuf[uw16_i+14]);
            f_Value.uc_d[2] = MTCI_datatypes::Convert_ASCIIToHEX(MTCI_CTRL::gR_CmdBuf[uw16_i+11]) << 4 | MTCI_datatypes::Convert_ASCIIToHEX(MTCI_CTRL::gR_CmdBuf[uw16_i+12]);
            f_Value.uc_d[3] = MTCI_datatypes::Convert_ASCIIToHEX(MTCI_CTRL::gR_CmdBuf[uw16_i+9])  << 4 | MTCI_datatypes::Convert_ASCIIToHEX(MTCI_CTRL::gR_CmdBuf[uw16_i+10]);

            PresentValue = (short int) (f_Value.f_d * 10);

            if( TH_S7_226CN::SetPresentValue_Check(ChanNo, PresentValue) )
            {
                //gW_ReadCmd_Flag[ChanNo] = SETPOINTVALUE_TEMPERATURE;

                if(MTCI_CTRL::gR_CmdBuf[uw16_i+8] == 0X43)   //  VD  035C��ȡʪ��   // �ڴ��������ֶ�����������£����������л���
                    uc_Temp4Humi_Flag = SETPOINTVALUE_HUMI;
                else
                    uc_Temp4Humi_Flag = SETPOINTVALUE_TEMPERATURE;
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue, uc_Temp4Humi_Flag, FIRST_CHAMBER_CHANNEL);
            }

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // ���½��ճ�ʱʱ��

            if((MTCI_CTRL::gC_IsRunning[ChanNo])&&(MTCI_CTRL::uc_LockModleT[ChanNo]==0))   // ��ʽ�������ÿ���ֵ��δ�����¶�ģʽ
            {
                temp = abs(MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[FIRST_CHAMBER_CHANNEL]-MTCI_CTRL::gR_PV_Back[ChanNo]);  //  ������Χ-+0.5�� 10.5~9.5
                if(temp < 1)   // ���仯����Ϊ��������
                {
                    MTCI_CTRL::uc_ReadCount[ChanNo] = 0;
                    return;
                }

                if(MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[FIRST_CHAMBER_CHANNEL] <= (100+5))     // �¶�ֵΪ10.5��ȱ���
                    Modle = 0; //  ��һ�������жϷ����¡�ʪ����
                else if(MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[FIRST_CHAMBER_CHANNEL] >= (90+5))
                    Modle = 1;

                if(++MTCI_CTRL::uc_ReadCount[ChanNo] >= 10)
                {
                    MTCI_CTRL::gR_PV_Back[ChanNo] = MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[FIRST_CHAMBER_CHANNEL];
                    if(MTCI_CTRL::uc_RunTH[ChanNo] != Modle)
                    {
                        MTCI_CTRL::uc_RunTH[ChanNo] = Modle;
                        MTCI_CTRL::uc_RunFrist[ChanNo] = 0;   // �����¡�ʪģʽ�л�
                        MTCI_CTRL::uc_RunCount[ChanNo] = 0;
                    }
                    MTCI_CTRL::uc_ReadCount[ChanNo] = 0;
                }
            }

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // ���½��ճ�ʱʱ��

            return;
        }

#ifdef COM_CMD_CHANNEL_DIFFER
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == uc_COM_ReadReply_S7_226CN[0]     // <
                && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == (ChanNo+S7_226CN_ASCII_FIRST)
                && MTCI_CTRL::gR_CmdBuf[uw16_i+7] == 0X36
                && MTCI_CTRL::gR_CmdBuf[uw16_i+8] == 0X43   //  VD 036C ��ȡDI
                && MTCI_CTRL::gR_CmdBuf[uw16_i+137] == 0X0D)  // \r
#else
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == uc_COM_ReadReply_S7_226CN[0]     // <
                && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == uc_COM_ReadReply_S7_226CN[2]
                && MTCI_CTRL::gR_CmdBuf[uw16_i+7] == 0X36
                && MTCI_CTRL::gR_CmdBuf[uw16_i+8] == 0X43    //  VD 036C ��ȡDI
                && MTCI_CTRL::gR_CmdBuf[uw16_i+137] == 0X0D)  // \r
#endif
        {
            // DIΪ�ϴ�������Ϣ
//             0001 1010 0000 0000 0000 0000 0000 0000
//             31λ                15λ               0λ
//             ���ĳλ�Ķ�����Ϊ1�����ʾ��Ӧ�������������庬�����±�
//             ����λ����˵��HMI��ʾ
//             0X01000000  ΪCM1��ѹ����     IO  0.0����24V��ѹ
//             0X02000000  ΪCM1��ѹ����     IO  0.1����24V��ѹ
//             0X04000000  ΪCM1�ȱ�������   IO  0.2����24V��ѹ
//             0X08000000  Ϊ����/ȱ�౨��   IO  0.3����24V��ѹ

//             0X10000000  ΪCM2��ѹ����     IO  0.4����24V��ѹ
//             0X20000000  ΪCM2��ѹ����     IO  0.5����24V��ѹ
//             0X40000000  ΪCM2�ȱ�������   IO  0.6����24V��ѹ
//             0X80000000  Ϊ���±���        IO  0.7����24V��ѹ

//             0X00010000  Ϊ�ķ��ȱ�������  IO  1.0����24V��ѹ
//             0X00020000  ΪCM3��ѹ����     IO  1.1����24V��ѹ
//             0X00040000  ΪCM3��ѹ����     IO  1.2����24V��ѹ
//             0X00080000  ΪCM3�ȱ�������   IO  1.3����24V��ѹ

//             0X00100000  Ϊ��������������  IO  1.4����24V��ѹ
//             0X00200000  Ϊ��ʪ����������  IO  1.5����24V��ѹ
//             0X00400000  Ϊ�����ȱˮ����  IO  1.6����24V��ѹ
//             0X00800000  Ϊ��ʪȱˮ����    IO  1.7����24V��ѹ

            PresentValue = ((short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+9] - 0X30) << 12 | (short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+10]- 0X30) << 8
                            |(short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+11]- 0X30) << 4 | (short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+12]- 0X30));//

            MTCI_CTRL::gR_PresentValue_DI32[ChanNo] = MTCI_CTRL::gR_PresentValue_DI32[0];  // �κδ��ڶ�Ĭ��0�ű���
            MTCI_CTRL::gR_PresentValue_DI32[0] = (MTCI_CTRL::gR_CmdBuf[uw16_i+9] - 0X30) << 28 | (MTCI_CTRL::gR_CmdBuf[uw16_i+10]- 0X30) << 24
                                                 |(MTCI_CTRL::gR_CmdBuf[uw16_i+11] - 0X30) << 20 | (MTCI_CTRL::gR_CmdBuf[uw16_i+12]- 0X30) << 16
                                                 |(MTCI_CTRL::gR_CmdBuf[uw16_i+13] - 0X30) << 12 | (MTCI_CTRL::gR_CmdBuf[uw16_i+14]- 0X30) << 8
                                                 |(MTCI_CTRL::gR_CmdBuf[uw16_i+15] - 0X30) << 4  | (MTCI_CTRL::gR_CmdBuf[uw16_i+16]- 0X30);

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // ���½��ճ�ʱʱ��

#if AUX_MULTI_FUNCTION_BOARD == 1
            // �๦�ܰ壺  4��Ϊ�¿���UART �¶ȣ�5��Ϊ�¿���UART ʪ��; 6���¿���UART 16DI;7���¿���UART 16D0;
            for(unsigned char boardIdx = 6; boardIdx<MAX_BOARD_PER_AUXMCU; boardIdx++)
            {
                if(Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_BType.m_AuxType == AUX_DI)
                {
                    unsigned char auxChnIdx = 0;
                    unsigned char auxDI = 0;
                    unsigned char DI_D[4];
                    unsigned long int DI_32Value = gR_PresentValue_DI32[0];
                    DI_32Value = (DI_32Value>>16);
                    DI_D[0] = (DI_32Value>>8)&0X0F;
                    DI_D[1] = (DI_32Value>>12)&0X0F;
                    DI_D[2] = DI_32Value&0X0F;
                    DI_D[3] = (DI_32Value>>4)&0X0F;
                    DI_32Value = DI_D[0]|(DI_D[1]<<4)|(DI_D[2]<<8)|(DI_D[3]<<12);
                    //while(auxChnIdx<Aux_DataManager::m_AllAuxBoards[boardIdx].m_BoardInfo.m_ChannelCount)
                    while(auxChnIdx<16)
                    {
                        auxDI = (unsigned char) ((DI_32Value>>auxChnIdx)&0X00000001);  // ���ø�16λֵ
                        if(auxDI)
                            Aux_DataManager::m_AuxDataBank[boardIdx][auxChnIdx].aux_data = 1;
                        else
                            Aux_DataManager::m_AuxDataBank[boardIdx][auxChnIdx].aux_data = 0;
                        auxChnIdx++;
                    }
                }
            }
#endif

#if(DEBUG_TRACE_RECORD== 1)
            uw32_Count[2]++;
#endif

            return;
        }

        uw16_i++;
    } while(uw16_i < uw16_Total);

}

void TH_S7_226CN::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����д��������,��Ҫ��ڣ������дֵ��д���з�֧
    long Time;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_S7_226CN_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    if(MTCI_CTRL::gW_T_Back[ChanNo] != MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL])
    {
        if(MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL] <= 105)     // �¶�ֵΪ10.5��ȱ���
        {
            MTCI_CTRL::uc_LockModleT[ChanNo] = 1;
            MTCI_CTRL::uc_RunTH[ChanNo] = 0; //  ��һ�������жϷ����¡�ʪ����
        }
        else
        {
            if(MTCI_CTRL::gW_HumiFlag[ChanNo] == 1)    //  �ж��Ƿ����ʪ�ȣ���������ʪģʽ
            {
                MTCI_CTRL::uc_LockModleT[ChanNo] = 0;
                MTCI_CTRL::uc_RunTH[ChanNo] = 1;
            }
            else
            {
                Time = CommonFunc::GetTimeTicks( &MTCI_CTRL::gW_HumiInterval[ChanNo]); //g_SystemTime - gW_HumiInterval[ChanNo]; // �¶����ú��Ƿ���6�������ù�ʪ��
                if(Time < 0)
                {
                    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_HumiInterval[ChanNo]);//MTCI_CTRL::gW_HumiInterval[ChanNo] = g_SystemTime;
                    WriteCmd_Code(ChanNo,uc_Temp4Humi_Flag);
                    return;
                }
                else if(Time < HUMI_INTERVALTIME)
                {
                    WriteCmd_Code(ChanNo,uc_Temp4Humi_Flag);
                    return;
                }
                MTCI_CTRL::uc_LockModleT[ChanNo] = 1;
                MTCI_CTRL::uc_RunTH[ChanNo] = 0; //  ��һ�������жϷ����¡�ʪ����
            }
        }
        MTCI_CTRL::uc_RunFrist[ChanNo] = 0;
        MTCI_CTRL::uc_RunCount[ChanNo] = 0;
        MTCI_CTRL::gW_T_Back[ChanNo] = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];   // �����¡�ʪģʽ�л�
    }

    if(MTCI_CTRL::uc_RunFrist[ChanNo] == 0)
    {
        if(++MTCI_CTRL::uc_RunCount[ChanNo] >= 6)
        {
            MTCI_CTRL::uc_RunFrist[ChanNo] = 1;
        }
        else
        {
            SendRunCmd(ChanNo,uc_Temp4Humi_Flag);  // �����¡�ʪģʽ�л�; //  ��һ�����ڵ��÷����¡�ʪ����
        }
        return;
    }

    WriteCmd_Code(ChanNo,uc_Temp4Humi_Flag);
}

void TH_S7_226CN::WriteCmd_Code(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����д����ֵ����
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_S7_226CN_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteTempCmd_S7_226CN[MbChkSum];

    MTCI_datatypes::ctrl_Value2String_Hex(&MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL],MbBuf+9);
    MTCI_datatypes::ctrl_Value2String_Hex(&MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL],MbBuf+13);

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = (ChanNo + S7_226CN_ASCII_FIRST);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);  // ���Ͷ�������

    //gW_WriteCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
    //gW_WriteCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// �¡�ʪ����Ϊ��ָͬ��ʱ�������־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}


void TH_S7_226CN::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ������������
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_S7_226CN_MAX_COM_RUN_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    if(MTCI_CTRL::uc_RunTH[ChanNo] == 0)    // �����¡�ʪģʽ�л�
    {
        for(MbChkSum = 0; MbChkSum < TH_S7_226CN_MAX_COM_RUN_CMD; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_RunCmd_S7_226CN_T[MbChkSum];
        MTCI_CTRL::uc_RunTH[ChanNo] = 0;
    }
    else
    {
        for(MbChkSum = 0; MbChkSum < TH_S7_226CN_MAX_COM_RUN_CMD; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_RunCmd_S7_226CN_TH[MbChkSum];
        MTCI_CTRL::uc_RunTH[ChanNo] = 1;
    }

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = (ChanNo + S7_226CN_ASCII_FIRST);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
    //gW_RunCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
    //gW_RunCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// �¡�ʪ����Ϊ��ָͬ��ʱ�������־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}

void TH_S7_226CN::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ���Ͷ�����
    static  unsigned char  uc_ReadDI_Count = 0;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_S7_226CN_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < TH_S7_226CN_MAX_COM_READ_CMD; MbChkSum++)
    {
#ifdef COM_TEMP4HUMI_CMD
        if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
        {
            MbBuf[MbChkSum] = uc_COM_ReadHumiCmd_S7_226CN[MbChkSum];
        }
        else
        {
            MbBuf[MbChkSum] = uc_COM_ReadTempCmd_S7_226CN[MbChkSum];
        }
#else
        MbBuf[MbChkSum] = uc_COM_ReadTempCmd_S7_226CN[MbChkSum];
#endif
    }


    if(++uc_ReadDI_Count >= 5)
    {
        uc_ReadDI_Count = 0;
        for(MbChkSum = 0; MbChkSum < TH_S7_226CN_MAX_COM_READ_CMD; MbChkSum++) // ������κ��һ��DI��DIΪ�ϴ�������Ϣ
            MbBuf[MbChkSum] = uc_COM_ReadDICmd_S7_226CN[MbChkSum];
    }


#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = (ChanNo + S7_226CN_ASCII_FIRST);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //gW_ReadCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
    //gW_ReadCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// �¡�ʪ����Ϊ��ָͬ��ʱ�������־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����


#if(DEBUG_TRACE_RECORD== 1)
    //uw32_TimeStamp_Test[uw16_Put] = uw32_TimeStamp;
    //uw16_Put =(uw16_Put+1)%100;
    uw32_Count[0]++;
#endif
}

void TH_S7_226CN::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����ֹͣ����

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_S7_226CN_MAX_COM_STOP_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��


    if(MTCI_CTRL::uc_RunTH[ChanNo])     // �¶�ֵΪ10��ȱ���
    {
        for(MbChkSum = 0; MbChkSum < TH_S7_226CN_MAX_COM_STOP_CMD; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_StopCmd_S7_226CN_TH[MbChkSum];
    }
    else
    {
        for(MbChkSum = 0; MbChkSum < TH_S7_226CN_MAX_COM_STOP_CMD; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_StopCmd_S7_226CN_T[MbChkSum];
    }

    MTCI_CTRL::uc_RunFrist[ChanNo] = 0;
    MTCI_CTRL::uc_RunCount[ChanNo] = 0;
    MTCI_CTRL::uc_ReadCount[ChanNo] = 0;
    MTCI_CTRL::uc_LockModleT[ChanNo] = 0;

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = (ChanNo + S7_226CN_ASCII_FIRST);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //gW_StopCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
    //gW_StopCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// �¡�ʪ����Ϊ��ָͬ��ʱ�������־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}



