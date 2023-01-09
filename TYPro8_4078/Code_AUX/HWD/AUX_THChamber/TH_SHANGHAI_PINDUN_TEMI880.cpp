/*****************************************************************************
// �����ļ�      :TH_SHANGHAI_PINDUN_TEMI880.C    �����¿����и��ִ����ļ�
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

// 1-M)�����¶��������ӻ� (���ֽڲ���)
// {
// �޸Ķ�ֵ�¶��趨ֵ
// ��ʽ��E01#600#478#�޸�ֵ#A
// �޸Ķ�ֵʪ���趨ֵ
// ��ʽ��E01#600#479#�޸�ֵ#A
// }
// Transmission : E01#600#478#22.00#A
// 45 30 31 23 36 30 30 23 34 37 38 23 32 32 2E 30 30 23 41

// 45 30 31 23 36 30 30 23 34 37 38 23 XX XX XX XX XX 23 41
// (XXXXXXXX�¶�ֵ�ȴ���λ�ٴ���λ)22.00

const unsigned char uc_COM_WriteTempCmd_SHANGHAI_PINDUN_TEMI880[TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_WRITE_CMD] = {
    0x45, 0x30, 0x31, 0x23, 0x36, 0x30, 0x30, 0x23, 0x34, 0x37,
    0x38, 0x23, 0x32, 0x32, 0x2E, 0x30, 0x30, 0x30, 0x23, 0X41
};     // write Master to  Slave cmd  (�����¶�22�ȣ�����22.00)  // ���ø�������ʽ


// Transmission : E01#600#479#55.00#A
// 45 30 31 23 36 30 30 23 34 37 39 23 35 35 2E 30 30 23 41

// 45 30 31 23 36 30 30 23 34 37 39 23 YY YY YY YY YY 23 41
// (YYYYYYYYYʪ��ֵ�ȴ���λ�ٴ���λ)55.00

const unsigned char uc_COM_WriteHumiCmd_SHANGHAI_PINDUN_TEMI880[TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_WRITE_CMD] = {
    0x45, 0x30, 0x31, 0x23, 0x36, 0x30, 0x30, 0x23, 0x34, 0x37,
    0x39, 0x23, 0x35, 0x35, 0x2E, 0x30, 0x30, 0x30, 0x23, 0X41
};     // write Master to  Slave cmd  (����ʪ��55%������55.00)  // ���ø�������ʽ


// const unsigned char uc_COM_WriteReply_SHANGHAI_PINDUN_TEMI880[TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_WRITE_CMD] = {
// 0x45, 0x30, 0x31, 0x23, 0x36, 0x30, 0x30, 0x23, 0x34, 0x37,
// 0x38, 0x23, 0x32, 0x32, 0x2E, 0x30, 0x30, 0x30, 0x23, 0X41};     // write Slave to  Master reply

//  2-M)��ȡ�¶��������ӻ�  (���ֽڲ���)
// {
// ��ѯ�Ǳ������״̬��
// S0199A ����S01#99#A
// ע��[01]���Ǳ�ĵ�ַ [99]�ǲ�ѯָ��
// }
// Transmission : S0199A
// 53 30 31 39 39 41

// Transmission : S01#99#A
// 53 30 31 23 39 39 23 41

const unsigned char uc_COM_ReadTempCmd_SHANGHAI_PINDUN_TEMI880[TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_READ_CMD] = {
    0x53, 0x30, 0x31, 0x39, 0x39, 0x41
};    // read Master to  Slave cmd (��ȡ�¶�)


// �ӻ��ظ�ʱ��
// Reply CMD:  {
// �Ǳ���	[��ַ(1B)][ID(1B)][����״̬(1B)][��ǰ�¶�(4B)][��ǰ�¶��趨ֵ(4B)] [��ǰʪ��(4B)][��ǰʪ���趨ֵ(4B)][У����1 (1B)] [У����2 (1B)]
// ���ľ���	�������Ǳ��ص����ݣ���16������ʾ
// 01 63 02 1B 18 00 00 7C 15 00 00 E8 1E 00 00 88 13 00 00 CB CB
// ���Ľ���
// 01 ���Ǳ�ĵ�ַ
// 63 ��ID����ʶ������ݰ��Ǻ��ָ�ʽ
// 02 ����״̬ 0:��ʽֹͣ1:��ʽ����2:��ֵֹͣ3:��ֵ����

// 1B 18 00 00 �ǵ�ǰ�¶�ֵ,

// ��ԭ�㷨���£����µ�����16����
// �¶�ֵ=(00 * 1000000 + 00*10000 +18 * 100 + 1B)/64
// =(1800+1B)/64
//  (181B/64=3D  =61) (181B%64=47  =71)
// �����:61.71
// (181B=6171)
// ���磺���¶�Ϊ����ʱ,��������ԭ�������ķ���
// ���λΪ1��ʾΪ���� ��60 F0 FF FF��
// <1>ԭ����60 F0 FF FF    (ʵΪ��-4000)
// <2>ȡ��: 9F 0F 00 00
// <3>���: (00 * 1000000 + 00*10000 +0F * 100 + 9F)=F9F
// <4>��1��(F9F=3999)  3999 + 1 = 4000
// <5>ȡ��-4000
// <6>ת���ɵ�����:-4000/100.0  = -40.0

// 7C 15 00 00 �ǵ�ǰ�¶��趨ֵ,

// ��ԭ�㷨���£����µ�����16����
// �¶��趨ֵ=(00 * 1000000 + 00*10000 +15 * 100 + 7C)/64
// �����:55.00

// E8 1E 00 00 �ǵ�ǰʪ��,

// ��ԭ�㷨���£����µ�����16����
// ʪ��=(00 * 1000000 + 00*10000 +1E * 100 + E8)/64
// 1EE8=7912   7912/100=79.12
// �����:79.12
// �����:78.22

// 88 13 00 00 �ǵ�ǰʪ���趨ֵ,

// ��ԭ�㷨���£����µ�����16����
// ʪ���趨ֵ=(00 * 1000000 + 00*10000 +13 * 100 + 88)/64
// �����:50.00

// ������ǵ����Ǳ�ʪ�ȵ��趨����ʾֵҲ�ᷢ���������þ����ˡ�
// CB CB     �Ǳ������ݵ�У���룬����ֵ��ͬ������ʹ�õ����ڶ�����
// }

// Response : 01 63 02 98 08 00 00 98 08 00 00 7C 15 00 00 7C 15 00 00 C8 C8
//
// 01 63 02 XX XX XX XX 98 08 00 00 YY YY YY YY 7C 15 00 00 C8 C8
// (XXXXXXXX�¶�ֵ�ȴ���λ�ٴ���λ; YYYYYYYYYʪ��ֵ�ȴ���λ�ٴ���λ)
// ʹ��full������ʽʱ����ʾֵ=�¶�ֵ/100;    �磺22(2200/100=898/A)��,55(5500/100=157C/A)%ʪ��Humi.
//  [�������ַ�ʽ: (45��*100)  4500 = 0X1194; (-60��*100) -6000 = 0XFFFFE890 (0x100000000-0xFFFFE890=0X1770 = 6000)]

const unsigned char uc_COM_ReadReply_SHANGHAI_PINDUN_TEMI880[TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_READ_REPLY] = {
    0x01, 0x63, 0x02, 0x98, 0x08, 0x30, 0x30, 0x98, 0x08, 0x30,
    0x30, 0x7C, 0x15, 0x30, 0x30, 0x7C, 0x15, 0x30, 0x30, 0xC8,
    0xC8
};   // read Slave to  Master reply  //(�ظ��¶�22�ȣ�����2200;ʪ��55%,����5500)

//  3-M)Ҫ�����������������ӻ� (���ֽڲ���)
// {
// �������У�
// ��ʽ��E01#99#1#A
// ˵����[01]���Ǳ�ĵ�ַ [99]�ǲ�ѯָ��  [1]��ʾ��������
// }
// Transmission : E01#99#1#A
// 45 30 31 23 39 39 23 31 23 41

const unsigned char uc_COM_RunCmd_SHANGHAI_PINDUN_TEMI880_T[TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_RUN_CMD] =  {
    0x45, 0x30, 0x31, 0x23, 0x39, 0x39, 0x23, 0x31, 0x23, 0x41
};    // Run Master to  Slave cmd


//  4-M)Ҫ��ֹͣ�����������ӻ�   (���ֽڲ���)
// {
// ֹͣ���У�
// ��ʽ��E01#99#2#A
// ˵����[01]���Ǳ�ĵ�ַ [99]�ǲ�ѯָ��  [2]��ʾֹͣ����
// }
// Transmission : E01#99#2#A
// 45 30 31 23 39 39 23 32 23 41

const unsigned char uc_COM_StopCmd_SHANGHAI_PINDUN_TEMI880_T[TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_STOP_CMD] =  {
    0x45, 0x30, 0x31, 0x23, 0x39, 0x39, 0x23, 0x32, 0x23, 0x41
};     // Stop Master to  Slave cmd


//
// TH_SHANGHAI_PINDUN_TEMI880  ����ͨ�ŷ�ʽ  ASCII  ���䷽ʽ��У�� //
//----------------------------------------------------------------------------------------------------


void TH_SHANGHAI_PINDUN_TEMI880::Init(unsigned char ChanNo)
{
// ��ʼ������ָ�롣
    unsigned char uc_i; // ��ֹ�����������ȫ����ʼ��һ�κ���ָ�롣
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_SHANGHAI_PINDUN_TEMI880::SendReadCmd);   //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_SHANGHAI_PINDUN_TEMI880::SendWriteCmd);  //  1-M)�����¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_SHANGHAI_PINDUN_TEMI880::SendReadCmd);    //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_SHANGHAI_PINDUN_TEMI880::SendRunCmd);      //  3-M)Ҫ�����������������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_SHANGHAI_PINDUN_TEMI880::SendStopCmd);    //  4-M)Ҫ��ֹͣ�����������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_SHANGHAI_PINDUN_TEMI880::ParseResult);  //  5-S)��ȡ�¶ȴӻ��ظ���������  (���ֽڲ���)

    MTCI_CTRL::uc_RunTH[ChanNo] = 1;
    MTCI_CTRL::uc_RunFrist[ChanNo] = 0;
    MTCI_CTRL::uc_RunCount[ChanNo] = 0;
    MTCI_CTRL::uc_ReadCount[ChanNo] = 0;
    MTCI_CTRL::uc_LockModleT[ChanNo] = 0;
    MTCI_CTRL::gW_T_Back[ChanNo] =  210; // 220
    MTCI_CTRL::gR_PV_Back[ChanNo] =  210; // 220
}

/*----*/
unsigned char TH_SHANGHAI_PINDUN_TEMI880::SetPresentValue_Check(unsigned char ChanNo, short int Value)
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

