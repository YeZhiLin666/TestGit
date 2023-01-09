#include "includes.h"

/******************************************************************************
                                     TMP75C
******************************************************************************/

float TMP75C_TEC::f_Read_Temp;
unsigned short TMP75C_TEC::Read_16;
float TMP75C_TEC::H_Temp = 0x9600;
float TMP75C_TEC::L_Temp = 0x3200;
//float TMP75C_TEC::H_Temp = 0x3200;
//float TMP75C_TEC::L_Temp = 0x2D00;

const float Temp_Ratio = 0.0625f;
float TMP75C_TEC::Local[8];

/******************************************************************************
** Function name:        Set_HighTemp
** Descriptions:         Set temperature high limit for TMP75C
** parameters:           Temp
** Returned value:       None
** Created Date:         2016/03/08
** Revision Dates:
******************************************************************************/
void TMP75C_TEC::Set_HighTemp(long Temp, unsigned int TECChannelNo)//Change by ZC 04.04.2018
{
    I2CBusNew::RunningBuffer[I2C0].OPMode = I2C_MASTWRITE; // MASTER WRITE

    I2CBusNew::m_I2CBusMgr[I2C0].CtrlByteNum = 1;
    I2CBusNew::m_I2CTaskMgr[I2C0].SubBufEndingAdr = 2;

    I2CBusNew::m_I2CBusMgr[I2C0].SlaveAdr = 0x90 + 2 * TECChannelNo;//slave address + Write bit
    I2CBusNew::m_I2CTaskMgr[I2C0].CurrentAdr[0] = 0x03;//

    I2CBusNew::RunningBuffer[I2C0].Buf[0] = (short)(Temp>>8);
    I2CBusNew::RunningBuffer[I2C0].Buf[1] = (short)Temp;
    HT16K33::I2CEngine(I2C0);

}
/******************************************************************************
** Function name:        Set_LowTemp
** Descriptions:         Set temperature low limit for TMP75C
** parameters:           Temp
** Returned value:       None
** Created Date:         2016/03/08
** Revision Dates:
******************************************************************************/
void TMP75C_TEC::Set_LowTemp(long Temp, unsigned int TECChannelNo)//Change by ZC 04.04.2018
{
    I2CBusNew::RunningBuffer[I2C0].OPMode = I2C_MASTWRITE; // MASTER WRITE

    I2CBusNew::m_I2CBusMgr[I2C0].CtrlByteNum = 1;
    I2CBusNew::m_I2CTaskMgr[I2C0].SubBufEndingAdr = 2;

    I2CBusNew::m_I2CBusMgr[I2C0].SlaveAdr = 0x90 + 2 * TECChannelNo;//slave address + Write bit
    I2CBusNew::m_I2CTaskMgr[I2C0].CurrentAdr[0] = 0x02;//

    I2CBusNew::RunningBuffer[I2C0].Buf[0] = (short)(Temp>>8);
    I2CBusNew::RunningBuffer[I2C0].Buf[1] = (short)Temp;
    HT16K33::I2CEngine(I2C0);
}
/***********************************************************************************************************
** Function name:        Init_TMP75C
** Descriptions:         Configure Registers for TMP75C, set temperature high limit to 65 and low limit to 50,
												 and prepare the pointer to the temperature register
** parameters:           None
** Returned value:       None
** Created Date:         2016/03/08
** Revision Dates:
*************************************************************************************************************/
void TMP75C_TEC::Init_TMP75C(unsigned int TECChannelNo)  //Change by ZC 04.04.2018
{
    I2CBusNew::RunningBuffer[I2C0].OPMode = I2C_MASTWRITE; // MASTER WRITE

    I2CBusNew::m_I2CBusMgr[I2C0].CtrlByteNum = 3;
    I2CBusNew::m_I2CTaskMgr[I2C0].SubBufEndingAdr = 0;

    I2CBusNew::m_I2CBusMgr[I2C0].SlaveAdr = 0x90 + 2 * TECChannelNo;//slave address + Write bit
    I2CBusNew::m_I2CTaskMgr[I2C0].CurrentAdr[0] = 0x01;//Turn on system oscillator
    I2CBusNew::m_I2CTaskMgr[I2C0].CurrentAdr[1] = 0x00;//set blinking frequence
    I2CBusNew::m_I2CTaskMgr[I2C0].CurrentAdr[2] = 0x00;//set brightness
    HT16K33::I2CEngine(I2C0);

    TMP75C_TEC::Set_HighTemp(H_Temp, TECChannelNo);
    TMP75C_TEC::Set_LowTemp(L_Temp, TECChannelNo);

    I2CBusNew::m_I2CBusMgr[I2C0].CtrlByteNum = 1;
    I2CBusNew::m_I2CTaskMgr[I2C0].SubBufEndingAdr = 0;

    I2CBusNew::m_I2CBusMgr[I2C0].SlaveAdr = 0x90 + 2 * TECChannelNo;//slave address + Write bit
    I2CBusNew::m_I2CTaskMgr[I2C0].CurrentAdr[0] = 0x00;//Temperature Register
    HT16K33::I2CEngine(I2C0);
}
/******************************************************************************
** Function name:        Get_Temp
** Descriptions:         Read temperature from TMP75C
** parameters:           None
** Returned value:       f_Read_Temp
** Created Date:         2016/03/08
** Revision Dates:
******************************************************************************/
float TMP75C_TEC::Get_Temp(void)
{
    I2CBusNew::RunningBuffer[I2C0].OPMode = I2C_MASTREAD;

    I2CBusNew::m_I2CTaskMgr[I2C0].SubBufEndingAdr = 2;

    I2CBusNew::m_I2CBusMgr[I2C0].SlaveAdr = 0x91;//slave address + Read bit
    HT16K33::I2CEngine(I2C0);

    Read_16 = (((unsigned short)I2CBusNew::RunningBuffer[I2C0].Buf[0])<<8)|I2CBusNew::RunningBuffer[I2C0].Buf[1];
    Read_16 = Read_16>>4;
    f_Read_Temp = (float)Read_16*Temp_Ratio;
    return f_Read_Temp;
}

