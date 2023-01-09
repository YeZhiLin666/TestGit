
//1.	����ͨ������EEPROM����Ϣ��listenforNetд��ȥ�������縨�����ͣ�ͨ�����ȡ�
//2.	��������ͨ�����ϵ���Ϣ��AUXMCU�ϵ�ʱ���������á�����һ�����ݽṹ�����λ����Ҫ�������õ�����AUXMCU��FRAM���ʱ�򣬽����������ϢҲ�浽FRAM��ȥ��
//3.	AUXMCU ���ϵ�FRAM�ﱣ���и�AUXMCU�������ศ��ͨ�����ܺ͡����ܺ�Ӧ�ܴӸ���ͨ�����϶���������Ϣ���ϡ����粻���ϣ�Ӧ������
//4.	��PC����AUXMCU��������Ϣ��ʱ��Ҳ�����˸���IVͨ�������䵽�ĸ���ͨ������Ϣ����������FRAM� �ڱ��浽FRAM���ʱ��Ҳ���������ϵ���Ϣ�浽FRAM�
//5.	�Ժ�AUXMCU�ϵ�ʱֱ�Ӵ�FRAM��ȡ���е�������Ϣ��
//6.	�������У����Ϣֱ����У����ʱ���¸�ͨ���塣�������ã�������ʱ��������ŵ��ƣң����ÿ���ϵ��ʱ����Ȼ�Ǵ�FRAM���ȡУ���������������á�


#include "includes.h"

#if HARDWARE_SDRAM_32M == 1
#pragma arm section rwdata = "SRAM32",zidata = "SRAM32"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x01F40000
#else
#pragma arm section rwdata = "SRAM64",zidata = "SRAM64"		   // ���ñ����洢���ⲿ��չ�洢����	   dynamic memory  0XA0000000  0x03E80000
#endif
unsigned char E2PROM_24LC128::WriteBuf[MAXI2CBUFLEN];
unsigned char E2PROM_24LC128::ReadBuf[MAXI2CBUFLEN];

unsigned char E2PROM_24LC128::uc_EEPROM_OnWriteBuf;			 //д��������������
unsigned char E2PROM_24LC128::uc_EEPROM_OnReadBuf;			//����������������
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

//дEEPROMǰ�����WriteBuf
void E2PROM_24LC128::WriteI2cBuf(UChar8 *Data,UWord16 Len)
{
    memcpy(WriteBuf,Data,Len);
}

//����д
// false(0)ʧ��
//true(1)�ɹ�
bool E2PROM_24LC128::Write24LC128(unsigned char DeviceAdr,unsigned short MemoryAdr,unsigned short WrLen)
{
    if(WrLen>MAXI2CBUFLEN) //��Ҫ���ϵ�ַƫ���ж�
        return false;
    st_I2CModule  I2Cmodule_a;
    I2Cmodule_a.I2Cx = E2PROM_I2C_PORT;
    I2Cmodule_a.CtrlByteNum = 2;// memory adr length
    I2Cmodule_a.BufLen = WrLen;
    memcpy(I2Cmodule_a.Buf ,WriteBuf,WrLen);
    I2Cmodule_a.MasterCtrlByte.SLA_Adr.SlaveAdr  =EEPROM_CMD|(DeviceAdr<<1);  //0xa0 /x
    I2Cmodule_a.MasterCtrlByte.SLA_Adr.WRRegAdr = MemoryAdr;
    I2Cmodule_a.PageSize = EEPROM_PAGESIZE;//ҳ��С
    I2Cmodule_a.OPMode = I2C_MASTWRITE;  //��дģʽ
    if(I2CBusNew::I2C_ImmidiateTrans(&I2Cmodule_a))
    {
        return true;    //�ɹ�
    }
    else
    {
        return false;   //ʧ��
    }
}
//������
// false(0)ʧ��
//true(1)�ɹ�
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
        //bufferת��
        memcpy(ReadBuf,I2CBusNew::RunningBuffer[I2Cmodule_a.I2Cx].Buf,Len);
        return true;
    }
    else
    {
        return false;
    }
}

//DO NOT DELETE!
//��д���󣬷�ʵʱ ���ѯhandleID״̬
//I2C_Handle_Status I2CBusNew::GetRequestStatus
//example
//  if(GetRequestStatus(MyHandle[i]) == I2C_ReqFin) //�����
//  SetRequestStatus(MyHandle[i],I2C_ReqHandled);  //���ó��Ѵ����꣬��I2C����ת����һ������
//
#if  USE_AS_AUXMCU == 1
unsigned short E2PROM_24LC128::WriteRequest(unsigned char DeviceAdr,unsigned short MemoryAdr,unsigned short Len)
{
    unsigned short HandleID = 0;
    if(Len>MAXI2CBUFLEN) //��Ҫ���ϵ�ַƫ���ж�
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
//read  �����I2CBusNew::RunningBuffer[I2Cx].Buf���ȡ
//  ������ɶ����󣬳���1sδ��ȡ�����Զ��������ģ�ת����һ����
//  if(GetRequestStatus(MyHandle[i]) == I2C_ReqFin)
//     SetRequestStatus(MyHandle[i],I2C_ReqHandled);  //�Ѷ�,I2C����ת����һ������
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
