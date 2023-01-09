/*****************************************************************************
// �����ļ�      :TH_GUANGZHOU_WUSUO.C    �����¿����и��ִ����ļ�
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
// ��������ϵ�л���ʵ����   ���ܿ����¶����ܿ���ʪ��
// TH_GUANGZHOU_WUSUO  ����ͨ�ŷ�ʽ  ACSII  ���䷽ʽ���޼��� //


// Ĭ�ϳ���ʱ�¶�ֵΪ22���
// Ĭ�ϳ�̬ʱʪ��ֵΪ55%RH

// 1-M)�����¶��������ӻ� (���ֽڲ���)
// {
// Monitor command
// Address Main command Optional parameter Setting data
// Transmission : 1,TEMP,S22.0
// 31 2C 54 45 4D 50 2C 53 32 32 2E 30

// 31 2C 54 45 4D 50 2C 53 XX XX XX XX XX
// (XXXXXXXX�¶�ֵ�ȴ���λ�ٴ���λ)22.0

const unsigned char uc_COM_WriteTempCmd_GUANGZHOU_WUSUO[TH_GUANGZHOU_WUSUO_MAX_COM_WRITE_CMD] = {
    0x31, 0x2C, 0x54, 0x45, 0x4D, 0x50, 0x2C, 0x53, 0x32, 0x32,
    0x2E, 0x30, 0x30
};     // write Master to  Slave cmd  (�����¶�22�ȣ�����22.0)  // ���ø�������ʽ


// Transmission : 1,HUMI,S55
// 31 2C 48 55 4D 49 2C 53 35 35

// 31 2C 48 55 4D 49 2C 53 XX XX XX XX XX
// (YYYYYYYYYʪ��ֵ�ȴ���λ�ٴ���λ)55

const unsigned char uc_COM_WriteHumiCmd_GUANGZHOU_WUSUO[TH_GUANGZHOU_WUSUO_MAX_COM_WRITE_CMD] = {
    0x31, 0x2C, 0x48, 0x55, 0x4D, 0x49, 0x2C, 0x53, 0x35, 0x35,
    0x2E, 0x30, 0x30
};     // write Master to  Slave cmd  (����ʪ��55%������55)


// const unsigned char uc_COM_WriteReply_GUANGZHOU_WUSUO[TH_GUANGZHOU_WUSUO_MAX_COM_WRITE_CMD] = {
// 0x31, 0x2C, 0x54, 0x45, 0x4D, 0x50, 0x2C, 0x53, 0x32, 0x32,
// 0x2E, 0x30, 0x30};     // write Slave to  Master reply

//  2-M)��ȡ�¶��������ӻ�  (���ֽڲ���)
// Monitor command
// Address Main command Optional parameter
// Transmission : 1,TEMP?
// 31 2C 54 45 4D 50 3F

const unsigned char uc_COM_ReadTempCmd_GUANGZHOU_WUSUO[TH_GUANGZHOU_WUSUO_MAX_COM_READ_CMD] = {
    0x31, 0x2C, 0x54, 0x45, 0x4D, 0x50, 0x3F
};    // read Master to  Slave cmd (��ȡ�¶�)


// Response format
// monitored temperature, target temperature, high limit temperature, low limit

//Reply CMD : 101.0,102.0,105.0,-45.0
//31 30 31 2E 30 2C 31 30 32 2E 30 2C 31 30 35 2E 30 2C 2D 34 35 2E 30

const unsigned char uc_COM_ReadReply_GUANGZHOU_WUSUO[TH_GUANGZHOU_WUSUO_MAX_COM_READ_REPLY] = {
    0x31, 0x30, 0x31, 0x2E, 0x30, 0x2C, 0x31, 0x30, 0x32, 0x2E,
    0x30, 0x2C, 0x31, 0x30, 0x35, 0x2E, 0x30, 0x2C, 0x2D, 0x34,
    0x35, 0x2E, 0x30
};   // read Slave to  Master reply  //(�ظ��¶�101.0��)


// Transmission : 1,HUMI?
// 31 2C 48 55 4D 49 3F

const unsigned char uc_COM_ReadHumiCmd_GUANGZHOU_WUSUO[TH_GUANGZHOU_WUSUO_MAX_COM_READ_CMD] = {
    0x31, 0x2C, 0x48, 0x55, 0x4D, 0x49, 0x3F
};    // read Master to  Slave cmd (��ȡʪ��)

//  3-M)Ҫ�����������������ӻ� (���ֽڲ���)
// Setting command
// Address Main command Optional parameter Setting data
// Transmission : 1,MODE,CONSTANT
// 31 2C 4D 4F 44 45 2C 43 4F 4E 53 54 41 4E 54

// Transmission : 1,POWER,ON
// 31 2C 50 4F 57 45 52 2C 4F 4E

const unsigned char uc_COM_RunCmd_GUANGZHOU_WUSUO[TH_GUANGZHOU_WUSUO_MAX_COM_RUN_CMD] =  {
    0x31, 0x2C, 0x50, 0x4F, 0x57, 0x45, 0x52, 0x2C, 0x4F, 0x4E
};    // Run Master to  Slave cmd

// Transmission : 1,MODE,CONSTANT
// 31 2C 4D 4F 44 45 2C 43 4F 4E 53 54 41 4E 54
const unsigned char uc_COM_ModeCmd_GUANGZHOU_WUSUO[TH_GUANGZHOU_WUSUO_MAX_COM_MODE_CMD] =  {
    0x31, 0x2C, 0x4D, 0x4F, 0x44, 0x45, 0x2C, 0x43, 0x4F, 0x4E,
    0x53, 0x54, 0x41, 0x4E, 0x54
};    // Run Master to  Slave cmd

//  4-M)Ҫ��ֹͣ�����������ӻ�   (���ֽڲ���)
// Setting command
// Address Main command Optional parameter Setting data
// Transmission : 1,POWER,OFF
// 31 2C 50 4F 57 45 52 2C 4F 46 46

const unsigned char uc_COM_StopCmd_GUANGZHOU_WUSUO[TH_GUANGZHOU_WUSUO_MAX_COM_STOP_CMD] =  {
    0x31, 0x2C, 0x50, 0x4F, 0x57, 0x45, 0x52, 0x2C, 0x4F, 0x46, 0x46
};    // Run Master to  Slave cmd


//
// TH_GUANGZHOU_WUSUO  ����ͨ�ŷ�ʽ  ASCII  ���䷽ʽ����У�� //
//----------------------------------------------------------------------------------------------------


void TH_GUANGZHOU_WUSUO::Init(unsigned char ChanNo)
{
// ��ʼ������ָ�롣
    unsigned char uc_i; // ��ֹ�����������ȫ����ʼ��һ�κ���ָ�롣
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_GUANGZHOU_WUSUO::SendReadCmd);   //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_GUANGZHOU_WUSUO::SendWriteCmd);  //  1-M)�����¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_GUANGZHOU_WUSUO::SendReadCmd);    //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_GUANGZHOU_WUSUO::SendRunCmd);      //  3-M)Ҫ�����������������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_GUANGZHOU_WUSUO::SendStopCmd);    //  4-M)Ҫ��ֹͣ�����������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_GUANGZHOU_WUSUO::ParseResult);  //  5-S)��ȡ�¶ȴӻ��ظ���������  (���ֽڲ���)

    MTCI_CTRL::uc_RunTH[ChanNo] = 1;
    MTCI_CTRL::uc_RunFrist[ChanNo] = 0;
    MTCI_CTRL::uc_RunCount[ChanNo] = 0;
    MTCI_CTRL::uc_ReadCount[ChanNo] = 0;
    MTCI_CTRL::uc_LockModleT[ChanNo] = 0;
    MTCI_CTRL::gW_T_Back[ChanNo] =  210; // 220
    MTCI_CTRL::gR_PV_Back[ChanNo] =  210; // 220
}

/*----*/
unsigned char TH_GUANGZHOU_WUSUO::SetPresentValue_Check(unsigned char ChanNo, short int Value)
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

