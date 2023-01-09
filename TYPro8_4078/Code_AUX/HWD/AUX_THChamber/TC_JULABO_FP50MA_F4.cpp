/*****************************************************************************
// �����ļ�      :TC_JULABO_FP50MA_F4.C    �����¿����и��ֿ��������ļ�
// �ļ�����      :ΪLPC2468 AUX-Temperature Controller Board����
// ��д����      :2018.10.09
*****************************************************************************/



#include "../../Entry/includes.h"

extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function ����ָ��   //��ʽ  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

// ע������
// #define  COM_INTERVAL_WRITE_AND_READ                 0         //�Ƿ�ʹ�ô��ڼ��д�Ͷ����� Ϊ0��ʾ��ʹ�ü��д�Ͷ�����
const unsigned char StartCmd[START_CMD_LEN] = {0x6F, 0x75, 0x74, 0x5F, 0x6D, 0x6F, 0x64, 0x65, 0x5F, 0x30, 0x35, 0x20, 0x31, 0x0D }; // "out_mode_05 1\r";
const unsigned char SelectCmd[SELECT_CMD_LEN] = {0x6F, 0x75, 0x74, 0x5F, 0x6D, 0x6F, 0x64, 0x65, 0x5F, 0x30, 0x31, 0x20, 0x30, 0x0D}; // "out_mode_01 0\r";
const unsigned char SetTempCmd[SET_TMEP_CMD_LEN] = {0x6F, 0x75, 0x74, 0x5F, 0x73, 0x70, 0x5F, 0x30, 0x30, 0x20, 0x35, 0x35, 0x2E, 0x35, 0x0D }; // "out_sp_00 55.5\r";
const unsigned char ReadTempCmd[READ_TMEP_CMD_LEN] = {0x69, 0x6E, 0x5F, 0x70, 0x76, 0x5F, 0x30, 0x30, 0x0D}; //"in_pv_00\r";
//{0x69, 0x6E, 0x5F, 0x73, 0x70, 0x5F, 0x30, 0x30, 0x0D};//"in_sp_00\r";
const unsigned char StopCmd[STOP_CMD_LEN] = {0x6F, 0x75, 0x74, 0x5F, 0x6D, 0x6F, 0x64, 0x65, 0x5F, 0x30, 0x35, 0x20, 0x30, 0x0D}; // "out_mode_05 0\r";

void TC_JULABO_FP50MA_F4::Init(unsigned char ChanNo)
{
    // ��ʼ������ָ�롣
    unsigned char uc_i; // ��ֹ�����������ȫ����ʼ��һ�κ���ָ�롣
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_JULABO_FP50MA_F4::SendRunCmd);   //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_JULABO_FP50MA_F4::SendWriteCmd);  //  1-M)�����¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_JULABO_FP50MA_F4::SendReadCmd);    //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_JULABO_FP50MA_F4::SendRunCmd);      //  3-M)Ҫ�����������������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_JULABO_FP50MA_F4::SendStopCmd);    //  4-M)Ҫ��ֹͣ�����������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_JULABO_FP50MA_F4::Parse);  //  5-S)��ȡ�¶ȴӻ��ظ���������  (���ֽڲ���)
}

void TC_JULABO_FP50MA_F4::Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    short int PresentValue = 0;
    bool Dir = true;
    if( UARTFunc::uart_bytenum(uc_ChNo) < TC_JULABO_FP50MA_F4_MIN_RESPONSE_CMD_LEN) //wait until MIN_LEN bytes have been read into the buffer
        return;

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    int Len = UARTFunc::uart_getcmd(uc_ChNo,MTCI_CTRL::gR_CmdBuf);
    if( Len == -1 ) //read feedback to gR_CmdBuf, max reading length given
        return;

    if(MTCI_CTRL::gR_CmdBuf[Len - 4] != 0x2E )// (MTCI_CTRL::gR_CmdBuf[Len - 3] != '.'
        return;

    for(unsigned short int uw16_i = 0; uw16_i < Len; uw16_i++)
    {
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] >= 0X30 && MTCI_CTRL::gR_CmdBuf[uw16_i] <= 0X39)    //MTCI_CTRL::gR_CmdBuf[uw16_i] >= '0' && MTCI_CTRL::gR_CmdBuf[uw16_i] <= '9'
        {
            PresentValue = PresentValue * 10 + (MTCI_CTRL::gR_CmdBuf[uw16_i] - 0X30);
        }
        else if(MTCI_CTRL::gR_CmdBuf[uw16_i] == 0x2E || MTCI_CTRL::gR_CmdBuf[uw16_i] == 0x0A)
        {
            continue;
        }
        else if(MTCI_CTRL::gR_CmdBuf[uw16_i] == 0x0D)
        {
            break;
        }
        else if(MTCI_CTRL::gR_CmdBuf[uw16_i] == 0x2D && uw16_i == 0)
        {
            //PresentValue = -1;
            Dir = false;
        }
        else
            return;
    }
    if(!Dir)
        PresentValue *= -1;
    MTCI_CTRL::SetPresentValue(uc_ChNo,PresentValue,SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL);    //KL update present value of the data read from chamber

