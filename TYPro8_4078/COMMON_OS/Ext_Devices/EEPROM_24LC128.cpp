
//1.	辅助通道板上EEPROM的信息由listenforNet写下去。　比如辅助类型，通道数等。
//2.	各个辅助通道板上的信息在AUXMCU上电时读回来备用。放在一个数据结构里。当上位机需要下载配置到辅助AUXMCU的FRAM里的时候，将各个板的信息也存到FRAM里去。
//3.	AUXMCU 板上的FRAM里保存有该AUXMCU下属各类辅助通道的总和。该总和应能从各个通道板上读回来的信息符合。（如不符合，应报出错
//4.	在PC机向AUXMCU下配置信息的时候，也包括了各个IV通道所分配到的辅助通道的信息。都将存在FRAM里。 在保存到FRAM里的时候，也将各个板上的信息存到FRAM里。
//5.	以后AUXMCU上电时直接从FRAM获取所有的配置信息。
//6.	辅助板的校正信息直接在校正的时候下个通道板。但在配置ｃｏｎｆｉｇ的时候读回来放到ＦＲＡＭ里。每次上电的时候，仍然是从FRAM里读取校正参数，供采样用。


#include "includes.h"

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // 设置变量存储在外部扩展存储器中	   dynamic memory  0XA0000000  0x03E80000
#endif
unsigned char E2PROM_24LC128::WriteBuf[MAXI2CBUFLEN];
unsigned char E2PROM_24LC128::ReadBuf[MAXI2CBUFLEN];

unsigned char E2PROM_24LC128::uc_EEPROM_OnWriteBuf;			 //写缓冲区中有数据
unsigned char E2PROM_24LC128::uc_EEPROM_OnReadBuf;			//读缓冲区中有数据
unsigned char E2PROM_24LC128::BoardCfgBit;

#pragma arm section



void E2PROM_24LC128::MyInit(void)
{
    memset(&WriteBuf[0],0,MAXI2CBUFLEN);
    memset(&ReadBuf[0],0,MAXI2CBUFLEN);
    uc_EEPROM_OnWriteBuf = 0;
    uc_EEPROM_OnReadBuf = 0;
    BoardCfgBit = 0;
}

//写EEPROM前需存入WriteBuf
void E2PROM_24LC128::WriteI2cBuf(UChar8 *Data,UWord16 Len)
{
    memcpy(WriteBuf,Data,Len);
}

//立即写
// false(0)失败
//true(1)成功
bool E2PROM_24LC128::Write24LC128(unsigned char DeviceAdr,unsigned short MemoryAdr,unsigned short WrLen)
{
    if(WrLen>MAXI2CBUFLEN) //还要加上地址偏移判断
        return false;
    st_I2CModule  I2Cmodule_a;
    I2Cmodule_a.I2Cx = E2PROM_I2C_PORT;
    I2Cmodule_a.CtrlByteNum = 2;// memory adr length
    I2Cmodule_a.BufLen = WrLen;
    memcpy(I2Cmodule_a.Buf ,WriteBuf,WrLen);
    I2Cmodule_a.MasterCtrlByte.SLA_Adr.SlaveAdr  =EEPROM_CMD|(DeviceAdr<<1);  //0xa0 /x
    I2Cmodule_a.MasterCtrlByte.SLA_Adr.WRRegAdr = MemoryAdr;
    I2Cmodule_a.PageSize = EEPROM_PAGESIZE;//页大小
    I2Cmodule_a.OPMode = I2C_MASTWRITE;  //主写模式
    if(I2CBusNew::I2C_ImmidiateTrans(&I2Cmodule_a))
    {
        return true;    //成功
    }
    else
    {
        return false;   //失败
    }
}
//立即读
// false(0)失败
//true(1)成功
bool E2PROM_24LC128::Read24LC128(unsigned char DeviceAdr,unsigned short MemoryAdr,unsigned short Len)
{
    if(Len>MAXI2CBUFLEN)
        return 0;
    st_I2CModule  I2Cmodule_a;
    I2Cmodule_a.I2Cx = E2PROM_I2C_PORT;
    I2Cmodule_a.CtrlByteNum = 2;
    I2Cmodule_a.BufLen = Len;
    memset(&ReadBuf[0],0,MAXI2CBUFLEN);
    I2Cmodule_a.MasterCtrlByte.SLA_Adr.SlaveAdr  =EEPROM_CMD|(DeviceAdr<<1);
    I2Cmodule_a.MasterCtrlByte.SLA_Adr.WRRegAdr = MemoryAdr;
    I2Cmodule_a.OPMode = I2C_MASTREAD;
    I2Cmodule_a.PageSize = EEPROM_PAGESIZE;
    if(I2CBusNew::I2C_ImmidiateTrans(&I2Cmodule_a))
    {
        //buffer转移
        memcpy(ReadBuf,I2CBusNew::RunningBuffer[I2Cmodule_a.I2Cx].Buf,Len);
        return true;
    }
    else
    {
        return false;
    }
}

