/*****************************************************************************
// �����ļ�      :TH_CHONGQING_ATAIHE.c    �����¿����и��ִ����ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"

extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function ����ָ��   //��ʽ  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];


//------------------------------------------------------------------------------------------------
//  CHONGQING_ATAIHE(���찢̩������UC8-60125-ES) Temperature Chamber //
//  ����ʽ��ʪ��������    ������ PLC S7-200 SMART
//  Model:UC8-60125-ES
//  ����ͨ�ŷ�ʽ��ASCIIģʽ ���䷽ʽ��ASCIIģʽ ��У��
//  �ر�ע�⣺���к�ֹͣ���������д���һ�¡�
// ���ض�����Ҫ��
// 1)#define  COM_INTERVAL_WRITE_AND_READ                 0         //�Ƿ�ʹ�ô��ڼ��д�Ͷ����� Ϊ0��ʾ��ʹ�ü��д�Ͷ�����
// 2)#define CMD_INTERVALTIME	     (500*10)   // ÿ�η���������ʱ�� 500����

//  Start    '$'                     ASCII code 36  (0X24)
//  End     <CR> Carriage Return     ASCII code 13  (0X0D)

//  1-M)�����¶��������ӻ� (���ַ�����)
// д������Ϣ��ʽ��
// �����ʽ���豸��+���Ժ�+�����ַ�+��ַ+a���ֶη���+����1+��+����N +д������+A����������

// �豸�ţ�4λ�����ַ���0~9999��������0001
// ���Ժţ�4λ�����ַ���0~9999�����̶�Ϊ0001
// �����ַ���
// ��λ��ĸ����:
// ǰ�ַ�Ϊ����ַ����̶�Ϊw
// ���ַ�Ϊ�����ַ���
// ��������Ϊ����a
// ��������Ϊλ���λΪb��
// �ر�λΪc


//  Send string:
//  00010001wa02106a000100550000A00010001wa02108a000100990000A
//  Response string:

const unsigned char uc_COM_WriteCmd_ATAIHE_T[TH_ATAIHE_MAX_COM_WRITE_CMD] =
    "00010001wa02106a000100220000A";     // write Master to  Slave cmd  (�����¶�22��)
const unsigned char uc_COM_WriteCmd_ATAIHE_H[TH_ATAIHE_MAX_COM_WRITE_CMD] =
    "00010001wa02108a000100550000A";     // write Master to  Slave cmd  (����ʪ��55%)

//  �ӻ��ظ�ʱ��Replyһģһ������ֵ��
const unsigned char uc_COM_WriteReply_ATAIHE[TH_ATAIHE_MAX_COM_WRITE_REPLY] =
    "00010001wa02106a000100220000A00010001wa02108a000100550000A";
// write Slave to  Master reply (�ظ������¶�22.0��)

//  2-M)��ȡ�¶��������ӻ�  (���ַ�����)
//  Send string (PC �� controller)
// ��ȡ���
// ���������ʽ��
// �豸��+���Ժ�+�����ַ�+��ַ+a���ֶη���+��ȡ����+A����������
// �豸�ţ�4λ�����ַ���0~9999��������0001
// ���Ժţ�4λ�����ַ���0~9999�����ã�Ŀǰ�̶�Ϊ0001

//  Send string:
//  00010001ra02100a0002A
//  Response string:
//  00010001ra02100a0002     299     600A
//  30 30 30 31 30 30 30 31 72 61 30 32 31 30 30 61 30 30 30 32 20 20 20 20 20 32 39 39 20 20 20 20 20 36 30 30 41

const unsigned char uc_COM_ReadCmd_ATAIHE[TH_ATAIHE_MAX_COM_READ_CMD] =
    "00010001ra02100a0002A";    // read Master to  Slave cmd (��ȡ)

//  �ӻ��ظ�ʱ��Reply
const unsigned char uc_COM_ReadReply_ATAIHE[TH_ATAIHE_MAX_COM_READ_REPLY] =
    "00010001ra02100a0002     220     550A";
// read Slave to  Master reply  //(�ظ��¶�22��)

//  3-M)Ҫ�����������������ӻ�
//  Send string:
//  00010001wb09540000A
//  Response string:
//  00010001wb09540000A

const unsigned char uc_COM_RunCmd_ATAIHE[TH_ATAIHE_MAX_COM_RUN_CMD] =
    "00010001wb09540000A";

//  4-M)Ҫ��ֹͣ�����������ӻ�����
//  Send string:
//  00010001wc09540000A
//  Response string:

const unsigned char uc_COM_StopCmd_ATAIHE[TH_ATAIHE_MAX_COM_STOP_CMD] =
    "00010001wc09540000A";

//
//  CHONGQING_ATAIHE(���찢̩������UC8-60125-ES) Temperature Chamber //
//  ����ʽ��ʪ��������
//  Model:UC8-60125-ES
//----------------------------------------------------------------------------------------------------

void TH_ATAIHE::Init(unsigned char ChanNo)
{
    // ��ʼ������ָ�롣
    unsigned char uc_i; // ��ֹ�����������ȫ����ʼ��һ�κ���ָ�롣
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_ATAIHE::SendReadCmd);   //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_ATAIHE::SendWriteCmd);  //  1-M)�����¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_ATAIHE::SendReadCmd);    //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_ATAIHE::SendRunCmd);      //  3-M)Ҫ�����������������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_ATAIHE::SendStopCmd);    //  4-M)Ҫ��ֹͣ�����������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_ATAIHE::ParseResult);  //  5-S)��ȡ�¶ȴӻ��ظ���������  (���ֽڲ���)
}

void TH_ATAIHE::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
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
    unsigned short int uw16_k,uw16_n,uw16_m;
    unsigned char uc_Buffer[2][8];
    unsigned long int uw32_DO = 0;
    unsigned int uw16_Total = PresentValue;
    uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

    if( uw16_Total < TH_ATAIHE_MIN_RESPONSE_CMD_LEN)
        return;  // �����������ϴ���һ����Ϣ�����ޱ���

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_j = 0;
    uw16_k = 0;
    uc_MoveFlag = 0;
    for(uw16_i = 0; uw16_i < uw16_Total; uw16_i ++)   //   ������ٸ�����������
    {   //  ��������190�ȸ�55�ȡ�  // Temperature range -55��C to +190��C //Resolution 0.1��C or ��F, 0.1% RH, or 0.01 for other linear applications
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == 0X41)  // Ѱ��End ��A�������˶����λ����
        {
            if((uw16_Total - uw16_i) < 37)
                return;  // ����һ���������ټ�顣

            if(MTCI_CTRL::gR_CmdBuf[uw16_i + 37] != 0X41)  // Ѱ��End ��A�������˶����λ����
                continue; // ������������,�����м��������Ļظ���Ϣ���롣���˾Ͷ�ȥ   ���дDIDOֵ��������

            if((MTCI_CTRL::gR_CmdBuf[uw16_i + 9] != 0X72)&&(MTCI_CTRL::gR_CmdBuf[uw16_i + 10] != 0X61))  // Ѱ��'ra'
                continue;

            uc_MoveFlag = 2;
            uw16_j = uw16_i + 8;  //gR_CmdBuf[uw16_i+8];
            uw16_k = uw16_i;
            break;
            //}
        }
    }

    if((uc_MoveFlag < 2)||(uw16_i >= uw16_Total))
        return;   // �Ҳ���������Զ��˳�,��Ϊ�������	 //

    if(MTCI_CTRL::gR_CmdBuf[uw16_k + 37] != 0X41)  // Ѱ��End ��A�������˶����λ����
        return; // ������������,�����м��������Ļظ���Ϣ���롣

    for(uw16_i = 0; uw16_i < 8; uw16_i ++)
    {
        uc_Buffer[0][uw16_i] = 0x20;
        uc_Buffer[1][uw16_i] = 0x20;
    }
    for(uw16_i = 0; uw16_i < 2; uw16_i ++)
    {
        switch(uw16_i)
        {
        case 0:
            uw16_j = uw16_k + TH_ATAIHE_READ_REPLY_POINT_VAlUE_T;     // Actual value of parameter 1 �¶�ʵ��ֵ
            break;                   // 1+7=8     Actual value of parameter 1  Get
        case 1:
            uw16_j = uw16_k + TH_ATAIHE_READ_REPLY_POINT_VAlUE_H;    // Nominal value of parameter 2  ʪ��ʵ��ֵ
            break;                   // 8+7=15    Nominal value of parameter 2 Get
        default:
            uw16_j =uw16_j;
            break;
        }
        pCmd = MTCI_CTRL::String_MoveBuff(&MTCI_CTRL::gR_CmdBuf[uw16_j],&uc_Buffer[uw16_i][0],4); //DATA FROM CHAMBER
        p = pCmd; //DATA FROM CHAMBER

        if( (*pCmd) == 0 )    //  Ϊ���˳���
            continue;

        if ( (*pCmd) == '+' || (*pCmd) == '-' || ( (*pCmd) >= '0' && (*pCmd) <= '9' ) )
        {
            //p=ctrl_String2Value(pCmd, &PresentValue);
            //PresentValue = (short int) (atof(pCmd)*10);  // �Ŵ�10���ϴ�

            //if( p == NULL )
            //	continue;

            switch(uw16_i)
            {
            case 0:
                MTCI_datatypes::ctrl_String2Value_Signed(pCmd, &MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[FIRST_CHAMBER_CHANNEL]);
                MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[FIRST_CHAMBER_CHANNEL] /=10;
                PresentValue = MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[FIRST_CHAMBER_CHANNEL];
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue,SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL);    // Actual value of parameter 1 �¶�ʵ��ֵ
                CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);
                break;

            case 1:
                for(uw16_n = 0; uw16_n < 8; uw16_n ++)
                {
                    if(uc_Buffer[1][uw16_n] == 0x41) // "A"
                    {
                        uc_Buffer[1][uw16_n] = 0x20;
                        for(uw16_m = uw16_n; uw16_m < 8; uw16_m ++)
                            uc_Buffer[1][uw16_m] = 0x20;
                    }
                }
                MTCI_datatypes::ctrl_String2Value_Signed(pCmd, &MTCI_CTRL::str_Uart[ChanNo].gR_PresentHumiValue[FIRST_CHAMBER_CHANNEL]); // Actual value of parameter 2  ʪ��ʵ��ֵ
                MTCI_CTRL::str_Uart[ChanNo].gR_PresentHumiValue[FIRST_CHAMBER_CHANNEL] /=10;
                PresentValue = MTCI_CTRL::str_Uart[ChanNo].gR_PresentHumiValue[FIRST_CHAMBER_CHANNEL];
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue,SETPOINTVALUE_HUMI, FIRST_CHAMBER_CHANNEL);
                break;

            default:
                break;
            }
        }
    }

}


void TH_ATAIHE::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  ����д����ֵ����

    short int  register_data;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_ATAIHE_MAX_COM_WRITE_CMD-1;
    unsigned char MbBuf[SEND_MAX_BUF];
    unsigned long int  uw32_DO;
    //unsigned char XcrcHi,XcrcLo;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

// 	if(MTCI_CTRL::gR_Updata_Flag[ChanNo] == FALSE)
// 		return;

    MbBuf[uw16_len-1] = 'A';  // ���������־
    if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
    {
        for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_WriteCmd_ATAIHE_H[MbChkSum];
        register_data = MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL];
        MTCI_datatypes::ctrl_Value2String_BeforeZero(&register_data,MbBuf + TH_ATAIHE_WRITE_CMD_POINT_VAlUE_H);  // Nominal value of parameter 2
        //gcvt((double) (register_data/10), 6, MbBuf+WRITE_CMD_POINT_VAlUE+6);    // ��С10������
    }
    else
    {
        for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_WriteCmd_ATAIHE_T[MbChkSum];
        register_data = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
        MTCI_datatypes::ctrl_Value2String_BeforeZero(&register_data,MbBuf + TH_ATAIHE_WRITE_CMD_POINT_VAlUE_T);  // Nominal value of parameter 1
        //gcvt((double) (register_data/10), 6, MbBuf+WRITE_CMD_POINT_VAlUE);    // ��С10������
    }

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[2] = (ChanNo + ASCII_FIRST_UART_NUM);
#endif

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
    {
        if(MbBuf[MbChkSum] == 0x2E)  // '.'
        {
            //MbBuf[MbChkSum] = 0x30;  // '0'
            MbBuf[MbChkSum] = MbBuf[MbChkSum+1]; // С����ֵƽ��һλ
            MbBuf[MbChkSum+1] = 0x30;  // '0'
        }
    }

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

}


void TH_ATAIHE::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  ������������
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_ATAIHE_MAX_COM_RUN_CMD-1;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��


    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_RunCmd_ATAIHE[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[2] = (ChanNo + ASCII_FIRST_UART_NUM);
#endif

    MbBuf[uw16_len-1] = 'A';  // ���������־

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

}

void TH_ATAIHE::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  ���Ͷ�����

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_ATAIHE_MAX_COM_READ_CMD-1;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��


    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_ReadCmd_ATAIHE[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[2] = (ChanNo + ASCII_FIRST_UART_NUM);
#endif

    MbBuf[uw16_len-1] = 'A';  // ���������־

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
}

void TH_ATAIHE::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  ����ֹͣ����

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_ATAIHE_MAX_COM_STOP_CMD-1;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��


    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_ATAIHE[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[2] = (ChanNo + ASCII_FIRST_UART_NUM);
#endif

    MbBuf[uw16_len-1] = 'A';  // ���������־

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

}




