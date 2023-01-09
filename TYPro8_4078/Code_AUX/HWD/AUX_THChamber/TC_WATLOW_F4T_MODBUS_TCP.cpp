/*****************************************************************************
// �����ļ�      :TC_WATLOW_F4T_MODBUS_TCP.C    �����¿����и��ֿ��������ļ�
// �ļ�����      :ΪLPC2468 AUX-Temperature Controller Board����
// ��д����      :2011.12.01
*****************************************************************************/



#include "../../Entry/includes.h"

extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function ����ָ��   //��ʽ  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];


void TC_WATLOW_F4T_MODBUS_TCP::Init(unsigned char ChanNo)
{
    // ��ʼ������ָ�롣
    unsigned char uc_i; // ��ֹ�����������ȫ����ʼ��һ�κ���ָ�롣
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_WATLOW_F4T_MODBUS_TCP::SendRunCmd);   //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_WATLOW_F4T_MODBUS_TCP::SendWriteCmd);  //  1-M)�����¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_WATLOW_F4T_MODBUS_TCP::SendReadCmd);    //  2-M)��ȡ�¶��������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_WATLOW_F4T_MODBUS_TCP::SendRunCmd);      //  3-M)Ҫ�����������������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_WATLOW_F4T_MODBUS_TCP::SendStopCmd);    //  4-M)Ҫ��ֹͣ�����������ӻ�����  (���ֽڲ���)
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_WATLOW_F4T_MODBUS_TCP::Parse);  //  5-S)��ȡ�¶ȴӻ��ظ���������  (���ֽڲ���)

    MTCI_CTRL::uw16_PUBLIC_REPEAT_NUM = 1; // PUBLIC_REPEAT_NUM;
    MTCI_CTRL::uw32_CMD_INTERVALTIME = 1000*10; //CMD_INTERVALTIME;
}

/***************************************************************
*   Parse the feedback from temperature chamber                *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/
void TC_WATLOW_F4T_MODBUS_TCP::Parse(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    short int PresentValue = 0;
    unsigned short int uw16_i = 0;

    if( UARTFunc::uart_bytenum(uc_ChNo) < TC_WATLOW_F4T_MODBUS_TCP_MIN_RESPONSE_CMD_LEN) //wait until MIN_LEN bytes have been read into the buffer
        return;

		memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    UARTFunc::uart_getcmd(uc_ChNo,MTCI_CTRL::gR_CmdBuf);
		
    do
    {
        if(MTCI_CTRL::gR_CmdBuf[uw16_i] == 0x01 && MTCI_CTRL::gR_CmdBuf[uw16_i+1] == 0x03 && MTCI_CTRL::gR_CmdBuf[uw16_i+2] == 0x02) // && gR_CmdBuf[uw16_i+7] == 0x00)  2009.07.25     //only parse the read cmd with 2 bytes data
        {
            PresentValue = ((short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+3]) << 8 | (short int)(MTCI_CTRL::gR_CmdBuf[uw16_i+4]) );     //KL combined
						PresentValue = (((PresentValue/10) - 32)/1.8)*10;
					
            MTCI_CTRL::SetPresentValue(uc_ChNo,PresentValue,SETPOINTVALUE_TEMPERATURE,FIRST_CHAMBER_CHANNEL);    //KL update present value of the data read from chamber

            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[uc_ChNo]);     //update time out stamp
            return;
        }

        uw16_i++;
    } while(uw16_i < TC_WATLOW_F4T_MODBUS_TCP_MIN_RESPONSE_CMD_LEN);
}

/**************************************************************
*   Send "write" command to temperature chamber.              *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TC_WATLOW_F4T_MODBUS_TCP::SendWriteCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char SendBuf[12] = {0x00,0x00,0x00,0x00,0x00,0x06,0x01,0x06,0x01,0x2C,0x00,0xDC};//д�¶�
    short int  register_data = 0;
    register_data = MTCI_CTRL::gW_TemperatureValue[uc_ChNo][FIRST_CHAMBER_CHANNEL];

    SendBuf[10] = (unsigned char) (register_data >> 8);
    SendBuf[11] = (unsigned char) (register_data & 255);
    UARTFunc::uart_putcmd(uc_ChNo,SendBuf,12);

    //MTCI_CTRL::gW_WriteCmd_Flag[uc_ChNo] = SETPOINTVALUE_MAX;   // ���¿��Ʊ���ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}


/**************************************************************
*   Send "run" command to temperature chamber.                *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TC_WATLOW_F4T_MODBUS_TCP::SendRunCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
// #if(TC_WATLOW_F4T_MODBUS_TCP_INPUT_CH == TC_WATLOW_F4T_MODBUS_TCP_INPUT_THIRD)
//     unsigned char CmdBuf[SEND_MAX_BUF] = "\x01\x03\x00\x6C\x00\x01\x44\x17";
//     UARTFunc::uart_putcmd(uc_ChNo,CmdBuf,8);      // 20150324  YY
// #else
//     unsigned char CmdBuf[SEND_MAX_BUF] = "\x01\x03\x00\x64\x00\x01\xC5\xD5";
//     UARTFunc::uart_putcmd(uc_ChNo,CmdBuf,8);
// #endif

//     //MTCI_CTRL::gW_RunCmd_Flag[uc_ChNo] = SETPOINTVALUE_MAX;  // ���¿��Ʊ���ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}

/**************************************************************
*   Send "read" command to temperature chamber.               *
*   In    : n/a                                                   *
*   Return: void                                                  *
***************************************************************/