void TH_SHANGHAI_PINDUN_TEMI880::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
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
    union  Int_data I_Value;
    short int temp = 0;

//     uw16_Total =  uart_bytenum(ChanNo);
    uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

    if( uw16_Total < TH_SHANGHAI_PINDUN_TEMI880_MIN_RESPONSE_CMD_LEN)
        return;  // �����������ϴ���һ����Ϣ�����ޱ���

    memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_i = 0;
    do {
#ifdef COM_CMD_CHANNEL_DIFFER
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == (ChanNo+SHANGHAI_PINDUN_TEMI880_ASCII_FIRST)
                && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_SHANGHAI_PINDUN_TEMI880[1]
                && MTCI_CTRL::gR_CmdBuf[uw16_i+19] == MTCI_CTRL::gR_CmdBuf[uw16_i+20])
#else
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == uc_COM_ReadReply_SHANGHAI_PINDUN_TEMI880[0]
                && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == uc_COM_ReadReply_SHANGHAI_PINDUN_TEMI880[1]
                && MTCI_CTRL::gR_CmdBuf[uw16_i+19] == MTCI_CTRL::gR_CmdBuf[uw16_i+20])  // ��ȡ�¶ȡ���ȡʪ��
#endif
        {   // ���ð�ͷ���β�ж�����ֵ��
// 	       if(gW_ReadRepeatNum[ChanNo] < TC_SHANGHAI_PINDUN_TEMI880_MIN_RESPONSE_CMD_NUM)
// 			   return;	// �����¶Ȼ�ʪ�Ƚ����������ʱ,���´���ֵ��

            I_Value.uc_d[3] = MTCI_CTRL::gR_CmdBuf[uw16_i+6];
            I_Value.uc_d[2] = MTCI_CTRL::gR_CmdBuf[uw16_i+5];
            I_Value.uc_d[1] = MTCI_CTRL::gR_CmdBuf[uw16_i+4];
            I_Value.uc_d[0] = MTCI_CTRL::gR_CmdBuf[uw16_i+3];
            if(I_Value.ul_member > 0X80000000)  // �����ر���
                PresentValue = (short int) (-1*(0X100000000 - I_Value.ul_member)/10);
            else
                PresentValue = (short int) (I_Value.ul_member/10);

            if( TH_SHANGHAI_PINDUN_TEMI880::SetPresentValue_Check(ChanNo, PresentValue) )
            {
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue, SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL);

                I_Value.uc_d[3] = MTCI_CTRL::gR_CmdBuf[uw16_i+14];
                I_Value.uc_d[2] = MTCI_CTRL::gR_CmdBuf[uw16_i+13];
                I_Value.uc_d[1] = MTCI_CTRL::gR_CmdBuf[uw16_i+12];
                I_Value.uc_d[0] = MTCI_CTRL::gR_CmdBuf[uw16_i+11];
                if(I_Value.ul_member > 0X80000000)  // �����ر���
                    PresentValue = (short int) (-1*(0X100000000 - I_Value.ul_member)/10);
                else
                    PresentValue = (short int) (I_Value.ul_member/10);
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue, SETPOINTVALUE_HUMI, FIRST_CHAMBER_CHANNEL);
            }

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);  // ���½��ճ�ʱʱ��

