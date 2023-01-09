/*****************************************************************************
// �����ļ�      :TC_JULABO_A45T.C    �����¿����и��ֿ��������ļ�
// �ļ�����      :ΪLPC2468 AUX-Temperature Controller Board����
// ��д����      :2019.09.02
*****************************************************************************/



#include "../../Entry/includes.h"

extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function ����ָ��   //��ʽ  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

// ע������
const unsigned char StartCmd[TC_JULABO_A45T_START_CMD_LEN] = {0x4F, 0x55, 0x54, 0x5F, 0x4D, 0x4F, 0x44, 0x45, 0x5F, 0x30, 0x35, 0x20, 0x31, 0x0D }; // "OUT_MODE_05 1\r";
const unsigned char StopCmd[TC_JULABO_A45T_STOP_CMD_LEN] = {0x4F, 0x55, 0x54, 0x5F, 0x4D, 0x4F, 0x44, 0x45, 0x5F, 0x30, 0x35, 0x20, 0x30, 0x0D }; // "OUT_MODE_05 0\r";
const unsigned char SetTempCmd[TC_JULABO_A45T_SET_TMEP_CMD_LEN] = {0x4F, 0x55, 0x54, 0x5F, 0x53, 0x50, 0x5F, 0x30, 0x30, 0x20, 0x35, 0x35, 0x2E, 0x35, 0x30, 0x0D}; // "OUT_SP_00 55.5\r";	//д�趨�¶�ֵ
const unsigned char ReadTempCmd_Inside[TC_JULABO_A45T_READ_TMEP_CMD_LEN] = {0x49, 0x4E, 0x5F, 0x50, 0x56, 0x5F, 0x30, 0x30, 0x0D}; //"IN_PV_00\r";//���ڲ��¶�
const unsigned char ReadTempCmd_Outside[TC_JULABO_A45T_READ_TMEP_CMD_LEN] = {0x49, 0x4E, 0x5F, 0x50, 0x56, 0x5F, 0x30, 0x32, 0x0D}; //"IN_PV_02\r";//���ⲿ�¶ȣ�����ʪ�����棩
const unsigned char SelectCmd[TC_JULABO_A45T_SELECT_CMD_LEN] = {0x4F, 0x55, 0x54, 0x5F, 0x4D, 0x4F, 0x44, 0x45, 0x5F, 0x30, 0x34, 0x20, 0x30, 0x0D }; // "OUT_MODE_04 0\r";//�̶�Ϊ�ڲ�����ģʽ



void TC_JULABO_A45T::Init(unsigned char ChanNo)
{
    // ��ʼ������ָ�롣
    unsigned char uc_i; // ��ֹ�����������ȫ����ʼ��һ�κ���ָ�롣
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_JULABO_A45T::SendRunCmd);   //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_JULABO_A45T::SendWriteCmd);  //  1-M)�����¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_JULABO_A45T::SendReadCmd);    //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_JULABO_A45T::SendRunCmd);      //  3-M)Ҫ�����������������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_JULABO_A45T::SendStopCmd);    //  4-M)Ҫ��ֹͣ�����������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_JULABO_A45T::Parse);  //  5-S)��ȡ�¶ȴӻ��ظ���������  (���ֽڲ���)

		unsigned char MbBuf[TC_JULABO_A45T_SELECT_CMD_LEN]= {0};

		CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��
    
		memcpy(MbBuf, SelectCmd, TC_JULABO_A45T_SELECT_CMD_LEN);
    UARTFunc::uart_putcmd(ChanNo,MbBuf,TC_JULABO_A45T_SELECT_CMD_LEN);//�����ڲ�����ģʽ����
}

