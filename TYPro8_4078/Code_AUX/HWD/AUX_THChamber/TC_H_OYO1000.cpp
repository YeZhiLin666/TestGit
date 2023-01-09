/*****************************************************************************
// Program files      	:TC_H_OYO1000.cpp
// File description     :为LPC2468 AUX-MPU board服务
// Write the date      	:2022.10.10
*****************************************************************************/
#include "includes.h"

extern struct FUNC {
    My_Func_Ptr_Ext1 Function;		//function 函数指针   //格式  void function(void);
} st_SendCMD[MAX_UART_NUM][MAX_COM_CMD_TOTAL];

void TC_H_OYO1000::Init(unsigned char ChanNo)
{
// Initializes the function pointer
    unsigned char uc_i; // To prevent panic situations, initialize all function pointers once.
    for(uc_i = 0; uc_i < MAX_COM_CMD_TOTAL; uc_i++)
    {
        st_SendCMD[ChanNo][uc_i].Function = (&TC_H_OYO1000::SendReadCmd);   //  2-M)Read temperature master-to-slave commands (byte operation)
    }

    st_SendCMD[ChanNo][SEND_COM_WRITE_CMD].Function = (&TC_H_OYO1000::SendWriteCmd);  //  1-M)Set temperature master-to-slave command (byte operation)
    st_SendCMD[ChanNo][SEND_COM_READ_CMD].Function = (&TC_H_OYO1000::SendReadCmd);    //  2-M)Read temperature master-to-slave commands (byte operation)
    st_SendCMD[ChanNo][SEND_COM_RUN_CMD].Function = (&TC_H_OYO1000::SendRunCmd);      //  3-M)Set the Run command master-to-slave command
    st_SendCMD[ChanNo][SEND_COM_STOP_CMD].Function = (&TC_H_OYO1000::SendStopCmd);    //  4-M)Set the Stop command master-to-slave command
    st_SendCMD[ChanNo][RECEIVE_COM_READ_CMD].Function = (&TC_H_OYO1000::ParseResult);  // 5-S)Read the temperature slave-to-master command
}

/***************************************************************
**   Parse the feedback from temperature chamber             ***
**   Input : n/a                                             ***
**   Return: void                                            ***
****************************************************************/
void TC_H_OYO1000::ParseResult(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    // Analyze the received result data
    short int PresentValue = 0;
    unsigned short int uw16_i;
    unsigned int uw16_Total;
    uw16_Total =  UARTFunc::uart_bytenum(ChanNo);

    if( uw16_Total < TC_H_OYO1000_MIN_RESPONSE_CMD_LEN )
        return;  // Collect more than one piece of information for processing once, and the lower limit is protected

    memset(MTCI_CTRL::gR_CmdBuf,0,sizeof(MTCI_CTRL::gR_CmdBuf));
    uw16_Total = UARTFunc::uart_getcmd(ChanNo,MTCI_CTRL::gR_CmdBuf);

    uw16_i = 0;
    do
    {
        if((MTCI_CTRL::gR_CmdBuf[uw16_i%MAX_UART_BUF_SIZE] == 0x01)
                && (MTCI_CTRL::gR_CmdBuf[(uw16_i+1)%MAX_UART_BUF_SIZE] == 0x03)
                && (MTCI_CTRL::gR_CmdBuf[(uw16_i+2)%MAX_UART_BUF_SIZE] == 0x02))
        {
            PresentValue = (MTCI_CTRL::gR_CmdBuf[uw16_i+3] << 8) & 0xFF00;// High bytes of data
            PresentValue |= MTCI_CTRL::gR_CmdBuf[uw16_i+4] & 0xFF;        //Low bytes of data
            PresentValue /= 10;   //The storage value of the temperature control box is 100 times the actual value

            MTCI_CTRL::SetPresentValue(ChanNo,PresentValue,uc_Temp4Humi_Flag,FIRST_CHAMBER_CHANNEL);//FIRST_CHAMBER_CHANNEL
            CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gC_TimeoutStamp[ChanNo]);

            return;
        }
        uw16_i++;
    } while(uw16_i < uw16_Total);
}