//     if(MTCI_datatypes::ctrl_String2Value_Signed(MTCI_CTRL::gR_CmdBuf, &PresentValue) != NULL)
//         MTCI_CTRL::SetPresentValue(uc_ChNo,PresentValue,SETPOINTVALUE_TEMPERATURE);    //KL update present value of the data read from chamber

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[uc_ChNo]);     //update time out stamp
    return;

}

/**************************************************************
*   Send "write" command to temperature chamber.              *
*   In    : n/a                                               *
*   Return: void                                              *
***************************************************************/

void TC_JULABO_FP50MA_F4::SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char MbBuf[SELECT_CMD_LEN]= {0};
    //short int  register_data;
    //unsigned int  MbChkSum;

    memcpy(MbBuf, SelectCmd, SELECT_CMD_LEN);
    UARTFunc::uart_putcmd(uc_ChNo,MbBuf,SELECT_CMD_LEN);

    //MTCI_CTRL::gW_WriteCmd_Flag[uc_ChNo] = SETPOINTVALUE_MAX;

    unsigned char MbBuf2[SET_TMEP_CMD_LEN]= {0};
    memcpy(MbBuf2,SetTempCmd,SET_TMEP_CMD_LEN);

    unsigned char  *p;
    //float SetPointValue=temperature;
    short SetPointValue= MTCI_CTRL::gW_TemperatureValue[uc_ChNo][FIRST_CHAMBER_CHANNEL];//555
    unsigned short Count = 0;

    p = (unsigned char*) &MbBuf2[10];
    Count=MTCI_datatypes::ctrl_Value2String_Dot(&SetPointValue,p);
    p = (unsigned char*)&MbBuf2[10 + Count];
    *p=0x0D; //'\r';
//  	p = (unsigned char*)&MbBuf2[0];

    UARTFunc::uart_putcmd(uc_ChNo,MbBuf2,(SET_TMEP_CMD_LEN));

    //MTCI_CTRL::gW_WriteCmd_Flag[uc_ChNo] = SETPOINTVALUE_TEMPERATURE;//SETPOINTVALUE_MAX;

}


/**************************************************************
*   Send "run" command to temperature chamber.                *
*   In    : n/a                                               *
*   Return: void                                              *
***************************************************************/

void TC_JULABO_FP50MA_F4::SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char Buf[START_CMD_LEN];
    memcpy(Buf, StartCmd, START_CMD_LEN);
    UARTFunc::uart_putcmd(uc_ChNo,Buf,START_CMD_LEN);
    //MTCI_CTRL::gW_RunCmd_Flag[uc_ChNo] = uc_Temp4Humi_Flag;//SETPOINTVALUE_TEMPERATURE;//SETPOINTVALUE_MAX;
}
/**************************************************************
*   Send "read" command to temperature chamber.               *
*   In    : n/a                                               *
*   Return: void                                              *
***************************************************************/


void TC_JULABO_FP50MA_F4::SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char Buf[READ_TMEP_CMD_LEN];
    memcpy(Buf, ReadTempCmd, READ_TMEP_CMD_LEN);
    UARTFunc::uart_putcmd(uc_ChNo, Buf, READ_TMEP_CMD_LEN );
    //MTCI_CTRL::gW_ReadCmd_Flag[uc_ChNo] = uc_Temp4Humi_Flag;//SETPOINTVALUE_TEMPERATURE;//SETPOINTVALUE_MAX;
}

/**************************************************************
*   Send "stop" command to temperature chamber.               *
*   In : n/a                                                  *
*   Out: void                                                 *
***************************************************************/
void TC_JULABO_FP50MA_F4::SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char Buf[STOP_CMD_LEN];
    memcpy(Buf, StopCmd, STOP_CMD_LEN);
    UARTFunc::uart_putcmd(uc_ChNo,Buf,STOP_CMD_LEN);
    //MTCI_CTRL::gW_StopCmd_Flag[uc_ChNo] = uc_Temp4Humi_Flag;//SETPOINTVALUE_TEMPERATURE;//SETPOINTVALUE_MAX;

}

