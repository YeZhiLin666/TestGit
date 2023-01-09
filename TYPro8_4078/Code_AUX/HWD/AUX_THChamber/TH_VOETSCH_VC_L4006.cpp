/*****************************************************************************
// �����ļ�      :TH_VOETSCH_VC_L4006.c    �����¿����и��ִ����ļ�
// �ļ�����      :ΪLPC2468 AUX-MPU board����
// ��д����      :2010.12.01
*****************************************************************************/

#include "../../Entry/includes.h"

extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function ����ָ��   //��ʽ  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];


//------------------------------------------------------------------------------------------------
//  Voetsch(�¹�WEISS-VOETSCH���������豸���޹�˾) Temperature Chamber //
//  MCU2468AUX  4RS232�� PN��502314		ASCIIģʽ
//  ����ͨ�ŷ�ʽ��ASCIIģʽ ���䷽ʽ��ASCII-2ģʽ ��У��
//  �ر�ע�⣺���к�ֹͣ���������д����һ�¡�


//  Start    '$'                     ASCII code 36  (0X24)
//  End     <CR> Carriage Return     ASCII code 13  (0X0D)

//  1-M)�����¶��������ӻ� (���ַ�����)
//  Send string (PC �� controller)
//  Set nominal values for parameters
//  The following string sets the nominal value for parameter 1 to 25 and the nominal value for
//  parameter 2 to 50 and starts operation.
//  The string reads as follows:
//  $01E 0023.0 0050.0 0080.0 0000.0 0000.0 0000.0 0000.0 01101010101010101010101010101010 <CR>

//  $01E
//  0023.0 Nominal value of parameter 1
//  0050.0 Nominal value of parameter 2
//  011
//  0 :Unused digital output 0   // ����Ҫ�������޷���ȫ������һ����Ͳ����лظ���
//  1 :digital output 1   // ����ֵ1��ʾ����;0��ʾֹͣ��
//  digital output 2
//  01010101010101010101010101010 Other digital outputs
//  <CR>

//  Send string:
//  $01E 0023.0 0050.0 0080.0 0000.0 0000.0 0000.0 0000.0 01101010101010101010101010101010 <CR>
//  Response string:

const unsigned char uc_COM_WriteCmd_VOETSCH_L4006[TH_VOETSCH_L4006_MAX_COM_WRITE_CMD] =
    "$01E 0022.0 0050.0 01101010101010101010101010101010";     // write Master to  Slave cmd  (�����¶�22��)

//  �ӻ��ظ�ʱ��Replyһģһ������ֵ��
const unsigned char uc_COM_WriteReply_VOETSCH_L4006[TH_VOETSCH_L4006_MAX_COM_WRITE_REPLY] =
    "-020.0 0022.0 0050.0 0020.0 0100.0 1200.0 0000.0 00000000000000000000000000000000";
// write Slave to  Master reply (�ظ������¶�22.0��)

//  2-M)��ȡ�¶��������ӻ�  (���ַ�����)
//  Send string (PC �� controller)
//  The string reads as follows: $01I<CR>
//  $
//  01 Bus address of the unit
//  I Request to send the actual status
//  <CR>

//  Response string (Controller �� PC) for a unit with 2 parameters
//  The following string contains nominal and actual values for temperature and humidity2)
//  together with various other parameters.
//  The values are instantaneous values:
//  The string reads as follows:
//  0023.0 0020.5 0050.0 0041.0 0080.0 0080.0 0000.0 0020.0 0000.0 0020.2 0000.0
//  0020.3 0000.0 0020.4 01101010101010101010101010101010 <CR>

//  0023.0 Nominal value of parameter 1
//  0020.5 Actual value of parameter 1
//  0050.0 Nominal value of parameter 2
//  0041.0 Actual value of parameter 2
//  0080.0 Set value 1
//  0080.0 Set value 1
//  0000.0 Not used
//  0020.0 Actual value Pt100-1 (��C, analogue I/O card) 1)
//  0000.0 Not used
//  0020.2 Actual value Pt100-2 (��C, analogue I/O card) 1)
//  0000.0 Not used
//  0020.3 Actual value Pt100-3 (��C, analogue I/O card) 1)
//  0000.0 Not used
//  0020.4 Actual value Pt100-4 (��C, analogue I/O card) 1)
//  011
//  0 :Unused digital output 0   // ����Ҫ�������޷���ȫ������һ����Ͳ����лظ���
//  1 :digital output 1   // ����ֵ1��ʾ����;0��ʾֹͣ��
//  digital output 2
//  01010101010101010101010101010 Other digital outputs
//  <CR>

