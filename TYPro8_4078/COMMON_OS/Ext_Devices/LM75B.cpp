
#include "includes.h"
#include "LM75B.h"

//const float TempGateRatio = 0.5f;
const float TempRatio = 0.125f;
unsigned char LM75B::bAssigned[8] = {0,0,0,0,0,0,0,0};//支持多个片子
unsigned short LM75B::HandleID[8] = {0,0,0,0,0,0,0,0};

//立即读温度
float LM75B::ReadTempf(unsigned char adr)
{
    float f_ReadTemp;
    st_I2CModule  tempModule;
    tempModule.I2Cx = LM75_I2C_PORT;
    tempModule.CtrlByteNum = 1;//POINTER BYTE
    tempModule.BufLen = 2;
    memset(tempModule.Buf,tempModule.BufLen,0);
    tempModule.MasterCtrlByte.SLA_Adr.SlaveAdr  =LM75_CMD|(adr<<1);
    tempModule.MasterCtrlByte.SLA_Adr.WRRegAdr = TEMP_REG_POINT;
    tempModule.OPMode = I2C_MASTREAD;
    //lock i2c
    if(I2CBusNew::I2C_ImmidiateTrans(&tempModule))
    {
        unsigned short Read16 = (uint16)I2CBusNew::RunningBuffer[tempModule.I2Cx].Buf[0]<<8| I2CBusNew::RunningBuffer[tempModule.I2Cx].Buf[1];
        Read16 = Read16>>5;
        if(Read16&0x400)
        {
            unsigned short n2dCode = (~Read16)&0x7FF;
            f_ReadTemp = -(n2dCode+1)*TempRatio;
        }
        else
            f_ReadTemp = (float)Read16*TempRatio;
        //解锁
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
//bool LM75B::ReadTempReq(unsigned char adr,float& fTemp)
//{
//	static st_I2CModule  tempModule;
//	if(adr>7)
//	 return false;
//	if(!bAssigned[adr]) //未请求
//	{
//		tempModule.I2Cx = LM75_I2C_PORT;
//		tempModule.CtrlByteNum = 1;//POINTER BYTE
//		tempModule.BufLen = 2;
//		memset(tempModule.Buf,tempModule.BufLen,0);
//		tempModule.MasterCtrlByte.SLA_Adr.SlaveAdr  =LM75_CMD|(adr<<1);
//		tempModule.MasterCtrlByte.SLA_Adr.WRRegAdr = TEMP_REG_POINT;
//		tempModule.OPMode = I2C_MASTREAD;
//		HandleID[adr] = I2CBusNew::I2C_Task_Request(I2C_MASTREAD,&tempModule);
//		bAssigned[adr] = 1;  //已交付
//	}
//	 if(bAssigned[adr] && I2CBusNew::GetRequestStatus(HandleID[adr]) == I2C_ReqFin) //已完成
//	 {
//		 bAssigned[adr] = 0;
//		 I2CBusNew::SetRequestStatus(HandleID[adr],I2C_ReqHandled);  //设置成已处理完，供I2C任务转入下一个请求
//		 unsigned short Read16 = ((uint16)I2CBusNew::RunningBuffer[tempModule.I2Cx].Buf[0]<<8)| I2CBusNew::RunningBuffer[tempModule.I2Cx].Buf[1];
//		 Read16 = Read16>>5;
//         if(Read16&0x400)
//		 {
//			 unsigned short n2dCode = (~Read16)&0x7FF;
//			 fTemp = -(n2dCode+1)*TempRatio;
//		 }
//         else
//		   fTemp = (float)Read16*TempRatio;
//		 return true;
//	 }
//	 else if(bAssigned[adr] && I2CBusNew::GetRequestStatus(HandleID[adr]) == I2C_ReqErr)
//	 {
//		 bAssigned[adr] = 0;
//		 I2CBusNew::SetRequestStatus(HandleID[adr],I2C_ReqHandled);
//	 }
//	 return false;
//}

//DO NOT DELETE!
//设置Tos温度
//bool LM75B::SetTosTemp(unsigned char adr,float TempTos)
//{
//	st_I2CModule  tempModule;
//	tempModule.I2Cx = LM75_I2C_PORT;
//	tempModule.CtrlByteNum = 1;//POINTER BYTE
//	tempModule.BufLen = 2;
//	memset(tempModule.Buf,tempModule.BufLen,0);
//	tempModule.MasterCtrlByte.SLA_Adr.SlaveAdr  =LM75_CMD|(adr<<1);
//	tempModule.MasterCtrlByte.SLA_Adr.WRRegAdr = TOS_REG_POINT;
//	tempModule.OPMode = I2C_MASTWRITE;
//	unsigned short TosCode =0;
//	if(TempTos>0.0f)
//		TosCode = (unsigned short)(TempTos/TempGateRatio);
//    else
//	{
//	   TosCode = (unsigned short)((-TempTos)/TempGateRatio);
//		unsigned short N2dCOde = (~TosCode)&0x1FF;
//		TosCode = (N2dCOde+1);
//	}
//		TosCode = TosCode<<7;
//	tempModule.Buf[0] = TosCode>>8;
//	tempModule.Buf[1] =  TosCode&0xFF;
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
//bool LM75B::SetThystTemp(unsigned char adr,float TempThyst)
//{
//	st_I2CModule  tempModule;
//	tempModule.I2Cx = LM75_I2C_PORT;
//	tempModule.CtrlByteNum = 1;//POINTER BYTE
//	tempModule.BufLen = 2;
//	memset(tempModule.Buf,tempModule.BufLen,0);
//	tempModule.MasterCtrlByte.SLA_Adr.SlaveAdr  =LM75_CMD|(adr<<1);
//	tempModule.MasterCtrlByte.SLA_Adr.WRRegAdr = THYST_REG_POINT;
//	tempModule.OPMode = I2C_MASTWRITE;
//	unsigned short TosCode =0;
//	if(TempThyst>0.0f)
//		TosCode = (unsigned short)(TempThyst/TempGateRatio);
//    else
//	{
//	   TosCode = (unsigned short)((-TempThyst)/TempGateRatio);
//		unsigned short N2dCOde = (~TosCode)&0x1FF;
//		TosCode = (N2dCOde+1);
//	}
//	TosCode = TosCode<<7;
//	tempModule.Buf[0] = TosCode>>8;
//	tempModule.Buf[1] =  TosCode&0xFF;
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
//float LM75B::ReadTosTemp(unsigned char adr)
//{
//	float f_ReadTemp;
//	st_I2CModule  tempModule;
//	tempModule.I2Cx = LM75_I2C_PORT;
//	tempModule.CtrlByteNum = 1;//POINTER BYTE
//	tempModule.BufLen = 2;
//	memset(tempModule.Buf,tempModule.BufLen,0);
//	tempModule.MasterCtrlByte.SLA_Adr.SlaveAdr  =LM75_CMD|(adr<<1);
//	tempModule.MasterCtrlByte.SLA_Adr.WRRegAdr = TOS_REG_POINT;
//	tempModule.OPMode = I2C_MASTREAD;
//	//lock i2c
//	if(I2CBusNew::I2C_ImmidiateTrans(&tempModule))
//	{
//		unsigned short Read16 = (uint16)I2CBusNew::RunningBuffer[tempModule.I2Cx].Buf[0]<<8| I2CBusNew::RunningBuffer[tempModule.I2Cx].Buf[1];
//		Read16 = Read16>>7;
//		 if(Read16&0x100)
//		 {
//			 unsigned short n2dCode = (~Read16)&0x1FF;
//			 f_ReadTemp = -(n2dCode+1)*TempGateRatio;
//		 }
//        else
//		    f_ReadTemp = (float)Read16*TempGateRatio;
//		//解锁
//		//unlock
//		I2CBusNew::m_I2CBusMgr[tempModule.I2Cx].I2CBusy = I2C_NOT_BUSY;
//		return f_ReadTemp;
//
//	}
//	else
//		return 0.0f;//失败
//
//}

// DO NOT DELETE!
//读迟滞温度REG
//float LM75B::ReadThystTemp(unsigned char adr)
//{
//	float f_ReadTemp;
//	st_I2CModule  tempModule;
//	tempModule.I2Cx = LM75_I2C_PORT;
//	tempModule.CtrlByteNum = 1;//POINTER BYTE
//	tempModule.BufLen = 2;
//	memset(tempModule.Buf,tempModule.BufLen,0);
//	tempModule.MasterCtrlByte.SLA_Adr.SlaveAdr  =LM75_CMD|(adr<<1);
//	tempModule.MasterCtrlByte.SLA_Adr.WRRegAdr = THYST_REG_POINT;
//	tempModule.OPMode = I2C_MASTREAD;
//	//lock i2c
//	if(I2CBusNew::I2C_ImmidiateTrans(&tempModule))
//	{
//		unsigned short Read16 = (uint16)I2CBusNew::RunningBuffer[tempModule.I2Cx].Buf[0]<<8| I2CBusNew::RunningBuffer[tempModule.I2Cx].Buf[1];
//		Read16 = Read16>>7;
//        if(Read16&0x100)
//		 {
//			 unsigned short n2dCode = (~Read16)&0x1FF;
//			 f_ReadTemp = -(n2dCode+1)*TempGateRatio;
//		 }
//        else
//		    f_ReadTemp = (float)Read16*TempGateRatio;
//		//解锁
//		//unlock
//		I2CBusNew::m_I2CBusMgr[tempModule.I2Cx].I2CBusy = I2C_NOT_BUSY;
//		return f_ReadTemp;
//
//	}
//	else//失败
//	 return 0.0f;
//}

bool LM75B::WriteConfig(unsigned char adr,un_ConfigReg ConfigReg)
{
    //default acitve Low ,compare mode  //
    st_I2CModule  tempModule;
    tempModule.I2Cx = LM75_I2C_PORT;
    tempModule.CtrlByteNum = 1;//POINTER BYTE
    tempModule.BufLen = 1;
    memset(tempModule.Buf,tempModule.BufLen,0);
    tempModule.MasterCtrlByte.SLA_Adr.SlaveAdr  =LM75_CMD|(adr<<1);
    tempModule.MasterCtrlByte.SLA_Adr.WRRegAdr = CONFIG_REG_POINT;
    tempModule.OPMode = I2C_MASTWRITE;
    tempModule.Buf[0] = ConfigReg.uc_Config;
    //lock i2c
    if(I2CBusNew::I2C_ImmidiateTrans(&tempModule))
    {
        //unlock
        I2CBusNew::m_I2CBusMgr[tempModule.I2Cx].I2CBusy = I2C_NOT_BUSY;
        return true;

    }
    else
        return false;//失败
}

unsigned char LM75B::ReadConfig(unsigned char adr)
{
    st_I2CModule  tempModule;
    tempModule.I2Cx = LM75_I2C_PORT;
    tempModule.CtrlByteNum = 1;//POINTER BYTE
    tempModule.BufLen = 1;
    memset(tempModule.Buf,tempModule.BufLen,0);
    tempModule.MasterCtrlByte.SLA_Adr.SlaveAdr  =LM75_CMD|(adr<<1);
    tempModule.MasterCtrlByte.SLA_Adr.WRRegAdr = CONFIG_REG_POINT;
    tempModule.OPMode = I2C_MASTREAD;
    //lock i2c
    if(I2CBusNew::I2C_ImmidiateTrans(&tempModule))
    {
        unsigned char Read8 = (uint16)I2CBusNew::RunningBuffer[tempModule.I2Cx].Buf[0];
        I2CBusNew::m_I2CBusMgr[tempModule.I2Cx].I2CBusy = I2C_NOT_BUSY;
        return Read8;
    }
    else
        return 0;//失败
}

// DO NOT DELETE!
//void LM75B::setPowerMode(unsigned char adr,PowerMode mode)
//{
//    //Read the current 8-bit register value
//     un_ConfigReg ConfigReg;
//     ConfigReg.uc_Config = ReadConfig(adr);
//
//    //Set or clear the SHUTDOWN bit
//    if (mode == POWER_SHUTDOWN)
//        ConfigReg.uc_Config |= (1 << 0);
//    else
//        ConfigReg.uc_Config &= ~(1 << 0);
//
//    //Write the value back out
//    WriteConfig(adr,ConfigReg);
//}

// DO NOT DELETE!
//OSMode LM75B::getOsMode(unsigned char adr)
//{
//    //Read the 8-bit register value
//     char value = ReadConfig(adr);
//
//    //Return the status of the OS_COMP_INT bit
//    if (value & (1 << 1))
//        return OS_INTERRUPT;
//    else
//        return OS_COMPARATOR;
//}

// DO NOT DELETE!
//void LM75B::setOsMode(unsigned char adr,OSMode mode)
//{
//    //Read the current 8-bit register value
//	  un_ConfigReg ConfigReg;
//     ConfigReg.uc_Config = ReadConfig(adr);
//
//    //Set or clear the OS_COMP_INT bit
//    if (mode == OS_INTERRUPT)
//        ConfigReg.uc_Config |= (1 << 1);
//    else
//        ConfigReg.uc_Config &= ~(1 << 1);
//
//    //Write the value back out
//     WriteConfig(adr,ConfigReg);
//}

// DO NOT DELETE!
//OSPolarity LM75B::getOsPolarity(unsigned char adr)
//{
//    //Read the 8-bit register value
//     un_ConfigReg ConfigReg;
//     ConfigReg.uc_Config = ReadConfig(adr);
//
//    //Return the status of the OS_POL bit
//    if (ConfigReg.uc_Config & (1 << 2))
//        return OS_ACTIVE_HIGH;
//    else
//        return OS_ACTIVE_LOW;
//}

// DO NOT DELETE!
//void LM75B::setOsPolarity(unsigned char adr,OSPolarity polarity)
//{
//    //Read the current 8-bit register value
//    un_ConfigReg ConfigReg;
//     ConfigReg.uc_Config = ReadConfig(adr);
//
//    //Set or clear the OS_POL bit
//    if (polarity == OS_ACTIVE_HIGH)
//        ConfigReg.uc_Config |= (1 << 2);
//    else
//        ConfigReg.uc_Config &= ~(1 << 2);
//
//    //Write the value back out
//   WriteConfig(adr,ConfigReg);
//}

