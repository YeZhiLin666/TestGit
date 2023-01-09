
/*****************Temperature LED Display using HT16K33***************************

**       Added by C.Zhong 03_14_2016

**********************************************************************************/

#include "includes.h"

unsigned long  HT16K33::I2Ctimeout;
bool HT16K33::ReturnVal;
unsigned char HT16K33::Num[16];
unsigned char HT16K33::Digit[3];

/*******************************************************************************
                                     HT16K33
*******************************************************************************/
/******************************************************************************
** Function name:        Init_NumTab
** Descriptions:         Set up number table for HT16K33
** parameters:           None
** Returned value:       None
** Created Date:         2016/03/08
** Revision Dates:
******************************************************************************/
void HT16K33::Init_NumTab(void)
{
    Num[0] = 0x3F; //** 0
    Num[1] = 0x06; //** 1
    Num[2] = 0x5B; //** 2
    Num[3] = 0x4F; //** 3
    Num[4] = 0x66; //** 4
    Num[5] = 0x6D; //** 5
    Num[6] = 0x7D; //** 6
    Num[7] = 0x07; //** 7
    Num[8] = 0x7F; //** 8
    Num[9] = 0x6F; //** 9
    Num[10] = 0x77; //** A
    Num[11] = 0x7C; //** B
    Num[12] = 0x39; //** C
    Num[13] = 0x5E; //** D
    Num[14] = 0x79; //** E
    Num[15] = 0x71; //** F
}

void HT16K33::Init_HT16K33(void)
{
    HT16K33::Init_NumTab();
    int Led_i;
    //** HT16K33 chip initialization
    I2CBusNew::RunningBuffer[I2C2].OPMode = I2C_MASTWRITE; // MASTER WRITE

    I2CBusNew::m_I2CBusMgr[I2C2].CtrlByteNum = 3;
    I2CBusNew::m_I2CTaskMgr[I2C2].SubBufEndingAdr = 0;

    I2CBusNew::m_I2CBusMgr[I2C2].SlaveAdr = 0xE0;//slave address + Write bit
    I2CBusNew::m_I2CTaskMgr[I2C2].CurrentAdr[0] = 0x21;//Turn on system oscillator
    I2CBusNew::m_I2CTaskMgr[I2C2].CurrentAdr[1] = 0x81;//set blinking frequence
    I2CBusNew::m_I2CTaskMgr[I2C2].CurrentAdr[2] = 0xE0;//set brightness
    HT16K33::I2CEngine(I2C2);

    //** Send first data, all 8 with dot
    I2CBusNew::m_I2CBusMgr[I2C2].CtrlByteNum = 1;
    I2CBusNew::m_I2CTaskMgr[I2C2].SubBufEndingAdr = 6;

    I2CBusNew::m_I2CBusMgr[I2C2].SlaveAdr = 0xE0;//slave address + Write bit
    I2CBusNew::m_I2CTaskMgr[I2C2].CurrentAdr[0] = 0x00;// start at address 0x00
    for(Led_i = 0; Led_i < 6; Led_i++)
    {
        I2CBusNew::RunningBuffer[I2C2].Buf[Led_i] = 0xFF;//First digit
    }
    HT16K33::I2CEngine(I2C2);

    //** Start to display
    I2CBusNew::m_I2CBusMgr[I2C2].CtrlByteNum = 1;
    I2CBusNew::m_I2CTaskMgr[I2C2].SubBufEndingAdr = 0;

    I2CBusNew::m_I2CBusMgr[I2C2].SlaveAdr = 0xE0;//slave address + Write bit
    I2CBusNew::m_I2CTaskMgr[I2C2].CurrentAdr[0] = 0x81;//Turn on system oscillator
    HT16K33::I2CEngine(I2C2);
}

void HT16K33::LED_BLINK(void)
{
    I2CBusNew::RunningBuffer[I2C2].OPMode = I2C_MASTWRITE; // MASTER WRITE
    I2CBusNew::m_I2CBusMgr[I2C2].CtrlByteNum = 1;
    I2CBusNew::m_I2CTaskMgr[I2C2].SubBufEndingAdr = 0;

    I2CBusNew::m_I2CBusMgr[I2C2].SlaveAdr = 0xE0;//slave address + Write bit
    I2CBusNew::m_I2CTaskMgr[I2C2].CurrentAdr[0] = 0x83;//Blinking
    HT16K33::I2CEngine(I2C2);
}

