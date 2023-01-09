#include "includes.h"
#include "DAC6571.h"



void DAC6571::DAC6571_SET(unsigned char I2Cx,unsigned char chan,unsigned long data)
{
    uint16_t DAC_InCoding = 0;
    st_I2CModule  I2Cmodule_a;

    DAC_InCoding = (data & 0x3FF) << 2;
    I2Cmodule_a.I2Cx = I2Cx;
    I2Cmodule_a.CtrlByteNum = 2;//POINTER BYTE
    I2Cmodule_a.BufLen = 0;
    memset(I2Cmodule_a.Buf,I2Cmodule_a.BufLen,0);
    if(chan==0)
        I2Cmodule_a.MasterCtrlByte.SLA_Adr.SlaveAdr  = 0x98;
    else if(chan==1)
        I2Cmodule_a.MasterCtrlByte.SLA_Adr.SlaveAdr  = 0x9A;
    I2Cmodule_a.MasterCtrlByte.SLA_Adr.WRRegAdr = DAC_InCoding & 0x0FFC;
    I2Cmodule_a.OPMode = I2C_MASTWRITE;
    if(I2CBusNew::I2C_ImmidiateTrans(&I2Cmodule_a))
    {
        //unlock
        I2CBusNew::m_I2CBusMgr[I2Cmodule_a.I2Cx].I2CBusy = I2C_NOT_BUSY;
        return ;

    }
    else
        return ;//Ê§°Ü
}
