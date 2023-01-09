/*****************************************************************************
// �����ļ�      :TC_DONGDA_MEWTO.C    �����¿����и��ִ����ļ�
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

//DONGDA  TCY1023

//------------------------------------------------------------------------------------------------
// T_DONGDA  ����ͨ�ŷ�ʽ  ACSII  ���䷽ʽ���޼��� //
//

// Ĭ�ϳ���ʱ�¶�ֵΪ22���

// 1-M)�����¶��������ӻ� (���ֽڲ���)
// {
// �޸Ķ�ֵ�¶��趨ֵ
// ��ʽ��%01#WDD0601206012DC00**<CR>
// }
// Transmission : %01#WDD0601206012DC00**<CR>
// 25 30 31 23 57 44 44 30 36 30 31 32 30 36 30 31 32 44 43 30 30 2A 2A 0D


const unsigned char uc_COM_WriteTempCmd_DONGDA[TC_DONGDA_MAX_COM_WRITE_CMD] = {
    0x25, 0x30, 0x31, 0x23, 0x57, 0x44, 0x44, 0x30, 0x36, 0x30,
    0x31, 0x32, 0x30, 0x36, 0x30, 0x31, 0x32, 0x44, 0x43, 0X30,
    0x30, 0x2A, 0x2A, 0x0D
};     // write Master to  Slave cmd  (�����¶�22�ȣ�����22.0)  // �ȵͺ��0xDC00;  ʵ��ֵ220



//  2-M)��ȡ�¶��������ӻ�  (���ֽڲ���)
// {
// ��ѯ�Ǳ������״̬��
// %01#RDD0602006020**<CR>
// }
// Transmission : %01#RDD0602006020**<CR>
// 25 30 31 23 52 44 44 30 36 30 32 30 30 36 30 32 30 2A 2A 0D


const unsigned char uc_COM_ReadTempCmd_DONGDA[TC_DONGDA_MAX_COM_READ_CMD] = {
    0x25, 0x30, 0x31, 0x23, 0x52, 0x44, 0x44, 0x30, 0x36, 0x30,
    0x32, 0x30, 0x30, 0x36, 0x30, 0x32, 0x30, 0x2A, 0x2A, 0x0D
};    // read Master to  Slave cmd (��ȡ�¶�)


// Response : %01$RD******
//
// 25 30 31 24 52 44 XX XX XX XX 2A 2A
// (XXXXXXXX�¶�ֵ)
// 25 30 31 24 52 44 33 30 30 30 2A 2A       (30�ȣ�


const unsigned char uc_COM_ReadReply_DONGDA[TC_DONGDA_MAX_COM_READ_REPLY] = {
    0x25, 0x30, 0x31, 0x24, 0x52, 0x44, 0x44, 0x43, 0x30, 0x30,
    0x2A, 0x2A
};   // read Slave to  Master reply  //(�ظ��¶�22�ȣ�����0xDC00)

//  3-M)Ҫ�����������������ӻ� (���ֽڲ���)
// {
// �������У�
// ��ʽ��%01#WDD06000060000100**<CR>
// }
// Transmission : %01#WDD06000060000100**<CR>
// 25 30 31 23 57 44 44 30 36 30 30 30 30 36 30 30 30 30 31 30 30 2A 2A 0D

const unsigned char uc_COM_RunCmd_DONGDA[TC_DONGDA_MAX_COM_RUN_CMD] =  {
    0x25, 0x30, 0x31, 0x23, 0x57, 0x44, 0x44, 0x30, 0x36, 0x30,
    0x30, 0x30, 0x30, 0x36, 0x30, 0x30, 0x30, 0x30, 0x31, 0X30,
    0x30, 0x2A, 0x2A, 0x0D
};    // Run Master to  Slave cmd


//  4-M)Ҫ��ֹͣ�����������ӻ�   (���ֽڲ���)
// {
// ֹͣ���У�
// ��ʽ��%01#WDD06000060000200**<CR>
// }
// Transmission : %01#WDD06000060000200**<CR>
// 25 30 31 23 57 44 44 30 36 30 30 30 30 36 30 30 30 30 32 30 30 2A 2A 0D

const unsigned char uc_COM_StopCmd_DONGDA[TC_DONGDA_MAX_COM_STOP_CMD] =  {
    0x25, 0x30, 0x31, 0x23, 0x57, 0x44, 0x44, 0x30, 0x36, 0x30,
    0x30, 0x30, 0x30, 0x36, 0x30, 0x30, 0x30, 0x30, 0x32, 0X30,
    0x30, 0x2A, 0x2A, 0x0D
};     // Stop Master to  Slave cmd

//
// DONGDA  ����ͨ�ŷ�ʽ  ASCII  ���䷽ʽ��У�� //
//----------------------------------------------------------------------------------------------------


void TC_DONGDA::Init(unsigned char ChanNo)
{
// ��ʼ������ָ�롣
    unsigned char uc_i; // ��ֹ�����������ȫ����ʼ��һ�κ���ָ�롣
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_DONGDA::SendReadCmd);   //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_DONGDA::SendWriteCmd);  //  1-M)�����¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_DONGDA::SendReadCmd);    //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_DONGDA::SendRunCmd);      //  3-M)Ҫ�����������������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_DONGDA::SendStopCmd);    //  4-M)Ҫ��ֹͣ�����������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_DONGDA::ParseResult);  //  5-S)��ȡ�¶ȴӻ��ظ���������  (���ֽڲ���)

    MTCI_CTRL::uc_RunTH[ChanNo] = 1;
    MTCI_CTRL::uc_RunFrist[ChanNo] = 0;
    MTCI_CTRL::uc_RunCount[ChanNo] = 0;
    MTCI_CTRL::uc_ReadCount[ChanNo] = 0;
    MTCI_CTRL::uc_LockModleT[ChanNo] = 0;
    MTCI_CTRL::gW_T_Back[ChanNo] =  210; // 220
    MTCI_CTRL::gR_PV_Back[ChanNo] =  210; // 220
		
    MTCI_CTRL::uw16_PUBLIC_REPEAT_NUM = 1; // PUBLIC_REPEAT_NUM;
    MTCI_CTRL::uw32_CMD_INTERVALTIME = 500*10; //CMD_INTERVALTIME;		
}

/*----*/
unsigned char TC_DONGDA::SetPresentValue_Check(unsigned char ChanNo, short int Value)
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