//  Send string:
//  $01I<CR>
//  Response string:
//  0023.0 0020.5 0050.0 0041.0 0080.0 0080.0 0000.0 0020.0 0000.0 0020.2 0000.0
//  0020.3 0000.0 0020.4 01101010101010101010101010101010 <CR>

const unsigned char uc_COM_ReadCmd_VOETSCH_L4006[TH_VOETSCH_L4006_MAX_COM_READ_CMD] = "$01I";    // read Master to  Slave cmd (��ȡ)

//  �ӻ��ظ�ʱ��Reply
const unsigned char uc_COM_ReadReply_VOETSCH_L4006[TH_VOETSCH_L4006_MAX_COM_READ_REPLY] =
    "0023.0 0020.5 0050.0 0041.0 01101010101010101010101010101010";
// read Slave to  Master reply  //(�ظ��¶�22��)

//  3-M)Ҫ�����������������ӻ�
//  (��ȡ�¶�)
//  Send string:
//  $01I<CR>
//  Response string:
//  0023.0 0020.5 0050.0 0041.0 0080.0 0080.0 0000.0 0020.0 0000.0 0020.2 0000.0
//  0020.3 0000.0 0020.4 01101010101010101010101010101010 <CR>

const unsigned char uc_COM_RunCmd_VOETSCH_L4006[TH_VOETSCH_L4006_MAX_COM_RUN_CMD] = "$01I";    // read Master to  Slave cmd (��ȡ)

//  4-M)Ҫ��ֹͣ�����������ӻ�����
//  (�����¶�22�ȣ�����220)
//  0 :Unused digital output 0   // ����Ҫ�������޷���ȫ������һ����Ͳ����лظ���
//  1 :digital output 1   // ����ֵ1��ʾ����;0��ʾֹͣ��
//  Send string:
//  $01E 0022.0 0050.0 0080.0 0000.0 0000.0 0000.0 0000.0 00101010101010101010101010101010 <CR>
//  Response string:

const unsigned char uc_COM_StopCmd_VOETSCH_L4006[TH_VOETSCH_L4006_MAX_COM_STOP_CMD] =
    "$01E 0022.0 0050.0 00101010101010101010101010101010";    // write Master to  Slave cmd  (�����¶�22��)


//  0 :Unused digital output 0   // ����Ҫ�������޷���ȫ������һ����Ͳ����лظ���
//  1 :digital output 1   // ����ֵ1��ʾ����;0��ʾֹͣ��
// ͣ��д��һ�¡�

//
//  Voetsch(�¹�WEISS-VOETSCH���������豸���޹�˾) Temperature Chamber //
//----------------------------------------------------------------------------------------------------

void TH_VOETSCH_L4006::Init(unsigned char ChanNo)
{
    // ��ʼ������ָ�롣
    unsigned char uc_i; // ��ֹ�����������ȫ����ʼ��һ�κ���ָ�롣
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TH_VOETSCH_L4006::SendReadCmd);   //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TH_VOETSCH_L4006::SendWriteCmd);  //  1-M)�����¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TH_VOETSCH_L4006::SendReadCmd);    //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TH_VOETSCH_L4006::SendRunCmd);      //  3-M)Ҫ�����������������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TH_VOETSCH_L4006::SendStopCmd);    //  4-M)Ҫ��ֹͣ�����������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TH_VOETSCH_L4006::ParseResult);  //  5-S)��ȡ�¶ȴӻ��ظ���������  (���ֽڲ���)
}