//DO NOT DELETE!
//读写请求，非实时 需查询handleID状态
//I2C_Handle_Status I2CBusNew::GetRequestStatus
//example
//  if(GetRequestStatus(MyHandle[i]) == I2C_ReqFin) //已完成
//  SetRequestStatus(MyHandle[i],I2C_ReqHandled);  //设置成已处理完，供I2C任务转入下一个请求
//
#if  USE_AS_AUXMCU == 1
unsigned short E2PROM_24LC128::WriteRequest(unsigned char DeviceAdr,unsigned short MemoryAdr,unsigned short Len)
{
    unsigned short HandleID = 0;
    if(Len>MAXI2CBUFLEN) //还要加上地址偏移判断
        return false;
    st_I2CModule  I2Cmodule_a;
    I2Cmodule_a.I2Cx = E2PROM_I2C_PORT;
    I2Cmodule_a.CtrlByteNum = 2;
    I2Cmodule_a.BufLen = Len;
    memcpy(I2Cmodule_a.Buf ,WriteBuf,Len);
    I2Cmodule_a.MasterCtrlByte.SLA_Adr.SlaveAdr  =EEPROM_CMD|(DeviceAdr<<1);  //0xa0 /x
    I2Cmodule_a.MasterCtrlByte.SLA_Adr.WRRegAdr = MemoryAdr;
    I2Cmodule_a.PageSize = EEPROM_PAGESIZE;
    I2Cmodule_a.OPMode = I2C_MASTWRITE;
    HandleID = I2CBusNew::I2C_Task_Request(I2C_MASTWRITE,&I2Cmodule_a);
    return HandleID;

}

// DO NOT DELETE!
//read  结果在I2CBusNew::RunningBuffer[I2Cx].Buf里获取
//  如已完成读请求，超过1s未获取，则自动丢弃报文，转入下一请求
//  if(GetRequestStatus(MyHandle[i]) == I2C_ReqFin)
//     SetRequestStatus(MyHandle[i],I2C_ReqHandled);  //已读,I2C任务转入下一个请求
unsigned short E2PROM_24LC128::ReadRequest(unsigned char DeviceAdr,unsigned short MemoryAdr,unsigned short Len)
{
    unsigned short HandleID = 0;
    if(Len>MAXI2CBUFLEN)
        return 0;
    st_I2CModule  I2Cmodule_a;
    I2Cmodule_a.I2Cx = E2PROM_I2C_PORT;
    I2Cmodule_a.CtrlByteNum = 2;
    I2Cmodule_a.BufLen = Len;
    I2Cmodule_a.MasterCtrlByte.SLA_Adr.SlaveAdr  =EEPROM_CMD|(DeviceAdr<<1);
    I2Cmodule_a.MasterCtrlByte.SLA_Adr.WRRegAdr = MemoryAdr;
    I2Cmodule_a.OPMode = I2C_MASTREAD;
    I2Cmodule_a.PageSize = EEPROM_PAGESIZE;
    HandleID = I2CBusNew::I2C_Task_Request(I2C_MASTREAD,&I2Cmodule_a);
    return HandleID;
}
#endif