void TC_WATLOW_F4T_MODBUS_TCP::SendReadCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned char SendBuf[12] = {0x00,0x00,0x00,0x00,0x00,0x06,0x01,0x03,0x00,0x64,0x00,0x01};//���¶�
    UARTFunc::uart_putcmd(uc_ChNo,SendBuf,12);
    //MTCI_CTRL::gW_ReadCmd_Flag[uc_ChNo] = SETPOINTVALUE_MAX;// ���¿��Ʊ���ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}

/**************************************************************
*   Send "stop" command to temperature chamber.               *
*   In : n/a                                                   *
*   Out: void                                                  *
***************************************************************/
void TC_WATLOW_F4T_MODBUS_TCP::SendStopCmd(unsigned char uc_ChNo,unsigned char uc_Temp4Humi_Flag)
{
//     unsigned char slave_addr = 0x01;
//     unsigned char hbspr300 = 0x01;
//     unsigned char lbspr300 = 0x2C;
//     unsigned int  MbChkSum;
//     unsigned char MbBuf[SEND_MAX_BUF];

//     //     #if(TC_WATLOW_F4T_MODBUS_TCP_INPUT_CH == TC_WATLOW_F4T_MODBUS_TCP_INPUT_THIRD)
//     //     hbspr300 = 0x02;                   // 20150324  YY
//     //     lbspr300 = 0x72;
//     //     #else
//     hbspr300 = 0x01;
//     lbspr300 = 0x2C;
//     //     #endif

//     MbBuf[0] = slave_addr;
//     MbBuf[1] = 0x06;
//     MbBuf[2] = hbspr300;
//     MbBuf[3] = lbspr300;
//     MbBuf[4] = 0x00;
//     MbBuf[5] = 0xDC;

// #if(TC_WATLOW_F4T_MODBUS_TCP_INPUT_CH == TC_WATLOW_F4T_MODBUS_TCP_INPUT_THIRD)
//     MbBuf[4] = 0x00;                // 20150324  YY
//     MbBuf[5] = 0x03;
// #endif

//     MbChkSum = MTCI_datatypes::CRC16(MbBuf,6);
//     MbBuf[6] = (unsigned char) (MbChkSum >> 8);
//     MbBuf[7] = (unsigned char) (MbChkSum & 255);

//     UARTFunc::uart_putcmd(uc_ChNo,MbBuf,8);

    //MTCI_CTRL::gW_StopCmd_Flag[uc_ChNo] = SETPOINTVALUE_MAX;// ���¿��Ʊ���ͬʱ�ñ�־�Խ����¡�ʪ�ֱ��͵�ָ��ϵͳ����
}