void TH_VOETSCH_L4006::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
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
    unsigned long int uw32_DO = 0;
    unsigned int uw16_Total = PresentValue;
    uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

    if( uw16_Total < TH_VOETSCH_L4006_MIN_RESPONSE_CMD_LEN)
        return;  // �����������ϴ���һ����Ϣ�����ޱ���

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_j = 0;
    uw16_k = 0;
    uc_MoveFlag = 0;
    for(uw16_i = 0; uw16_i < uw16_Total; uw16_i ++)   //   ������ٸ�����������
    {   //  ��������190�ȸ�55�ȡ�  // Temperature range -55��C to +190��C //Resolution 0.1��C or ��F, 0.1% RH, or 0.01 for other linear applications
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == 0X0D)  // Ѱ��End <CR>�����˶����λ����
        {
            //  "0023.0 0020.5 0050.0 0041.0 0080.0 0080.0 0000.0 0020.0 0000.0 0020.2 0000.0 0020.3 0000.0 0020.4 01101010101010101010101010101010"
            //  14(Value)*6(����)+32(DIDO)+14(�ո�0X20)+1(0X0D)=131�ֽ�
            //  ��8λ��ʼ 0022.0 Actual value of parameter 1
            // 1         Nominal value of parameter 1 Set  (Value 1)
            // 1+7=8     Actual value of parameter 1  Get  (Value 2)
            // 8+7=15    Nominal value of parameter 2 Set  (Value 3)
            // 15+7=22   Actual value of parameter 2  Get  (Value 4)

            if((uw16_Total - uw16_i) < 61)
                return;  // ����һ���������ټ�顣

            if(MTCI_CTRL::gR_CmdBuf[uw16_i + 61] != 0X0D)  // Ѱ��End <CR>�����˶����λ����
                continue; // ������������,�����м��������Ļظ���Ϣ���롣���˾Ͷ�ȥ   ���дDIDOֵ��������

            //if(uw16_i < 14*READ_REPLY_POINT_VAlUE)  //  û��������   ������ٸ�����������
            //{
            //	continue;
            //}
            //else
            //{
            uc_MoveFlag = 2;
            uw16_j = uw16_i + 8;  //gR_CmdBuf[uw16_i+8];
            uw16_k = uw16_i;
            break;
            //}
        }
    }

    if((uc_MoveFlag < 2)||(uw16_i >= uw16_Total)
            ||MTCI_CTRL::gR_CmdBuf[uw16_k+1] == '$')
        return;   // �Ҳ���������Զ��˳�,��Ϊ�������	 //

    if(MTCI_CTRL::gR_CmdBuf[uw16_k + 61] != 0X0D)  // Ѱ��End <CR>�����˶����λ����
        return; // ������������,�����м��������Ļظ���Ϣ���롣���˾Ͷ�ȥ    ���дDIDOֵ��������

    for(uw16_i = 0; uw16_i < 4; uw16_i ++)
    {
        switch(uw16_i)
        {
        case 0:
            uw16_j = uw16_k + 8;     // Actual value of parameter 1 �¶�ʵ��ֵ
            break;                   // 1+7=8     Actual value of parameter 1  Get
        case 1:
            uw16_j = uw16_k + 15;    // Nominal value of parameter 2  ʪ���趨ֵ
            break;                   // 8+7=15    Nominal value of parameter 2 Set
        case 2:
            uw16_j = uw16_k + 22;    // Actual value of parameter 2  ʪ��ʵ��ֵ
            break;                   // 22+7=29   Set value 1
        default:
            uw16_j =uw16_j;
            break;
        }
        pCmd = MTCI_CTRL::String_MoveBuff(&MTCI_CTRL::gR_CmdBuf[uw16_j],&uc_Buffer[uw16_i][0],TH_VOETSCH_L4006_READ_REPLY_POINT_VAlUE); //DATA FROM CHAMBER
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
                PresentValue = MTCI_CTRL::str_Uart[ChanNo].gR_PresentTempValue[FIRST_CHAMBER_CHANNEL];
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue,SETPOINTVALUE_TEMPERATURE, FIRST_CHAMBER_CHANNEL);    // Actual value of parameter 1 �¶�ʵ��ֵ
                CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);
                break;

            case 1:
                if(MTCI_CTRL::gR_Updata_Flag[ChanNo] == FALSE)
                {
                    MTCI_datatypes::ctrl_String2Value_Signed(pCmd, &MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL]);      //  ���¿�������
                    PresentValue = MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL];
                }
                break;

            case 2:
                MTCI_datatypes::ctrl_String2Value_Signed(pCmd, &MTCI_CTRL::str_Uart[ChanNo].gR_PresentHumiValue[FIRST_CHAMBER_CHANNEL]); // Actual value of parameter 2  ʪ��ʵ��ֵ
                PresentValue = MTCI_CTRL::str_Uart[ChanNo].gR_PresentHumiValue[FIRST_CHAMBER_CHANNEL];
                MTCI_CTRL::SetPresentValue(ChanNo, PresentValue,SETPOINTVALUE_HUMI, FIRST_CHAMBER_CHANNEL);
                break;

            case 3:  //if(gR_Updata_Counter[ChanNo] > 3)
                // {
                // gR_Updata_Counter[ChanNo] = 0;
                MTCI_CTRL::gR_Updata_Flag[ChanNo] = TRUE;

                MTCI_datatypes::ctrl_String2Value_Hex(pCmd, &MTCI_CTRL::gR_PresentValue_Set1[ChanNo]); //    Set value 1
                PresentValue = MTCI_CTRL::gR_PresentValue_Set1[ChanNo];

                //gR_PresentValue_Set1[ChanNo] = PresentValue;    //    Set value 1

                //ctrl_SendWriteCmd(ChanNo);   // USE TO TEST
                //ctrl_SendStopCmd(ChanNo);  // USE TO TEST


                uw32_DO = 0;
                for(uw16_j = 0; uw16_j < 30; uw16_j ++)   //  ��֡ȡǰһ֡��DIDO
                {
                    //  0 :Unused digital output 0   // ����Ҫ�������޷���ȫ������һ����Ͳ����лظ���
                    //  1 :digital output 1   // ����ֵ1��ʾ����;0��ʾֹͣ��
                    if(MTCI_CTRL::gR_CmdBuf[uw16_k + 130 - uw16_j] == '1') //Unused digital output 0��1��2��Other digital outputs
                    {   //Unused digital output 0��1��2��Other digital outputs
                        uw32_DO |= (1<<uw16_j);  // ��1     // ȡ��һ��
                    }
                    else
                    {
                        uw32_DO &= ~(1<<uw16_j);  // ��0
                    }
                }
                //gR_PV_DO32[ChanNo] = uw32_DO; // ȡ��DIDOͨ����Ϣֵ��

                if(uw32_DO != MTCI_CTRL::gR_PV_DO32[ChanNo])
                    MTCI_CTRL::gR_PV_DO32[ChanNo] = uw32_DO; // ȡ��DIDOͨ����Ϣֵ��

                MTCI_CTRL::gR_PresentValue_DI32[0] = MTCI_CTRL::gR_PV_DO32[ChanNo];


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
                        DI_D[0] = (DI_32Value>>12)&0X0F;
                        DI_D[1] = (DI_32Value>>8)&0X0F;
                        DI_D[2] = (DI_32Value>>4)&0X0F;
                        DI_D[3] = (DI_32Value>>0)&0X0F;
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

                //}
                //else
                //  gR_Updata_Counter[ChanNo]++ ;
                break;

            default:
                break;
            }

            //if(gR_Updata_Flag[ChanNo] == TRUE)
            //	return;  //break;   // ����һ��ȡ����ʪ��,DIDO.
        }
    }

}