void TC_JULABO_A45T::Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    int PresentValue = 0;
    bool Dir = true;
    if( UARTFunc::uart_bytenum(uc_ChNo) < TC_JULABO_A45T_MIN_RESPONSE_CMD_LEN) //wait until MIN_LEN bytes have been read into the buffer
        return;

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    int Len = UARTFunc::uart_getcmd(uc_ChNo,MTCI_CTRL::gR_CmdBuf);
    if( Len == -1 ) //read feedback to gR_CmdBuf, max reading length given
        return;

    //if(MTCI_CTRL::gR_CmdBuf[Len - 4] != 0x2E )
		if(MTCI_CTRL::gR_CmdBuf[Len - 5] != 0x2E )
        return;

    for(unsigned short int uw16_i = 0; uw16_i < Len; uw16_i++)
    {
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] >= 0X30 && MTCI_CTRL::gR_CmdBuf[uw16_i] <= 0X39)
        {
            PresentValue = PresentValue * 10 + (MTCI_CTRL::gR_CmdBuf[uw16_i] - 0X30);
        }
        //else if(MTCI_CTRL::gR_CmdBuf[uw16_i] == 0x2E || MTCI_CTRL::gR_CmdBuf[uw16_i] == 0x0A)
				else if(MTCI_CTRL::gR_CmdBuf[uw16_i] == 0x2E || MTCI_CTRL::gR_CmdBuf[uw16_i] == 0x0D)
        {
            continue;
        }
        //else if(MTCI_CTRL::gR_CmdBuf[uw16_i] == 0x0D)
				else if(MTCI_CTRL::gR_CmdBuf[uw16_i] == 0x0A)
        {
            break;
        }
        else if(MTCI_CTRL::gR_CmdBuf[uw16_i] == 0x2D && uw16_i == 0)
        {
            Dir = false;
        }
        else
            return;
    }
    if(!Dir)
        PresentValue *= -1;
		PresentValue /= 10;//��Ϊ���ܵ���ֵ�Ǿ�ȷ��С�������λ�ģ���:55�� ����������5500������������С10�� �����������ٴ���С10��
    //MTCI_CTRL::SetPresentValue(uc_ChNo,PresentValue,SETPOINTVALUE_TEMPERATURE);    //KL update present value of the data read from chamber
		MTCI_CTRL::SetPresentValue(uc_ChNo,PresentValue,uc_Temp4Humi_Flag, FIRST_CHAMBER_CHANNEL);    //KL update present value of the data read from chamber
    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[uc_ChNo]);     //update time out stamp
    return;

}

/**************************************************************
*   Send "write" command to temperature chamber.              *
*   In    : n/a                                               *
*   Return: void                                              *
***************************************************************/

void TC_JULABO_A45T::SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
		unsigned char MbBuf2[TC_JULABO_A45T_SET_TMEP_CMD_LEN]= {0};

		CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[uc_ChNo]);  // �Դ��ڷ��������������ʱ��

    memcpy(MbBuf2,SetTempCmd,TC_JULABO_A45T_SET_TMEP_CMD_LEN);

    unsigned char  *p;
    short SetPointValue= MTCI_CTRL::gW_TemperatureValue[uc_ChNo][FIRST_CHAMBER_CHANNEL];//555
    unsigned short Count = 0;

    p = (unsigned char*) &MbBuf2[10];
    Count=MTCI_datatypes::ctrl_Value2String_Dot(&SetPointValue,p);
    p = (unsigned char*)&MbBuf2[10 + Count];
		*p = 0x30;
		p++;
    *p=0x0D; //'\r';

    UARTFunc::uart_putcmd(uc_ChNo,MbBuf2,(TC_JULABO_A45T_SET_TMEP_CMD_LEN));

}


/**************************************************************
*   Send "run" command to temperature chamber.                *
*   In    : n/a                                               *
*   Return: void                                              *
***************************************************************/

void TC_JULABO_A45T::SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char Buf[TC_JULABO_A45T_START_CMD_LEN];

		CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[uc_ChNo]);  // �Դ��ڷ��������������ʱ��
    
		memcpy(Buf, StartCmd, TC_JULABO_A45T_START_CMD_LEN);
    UARTFunc::uart_putcmd(uc_ChNo,Buf,TC_JULABO_A45T_START_CMD_LEN);	//������������

}
/**************************************************************
*   Send "read" command to temperature chamber.               *
*   In    : n/a                                               *
*   Return: void                                              *
***************************************************************/


void TC_JULABO_A45T::SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char Buf[TC_JULABO_A45T_READ_TMEP_CMD_LEN];
	
		CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[uc_ChNo]);  // �Դ��ڷ��������������ʱ��
	
		if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
			memcpy(Buf, ReadTempCmd_Inside, TC_JULABO_A45T_READ_TMEP_CMD_LEN);
		else
			memcpy(Buf, ReadTempCmd_Outside, TC_JULABO_A45T_READ_TMEP_CMD_LEN);
		
    UARTFunc::uart_putcmd(uc_ChNo, Buf, TC_JULABO_A45T_READ_TMEP_CMD_LEN );
}

/**************************************************************
*   Send "stop" command to temperature chamber.               *
*   In : n/a                                                  *
*   Out: void                                                 *
***************************************************************/
void TC_JULABO_A45T::SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char Buf[TC_JULABO_A45T_STOP_CMD_LEN];
	
		CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[uc_ChNo]);  // �Դ��ڷ��������������ʱ��
	
    memcpy(Buf, StopCmd, TC_JULABO_A45T_STOP_CMD_LEN);
    UARTFunc::uart_putcmd(uc_ChNo,Buf,TC_JULABO_A45T_STOP_CMD_LEN);

}