//            if((MTCI_CTRL::gC_IsRunning[ChanNo])&&(MTCI_CTRL::uc_LockModleT[ChanNo]==0))   // ��ʽ�������ÿ���ֵ��δ�����¶�ģʽ
//            {
//                temp = abs(MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue-MTCI_CTRL::gR_PV_Back[ChanNo]);  //  ������Χ-+0.5�� 10.5~9.5
//                if(temp < 1)   // ���仯����Ϊ��������
//                {
//                    MTCI_CTRL::uc_ReadCount[ChanNo] = 0;
//                    return;
//                }
//
//                if(MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue <= (100+5))     // �¶�ֵΪ10.5��ȱ���
//                    Modle = 0; //  ��һ�������жϷ����¡�ʪ����
//                else if(MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue >= (90+5))
//                    Modle = 1;
//
//                if(++MTCI_CTRL::uc_ReadCount[ChanNo] >= 10)
//                {
//                    MTCI_CTRL::gR_PV_Back[ChanNo] = MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue;
//                    if(MTCI_CTRL::uc_RunTH[ChanNo] != Modle)
//                    {
//                        MTCI_CTRL::uc_RunTH[ChanNo] = Modle;
//                        MTCI_CTRL::uc_RunFrist[ChanNo] = 0;   // �����¡�ʪģʽ�л�
//                        MTCI_CTRL::uc_RunCount[ChanNo] = 0;
//                    }
//                    MTCI_CTRL::uc_ReadCount[ChanNo] = 0;
//                }
//            }

            return;
        }

        uw16_i++;
    } while(uw16_i < uw16_Total);

}