void TH_VOETSCH_L4006::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  ����д����ֵ����

    short int  register_data;
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_VOETSCH_L4006_MAX_COM_WRITE_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];
    unsigned long int  uw32_DO;
    //unsigned char XcrcHi,XcrcLo;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��

// 	if(MTCI_CTRL::gR_Updata_Flag[ChanNo] == FALSE)
// 		return;



    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_WriteCmd_VOETSCH_L4006[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[2] = (ChanNo + ASCII_FIRST_UART_NUM);
#endif

    MbBuf[uw16_len-1] = 0X0D;  // ���������־<CR>
    register_data = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
    //MbBuf[WRITE_CMD_POINT_VAlUE] = (unsigned char) (register_data >> 8);
    //MbBuf[WRITE_CMD_POINT_VAlUE+1] = (unsigned char) (register_data & 255);

    MTCI_datatypes::ctrl_Value2String_BeforeZero(&register_data,MbBuf + TH_VOETSCH_L4006_WRITE_CMD_POINT_VAlUE + 1);  // Nominal value of parameter 1
    //gcvt((double) (register_data/10), 6, MbBuf+WRITE_CMD_POINT_VAlUE);    // ��С10������

    register_data = MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL];
    MTCI_datatypes::ctrl_Value2String_BeforeZero(&register_data,MbBuf + TH_VOETSCH_L4006_WRITE_CMD_POINT_VAlUE + 8);  // Nominal value of parameter 2
    //gcvt((double) (register_data/10), 6, MbBuf+WRITE_CMD_POINT_VAlUE+6);    // ��С10������

// 	register_data = MTCI_CTRL::gR_PresentValue_Set1[ChanNo];
// 	MTCI_datatypes::ctrl_Value2String_BeforeZero(&register_data,MbBuf + TH_VOETSCH_L4006_WRITE_CMD_POINT_VAlUE + 15);  // Set value 1
    //gcvt((double) (register_data/10), 6, MbBuf+WRITE_CMD_POINT_VAlUE+12);    // ��С10������

    for(MbChkSum = 0; MbChkSum < 30; MbChkSum++)
    {
        //  0 :Unused digital output 0   // ����Ҫ�������޷���ȫ������һ����Ͳ����лظ���
        //  1 :digital output 1   // ����ֵ1��ʾ����;0��ʾֹͣ��
        uw32_DO = MTCI_CTRL::gR_PV_DO32[ChanNo];    // Unused digital output 0,1,2, Other digital outputs
        uw32_DO = (uw32_DO>>MbChkSum) & 0X00000001;
        if(uw32_DO == 1) //Unused digital output 0��1��2��Other digital outputs
        {
            MbBuf[TH_VOETSCH_L4006_MAX_COM_WRITE_CMD-2-MbChkSum]= '1';  // ��1
        }
        else
        {
            MbBuf[TH_VOETSCH_L4006_MAX_COM_WRITE_CMD-2-MbChkSum]= '0';  // ��0
        }
    }

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::gW_WriteCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}