void TC_DONGDA::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// �������յĽ������
    unsigned char  Modle = 1;
    short int PresentValue = 0;
    unsigned short int uw16_i;
    unsigned int uw16_Total;
    union  Int_data I_Value;
    short int temp = 0;

    uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

    if( uw16_Total < TC_DONGDA_MIN_RESPONSE_CMD_LEN)
        return;  // �����������ϴ���һ����Ϣ�����ޱ���

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_i = 0;
    do {
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == uc_COM_ReadReply_DONGDA[4]
                && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_DONGDA[5])  // ��ȡ�¶�
        {   // ���ð�ͷ���β�ж�����ֵ��
            I_Value.uc_d[3] = MTCI_CTRL::gR_CmdBuf[uw16_i+5];
            I_Value.uc_d[2] = MTCI_CTRL::gR_CmdBuf[uw16_i+4];
            I_Value.uc_d[1] = MTCI_CTRL::gR_CmdBuf[uw16_i+3];
            I_Value.uc_d[0] = MTCI_CTRL::gR_CmdBuf[uw16_i+2];
//             if(I_Value.ul_member > 0X80000000)  // �����ر���
//                 PresentValue = (short int) (-1*(0X100000000 - I_Value.ul_member)/10);
//             else
//                 PresentValue = (short int) (I_Value.ul_member/10);

            MTCI_datatypes::ctrl_String2Value_Hex(&MTCI_CTRL::gR_CmdBuf[uw16_i+2],&PresentValue);
						PresentValue = ((PresentValue>>8)&0x00ff) | ((PresentValue<<8)&0xff00);//ת���ߵ�λ

//             if( TC_DONGDA::SetPresentValue_Check(ChanNo, PresentValue) )
//             {
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue, SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL);
//             }

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // ���½��ճ�ʱʱ��
            return;
        }
        uw16_i++;
    } while(uw16_i < uw16_Total);

}

void TC_DONGDA::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����д��������,��Ҫ��ڣ������дֵ��д���з�֧
    long Time;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TC_DONGDA_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    WriteCmd_Code(ChanNo,uc_Temp4Humi_Flag);
}

void TC_DONGDA::WriteCmd_Code(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����д����ֵ����
    short int  register_data;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TC_DONGDA_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteTempCmd_DONGDA[MbChkSum];
    register_data = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
		register_data = ((register_data>>8)&0x00ff) | ((register_data<<8)&0xff00);//ת���ߵ�λ
    //gcvt((double) (MTCI_CTRL::gW_TemperatureValue[ChanNo]/10), 5, MbBuf+12);    // ��С10������
    MTCI_datatypes::ctrl_Value2String_Hex(&register_data,MbBuf + 17);

//     MbChkSum = MTCI_datatypes::CRC16(MbBuf,21);
//     MbBuf[22] = (unsigned char) (MbChkSum >>8);
//     MbBuf[21] = (unsigned char) (MbChkSum & 255);

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);  // ���Ͷ�������

    //gW_WriteCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
    //gW_WriteCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// �¡�ʪ����Ϊ��ָͬ��ʱ�������־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}


void TC_DONGDA::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ������������
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TC_DONGDA_MAX_COM_RUN_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < TC_DONGDA_MAX_COM_RUN_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_RunCmd_DONGDA[MbChkSum];

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
    //gW_RunCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
    //gW_RunCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// �¡�ʪ����Ϊ��ָͬ��ʱ�������־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}

void TC_DONGDA::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ���Ͷ�����
    static  unsigned char  uc_ReadDI_Count = 0;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TC_DONGDA_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < TC_DONGDA_MAX_COM_READ_CMD; MbChkSum++)
    {
        MbBuf[MbChkSum] = uc_COM_ReadTempCmd_DONGDA[MbChkSum];
    }

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //gW_ReadCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
    //gW_ReadCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// �¡�ʪ����Ϊ��ָͬ��ʱ�������־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}

void TC_DONGDA::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����ֹͣ����
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TC_DONGDA_MAX_COM_STOP_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < TC_DONGDA_MAX_COM_STOP_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_DONGDA[MbChkSum];

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //gW_StopCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
    //gW_StopCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// �¡�ʪ����Ϊ��ָͬ��ʱ�������־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}