void TH_SHANGHAI_PINDUN_TEMI880::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����д��������,��Ҫ��ڣ������дֵ��д���з�֧
    long Time;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    WriteCmd_Code(ChanNo,uc_Temp4Humi_Flag);
}

void TH_SHANGHAI_PINDUN_TEMI880::WriteCmd_Code(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����д����ֵ����
    short int  register_data;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
    {
        for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_WriteHumiCmd_SHANGHAI_PINDUN_TEMI880[MbChkSum];
        register_data = MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL];
        //gcvt((double) (MTCI_CTRL::gW_TemperatureValue[ChanNo]/10), 5, MbBuf+12);    // ��С10������
        MTCI_datatypes::ctrl_Value2String_Dot(&register_data,MbBuf + 12);
    }
    else
    {
        for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
            MbBuf[MbChkSum] = uc_COM_WriteTempCmd_SHANGHAI_PINDUN_TEMI880[MbChkSum];
        register_data = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
        //gcvt((double) (MTCI_CTRL::gW_TemperatureValue[ChanNo]/10), 5, MbBuf+12);    // ��С10������
        MTCI_datatypes::ctrl_Value2String_Dot(&register_data,MbBuf + 12);
    }


#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = (ChanNo + SHANGHAI_PINDUN_TEMI880_ASCII_FIRST);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);  // ���Ͷ�������

    //gW_WriteCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
    //gW_WriteCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// �¡�ʪ����Ϊ��ָͬ��ʱ�������־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}


void TH_SHANGHAI_PINDUN_TEMI880::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ������������
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_RUN_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_RUN_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_RunCmd_SHANGHAI_PINDUN_TEMI880_T[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = (ChanNo + SHANGHAI_PINDUN_TEMI880_ASCII_FIRST);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);
    //gW_RunCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
    //gW_RunCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// �¡�ʪ����Ϊ��ָͬ��ʱ�������־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}

void TH_SHANGHAI_PINDUN_TEMI880::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ���Ͷ�����
    static  unsigned char  uc_ReadDI_Count = 0;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_READ_CMD; MbChkSum++)
    {
        MbBuf[MbChkSum] = uc_COM_ReadTempCmd_SHANGHAI_PINDUN_TEMI880[MbChkSum];
    }

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = (ChanNo + SHANGHAI_PINDUN_TEMI880_ASCII_FIRST);
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

void TH_SHANGHAI_PINDUN_TEMI880::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
//  ����ֹͣ����

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_STOP_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

    for(MbChkSum = 0; MbChkSum < TH_SHANGHAI_PINDUN_TEMI880_MAX_COM_STOP_CMD; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_SHANGHAI_PINDUN_TEMI880_T[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
    {
        MbBuf[2] = (ChanNo + SHANGHAI_PINDUN_TEMI880_ASCII_FIRST);
    }
#endif

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //gW_StopCmd_Flag[ChanNo] = (SETPOINTVALUE_TEMPERATURE|SETPOINTVALUE_HUMI);// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
    //gW_StopCmd_Flag[ChanNo] = uc_Temp4Humi_Flag;// �¡�ʪ����Ϊ��ָͬ��ʱ�������־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}



