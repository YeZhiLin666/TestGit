
#include "includes.h"
#include "TMP75C.h"

//const float TempGateRatio = 0.5f;
const float TempRatio = 0.0625f;
unsigned char TMP75C::bAssigned[8] = {0,0,0,0,0,0,0,0};//支持多个片子
unsigned short TMP75C::HandleID[8] = {0,0,0,0,0,0,0,0};

//立即读温度
float TMP75C::ReadTempf(unsigned char adr)
{
    float f_ReadTemp;
    st_I2CModule  tempModule;
    tempModule.I2Cx = TMP75C_I2C_PORT;

    tempModule.CtrlByteNum = 1;//POINTER
    tempModule.BufLen = 2;
    memset(tempModule.Buf,tempModule.BufLen,0);
    tempModule.MasterCtrlByte.SLA_Adr.SlaveAdr  =TMP75C_CMD|(adr<<1);
    tempModule.MasterCtrlByte.SLA_Adr.WRRegAdr = TEMP_REG_POINT_TMP75C;
    tempModule.OPMode = I2C_MASTREAD;
    //lock i2c
    if(I2CBusNew::I2C_ImmidiateTrans(&tempModule))
    {
        unsigned short Read16 = (uint16)I2CBusNew::RunningBuffer[tempModule.I2Cx].Buf[0]<<8| I2CBusNew::RunningBuffer[tempModule.I2Cx].Buf[1];
        Read16 = Read16>>4;
        f_ReadTemp = (float)Read16*TempRatio;

        //unlock
        I2CBusNew::m_I2CBusMgr[tempModule.I2Cx].I2CBusy = I2C_NOT_BUSY;
        return f_ReadTemp;
    }
    else
        return 0;//失败
}

// DO NOT DELETE!
//任务请求模式读温度，
//adr 地址最多（0-7)
//fTemp 引用 返回温度值
//bool TMP75C::ReadTempReq(unsigned char adr,float& fTemp)
//{
//	st_I2CModule  tempModule;
//	if(adr>7)
//	 return false;
//	if(!bAssigned[adr]) //未请求
//	{
//		tempModule.I2Cx = TMP75C_I2C_PORT;
//		tempModule.CtrlByteNum = 1;//POINTER BYTE
//		tempModule.BufLen = 2;
//		memset(tempModule.Buf,tempModule.BufLen,0);
//		tempModule.MasterCtrlByte.SLA_Adr.SlaveAdr  =TMP75C_CMD|(adr<<1);
//		tempModule.MasterCtrlByte.SLA_Adr.WRRegAdr = TEMP_REG_POINT_TMP75C;
//		tempModule.OPMode = I2C_MASTREAD;
//		HandleID[adr] = I2CBusNew::I2C_Task_Request(I2C_MASTREAD,&tempModule);
//		bAssigned[adr] = 1;  //已交付
//	}
//	 if(bAssigned[adr] && I2CBusNew::GetRequestStatus(HandleID[adr]) == I2C_ReqFin) //已完成
//	 {
//		 bAssigned[adr] = 0;
//		 I2CBusNew::SetRequestStatus(HandleID[adr],I2C_ReqHandled);  //设置成已处理完，供I2C任务转入下一个请求
//		 unsigned short Read16 = (uint16)I2CBusNew::RunningBuffer[tempModule.I2Cx].Buf[0]<<8| I2CBusNew::RunningBuffer[tempModule.I2Cx].Buf[1];
//		 Read16 = Read16>>4;
//		 fTemp = (float)Read16*TempRatio;
//		 return true;
//	 }
//	 else
//		 return false;
//}

// DO NOT DELETE!
//设置Tos温度
//bool TMP75C::SetThighTemp(unsigned char adr,float TempThigh)
//{
//	st_I2CModule  tempModule;
//	tempModule.I2Cx = TMP75C_I2C_PORT;
//	tempModule.CtrlByteNum = 1;//POINTER BYTE
//	tempModule.BufLen = 2;
//	memset(tempModule.Buf,tempModule.BufLen,0);
//	tempModule.MasterCtrlByte.SLA_Adr.SlaveAdr  =TMP75C_CMD|(adr<<1);
//	tempModule.MasterCtrlByte.SLA_Adr.WRRegAdr = Thigh_REG_POINT_TMP75C;
//	tempModule.OPMode = I2C_MASTWRITE;
//	unsigned short ThighCode = (unsigned short)(TempThigh/TempGateRatio);
//	ThighCode = ThighCode<<4;
//	tempModule.Buf[0] = ThighCode>>8;
//	tempModule.Buf[1] =  ThighCode&0xFF;
//	//lock i2c
//	if(I2CBusNew::I2C_ImmidiateTrans(&tempModule))
//	{
//		//unlock
//		I2CBusNew::m_I2CBusMgr[tempModule.I2Cx].I2CBusy = I2C_NOT_BUSY;
//		return true;
//	}
//	else
//		return false;//失败
//}

