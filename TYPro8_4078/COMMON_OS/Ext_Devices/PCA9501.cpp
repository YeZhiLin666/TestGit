#include "includes.h"
#include "LM75B.h"
#include "PCA9501.h"


bool PCA9501::SetExpander(unsigned char adr,unsigned char bitIO)
{
    st_I2CModule  tempModule;
    tempModule.I2Cx = PCA9501_PORT;
    tempModule.CtrlByteNum = 1;//POINTER BYTE
    tempModule.BufLen = 0;
    memset(tempModule.Buf,tempModule.BufLen,0);
    tempModule.MasterCtrlByte.SLA_Adr.SlaveAdr  =PCA9501_IO_WRITE(adr);
    tempModule.MasterCtrlByte.SLA_Adr.WRRegAdr = 0xff&(~bitIO);		//CtrlByteNum = 1;//POINTER BYTE
    tempModule.OPMode = I2C_MASTWRITE;
    //lock i2c
    if(I2CBusNew::I2C_ImmidiateTrans(&tempModule))
    {
        //½âËø  //unlock
        I2CBusNew::m_I2CBusMgr[tempModule.I2Cx].I2CBusy = I2C_NOT_BUSY;
        return true;
    }
    else
        return false;//Ê§°Ü
}

// DO NOT DELETE!
//bool PCA9501::PCA9501_ReadTemp(unsigned char pca9501_Adr,unsigned char sub_adr,float &f_temp)
//{
//	unsigned char setBitIO = 0;
//	setBitIO |= (1<<sub_adr);  // 0 ~7 IO Set
//	if(SetExpander(pca9501_Adr,setBitIO))	 //address locked pca9501_Adr
//	{
//		f_temp = LM75B::ReadTempf(0);//address 0 is valid
//		SetExpander(pca9501_Adr,0);  //pca9501_Adr
//		return true;
//	}
//	else
//		return false;
//}
//bool PCA9501::readExpander(unsigned char adr,unsigned char &bitIO)
//{
//	return false;
//}
//void PCA9501::SetE2PromMode(unsigned char adr,unsigned char *buf)
//{
//}