void TH_GUANGZHOU_WUSUO::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
// �������յĽ������
    unsigned char *pCmd;
    //unsigned char uc_MoveFlag = 0;
    //bool b_is_temp;
    //bool NoZero;
    unsigned char  Modle = 1;
    short int PresentValue = 0;
    unsigned short int uw16_i;
    //unsigned short int uw16_j;
    unsigned short int uw16_k;
    unsigned int uw16_Total;
    union  Int_data I_Value;
    short int temp = 0;

//     uw16_Total =  uart_bytenum(ChanNo);
    uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

    if( uw16_Total < TH_GUANGZHOU_WUSUO_MIN_RESPONSE_CMD_LEN)
        return;  // �����������ϴ���һ����Ϣ�����ޱ���

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);


    uw16_i = 0;
    uw16_k = 0;
    do {
        /*          Transmission : 1,TEMP?
                    Reply CMD : 23.0,85.0,105.0,-45.0
                    Transmission : 1,HUMI?
                    Reply CMD : 25,85,100,0
        */
        if((MTCI_CTRL::gR_CmdBuf[uw16_i] == ',' )&&(uw16_k == 0))  // ','
        {   // ���ð�ͷ���β�ж�����ֵ��
            uw16_k ++;
            pCmd = &MTCI_CTRL::gR_CmdBuf[0]; //DATA FROM CHAMBER
            if((*pCmd) == 0 )    //  if Null, then give up this reading
                return;
            if( (*pCmd)==' ')	 //	 for temperature less than 100, the first byte will be a space, just skip this space
                pCmd++;

            if ( (*pCmd) == '+' || (*pCmd) == '-' || ( (*pCmd) >= '0' && (*pCmd) <= '9' ) )
            {
                pCmd=MTCI_datatypes::ctrl_String2Value_Signed(pCmd, &PresentValue);

                if(pCmd == NULL )
                    return;
                if(TH_GUANGZHOU_WUSUO::SetPresentValue_Check(ChanNo, PresentValue))
                {
                    MTCI_CTRL::SetPresentValue(ChanNo, PresentValue,uc_Temp4Humi_Flag, FIRST_CHAMBER_CHANNEL);
                }
                CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);
                return;
            }
        }
        uw16_i++;
    } while(uw16_i < uw16_Total);
}