// DO NOT DELETE!
//设置迟滞温度
//bool TMP75C::SetTlowTemp(unsigned char adr,float TempTlow)
//{
//	st_I2CModule  tempModule;
//	tempModule.I2Cx = TMP75C_I2C_PORT;
//	tempModule.CtrlByteNum = 1;//POINTER BYTE
//	tempModule.BufLen = 2;
//	memset(tempModule.Buf,tempModule.BufLen,0);
//	tempModule.MasterCtrlByte.SLA_Adr.SlaveAdr  =TMP75C_CMD|(adr<<1);
//	tempModule.MasterCtrlByte.SLA_Adr.WRRegAdr = Tlow_REG_POINT_TMP75C;
//	tempModule.OPMode = I2C_MASTWRITE;
//	unsigned short TlowCode = (unsigned short)(TempTlow/TempGateRatio);
//	TlowCode = TlowCode<<4;
//	tempModule.Buf[0] = TlowCode>>11;
//	tempModule.Buf[1] =  TlowCode&0xFF;
//	//lock i2c
//	if(I2CBusNew::I2C_ImmidiateTrans(&tempModule))
//	{
//		//unlock
//		I2CBusNew::m_I2CBusMgr[tempModule.I2Cx].I2CBusy = I2C_NOT_BUSY;
//		return true;
//
//	}
//	else
//		return false;//失败
//}

// DO NOT DELETE!
//读Tos温度REG
//float TMP75C::ReadThighTemp(unsigned char adr)
//{
//	float f_ReadTemp;
//	st_I2CModule  tempModule;
//	tempModule.I2Cx = TMP75C_I2C_PORT;
//	tempModule.CtrlByteNum = 1;//POINTER BYTE
//	tempModule.BufLen = 2;
//	memset(tempModule.Buf,tempModule.BufLen,0);
//	tempModule.MasterCtrlByte.SLA_Adr.SlaveAdr  =TMP75C_CMD|(adr<<1);
//	tempModule.MasterCtrlByte.SLA_Adr.WRRegAdr = Thigh_REG_POINT_TMP75C;
//	tempModule.OPMode = I2C_MASTREAD;
//	//lock i2c
//	if(I2CBusNew::I2C_ImmidiateTrans(&tempModule))
//	{
//		unsigned short Read16 = (uint16)I2CBusNew::RunningBuffer[tempModule.I2Cx].Buf[0]<<8| I2CBusNew::RunningBuffer[tempModule.I2Cx].Buf[1];
//		Read16 = Read16>>4;
//		f_ReadTemp = (float)Read16*TempGateRatio;
//		//解锁
//		//unlock
//		I2CBusNew::m_I2CBusMgr[tempModule.I2Cx].I2CBusy = I2C_NOT_BUSY;
//		return f_ReadTemp;
//	}
//	else
//		return 0.0f;//失败
//
//}

// DO NOT DELETE!
//读迟滞温度REG
//float TMP75C::ReadTlowTemp(unsigned char adr)
//{
//	float f_ReadTemp;
//	st_I2CModule  tempModule;
//	tempModule.I2Cx = TMP75C_I2C_PORT;
//	tempModule.CtrlByteNum = 1;//POINTER BYTE
//	tempModule.BufLen = 2;
//	memset(tempModule.Buf,tempModule.BufLen,0);
//	tempModule.MasterCtrlByte.SLA_Adr.SlaveAdr  =TMP75C_CMD|(adr<<1);
//	tempModule.MasterCtrlByte.SLA_Adr.WRRegAdr = Tlow_REG_POINT_TMP75C;
//	tempModule.OPMode = I2C_MASTREAD;
//	//lock i2c
//	if(I2CBusNew::I2C_ImmidiateTrans(&tempModule))
//	{
//		unsigned short Read16 = (uint16)I2CBusNew::RunningBuffer[tempModule.I2Cx].Buf[0]<<8| I2CBusNew::RunningBuffer[tempModule.I2Cx].Buf[1];
//		Read16 = Read16>>4;
//		f_ReadTemp = (float)Read16*TempGateRatio;
//		//解锁
//		//unlock
//		I2CBusNew::m_I2CBusMgr[tempModule.I2Cx].I2CBusy = I2C_NOT_BUSY;
//		return f_ReadTemp;
//	}
//	else//失败
//	    return 0.0f;
//}