void HT16K33::LED_NBLINK(void)
{
    I2CBusNew::RunningBuffer[I2C2].OPMode = I2C_MASTWRITE; // MASTER WRITE
    I2CBusNew::m_I2CBusMgr[I2C2].CtrlByteNum = 1;
    I2CBusNew::m_I2CTaskMgr[I2C2].SubBufEndingAdr = 0;

    I2CBusNew::m_I2CBusMgr[I2C2].SlaveAdr = 0xE0;//slave address + Write bit
    I2CBusNew::m_I2CTaskMgr[I2C2].CurrentAdr[0] = 0x81;//Turn on system oscillator
    HT16K33::I2CEngine(I2C2);
}


/******************************************************************************
** Function name:        DispTemp_DoWork
** Descriptions:         Send data to HT16K33 for displaying temperature
** parameters:           temp
** Returned value:       None
** Created Date:         2016/03/08
** Revision Dates:
******************************************************************************/
void HT16K33::DispTemp(float temp)
{
//	int disptemp1;
//	int disptemp2;

    I2CBusNew::RunningBuffer[I2C2].OPMode = I2C_MASTWRITE; // MASTER WRITE

    I2CBusNew::m_I2CBusMgr[I2C2].CtrlByteNum = 1;
    I2CBusNew::m_I2CTaskMgr[I2C2].SubBufEndingAdr = 6;

    I2CBusNew::m_I2CBusMgr[I2C2].SlaveAdr = 0xE0;//slave address + Write bit
    I2CBusNew::m_I2CTaskMgr[I2C2].CurrentAdr[0] = 0x00;// start at address 0x00

//	disptemp1 = (int)((temp+0.001f)/10.0f);
    Digit[0] = (unsigned char)((temp+0.001f)/10.0f);
//	I2CBusNew::RunningBuffer[I2C2].Buf[0] = Num[disptemp1];//first digit
    I2CBusNew::RunningBuffer[I2C2].Buf[0] = Num[Digit[0]];//first digit
    I2CBusNew::RunningBuffer[I2C2].Buf[1] = 0xFF;

//	disptemp2 = (int)(temp-disptemp1*10.0f+0.01f);
    Digit[1] = (unsigned char)(temp-Digit[0]*10.0f+0.01f);
//	I2CBusNew::RunningBuffer[I2C2].Buf[2] = Num[disptemp2]|0x80;//second digit
    I2CBusNew::RunningBuffer[I2C2].Buf[2] = Num[Digit[1]]|0x80;//second digit
    I2CBusNew::RunningBuffer[I2C2].Buf[3] = 0xFF;

//	disptemp2 = (int)((temp-disptemp1*10.0f-disptemp2)*10.0f+0.5f);
    Digit[2] = (unsigned char)((temp-Digit[0]*10.0f-Digit[1])*10.0f+0.5f);
//	I2CBusNew::RunningBuffer[I2C2].Buf[4] = Num[disptemp2];//third digit
    I2CBusNew::RunningBuffer[I2C2].Buf[4] = Num[Digit[2]];//third digit
    I2CBusNew::RunningBuffer[I2C2].Buf[5] = 0xFF;

    HT16K33::I2CEngine(I2C2);
}

unsigned char HT16K33::I2CEngine(unsigned char I2Cx)
{

    HT16K33::I2Ctimeout=0;
    HT16K33::ReturnVal = false;

    I2CBusNew::m_I2CBusMgr[I2Cx].BusState = I2CBus_IDLE;//set i2c masterstate flag as idle

    I2CBusNew::m_I2CBusMgr[I2Cx].CtrlByteIdx = 0;
    I2CBusNew::RunningBuffer[I2Cx].BufIdx = 0;

    if ( I2CBusNew::I2CStart(I2Cx) != TRUE )//if do not start,then stop .
    {
        I2CBusNew::I2CStop(I2Cx);
        return ( FALSE );
    }

    while(1)
    {
        if(I2Ctimeout >= I2CMAX_TIMEOUT)
        {
            I2CBusNew::I2CStop(I2Cx);
            HT16K33::I2Ctimeout=0;;
            HT16K33::ReturnVal = false;
            break;
        }
        else if(I2CBusNew::m_I2CBusMgr[I2Cx].BusState == I2CBus_NACK)
        {
            I2CBusNew::I2CStop(I2Cx);
            HT16K33::ReturnVal = false;
            break;
        }
        else if(I2CBusNew::m_I2CBusMgr[I2Cx].BusState == I2CBus_STOPPED && I2CBusNew::m_I2CBusMgr[I2Cx].StopCondition == I2CNormalStop)
        {
            HT16K33::ReturnVal = true;
            break;
        }
        HT16K33::I2Ctimeout++;
    }//while
    I2CBusNew::m_I2CBusMgr[I2Cx].BusState = I2CBus_IDLE;
    return HT16K33::ReturnVal;
}