void TH_VOETSCH_L4006::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  ������������
    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_VOETSCH_L4006_MAX_COM_RUN_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��


    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_RunCmd_VOETSCH_L4006[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[2] = (ChanNo + ASCII_FIRST_UART_NUM);
#endif

    MbBuf[uw16_len-1] = 0X0D;  // ���������־<CR>
    //uart_putcmd(ChanNo,"\x01\x03\x00\x64\x00\x01\xC5\xD5");
    //uart_putcmd(ChanNo,uc_COM_ReadCmd,MAX_COM_READ_CMD); // ʵ���ϸ����Ͷ�����һ���Ƕ�ȡ�¶�����
    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::gW_RunCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}

void TH_VOETSCH_L4006::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  ���Ͷ�����

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_VOETSCH_L4006_MAX_COM_READ_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��


    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_ReadCmd_VOETSCH_L4006[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[2] = (ChanNo + ASCII_FIRST_UART_NUM);
#endif

    MbBuf[uw16_len-1] = 0X0D;  // ���������־<CR>

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::gW_ReadCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}

void TH_VOETSCH_L4006::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //  ����ֹͣ����

    unsigned int  MbChkSum;
    unsigned int  uw16_len = TH_VOETSCH_L4006_MAX_COM_STOP_CMD;
    unsigned char MbBuf[SEND_MAX_BUF];
    unsigned long int  uw32_DO;

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // �Դ��ڷ��������������ʱ��


    for(MbChkSum = 0; MbChkSum < uw16_len; MbChkSum++)
        MbBuf[MbChkSum] = uc_COM_StopCmd_VOETSCH_L4006[MbChkSum];

#ifdef COM_CMD_CHANNEL_DIFFER
    if( ChanNo != 0 )
        MbBuf[2] = (ChanNo + ASCII_FIRST_UART_NUM);
#endif

    MbBuf[uw16_len-1] = 0X0D;  // ���������־<CR>
    //MbChkSum = CRC16(MbBuf,MAX_COM_WRITE_CMD-2);
    //MbBuf[MAX_COM_WRITE_CMD-2] = (unsigned char) (MbChkSum >> 8);
    //MbBuf[MAX_COM_WRITE_CMD-1] = (unsigned char) (MbChkSum & 255);

    MTCI_datatypes::ctrl_Value2String_BeforeZero(&MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL],MbBuf+TH_VOETSCH_L4006_WRITE_CMD_POINT_VAlUE+8);  // Nominal value of parameter 2
    //gcvt((double) (register_data/10), 6, MbBuf+WRITE_CMD_POINT_VAlUE+6);    // ��С10������

    MTCI_datatypes::ctrl_Value2String_BeforeZero(&MTCI_CTRL::gR_PresentValue_Set1[ChanNo],MbBuf+TH_VOETSCH_L4006_WRITE_CMD_POINT_VAlUE+15);  // Set value 1
    //gcvt((double) (register_data/10), 6, MbBuf+WRITE_CMD_POINT_VAlUE+12);    // ��С10������

    for(MbChkSum = 0; MbChkSum < 30; MbChkSum++)
    {
        //  0 :Unused digital output 0   // ����Ҫ�������޷���ȫ������һ����Ͳ����лظ���
        //  1 :digital output 1   // ����ֵ1��ʾ����;0��ʾֹͣ��
        uw32_DO = MTCI_CTRL::gR_PV_DO32[ChanNo];    // Unused digital output 0,1,2, Other digital outputs
        uw32_DO = (uw32_DO>>MbChkSum) & 0X00000001;
        if(uw32_DO == 1) //Unused digital output 0��1��2��Other digital outputs
        {
            MbBuf[TH_VOETSCH_L4006_MAX_COM_WRITE_CMD-2-MbChkSum]= '1';  // ��1
        }
        else
        {
            MbBuf[TH_VOETSCH_L4006_MAX_COM_WRITE_CMD-2-MbChkSum]= '0';  // ��0
        }
    }

    UARTFunc::uart_putcmd(ChanNo,MbBuf,uw16_len);

    //MTCI_CTRL::MTCI_CTRL::gW_StopCmd_Flag[ChanNo] = SETPOINTVALUE_MAX;// �¡�ʪ����ͬΪһָ��ʱ����ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}