// bool TMP75C::WriteConfig(unsigned char adr,un_TMP75C_ConfigReg ConfigReg)
// {
// 	//default acitve Low ,compare mode  //
// 	st_I2CModule  tempModule;
// 	tempModule.I2Cx = TMP75C_I2C_PORT;
// 	tempModule.CtrlByteNum = 1;//POINTER BYTE
// 	tempModule.BufLen = 1;
// 	memset(tempModule.Buf,tempModule.BufLen,0);
// 	tempModule.MasterCtrlByte.SLA_Adr.SlaveAdr  =TMP75C_CMD|(adr<<1);
// 	tempModule.MasterCtrlByte.SLA_Adr.WRRegAdr = CONFIG_REG_POINT_TMP75C;
// 	tempModule.OPMode = I2C_MASTWRITE;
// 	tempModule.Buf[0] = ConfigReg.uc_Config;
// 	//lock i2c
// 	if(I2CBusNew::I2C_ImmidiateTrans(&tempModule))
// 	{
// 		//unlock
// 		I2CBusNew::m_I2CBusMgr[tempModule.I2Cx].I2CBusy = I2C_NOT_BUSY;
// 		return true;
// 	}
// 	else
// 		return false;//失败
// }

// unsigned char TMP75C::ReadConfig(unsigned char adr)
// {
// 	st_I2CModule  tempModule;
// 	tempModule.I2Cx = TMP75C_I2C_PORT;
// 	tempModule.CtrlByteNum = 1;//POINTER BYTE
// 	tempModule.BufLen = 1;
// 	memset(tempModule.Buf,tempModule.BufLen,0);
// 	tempModule.MasterCtrlByte.SLA_Adr.SlaveAdr  = TMP75C_CMD|(adr<<1);
// 	tempModule.MasterCtrlByte.SLA_Adr.WRRegAdr = CONFIG_REG_POINT_TMP75C;
// 	tempModule.OPMode = I2C_MASTREAD;
// 	//lock i2c
// 	if(I2CBusNew::I2C_ImmidiateTrans(&tempModule))
// 	{
// 		unsigned char Read8 = (uint16)I2CBusNew::RunningBuffer[tempModule.I2Cx].Buf[0];
// 		I2CBusNew::m_I2CBusMgr[tempModule.I2Cx].I2CBusy = I2C_NOT_BUSY;
// 		return Read8;
// 	}
// 	else
// 		return 0;//失败
// }

// DO NOT DELETE!
//void TMP75C::setPowerMode(unsigned char adr,TMP75C_PowerMode mode)
//{
//    //Read the current 8-bit register value
//     un_TMP75C_ConfigReg ConfigReg;
//     ConfigReg.uc_Config = ReadConfig(adr);
//
//    //Set or clear the SHUTDOWN bit
//    if (mode == POWER_SHUTDOWN_TMP75C)
//        ConfigReg.uc_Config |= (1 << 0);
//    else
//        ConfigReg.uc_Config &= ~(1 << 0);
//
//    //Write the value back out
//    WriteConfig(adr,ConfigReg);
//}

// DO NOT DELETE!
// TMP75C_OSMode TMP75C::getOsMode(unsigned char adr)
//{
//    //Read the 8-bit register value
//     char value = ReadConfig(adr);
//
//    //Return the status of the OS_COMP_INT bit
//    if (value & (1 << 1))
//        return OS_INTERRUPT_TMP75C;
//    else
//        return OS_COMPARATOR_TMP75C;
//}

// DO NOT DELETE!
//void TMP75C::setOsMode(unsigned char adr,TMP75C_OSMode mode)
//{
//    //Read the current 8-bit register value
//	un_TMP75C_ConfigReg ConfigReg;
//    ConfigReg.uc_Config = ReadConfig(adr);
//
//    //Set or clear the OS_COMP_INT bit
//    if (mode == OS_INTERRUPT_TMP75C)
//        ConfigReg.uc_Config |= (1 << 1);
//    else
//        ConfigReg.uc_Config &= ~(1 << 1);
//
//    //Write the value back out
//     WriteConfig(adr,ConfigReg);
//}

// DO NOT DELETE!
//TMP75C_OSPolarity TMP75C::getOsPolarity(unsigned char adr)
//{
//    //Read the 8-bit register value
//    un_TMP75C_ConfigReg ConfigReg;
//    ConfigReg.uc_Config = ReadConfig(adr);
//
//    //Return the status of the OS_POL bit
//    if (ConfigReg.uc_Config & (1 << 2))
//        return OS_ACTIVE_HIGH_TMP75C;
//    else
//        return OS_ACTIVE_LOW_TMP75C;
//}

// DO NOT DELETE!
//void TMP75C::setOsPolarity(unsigned char adr,TMP75C_OSPolarity polarity)
//{
//    //Read the current 8-bit register value
//    un_TMP75C_ConfigReg ConfigReg;
//    ConfigReg.uc_Config = ReadConfig(adr);
//
//    //Set or clear the OS_POL bit
//    if (polarity == OS_ACTIVE_HIGH_TMP75C)
//        ConfigReg.uc_Config |= (1 << 2);
//    else
//        ConfigReg.uc_Config &= ~(1 << 2);
//
//    //Write the value back out
//   WriteConfig(adr,ConfigReg);
//}