void TH_GUANGZHOU_WUSUO::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����д��������,��Ҫ��ڣ������дֵ��д���з�֧
    long Time;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_GUANGZHOU_WUSUO_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    WriteCmd_Code(ChanNo,uc_Temp4Humi_Flag);
}

void TH_GUANGZHOU_WUSUO::WriteCmd_Code(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����д����ֵ����
    short int  register_data;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_GUANGZHOU_WUSUO_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
    {
        for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_WriteHumiCmd_GUANGZHOU_WUSUO[MbChkSum];
        register_data = MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL];
        //gcvt((double) (MTCI_CTRL::gW_TemperatureValue[ChanNo]/10), 5, MbBuf+12);    // ��С10������
        MTCI_datatypes::ctrl_Value2String_Dot(&register_data,MbBuf + 8);
    }
    else
    {
        for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_WriteTempCmd_GUANGZHOU_WUSUO[MbChkSum];
        register_data = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
        //gcvt((double) (MTCI_CTRL::gW_TemperatureValue[ChanNo]/10), 5, MbBuf+12);    // ��С10������
        MTCI_datatypes::ctrl_Value2String_Dot(&register_data,MbBuf + 8);
    }


#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = (ChanNo + GUANGZHOU_WUSUO_ASCII_FIRST);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);  // ���Ͷ�������

    //gW_WriteCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
    //gW_WriteCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// �¡�ʪ����Ϊ��ָͬ��ʱ�������־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}


void TH_GUANGZHOU_WUSUO::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ������������
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_GUANGZHOU_WUSUO_MAX_COM_RUN_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��


    for(MbChkSum = 0; MbChkSum < TH_GUANGZHOU_WUSUO_MAX_COM_MODE_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_ModeCmd_GUANGZHOU_WUSUO[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = (ChanNo + GUANGZHOU_WUSUO_ASCII_FIRST);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);


    for(MbChkSum = 0; MbChkSum < TH_GUANGZHOU_WUSUO_MAX_COM_RUN_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_RunCmd_GUANGZHOU_WUSUO[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = (ChanNo + GUANGZHOU_WUSUO_ASCII_FIRST);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
    //gW_RunCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
    //gW_RunCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// �¡�ʪ����Ϊ��ָͬ��ʱ�������־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}

void TH_GUANGZHOU_WUSUO::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ���Ͷ�����
    static  unsigned char  uc_ReadDI_Count = 0;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_GUANGZHOU_WUSUO_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < TH_GUANGZHOU_WUSUO_MAX_COM_READ_CMD; MbChkSum++)
    {
#ifdef COM_TEMP4HUMI_CMD
        if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
        {
            MbBuf[MbChkSum] = uc_COM_ReadHumiCmd_GUANGZHOU_WUSUO[MbChkSum];
        }
        else
        {
            MbBuf[MbChkSum] = uc_COM_ReadTempCmd_GUANGZHOU_WUSUO[MbChkSum];
        }
#else
        MbBuf[MbChkSum] = uc_COM_ReadTempCmd_GUANGZHOU_WUSUO[MbChkSum];
#endif
    }

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = (ChanNo + GUANGZHOU_WUSUO_ASCII_FIRST);
    }
#endif

    UARTFunc::uart_empty(ChanNo);
    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //gW_ReadCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
    //gW_ReadCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// �¡�ʪ����Ϊ��ָͬ��ʱ�������־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����


#if(DEBUG_TRACE_RECORD== 1)
    //uw32_TimeStamp_Test[uw16_Put] = uw32_TimeStamp;
    //uw16_Put =(uw16_Put+1)%100;
    uw32_Count[0]++;
#endif
}

void TH_GUANGZHOU_WUSUO::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����ֹͣ����

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_GUANGZHOU_WUSUO_MAX_COM_STOP_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < TH_GUANGZHOU_WUSUO_MAX_COM_STOP_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_GUANGZHOU_WUSUO[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = (ChanNo + GUANGZHOU_WUSUO_ASCII_FIRST);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //gW_StopCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
    //gW_StopCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// �¡�ʪ����Ϊ��ָͬ��ʱ�������־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}