void TC_H_OYO1000::SendWriteCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    short temp;
    unsigned short crc16;
    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);
    if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
    {
        // write CMD:  {[controller address (1 byte)] [wirte command (0x06)] [ register data high byte] [ register data low byte] [data high and l (2 byte)] [CRC low byte] [CRC high byte]}
        // Set Code = Tempture*100;
        // write Master to  Slave cmd  (Set the temperature to 22 ° C, 100 times is 22*100)(hex：0x08 0x98)
        unsigned char SetTempCmd[] = {0x01, 0x06, 0x01, 0x90, 0x08, 0x98, 0x8E, 0x71};//Set Tempture
        unsigned short uw16_len = sizeof(SetTempCmd);

        temp = MTCI_CTRL::gW_TemperatureValue[ChanNo][FIRST_CHAMBER_CHANNEL];
        temp *= 10;
        SetTempCmd[uw16_len - 4] = (temp >> 8) & 0xFF;
        SetTempCmd[uw16_len - 3] = temp & 0xFF;

        crc16 = MTCI_datatypes::CRC16(SetTempCmd,uw16_len-2);
        SetTempCmd[uw16_len - 2] = (unsigned char) (crc16 >>8);
        SetTempCmd[uw16_len - 1] = (unsigned char) (crc16 & 0xff);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);// If the serial port issues a command, the update time is made
        UARTFunc::uart_putcmd(ChanNo,SetTempCmd,uw16_len);
    }
    else if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
    {
        //Set Code=Humidity*100;
        //write Master to  Slave cmd  (Set the humidity to 55%，100 times is 5500)（hex：0x15 0x7c）
        unsigned char SetHumiCmd[] = {0x01, 0x06, 0x01, 0x92, 0x15, 0x7C, 0x26, 0xAA};//Set Humidity
        unsigned short uw16_len = sizeof(SetHumiCmd);

        temp = MTCI_CTRL::gW_HumiValue[ChanNo][FIRST_CHAMBER_CHANNEL];
        temp *= 10;
        SetHumiCmd[uw16_len - 4] = (temp >> 8) & 0xFF;
        SetHumiCmd[uw16_len - 3] = temp & 0xFF;

        crc16 = MTCI_datatypes::CRC16(SetHumiCmd,uw16_len-2);
        SetHumiCmd[uw16_len - 2] = (unsigned char) (crc16 >>8);
        SetHumiCmd[uw16_len - 1] = (unsigned char) (crc16 & 0xff);

        CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);
        UARTFunc::uart_putcmd(ChanNo,SetHumiCmd,uw16_len);
    }
}

void TC_H_OYO1000::SendReadCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    unsigned short crc16;

    //CMD: {[controller address (1 byte)] [read command (0x03)] [register high byte] [register low byte]
    //[data byte number 2Byte] [CRC low byte] [CRC high byte]}
    // Reply {[controller address (1 byte)] [read command (0x03)] [data byte of number]
    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);
    if(uc_Temp4Humi_Flag == SETPOINTVALUE_TEMPERATURE)
    {
        unsigned char ReadTempCmd[] = {0x01,0x03,0x00,0xCA,0x00,0x01,0xA4,0x34};//Gets the temperature value of a channel
        unsigned short uw16_len = sizeof(ReadTempCmd);

        crc16 = MTCI_datatypes::CRC16(ReadTempCmd,uw16_len-2);
        ReadTempCmd[6] = (unsigned char) (crc16 >>8);
        ReadTempCmd[7] = (unsigned char) (crc16 & 0xFF);

        UARTFunc::uart_putcmd(ChanNo,ReadTempCmd,uw16_len);
    }
    else if(uc_Temp4Humi_Flag == SETPOINTVALUE_HUMI)
    {
        unsigned char ReadHumiCmd[] = {0x01,0x03,0x00,0xD6,0x00,0x01,0x65,0xF2};//Gets the humidity value of a channel
        unsigned short uw16_len = sizeof(ReadHumiCmd);

        crc16 = MTCI_datatypes::CRC16(ReadHumiCmd,uw16_len-2);
        ReadHumiCmd[6] = (unsigned char) (crc16 >>8);
        ReadHumiCmd[7] = (unsigned char) (crc16 & 0xFF);

        UARTFunc::uart_putcmd(ChanNo,ReadHumiCmd,uw16_len);
    }
}

void TC_H_OYO1000::SendRunCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //CMD: {[controller address (1 byte)] [read command (0x03)] [register high byte] [register low byte]
    //[data byte of number 2byte] [CRC low byte] [CRC high byte]} Reply command code is the same
    unsigned char RunCmd[] = {0x01,0x06,0x00,0x66,0x00,0x01,0xA8,0x15};//RUN CMD
    unsigned short crc16;
    unsigned short uw16_len = sizeof(RunCmd);

    crc16 = MTCI_datatypes::CRC16(RunCmd,uw16_len-2);
    RunCmd[6] = (unsigned char) (crc16 >>8);
    RunCmd[7] = (unsigned char) (crc16 & 0xFF);

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);
    UARTFunc::uart_putcmd(ChanNo, RunCmd, uw16_len);
}

void TC_H_OYO1000::SendStopCmd(unsigned char ChanNo,unsigned char uc_Temp4Humi_Flag)
{
    //CMD: {[controller address (1 byte)] [write command (0x06)] [register high byte] [register low byte]
    //[data high byte] [data low byte] [CRC low byte] [CRC high byte]}  Reply command code is the same
    unsigned char StopCmd[] = {0x01,0x06,0x00,0x66,0x00,0x02,0xE8,0x14};//Stop command code
    unsigned short crc16;
    unsigned short uw16_len = sizeof(StopCmd);

    crc16 = MTCI_datatypes::CRC16(StopCmd,uw16_len-2);
    StopCmd[6] = (unsigned char) (crc16 >>8);
    StopCmd[7] = (unsigned char) (crc16 & 0xFF);

    CommonFunc::GetTimeTicksUpdate( &MTCI_CTRL::gW_WriteStamp[ChanNo]);  // If the serial port issues a command, the update time is made
    UARTFunc::uart_putcmd(ChanNo, StopCmd, uw16_len);
}