/******************************************************************************
** Function name:        Get_Temp
** Descriptions:         Read temperature from TMP75C
** parameters:           TECChannelNo
** Returned value:       f_Read_Temp
** Created Date:         2018/04/04
** Revision Dates:
******************************************************************************/

float TMP75C_TEC::Get_MTC_Temp(unsigned int TECChannelNo)
{
    f_Read_Temp = 0;
    I2CBusNew::RunningBuffer[I2C0].OPMode = I2C_MASTREAD;
    I2CBusNew::m_I2CBusMgr[I2C0].CtrlByteNum = 1;
    I2CBusNew::m_I2CTaskMgr[I2C0].SubBufEndingAdr = 2;
    I2CBusNew::m_I2CTaskMgr[I2C0].CurrentAdr[0] = 0x00;
    I2CBusNew::m_I2CBusMgr[I2C0].SlaveAdr = 0x91 + 2 * TECChannelNo;//slave address + Read bit
    HT16K33::I2CEngine(I2C0);

    Read_16 = (((unsigned short)I2CBusNew::RunningBuffer[I2C0].Buf[0])<<8)|I2CBusNew::RunningBuffer[I2C0].Buf[1];
    Read_16 = Read_16>>4;
    f_Read_Temp = (float)Read_16*Temp_Ratio;
    if(f_Read_Temp>200)f_Read_Temp=f_Read_Temp-256;

//--------clean the buffer------------------
    I2CBusNew::RunningBuffer[I2C0].Buf[0]=0;
    I2CBusNew::RunningBuffer[I2C0].Buf[1]=0;
//--------if no new value, keep prevoius value------------------
    if(f_Read_Temp!=0)
        TMP75C_TEC::Local[TECChannelNo]=f_Read_Temp;
    else
        f_Read_Temp=TMP75C_TEC::Local[TECChannelNo];

    return f_Read_Temp;
}
